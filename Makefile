# Stunt Car Racer — Emscripten Build
#
# Usage:
#   make            — production build (fullscreen, custom shell, PWA assets)
#   make debug      — debug build (small canvas, log area)
#   make CHEAT=1    — build with cheat keys (W=win, L=lose during races)
#   make clean      — remove build output
#
# Requires Emscripten 5.0+ (emcc on PATH).

ENGINE = game-engine
WEB    = web

SOURCES = \
	$(ENGINE)/StuntCarRacer.cpp \
	$(ENGINE)/dxstdafx.cpp \
	$(ENGINE)/Car\ Behaviour.cpp \
	$(ENGINE)/3D\ Engine.cpp \
	$(ENGINE)/Opponent\ Behaviour.cpp \
	$(ENGINE)/Car.cpp \
	$(ENGINE)/Track.cpp \
	$(ENGINE)/wavefunctions.cpp \
	$(ENGINE)/Backdrop.cpp \
	$(wildcard $(ENGINE)/Substitutes/*.cpp)

EMCC_FLAGS = \
	-O2 \
	-ferror-limit=1000 \
	-s LLD_REPORT_UNDEFINED \
	-s EXPORTED_FUNCTIONS='["_main","_touchSetDriveInput","_jsSelectTrack","_jsStartPreview","_jsStartGame","_jsGoToMenu","_jsGetTrackID","_jsGetNumTracks","_jsIsRaceFinished","_jsIsRaceWon","_jsIsPlayerWrecked","_jsGetOpponentId","_jsGetBoostReserve","_jsGetBoostMax","_jsGetDamage","_jsGetDamageHolePosition","_jsSetDamageHolePosition","_jsGetLapNumber","_jsGetGameMode","_jsSetGameOver","_jsGetTrackName","_jsGetOpponentName","_jsGetPlayerBestLap","_jsGetOpponentBestLap","_jsIsSoloMode","_jsGetDisplaySpeed","_jsGetCurrentLapTime","_jsGetDistanceToOpponent","_jsSetTwoPlayerMode","_jsSetTwoPlayerSide","_jsGetPlayerRoadSection","_jsGetPlayerDistanceIntoSection","_jsGetPlayerRoadXPosition","_jsGetPlayerZSpeed","_jsGetPlayerDamage","_jsGetPlayerWheelFL","_jsGetPlayerWheelFR","_jsGetPlayerWheelR","_jsSetOpponentState","_jsSetSuperLeague","_jsGetSparkFerocity"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString"]' \
	-lopenal \
	--use-preload-plugins \
	-s USE_SDL_IMAGE=2 \
	-s SDL2_IMAGE_FORMATS='["bmp"]' \
	-s MAX_WEBGL_VERSION=2 \
	--embed-file $(ENGINE)/Tracks@Tracks \
	--embed-file $(ENGINE)/Bitmap@Bitmap \
	--embed-file $(ENGINE)/Sounds@Sounds

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

# JavaScript sources (copied flat into dist/)
JS_SOURCES = $(WEB)/javascript/sw.js $(WEB)/javascript/multiplayer.js $(WEB)/javascript/game.js
JS_DIST    = $(patsubst $(WEB)/javascript/%,$(DIST)/%,$(JS_SOURCES))

# ─── Targets ────────────────────────────────────────────────

.PHONY: all debug clean

# Image assets under web/images/ to copy into dist/images/
IMAGES     = $(shell find $(WEB)/images -type f)
IMAGES_DIST = $(patsubst $(WEB)/%,$(DIST)/%,$(IMAGES))

all: $(DIST)/source.html $(addprefix $(DIST)/,$(PWA_ASSETS)) $(JS_DIST) $(IMAGES_DIST)

debug: $(DIST)/source.html

$(DIST)/source.html: $(SOURCES) $(WEB)/custom_shell.html $(WEB)/game.css $(JS_SOURCES) | $(DIST)
	@echo "Building…"
	@if [ "$(MAKECMDGOALS)" = "debug" ]; then \
		echo "  (debug build)"; \
		emcc $(SOURCES) $(EMCC_FLAGS) -o $(DIST)/source.html; \
	else \
		echo "  (production build — using custom shell)"; \
		emcc $(SOURCES) $(EMCC_FLAGS) --shell-file $(WEB)/custom_shell.html -o $(DIST)/source.html; \
	fi

$(DIST)/%.json: $(WEB)/%.json | $(DIST)
	cp $< $@

$(DIST)/%.js: $(WEB)/javascript/%.js | $(DIST)
	cp $< $@

$(DIST)/%.css: $(WEB)/%.css | $(DIST)
	cp $< $@


$(DIST)/images/%: $(WEB)/images/% | $(DIST)
	mkdir -p $(dir $@)
	cp $< $@

$(DIST)/%.png: $(WEB)/%.png | $(DIST)
	cp $< $@

$(DIST):
	mkdir -p $(DIST)

$(DIST)/images:
	mkdir -p $(DIST)/images

clean:
	rm -rf $(DIST)
