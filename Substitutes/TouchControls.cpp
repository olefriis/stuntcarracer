// Touch controls overlay for mobile devices.
// Creates HTML button elements over the WebGL canvas and injects keyboard
// events into the game via the existing KeyboardProc callback mechanism.

#include "../dxstdafx.h"
#include "../StuntCarRacer.h"
#include "TouchControls.h"
#include <emscripten.h>

extern GameModeType GameMode;
extern UINT keyPress;
extern DWORD lastInput;

// Expose current game mode to JavaScript
EM_JS(int, js_getGameMode, (), {
    return Module._getTouchGameMode();
});

// C function callable from JS to get game mode
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int getTouchGameMode() {
        return (int)GameMode;
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
    createButton('tc-prev', '◀', 'left:2vw;bottom:6vh;width:15vw;height:15vw;font-size:7vw;max-width:80px;max-height:80px;');
    createButton('tc-next', '▶', 'left:19vw;bottom:6vh;width:15vw;height:15vw;font-size:7vw;max-width:80px;max-height:80px;');
    createButton('tc-select', 'SELECT', 'right:2vw;bottom:6vh;width:25vw;height:15vw;font-size:3.5vw;max-width:140px;max-height:80px;');

    // --- Track Preview buttons ---
    createButton('tc-back', 'BACK', 'left:2vw;bottom:6vh;width:22vw;height:12vw;font-size:4.5vw;max-width:120px;max-height:70px;');
    createButton('tc-start', 'START', 'right:2vw;bottom:6vh;width:22vw;height:12vw;font-size:4.5vw;max-width:120px;max-height:70px;');

    // --- In-Game controls ---
    // Left side: steering
    createButton('tc-left', '◀', 'left:2vw;bottom:8vh;width:18vw;height:18vw;font-size:8vw;max-width:100px;max-height:100px;');
    createButton('tc-right', '▶', 'left:22vw;bottom:8vh;width:18vw;height:18vw;font-size:8vw;max-width:100px;max-height:100px;');
    // Right side: accel/brake
    createButton('tc-accel', 'GAS', 'right:2vw;bottom:8vh;width:18vw;height:18vw;font-size:5vw;max-width:100px;max-height:100px;');
    createButton('tc-brake', 'BRK', 'right:22vw;bottom:8vh;width:18vw;height:18vw;font-size:5vw;max-width:100px;max-height:100px;');
    // Top: boost and menu
    createButton('tc-boost', 'BOOST', 'right:2vw;bottom:35vh;width:18vw;height:12vw;font-size:3.5vw;max-width:100px;max-height:70px;');
    createButton('tc-menu', '✕', 'right:2vw;top:2vh;width:12vw;height:12vw;font-size:6vw;max-width:60px;max-height:60px;');

    // --- Game Over button ---
    createButton('tc-gameover', 'MENU', 'left:50%;bottom:20vh;width:30vw;height:12vw;font-size:5vw;max-width:160px;max-height:70px;transform:translateX(-50%);');

    // Track the current track index for cycling through tracks in menu
    window._touchTrackIndex = 0;

    // Wire up touch events. We simulate key presses via the keyboard callback.
    // For instantaneous keys (menu actions) we use touchstart only.
    // For held keys (steering, gas, brake) we use touchstart + touchend.
    function simulateKeyDown(keyCode) {
        // Call the C keyboard callback through Emscripten
        Module._touchKeyDown(keyCode);
    }
    function simulateKeyUp(keyCode) {
        Module._touchKeyUp(keyCode);
    }
    function simulateKeyPress(keyCode) {
        // Press and release for menu-type actions
        Module._touchKeyDown(keyCode);
        // Release on next frame to ensure it's processed
        setTimeout(function() { Module._touchKeyUp(keyCode); }, 100);
    }

    function addHoldButton(id, keyCode) {
        var el = document.getElementById(id);
        el.addEventListener('touchstart', function(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.5)';
            simulateKeyDown(keyCode);
        }, {passive: false});
        el.addEventListener('touchend', function(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.25)';
            simulateKeyUp(keyCode);
        }, {passive: false});
        el.addEventListener('touchcancel', function(e) {
            e.preventDefault();
            el.style.background = 'rgba(255,255,255,0.25)';
            simulateKeyUp(keyCode);
        }, {passive: false});
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
    // Prev track: cycle down through track numbers
    addTapButton('tc-prev', function() {
        window._touchTrackIndex--;
        if (window._touchTrackIndex < 0) window._touchTrackIndex = 7; // NUM_TRACKS - 1
        // Send the track number key (1-8 = key codes 49-56)
        simulateKeyPress(49 + window._touchTrackIndex); // '1' + index
    });
    // Next track: cycle up through track numbers
    addTapButton('tc-next', function() {
        window._touchTrackIndex++;
        if (window._touchTrackIndex > 7) window._touchTrackIndex = 0;
        simulateKeyPress(49 + window._touchTrackIndex); // '1' + index
    });
    // Select track: press 'S'
    addTapButton('tc-select', function() {
        simulateKeyPress(83); // 'S'
    });

    // --- Track Preview ---
    // Back to menu: press 'M'
    addTapButton('tc-back', function() {
        simulateKeyPress(77); // 'M'
    });
    // Start game: press 'S'
    addTapButton('tc-start', function() {
        simulateKeyPress(83); // 'S'
    });

    // --- In-Game controls (hold buttons) ---
    addHoldButton('tc-left', 83);   // 'S' = steer left
    addHoldButton('tc-right', 68);  // 'D' = steer right
    addHoldButton('tc-accel', 13);  // Enter = accelerate
    addHoldButton('tc-brake', 32);  // Space = brake
    addHoldButton('tc-boost', 222); // 0xDE = boost (hash key)

    // Menu button during game: press 'M'
    addTapButton('tc-menu', function() {
        simulateKeyPress(77); // 'M'
    });

    // --- Game Over ---
    addTapButton('tc-gameover', function() {
        simulateKeyPress(77); // 'M'
    });

    window._touchControlsReady = true;
    window._lastTouchGameMode = -1;
});

// Update which buttons are visible based on game mode
EM_JS(void, js_updateTouchControls, (int gameMode), {
    if (!window._isTouchDevice || !window._touchControlsReady) return;
    if (gameMode === window._lastTouchGameMode) return;
    window._lastTouchGameMode = gameMode;

    // Game mode constants (must match GameModeType enum)
    var TRACK_MENU = 0;
    var TRACK_PREVIEW = 1;
    var GAME_IN_PROGRESS = 2;
    var GAME_OVER = 3;

    // All button ids grouped by mode
    var menuBtns = ['tc-prev', 'tc-next', 'tc-select'];
    var previewBtns = ['tc-back', 'tc-start'];
    var gameBtns = ['tc-left', 'tc-right', 'tc-accel', 'tc-brake', 'tc-boost', 'tc-menu'];
    var gameOverBtns = ['tc-gameover'];

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
    else if (gameMode === GAME_OVER) show = gameOverBtns.concat(['tc-left', 'tc-right', 'tc-accel', 'tc-brake']); // keep game btns but also show menu

    for (var i = 0; i < show.length; i++) {
        var el = document.getElementById(show[i]);
        if (el) el.style.display = 'flex';
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
    js_updateTouchControls((int)GameMode);
}
