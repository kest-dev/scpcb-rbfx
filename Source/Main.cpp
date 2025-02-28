// Copyright (c) 2023-2023 the rbfx project.
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT> or the accompanying LICENSE file.

#include "Main.h"

#include "Menu/MainMenu.h"

// Define entry point.
URHO3D_DEFINE_APPLICATION_MAIN(App);

App::App(Context *context)
    : Application(context)
{
}

void App::Setup()
{
    // Organization and application names are used to create writeable folder in OS-specific location.
    // For example, on Windows it would be C:/Users/<username>/AppData/Roaming/<orgname>/<appname>
    engineParameters_[EP_ORGANIZATION_NAME] = "My Organization";
    engineParameters_[EP_APPLICATION_NAME] = "My Application";
    // conf:// directory is mapped to that writeable folder.
    engineParameters_[EP_LOG_NAME] = "conf://App.log";
    engineParameters_[EP_SOUND] = true;

    // Register Object
    MainMenuRML::RegisterObject(context_);
    MainMenu::RegisterObject(context_);
}

void App::Start()
{
    auto engine = GetSubsystem<Engine>();
    auto stateManager = GetSubsystem<StateManager>();

    mainGame_ = MakeShared<MainGame>(context_);

    stateManager->EnqueueState(mainGame_);
}

void App::Stop()
{

}

void App::Update(VariantMap& eventData)
{

}
