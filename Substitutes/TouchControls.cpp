// Touch/drive input bridge for Emscripten.
// All UI creation and game flow logic lives in game.js.
// This file only exposes C functions that JS calls to inject input.

#include "../dxstdafx.h"
#include "../StuntCarRacer.h"
#include "../Car Behaviour.h"
#include "TouchControls.h"
#include <emscripten.h>

extern DWORD lastInput;

extern "C" {

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

// Called once at startup — JS handles its own init via game.js
void initTouchControls() {
}

// Called every frame — JS polls state directly, nothing to do here
void updateTouchControls() {
}
