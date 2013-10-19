Webapp::Application.routes.draw do
  devise_for :users
  
  root 'main#index'

  resources :devices, shallow: true  do
    resources :conditions
  end
end
