class CoffeeMaker
	def initialize
		@shouldBrew = false, @shouldStop = false, @brewing = false
	end

	# Methods involving @shouldBrew (order methods)
	def place_order
		@shouldBrew = true
	end

	def cancel_order
		@shouldBrew = false
	end

	def should_brew
		@shouldBrew
	end

	# Methods involving @shouldStop (stop methods)
	def stop
		@shouldStop = true
	end

	def stopped
		@shouldStop = false
	end

	def should_stop
		@shouldStop
	end

	# Methods involving @brewing (brewing methods)
	def stop_brewing
		@brewing = false
	end

	def start_brewing
		@brewing = true
	end

	def is_brewing
		@brewing
	end
end
