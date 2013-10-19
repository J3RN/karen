class MainController < ApplicationController
  def index
  end

  def show
    @devices = Device.all
  end
end
