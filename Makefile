# Stunt Car Racer — Emscripten Build
#
# Usage:
#   make            — production build (fullscreen, custom shell, PWA assets)
#   make debug      — debug build (small canvas, log area)
#   make CHEAT=1    — build with cheat keys (W=win, L=lose during races)
#   make clean      — remove build output
#
# Requires Emscripten 5.0+ (emcc on PATH).

SOURCES = \
	StuntCarRacer.cpp \
	dxstdafx.cpp \
	Car\ Behaviour.cpp \
	3D\ Engine.cpp \
	Opponent\ Behaviour.cpp \
	Car.cpp \
	Track.cpp \
	wavefunctions.cpp \
	Backdrop.cpp \
	$(wildcard Substitutes/*.cpp)

EMCC_FLAGS = \
	-O2 \
	-ferror-limit=1000 \
	-s LLD_REPORT_UNDEFINED \
	-s EXPORTED_FUNCTIONS='["_main","_touchSetDriveInput","_jsSelectTrack","_jsStartPreview","_jsStartGame","_jsGoToMenu","_jsGetTrackID","_jsGetNumTracks","_jsIsRaceFinished","_jsIsRaceWon","_jsIsPlayerWrecked","_jsGetOpponentId","_jsGetBoostReserve","_jsGetBoostMax","_jsGetDamage","_jsGetLapNumber","_jsGetGameMode","_jsSetGameOver","_jsGetTrackName","_jsGetOpponentName","_jsGetPlayerBestLap","_jsGetOpponentBestLap","_jsIsSoloMode","_jsGetDisplaySpeed","_jsGetCurrentLapTime","_jsGetDistanceToOpponent","_jsSetTwoPlayerMode","_jsSetTwoPlayerSide","_jsGetPlayerRoadSection","_jsGetPlayerDistanceIntoSection","_jsGetPlayerRoadXPosition","_jsGetPlayerZSpeed","_jsGetPlayerDamage","_jsGetPlayerWheelFL","_jsGetPlayerWheelFR","_jsGetPlayerWheelR","_jsSetOpponentState"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString"]' \
	-lopenal \
	--use-preload-plugins \
	-s USE_SDL_IMAGE=2 \
	-s SDL2_IMAGE_FORMATS='["bmp"]' \
	-s MAX_WEBGL_VERSION=2 \
	--embed-file Tracks \
	--embed-file Bitmap \
	--embed-file Sounds

# Cheat mode: CHEAT=1 adds W/L keys to force win/loss during races
comma := ,
ifdef CHEAT
EMCC_FLAGS += -DCHEAT_MODE
# Append cheat functions to the export list
EMCC_FLAGS := $(subst _jsSetOpponentState"],_jsSetOpponentState"$(comma)"_jsCheatWin"$(comma)"_jsCheatLose"],$(EMCC_FLAGS))
endif

DIST = dist

# Static PWA assets to copy into dist/ for production builds
PWA_ASSETS = manifest.json icon-192.png icon-512.png game.css

# JavaScript sources (live in javascript/ but get copied flat into dist/)
JS_SOURCES = javascript/sw.js javascript/multiplayer.js javascript/game.js
JS_DIST    = $(patsubst javascript/%,$(DIST)/%,$(JS_SOURCES))

# ─── Targets ────────────────────────────────────────────────

.PHONY: all debug clean

all: $(DIST)/source.html $(addprefix $(DIST)/,$(PWA_ASSETS)) $(JS_DIST)

debug: $(DIST)/source.html

$(DIST)/source.html: $(SOURCES) custom_shell.html game.css $(JS_SOURCES) | $(DIST)
	@echo "Building…"
	@if [ "$(MAKECMDGOALS)" = "debug" ]; then \
		echo "  (debug build)"; \
		emcc $(SOURCES) $(EMCC_FLAGS) -o $(DIST)/source.html; \
	else \
		echo "  (production build — using custom shell)"; \
		emcc $(SOURCES) $(EMCC_FLAGS) --shell-file custom_shell.html -o $(DIST)/source.html; \
	fi

$(DIST)/%.json: %.json | $(DIST)
	cp $< $@

$(DIST)/%.js: javascript/%.js | $(DIST)
	cp $< $@

$(DIST)/%.css: %.css | $(DIST)
	cp $< $@

$(DIST)/%.png: %.png | $(DIST)
	cp $< $@

$(DIST):
	mkdir -p $(DIST)

clean:
	rm -rf $(DIST)
