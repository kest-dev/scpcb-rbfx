#pragma once

#include <Urho3D/Engine/StateManager.h>
#include <Urho3D/RmlUI/RmlUIComponent.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

class MainMenuRML : public RmlUIComponent
{
    URHO3D_OBJECT(MainMenuRML, RmlUIComponent)

public:
    explicit MainMenuRML(Context* context);

    static void RegisterObject(Context* context);

private:
    void Update(float timeStep) override;
    void OnDataModelInitialized() override;

    void OnNewGame();
    void OnSettings();
    void OnExit();
};

class MainMenu : public ApplicationState
{
    URHO3D_OBJECT(MainMenu, ApplicationState)

public:
    explicit MainMenu(Context* context);

    void Activate(StringVariantMap& bundle) override;
    void Update(float timeStep) override;
    void Deactivate() override;

private:
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);

    SharedPtr<Scene> scene_;
    SharedPtr<Node> cameraNode_;
    SharedPtr<MainMenuRML> window_;
};
