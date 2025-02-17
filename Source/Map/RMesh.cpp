#include "RMesh.h"

Color RMesh::ConvertStringToColor(ea::string color, float intensity)
{
    int r, g, b;

    int arg = sscanf(color.c_str(), "%i %i %i", &r, &g, &b);

    float fr, fg, fb;
    fr = (float)r * intensity;
    fg = (float)g * intensity;
    fb = (float)b * intensity;

    Vector3 newColor = Vector3(fr, fg, fb).Normalized();

    return Color(newColor);
}

RMesh::RMesh(Context* context)
	: Component(context)
{
}

void RMesh::RegisterObject(Context *context)
{
    if(!context->IsReflected<RMesh>())
        context->AddFactoryReflection<RMesh>();
}

RMesh::~RMesh() = default;

ea::string RMesh::ReadBlitzString(AbstractFilePtr src)
{
    eastl_size_t strSize;
    strSize = src->ReadUInt();
    ea::string tempStr;
    for (uint i = 0; i < strSize; i++)
    {
        tempStr += src->ReadUByte();
    }
    return tempStr;
}

bool RMesh::LoadFile(ea::string path)
{
    AbstractFilePtr source = GetSubsystem<VirtualFileSystem>()->OpenFile(path, FILE_READ);

    if (!source)
    {
        URHO3D_LOGERROR("Failed to open: " + source->GetName());
        return false;
    }

    //Ensure the file is RMesh
    ea::string fileHeader = ReadBlitzString(source);
    if (!fileHeader.contains("RoomMesh"))
    {
        URHO3D_LOGERROR(source->GetName() + " is not a valid RMesh file!");
        return false;
    }

    int lmIndex = GetScene()->GetNumLightmaps();

    auto cache = GetSubsystem<ResourceCache>();

    //Read the amount of meshes
    unsigned numMeshBuffers = source->ReadUInt();

    //Reserve the space for the mesh buffer count
    vertexBuffers_.reserve(numMeshBuffers);
    indexBuffers_.reserve(numMeshBuffers);
    geometries_.reserve(numMeshBuffers);

    //Define the contents that the mesh contains
    unsigned elements = MASK_POSITION | MASK_NORMAL | MASK_TEXCOORD1 | MASK_TEXCOORD2 | MASK_TANGENT;

    SharedPtr<Material> invMaterial(new Material(context_));
    invMaterial = cache->GetResource<Material>("Materials/Map/invisible_collision.xml");

    for (unsigned i = 0; i < numMeshBuffers; i++)
    {
        SharedPtr<Model> model(new Model(context_));
        model->SetNumGeometries(numMeshBuffers);
        SharedPtr<Material> material(new Material(context_));

        SharedPtr<VertexBuffer> vBuffer(new VertexBuffer(context_));
        SharedPtr<IndexBuffer> iBuffer(new IndexBuffer(context_));
        SharedPtr<Geometry> gBuffer(new Geometry(context_));

        StaticModel* tempModel = node_->CreateComponent<StaticModel>();
        RigidBody* body = node_->CreateComponent<RigidBody>();

        CollisionShape* shape = node_->CreateComponent<CollisionShape>();

        ea::string lmTexture;

        //Retrieve textures and lightmaps
        for (int j = 0; j < 2; j++)
        {
            unsigned char texFlag = source->ReadUByte();
            ea::string rawTextureName = "";

            if (texFlag != 0)
            {
                rawTextureName = ReadBlitzString(source);

                if (!rawTextureName.empty())
                {
                    //Remove the extensions
                    size_t lastIndex = rawTextureName.find_last_of(".");
                    ea::string textureName = rawTextureName.substr(0, lastIndex);

                    //Check whether the texture is a lightmap or map texture
                    if (!textureName.contains("lm"))
                    {
                        material = cache->GetResource<Material>("Materials/Map/" + textureName + ".xml");
                    }
                    else
                    {
                        lmTexture = textureName;
                    }
                }
            }
        }

        //Remove the last directory
        size_t lastIndex = path.find_last_of("/");
        ea::string lightMapPath = path.substr(0, lastIndex);

        //Add the lightmap texture to the scene
        GetScene()->AddLightmap(lightMapPath + "/" + lmTexture + ".png");

        //Collect all the vertices
        unsigned vertices = source->ReadUInt();

        for (uint j = 0; j < vertices; j++)
        {
            Brush brush;

            Vector3 pos = source->ReadVector3();
            Vector2 uv1 = source->ReadVector2();

            Vector2 uv2 = source->ReadVector2();

            //This is useless but it needs read anyways.
            byte r = source->ReadUByte();
            byte g = source->ReadUByte();
            byte b = source->ReadUByte();

            brush.Vertex = pos;
            brush.UV1 = uv1;
            brush.UV2 = uv2;

            brush_.push_back(brush);

            boundingBox_.Merge(pos);
        }

        //Collect the indices.
        unsigned indices = source->ReadUInt();

        for (uint j = 0; j < indices; j++)
        {
            float angle = 0;

            //The ReadUShort/ReadShort freezes the game, so we have to do this.
            ushort tri1 = (ushort)source->ReadInt();
            ushort tri2 = (ushort)source->ReadInt();
            ushort tri3 = (ushort)source->ReadInt();

            Vector3 v1 = brush_[tri1].Vertex;
            Vector3 v2 = brush_[tri2].Vertex;
            Vector3 v3 = brush_[tri3].Vertex;

            brush_[tri1].Normal = Vector3::ZERO;
            brush_[tri2].Normal = Vector3::ZERO;
            brush_[tri3].Normal = Vector3::ZERO;

            Vector3 edge1 = v2 - v1;
            Vector3 edge2 = v3 - v1;

            brush_[tri1].Edges[0] = edge1;
            brush_[tri1].Edges[1] = edge2;

            brush_[tri2].Edges[0] = edge1;
            brush_[tri2].Edges[1] = edge2;

            brush_[tri3].Edges[0] = edge1;
            brush_[tri3].Edges[1] = edge2;

            brush_[tri1].Normal = edge1.CrossProduct(edge2);
            brush_[tri2].Normal = edge1.CrossProduct(edge2);
            brush_[tri3].Normal = edge1.CrossProduct(edge2);

            brush_[tri1].Normal.Normalize();
            brush_[tri2].Normal.Normalize();
            brush_[tri3].Normal.Normalize();

            indexData_.push_back(tri1);
            indexData_.push_back(tri2);
            indexData_.push_back(tri3);

            //Calculate tangents and bi-tangents for normal maps
            //Taken from here: http://www.terathon.com/code/tangent.html
            {
                auto* tan1 = new Vector3[vertices * 2];
                Vector3* tan2 = tan1 + vertices;

                const Vector2& w1 = brush_[tri1].UV1;
                const Vector2& w2 = brush_[tri2].UV1;
                const Vector2& w3 = brush_[tri3].UV1;

                float x1 = v2.x_ - v1.x_;
                float x2 = v3.x_ - v1.x_;
                float y1 = v2.y_ - v1.y_;
                float y2 = v3.y_ - v1.y_;
                float z1 = v2.z_ - v1.z_;
                float z2 = v3.z_ - v1.z_;

                float s1 = w2.x_ - w1.x_;
                float s2 = w3.x_ - w1.x_;
                float t1 = w2.y_ - w1.y_;
                float t2 = w3.y_ - w1.y_;

                float r = 1.0f / (s1 * t2 - s2 * t1);
                Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
                Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

                tan1[tri1] += sdir;
                tan1[tri2] += sdir;
                tan1[tri3] += sdir;

                tan2[tri1] += tdir;
                tan2[tri2] += tdir;
                tan2[tri3] += tdir;

                const Vector3& n1 = brush_[tri1].Vertex;
                const Vector3& n2 = brush_[tri2].Vertex;
                const Vector3& n3 = brush_[tri3].Vertex;
                const Vector3& ta1 = tan1[tri1];
                const Vector3& ta2 = tan1[tri2];
                const Vector3& ta3 = tan1[tri3];
                Vector3 xyz1;
                Vector3 xyz2;
                Vector3 xyz3;
                float wa1;
                float wa2;
                float wa3;

                // Gram-Schmidt orthogonalize
                xyz1 = (ta1 - n1 * n1.DotProduct(ta1)).Normalized();
                xyz2 = (ta2 - n2 * n2.DotProduct(ta2)).Normalized();
                xyz3 = (ta3 - n3 * n3.DotProduct(ta3)).Normalized();

                // Calculate handedness
                wa1 = n1.CrossProduct(ta1).DotProduct(tan2[tri1]) < 0.0f ? -1.0f : 1.0f;
                wa2 = n2.CrossProduct(ta2).DotProduct(tan2[tri2]) < 0.0f ? -1.0f : 1.0f;
                wa3 = n3.CrossProduct(ta3).DotProduct(tan2[tri3]) < 0.0f ? -1.0f : 1.0f;

                brush_[tri1].Tangent = xyz1.ToVector4(wa1);
                brush_[tri2].Tangent = xyz2.ToVector4(wa2);
                brush_[tri3].Tangent = xyz3.ToVector4(wa3);

                delete[] tan1;
            }
        }

        for (int j = 0; j < brush_.size(); j++)
        {
            //Upload the vertex positions
            vertexData_.push_back(brush_[j].Vertex.x_);
            vertexData_.push_back(brush_[j].Vertex.y_);
            vertexData_.push_back(brush_[j].Vertex.z_);

            //Upload the normals
            vertexData_.push_back(brush_[j].Normal.x_);
            vertexData_.push_back(brush_[j].Normal.y_);
            vertexData_.push_back(brush_[j].Normal.z_);

            //Update the UV Coords
            vertexData_.push_back(brush_[j].UV1.x_);
            vertexData_.push_back(brush_[j].UV1.y_);

            vertexData_.push_back(brush_[j].UV2.x_);
            vertexData_.push_back(brush_[j].UV2.y_);

            //Upload the Tangents and Bi-Tangents
            vertexData_.push_back(brush_[j].Tangent.x_);
            vertexData_.push_back(brush_[j].Tangent.y_);
            vertexData_.push_back(brush_[j].Tangent.z_);
            vertexData_.push_back(brush_[j].Tangent.w_);
        }

        //Add the vertex data to the vertex buffer
        vBuffer->SetShadowed(true);
        vBuffer->SetSize(vertices, elements);
        vBuffer->Update(&vertexData_[0]);
        vertexBuffers_.push_back(vBuffer);

        //Add the index data to the index buffer
        iBuffer->SetShadowed(true);
        iBuffer->SetSize(indices * 3, false);
        iBuffer->Update(&indexData_[0]);
        indexBuffers_.push_back(iBuffer);

        //Set the geometry vertex buffer and index buffer
        gBuffer->SetVertexBuffer(0, vBuffer);
        gBuffer->SetIndexBuffer(iBuffer);
        gBuffer->SetDrawRange(TRIANGLE_LIST, 0, indices * 3);
        geometries_.push_back(gBuffer);

        //Finally, set the data to the model
        model->SetGeometry(i, 0, gBuffer);
        model->SetNumGeometryLodLevels(i, 1);
        model->SetBoundingBox(boundingBox_);
        models_.push_back(model);

        //Apply the model to the collision shape
        shape->SetTriangleMesh(model);

        //Apply the model and map texture to the static model
        tempModel->SetOccluder(true);
        tempModel->SetModel(model);
        tempModel->SetMaterial(material);
        tempModel->SetLightMask(0x0);

        //Apply light map support
        tempModel->SetGlobalIlluminationType(GlobalIlluminationType::UseLightMap);
        tempModel->SetBakeLightmap(true);
        tempModel->SetLightmapIndex(i + lmIndex);

        materials_.push_back(material);
        staticModels_.push_back(tempModel);

        //Clean the data
        vertexData_.clear();
        indexData_.clear();
        brush_.clear();
    }

    int invisNumMeshBuffers = source->ReadInt();
    for (unsigned i = 0; i < invisNumMeshBuffers; i++)
    {
        SharedPtr<Model> model(new Model(context_));
        model->SetNumGeometries(numMeshBuffers);
        SharedPtr<Material> material(new Material(context_));

        SharedPtr<VertexBuffer> vBuffer(new VertexBuffer(context_));
        SharedPtr<IndexBuffer> iBuffer(new IndexBuffer(context_));
        SharedPtr<Geometry> gBuffer(new Geometry(context_));

        StaticModel* tempModel = node_->CreateComponent<StaticModel>();

        CollisionShape* shape = node_->CreateComponent<CollisionShape>();

        //Collect all the vertices
        unsigned vertices = source->ReadUInt();

        for (uint j = 0; j < vertices; j++)
        {
            Brush brush;
            Vector3 pos = source->ReadVector3();
            brush.Vertex = pos;

            brush_.push_back(brush);
            boundingBox_.Merge(pos);
        }

        //Collect the indices.
        unsigned indices = source->ReadUInt();

        for (uint j = 0; j < indices; j++)
        {
            Vector3 normal;

            //The ReadUShort/ReadShort freezes the game, so we have to do this.
            ushort tri1 = (ushort)source->ReadInt();
            ushort tri2 = (ushort)source->ReadInt();
            ushort tri3 = (ushort)source->ReadInt();

            //Calcuate the normals for the polygon
            Vector3 v1 = brush_[tri1].Vertex;
            Vector3 v2 = brush_[tri2].Vertex;
            Vector3 v3 = brush_[tri3].Vertex;

            Vector3 edge1 = v1 - v2;
            Vector3 edge2 = v1 - v3;
            normal = edge1.CrossProduct(edge2).Normalized();

            brush_[tri1].Normal = normal;
            brush_[tri2].Normal = normal;
            brush_[tri3].Normal = normal;

            indexData_.push_back(tri1);
            indexData_.push_back(tri2);
            indexData_.push_back(tri3);
        }

        for (int j = 0; j < brush_.size(); j++)
        {
            //Upload the vertex positions
            vertexData_.push_back(brush_[j].Vertex.x_);
            vertexData_.push_back(brush_[j].Vertex.y_);
            vertexData_.push_back(brush_[j].Vertex.z_);

            //Upload the normals
            vertexData_.push_back(brush_[j].Normal.x_);
            vertexData_.push_back(brush_[j].Normal.y_);
            vertexData_.push_back(brush_[j].Normal.z_);
        }

        elements = MASK_POSITION | MASK_NORMAL;

        //Add the vertex data to the vertex buffer
        vBuffer->SetShadowed(true);
        vBuffer->SetSize(vertices, elements);
        vBuffer->Update(&vertexData_[0]);
        vertexBuffers_.push_back(vBuffer);

        //Add the index data to the index buffer
        iBuffer->SetShadowed(true);
        iBuffer->SetSize(indices * 3, false);
        iBuffer->Update(&indexData_[0]);
        indexBuffers_.push_back(iBuffer);

        //Set the geometry vertex buffer and index buffer
        gBuffer->SetVertexBuffer(0, vBuffer);
        gBuffer->SetIndexBuffer(iBuffer);
        gBuffer->SetDrawRange(TRIANGLE_LIST, 0, indices * 3);
        geometries_.push_back(gBuffer);

        //Finally, set the data to the model
        model->SetGeometry(i, 0, gBuffer);
        model->SetNumGeometryLodLevels(i, 1);
        model->SetBoundingBox(boundingBox_);
        models_.push_back(model);

        shape->SetTriangleMesh(model);

        //Apply the model and map texture to the static model
        tempModel->SetModel(model);
        tempModel->SetMaterial(invMaterial);

        staticModels_.push_back(tempModel);

        //Clean the data
        vertexData_.clear();
        indexData_.clear();
        brush_.clear();
    }

    ea::string entityType;
    int entCount = source->ReadInt();
    for (int ent = 0; ent < entCount; ent++)
    {
        entityType = ReadBlitzString(source);
        if (entityType == "light")
        {
            Vector3 pos = source->ReadVector3();// *ROOMSCALE;

            float range = source->ReadFloat() / 175.0f;
            ea::string color = ReadBlitzString(source);
            float intensity = Min(source->ReadFloat() * 0.8f, 1.0f);
            Color tColor = ConvertStringToColor(color, intensity);

            auto* lightNode = node_->CreateChild();
            lightNode->SetPosition(pos);
            auto* light = lightNode->CreateComponent<Light>();
            light->SetLightType(LIGHT_POINT);
            light->SetLightMode(LM_REALTIME);
            light->SetSpecularIntensity(0.4f);
            light->SetColor(tColor);
            light->SetRange(range);
            light->SetCastShadows(true);
            light->SetDrawDistance(25);
        }
        else if (entityType == "waypoint")
        {
            Vector3 pos = source->ReadVector3();
        }
        else if (entityType == "screen")
        {
            Vector3 pos = source->ReadVector3();
            ea::string screen = ReadBlitzString(source);
        }
        else if (entityType == "spotlight")
        {
            Vector3 pos = source->ReadVector3();

            float range = source->ReadFloat() / 2000.0f;
            ea::string color = ReadBlitzString(source);
            float intensity = Min(source->ReadFloat() * 0.8f, 1.0f);
            ea::string angle = ReadBlitzString(source);

            int innercone = source->ReadInt();
            int outercone = source->ReadInt();
        }
        else if (entityType == "model")
        {
            //auto* mdlshape = node_->CreateComponent<CollisionShape>();
            ea::string file = ReadBlitzString(source);
            file.replace(".x", "");
            file.replace(".b3d", "");
            Vector3 pos = source->ReadVector3();
            Vector3 rot = source->ReadVector3();
            Quaternion rotation = Quaternion(-rot.x_, rot.y_, rot.z_);
            Vector3 scale = source->ReadVector3();

            Node* propNode = node_->CreateChild();
            propNode->SetPosition(pos);
            propNode->SetRotation(rotation);
            propNode->SetScale(scale);
            StaticModel* prop = propNode->CreateComponent<StaticModel>();
            prop->SetModel(cache->GetResource<Model>("GFX/Models/Map/Props/" + file + ".mdl"));
            prop->SetCastShadows(true);
            //mdlshape->SetTriangleMesh(prop->GetModel());

        }
        else if (entityType == "model_nocoll")
        {
            ea::string file = ReadBlitzString(source);
            file.replace(".x", "");
            file.replace(".b3d", "");
            Vector3 pos = source->ReadVector3();
            Vector3 rot = source->ReadVector3();
            Quaternion rotation = Quaternion(rot.x_, rot.y_, rot.z_);
            Vector3 scale = source->ReadVector3();
        }
        else if (entityType == "soundemitter")
        {
            Vector3 pos = source->ReadVector3();
            int sfx = source->ReadInt();
            float sfxrange = source->ReadFloat();

            Node* sfxEmitter = node_->CreateChild();
            sfxEmitter->SetPosition(pos);
            SoundSource3D* emitter = sfxEmitter->CreateComponent<SoundSource3D>();
            Sound* soundfx = cache->GetResource<Sound>(ambience[sfx - 1]);

            //soundfx->SetLooped(true);

            emitter->SetFarDistance(sfxrange * 1.35f);
            emitter->Play(soundfx);
        }
        else if(entityType == "playerstart")
        {
            Vector3 startPoint = source->ReadVector3();

            ea::string angle = ReadBlitzString(source);

        }
    }

    node_->SetScale(ROOMSCALE);

    URHO3D_LOGINFO(source->GetName() + " has been successfully loaded!");

    source->Close();

    return true;
}