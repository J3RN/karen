class Condition

  def initialize(comaparison, value)
    @comparison = comparison
    @value = value
  end

  def check(value)
    if (@comparison == -1)
      @value > value
    else if (@comparison == 0)
      @value == value
    else
      @value < value
    end
  end
end 
