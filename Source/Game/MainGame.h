#pragma once

#include <Urho3D/Engine/StateManager.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Scene/Scene.h>

#include "../Map/Room.h"
#include "../PlayerController.h"

using namespace Urho3D;

class MainGame : public ApplicationState
{
    URHO3D_OBJECT(MainGame, ApplicationState)

public:
    explicit MainGame(Context* context);

    void Activate(StringVariantMap& bundle) override;
    void Update(float timeStep) override;
    void Deactivate() override;

private:
    SharedPtr<Scene> scene_;
    SharedPtr<Node> cameraNode_;
    SharedPtr<Room> testRoom_;
    SharedPtr<PlayerController> player_;
};