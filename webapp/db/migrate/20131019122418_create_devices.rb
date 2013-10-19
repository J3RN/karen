class CreateDevices < ActiveRecord::Migration
  def change
    create_table :devices do |t|
      t.string :name
      t.integer :pin
      t.boolean :on

      t.timestamps
    end
  end
end
