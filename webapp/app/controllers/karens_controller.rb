class KarensController < ApplicationController
  before_action :load_karen, only: [:show, :edit, :update, :destroy]
  before_action :check_access, except: [:index]

  def index
    @karens = Karen.all
  end

  def show
  end

  def new
    @karen = Karen.new
  end

  def create
    @karen = Karen.new(karen_params)
    @karen.password = params[:password]
    
    @relationship = @karen.relationships.new
    @relationship.user_id = current_user.id
    @relationship.save
    
    if @karen.save
      redirect_to karen_path(@karen), notice: "Karen successfully added"
    else
      redirect_to new_karen_path(@karen), notice: "Karen could not be saved"
    end
  end

  def edit
  end

  def update
  end

  def destroy
  end

  private
    def load_karen
      @karen = Karen.find(params[:id])
    end
  
    def check_access
      has_access = false
      @karen.relationships.each do |rel|
        has_access = true if rel.user_id == current_user.id
      end

      if not has_access
        redirect_to karens_path, notice: "You do not have access to this Karen"
      end
    end

    def karen_params
      params.require(:karen).permit(:name, :password)
    end
end
