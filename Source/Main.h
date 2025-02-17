#pragma once

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

#include "Menu/MainMenu.h"
#include "Game/MainGame.h"

using namespace Urho3D;

class App : public Application
{
    // Enable type information.
URHO3D_OBJECT(App, Application);

public:
    /// Construct.
    explicit App(Context* context);

    /// Setup before engine initialization. Modifies the engine parameters.
    void Setup() override;
    /// Setup after engine initialization. Creates the logo, console & debug HUD.
    void Start() override;
    /// Cleanup after the main loop. Called by Application.
    void Stop() override;

private:
    /// Update event handler.
    void Update(VariantMap& eventData);

    /// Scene to be rendered.
    SharedPtr<Scene> scene_;
    /// Viewport that renders the scene.
    SharedPtr<Viewport> viewport_;

    /// Geometry in the scene.
    WeakPtr<Node> geometryNode_;

    SharedPtr<MainMenu> menuScreen_;
    SharedPtr<MainGame> mainGame_;
};