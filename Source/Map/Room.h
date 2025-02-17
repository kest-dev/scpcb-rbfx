#pragma once

#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/VirtualFileSystem.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Core/CoreEvents.h>

#include "RMesh.h"

using namespace Urho3D;

class Room : public Component
{
    // Enable type information.
    URHO3D_OBJECT(Room, Component);

public:
    explicit Room(Context* context);

    static void RegisterObject(Context* context);

    ~Room();

    bool LoadXML(const ea::string& path);

    SharedPtr<XMLFile> roomXML_;
protected:
    virtual void FillRoom() {};

    virtual void UpdateEvent(VariantMap &eventData) {};

    const float ROOMSCALE = 8.0f / 2048.0f;

    float eventState_[3];

    int probability_;
    ea::string name_;
    ea::string shape_;
    StringVector zone_;
    StringVector rmesh_;
    ea::string roomClass_;

    Component* roomComp_;

    Vector2 gridPos_;
private:
    void LoadRMesh();
};