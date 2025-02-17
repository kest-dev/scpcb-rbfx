#include <Urho3D/RmlUI/RmlUI.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Zone.h>

#include "MainMenu.h"

MainMenuRML::MainMenuRML(Context* context)
    : RmlUIComponent(context)
{
    SetResource("UI/MainMenu.rml");
}

void MainMenuRML::RegisterObject(Context* context)
{
    context->RegisterFactory<MainMenuRML>();
}

void MainMenuRML::OnDataModelInitialized()
{
    Rml::DataModelConstructor* constructor = GetDataModelConstructor();
    URHO3D_ASSERT(constructor);

    constructor->RegisterArray<StringVector>();

    constructor->BindEventCallback("on_new_game", WrapCallback(&MainMenuRML::OnNewGame));
    constructor->BindEventCallback("on_settings", WrapCallback(&MainMenuRML::OnSettings));
    constructor->BindEventCallback("on_exit", WrapCallback(&MainMenuRML::OnExit));
}

void MainMenuRML::Update(float timeStep)
{
    BaseClassName::Update(timeStep);
}

void MainMenuRML::OnNewGame()
{
    // DO SOMETHING!
}

void MainMenuRML::OnSettings()
{
    // DO SOMETHING!
}

void MainMenuRML::OnExit()
{
    auto* engine = context_->GetSubsystem<Engine>();
    engine->Exit();
}

MainMenu::MainMenu(Context* context)
    : ApplicationState(context)
{
    auto ui = context_->GetSubsystem<RmlUI>();

    SetMouseMode(MM_FREE);
    SetMouseVisible(true);

    auto* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    auto* zone = scene_->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetFogColor(Color::GRAY);
    zone->SetFogStart(100.0f);
    zone->SetFogEnd(300.0f);

    // Create a camera.
    cameraNode_ = scene_->CreateChild("Camera");
    auto camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFov(90.0f);

    // Set an initial position for the camera scene node.
    cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -2.0f));

    // Create a window rendered into backbuffer.
    window_ = scene_->CreateComponent<MainMenuRML>();
    window_->SetEnabled(false);

    // Set up a viewport so 3D scene can be visible.
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, camera));
    SetViewport(0, viewport);

    // Disable updates until state is activated.
    scene_->SetUpdateEnabled(false);
}

void MainMenu::Activate(StringVariantMap& bundle)
{
    BaseClassName::Activate(bundle);
    window_->SetEnabled(true);
    SubscribeToEvent(Urho3D::E_KEYDOWN, URHO3D_HANDLER(MainMenu, HandleKeyDown));

    // Enable scene updates.
    scene_->SetUpdateEnabled(true);
}

void MainMenu::Update(float timeStep)
{
}

void MainMenu::Deactivate()
{
    BaseClassName::Deactivate();

    // Disable updates until state is activated.
    scene_->SetUpdateEnabled(false);

    UnsubscribeFromEvent(Urho3D::E_KEYDOWN);
    window_->SetEnabled(false);
}

void MainMenu::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    const auto key = static_cast<Key>(eventData[KeyDown::P_KEY].GetUInt());

    if(key == KEY_R)
    {
        window_->SetResource("");
        window_->SetResource("UI/MainMenu.rml");
    }
}