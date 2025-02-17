#include "MainGame.h"

#include <Urho3D/Input/FreeFlyController.h>

MainGame::MainGame(Context* context)
    : ApplicationState(context)
{
    Room::RegisterObject(context);
    PlayerController::RegisterObject(context);
}

void MainGame::Activate(StringVariantMap &bundle)
{
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    auto* zone = scene_->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
    zone->SetFogColor(Color::GRAY);
    zone->SetFogStart(100.0f);
    zone->SetFogEnd(300.0f);

    Node* roomNode = scene_->CreateChild();
    testRoom_ = roomNode->CreateComponent<Room>();
    testRoom_->LoadXML("Rooms/008/room.xml");

    Node* playerNode = scene_->CreateChild();
    playerNode->SetPosition(Vector3::UP);
    player_ = playerNode->CreateComponent<PlayerController>();
}

void MainGame::Update(float timeStep)
{

}

void MainGame::Deactivate()
{

}