class Device

  def initialize(pin)
    @pin = pin, @on = false, @triggeredOn = false, @triggeredOff = false
  end

  def askStart
    if !@on
      @triggeredOn = true
    end
  end

  def askStop
    if @on
      @triggeredOff = true
    end
  end

  def confirmOn
    if @triggeredOn
      @triggeredOn = false
      @on = true
    end
  end

  def confirmOff
    if @triggeredOff
      @triggeredOff = false
      @on = false
    end
  end

  def isOn
    @on
  end

end
