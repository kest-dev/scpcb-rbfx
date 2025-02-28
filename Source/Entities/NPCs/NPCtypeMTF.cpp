#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include "NPCtypeMTF.h"

#include <Urho3D/Audio/Sound.h>

NPCtypeMTF::NPCtypeMTF(Context* context)
    : NPCtypeBase(context)
{
}

void NPCtypeMTF::RegisterObject(Context* context)
{
    context->RegisterFactory<NPCtypeMTF>();
}

void NPCtypeMTF::Start()
{
    auto cache = GetSubsystem<ResourceCache>();

    auto modelNode = node_->CreateChild();

    modelNode->SetPosition(Vector3(0, -0.15f, 0));
    modelNode->SetRotation(Quaternion(-90.0f, 0, 0));
    model_ = modelNode->CreateComponent<AnimatedModel>();
    model_->SetModel(cache->GetResource<Model>("Models/NPCs/MTF2.mdl"));
    model_->SetMaterial(1, cache->GetResource<Material>("Materials/NPCs/MTF_P90_diffuse02.xml"));
    model_->SetMaterial(2, cache->GetResource<Material>("Materials/NPCs/MTF_lens_diffuse01.xml"));
    model_->SetMaterial(3, cache->GetResource<Material>("Materials/NPCs/MTF_newdiffuse02.xml"));

    node_->SetScale(0.29f / 2.5f);

    soundSource_ = node_->CreateComponent<SoundSource3D>();
    auto snd = cache->GetResource<Sound>("Sounds/Character/MTF/Breath.ogg");
    snd->SetLooped(true);
    soundSource_->Play(snd);
    soundSource_->SetFarDistance(3.25f);

    states_ = MTFStates::Calm;

    BaseClassName::Start();
}

void NPCtypeMTF::Update(float timeStep)
{
    BaseClassName::Update(timeStep);
}

void NPCtypeMTF::FixedUpdate(float timeStep)
{
    BaseClassName::FixedUpdate(timeStep);

    if (!controller_)
        return;

    switch (states_)
    {
    case MTFStates::None:
        break;
    case MTFStates::Calm:
        if (GetSpeed() > agent_->GetRadius() + 0.5f)
        {
            if (!controller_->IsPlaying("Animations/MTF2/walking1.ani"))
            {
                controller_->Play("Animations/MTF2/walking1.ani", 0, true, 0.1f);

                float speedRatio = GetSpeed() / agent_->GetMaxSpeed();

                controller_->SetSpeed("Animations/MTF2/walking1.ani", speedRatio);
            }
        }
        else
        {
            controller_->Stop("Animations/MTF2/walking1.ani", 0.5f);

            if (!controller_->IsPlaying("Animations/MTF2/idle1.ani"))
            {
                controller_->Play("Animations/MTF2/idle1.ani", 0, true, 0.1f);
            }


        }

        break;
    case MTFStates::Pursuit:
        break;
    case MTFStates::Containment:
        break;
    }
}