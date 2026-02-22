#!/usr/bin/env ruby
# Signaling server for Stunt Car Racer two-player WebRTC.
#
# A host creates a room (gets a 4-char code). A joiner provides the code.
# Both exchange WebRTC offers, answers, and ICE candidates through this server
# using simple polling.
#
# Usage:
#   cd signaling && bundle install && ruby server.rb
#
# Endpoints:
#   POST /rooms          — host creates a room, gets { code, hostId }
#   POST /rooms/:code/join — joiner joins, gets { joinerId }
#   GET  /rooms/:code/peer/:id — poll for messages from the other peer
#   POST /rooms/:code/peer/:id — send a message to the other peer

require 'sinatra'
require 'json'
require 'securerandom'

set :port, 9292
set :bind, '0.0.0.0'

# Enable CORS for local development
before do
  headers 'Access-Control-Allow-Origin' => '*',
          'Access-Control-Allow-Methods' => 'GET, POST, OPTIONS',
          'Access-Control-Allow-Headers' => 'Content-Type'
  content_type :json
end

options '*' do
  200
end

# In-memory room storage. Each room has:
#   host_id:   random id for the host
#   joiner_id: random id for the joiner (nil until joined)
#   messages:  { peer_id => [array of messages waiting for this peer] }
#   created_at: timestamp
$rooms = {}

# Clean up rooms older than 10 minutes
def cleanup!
  cutoff = Time.now - 600
  $rooms.delete_if { |_, r| r[:created_at] < cutoff }
end

def gen_code
  # 4-char uppercase code, avoiding ambiguous chars
  chars = 'ABCDEFGHJKLMNPQRSTUVWXYZ23456789'
  4.times.map { chars[rand(chars.length)] }.join
end

# Host creates a room
post '/rooms' do
  cleanup!
  code = gen_code
  code = gen_code while $rooms.key?(code)

  host_id = SecureRandom.hex(8)
  $rooms[code] = {
    host_id: host_id,
    joiner_id: nil,
    messages: { host_id => [] },
    created_at: Time.now
  }

  { code: code, hostId: host_id }.to_json
end

# Joiner joins a room
post '/rooms/:code/join' do
  code = params[:code].upcase
  room = $rooms[code]
  halt 404, { error: 'Room not found' }.to_json unless room
  # If a joiner already exists, reset them (allow re-joining after a failed connection)
  if room[:joiner_id]
    room[:messages].delete(room[:joiner_id])
  end

  joiner_id = SecureRandom.hex(8)
  room[:joiner_id] = joiner_id
  room[:messages][joiner_id] = []

  # Clear host message queue too so stale ICE/offers don't confuse the new attempt
  room[:messages][room[:host_id]] = []

  { joinerId: joiner_id }.to_json
end

# Poll for messages destined for this peer
get '/rooms/:code/peer/:id' do
  code = params[:code].upcase
  room = $rooms[code]
  halt 404, { error: 'Room not found' }.to_json unless room

  peer_id = params[:id]
  msgs = room[:messages][peer_id]
  halt 403, { error: 'Not a member' }.to_json unless msgs

  # Drain the queue
  result = msgs.dup
  msgs.clear

  { messages: result }.to_json
end

# Send a message to the other peer
post '/rooms/:code/peer/:id' do
  code = params[:code].upcase
  room = $rooms[code]
  halt 404, { error: 'Room not found' }.to_json unless room

  sender_id = params[:id]
  halt 403, { error: 'Not a member' }.to_json unless room[:messages].key?(sender_id)

  # Determine the OTHER peer
  other_id = if sender_id == room[:host_id]
               room[:joiner_id]
             else
               room[:host_id]
             end

  # If other peer hasn't joined yet, queue message anyway (it'll be there when they poll)
  if other_id && room[:messages][other_id]
    body = JSON.parse(request.body.read)
    room[:messages][other_id] << body
  end

  { ok: true }.to_json
end
