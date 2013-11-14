require 'bcrypt'

class Karen < ActiveRecord::Base
  include BCrypt

  has_many :users
  has_many :devices

  def password
    @password ||= Password.new(password_hash)
  end

  def password=(new_password)
    @password = Password.create(new_password)
    self.password_hash = @password
  end
end
