#pragma once

#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Navigation/CrowdAgent.h>
#include <Urho3D/Navigation/NavigationEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Navigation/DynamicNavigationMesh.h>

#include <Urho3D/Audio/SoundSource3D.h>

using namespace Urho3D;

class NPCtypeBase : public LogicComponent
{
    URHO3D_OBJECT(NPCtypeBase, LogicComponent)

public:
    explicit NPCtypeBase(Context* context);

    static void RegisterObject(Context* context);

    void Start() override;

    void Update(float timeStep) override;

    void FixedUpdate(float timeStep) override;

    void FindPath(const Vector3& position);

    void SetCollisionHeight(float height);

    void SetCollisionRadius(float radius);

protected:
    SharedPtr<AnimatedModel> model_;
    SharedPtr<SoundSource3D> soundSource_;

    SharedPtr<CrowdAgent> agent_;
    SharedPtr<AnimationController> controller_;

    SharedPtr<CollisionShape> shape_;
    float shapeHeight_;
    float shapeRadius_;
    Vector3 shapeOffset_;

    float GetSpeed() { return speed_; }

private:
    void HandleCrowdAgentFailure(StringHash eventType, VariantMap& eventData);

    void HandleCrowdAgentReposition(StringHash eventType, VariantMap& eventData);

    void HandleCrowdAgentFormation(StringHash eventType, VariantMap& eventData);

    void UpdateCollisionShape();

    float speed_;

    bool dirtyShape_;
};
