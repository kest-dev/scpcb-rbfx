#pragma once

#include "NPCtypeBase.h"

enum class MTFStates
{
    None,
    Calm,
    Pursuit,
    Containment
};

class NPCtypeMTF : public NPCtypeBase
{
    URHO3D_OBJECT(NPCtypeMTF, NPCtypeBase)

public:
    explicit NPCtypeMTF(Context* context);

    static void RegisterObject(Context* context);

    void Start() override;

    void Update(float timeStep) override;

    void FixedUpdate(float timeStep) override;

private:
    MTFStates states_;
};
