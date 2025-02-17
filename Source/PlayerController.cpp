#include <Urho3D/Input/FreeFlyController.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/MoveAndOrbitController.h>

#include "PlayerController.h"

#include "Urho3D/Resource/ResourceCache.h"

PlayerController::PlayerController(Context* context)
        : MoveAndOrbitComponent(context)
{
}

void PlayerController::RegisterObject(Context* context)
{
    context->RegisterFactory<ClassName>();
}

void PlayerController::DelayedStart()
{
    const auto moveAndOrbit = node_->CreateComponent<MoveAndOrbitController>();
    moveAndOrbit->LoadInputMap("Input/MoveAndOrbit.inputmap");
    inputMap_ = moveAndOrbit->GetInputMap();
}


void PlayerController::FixedUpdate(float timeStep)
{
    const float moveSpeed = 5.0f;

    // Get local move direction.
    Vector3 localDirection = GetVelocity();
    localDirection.Normalize();

    // Get world move direction.
    const float yawAngle = cameraNode_->GetWorldRotation().YawAngle();
    const Vector3 worldDirection = Quaternion{ yawAngle, Vector3::UP } *localDirection;
    kinematicController_->SetWalkIncrement(worldDirection * moveSpeed * timeStep);
}

void PlayerController::Update(float timeStep)
{
}

void PlayerController::OnNodeSet(Node *previousNode, Node *currentNode)
{
    auto renderer = GetSubsystem<Renderer>();

    cameraNode_ = node_->CreateChild("Camera");
    auto camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFov(60.0f);
    const auto controller = cameraNode_->CreateComponent<FreeFlyController>();
    controller->SetSpeed(0.0f);
    controller->SetAcceleratedSpeed(0.0f);

    renderer->SetViewport(0, new Viewport(context_, GetScene(), camera));

    kinematicController_ = node_->CreateComponent<KinematicCharacterController>();
    kinematicController_->SetHeight(1.05f);
    kinematicController_->SetDiameter(0.35f);
    kinematicController_->SetOffset(Vector3(0.0f, 0.9f, 0.0f));
    kinematicController_->SetStepHeight(0.05f);

    cameraNode_->SetPosition(Vector3::UP * 1.3f);
}