// Touch controls overlay for mobile devices.
// Shows on-screen buttons that inject keyboard events into the game.

#ifndef _TOUCH_CONTROLS_H
#define _TOUCH_CONTROLS_H

// Initialize touch controls (call once during setup).
// Creates the HTML elements and registers touch event handlers.
void initTouchControls();

// Update touch controls visibility based on current game mode.
// Call this once per frame.
void updateTouchControls();

#endif // _TOUCH_CONTROLS_H
