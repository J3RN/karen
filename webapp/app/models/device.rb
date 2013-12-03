class Device < ActiveRecord::Base
  belongs_to :karen
  validates_presence_of :name, :status, :pin, :karen_id

  OFF = 0
  REQUESTED_ON = 1
  ON = 2
  REQUESTED_OFF = 3
end
