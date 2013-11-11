KarenPowerControl::Application.routes.draw do
  devise_for :users

  root 'welcome#index'

  resources :karens, :except => [:index], shallow: true do
    resources :device, :except => [:show, :index]
  end
end
