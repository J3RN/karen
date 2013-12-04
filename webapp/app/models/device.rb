class Device < ActiveRecord::Base
  belongs_to :karen
  validates_presence_of :name, :status, :pin, :karen_id

  OFF = 0
  REQUESTED_ON = 1
  ON = 2
  REQUESTED_OFF = 3

  def status_string
    case self.status
    when OFF
      "Off"
    when REQUESTED_ON
      "Requested to turn on"
    when ON
      "On"
    when REQUESTED_OFF
      "Requested to turn off"
    end
  end
end
