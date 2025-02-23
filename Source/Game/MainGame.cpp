#include "MainGame.h"

#include <Urho3D/Navigation/DynamicNavigationMesh.h>
#include <Urho3D/Navigation/Navigable.h>
#include <Urho3D/Navigation/CrowdManager.h>
#include <Urho3D/Navigation/CrowdAgent.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/DebugNew.h>

MainGame::MainGame(Context* context)
    : ApplicationState(context),
    drawDebug_(false)
{
    Room::RegisterObject(context);
    PlayerController::RegisterObject(context);
}

void MainGame::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    auto* debug = scene_->GetComponent<DebugRenderer>();

    {
        scene_->GetComponent<DynamicNavigationMesh>()->DrawDebugGeometry(true);
    }
}

void MainGame::Activate(StringVariantMap &bundle)
{
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();
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

    auto* navMesh = scene_->CreateComponent<DynamicNavigationMesh>();
    navMesh->SetAgentHeight(0.1f);
    navMesh->SetTileSize(32);
    navMesh->SetCellSize(0.02f);
    navMesh->SetAgentRadius(0.1f);
    scene_->CreateComponent<Navigable>();
    navMesh->Rebuild();

    auto* crowdManager = scene_->CreateComponent<CrowdManager>();
    CrowdObstacleAvoidanceParams params = crowdManager->GetObstacleAvoidanceParams(0);
    // Set the params to "High (66)" setting
    params.velBias = 0.5f;
    params.adaptiveDivs = 7;
    params.adaptiveRings = 3;
    params.adaptiveDepth = 3;
    crowdManager->SetObstacleAvoidanceParams(0, params);

    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MainGame, HandlePostRenderUpdate));
}

void MainGame::Update(float timeStep)
{
    auto* input = GetSubsystem<Input>();

    if (input->GetKeyPress(KEY_SPACE))
        drawDebug_ = !drawDebug_;
}

void MainGame::Deactivate()
{

}