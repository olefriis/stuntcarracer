/**
 * Game flow controller for Stunt Car Racer.
 *
 * This file owns all UI (HTML overlay) and drives the game state machine.
 * It polls the C++ engine each frame via Module._jsXxx() exported functions
 * and calls into C++ to trigger state changes (select track, start race, etc.).
 *
 * State flow:
 *   TRACK_MENU  →  TRACK_PREVIEW  →  GAME_IN_PROGRESS  →  GAME_OVER  →  TRACK_MENU
 *
 * All transitions use a short fade-to-black CSS animation for a polished feel.
 */

(function () {
  'use strict';

  // ── Game modes (must match C++ GameModeType enum) ───────────
  var TRACK_MENU      = 0;
  var TRACK_PREVIEW   = 1;
  var GAME_IN_PROGRESS = 2;
  var GAME_OVER       = 3;

  // ── Key bitmask constants (must match C++ KEY_P1_* defines) ─
  var KEY_LEFT        = 0x01;
  var KEY_RIGHT       = 0x02;
  var KEY_HASH        = 0x04;
  var KEY_BRAKE_BOOST = 0x08;
  var KEY_ACCEL_BOOST = 0x10;
  var KEY_ACCEL_ONLY  = 0x20;

  // ── State ──────────────────────────────────────────────────
  var trackIndex     = 0;
  var lastGameMode   = -1;
  var fading         = false;
  var raceEndTime    = 0;     // timestamp when race ended (for 6-second timer)
  var isMobile       = false;
  var ready          = false;

  // Touch drive state (mobile only)
  var touchDrive = { left: false, right: false, gas: false, brake: false, boost: false };

  // ── C++ API helpers ────────────────────────────────────────
  // These are thin wrappers so the rest of the code reads nicely.
  // They are only valid after the Emscripten runtime has initialised.

  function getGameMode()       { return Module._jsGetGameMode(); }
  function getTrackID()        { return Module._jsGetTrackID(); }
  function getNumTracks()      { return Module._jsGetNumTracks(); }
  function isRaceFinished()    { return !!Module._jsIsRaceFinished(); }
  function isRaceWon()         { return !!Module._jsIsRaceWon(); }
  function isPlayerWrecked()   { return !!Module._jsIsPlayerWrecked(); }
  function getBoostReserve()   { return Module._jsGetBoostReserve(); }
  function getBoostMax()       { return Module._jsGetBoostMax(); }
  function getDamage()         { return Module._jsGetDamage(); }
  function getLapNumber()      { return Module._jsGetLapNumber(); }
  function getOpponentId()     { return Module._jsGetOpponentId(); }

  function getTrackName() {
    var ptr = Module._jsGetTrackName();
    return ptr ? Module.UTF8ToString(ptr) : '';
  }
  function getOpponentName() {
    var ptr = Module._jsGetOpponentName();
    return ptr ? Module.UTF8ToString(ptr) : '';
  }

  function selectTrack(index)  { Module._jsSelectTrack(index); }
  function startPreview()      { Module._jsStartPreview(); }
  function startGame(opp)      { Module._jsStartGame(opp); }
  function goToMenu()          { Module._jsGoToMenu(); }
  function setGameOver()       { Module._jsSetGameOver(); }
  function setDriveInput(f)    { Module._touchSetDriveInput(f); }

  // ── Fade transition helper ─────────────────────────────────

  function fadeAndDo(callback) {
    if (fading) return;
    fading = true;
    var el = document.getElementById('fadeOverlay');
    el.style.opacity = '1';
    setTimeout(function () {
      callback();
      setTimeout(function () {
        el.style.opacity = '0';
        fading = false;
      }, 60);
    }, 350);
  }

  // ── Track navigation ───────────────────────────────────────

  function prevTrack() {
    trackIndex--;
    if (trackIndex < 0) trackIndex = getNumTracks() - 1;
    selectTrack(trackIndex);
  }

  function nextTrack() {
    trackIndex++;
    if (trackIndex >= getNumTracks()) trackIndex = 0;
    selectTrack(trackIndex);
  }

  // ── UI creation ────────────────────────────────────────────

  function createUI() {
    isMobile = ('ontouchstart' in window) || (navigator.maxTouchPoints > 0);

    // Fade overlay
    var fade = document.createElement('div');
    fade.id = 'fadeOverlay';
    fade.style.cssText =
      'position:fixed;left:0;top:0;width:100%;height:100%;background:#000;' +
      'opacity:0;pointer-events:none;z-index:200;transition:opacity 0.35s ease;';
    document.body.appendChild(fade);

    // Container for all game UI
    var container = document.createElement('div');
    container.id = 'gameUI';
    container.style.cssText =
      'position:fixed;left:0;top:0;width:100%;height:100%;' +
      'pointer-events:none;z-index:100;user-select:none;-webkit-user-select:none;';
    document.body.appendChild(container);

    // Helper: create a styled element inside the container
    function el(id, text, css) {
      var d = document.createElement('div');
      d.id = id;
      if (text) d.textContent = text;
      d.style.cssText =
        'position:absolute;pointer-events:auto;display:none;' +
        'background:rgba(255,255,255,0.18);color:#fff;' +
        'border:2px solid rgba(255,255,255,0.4);border-radius:12px;' +
        'font-family:Arial,sans-serif;font-weight:bold;' +
        'display:flex;align-items:center;justify-content:center;' +
        'touch-action:none;user-select:none;-webkit-user-select:none;' +
        'box-sizing:border-box;text-align:center;overflow:hidden;white-space:nowrap;' +
        'cursor:pointer;' + (css || '');
      d.style.display = 'none';
      container.appendChild(d);
      return d;
    }

    // ── Track Menu ──
    el('tc-prev', '\u25C0\uFE0E',
      'left:2vw;bottom:6vh;width:14vw;height:14vw;font-size:min(6vw,32px);max-width:75px;max-height:75px;');
    el('tc-next', '\u25B6\uFE0E',
      'left:18vw;bottom:6vh;width:14vw;height:14vw;font-size:min(6vw,32px);max-width:75px;max-height:75px;');
    el('tc-select', 'SELECT',
      'right:2vw;bottom:6vh;width:22vw;height:14vw;font-size:min(3.5vw,18px);max-width:130px;max-height:75px;');
    el('tc-trackname', '',
      'left:34vw;right:26vw;width:auto;bottom:6vh;height:14vw;max-height:75px;font-size:min(3.8vw,20px);' +
      'pointer-events:none;background:none;border:none;text-shadow:0 0 8px rgba(0,0,0,0.8);');

    // ── Track Preview ──
    el('tc-back', 'MENU',
      'left:2vw;bottom:6vh;width:22vw;height:12vw;font-size:min(4.5vw,22px);max-width:120px;max-height:70px;');
    el('tc-start', 'START',
      'right:2vw;bottom:6vh;width:22vw;height:12vw;font-size:min(4.5vw,22px);max-width:120px;max-height:70px;');
    el('tc-opponent', '',
      'left:26vw;right:26vw;width:auto;bottom:6vh;height:12vw;max-height:70px;font-size:min(3.5vw,18px);' +
      'pointer-events:none;background:none;border:none;text-shadow:0 0 8px rgba(0,0,0,0.8);');

    // ── In-Game driving controls (mobile only) ──
    el('tc-left', '\u25C0\uFE0E',
      'left:2vw;bottom:6vh;width:11vw;height:11vw;font-size:min(5vw,28px);max-width:65px;max-height:65px;');
    el('tc-right', '\u25B6\uFE0E',
      'left:15vw;bottom:6vh;width:11vw;height:11vw;font-size:min(5vw,28px);max-width:65px;max-height:65px;');
    el('tc-accel', '\u25B2\uFE0E',
      'right:2vw;bottom:30vh;width:11vw;height:11vw;font-size:min(5vw,28px);max-width:65px;max-height:65px;');
    el('tc-brake', '\u25BC\uFE0E',
      'right:2vw;bottom:6vh;width:11vw;height:11vw;font-size:min(5vw,28px);max-width:65px;max-height:65px;');
    el('tc-boost', '\u00A0\uD83D\uDD25\u00A0',
      'left:50%;bottom:6vh;width:22vw;height:11vw;font-size:min(5vw,28px);max-width:120px;max-height:65px;transform:translateX(-50%);');

    // ── In-Game common (shown for everyone) ──
    el('tc-menu', '\u2715',
      'right:2vw;top:2vh;width:10vw;height:10vw;font-size:min(5vw,28px);max-width:55px;max-height:55px;');
    el('tc-lap', '',
      'right:2vw;top:calc(2vh + 10vw + 1vh);width:auto;height:auto;font-size:min(3vw,16px);' +
      'padding:0.4em 0.8em;max-width:120px;pointer-events:none;' +
      'background:rgba(0,0,0,0.4);border:1px solid rgba(255,255,255,0.3);border-radius:8px;');

    // ── Game Over ──
    el('tc-gameover-label', '',
      'left:50%;top:40%;width:50vw;height:auto;font-size:min(7vw,40px);max-width:300px;' +
      'pointer-events:none;background:none;border:none;text-shadow:0 0 12px rgba(0,0,0,0.9);' +
      'transform:translate(-50%,-50%);');
    el('tc-gameover', 'MENU',
      'left:50%;bottom:6vh;width:18vw;height:11vw;font-size:min(3.5vw,18px);max-width:100px;max-height:65px;transform:translateX(-50%);');

    // ── HUD bars ──
    createHudBar('tc-hud-boost', '\uD83D\uDD25', '#ff9900');
    document.getElementById('tc-hud-boost').style.cssText +=
      'left:2vw;right:50%;top:2vh;padding-right:1vw;';
    createHudBar('tc-hud-damage', '\u26A0\uFE0F', '#ff3333');
    document.getElementById('tc-hud-damage').style.cssText +=
      'left:50%;right:14vw;top:2vh;padding-left:1vw;';

    // Wire up buttons
    wireButtons();
    wireKeyboard();
  }

  function createHudBar(id, icon, color) {
    var container = document.getElementById('gameUI');
    var row = document.createElement('div');
    row.id = id;
    row.style.cssText =
      'position:absolute;display:none;align-items:center;pointer-events:none;height:2.5vh;min-height:14px;';
    var iconEl = document.createElement('span');
    iconEl.textContent = icon;
    iconEl.style.cssText = 'font-size:min(6vh,30px);margin-right:1vw;line-height:1;';
    var track = document.createElement('div');
    track.style.cssText =
      'flex:1;height:100%;background:rgba(0,0,0,0.4);border-radius:4px;overflow:hidden;';
    var fill = document.createElement('div');
    fill.id = id + '-fill';
    fill.style.cssText =
      'height:100%;width:0%;background:' + color + ';border-radius:4px;transition:width 0.15s;';
    track.appendChild(fill);
    row.appendChild(iconEl);
    row.appendChild(track);
    container.appendChild(row);
  }

  // ── Button wiring ──────────────────────────────────────────

  function addBtn(id, callback) {
    var btn = document.getElementById(id);
    function handler(e) {
      e.preventDefault();
      btn.style.background = 'rgba(255,255,255,0.45)';
      callback();
      setTimeout(function () { btn.style.background = 'rgba(255,255,255,0.18)'; }, 200);
    }
    btn.addEventListener('touchstart', handler, { passive: false });
    btn.addEventListener('mousedown', handler);
  }

  function addDriveBtn(id, field) {
    var btn = document.getElementById(id);
    btn.addEventListener('touchstart', function (e) {
      e.preventDefault();
      btn.style.background = 'rgba(255,255,255,0.45)';
      touchDrive[field] = true;
      updateDriveFlags();
    }, { passive: false });
    btn.addEventListener('touchend', function (e) {
      e.preventDefault();
      btn.style.background = 'rgba(255,255,255,0.18)';
      touchDrive[field] = false;
      updateDriveFlags();
    }, { passive: false });
    btn.addEventListener('touchcancel', function (e) {
      e.preventDefault();
      btn.style.background = 'rgba(255,255,255,0.18)';
      touchDrive[field] = false;
      updateDriveFlags();
    }, { passive: false });
  }

  function updateDriveFlags() {
    var d = touchDrive;
    var flags = 0;
    if (d.left)  flags |= KEY_LEFT;
    if (d.right) flags |= KEY_RIGHT;
    if (d.gas && d.boost)   flags |= KEY_ACCEL_BOOST;
    else if (d.gas)         flags |= KEY_ACCEL_ONLY;
    if (d.brake && d.boost) flags |= KEY_BRAKE_BOOST;
    else if (d.brake)       flags |= KEY_HASH;
    setDriveInput(flags);
  }

  function wireButtons() {
    // Track Menu
    addBtn('tc-prev', prevTrack);
    addBtn('tc-next', nextTrack);
    addBtn('tc-select', function () {
      if (getTrackID() < 0) return;
      fadeAndDo(function () { startPreview(); });
    });

    // Track Preview
    addBtn('tc-back', function () {
      fadeAndDo(function () { goToMenu(); });
    });
    addBtn('tc-start', function () {
      fadeAndDo(function () { startGame(-1); });
    });

    // In-Game drive controls (touch only)
    addDriveBtn('tc-left', 'left');
    addDriveBtn('tc-right', 'right');
    addDriveBtn('tc-accel', 'gas');
    addDriveBtn('tc-brake', 'brake');
    addDriveBtn('tc-boost', 'boost');

    // In-Game menu / close
    addBtn('tc-menu', function () {
      fadeAndDo(function () { goToMenu(); });
    });

    // Game Over
    addBtn('tc-gameover', function () {
      fadeAndDo(function () { goToMenu(); });
    });
  }

  // ── Keyboard shortcuts ─────────────────────────────────────

  function wireKeyboard() {
    document.addEventListener('keydown', function (e) {
      var mode = getGameMode();

      if (mode === TRACK_MENU) {
        if (e.key === 'ArrowLeft')  { e.preventDefault(); prevTrack(); }
        else if (e.key === 'ArrowRight') { e.preventDefault(); nextTrack(); }
        else if (e.key === 'Enter' || e.key === ' ') {
          e.preventDefault();
          if (getTrackID() >= 0) fadeAndDo(function () { startPreview(); });
        }
      }

      else if (mode === TRACK_PREVIEW) {
        if (e.key === 'Enter' || e.key === ' ') {
          e.preventDefault();
          fadeAndDo(function () { startGame(-1); });
        } else if (e.key === 'Backspace' || e.key === 'Escape') {
          e.preventDefault();
          fadeAndDo(function () { goToMenu(); });
        }
      }

      else if (mode === GAME_IN_PROGRESS) {
        if (e.key === 'Backspace' || e.key === 'Escape') {
          e.preventDefault();
          fadeAndDo(function () { goToMenu(); });
        }
      }

      else if (mode === GAME_OVER) {
        if (e.key === 'Enter' || e.key === ' ' || e.key === 'Backspace' || e.key === 'Escape') {
          e.preventDefault();
          fadeAndDo(function () { goToMenu(); });
        }
      }
    });
  }

  // ── Per-frame update ───────────────────────────────────────
  // Called via requestAnimationFrame. Polls the C++ engine for
  // the current state and updates all on-screen elements.

  function update() {
    if (!ready) { requestAnimationFrame(update); return; }

    var mode = getGameMode();

    // ── Mode change → show/hide element groups ──
    if (mode !== lastGameMode) {
      lastGameMode = mode;
      raceEndTime = 0;

      var groups = {
        menu:     ['tc-prev', 'tc-next', 'tc-select', 'tc-trackname'],
        preview:  ['tc-back', 'tc-start', 'tc-opponent'],
        drive:    ['tc-left', 'tc-right', 'tc-accel', 'tc-brake', 'tc-boost'],
        common:   ['tc-menu', 'tc-lap', 'tc-hud-boost', 'tc-hud-damage'],
        gameover: ['tc-gameover-label', 'tc-gameover']
      };

      // Hide everything
      var all = [].concat(groups.menu, groups.preview, groups.drive, groups.common, groups.gameover);
      for (var i = 0; i < all.length; i++) {
        var e = document.getElementById(all[i]);
        if (e) e.style.display = 'none';
      }

      // Show the right group
      var show = [];
      if (mode === TRACK_MENU)           show = groups.menu;
      else if (mode === TRACK_PREVIEW)   show = groups.preview;
      else if (mode === GAME_IN_PROGRESS) {
        show = groups.common.slice();
        if (isMobile) show = show.concat(groups.drive);
      }
      else if (mode === GAME_OVER)       show = groups.gameover;

      for (var i = 0; i < show.length; i++) {
        var e = document.getElementById(show[i]);
        if (e) e.style.display = 'flex';
      }

      // Game Over label
      if (mode === GAME_OVER) {
        var lbl = document.getElementById('tc-gameover-label');
        if (lbl) {
          lbl.textContent = isPlayerWrecked() ? 'WRECKED' : (isRaceWon() ? 'YOU WON' : 'YOU LOST');
        }
      }
    }

    // ── Track Menu: track name ──
    if (mode === TRACK_MENU) {
      var label = document.getElementById('tc-trackname');
      if (label) label.textContent = getTrackName();
    }

    // ── Track Preview: opponent name ──
    if (mode === TRACK_PREVIEW) {
      var oppLabel = document.getElementById('tc-opponent');
      if (oppLabel) {
        var name = getOpponentName();
        oppLabel.textContent = name ? 'vs ' + name : '';
      }
    }

    // ── In-Game: race-finished flashing label + timer ──
    if (mode === GAME_IN_PROGRESS && isRaceFinished()) {
      // Start the 6-second timer on first detection
      if (raceEndTime === 0) raceEndTime = Date.now();

      // Show flashing result label
      var lbl = document.getElementById('tc-gameover-label');
      if (lbl) {
        lbl.textContent = isPlayerWrecked() ? 'WRECKED' : (isRaceWon() ? 'RACE WON' : 'RACE LOST');
        lbl.style.display = 'flex';
        var flash = (Math.floor(Date.now() / 500) % 2 === 0);
        lbl.style.opacity = flash ? '1' : '0.2';
      }

      // After 6 seconds, transition to GAME_OVER
      if (Date.now() - raceEndTime > 6000) {
        setGameOver();
      }
    }

    // ── In-Game: lap counter ──
    if (mode === GAME_IN_PROGRESS) {
      var lapEl = document.getElementById('tc-lap');
      if (lapEl) {
        var lap = getLapNumber();
        if (lap < 1) {
          lapEl.style.display = 'none';
        } else {
          lapEl.style.display = 'flex';
          lapEl.textContent = 'Lap ' + Math.min(lap, 3) + '/3';
        }
      }
    }

    // ── HUD bars (in-game + game over) ──
    if (mode === GAME_IN_PROGRESS || mode === GAME_OVER) {
      var boostFill = document.getElementById('tc-hud-boost-fill');
      if (boostFill) {
        var max = getBoostMax();
        var pct = max > 0 ? Math.round(100 * getBoostReserve() / max) : 0;
        boostFill.style.width = pct + '%';
      }
      var dmgFill = document.getElementById('tc-hud-damage-fill');
      if (dmgFill) {
        dmgFill.style.width = Math.min(100, Math.round(100 * getDamage() / 255)) + '%';
      }
    }

    requestAnimationFrame(update);
  }

  // ── Bootstrap ──────────────────────────────────────────────
  // Wait for the Emscripten runtime to be fully initialised,
  // then create the UI and start the update loop.

  function boot() {
    createUI();
    ready = true;
    requestAnimationFrame(update);
  }

  // Module.onRuntimeInitialized fires after wasm instantiation.
  // If it already ran (e.g. script loaded late), call boot directly.
  if (typeof Module !== 'undefined' && Module.calledRun) {
    boot();
  } else {
    var prev = (typeof Module !== 'undefined' && Module.onRuntimeInitialized) || null;
    Module.onRuntimeInitialized = function () {
      if (prev) prev();
      boot();
    };
  }
})();
