class DevicesController < ApplicationController
  before_action :load_device, only: [:edit, :update, :destroy]
  before_action :load_karen
  before_action :check_access, except: [:new, :create]

  def new
    @device = Device.new
  end

  def create
    @device = Device.new(device_params)
    @device.karen_id = @karen.id
    
    if @device.save
      redirect_to karen_path(@karen), notice: "Device successfully added"
    else
      redirect_to new_karen_device_path(@device), notice: "Device could not be saved"
    end
  end

  def edit
  end

  def update
  end

  def destroy
  end

  private
    def load_device
      @device = Device.find(params[:id])
    end
  
    def load_karen
      @karen = Karen.find(params[:karen_id])
    end

    def device_params
      params.require(:device).permit(:name, :pin, :status, :karen_id)
    end
end
