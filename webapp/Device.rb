class Device

  def initialize(pin)
    @pin = pin
    @on, @triggeredOn, @triggeredOff = false, false, false
    @onConditions, @offConditions = Array.new, Array.new
  end

  def addOnCondition(condition)
    @onConditions << condition
  end

  def addOffCondition(condition)
    @offConditions << condition
  end

  def getOnConditions
    @onConditions
  end

  def getOffConditions
    @offConditions
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
