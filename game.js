/**
 * Game flow controller for Stunt Car Racer.
 *
 * This file owns all UI (HTML overlay) and drives the game state machine.
 * It polls the C++ engine each frame via Module._jsXxx() exported functions
 * and calls into C++ to trigger state changes (select track, start race, etc.).
 *
 * Modes:
 *   MAIN_MENU  →  (Practise) TRACK_MENU → TRACK_PREVIEW → race → result → TRACK_MENU
 *              →  (Season)   season flow with schedule, results, standings
 */

(function () {
  'use strict';

  // ── Game modes (must match C++ GameModeType enum) ───────────
  var TRACK_MENU       = 0;
  var TRACK_PREVIEW    = 1;
  var GAME_IN_PROGRESS = 2;
  var GAME_OVER        = 3;

  // ── Key bitmask constants (must match C++ KEY_P1_* defines) ─
  var KEY_LEFT         = 0x01;
  var KEY_RIGHT        = 0x02;
  var KEY_HASH         = 0x04;
  var KEY_BRAKE_BOOST  = 0x08;
  var KEY_ACCEL_BOOST  = 0x10;
  var KEY_ACCEL_ONLY   = 0x20;

  // ══════════════════════════════════════════════════════════════
  //  TOURNAMENT DATA  (from original Amiga source)
  // ══════════════════════════════════════════════════════════════

  var OPPONENT_NAMES = [
    'Hot Rod', 'Whizz Kid', 'Bad Guy', 'The Dodger', 'Big Ed',
    'Max Boost', 'Dare Devil', 'High Flyer', 'Bully Boy',
    'Jumping Jack', 'Road Hog'
  ];

  var TRACK_NAMES = [
    'Little Ramp', 'Stepping Stones', 'Hump Back', 'Big Ramp',
    'Ski Jump', 'Draw Bridge', 'High Jump', 'Roller Coaster'
  ];

  // Base strength for computer vs computer outcomes (index 0 = strongest)
  var BASE_STRENGTH = [120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10];

  // Tracks per division: index 0 = Div 4 (lowest), index 3 = Div 1 (top)
  var DIVISION_TRACKS = [
    [0, 2],  // Div 4: Little Ramp, Hump Back
    [1, 3],  // Div 3: Stepping Stones, Big Ramp
    [6, 7],  // Div 2: High Jump, Roller Coaster
    [4, 5]   // Div 1: Ski Jump, Draw Bridge
  ];

  // Starting division assignments: index = driver ID (0–11), value = division (0=Div4, 3=Div1)
  // Human player is index 11
  var INITIAL_DIVISIONS = [3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0];

  var HUMAN_PLAYER = 11;

  // Race pairing offsets within a 3-player division:
  // 6 races: each pair plays each track
  var PAIR_A     = [0, 0, 0, 0, 1, 1];
  var PAIR_B     = [1, 1, 2, 2, 2, 2];
  var RACE_TRACK = [0, 1, 0, 1, 0, 1];

  // ── State ──────────────────────────────────────────────────
  var trackIndex     = 0;
  var fading         = false;
  var raceEndTime    = 0;
  var isMobile       = false;
  var ready          = false;

  // UI mode (our own higher-level state on top of C++ GameMode)
  var UI_MAIN_MENU       = 'main_menu';
  var UI_PRACTISE_MENU   = 'practise_menu';
  var UI_PRACTISE_PREVIEW = 'practise_preview';
  var UI_PRACTISE_RACE   = 'practise_race';
  var UI_PRACTISE_RESULT = 'practise_result';
  var UI_SEASON_OVERVIEW = 'season_overview';
  var UI_SEASON_PRE_RACE = 'season_pre_race';
  var UI_SEASON_RACE     = 'season_race';
  var UI_SEASON_RESULT   = 'season_result';
  var UI_SEASON_STANDINGS = 'season_standings';
  var UI_MP_ROLE_SELECT  = 'mp_role_select';
  var UI_MP_HOST_LOBBY   = 'mp_host_lobby';
  var UI_MP_HOST_TRACK   = 'mp_host_track';
  var UI_MP_JOIN          = 'mp_join';
  var UI_MP_JOIN_LOBBY    = 'mp_join_lobby';
  var UI_MP_RACE          = 'mp_race';
  var UI_MP_RESULT        = 'mp_result';

  var uiMode = UI_MAIN_MENU;

  // Touch drive state (mobile only)
  var touchDrive = { left: false, right: false, gas: false, brake: false, boost: false };

  // ── Multiplayer state ──────────────────────────────────────
  var DEFAULT_SIGNALING_URL = 'https://stuntcarracer.fly.dev';
  localStorage.removeItem('scr_signaling_url');  // clean up from older versions
  var signalingUrl = DEFAULT_SIGNALING_URL;
  var mpConnected = false;
  var mpTrackIndex = 0;
  var mpOpponentFinished = false;

  // ── Season state ───────────────────────────────────────────
  var season = null;
  var humanDivision = 0;
  var currentDivAssign = INITIAL_DIVISIONS.slice();

  function createNewSeason(divAssign) {
    // divAssign: array[12], index = driverID, value = division (0-3)
    var divisions = [[], [], [], []];
    for (var i = 0; i < 12; i++) {
      divisions[divAssign[i]].push(i);
    }

    // Random strength for each driver this season
    var strengths = [];
    for (var i = 0; i < 12; i++) {
      strengths.push(BASE_STRENGTH[i] + Math.floor(Math.random() * 64));
    }

    var humanDiv = divAssign[HUMAN_PLAYER];

    // Build race schedule for the human's division
    var divPlayers = divisions[humanDiv].slice();
    var schedule = [];
    for (var r = 0; r < 6; r++) {
      var a = divPlayers[PAIR_A[r]];
      var b = divPlayers[PAIR_B[r]];
      var tIdx = DIVISION_TRACKS[humanDiv][RACE_TRACK[r]];
      schedule.push({
        driverA: a, driverB: b, trackIndex: tIdx,
        isHumanRace: (a === HUMAN_PLAYER || b === HUMAN_PLAYER),
        played: false, winnerDriver: -1, bestLapDriver: -1,
        playerBestLapMs: 0
      });
    }

    // Points tracker
    var points = [];
    for (var i = 0; i < 12; i++) points.push({ wins: 0, bestLaps: 0 });

    return {
      divAssign: divAssign.slice(),
      divisions: divisions,
      strengths: strengths,
      humanDiv: humanDiv,
      schedule: schedule,
      currentRace: 0,
      points: points
    };
  }

  function resolveComputerRace(race) {
    var sA = season.strengths[race.driverA];
    var sB = season.strengths[race.driverB];
    var winner, loser;
    if (sA > sB)       { winner = race.driverA; loser = race.driverB; }
    else if (sB > sA)  { winner = race.driverB; loser = race.driverA; }
    else               { // tie → coin flip
      if (Math.random() < 0.5) { winner = race.driverA; loser = race.driverB; }
      else                     { winner = race.driverB; loser = race.driverA; }
    }
    race.winnerDriver = winner;
    race.bestLapDriver = loser;   // Amiga original: loser gets best lap
    race.played = true;
    season.points[winner].wins++;
    season.points[loser].bestLaps++;
  }

  function driverName(id) {
    if (id === HUMAN_PLAYER) return 'You';
    return OPPONENT_NAMES[id];
  }

  function driverPoints(id) {
    var p = season.points[id];
    return p.wins * 2 + p.bestLaps;
  }

  function divStandings(divIdx) {
    var players = season.divisions[divIdx].slice();
    players.sort(function (a, b) {
      var pa = driverPoints(a), pb = driverPoints(b);
      if (pb !== pa) return pb - pa;
      return season.points[b].wins - season.points[a].wins;
    });
    return players;
  }

  function divLabel(idx) { return 'Division ' + (4 - idx); }

  function fmtLap(ms) {
    if (!ms || ms <= 0) return '-';
    var s = ms / 1000, m = Math.floor(s / 60);
    return m + ':' + ((s - m * 60) < 10 ? '0' : '') + (s - m * 60).toFixed(2);
  }

  // ── C++ API helpers ────────────────────────────────────────

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
  function getPlayerBestLap()  { return Module._jsGetPlayerBestLap(); }
  function getOpponentBestLap(){ return Module._jsGetOpponentBestLap(); }

  // Two-player C++ API
  function setTwoPlayerMode(on)      { Module._jsSetTwoPlayerMode(on ? 1 : 0); }
  function setTwoPlayerSide(side)    { Module._jsSetTwoPlayerSide(side); }
  function getPlayerRoadSection()    { return Module._jsGetPlayerRoadSection(); }
  function getPlayerDistIntoSection(){ return Module._jsGetPlayerDistanceIntoSection(); }
  function getPlayerRoadXPosition()  { return Module._jsGetPlayerRoadXPosition(); }
  function getPlayerZSpeed()         { return Module._jsGetPlayerZSpeed(); }
  function getPlayerDamage()         { return Module._jsGetPlayerDamage(); }
  function getPlayerWheelFL()        { return Module._jsGetPlayerWheelFL(); }
  function getPlayerWheelFR()        { return Module._jsGetPlayerWheelFR(); }
  function getPlayerWheelR()         { return Module._jsGetPlayerWheelR(); }
  function setOpponentState(rs, dist, xPos, zSpd, wFL, wFR, wR) {
    Module._jsSetOpponentState(rs, dist, xPos, zSpd, wFL, wFR, wR);
  }

  function getTrackName() {
    var ptr = Module._jsGetTrackName();
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

  // ══════════════════════════════════════════════════════════════
  //  UI CREATION
  // ══════════════════════════════════════════════════════════════

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

    // ── Main Menu ──
    el('mm-title', 'STUNT CAR RACER',
      'left:50%;top:18%;width:70vw;height:auto;max-width:400px;font-size:min(6vw,32px);' +
      'pointer-events:none;background:none;border:none;text-shadow:0 0 12px rgba(0,0,0,0.9);' +
      'transform:translateX(-50%);');
    el('mm-practise', 'Practise',
      'left:50%;top:40%;width:50vw;height:14vw;max-width:280px;max-height:70px;' +
      'font-size:min(5vw,26px);transform:translateX(-50%);');
    el('mm-season', 'Start the Racing Season',
      'left:50%;top:calc(40% + 16vw);width:50vw;height:14vw;max-width:280px;max-height:70px;' +
      'font-size:min(5vw,26px);transform:translateX(-50%);');

    // ── Track Menu (practise) ──
    el('tc-prev', '\u25C0\uFE0E',
      'left:2vw;bottom:6vh;width:14vw;height:14vw;font-size:min(6vw,32px);max-width:75px;max-height:75px;');
    el('tc-next', '\u25B6\uFE0E',
      'left:18vw;bottom:6vh;width:14vw;height:14vw;font-size:min(6vw,32px);max-width:75px;max-height:75px;');
    el('tc-select', 'Select',
      'right:2vw;bottom:6vh;width:22vw;height:14vw;font-size:min(3.5vw,18px);max-width:130px;max-height:75px;');
    el('tc-trackname', '',
      'left:34vw;right:26vw;width:auto;bottom:6vh;height:14vw;max-height:75px;font-size:min(3.8vw,20px);' +
      'pointer-events:none;background:none;border:none;text-shadow:0 0 8px rgba(0,0,0,0.8);');
    el('tc-backmain', 'Menu',
      'left:2vw;top:2vh;width:18vw;height:10vw;font-size:min(3.5vw,18px);max-width:100px;max-height:55px;');

    // ── Track Preview (practise) ──
    el('tc-back', 'Back',
      'left:2vw;bottom:6vh;width:22vw;height:12vw;font-size:min(4.5vw,22px);max-width:120px;max-height:70px;');
    el('tc-start', 'Start',
      'right:2vw;bottom:6vh;width:22vw;height:12vw;font-size:min(4.5vw,22px);max-width:120px;max-height:70px;');

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

    // ── In-Game common ──
    el('tc-menu', '\u2715',
      'right:2vw;top:2vh;width:10vw;height:10vw;font-size:min(5vw,28px);max-width:55px;max-height:55px;');
    el('tc-lap', '',
      'right:2vw;top:calc(2vh + 10vw + 1vh);width:auto;height:auto;font-size:min(3vw,16px);' +
      'padding:0.4em 0.8em;max-width:120px;pointer-events:none;' +
      'background:rgba(0,0,0,0.4);border:1px solid rgba(255,255,255,0.3);border-radius:8px;');

    // ── Game Over / result label ──
    el('tc-gameover-label', '',
      'left:50%;top:40%;width:80vw;height:auto;font-size:min(7vw,40px);max-width:500px;' +
      'pointer-events:none;background:none;border:none;text-shadow:0 0 12px rgba(0,0,0,0.9);' +
      'transform:translate(-50%,-50%);');
    el('tc-gameover', 'Menu',
      'left:50%;bottom:6vh;width:18vw;height:11vw;font-size:min(3.5vw,18px);max-width:100px;max-height:65px;transform:translateX(-50%);');

    // ── HUD bars ──
    createHudBar('tc-hud-boost', '\uD83D\uDD25', '#ff9900');
    document.getElementById('tc-hud-boost').style.cssText +=
      'left:2vw;right:50%;top:2vh;padding-right:1vw;';
    createHudBar('tc-hud-damage', '\u26A0\uFE0F', '#ff3333');
    document.getElementById('tc-hud-damage').style.cssText +=
      'left:50%;right:14vw;top:2vh;padding-left:1vw;';

    // ── Season overlay ──
    var overlay = document.createElement('div');
    overlay.id = 'season-overlay';
    overlay.style.cssText =
      'position:fixed;left:0;top:0;width:100%;height:100%;' +
      'display:none;align-items:center;justify-content:center;' +
      'pointer-events:auto;z-index:150;background:rgba(0,0,0,0.85);';
    var card = document.createElement('div');
    card.id = 'season-card';
    card.style.cssText =
      'background:rgba(20,20,40,0.95);color:#fff;border:2px solid rgba(255,255,255,0.3);' +
      'border-radius:16px;padding:3vh 4vw;max-width:90vw;max-height:85vh;' +
      'overflow-y:auto;font-family:Arial,sans-serif;text-align:center;' +
      'box-sizing:border-box;min-width:min(80vw,360px);';
    overlay.appendChild(card);
    document.body.appendChild(overlay);

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

  // ══════════════════════════════════════════════════════════════
  //  SEASON OVERLAY SCREENS
  // ══════════════════════════════════════════════════════════════

  function showOverlay(html) {
    document.getElementById('season-card').innerHTML = html;
    document.getElementById('season-overlay').style.display = 'flex';
  }

  function hideOverlay() {
    document.getElementById('season-overlay').style.display = 'none';
  }

  function btnCss() {
    return 'display:inline-block;margin:1.5vh 1vw;padding:1.5vh 4vw;' +
      'background:rgba(255,255,255,0.18);color:#fff;border:2px solid rgba(255,255,255,0.4);' +
      'border-radius:12px;font-family:Arial,sans-serif;font-weight:bold;' +
      'font-size:min(4vw,20px);cursor:pointer;touch-action:none;user-select:none;-webkit-user-select:none;';
  }

  function overlayBtn(id, label, handler) {
    var el = document.getElementById(id);
    if (!el) return;
    el.addEventListener('click', function () { handler(); });
    el.addEventListener('touchstart', function (e) { e.preventDefault(); handler(); }, { passive: false });
  }

  // ── Season overview screen ──
  function showSeasonOverview() {
    uiMode = UI_SEASON_OVERVIEW;
    var h = '<div style="font-size:min(5vw,28px);margin-bottom:2vh;">Season Overview</div>';
    h += '<div style="display:flex;gap:1vw;justify-content:center;align-items:stretch;margin:1vh 0;">';
    // Show divisions from highest (Div 1 = index 3) to lowest (Div 4 = index 0)
    for (var di = 3; di >= 0; di--) {
      var players = season.divisions[di];
      var tracks = DIVISION_TRACKS[di];
      var isHumanDiv = (di === season.humanDiv);
      h += '<div style="flex:1;background:rgba(255,255,255,' + (isHumanDiv ? '0.12' : '0.05') + ');' +
        'border:1px solid rgba(255,255,255,' + (isHumanDiv ? '0.4' : '0.15') + ');' +
        'border-radius:10px;padding:1.5vh 1vw;display:flex;flex-direction:column;">';
      h += '<div style="font-size:min(3.5vw,17px);font-weight:bold;margin-bottom:1vh;' +
        (isHumanDiv ? 'color:#ffdd44;' : '') + '">' + divLabel(di) + '</div>';
      // Players
      for (var p = 0; p < players.length; p++) {
        var pid = players[p];
        var isH = (pid === HUMAN_PLAYER);
        h += '<div style="font-size:min(3vw,15px);padding:0.3vh 0;' +
          (isH ? 'color:#ffdd44;font-weight:bold;' : 'opacity:0.8;') + '">' +
          driverName(pid) + '</div>';
      }
      // Spacer
      h += '<div style="flex:1;"></div>';
      // Tracks
      h += '<div style="margin-top:1.5vh;border-top:1px solid rgba(255,255,255,0.15);padding-top:1vh;">';
      for (var t = 0; t < tracks.length; t++) {
        h += '<div style="font-size:min(2.5vw,13px);opacity:0.6;padding:0.2vh 0;">' +
          TRACK_NAMES[tracks[t]] + '</div>';
      }
      h += '</div></div>';
    }
    h += '</div>';
    h += '<div id="s-btn-go" style="' + btnCss() + 'margin-top:2vh;">Continue</div>';
    showOverlay(h);
    overlayBtn('s-btn-go', 'GO', function () { showPreRace(); });
  }

  // ── Pre-race screen ──
  function showPreRace() {
    var race = season.schedule[season.currentRace];
    if (!race.isHumanRace) {
      resolveComputerRace(race);
      showRaceResult(race);
      return;
    }
    uiMode = UI_SEASON_PRE_RACE;
    var h = '<div style="font-size:min(3.5vw,18px);opacity:0.7;margin-bottom:1vh;">' +
      divLabel(season.humanDiv) + ' \u2014 Race ' + (season.currentRace + 1) + ' of 6</div>';
    h += '<div style="font-size:min(5vw,28px);margin:2vh 0;">' + TRACK_NAMES[race.trackIndex] + '</div>';
    h += '<div style="font-size:min(6vw,32px);margin:2vh 0;">' +
      driverName(race.driverA) + ' <span style="opacity:0.5;font-size:min(4vw,22px);">vs</span> ' +
      driverName(race.driverB) + '</div>';
    h += '<div id="s-btn-race" style="' + btnCss() + '">Race</div>';
    h += '<div id="s-btn-quit" style="' + btnCss() + 'opacity:0.5;font-size:min(3vw,14px);">Quit Season</div>';
    showOverlay(h);

    overlayBtn('s-btn-race', 'RACE', beginSeasonRace);
    overlayBtn('s-btn-quit', 'QUIT', quitSeason);
  }

  function beginSeasonRace() {
    var race = season.schedule[season.currentRace];
    var oppId = (race.driverA === HUMAN_PLAYER) ? race.driverB : race.driverA;
    hideOverlay();
    fadeAndDo(function () {
      selectTrack(race.trackIndex);
      startGame(oppId);
      uiMode = UI_SEASON_RACE;
      showUIForMode();
    });
  }

  function simComputerRace() {
    var race = season.schedule[season.currentRace];
    resolveComputerRace(race);
    showRaceResult(race);
  }

  function showRaceResult(race) {
    uiMode = UI_SEASON_RESULT;
    var h = '<div style="font-size:min(3.5vw,18px);opacity:0.7;margin-bottom:1vh;">' +
      'Race Result \u2014 ' + TRACK_NAMES[race.trackIndex] + '</div>';
    h += '<div style="font-size:min(5vw,26px);margin:2vh 0;">' +
      driverName(race.driverA) + ' vs ' + driverName(race.driverB) + '</div>';
    h += '<div style="font-size:min(4vw,22px);margin:1vh 0;">' +
      '\uD83C\uDFC6 Winner: <b>' + driverName(race.winnerDriver) + '</b> (+2 pts)</div>';
    h += '<div style="font-size:min(3.5vw,18px);margin:1vh 0;">' +
      '\u23F1\uFE0F Fastest Lap: <b>' + driverName(race.bestLapDriver) + '</b> (+1 pt)</div>';
    if (race.playerBestLapMs > 0) {
      h += '<div style="font-size:min(3vw,16px);opacity:0.6;margin:1vh 0;">Your best lap: ' +
        fmtLap(race.playerBestLapMs) + '</div>';
    }
    h += '<div id="s-btn-cont" style="' + btnCss() + '">Continue</div>';
    showOverlay(h);
    overlayBtn('s-btn-cont', 'CONTINUE', advanceSeason);
  }

  function advanceSeason() {
    season.currentRace++;
    if (season.currentRace >= 6) {
      showStandings();
    } else {
      showPreRace();
    }
  }

  function showStandings() {
    uiMode = UI_SEASON_STANDINGS;
    var di = season.humanDiv;
    var st = divStandings(di);

    // Compute and save new division assignments
    var na = season.divAssign.slice();
    for (var d = 0; d < 3; d++) {
      var sH = divStandings(d), sA = divStandings(d + 1);
      na[sH[0]] = d + 1;
      na[sA[sA.length - 1]] = d;
    }
    currentDivAssign = na;
    humanDivision = na[HUMAN_PLAYER];

    var h = '<div style="font-size:min(5vw,28px);margin-bottom:2vh;">' + divLabel(di) + ' Standings</div>';
    h += '<table style="width:100%;border-collapse:collapse;font-size:min(3.5vw,18px);margin:1vh 0;">';
    h += '<tr style="opacity:0.6;"><td style="text-align:left;padding:0.5vh 1vw;">#</td>' +
      '<td style="text-align:left;padding:0.5vh 1vw;">Driver</td>' +
      '<td style="text-align:center;padding:0.5vh 1vw;">W</td>' +
      '<td style="text-align:center;padding:0.5vh 1vw;">FL</td>' +
      '<td style="text-align:center;padding:0.5vh 1vw;">Pts</td></tr>';

    for (var i = 0; i < st.length; i++) {
      var d = st[i], p = season.points[d], pts = p.wins * 2 + p.bestLaps;
      var isH = (d === HUMAN_PLAYER);
      var badge = '';
      if (i === 0 && di < 3)  badge = ' \u2B06\uFE0F';
      if (i === 0 && di === 3) badge = ' \uD83C\uDFC6';
      if (i === st.length - 1 && di > 0) badge = ' \u2B07\uFE0F';
      h += '<tr style="' + (isH ? 'color:#ffdd44;' : '') + '">' +
        '<td style="text-align:left;padding:0.5vh 1vw;">' + (i + 1) + '</td>' +
        '<td style="text-align:left;padding:0.5vh 1vw;">' + driverName(d) + badge + '</td>' +
        '<td style="text-align:center;padding:0.5vh 1vw;">' + p.wins + '</td>' +
        '<td style="text-align:center;padding:0.5vh 1vw;">' + p.bestLaps + '</td>' +
        '<td style="text-align:center;padding:0.5vh 1vw;font-weight:bold;">' + pts + '</td></tr>';
    }
    h += '</table>';

    var top = st[0], bot = st[st.length - 1];
    if (top === HUMAN_PLAYER) {
      if (di === 3) h += '<div style="font-size:min(5vw,26px);margin:2vh 0;color:#ffd700;">' +
        '\uD83C\uDFC6 SUPER LEAGUE CHAMPION! \uD83C\uDFC6</div>';
      else h += '<div style="font-size:min(4vw,20px);margin:2vh 0;color:#44ff44;">' +
        '\u2B06\uFE0F Promoted to ' + divLabel(di + 1) + '!</div>';
    } else if (bot === HUMAN_PLAYER) {
      if (di === 0) h += '<div style="font-size:min(4vw,20px);margin:2vh 0;color:#ff8844;">' +
        'Bottom of the league \u2014 try again!</div>';
      else h += '<div style="font-size:min(4vw,20px);margin:2vh 0;color:#ff4444;">' +
        '\u2B07\uFE0F Relegated to ' + divLabel(di - 1) + '</div>';
    } else {
      h += '<div style="font-size:min(3.5vw,18px);margin:2vh 0;opacity:0.7;">' +
        'Staying in ' + divLabel(di) + '</div>';
    }

    h += '<div id="s-btn-next" style="' + btnCss() + '">Finish Season</div>';
    showOverlay(h);
    overlayBtn('s-btn-next', 'FINISH', finishSeason);
  }

  function finishSeason() {
    season = null;
    hideOverlay();
    goToMenu();
    uiMode = UI_MAIN_MENU;
    showUIForMode();
  }

  function finishSeasonRace() {
    var race = season.schedule[season.currentRace];
    var won = isRaceWon();
    var wrecked = isPlayerWrecked();
    var pBest = getPlayerBestLap();
    var oBest = getOpponentBestLap();
    var opponent = (race.driverA === HUMAN_PLAYER) ? race.driverB : race.driverA;

    if (wrecked) {
      race.winnerDriver = opponent;
      race.bestLapDriver = opponent;
    } else {
      race.winnerDriver = won ? HUMAN_PLAYER : opponent;
      if (pBest > 0 && oBest > 0)
        race.bestLapDriver = (pBest <= oBest) ? HUMAN_PLAYER : opponent;
      else if (pBest > 0)
        race.bestLapDriver = HUMAN_PLAYER;
      else
        race.bestLapDriver = opponent;
    }

    race.playerBestLapMs = pBest;
    race.played = true;
    season.points[race.winnerDriver].wins++;
    season.points[race.bestLapDriver].bestLaps++;

    fadeAndDo(function () {
      goToMenu();
      showRaceResult(race);
    });
  }

  function quitSeason() {
    season = null;
    hideOverlay();
    goToMenu();
    uiMode = UI_MAIN_MENU;
    showUIForMode();
  }

  // ══════════════════════════════════════════════════════════════
  //  MULTIPLAYER
  // ══════════════════════════════════════════════════════════════

  // ── Per-frame state exchange ───────────────────────────────
  // Packet: 7 × Int32 = 28 bytes
  //   [0] road section, [1] distance into section, [2] road X position,
  //   [3] z speed, [4] wheel FL, [5] wheel FR, [6] wheel R

  function mpSendState() {
    if (!SCR_Multiplayer.isConnected()) return;
    var buf = new ArrayBuffer(28);
    var view = new Int32Array(buf);
    view[0] = getPlayerRoadSection();
    view[1] = getPlayerDistIntoSection();
    view[2] = getPlayerRoadXPosition();
    view[3] = getPlayerZSpeed();
    view[4] = getPlayerWheelFL();
    view[5] = getPlayerWheelFR();
    view[6] = getPlayerWheelR();
    SCR_Multiplayer.send(buf);
  }

  function mpReceiveState(data) {
    if (!(data instanceof ArrayBuffer) || data.byteLength < 28) return;
    var view = new Int32Array(data);
    setOpponentState(view[0], view[1], view[2], view[3], view[4], view[5], view[6]);
  }

  function mpSetupCallbacks() {
    SCR_Multiplayer.onMessage = mpReceiveState;
    SCR_Multiplayer.onReliableMessage = function (msg) {
      if (msg.type === 'track' && !SCR_Multiplayer.isHost()) {
        // Host selected a track
        mpTrackIndex = msg.trackIndex;
        selectTrack(mpTrackIndex);
        // Start the race
        hideOverlay();
        fadeAndDo(function () {
          setTwoPlayerMode(true);
          setTwoPlayerSide(1);  // joiner on right
          startGame(-2);
          uiMode = UI_MP_RACE;
          showUIForMode();
        });
      } else if (msg.type === 'finished') {
        mpOpponentFinished = true;
      } else if (msg.type === 'quit') {
        // Opponent explicitly quit — trigger the same disconnect handling
        mpConnected = false;
        if (uiMode === UI_MP_RACE) {
          var lbl = document.getElementById('tc-gameover-label');
          if (lbl) { lbl.textContent = 'OPPONENT QUIT'; lbl.style.display = 'flex'; lbl.style.opacity = '1'; }
          setTimeout(function () {
            setGameOver();
            mpCleanup();
            goToMenu();
            uiMode = UI_MAIN_MENU;
            showUIForMode();
          }, 3000);
        }
      }
    };
    SCR_Multiplayer.onClose = function () {
      mpConnected = false;
      if (uiMode === UI_MP_RACE) {
        // Show "Opponent quit" message and end the race after a short delay
        var lbl = document.getElementById('tc-gameover-label');
        if (lbl) { lbl.textContent = 'OPPONENT QUIT'; lbl.style.display = 'flex'; lbl.style.opacity = '1'; }
        setTimeout(function () {
          setGameOver();
          mpCleanup();
          goToMenu();
          uiMode = UI_MAIN_MENU;
          showUIForMode();
        }, 3000);
      } else {
        // Back to main menu
        setTwoPlayerMode(false);
        goToMenu();
        uiMode = UI_MAIN_MENU;
        showUIForMode();
      }
    };
  }

  function mpCleanup() {
    SCR_Multiplayer.cleanup();
    mpConnected = false;
    mpOpponentFinished = false;
    setTwoPlayerMode(false);
  }

  // ── Multiplayer UI screens ─────────────────────────────────

  function showMpRoleSelect() {
    uiMode = UI_MP_ROLE_SELECT;
    var h = '<div style="font-size:min(5vw,28px);margin-bottom:2vh;">Two Players</div>';
    h += '<div style="font-size:min(3vw,16px);opacity:0.7;margin-bottom:2vh;">' +
      'Connect via WebRTC peer-to-peer</div>';
    h += '<div style="font-size:min(2.5vw,13px);opacity:0.5;margin-bottom:0.5vh;">Signaling server</div>';
    h += '<input id="mp-sig-url" type="text" value="' + signalingUrl.replace(/"/g, '&quot;') + '" ' +
      'style="font-size:min(3vw,15px);text-align:center;width:min(70vw,340px);' +
      'padding:0.8vh 1.5vw;border:1px solid rgba(255,255,255,0.3);border-radius:8px;' +
      'background:rgba(255,255,255,0.08);color:#fff;font-family:monospace;outline:none;' +
      'margin-bottom:2vh;" />';
    h += '<div id="mp-btn-host" style="' + btnCss() + '">Host Game</div><br>';
    h += '<div id="mp-btn-join" style="' + btnCss() + '">Join Game</div><br>';
    h += '<div id="mp-btn-back" style="' + btnCss() + 'opacity:0.5;font-size:min(3vw,14px);">Back</div>';
    showOverlay(h);

    function saveSignalingUrl() {
      var inp = document.getElementById('mp-sig-url');
      if (inp) {
        signalingUrl = inp.value.replace(/\/+$/, '');
      }
    }

    overlayBtn('mp-btn-host', 'HOST', function () { saveSignalingUrl(); startHosting(); });
    overlayBtn('mp-btn-join', 'JOIN', function () { saveSignalingUrl(); showJoinScreen(); });
    overlayBtn('mp-btn-back', 'BACK', function () {
      hideOverlay();
      uiMode = UI_MAIN_MENU;
      showUIForMode();
    });
  }

  function startHosting() {
    uiMode = UI_MP_HOST_LOBBY;
    var h = '<div style="font-size:min(5vw,28px);margin-bottom:2vh;">Hosting Game</div>';
    h += '<div style="font-size:min(3vw,16px);opacity:0.7;margin-bottom:2vh;">Connecting to signaling server\u2026</div>';
    h += '<div id="mp-host-code" style="font-size:min(10vw,60px);letter-spacing:0.3em;margin:2vh 0;"></div>';
    h += '<div id="mp-host-status" style="font-size:min(3vw,16px);opacity:0.7;margin:1vh 0;">Setting up\u2026</div>';
    h += '<div id="mp-btn-cancel" style="' + btnCss() + 'opacity:0.5;font-size:min(3vw,14px);">Cancel</div>';
    showOverlay(h);
    overlayBtn('mp-btn-cancel', 'CANCEL', function () {
      mpCleanup();
      hideOverlay();
      uiMode = UI_MAIN_MENU;
      showUIForMode();
    });

    mpSetupCallbacks();

    // Override onOpen to detect connection
    var origReliableHandler = SCR_Multiplayer.onReliableMessage;
    SCR_Multiplayer.onOpen = function () {
      mpConnected = true;
      SCR_Multiplayer.onReliableMessage = origReliableHandler;
      // Go to track selection
      showMpHostTrack();
    };

    SCR_Multiplayer.host(signalingUrl).then(function (code) {
      var codeEl = document.getElementById('mp-host-code');
      if (codeEl) codeEl.textContent = code;
      var statusEl = document.getElementById('mp-host-status');
      if (statusEl) statusEl.textContent = 'Share this code \u2014 waiting for opponent\u2026';
    }).catch(function (err) {
      var statusEl = document.getElementById('mp-host-status');
      if (statusEl) statusEl.textContent = 'Error: ' + err.message;
    });
  }

  function showMpHostTrack() {
    uiMode = UI_MP_HOST_TRACK;
    mpTrackIndex = 0;
    selectTrack(mpTrackIndex);
    hideOverlay();
    // Show track selection UI (reuse practise menu style but with different buttons)
    var h = '<div style="font-size:min(5vw,28px);margin-bottom:2vh;">Select Track</div>';
    h += '<div style="font-size:min(3.5vw,18px);opacity:0.7;margin-bottom:2vh;">Opponent connected!</div>';
    h += '<div id="mp-track-name" style="font-size:min(5vw,26px);margin:2vh 0;">' +
      TRACK_NAMES[mpTrackIndex] + '</div>';
    h += '<div style="display:flex;justify-content:center;gap:2vw;">';
    h += '<div id="mp-btn-prev" style="' + btnCss() + '">\u25C0\uFE0E</div>';
    h += '<div id="mp-btn-next" style="' + btnCss() + '">\u25B6\uFE0E</div>';
    h += '</div>';
    h += '<div id="mp-btn-go" style="' + btnCss() + 'margin-top:2vh;">Start Race</div>';
    h += '<div id="mp-btn-cancel2" style="' + btnCss() + 'opacity:0.5;font-size:min(3vw,14px);">Cancel</div>';
    showOverlay(h);
    overlayBtn('mp-btn-prev', 'PREV', function () {
      mpTrackIndex--;
      if (mpTrackIndex < 0) mpTrackIndex = getNumTracks() - 1;
      selectTrack(mpTrackIndex);
      var el = document.getElementById('mp-track-name');
      if (el) el.textContent = TRACK_NAMES[mpTrackIndex];
    });
    overlayBtn('mp-btn-next', 'NEXT', function () {
      mpTrackIndex++;
      if (mpTrackIndex >= getNumTracks()) mpTrackIndex = 0;
      selectTrack(mpTrackIndex);
      var el = document.getElementById('mp-track-name');
      if (el) el.textContent = TRACK_NAMES[mpTrackIndex];
    });
    overlayBtn('mp-btn-go', 'GO', function () {
      // Tell the joiner which track
      SCR_Multiplayer.sendReliable({ type: 'track', trackIndex: mpTrackIndex });
      // Start our own race
      hideOverlay();
      fadeAndDo(function () {
        selectTrack(mpTrackIndex);
        setTwoPlayerMode(true);
        setTwoPlayerSide(0);  // host on left
        startGame(-2);
        uiMode = UI_MP_RACE;
        mpOpponentFinished = false;
        showUIForMode();
      });
    });
    overlayBtn('mp-btn-cancel2', 'CANCEL', function () {
      mpCleanup();
      hideOverlay();
      uiMode = UI_MAIN_MENU;
      showUIForMode();
    });
  }

  function showJoinScreen() {
    uiMode = UI_MP_JOIN;
    var h = '<div style="font-size:min(5vw,28px);margin-bottom:2vh;">Join Game</div>';
    h += '<div style="font-size:min(3vw,16px);opacity:0.7;margin-bottom:2vh;">Enter the 4-letter code from the host</div>';
    h += '<input id="mp-code-input" type="text" maxlength="4" autocapitalize="characters" ' +
      'style="font-size:min(10vw,60px);text-align:center;letter-spacing:0.3em;width:min(60vw,300px);' +
      'padding:1vh 2vw;border:2px solid rgba(255,255,255,0.4);border-radius:12px;' +
      'background:rgba(255,255,255,0.1);color:#fff;font-family:monospace;outline:none;" />';
    h += '<div id="mp-join-status" style="font-size:min(3vw,16px);opacity:0.7;margin:1vh 0;min-height:3vh;"></div>';
    h += '<div id="mp-btn-connect" style="' + btnCss() + '">Connect</div>';
    h += '<div id="mp-btn-jback" style="' + btnCss() + 'opacity:0.5;font-size:min(3vw,14px);">Back</div>';
    showOverlay(h);
    // Focus input
    setTimeout(function () {
      var inp = document.getElementById('mp-code-input');
      if (inp) inp.focus();
    }, 100);
    overlayBtn('mp-btn-connect', 'CONNECT', function () {
      var code = (document.getElementById('mp-code-input').value || '').toUpperCase().trim();
      if (code.length !== 4) {
        document.getElementById('mp-join-status').textContent = 'Code must be 4 characters';
        return;
      }
      joinGame(code);
    });
    overlayBtn('mp-btn-jback', 'BACK', function () {
      hideOverlay();
      showMpRoleSelect();
    });
    // Also allow Enter to connect
    var inp = document.getElementById('mp-code-input');
    if (inp) {
      inp.addEventListener('keydown', function (e) {
        if (e.key === 'Enter') {
          e.preventDefault();
          var btn = document.getElementById('mp-btn-connect');
          if (btn) btn.click();
        }
      });
    }
  }

  function joinGame(code) {
    uiMode = UI_MP_JOIN_LOBBY;
    var statusEl = document.getElementById('mp-join-status');
    if (statusEl) statusEl.textContent = 'Connecting\u2026';

    mpSetupCallbacks();

    SCR_Multiplayer.onOpen = function () {
      mpConnected = true;
      if (statusEl) statusEl.textContent = 'Connected! Waiting for host to select track\u2026';
      // Disable the Connect button and gray out the code input
      var btn = document.getElementById('mp-btn-connect');
      if (btn) { btn.style.opacity = '0.3'; btn.style.pointerEvents = 'none'; }
      var inp = document.getElementById('mp-code-input');
      if (inp) { inp.disabled = true; inp.style.opacity = '0.3'; }
    };

    SCR_Multiplayer.join(signalingUrl, code).then(function () {
      // Connected, waiting for track selection message via onReliableMessage
    }).catch(function (err) {
      if (statusEl) statusEl.textContent = 'Error: ' + err.message;
    });
  }

  function finishMpRace() {
    // Notify opponent we finished
    if (SCR_Multiplayer.isConnected()) {
      SCR_Multiplayer.sendReliable({ type: 'finished' });
    }
    var won = isRaceWon();
    var wrecked = isPlayerWrecked();
    uiMode = UI_MP_RESULT;
    var h = '<div style="font-size:min(5vw,28px);margin-bottom:2vh;">Race Complete</div>';
    if (wrecked) {
      h += '<div style="font-size:min(6vw,32px);margin:2vh 0;color:#ff4444;">WRECKED</div>';
    } else if (won) {
      h += '<div style="font-size:min(6vw,32px);margin:2vh 0;color:#44ff44;">\uD83C\uDFC6 YOU WIN!</div>';
    } else {
      h += '<div style="font-size:min(6vw,32px);margin:2vh 0;color:#ff8844;">YOU LOSE</div>';
    }
    var pBest = getPlayerBestLap();
    if (pBest > 0) {
      h += '<div style="font-size:min(3.5vw,18px);opacity:0.7;margin:1vh 0;">Your best lap: ' + fmtLap(pBest) + '</div>';
    }
    h += '<div id="mp-btn-again" style="' + btnCss() + '">Play Again</div>';
    h += '<div id="mp-btn-quit" style="' + btnCss() + 'opacity:0.5;font-size:min(3vw,14px);">Quit</div>';
    showOverlay(h);
    overlayBtn('mp-btn-again', 'AGAIN', function () {
      hideOverlay();
      mpOpponentFinished = false;
      if (SCR_Multiplayer.isHost()) {
        showMpHostTrack();
      } else {
        // Show waiting screen
        uiMode = UI_MP_JOIN_LOBBY;
        var h2 = '<div style="font-size:min(5vw,28px);margin-bottom:2vh;">Waiting</div>';
        h2 += '<div style="font-size:min(3vw,16px);opacity:0.7;">Waiting for host to select next track\u2026</div>';
        showOverlay(h2);
      }
    });
    overlayBtn('mp-btn-quit', 'QUIT', function () {
      mpCleanup();
      hideOverlay();
      goToMenu();
      uiMode = UI_MAIN_MENU;
      showUIForMode();
    });
  }

  // ── Main menu screen ──
  function showMainMenu() {
    var h = '<div style="font-size:min(6vw,32px);margin-bottom:1vh;">STUNT CAR RACER</div>';
    h += '<div style="font-size:min(3.5vw,18px);opacity:0.7;margin-bottom:3vh;">' + divLabel(humanDivision) + '</div>';
    h += '<div id="mm-btn-practise" style="' + btnCss() + '">Practise</div><br>';
    h += '<div id="mm-btn-season" style="' + btnCss() + '">Start the Racing Season</div><br>';
    h += '<div id="mm-btn-twoplayer" style="' + btnCss() + '">Two Players</div>';
    showOverlay(h);
    overlayBtn('mm-btn-practise', 'PRACTISE', function () {
      hideOverlay();
      fadeAndDo(function () { uiMode = UI_PRACTISE_MENU; showUIForMode(); });
    });
    overlayBtn('mm-btn-season', 'SEASON', function () {
      hideOverlay();
      fadeAndDo(function () {
        season = createNewSeason(currentDivAssign.slice());
        showSeasonOverview();
      });
    });
    overlayBtn('mm-btn-twoplayer', 'TWO PLAYERS', function () {
      hideOverlay();
      showMpRoleSelect();
    });
  }

  // ══════════════════════════════════════════════════════════════
  //  BUTTON & KEYBOARD WIRING
  // ══════════════════════════════════════════════════════════════

  function handleMenuDuringRace() {
    fadeAndDo(function () {
      if (uiMode === UI_MP_RACE) {
        setGameOver();
        // Notify opponent we're quitting
        if (SCR_Multiplayer.isConnected()) {
          try { SCR_Multiplayer.sendReliable({ type: 'quit' }); } catch(e) {}
        }
        mpCleanup();
        goToMenu();
        uiMode = UI_MAIN_MENU;
        showUIForMode();
      } else if (uiMode === UI_SEASON_RACE) {
        var race = season.schedule[season.currentRace];
        var opp = (race.driverA === HUMAN_PLAYER) ? race.driverB : race.driverA;
        race.winnerDriver = opp;
        race.bestLapDriver = opp;
        race.played = true;
        season.points[opp].wins++;
        season.points[opp].bestLaps++;
        goToMenu();
        showRaceResult(race);
      } else {
        goToMenu();
        uiMode = UI_MAIN_MENU;
        showUIForMode();
      }
    });
  }

  function addBtn(id, cb) {
    var btn = document.getElementById(id);
    function h(e) {
      e.preventDefault();
      btn.style.background = 'rgba(255,255,255,0.45)';
      cb();
      setTimeout(function () { btn.style.background = 'rgba(255,255,255,0.18)'; }, 200);
    }
    btn.addEventListener('touchstart', h, { passive: false });
    btn.addEventListener('mousedown', h);
  }

  function addDriveBtn(id, field) {
    var btn = document.getElementById(id);
    btn.addEventListener('touchstart', function (e) {
      e.preventDefault(); btn.style.background = 'rgba(255,255,255,0.45)';
      touchDrive[field] = true; updateDriveFlags();
    }, { passive: false });
    btn.addEventListener('touchend', function (e) {
      e.preventDefault(); btn.style.background = 'rgba(255,255,255,0.18)';
      touchDrive[field] = false; updateDriveFlags();
    }, { passive: false });
    btn.addEventListener('touchcancel', function (e) {
      e.preventDefault(); btn.style.background = 'rgba(255,255,255,0.18)';
      touchDrive[field] = false; updateDriveFlags();
    }, { passive: false });
  }

  function updateDriveFlags() {
    var d = touchDrive, f = 0;
    if (d.left)  f |= KEY_LEFT;
    if (d.right) f |= KEY_RIGHT;
    if (d.gas && d.boost)   f |= KEY_ACCEL_BOOST;
    else if (d.gas)         f |= KEY_ACCEL_ONLY;
    if (d.brake && d.boost) f |= KEY_BRAKE_BOOST;
    else if (d.brake)       f |= KEY_HASH;
    setDriveInput(f);
  }

  function wireButtons() {
    // Main Menu
    addBtn('mm-practise', function () {
      fadeAndDo(function () { uiMode = UI_PRACTISE_MENU; showUIForMode(); });
    });
    addBtn('mm-season', function () {
      fadeAndDo(function () {
        season = createNewSeason(INITIAL_DIVISIONS.slice());
        showPreRace();
      });
    });

    // Track Menu (practise)
    addBtn('tc-prev', prevTrack);
    addBtn('tc-next', nextTrack);
    addBtn('tc-select', function () {
      if (getTrackID() < 0) return;
      fadeAndDo(function () { startPreview(); uiMode = UI_PRACTISE_PREVIEW; showUIForMode(); });
    });
    addBtn('tc-backmain', function () {
      fadeAndDo(function () { goToMenu(); uiMode = UI_MAIN_MENU; showUIForMode(); });
    });

    // Track Preview (practise)
    addBtn('tc-back', function () {
      fadeAndDo(function () { goToMenu(); uiMode = UI_PRACTISE_MENU; showUIForMode(); });
    });
    addBtn('tc-start', function () {
      fadeAndDo(function () { startGame(-2); uiMode = UI_PRACTISE_RACE; showUIForMode(); });
    });

    // In-Game drive
    addDriveBtn('tc-left', 'left');
    addDriveBtn('tc-right', 'right');
    addDriveBtn('tc-accel', 'gas');
    addDriveBtn('tc-brake', 'brake');
    addDriveBtn('tc-boost', 'boost');

    // Close / menu
    addBtn('tc-menu', handleMenuDuringRace);

    // Game Over (practise)
    addBtn('tc-gameover', function () {
      fadeAndDo(function () { goToMenu(); uiMode = UI_PRACTISE_MENU; showUIForMode(); });
    });
  }

  function wireKeyboard() {
    document.addEventListener('keydown', function (e) {
      // Season overlay: Enter/Space → primary button, Escape → quit
      if (uiMode === UI_SEASON_OVERVIEW || uiMode === UI_SEASON_PRE_RACE || uiMode === UI_SEASON_RESULT || uiMode === UI_SEASON_STANDINGS) {
        if (e.key === 'Enter' || e.key === ' ') {
          e.preventDefault();
          var btns = document.querySelectorAll('#season-card div[id^="s-btn-"]');
          if (btns.length > 0) btns[0].click();
          return;
        }
        if (e.key === 'Escape') { e.preventDefault(); quitSeason(); return; }
      }

      // Multiplayer overlays: Escape → back/cancel
      if (uiMode === UI_MP_ROLE_SELECT || uiMode === UI_MP_HOST_LOBBY || uiMode === UI_MP_HOST_TRACK ||
          uiMode === UI_MP_JOIN || uiMode === UI_MP_JOIN_LOBBY || uiMode === UI_MP_RESULT) {
        if (e.key === 'Escape') {
          e.preventDefault();
          mpCleanup();
          hideOverlay();
          goToMenu();
          uiMode = UI_MAIN_MENU;
          showUIForMode();
          return;
        }
        if (e.key === 'Enter' || e.key === ' ') {
          e.preventDefault();
          var btns = document.querySelectorAll('#season-card div[id^="mp-btn-"]');
          if (btns.length > 0) btns[0].click();
          return;
        }
        if (uiMode === UI_MP_HOST_TRACK) {
          if (e.key === 'ArrowLeft') {
            e.preventDefault();
            var b = document.getElementById('mp-btn-prev');
            if (b) b.click();
          } else if (e.key === 'ArrowRight') {
            e.preventDefault();
            var b = document.getElementById('mp-btn-next');
            if (b) b.click();
          }
        }
        return;
      }

      if (uiMode === UI_MAIN_MENU) {
        if (e.key === '1' || e.key === 'p' || e.key === 'P') {
          e.preventDefault();
          var b = document.getElementById('mm-btn-practise');
          if (b) b.click();
        } else if (e.key === '2' || e.key === 's' || e.key === 'S' || e.key === 'Enter' || e.key === ' ') {
          e.preventDefault();
          var b = document.getElementById('mm-btn-season');
          if (b) b.click();
        } else if (e.key === '3' || e.key === 'm' || e.key === 'M') {
          e.preventDefault();
          var b = document.getElementById('mm-btn-twoplayer');
          if (b) b.click();
        }
        return;
      }

      if (uiMode === UI_PRACTISE_MENU) {
        if (e.key === 'ArrowLeft')       { e.preventDefault(); prevTrack(); }
        else if (e.key === 'ArrowRight') { e.preventDefault(); nextTrack(); }
        else if (e.key === 'Enter' || e.key === ' ') {
          e.preventDefault();
          if (getTrackID() >= 0) fadeAndDo(function () { startPreview(); uiMode = UI_PRACTISE_PREVIEW; showUIForMode(); });
        } else if (e.key === 'Backspace' || e.key === 'Escape') {
          e.preventDefault(); fadeAndDo(function () { goToMenu(); uiMode = UI_MAIN_MENU; showUIForMode(); });
        }
        return;
      }

      if (uiMode === UI_PRACTISE_PREVIEW) {
        if (e.key === 'Enter' || e.key === ' ') {
          e.preventDefault(); fadeAndDo(function () { startGame(-2); uiMode = UI_PRACTISE_RACE; showUIForMode(); });
        } else if (e.key === 'Backspace' || e.key === 'Escape') {
          e.preventDefault(); fadeAndDo(function () { goToMenu(); uiMode = UI_PRACTISE_MENU; showUIForMode(); });
        }
        return;
      }

      if (uiMode === UI_PRACTISE_RACE || uiMode === UI_SEASON_RACE || uiMode === UI_MP_RACE) {
        if (e.key === 'Backspace' || e.key === 'Escape') {
          e.preventDefault(); handleMenuDuringRace();
        }
        return;
      }

      if (uiMode === UI_PRACTISE_RESULT) {
        if (e.key === 'Enter' || e.key === ' ' || e.key === 'Backspace' || e.key === 'Escape') {
          e.preventDefault(); fadeAndDo(function () { goToMenu(); uiMode = UI_PRACTISE_MENU; showUIForMode(); });
        }
        return;
      }
    });
  }

  // ══════════════════════════════════════════════════════════════
  //  UI VISIBILITY
  // ══════════════════════════════════════════════════════════════

  var ALL_ELS = [
    'mm-practise', 'mm-season', 'mm-title',
    'tc-prev', 'tc-next', 'tc-select', 'tc-trackname', 'tc-backmain',
    'tc-back', 'tc-start',
    'tc-left', 'tc-right', 'tc-accel', 'tc-brake', 'tc-boost',
    'tc-menu', 'tc-lap', 'tc-hud-boost', 'tc-hud-damage',
    'tc-gameover-label', 'tc-gameover'
  ];

  function hideAllUI() {
    for (var i = 0; i < ALL_ELS.length; i++) {
      var e = document.getElementById(ALL_ELS[i]);
      if (e) e.style.display = 'none';
    }
    hideOverlay();
  }

  function showEls(ids) {
    for (var i = 0; i < ids.length; i++) {
      var e = document.getElementById(ids[i]);
      if (e) e.style.display = 'flex';
    }
  }

  function showUIForMode() {
    hideAllUI();
    switch (uiMode) {
      case UI_MAIN_MENU:
        showMainMenu(); break;
      case UI_PRACTISE_MENU:
        showEls(['tc-prev', 'tc-next', 'tc-select', 'tc-trackname', 'tc-backmain']); break;
      case UI_PRACTISE_PREVIEW:
        showEls(['tc-back', 'tc-start']); break;
      case UI_PRACTISE_RACE:
      case UI_SEASON_RACE:
      case UI_MP_RACE:
        showEls(['tc-menu', 'tc-lap', 'tc-hud-boost', 'tc-hud-damage']);
        if (isMobile) showEls(['tc-left', 'tc-right', 'tc-accel', 'tc-brake', 'tc-boost']);
        break;
      case UI_PRACTISE_RESULT:
        showEls(['tc-gameover-label', 'tc-gameover']); break;
      // Season overlays managed by showOverlay()
    }
  }

  // ══════════════════════════════════════════════════════════════
  //  PER-FRAME UPDATE
  // ══════════════════════════════════════════════════════════════

  function update() {
    if (!ready) { requestAnimationFrame(update); return; }

    var cppMode = getGameMode();

    // Track name in practise menu
    if (uiMode === UI_PRACTISE_MENU) {
      var label = document.getElementById('tc-trackname');
      if (label) label.textContent = getTrackName();
    }

    // Race-finished detection
    if ((uiMode === UI_PRACTISE_RACE || uiMode === UI_SEASON_RACE || uiMode === UI_MP_RACE) &&
        cppMode === GAME_IN_PROGRESS && isRaceFinished()) {
      if (raceEndTime === 0) raceEndTime = Date.now();

      var lbl = document.getElementById('tc-gameover-label');
      if (lbl) {
        if (uiMode === UI_SEASON_RACE || uiMode === UI_MP_RACE)
          lbl.textContent = isPlayerWrecked() ? 'WRECKED' : (isRaceWon() ? 'RACE WON' : 'RACE LOST');
        else
          lbl.textContent = isPlayerWrecked() ? 'WRECKED' : 'RACE COMPLETE';
        lbl.style.display = 'flex';
        lbl.style.opacity = (Math.floor(Date.now() / 500) % 2 === 0) ? '1' : '0.2';
      }

      if (Date.now() - raceEndTime > 6000) {
        raceEndTime = 0;
        if (uiMode === UI_MP_RACE) {
          setGameOver();
          finishMpRace();
        } else if (uiMode === UI_SEASON_RACE) {
          setGameOver();
          finishSeasonRace();
        } else {
          setGameOver();
          uiMode = UI_PRACTISE_RESULT;
          var rl = document.getElementById('tc-gameover-label');
          if (rl) { rl.textContent = isPlayerWrecked() ? 'WRECKED' : 'RACE COMPLETE'; rl.style.opacity = '1'; }
          showUIForMode();
        }
      }
    }

    // Lap counter
    if (uiMode === UI_PRACTISE_RACE || uiMode === UI_SEASON_RACE || uiMode === UI_MP_RACE) {
      var lapEl = document.getElementById('tc-lap');
      if (lapEl) {
        var lap = getLapNumber();
        if (lap < 1) lapEl.style.display = 'none';
        else { lapEl.style.display = 'flex'; lapEl.textContent = 'Lap ' + Math.min(lap, 3) + '/3'; }
      }
    }

    // HUD bars
    if (uiMode === UI_PRACTISE_RACE || uiMode === UI_SEASON_RACE || uiMode === UI_MP_RACE || uiMode === UI_PRACTISE_RESULT) {
      var bf = document.getElementById('tc-hud-boost-fill');
      if (bf) { var mx = getBoostMax(); bf.style.width = (mx > 0 ? Math.round(100 * getBoostReserve() / mx) : 0) + '%'; }
      var df = document.getElementById('tc-hud-damage-fill');
      if (df) df.style.width = Math.min(100, Math.round(100 * getDamage() / 255)) + '%';
    }

    // ── Multiplayer per-frame state exchange ──
    if (uiMode === UI_MP_RACE && mpConnected) {
      mpSendState();
    }

    requestAnimationFrame(update);
  }

  // ══════════════════════════════════════════════════════════════
  //  BOOTSTRAP
  // ══════════════════════════════════════════════════════════════

  function boot() {
    createUI();
    ready = true;
    uiMode = UI_MAIN_MENU;
    showUIForMode();
    requestAnimationFrame(update);
  }

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
