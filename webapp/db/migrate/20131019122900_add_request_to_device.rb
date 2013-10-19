class AddRequestToDevice < ActiveRecord::Migration
  def change
    add_column :devices, :requested, :boolean
  end
end
