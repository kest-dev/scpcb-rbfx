#include "Room.h"

Room::Room(Context* context)
	: Component(context)
	, probability_(0)
{
    RMesh::RegisterObject(context);
}

void Room::RegisterObject(Context *context)
{
    if(!context->IsReflected<Room>())
        context->AddFactoryReflection<Room>();
}

Room::~Room()
{
}

bool Room::LoadXML(const ea::string& path)
{
	roomXML_ = MakeShared<XMLFile>(context_);
	if (!roomXML_->LoadFile(path))
    {
        URHO3D_LOGERROR("Failed to load: " + path + " room!");
        return false;
    }

    XMLElement root = roomXML_->GetRoot();
    XMLElement roomName = root.GetChild("room");
    name_ = roomName.GetAttribute("name");

    XMLElement roomShape = root.GetChild("roomshape");
    shape_ = roomShape.GetAttribute("shape");

    XMLElement roomProbability = root.GetChild("probability");
    probability_ = roomProbability.GetInt("value");

    XMLElement zone = root.GetChild("zone");
    while (zone)
    {
        ea::string name = zone.GetAttribute("name");
        URHO3D_LOGDEBUG("Zone: " + name);
        zone_.push_back(name);
        zone = zone.GetNext("zone");
    }

    XMLElement rmesh = root.GetChild("rmesh");
    while (rmesh)
    {
        ea::string name = rmesh.GetAttribute("name");
        URHO3D_LOGDEBUG("RMesh: " + name);
        rmesh_.push_back("Rooms/" + name);
        rmesh = rmesh.GetNext("rmesh");
    }

    roomClass_ = roomName.GetAttribute("id");
    URHO3D_LOGDEBUG("Room Class: " + roomClass_);

    //roomComp_ = node_->CreateComponent(roomClass_);
    LoadRMesh();

    roomXML_.Reset();

    return true;
}

void Room::LoadRMesh()
{
    for(ea::string mesh : rmesh_)
    {
        Node *childRoom = node_->CreateChild();
        RMesh *rmesh = childRoom->CreateComponent<RMesh>();
        URHO3D_LOGDEBUG("Loading: " + mesh);
        rmesh->LoadFile("" + mesh);
    }
}