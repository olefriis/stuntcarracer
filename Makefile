# Stunt Car Racer — Emscripten Build
#
# Usage:
#   make       — production build (fullscreen, custom shell, PWA assets)
#   make debug — debug build (small canvas, log area)
#   make clean — remove build output
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
	-s EXPORTED_FUNCTIONS='["_main","_touchKeyDown","_touchKeyUp","_getTouchGameMode"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
	-lopenal \
	--use-preload-plugins \
	-s USE_SDL_IMAGE=2 \
	-s SDL2_IMAGE_FORMATS='["bmp"]' \
	-s MAX_WEBGL_VERSION=2 \
	--embed-file Tracks \
	--embed-file Bitmap \
	--embed-file Sounds

DIST = dist

# Static PWA assets to copy into dist/ for production builds
PWA_ASSETS = manifest.json sw.js icon-192.png icon-512.png

# ─── Targets ────────────────────────────────────────────────

.PHONY: all debug clean

all: $(DIST)/source.html $(addprefix $(DIST)/,$(PWA_ASSETS))

debug: $(DIST)/source.html

$(DIST)/source.html: $(SOURCES) custom_shell.html | $(DIST)
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

$(DIST)/%.js: %.js | $(DIST)
	cp $< $@

$(DIST)/%.png: %.png | $(DIST)
	cp $< $@

$(DIST):
	mkdir -p $(DIST)

clean:
	rm -rf $(DIST)
