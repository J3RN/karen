class AddPasswordHashToKarens < ActiveRecord::Migration
  def change
    add_column :karens, :password_hash, :string
  end
end
