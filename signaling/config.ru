require_relative 'server'

# Fix "Attack prevented by Rack::Protection::HostAuthorization"...
set :environment, :production

run Sinatra::Application
