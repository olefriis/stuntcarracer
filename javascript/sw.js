// Service worker for Stunt Car Racer PWA
// Caches game files for offline play and home-screen launch

var CACHE_NAME = 'scr-v61';
var ASSETS = [
  'source.html',
  'source.js',
  'source.wasm',
  'multiplayer.js',
  'game.js',
  'game.css',
  'manifest.json',
  'icon-192.png',
  'icon-512.png'
];

// Install: pre-cache core assets
self.addEventListener('install', async function () {
  var cache = await caches.open(CACHE_NAME);
  await cache.addAll(ASSETS);
  await self.skipWaiting();
});

// Activate: clean up old caches
self.addEventListener('activate', async function () {
  var names = await caches.keys();
  await Promise.all(
    names.filter(function (n) { return n !== CACHE_NAME; })
         .map(function (n) { return caches.delete(n); })
  );
  await self.clients.claim();
});

// Fetch: serve from cache, fall back to network
self.addEventListener('fetch', function (event) {
  event.respondWith(
    caches.match(event.request).then(function (cached) {
      return cached || fetch(event.request);
    })
  );
});
