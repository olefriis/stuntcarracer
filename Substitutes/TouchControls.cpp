// UI overlay for all devices.
// Creates HTML elements over the WebGL canvas.
// Menu/preview buttons call exported C++ API functions directly.
// In-game driving buttons (mobile only) manipulate the lastInput bitmask
// so that BOOST works as a modifier (like the original Amiga fire button).

#include "../dxstdafx.h"
#include "../StuntCarRacer.h"
#include "../Car Behaviour.h"
#include "../Track.h"
#include "TouchControls.h"
#include <emscripten.h>
#include <wchar.h>

extern GameModeType GameMode;
extern UINT keyPress;
extern DWORD lastInput;
extern long TrackID;
extern bool raceFinished, raceWon;
extern long boostReserve, StandardBoost;
extern long new_damage;
extern long opponentsID;
extern long lapNumber[];

// Expose current game mode to JavaScript
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int getTouchGameMode() {
        return (int)GameMode;
    }

    // Direct drive input for touch controls.
    // Receives a bitmask of KEY_P1_* flags and applies it to lastInput,
    // replacing only the drive-related bits (steering + accel/brake/boost).
    EMSCRIPTEN_KEEPALIVE
    void touchSetDriveInput(int flags) {
        const DWORD DRIVE_MASK = KEY_P1_LEFT | KEY_P1_RIGHT | KEY_P1_HASH
                               | KEY_P1_BRAKE_BOOST | KEY_P1_ACCEL_BOOST
                               | KEY_P1_ACCEL_ONLY;
        lastInput = (lastInput & ~DRIVE_MASK) | (((DWORD)flags) & DRIVE_MASK);
    }
}

// ────────────────────────────────────────────────────────────────
// Create and manage overlay HTML elements
// ────────────────────────────────────────────────────────────────
EM_JS(void, js_initTouchControls, (), {
    if (window._touchControlsReady) return;

    var isMobile = ('ontouchstart' in window) || (navigator.maxTouchPoints > 0);
    window._isTouchDevice = isMobile;

    // ── Fade overlay ──
    var fade = document.createElement('div');
    fade.id = 'fadeOverlay';
    fade.style.cssText = 'position:fixed;left:0;top:0;width:100%;height:100%;background:#000;' +
        'opacity:0;pointer-events:none;z-index:200;transition:opacity 0.35s ease;';
    document.body.appendChild(fade);

    // ── Container ──
    var container = document.createElement('div');
    container.id = 'touchControls';
    container.style.cssText = 'position:fixed;left:0;top:0;width:100%;height:100%;' +
        'pointer-events:none;z-index:100;user-select:none;-webkit-user-select:none;';
    document.body.appendChild(container);

    // ── Helper: create a styled element ──
    function createEl(id, text, cssText) {
        var el = document.createElement('div');
        el.id = id;
        if (text) el.textContent = text;
        el.style.cssText = 'position:absolute;pointer-events:auto;display:none;' +
            'background:rgba(255,255,255,0.18);color:#fff;border:2px solid rgba(255,255,255,0.4);' +
            'border-radius:12px;font-family:Arial,sans-serif;font-weight:bold;' +
            'display:flex;align-items:center;justify-content:center;' +
            'touch-action:none;user-select:none;-webkit-user-select:none;' +
            'box-sizing:border-box;text-align:center;overflow:hidden;white-space:nowrap;' +
            'cursor:pointer;' + (cssText || '');
        el.style.display = 'none';
        container.appendChild(el);
        return el;
    }

    // ── Track Menu ──
    createEl('tc-prev', '\u25C0\uFE0E',
        'left:2vw;bottom:6vh;width:14vw;height:14vw;font-size:min(6vw,32px);max-width:75px;max-height:75px;');
    createEl('tc-next', '\u25B6\uFE0E',
        'left:18vw;bottom:6vh;width:14vw;height:14vw;font-size:min(6vw,32px);max-width:75px;max-height:75px;');
    createEl('tc-select', 'SELECT',
        'right:2vw;bottom:6vh;width:22vw;height:14vw;font-size:min(3.5vw,18px);max-width:130px;max-height:75px;');
    createEl('tc-trackname', '',
        'left:34vw;right:26vw;width:auto;bottom:6vh;height:14vw;max-height:75px;font-size:min(3.8vw,20px);' +
        'pointer-events:none;background:none;border:none;text-shadow:0 0 8px rgba(0,0,0,0.8);');

    // ── Track Preview ──
    createEl('tc-back', 'MENU',
        'left:2vw;bottom:6vh;width:22vw;height:12vw;font-size:min(4.5vw,22px);max-width:120px;max-height:70px;');
    createEl('tc-start', 'START',
        'right:2vw;bottom:6vh;width:22vw;height:12vw;font-size:min(4.5vw,22px);max-width:120px;max-height:70px;');
    createEl('tc-opponent', '',
        'left:26vw;right:26vw;width:auto;bottom:6vh;height:12vw;max-height:70px;font-size:min(3.5vw,18px);' +
        'pointer-events:none;background:none;border:none;text-shadow:0 0 8px rgba(0,0,0,0.8);');

    // ── In-Game driving controls (mobile only) ──
    createEl('tc-left', '\u25C0\uFE0E',
        'left:2vw;bottom:6vh;width:11vw;height:11vw;font-size:min(5vw,28px);max-width:65px;max-height:65px;');
    createEl('tc-right', '\u25B6\uFE0E',
        'left:15vw;bottom:6vh;width:11vw;height:11vw;font-size:min(5vw,28px);max-width:65px;max-height:65px;');
    createEl('tc-accel', '\u25B2\uFE0E',
        'right:2vw;bottom:30vh;width:11vw;height:11vw;font-size:min(5vw,28px);max-width:65px;max-height:65px;');
    createEl('tc-brake', '\u25BC\uFE0E',
        'right:2vw;bottom:6vh;width:11vw;height:11vw;font-size:min(5vw,28px);max-width:65px;max-height:65px;');
    createEl('tc-boost', 'BOOST',
        'left:50%;bottom:6vh;width:20vw;height:11vw;font-size:min(3.5vw,18px);max-width:110px;max-height:65px;transform:translateX(-50%);');

    // ── In-Game close/menu button (shown for everyone) ──
    createEl('tc-menu', '\u2715',
        'right:2vw;top:2vh;width:10vw;height:10vw;font-size:min(5vw,28px);max-width:55px;max-height:55px;');

    // ── Game Over ──
    createEl('tc-gameover-label', '',
        'left:50%;top:40%;width:50vw;height:auto;font-size:min(7vw,40px);max-width:300px;' +
        'pointer-events:none;background:none;border:none;text-shadow:0 0 12px rgba(0,0,0,0.9);' +
        'transform:translate(-50%,-50%);');
    createEl('tc-gameover', 'MENU',
        'left:50%;bottom:6vh;width:18vw;height:11vw;font-size:min(3.5vw,18px);max-width:100px;max-height:65px;transform:translateX(-50%);');

    // ── HUD bars (shown for everyone during gameplay) ──
    function createHudBar(id, icon, color) {
        var row = document.createElement('div');
        row.id = id;
        row.style.cssText = 'position:absolute;display:none;align-items:center;pointer-events:none;height:2.5vh;min-height:14px;';
        var iconEl = document.createElement('span');
        iconEl.textContent = icon;
        iconEl.style.cssText = 'font-size:min(6vh,30px);margin-right:1vw;line-height:1;';
        var track = document.createElement('div');
        track.style.cssText = 'flex:1;height:100%;background:rgba(0,0,0,0.4);border-radius:4px;overflow:hidden;';
        var fill = document.createElement('div');
        fill.id = id + '-fill';
        fill.style.cssText = 'height:100%;width:0%;background:' + color + ';border-radius:4px;transition:width 0.15s;';
        track.appendChild(fill);
        row.appendChild(iconEl);
        row.appendChild(track);
        container.appendChild(row);
    }
    createHudBar('tc-hud-boost', '\uD83D\uDD25', '#ff9900');
    document.getElementById('tc-hud-boost').style.cssText += 'left:2vw;right:50%;top:2vh;padding-right:1vw;';
    createHudBar('tc-hud-damage', '\u26A0\uFE0F', '#ff3333');
    document.getElementById('tc-hud-damage').style.cssText += 'left:50%;right:14vw;top:2vh;padding-left:1vw;';

    // ── Fade helper ──
    window._fadeAndDo = function(callback) {
        if (window._fading) return;
        window._fading = true;
        var f = document.getElementById('fadeOverlay');
        f.style.opacity = '1';
        setTimeout(function() {
            callback();
            setTimeout(function() { f.style.opacity = '0'; window._fading = false; }, 60);
        }, 350);
    };

    // ── Track index state ──
    window._touchTrackIndex = 0;

    // ── Button interaction helpers ──
    function addBtn(id, callback) {
        var el = document.getElementById(id);
        function handler(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.45)';
            callback();
            setTimeout(function() { el.style.background = 'rgba(255,255,255,0.18)'; }, 200);
        }
        el.addEventListener('touchstart', handler, {passive: false});
        el.addEventListener('mousedown', handler);
    }

    // ── Track navigation helpers ──
    function prevTrack() {
        window._touchTrackIndex--;
        if (window._touchTrackIndex < 0) window._touchTrackIndex = Module._jsGetNumTracks() - 1;
        Module._jsSelectTrack(window._touchTrackIndex);
    }
    function nextTrack() {
        window._touchTrackIndex++;
        if (window._touchTrackIndex >= Module._jsGetNumTracks()) window._touchTrackIndex = 0;
        Module._jsSelectTrack(window._touchTrackIndex);
    }

    // ── Track Menu buttons ──
    addBtn('tc-prev', prevTrack);
    addBtn('tc-next', nextTrack);
    addBtn('tc-select', function() {
        if (Module._jsGetTrackID() < 0) return;
        window._fadeAndDo(function() { Module._jsStartPreview(); });
    });

    // ── Track Preview buttons ──
    addBtn('tc-back', function() {
        window._fadeAndDo(function() { Module._jsGoToMenu(); });
    });
    addBtn('tc-start', function() {
        window._fadeAndDo(function() { Module._jsStartGame(-1); });
    });

    // ── In-Game drive controls (touch only) ──
    var KEY_LEFT        = 0x01;
    var KEY_RIGHT       = 0x02;
    var KEY_HASH        = 0x04;
    var KEY_BRAKE_BOOST = 0x08;
    var KEY_ACCEL_BOOST = 0x10;
    var KEY_ACCEL_ONLY  = 0x20;

    window._touchDrive = { left:false, right:false, gas:false, brake:false, boost:false };

    function updateDriveFlags() {
        var d = window._touchDrive;
        var flags = 0;
        if (d.left)  flags |= KEY_LEFT;
        if (d.right) flags |= KEY_RIGHT;
        if (d.gas && d.boost)       flags |= KEY_ACCEL_BOOST;
        else if (d.gas)             flags |= KEY_ACCEL_ONLY;
        if (d.brake && d.boost)     flags |= KEY_BRAKE_BOOST;
        else if (d.brake)           flags |= KEY_HASH;
        Module._touchSetDriveInput(flags);
    }

    function addDriveBtn(id, field) {
        var el = document.getElementById(id);
        el.addEventListener('touchstart', function(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.45)';
            window._touchDrive[field] = true;
            updateDriveFlags();
        }, {passive: false});
        el.addEventListener('touchend', function(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.18)';
            window._touchDrive[field] = false;
            updateDriveFlags();
        }, {passive: false});
        el.addEventListener('touchcancel', function(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.18)';
            window._touchDrive[field] = false;
            updateDriveFlags();
        }, {passive: false});
    }

    addDriveBtn('tc-left', 'left');
    addDriveBtn('tc-right', 'right');
    addDriveBtn('tc-accel', 'gas');
    addDriveBtn('tc-brake', 'brake');
    addDriveBtn('tc-boost', 'boost');

    // ── In-Game menu button ──
    addBtn('tc-menu', function() {
        window._fadeAndDo(function() { Module._jsGoToMenu(); });
    });

    // ── Game Over menu button ──
    addBtn('tc-gameover', function() {
        window._fadeAndDo(function() { Module._jsGoToMenu(); });
    });

    // ── Keyboard shortcuts for desktop ──
    document.addEventListener('keydown', function(e) {
        var mode = window._lastTouchGameMode;
        if (mode === 0) { // TRACK_MENU
            if (e.key === 'ArrowLeft')  { e.preventDefault(); prevTrack(); }
            else if (e.key === 'ArrowRight') { e.preventDefault(); nextTrack(); }
            else if (e.key === 'Enter' || e.key === ' ') { e.preventDefault(); if (Module._jsGetTrackID() >= 0) window._fadeAndDo(function() { Module._jsStartPreview(); }); }
        } else if (mode === 1) { // TRACK_PREVIEW
            if (e.key === 'Enter' || e.key === ' ') { e.preventDefault(); window._fadeAndDo(function() { Module._jsStartGame(-1); }); }
            else if (e.key === 'Backspace' || e.key === 'Escape') { e.preventDefault(); window._fadeAndDo(function() { Module._jsGoToMenu(); }); }
        } else if (mode === 2) { // GAME_IN_PROGRESS
            if (e.key === 'Backspace' || e.key === 'Escape') { e.preventDefault(); window._fadeAndDo(function() { Module._jsGoToMenu(); }); }
        } else if (mode === 3) { // GAME_OVER
            if (e.key === 'Enter' || e.key === ' ' || e.key === 'Backspace' || e.key === 'Escape') { e.preventDefault(); window._fadeAndDo(function() { Module._jsGoToMenu(); }); }
        }
    });

    window._touchControlsReady = true;
    window._lastTouchGameMode = -1;
});

// ────────────────────────────────────────────────────────────────
// Per-frame update — show/hide elements based on game mode
// ────────────────────────────────────────────────────────────────
EM_JS(void, js_updateTouchControls, (int gameMode, const char* trackNamePtr, int raceWonFlag,
                                      int boostVal, int boostMax, int damageVal,
                                      const char* opponentNamePtr, int raceFinishedFlag,
                                      int lapNum), {
    if (!window._touchControlsReady) return;

    var TRACK_MENU = 0, TRACK_PREVIEW = 1, GAME_IN_PROGRESS = 2, GAME_OVER = 3;
    var isMobile = window._isTouchDevice;

    if (gameMode !== window._lastTouchGameMode) {
        window._lastTouchGameMode = gameMode;

        var menuBtns     = ['tc-prev', 'tc-next', 'tc-select', 'tc-trackname'];
        var previewBtns  = ['tc-back', 'tc-start', 'tc-opponent'];
        var gameDrive    = ['tc-left', 'tc-right', 'tc-accel', 'tc-brake', 'tc-boost'];
        var gameCommon   = ['tc-menu', 'tc-hud-boost', 'tc-hud-damage'];
        var gameOverBtns = ['tc-gameover-label', 'tc-gameover'];

        var allBtns = menuBtns.concat(previewBtns, gameDrive, gameCommon, gameOverBtns);

        for (var i = 0; i < allBtns.length; i++) {
            var el = document.getElementById(allBtns[i]);
            if (el) el.style.display = 'none';
        }

        var show = [];
        if (gameMode === TRACK_MENU)          show = menuBtns;
        else if (gameMode === TRACK_PREVIEW)  show = previewBtns;
        else if (gameMode === GAME_IN_PROGRESS) {
            show = gameCommon.slice();
            if (isMobile) show = show.concat(gameDrive);
        }
        else if (gameMode === GAME_OVER)      show = gameOverBtns;

        for (var i = 0; i < show.length; i++) {
            var el = document.getElementById(show[i]);
            if (el) el.style.display = 'flex';
        }

        if (gameMode === GAME_OVER) {
            var lbl = document.getElementById('tc-gameover-label');
            if (lbl) lbl.textContent = raceWonFlag ? 'YOU WON' : 'YOU LOST';
        }
    }

    // Track name (track menu)
    if (gameMode === TRACK_MENU) {
        var label = document.getElementById('tc-trackname');
        if (label) {
            label.textContent = trackNamePtr ? UTF8ToString(trackNamePtr) : '';
        }
    }

    // Opponent name (preview)
    if (gameMode === TRACK_PREVIEW) {
        var oppLabel = document.getElementById('tc-opponent');
        if (oppLabel) {
            oppLabel.textContent = opponentNamePtr ? 'vs ' + UTF8ToString(opponentNamePtr) : '';
        }
    }

    // Flashing result label while race is finishing (before GAME_OVER)
    if (gameMode === GAME_IN_PROGRESS && raceFinishedFlag) {
        var lbl = document.getElementById('tc-gameover-label');
        if (lbl) {
            lbl.textContent = raceWonFlag ? 'RACE WON' : 'RACE LOST';
            lbl.style.display = 'flex';
            var flash = (Math.floor(Date.now() / 500) % 2 === 0);
            lbl.style.opacity = flash ? '1' : '0.2';
        }
    }

    // HUD bars (in-game + game over)
    if (gameMode === GAME_IN_PROGRESS || gameMode === GAME_OVER) {
        var boostFill = document.getElementById('tc-hud-boost-fill');
        if (boostFill) {
            var pct = boostMax > 0 ? Math.round(100 * boostVal / boostMax) : 0;
            boostFill.style.width = pct + '%';
        }
        var dmgFill = document.getElementById('tc-hud-damage-fill');
        if (dmgFill) {
            var pct = Math.min(100, Math.round(100 * damageVal / 255));
            dmgFill.style.width = pct + '%';
        }
    }
});

// ────────────────────────────────────────────────────────────────
// C functions for JS to inject key events (keyboard still works)
// ────────────────────────────────────────────────────────────────
extern void CALLBACK KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void *pUserContext);

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void touchKeyDown(int keyCode) {
        KeyboardProc((UINT)keyCode, true, false, NULL);
    }

    EMSCRIPTEN_KEEPALIVE
    void touchKeyUp(int keyCode) {
        KeyboardProc((UINT)keyCode, false, false, NULL);
    }
}

// ────────────────────────────────────────────────────────────────
// Public interface
// ────────────────────────────────────────────────────────────────
void initTouchControls() {
    js_initTouchControls();
}

extern WCHAR *opponentNames[];
#define NUM_OPPONENTS 11

void updateTouchControls() {
    // Track name
    static char trackNameBuf[128];
    const char* trackName = NULL;
    if (GameMode == TRACK_MENU) {
        if (TrackID == NO_TRACK) {
            trackName = "None";
        } else {
            WCHAR* name = GetTrackName(TrackID);
            wcstombs(trackNameBuf, name, sizeof(trackNameBuf) - 1);
            trackNameBuf[sizeof(trackNameBuf) - 1] = '\0';
            trackName = trackNameBuf;
        }
    }

    // Opponent name
    static char opponentNameBuf[128];
    const char* opponentName = NULL;
    if (opponentsID >= 0 && opponentsID < NUM_OPPONENTS) {
        wcstombs(opponentNameBuf, opponentNames[opponentsID], sizeof(opponentNameBuf) - 1);
        opponentNameBuf[sizeof(opponentNameBuf) - 1] = '\0';
        opponentName = opponentNameBuf;
    }

    js_updateTouchControls((int)GameMode, trackName, raceWon ? 1 : 0,
                            (int)boostReserve, (int)StandardBoost, (int)new_damage,
                            opponentName, raceFinished ? 1 : 0,
                            (int)lapNumber[PLAYER]);
}
