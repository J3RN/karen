class CreateConditions < ActiveRecord::Migration
  def change
    create_table :conditions do |t|
      t.integer :sensor
      t.integer :value
      t.integer :comparison

      t.timestamps
    end
  end
end
