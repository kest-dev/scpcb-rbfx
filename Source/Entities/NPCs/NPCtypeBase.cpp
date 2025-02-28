#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include "NPCtypeBase.h"

NPCtypeBase::NPCtypeBase(Context* context)
    : LogicComponent(context)
    , shapeHeight_(8.5f)
    , shapeRadius_(3.3f)
    , shapeOffset_(0.0f, 4.3f, 0.0f)
    , dirtyShape_(false)
{
}

void NPCtypeBase::RegisterObject(Context* context)
{
    context->RegisterFactory<NPCtypeBase>();
}

void NPCtypeBase::Start()
{
    agent_ = node_->CreateComponent<CrowdAgent>();
    agent_->SetHeight(1.0f);
    agent_->SetRadius(0.13f);
    agent_->SetMaxSpeed(2.0f);
    agent_->SetMaxAccel(99.0f);

    controller_ = node_->CreateComponent<AnimationController>();

    auto* body = node_->CreateComponent<RigidBody>();
    body->SetMass(1.0f);
    body->SetFriction(999.0f);
    body->SetAngularFactor(Vector3::ZERO);
    body->SetLinearFactor(Vector3::ZERO);
    body->SetCollisionEventMode(COLLISION_ALWAYS);
    shape_ = node_->CreateComponent<CollisionShape>();
    shape_->SetCapsule(shapeRadius_, shapeHeight_, shapeOffset_);

    SubscribeToEvent(E_CROWD_AGENT_FAILURE, URHO3D_HANDLER(NPCtypeBase, HandleCrowdAgentFailure));

    SubscribeToEvent(E_CROWD_AGENT_REPOSITION, URHO3D_HANDLER(NPCtypeBase, HandleCrowdAgentReposition));

    SubscribeToEvent(E_CROWD_AGENT_FORMATION, URHO3D_HANDLER(NPCtypeBase, HandleCrowdAgentFormation));
}

void NPCtypeBase::Update(float timeStep)
{
    if (dirtyShape_)
    {
        UpdateCollisionShape();
        dirtyShape_ = false;
    }
}

void NPCtypeBase::FixedUpdate(float timeStep)
{
}

void NPCtypeBase::FindPath(const Vector3& position)
{
    auto navMesh = GetScene()->GetComponent<DynamicNavigationMesh>();
    Vector3 pathPos = navMesh->FindNearestPoint(position, Vector3(1.0f, 1.0f, 1.0f));

    GetScene()->GetComponent<CrowdManager>()->SetCrowdTarget(pathPos, node_);
}

void NPCtypeBase::HandleCrowdAgentFailure(StringHash eventType, VariantMap& eventData)
{

}

void NPCtypeBase::HandleCrowdAgentFormation(StringHash eventType, VariantMap& eventData)
{
    using namespace CrowdAgentFormation;

    unsigned index = eventData[P_INDEX].GetUInt();
    unsigned size = eventData[P_SIZE].GetUInt();
    Vector3 position = eventData[P_POSITION].GetVector3();

    // The first agent will always move to the exact position, all other agents will select a random point nearby
    if (index)
    {
        auto* crowdManager = static_cast<CrowdManager*>(GetEventSender());
        auto* agent = static_cast<CrowdAgent*>(eventData[P_CROWD_AGENT].GetPtr());
        eventData[P_POSITION] = crowdManager->GetRandomPointInCircle(position, agent->GetRadius(), agent->GetQueryFilterType());
    }
}

void NPCtypeBase::HandleCrowdAgentReposition(StringHash eventType, VariantMap& eventData)
{
    using namespace CrowdAgentReposition;

    auto* node = static_cast<Node*>(eventData[P_NODE].GetPtr());
    auto* agent = static_cast<CrowdAgent*>(eventData[P_CROWD_AGENT].GetPtr());
    Vector3 velocity = eventData[P_VELOCITY].GetVector3();
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    speed_ = velocity.Length();

    float speedRatio = speed_ / agent->GetMaxSpeed();
    // Face the direction of its velocity but moderate the turning speed based on the speed ratio and timeStep
    node->SetRotation(node->GetRotation().Slerp(Quaternion(Vector3::FORWARD, velocity), 10.0f * timeStep * speedRatio));
}

void NPCtypeBase::SetCollisionHeight(float height)
{
    shapeHeight_ = height;
    dirtyShape_ = true;
}

void NPCtypeBase::SetCollisionRadius(float radius)
{
    shapeRadius_ = radius;
    dirtyShape_ = true;
}

void NPCtypeBase::UpdateCollisionShape()
{
    shape_->SetCapsule(shapeRadius_, shapeHeight_, shapeOffset_);
}