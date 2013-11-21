KarenPowerControl::Application.routes.draw do
  devise_for :users

  root 'welcome#index'

  resources :karens, shallow: true do
    resources :devices, :except => [:show, :index]
  end
end
