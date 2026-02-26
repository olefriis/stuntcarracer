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
 *              →  (Two Players) WebRTC multiplayer via signaling server
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
  var signalingUrl = 'https://stuntcarracer.fly.dev';
  var mpConnected = false;
  var mpTrackIndex = 0;
  var mpOpponentFinished = false;
  var mpOpponentWrecked = false;
  var mpPlayerFinishedFirst = false;
  var mpPlayerNotified = false;

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
  function getDisplaySpeed()   { return Module._jsGetDisplaySpeed(); }
  function getCurrentLapTime() { return Module._jsGetCurrentLapTime(); }
  function getDistanceToOpponent() { return Module._jsGetDistanceToOpponent(); }

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
    var element = document.getElementById('fadeOverlay');
    element.style.opacity = '1';
    setTimeout(function () {
      callback();
      setTimeout(function () {
        element.style.opacity = '0';
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

    // Fade overlay (styled via #fadeOverlay in game.css)
    var fade = document.createElement('div');
    fade.id = 'fadeOverlay';
    document.body.appendChild(fade);

    // Container for all game UI (styled via #gameUI in game.css)
    var container = document.createElement('div');
    container.id = 'gameUI';
    document.body.appendChild(container);

    // Helper: create a game button with the .game-button base class.
    // All positioning is handled by ID selectors in game.css.
    function element(id, text) {
      var d = document.createElement('div');
      d.id = id;
      d.className = 'game-button';
      if (text) d.textContent = text;
      container.appendChild(d);
      return d;
    }

    // ── Main Menu ──
    element('mm-title', 'STUNT CAR RACER');
    element('mm-practise', 'Practise');
    element('mm-season', 'Start the Racing Season');

    // ── Track Menu (practise) ──
    element('tc-prev', '\u25C0\uFE0E');
    element('tc-next', '\u25B6\uFE0E');
    element('tc-select', 'Select');
    element('tc-trackname', '');
    element('tc-backmain', 'Menu');

    // ── Track Preview (practise) ──
    element('tc-back', 'Back');
    element('tc-start', 'Start');

    // ── In-Game driving controls (mobile only) ──
    element('tc-left', '\u25C0\uFE0E');
    element('tc-right', '\u25B6\uFE0E');
    element('tc-accel', '\u25B2\uFE0E');
    element('tc-brake', '\u25BC\uFE0E');
    element('tc-boost', '\u00A0\uD83D\uDD25\u00A0');

    // ── In-Game common ──
    element('tc-menu', '\u2715');

    // ── Game Over / result label ──
    element('tc-gameover-label', '');
    element('tc-gameover', 'Menu');

    // ── HUD: damage bar at top ──
    createHudBar('tc-hud-damage', '\u26A0\uFE0F');

    // ── HUD: info box (left side) ──
    createHudBox();

    // ── Season overlay (styled via #season-overlay / #season-card in game.css) ──
    var overlay = document.createElement('div');
    overlay.id = 'season-overlay';
    var card = document.createElement('div');
    card.id = 'season-card';
    overlay.appendChild(card);
    document.body.appendChild(overlay);

    wireButtons();
    wireKeyboard();
  }

  function createHudBar(id, icon) {
    var container = document.getElementById('gameUI');
    var row = document.createElement('div');
    row.id = id;
    row.className = 'hud-bar';
    var iconEl = document.createElement('span');
    iconEl.textContent = icon;
    iconEl.className = 'hud-icon';
    var track = document.createElement('div');
    track.className = 'hud-track';
    var fill = document.createElement('div');
    fill.id = id + '-fill';
    fill.className = 'hud-fill';
    track.appendChild(fill);
    row.appendChild(iconEl);
    row.appendChild(track);
    container.appendChild(row);
  }

  function createHudBox() {
    var container = document.getElementById('gameUI');
    var box = document.createElement('div');
    box.id = 'tc-hud-box';
    if (isMobile) box.classList.add('hud-box-mobile');

    // Vertical speed bar
    var track = document.createElement('div');
    track.className = 'hud-speed-track';
    var fill = document.createElement('div');
    fill.id = 'hud-speed-fill';
    fill.className = 'hud-speed-fill';
    track.appendChild(fill);
    box.appendChild(track);

    // Text column
    var text = document.createElement('div');
    text.className = 'hud-text';
    var ids = ['hud-lap', 'hud-boost', 'hud-blank', 'hud-distance', 'hud-laptime', 'hud-bestlap'];
    for (var i = 0; i < ids.length; i++) {
      var row = document.createElement('div');
      row.className = 'hud-row';
      row.id = ids[i];
      row.textContent = '\u00A0'; // non-breaking space to reserve height
      text.appendChild(row);
    }
    text.appendChild(document.createElement('div')); // spacer
    box.appendChild(text);

    container.appendChild(box);
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

  function overlayBtn(id, label, handler) {
    var element = document.getElementById(id);
    if (!element) return;
    element.addEventListener('click', function () { handler(); });
    element.addEventListener('touchstart', function (e) { e.preventDefault(); handler(); }, { passive: false });
  }

  // ── Season overview screen ──
  function showSeasonOverview() {
    uiMode = UI_SEASON_OVERVIEW;
    var h = '<div class="overlay-title">Season Overview</div>';
    h += '<div class="season-grid">';
    // Show divisions from highest (Div 1 = index 3) to lowest (Div 4 = index 0)
    for (var di = 3; di >= 0; di--) {
      var players = season.divisions[di];
      var tracks = DIVISION_TRACKS[di];
      var isHumanDiv = (di === season.humanDiv);
      h += '<div class="season-card' + (isHumanDiv ? ' season-card-active' : '') + '">';
      h += '<div class="season-label' + (isHumanDiv ? ' color-yellow' : '') + '">' + divLabel(di) + '</div>';
      // Players
      for (var p = 0; p < players.length; p++) {
        var pid = players[p];
        var isH = (pid === HUMAN_PLAYER);
        h += '<div class="season-player' + (isH ? ' season-player-human' : '') + '">' +
          driverName(pid) + '</div>';
      }
      // Spacer
      h += '<div class="season-spacer"></div>';
      // Tracks
      h += '<div class="season-tracks">';
      for (var t = 0; t < tracks.length; t++) {
        h += '<div class="season-track-name">' + TRACK_NAMES[tracks[t]] + '</div>';
      }
      h += '</div></div>';
    }
    h += '</div>';
    h += '<div id="s-btn-go" class="overlay-button" style="margin-top:2vh;">Continue</div>';
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
    var h = '<div class="overlay-subtitle">' +
      divLabel(season.humanDiv) + ' \u2014 Race ' + (season.currentRace + 1) + ' of 6</div>';
    h += '<div class="overlay-track">' + TRACK_NAMES[race.trackIndex] + '</div>';
    h += '<div class="overlay-matchup">' +
      driverName(race.driverA) + ' <span class="overlay-matchup-vs">vs</span> ' +
      driverName(race.driverB) + '</div>';
    h += '<div id="s-btn-race" class="overlay-button">Race</div>';
    h += '<div id="s-btn-quit" class="overlay-button overlay-button-secondary">Quit Season</div>';
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
    var h = '<div class="overlay-subtitle">' +
      'Race Result \u2014 ' + TRACK_NAMES[race.trackIndex] + '</div>';
    h += '<div class="overlay-result">' +
      driverName(race.driverA) + ' vs ' + driverName(race.driverB) + '</div>';
    h += '<div class="overlay-winner">' +
      '\uD83C\uDFC6 Winner: <b>' + driverName(race.winnerDriver) + '</b> (+2 pts)</div>';
    h += '<div class="overlay-fastest">' +
      '\u23F1\uFE0F Fastest Lap: <b>' + driverName(race.bestLapDriver) + '</b> (+1 pt)</div>';
    if (race.playerBestLapMs > 0) {
      h += '<div class="overlay-detail">Your best lap: ' + fmtLap(race.playerBestLapMs) + '</div>';
    }
    h += '<div id="s-btn-cont" class="overlay-button">Continue</div>';
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

    var h = '<div class="overlay-title">' + divLabel(di) + ' Standings</div>';
    h += '<table class="standings-table">';
    h += '<tr class="standings-header"><td>#</td>' +
      '<td>Driver</td>' +
      '<td class="standings-center">W</td>' +
      '<td class="standings-center">FL</td>' +
      '<td class="standings-center">Pts</td></tr>';

    for (var i = 0; i < st.length; i++) {
      var d = st[i], p = season.points[d], pts = p.wins * 2 + p.bestLaps;
      var isH = (d === HUMAN_PLAYER);
      var badge = '';
      if (i === 0 && di < 3)  badge = ' \u2B06\uFE0F';
      if (i === 0 && di === 3) badge = ' \uD83C\uDFC6';
      if (i === st.length - 1 && di > 0) badge = ' \u2B07\uFE0F';
      h += '<tr' + (isH ? ' class="color-yellow"' : '') + '>' +
        '<td>' + (i + 1) + '</td>' +
        '<td>' + driverName(d) + badge + '</td>' +
        '<td class="standings-center">' + p.wins + '</td>' +
        '<td class="standings-center">' + p.bestLaps + '</td>' +
        '<td class="standings-center standings-bold">' + pts + '</td></tr>';
    }
    h += '</table>';

    var top = st[0], bot = st[st.length - 1];
    if (top === HUMAN_PLAYER) {
      if (di === 3) h += '<div class="overlay-result color-gold">' +
        '\uD83C\uDFC6 SUPER LEAGUE CHAMPION! \uD83C\uDFC6</div>';
      else h += '<div class="overlay-result-small color-green">' +
        '\u2B06\uFE0F Promoted to ' + divLabel(di + 1) + '!</div>';
    } else if (bot === HUMAN_PLAYER) {
      if (di === 0) h += '<div class="overlay-result-small color-orange">' +
        'Bottom of the league \u2014 try again!</div>';
      else h += '<div class="overlay-result-small color-red">' +
        '\u2B07\uFE0F Relegated to ' + divLabel(di - 1) + '</div>';
    } else {
      h += '<div class="overlay-info">' +
        'Staying in ' + divLabel(di) + '</div>';
    }

    h += '<div id="s-btn-next" class="overlay-button">Finish Season</div>';
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
        mpOpponentWrecked = !!msg.wrecked;
        // If we haven't notified yet, opponent finished first — we did NOT finish first
        if (!mpPlayerNotified) mpPlayerFinishedFirst = false;
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
    mpOpponentWrecked = false;
    mpPlayerFinishedFirst = false;
    mpPlayerNotified = false;
    setTwoPlayerMode(false);
  }

  // ── Multiplayer UI screens ─────────────────────────────────

  function showMpRoleSelect() {
    uiMode = UI_MP_ROLE_SELECT;
    var h = '<div class="overlay-title">Two Players</div>';
    h += '<div class="overlay-description">Connect via WebRTC peer-to-peer</div>';
    h += '<div class="overlay-label">Signaling server</div>';
    h += '<input id="mp-sig-url" type="text" class="multiplayer-signaling-input" value="' +
      signalingUrl.replace(/"/g, '&quot;') + '" />';
    h += '<div id="mp-btn-host" class="overlay-button">Host Game</div><br>';
    h += '<div id="mp-btn-join" class="overlay-button">Join Game</div><br>';
    h += '<div id="mp-btn-back" class="overlay-button overlay-button-secondary">Back</div>';
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
    var h = '<div class="overlay-title">Hosting Game</div>';
    h += '<div class="overlay-description">Connecting to signaling server\u2026</div>';
    h += '<div id="mp-host-code" class="multiplayer-code-display"></div>';
    h += '<div id="mp-host-status" class="multiplayer-status">Setting up\u2026</div>';
    h += '<div id="mp-btn-cancel" class="overlay-button overlay-button-secondary">Cancel</div>';
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
    // Show track selection UI
    var h = '<div class="overlay-title">Select Track</div>';
    h += '<div class="overlay-subtitle" style="margin-bottom:2vh;">Opponent connected!</div>';
    h += '<div id="mp-track-name" class="overlay-result">' + TRACK_NAMES[mpTrackIndex] + '</div>';
    h += '<div style="display:flex;justify-content:center;gap:2vw;">';
    h += '<div id="mp-btn-prev" class="overlay-button">\u25C0\uFE0E</div>';
    h += '<div id="mp-btn-next" class="overlay-button">\u25B6\uFE0E</div>';
    h += '</div>';
    h += '<div id="mp-btn-go" class="overlay-button" style="margin-top:2vh;">Start Race</div>';
    h += '<div id="mp-btn-cancel2" class="overlay-button overlay-button-secondary">Cancel</div>';
    showOverlay(h);
    overlayBtn('mp-btn-prev', 'PREV', function () {
      mpTrackIndex--;
      if (mpTrackIndex < 0) mpTrackIndex = getNumTracks() - 1;
      selectTrack(mpTrackIndex);
      var element = document.getElementById('mp-track-name');
      if (element) element.textContent = TRACK_NAMES[mpTrackIndex];
    });
    overlayBtn('mp-btn-next', 'NEXT', function () {
      mpTrackIndex++;
      if (mpTrackIndex >= getNumTracks()) mpTrackIndex = 0;
      selectTrack(mpTrackIndex);
      var element = document.getElementById('mp-track-name');
      if (element) element.textContent = TRACK_NAMES[mpTrackIndex];
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
        mpOpponentWrecked = false;
        mpPlayerFinishedFirst = false;
        mpPlayerNotified = false;
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
    var h = '<div class="overlay-title">Join Game</div>';
    h += '<div class="overlay-description">Enter the 4-letter code from the host</div>';
    h += '<input id="mp-code-input" type="text" maxlength="4" autocapitalize="characters" class="multiplayer-code-input" />';
    h += '<div id="mp-join-status" class="multiplayer-status"></div>';
    h += '<div id="mp-btn-connect" class="overlay-button">Connect</div>';
    h += '<div id="mp-btn-jback" class="overlay-button overlay-button-secondary">Back</div>';
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
    // 'finished' message was already sent during race-end detection
    var wrecked = isPlayerWrecked();
    // Determine result: won if we finished first and weren't wrecked,
    // OR if opponent was wrecked and we weren't
    var won = false;
    if (!wrecked) {
      won = mpPlayerFinishedFirst || mpOpponentWrecked;
    }
    uiMode = UI_MP_RESULT;
    var h = '<div class="overlay-title">Race Complete</div>';
    if (wrecked && mpOpponentWrecked) {
      h += '<div class="overlay-result-large color-orange">BOTH WRECKED</div>';
    } else if (wrecked) {
      h += '<div class="overlay-result-large color-red">WRECKED</div>';
    } else if (won) {
      h += '<div class="overlay-result-large color-green">\uD83C\uDFC6 YOU WIN!</div>';
    } else {
      h += '<div class="overlay-result-large color-orange">YOU LOSE</div>';
    }
    var pBest = getPlayerBestLap();
    if (pBest > 0) {
      h += '<div class="overlay-info">Your best lap: ' + fmtLap(pBest) + '</div>';
    }
    h += '<div id="mp-btn-again" class="overlay-button">Play Again</div>';
    h += '<div id="mp-btn-quit" class="overlay-button overlay-button-secondary">Quit</div>';
    showOverlay(h);
    overlayBtn('mp-btn-again', 'AGAIN', function () {
      hideOverlay();
      mpOpponentFinished = false;
      mpOpponentWrecked = false;
      mpPlayerFinishedFirst = false;
      mpPlayerNotified = false;
      goToMenu();  // reset C++ state fully between races
      if (SCR_Multiplayer.isHost()) {
        showMpHostTrack();
      } else {
        // Show waiting screen
        uiMode = UI_MP_JOIN_LOBBY;
        var h2 = '<div class="overlay-title">Waiting</div>';
        h2 += '<div class="overlay-description">Waiting for host to select next track\u2026</div>';
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
    var h = '<div class="overlay-title-large">STUNT CAR RACER</div>';
    h += '<div class="overlay-subtitle" style="margin-bottom:3vh;">' + divLabel(humanDivision) + '</div>';
    h += '<div id="mm-btn-practise" class="overlay-button">Practise</div><br>';
    h += '<div id="mm-btn-season" class="overlay-button">Start the Racing Season</div><br>';
    h += '<div id="mm-btn-twoplayer" class="overlay-button">Two Players</div>';
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
    'tc-menu', 'tc-hud-damage', 'tc-hud-box',
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
        showEls(['tc-menu', 'tc-hud-damage', 'tc-hud-box']);
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

      // ── Multiplayer: notify opponent and determine winner ──
      if (uiMode === UI_MP_RACE && !mpPlayerNotified) {
        mpPlayerNotified = true;
        // If opponent hasn't notified us yet, we finished first
        mpPlayerFinishedFirst = !mpOpponentFinished;
        if (SCR_Multiplayer.isConnected()) {
          SCR_Multiplayer.sendReliable({ type: 'finished', wrecked: isPlayerWrecked() });
        }
      }

      if (raceEndTime === 0) raceEndTime = Date.now();

      var lbl = document.getElementById('tc-gameover-label');
      if (lbl) {
        if (uiMode === UI_MP_RACE) {
          if (isPlayerWrecked()) {
            lbl.textContent = mpOpponentFinished ? 'WRECKED' : 'WRECKED \u2013 WAITING\u2026';
          } else {
            lbl.textContent = mpPlayerFinishedFirst ? 'RACE WON' : 'RACE LOST';
          }
        } else if (uiMode === UI_SEASON_RACE) {
          lbl.textContent = isPlayerWrecked() ? 'WRECKED' : (isRaceWon() ? 'RACE WON' : 'RACE LOST');
        } else {
          lbl.textContent = isPlayerWrecked() ? 'WRECKED' : 'RACE COMPLETE';
        }
        lbl.style.display = 'flex';
        lbl.style.opacity = (Math.floor(Date.now() / 500) % 2 === 0) ? '1' : '0.2';
      }

      // ── Determine when to exit the race ──
      // For MP: wait until both players have finished (or 30s safety timeout)
      var canExit = true;
      if (uiMode === UI_MP_RACE) {
        var elapsed = Date.now() - raceEndTime;
        canExit = (mpOpponentFinished && elapsed > 3000) || elapsed > 30000;
      }

      if (canExit && Date.now() - raceEndTime > 6000) {
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

    // HUD updates
    if (uiMode === UI_PRACTISE_RACE || uiMode === UI_SEASON_RACE || uiMode === UI_MP_RACE || uiMode === UI_PRACTISE_RESULT) {
      // Damage bar (top)
      var df = document.getElementById('tc-hud-damage-fill');
      if (df) df.style.width = Math.min(100, Math.round(100 * getDamage() / 255)) + '%';

      // Vertical speed bar
      var sf = document.getElementById('hud-speed-fill');
      if (sf) sf.style.height = Math.min(100, Math.round(100 * getDisplaySpeed() / 240)) + '%';

      // Lap row
      var lap = getLapNumber();
      var lapEl = document.getElementById('hud-lap');
      if (lapEl) lapEl.textContent = lap >= 1 ? ('Lap ' + Math.min(lap, 3) + '/3') : 'Lap -/3';

      // Boost row
      var bEl = document.getElementById('hud-boost');
      if (bEl) bEl.textContent = 'Boost ' + getBoostReserve();

      // Distance to opponent
      var distEl = document.getElementById('hud-distance');
      if (distEl) {
        if (uiMode === UI_PRACTISE_RACE || uiMode === UI_PRACTISE_RESULT) {
          distEl.textContent = '\u00A0';
        } else {
          var rawDist = getDistanceToOpponent();
          var sign = rawDist < 0 ? '-' : '';
          var absDist = Math.abs(rawDist);
          var digits = absDist > 9999 ? '9999' : ('0000' + absDist).slice(-4);
          distEl.textContent = sign + digits;
        }
      }

      // Current lap time
      var ltEl = document.getElementById('hud-laptime');
      if (ltEl) {
        var curMs = getCurrentLapTime();
        ltEl.textContent = (lap >= 1 && curMs > 0) ? fmtLap(curMs) : '\u00A0';
      }

      // Best lap time
      var blEl = document.getElementById('hud-bestlap');
      if (blEl) {
        var bestMs = getPlayerBestLap();
        blEl.textContent = bestMs > 0 ? (fmtLap(bestMs) + ' \u2605') : '\u00A0';
      }
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
