class CreateKarens < ActiveRecord::Migration
  def change
    create_table :karens do |t|
      t.string :name

      t.timestamps
    end
  end
end
