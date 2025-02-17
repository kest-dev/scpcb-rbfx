#pragma once

#include <Urho3D/IO/File.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Graphics/IndexBuffer.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/IO/VirtualFileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Audio/Sound.h>

using namespace Urho3D;


struct Brush
{
    Vector3 Vertex;
    Vector3 Edges[2];

    Vector3 Normal;

    Vector2 UV1;
    Vector2 UV2;
    Color Colors;

    Vector4 Tangent;
};

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned short ushort;

class RMesh : public Component
{
    // Enable type information.
    URHO3D_OBJECT(RMesh, Component);

public:
    explicit RMesh(Context* context);

    static void RegisterObject(Context* context);

    ~RMesh();

    /// Load the RMesh file. Returns if successful.
    bool LoadFile(ea::string path);

    ea::vector<SharedPtr<Model>> GetRMeshModels() { return models_; }

    const float ROOMSCALE = 8.0f / 2048.0f;

private:
    /// Reads Blitz3D specific strings.
    ea::string ReadBlitzString(AbstractFilePtr src);

    Color ConvertStringToColor(ea::string color, float intensity);

    ea::string ambience[13] = {
        "Sounds/Ambient/Room ambience/rumble.ogg",
        "Sounds/Ambient/Room ambience/lowdrone.ogg",
        "Sounds/Ambient/Room ambience/pulsing.ogg",
        "Sounds/Ambient/Room ambience/ventilation.ogg",
        "Sounds/Ambient/Room ambience/drip.ogg",
        "Sounds/Alarm/Alarm.ogg",
        "Sounds/Ambient/Room ambience/895.ogg",
        "Sounds/Ambient/Room ambience/fuelpump.ogg",
        "Sounds/Ambient/Room ambience/Fan.ogg",
        "Sounds/Ambient/Room ambience/servers1.ogg",
        "Sounds/Ambient/Room ambience/feuer.ogg",
        "Sounds/Ambient/Room ambience/drips.ogg",
        "Sounds/Ambient/Room ambience/broken_fan.ogg"
    };

    /// Bounding box.
    BoundingBox boundingBox_;

    /// Room brushes.
    ea::vector<Brush> brush_;

    /// Vertex buffers.
    ea::vector<SharedPtr<VertexBuffer>> vertexBuffers_;

    /// Index buffers.
    ea::vector<SharedPtr<IndexBuffer>> indexBuffers_;

    /// Geometries.
    ea::vector<SharedPtr<Geometry>> geometries_;

    // Vertex data.
    ea::vector<float> vertexData_;

    // Index data.
    ea::vector<ushort> indexData_;

    // RMesh sub-models.
    ea::vector<SharedPtr<Model>> models_;
    ea::vector<StaticModel*> staticModels_;

    // RMesh materials
    ea::vector<SharedPtr<Material>> materials_;
};