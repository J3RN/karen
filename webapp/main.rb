# A web app to act as a middleman between the user and the coffee maker
# Author: Jonathan Arnett
# Modified: 05/10/2013

require 'sinatra'
require './CoffeeMaker.rb'

# Make a CoffeeMaker object to hold info about what is going on
configure do
	@@myCoffeeMaker = CoffeeMaker.new
end

# Print a webpage to have a nice webpage for placing orders
get '/' do
	"<html>
		<head>
			<script src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js\"></script>
			<script type=\"text/javascript\">
				$(\".start-button\").click( function() {
					$(\".result-div\").load(\"make_coffee\");
				});
			</script>
		</head>
		<body>
			<button class=\"start-button\">Make Coffee</button>
			<div class=\"result-div\"></div>
		</body>
	</html>"
end

# Put in an order for coffee
# If coffee is already brewing, yell at user
# If an order has already been placed, tease user
# Otherwise, place an order
get '/make_coffee' do
	if @@myCoffeeMaker.is_brewing
		msg = "Coffee is already brewing!"
	elsif @@myCoffeeMaker.should_brew
		msg = "The coffee is about to be brewed! Be calm, young one."
	else
		@@myCoffeeMaker.place_order
		msg = "Your coffee will be brewed soon."
	end

	msg
end

# Tell the coffee maker to stop
# If is brewing, tells the coffee maker to stop
# If an order has been placed, cancels the order
get '/stop_coffee' do
	if @@myCoffeeMaker.is_brewing
		@@myCoffeeMaker.stop
		msg = "The coffee maker will be stopped."
	elsif @@myCoffeeMaker.should_brew
		@@myCoffeeMaker.cancel_order
		msg = "The order for coffee has been cancelled."
	else
		msg = "Coffee was not brewing and no orders were placed."
	end

	msg
end

# Prints a message describing the status of the system
get '/status' do
	if @@myCoffeeMaker.is_brewing
		msg = "Coffee is brewing"
	elsif @@myCoffeeMaker.should_brew
		msg = "Coffee will be brewed soon"
	elsif @@myCoffeeMaker.should_stop
		msg = "The coffee maker will be turned off soon"
	else
		msg = "The coffee maker is off and awaiting your order"
	end

	msg
end

# Prints a 1 or 0 for the coffee maker to interpret as start brewing or remain off
# If coffee should be made, update brewing, cancel the order, and tell the coffee maker to start
# Else, tell the coffee maker coffee should not be made
get '/should_brew' do
	stat = @@myCoffeeMaker.should_brew
	
	if stat
		@@myCoffeeMaker.cancel_order
		@@myCoffeeMaker.start_brewing

		numStat = 1
	else
		numStat = 0
	end

	"#{numStat}"
end

# Prints a 1 or the 0 for the coffee maker to interpret as stop brewing or continute to brew
# If coffee should be stopped, update brewing, say that it has been stopped, and tell the coffee
# 	maker to stop
# If coffee should not be stopped, tell the coffee maker to continue brewing
get '/should_stop' do
	stat = @@myCoffeeMaker.should_stop

	if stat
		@@myCoffeeMaker.stopped
		@@myCoffeeMaker.stop_brewing

		numStat = 1
	else
		numStat = 0
	end

	"#{numStat}"
end

