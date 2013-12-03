class Device < ActiveRecord::Base
  belongs_to :karen
  validates_presence_of :name, :status, :pin, :karen_id
end
