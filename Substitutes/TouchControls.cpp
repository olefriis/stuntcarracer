// Touch controls overlay for mobile devices.
// Creates HTML button elements over the WebGL canvas.
// Menu/preview buttons inject keyboard events via KeyboardProc.
// In-game driving buttons directly manipulate the lastInput bitmask
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
extern bool raceWon;

// Expose current game mode to JavaScript
EM_JS(int, js_getGameMode, (), {
    return Module._getTouchGameMode();
});

// C functions callable from JS
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
        // Mask covers all drive bits: LEFT|RIGHT|HASH|BRAKE_BOOST|ACCEL_BOOST|ACCEL_ONLY
        const DWORD DRIVE_MASK = KEY_P1_LEFT | KEY_P1_RIGHT | KEY_P1_HASH
                               | KEY_P1_BRAKE_BOOST | KEY_P1_ACCEL_BOOST
                               | KEY_P1_ACCEL_ONLY;
        lastInput = (lastInput & ~DRIVE_MASK) | (((DWORD)flags) & DRIVE_MASK);
    }
}

// Create and manage touch control HTML elements
EM_JS(void, js_initTouchControls, (), {
    if (window._touchControlsReady) return;

    // Only show on touch devices
    var isTouchDevice = ('ontouchstart' in window) || (navigator.maxTouchPoints > 0);
    window._isTouchDevice = isTouchDevice;
    if (!isTouchDevice) {
        window._touchControlsReady = true;
        return;
    }

    // Create the touch controls container
    var container = document.createElement('div');
    container.id = 'touchControls';
    container.style.cssText = 'position:fixed;left:0;top:0;width:100%;height:100%;pointer-events:none;z-index:100;user-select:none;-webkit-user-select:none;';
    document.body.appendChild(container);

    // Helper to create a touch button
    function createButton(id, text, cssText) {
        var btn = document.createElement('div');
        btn.id = id;
        btn.textContent = text;
        btn.style.cssText = 'position:absolute;pointer-events:auto;display:none;' +
            'background:rgba(255,255,255,0.25);color:#fff;border:2px solid rgba(255,255,255,0.5);' +
            'border-radius:12px;font-family:Arial,sans-serif;font-weight:bold;' +
            'display:flex;align-items:center;justify-content:center;' +
            'touch-action:none;user-select:none;-webkit-user-select:none;' +
            'box-sizing:border-box;text-align:center;overflow:hidden;white-space:nowrap;' + cssText;
        // Use flexbox centering
        btn.style.display = 'none';
        container.appendChild(btn);
        return btn;
    }

    // --- Track Menu buttons ---
    createButton('tc-prev', '\u25C0\uFE0E', 'left:2vw;bottom:6vh;width:15vw;height:15vw;font-size:7vw;max-width:80px;max-height:80px;');
    createButton('tc-next', '\u25B6\uFE0E', 'left:19vw;bottom:6vh;width:15vw;height:15vw;font-size:7vw;max-width:80px;max-height:80px;');
    createButton('tc-select', 'SELECT', 'right:2vw;bottom:6vh;width:25vw;height:15vw;font-size:3.5vw;max-width:140px;max-height:80px;');
    // Track name label (centred between arrows and select button)
    createButton('tc-trackname', '', 'left:36vw;right:29vw;width:auto;bottom:6vh;height:15vw;max-height:80px;font-size:4vw;pointer-events:none;background:none;border:none;text-shadow:0 0 8px rgba(0,0,0,0.8);');
    // --- Track Preview buttons ---
    createButton('tc-back', 'BACK', 'left:2vw;bottom:6vh;width:22vw;height:12vw;font-size:4.5vw;max-width:120px;max-height:70px;');
    createButton('tc-start', 'START', 'right:2vw;bottom:6vh;width:22vw;height:12vw;font-size:4.5vw;max-width:120px;max-height:70px;');

    // --- In-Game controls ---
    // Left side: steering
    createButton('tc-left', '\u25C0\uFE0E', 'left:2vw;bottom:6vh;width:13vw;height:13vw;font-size:6vw;max-width:75px;max-height:75px;');
    createButton('tc-right', '\u25B6\uFE0E', 'left:17vw;bottom:6vh;width:13vw;height:13vw;font-size:6vw;max-width:75px;max-height:75px;');
    // Right side: accel on top, brake below (gap accounts for 13vw height in landscape)
    createButton('tc-accel', '\u25B2\uFE0E', 'right:2vw;bottom:36vh;width:13vw;height:13vw;font-size:6vw;max-width:75px;max-height:75px;');
    createButton('tc-brake', '\u25BC\uFE0E', 'right:2vw;bottom:6vh;width:13vw;height:13vw;font-size:6vw;max-width:75px;max-height:75px;');
    // Centre: BOOST
    createButton('tc-boost', 'BOOST', 'left:50%;bottom:6vh;width:22vw;height:13vw;font-size:4vw;max-width:120px;max-height:75px;transform:translateX(-50%);');
    // Menu close button
    createButton('tc-menu', '\u2715', 'right:2vw;top:2vh;width:10vw;height:10vw;font-size:5vw;max-width:55px;max-height:55px;');

    // --- Game Over ---
    // "GAME OVER" label (non-interactive, centred)
    createButton('tc-gameover-label', 'GAME OVER', 'left:50%;top:40%;width:50vw;height:auto;font-size:7vw;max-width:300px;pointer-events:none;background:none;border:none;text-shadow:0 0 12px rgba(0,0,0,0.9);transform:translate(-50%,-50%);');
    // MENU button (same position as BOOST button)
    createButton('tc-gameover', 'MENU', 'left:50%;bottom:6vh;width:22vw;height:13vw;font-size:4vw;max-width:120px;max-height:75px;transform:translateX(-50%);');

    // Track the current track index for cycling through tracks in menu
    window._touchTrackIndex = 0;

    // --- Key simulation helpers (for menu/preview buttons only) ---
    function simulateKeyPress(keyCode) {
        Module._touchKeyDown(keyCode);
        setTimeout(function() { Module._touchKeyUp(keyCode); }, 100);
    }

    function addTapButton(id, callback) {
        var el = document.getElementById(id);
        el.addEventListener('touchstart', function(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.5)';
            callback();
            setTimeout(function() {
                el.style.background = 'rgba(255,255,255,0.25)';
            }, 200);
        }, {passive: false});
    }

    // --- Track Menu ---
    addTapButton('tc-prev', function() {
        window._touchTrackIndex--;
        if (window._touchTrackIndex < 0) window._touchTrackIndex = 7;
        simulateKeyPress(49 + window._touchTrackIndex);
    });
    addTapButton('tc-next', function() {
        window._touchTrackIndex++;
        if (window._touchTrackIndex > 7) window._touchTrackIndex = 0;
        simulateKeyPress(49 + window._touchTrackIndex);
    });
    addTapButton('tc-select', function() {
        simulateKeyPress(83); // 'S'
    });

    // --- Track Preview ---
    addTapButton('tc-back', function() {
        simulateKeyPress(77); // 'M'
    });
    addTapButton('tc-start', function() {
        simulateKeyPress(83); // 'S'
    });

    // --- In-Game drive controls ---
    // These directly set lastInput flags via touchSetDriveInput() so that
    // BOOST works as a proper modifier (like the original Amiga fire button):
    //   GAS alone        = accelerate (no boost)
    //   GAS + BOOST held = accelerate WITH boost
    //   BRK alone        = brake/reverse (no boost)
    //   BRK + BOOST held = brake/reverse WITH boost
    //
    // Input flag constants (must match Car Behaviour.h):
    var KEY_LEFT        = 0x01;
    var KEY_RIGHT       = 0x02;
    var KEY_HASH        = 0x04;  // brake only
    var KEY_BRAKE_BOOST = 0x08;  // brake + boost
    var KEY_ACCEL_BOOST = 0x10;  // accel + boost
    var KEY_ACCEL_ONLY  = 0x20;  // accel without boost

    // Track which drive buttons are currently held
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

    function addDriveButton(id, field) {
        var el = document.getElementById(id);
        el.addEventListener('touchstart', function(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.5)';
            window._touchDrive[field] = true;
            updateDriveFlags();
        }, {passive: false});
        el.addEventListener('touchend', function(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.25)';
            window._touchDrive[field] = false;
            updateDriveFlags();
        }, {passive: false});
        el.addEventListener('touchcancel', function(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.25)';
            window._touchDrive[field] = false;
            updateDriveFlags();
        }, {passive: false});
    }

    addDriveButton('tc-left', 'left');
    addDriveButton('tc-right', 'right');
    addDriveButton('tc-accel', 'gas');
    addDriveButton('tc-brake', 'brake');
    addDriveButton('tc-boost', 'boost');

    // Menu button during game: press 'M'
    addTapButton('tc-menu', function() {
        simulateKeyPress(77);
    });

    // --- Game Over ---
    addTapButton('tc-gameover', function() {
        simulateKeyPress(77);
    });

    window._touchControlsReady = true;
    window._lastTouchGameMode = -1;
});

// Update which buttons are visible based on game mode
EM_JS(void, js_updateTouchControls, (int gameMode, const char* trackNamePtr, int raceWonFlag), {
    if (!window._isTouchDevice || !window._touchControlsReady) return;

    if (gameMode !== window._lastTouchGameMode) {
        window._lastTouchGameMode = gameMode;

    // Game mode constants (must match GameModeType enum)
    var TRACK_MENU = 0;
    var TRACK_PREVIEW = 1;
    var GAME_IN_PROGRESS = 2;
    var GAME_OVER = 3;

    // All button ids grouped by mode
    var menuBtns = ['tc-prev', 'tc-next', 'tc-select', 'tc-trackname'];
    var previewBtns = ['tc-back', 'tc-start'];
    var gameBtns = ['tc-left', 'tc-right', 'tc-accel', 'tc-brake', 'tc-boost', 'tc-menu'];
    var gameOverBtns = ['tc-gameover-label', 'tc-gameover'];

    var allBtns = menuBtns.concat(previewBtns, gameBtns, gameOverBtns);

    // Hide all
    for (var i = 0; i < allBtns.length; i++) {
        var el = document.getElementById(allBtns[i]);
        if (el) el.style.display = 'none';
    }

    // Show buttons for current mode
    var show = [];
    if (gameMode === TRACK_MENU) show = menuBtns;
    else if (gameMode === TRACK_PREVIEW) show = previewBtns;
    else if (gameMode === GAME_IN_PROGRESS) show = gameBtns;
    else if (gameMode === GAME_OVER) show = gameOverBtns;

    for (var i = 0; i < show.length; i++) {
        var el = document.getElementById(show[i]);
        if (el) el.style.display = 'flex';
    }

    // Set game-over label text based on outcome
    if (gameMode === GAME_OVER) {
        var lbl = document.getElementById('tc-gameover-label');
        if (lbl) lbl.textContent = raceWonFlag ? 'YOU WON' : 'YOU LOST';
    }

    } // end if (gameMode !== _lastTouchGameMode)

    // Update the track name label every frame when on the track menu
    if (gameMode === 0) {
        var label = document.getElementById('tc-trackname');
        if (label) {
            label.textContent = trackNamePtr ? UTF8ToString(trackNamePtr) : '';
        }
    }
});

// C functions that JS calls to inject key events
// These go through the same KeyboardProc path as real keyboard input
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

void initTouchControls() {
    js_initTouchControls();
}

void updateTouchControls() {
    // Get the current track name as UTF-8 to pass to JS
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
    js_updateTouchControls((int)GameMode, trackName, raceWon ? 1 : 0);
}
