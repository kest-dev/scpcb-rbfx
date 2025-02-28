#pragma once

#include <Urho3D/Input/MoveAndOrbitComponent.h>
#include <Urho3D/Physics/KinematicCharacterController.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Input/InputMap.h>

using namespace Urho3D;

class PlayerController : public MoveAndOrbitComponent
{
    URHO3D_OBJECT(PlayerController, MoveAndOrbitComponent);

public:
    explicit PlayerController(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    void DelayedStart() override;

    void FixedUpdate(float timeStep) override;

    void Update(float timeStep) override;

private:
    float jumpInterval_{ 0.3f };
    float jumpCooldown_{};
    float shake_{};
    float sprint_{};
    float crouchState_{};
    float injuries_{3.0f};
    float mouseSensitivity_;

    Vector3 mouseMovement_;

    SharedPtr<Node> cameraNode_;
    KinematicCharacterController* kinematicController_;
    SharedPtr<InputMap> inputMap_;
};
