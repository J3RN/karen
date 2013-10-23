require "sinatra"
require "./Device.rb"

configure do
  @devices = Hash.new
end

get '/' do
  "Hello, there!"
end

post '/new' do
  pin = params[:pin]
  newDevice = Device.new(pin)
  @devices[pin] = newDevice
end

post '/add_condition' do
  comparison, value = params[:comp], params[:val]
  device = getDevice 
  if device
    newConditon = Condition.new(comparison, value)
    device.addCondition(newCondition)
  end
end
  
post '/update' do
  
end

get '/requestOn/:id' do
  device.askStart if device = getDevice
end

get '/requestOff/:id' do 
  device.askStop if device = getDevice
end

get '/requestedOn/:id' do
  device.requestOn if device = getDevice 
end

get '/requestedOff/:id' do
  device.requestOff if device = getDevice
end

get '/confirmOn/:id' do
  device.confirmOn if device = getDevice
end

get '/confirmOff/:id' do
  device.confirmOff if device = getDevice
end

get '/status/:id' do
  device.isOn if device = getDevice
end

def getDevice
  pin = params[:id]
  if (@devices.hasKey(pin))
    @devices[pin]
  else
    nil
  end
end
