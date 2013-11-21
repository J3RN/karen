class AddKarenIdToDevice < ActiveRecord::Migration
  def change
    add_column :devices, :karen_id, :integer
  end
end
