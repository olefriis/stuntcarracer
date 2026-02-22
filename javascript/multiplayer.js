/**
 * WebRTC peer connection module for Stunt Car Racer two-player mode.
 *
 * Handles:
 *  - Signaling via the Ruby server (offer/answer/ICE exchange)
 *  - WebRTC peer connection and data channel
 *  - Reliable (signaling) and unreliable (game state) channels
 *
 * Usage from game.js:
 *   var mp = SCR_Multiplayer;
 *   mp.host(signalingUrl).then(function(code) { ... });
 *   mp.join(signalingUrl, code).then(function() { ... });
 *   mp.onOpen = function() { ... };
 *   mp.onMessage = function(data) { ... };
 *   mp.onClose = function() { ... };
 *   mp.send(data);      // unreliable, for game state
 *   mp.sendReliable(data); // reliable, for signaling messages
 */
var SCR_Multiplayer = (function () {
  'use strict';

  var signalingUrl = '';
  var roomCode = '';
  var peerId = '';
  var isHost = false;
  var pollTimer = null;

  var pc = null;           // RTCPeerConnection
  var dcGame = null;       // unreliable data channel (game state)
  var dcReliable = null;   // reliable data channel (control messages)

  // Callbacks
  var onOpen = null;
  var onMessage = null;
  var onReliableMessage = null;
  var onClose = null;

  var connected = false;

  var iceServers = [{ urls: 'stun:stun.l.google.com:19302' }];

  // ── Signaling helpers ──

  function sigPost(path, body) {
    return fetch(signalingUrl + path, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: body ? JSON.stringify(body) : '{}'
    }).then(function (r) {
      if (!r.ok) throw new Error('Signaling error: ' + r.status);
      return r.json();
    });
  }

  function sigGet(path) {
    return fetch(signalingUrl + path).then(function (r) {
      if (!r.ok) throw new Error('Signaling error: ' + r.status);
      return r.json();
    });
  }

  function sigSend(msg) {
    return sigPost('/rooms/' + roomCode + '/peer/' + peerId, msg);
  }

  function sigPoll() {
    return sigGet('/rooms/' + roomCode + '/peer/' + peerId);
  }

  // ── Peer connection setup ──

  function createPeerConnection() {
    pc = new RTCPeerConnection({ iceServers: iceServers });

    pc.onicecandidate = function (e) {
      if (e.candidate) {
        sigSend({ type: 'ice', candidate: e.candidate });
      }
    };

    pc.onconnectionstatechange = function () {
      if (pc.connectionState === 'disconnected' || pc.connectionState === 'failed' ||
          pc.connectionState === 'closed') {
        cleanup();
        if (onClose) onClose();
      }
    };
  }

  function setupDataChannels() {
    // Unreliable channel for game state (low latency, can drop)
    dcGame = pc.createDataChannel('game', {
      ordered: false,
      maxRetransmits: 0
    });
    setupDcGame(dcGame);

    // Reliable channel for control messages (track selection, ready, etc.)
    dcReliable = pc.createDataChannel('reliable', {
      ordered: true
    });
    setupDcReliable(dcReliable);
  }

  function setupDcGame(dc) {
    dc.binaryType = 'arraybuffer';
    dc.onopen = function () {
      connected = true;
      if (onOpen) onOpen();
    };
    dc.onclose = function () {
      connected = false;
      if (onClose) onClose();
    };
    dc.onmessage = function (e) {
      if (onMessage) onMessage(e.data);
    };
  }

  function setupDcReliable(dc) {
    dc.binaryType = 'arraybuffer';
    dc.onmessage = function (e) {
      if (onReliableMessage) {
        try {
          onReliableMessage(JSON.parse(e.data));
        } catch (err) {
          console.warn('Bad reliable message:', err);
        }
      }
    };
  }

  function handleIncomingChannels() {
    pc.ondatachannel = function (e) {
      var ch = e.channel;
      if (ch.label === 'game') {
        dcGame = ch;
        setupDcGame(dcGame);
      } else if (ch.label === 'reliable') {
        dcReliable = ch;
        setupDcReliable(dcReliable);
      }
    };
  }

  // ── Signaling poll loop ──

  function startPolling(handler) {
    if (pollTimer) clearInterval(pollTimer);
    pollTimer = setInterval(function () {
      sigPoll().then(function (data) {
        if (data.messages) {
          data.messages.forEach(handler);
        }
      }).catch(function () {});
    }, 300);
  }

  function stopPolling() {
    if (pollTimer) { clearInterval(pollTimer); pollTimer = null; }
  }

  // ── Host flow ──

  function host(url) {
    signalingUrl = url;
    isHost = true;

    return sigPost('/rooms').then(function (data) {
      roomCode = data.code;
      peerId = data.hostId;

      createPeerConnection();
      setupDataChannels();

      // Start polling for joiner's answer and ICE
      return new Promise(function (resolve) {
        startPolling(function (msg) {
          if (msg.type === 'answer') {
            pc.setRemoteDescription(new RTCSessionDescription(msg.sdp))
              .catch(function (e) { console.error('setRemote answer:', e); });
          } else if (msg.type === 'ice' && msg.candidate) {
            pc.addIceCandidate(new RTCIceCandidate(msg.candidate))
              .catch(function (e) { console.error('addIce:', e); });
          } else if (msg.type === 'joined') {
            // Joiner is connected to signaling, create and send offer
            pc.createOffer().then(function (offer) {
              return pc.setLocalDescription(offer);
            }).then(function () {
              sigSend({ type: 'offer', sdp: pc.localDescription });
            });
          }
        });

        // When data channel opens, we're connected
        var origOnOpen = onOpen;
        onOpen = function () {
          stopPolling();
          onOpen = origOnOpen;
          if (onOpen) onOpen();
        };

        resolve(roomCode);
      });
    });
  }

  // ── Join flow ──

  function join(url, code) {
    signalingUrl = url;
    roomCode = code.toUpperCase();
    isHost = false;

    return sigPost('/rooms/' + roomCode + '/join').then(function (data) {
      peerId = data.joinerId;

      createPeerConnection();
      handleIncomingChannels();

      // Notify host we've joined
      sigSend({ type: 'joined' });

      // Start polling for host's offer and ICE
      return new Promise(function (resolve, reject) {
        startPolling(function (msg) {
          if (msg.type === 'offer') {
            pc.setRemoteDescription(new RTCSessionDescription(msg.sdp))
              .then(function () {
                return pc.createAnswer();
              })
              .then(function (answer) {
                return pc.setLocalDescription(answer);
              })
              .then(function () {
                sigSend({ type: 'answer', sdp: pc.localDescription });
              })
              .catch(function (e) { console.error('answer flow:', e); reject(e); });
          } else if (msg.type === 'ice' && msg.candidate) {
            pc.addIceCandidate(new RTCIceCandidate(msg.candidate))
              .catch(function (e) { console.error('addIce:', e); });
          }
        });

        // When data channel opens, resolve
        var origOnOpen = onOpen;
        onOpen = function () {
          stopPolling();
          onOpen = origOnOpen;
          if (onOpen) onOpen();
          resolve();
        };
      });
    });
  }

  // ── Send ──

  function send(arrayBuffer) {
    if (dcGame && dcGame.readyState === 'open') {
      dcGame.send(arrayBuffer);
    }
  }

  function sendReliable(obj) {
    if (dcReliable && dcReliable.readyState === 'open') {
      dcReliable.send(JSON.stringify(obj));
    }
  }

  // ── Cleanup ──

  function cleanup() {
    stopPolling();
    connected = false;
    if (dcGame) { try { dcGame.close(); } catch (e) {} dcGame = null; }
    if (dcReliable) { try { dcReliable.close(); } catch (e) {} dcReliable = null; }
    if (pc) { try { pc.close(); } catch (e) {} pc = null; }
    roomCode = '';
    peerId = '';
  }

  // ── Public API ──

  return {
    host: host,
    join: join,
    send: send,
    sendReliable: sendReliable,
    cleanup: cleanup,

    isConnected: function () { return connected; },
    isHost: function () { return isHost; },
    getCode: function () { return roomCode; },

    set onOpen(fn) { onOpen = fn; },
    set onMessage(fn) { onMessage = fn; },
    set onReliableMessage(fn) { onReliableMessage = fn; },
    set onClose(fn) { onClose = fn; }
  };
})();
