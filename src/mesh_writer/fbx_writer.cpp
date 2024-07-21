#include <stdexcept>
#include <filesystem>
#include <cassert>

#include <plateau/mesh_writer/fbx_writer.h>

#include <fbxsdk.h>
#include <set>

namespace fs = std::filesystem;

namespace {
    void copyTexture(const std::string& fbx_path, const std::string& texture_url) {
        auto src_path = fs::u8path(texture_url);
        auto t_path = src_path.filename();
        auto dst_path = fs::u8path(fbx_path).parent_path();
        dst_path /= src_path.parent_path().filename();
        create_directories(dst_path);
        dst_path /= t_path;

        constexpr auto copy_options = fs::copy_options::skip_existing;
        copy(src_path, dst_path, copy_options);
    }
}

namespace plateau::meshWriter {
    class FbxWriterImpl {
    public:
        FbxWriterImpl()
            : manager_(FbxManager::Create()) {
        }

        ~FbxWriterImpl() {
            manager_->Destroy();
        }

        bool write(const std::string& fbx_file_path, const plateau::polygonMesh::Model& model, const FbxWriteOptions& options) {
            FbxIOSettings* ios = FbxIOSettings::Create(manager_, IOSROOT);
            manager_->SetIOSettings(ios);
            ios->SetBoolProp(EXP_FBX_MATERIAL, true);
            ios->SetBoolProp(EXP_FBX_TEXTURE, true);
            ios->SetBoolProp(EXP_FBX_SHAPE, true);
            ios->SetBoolProp(EXP_FBX_GOBO, true);
            ios->SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);
            ios->SetBoolProp(EXP_ASCIIFBX, options.file_format == FbxFileFormat::ASCII);
            const auto fbx_scene = FbxScene::Create(manager_, "");
            FbxAxisSystem axis_system;
            switch (options.coordinate_system) {
            case geometry::CoordinateSystem::ENU:
                axis_system = FbxAxisSystem(
                    FbxAxisSystem::EUpVector::eZAxis,
                    FbxAxisSystem::EFrontVector::eParityEven,
                    FbxAxisSystem::eRightHanded);
                break;
            case geometry::CoordinateSystem::ESU:
                axis_system = FbxAxisSystem(
                    FbxAxisSystem::EUpVector::eZAxis,
                    FbxAxisSystem::EFrontVector::eParityEven,
                    FbxAxisSystem::eLeftHanded);
                break;
            case geometry::CoordinateSystem::WUN:
                axis_system = FbxAxisSystem(
                    FbxAxisSystem::EUpVector::eYAxis,
                    FbxAxisSystem::EFrontVector::eParityEven,
                    FbxAxisSystem::eRightHanded);
                break;
            case geometry::CoordinateSystem::EUN:
                axis_system = FbxAxisSystem(
                    FbxAxisSystem::EUpVector::eYAxis,
                    FbxAxisSystem::EFrontVector::eParityEven,
                    FbxAxisSystem::eLeftHanded);
                break;
            }
            axis_system.ConvertScene(fbx_scene);

            // create scene info
            FbxDocumentInfo* SceneInfo = FbxDocumentInfo::Create(manager_, "SceneInfo");
            fbx_scene->SetSceneInfo(SceneInfo);

            std::filesystem::path path = std::filesystem::u8path(fbx_file_path);
            if (path.is_relative()) {
                auto pathCurrent = std::filesystem::current_path();
                pathCurrent /= path;
                pathCurrent.swap(path);
            }

            std::filesystem::path pathFile = path.filename();

            for (int i = 0; i < model.getRootNodeCount(); ++i) {
                const auto& node = model.getRootNodeAt(i);
                processNodeRecursive(node, fbx_scene->GetRootNode(), fbx_scene);
            }

            const auto exporter = FbxExporter::Create(manager_, "");

            int FileFormat =
                options.file_format == FbxFileFormat::ASCII
                ? manager_->GetIOPluginRegistry()->FindWriterIDByDescription("FBX ascii (*.fbx)")
                : manager_->GetIOPluginRegistry()->GetNativeWriterFormat();

            if (!exporter->Initialize(fbx_file_path.c_str(), FileFormat, manager_->GetIOSettings())) {
                return false;
            }
            if (!exporter->Export(fbx_scene)) {
                return false;
            }
            exporter->Destroy();

            // テクスチャファイルコピー
            for (const auto& texture : required_textures_) {
                copyTexture(fs::absolute(path).u8string(), texture);
            }

            return true;
        }

        void processNodeRecursive(const polygonMesh::Node& node, FbxNode* parent_fbx_node, FbxScene* fbx_scene) {
            const auto fbx_node = FbxNode::Create(fbx_scene, node.getName().c_str());
            parent_fbx_node->AddChild(fbx_node);

            // ノード位置をローカル座標で指定
            auto local_pos = node.getLocalPosition();
            fbx_node->LclTranslation.Set(FbxDouble3(local_pos.x, local_pos.y, local_pos.z));

            // ノードのローカルスケールを指定
            auto local_scale = node.getLocalScale();
            fbx_node->LclScaling.Set(FbxDouble3(local_scale.x, local_scale.y, local_scale.z));

            // ノードのローカル向きを指定
            // 参考 : https://stackoverflow.com/questions/47753266/fbxsdk-using-quaternions-to-set-rotation-keys
            auto local_rotation = node.getLocalRotation();
            auto local_quaternion = FbxQuaternion(local_rotation.getX(), local_rotation.getY(), local_rotation.getZ(), local_rotation.getW());
            FbxAMatrix rotation_matrix;
            rotation_matrix.SetQ(local_quaternion);
            auto rotation_euler = rotation_matrix.GetR();
            fbx_node->LclRotation.Set(rotation_euler);

            const auto mesh = node.getMesh();
            if (mesh != nullptr)
                addMesh(*mesh, fbx_scene, fbx_node);

            for (size_t i = 0; i < node.getChildCount(); ++i) {
                const auto& child_node = node.getChildAt(i);
                processNodeRecursive(child_node, fbx_node, fbx_scene);
            }
        }

        void addMesh(const polygonMesh::Mesh& mesh, FbxScene* fbx_scene, FbxNode* fbx_node) {
            const auto fbx_mesh = FbxMesh::Create(fbx_scene, "");

            // Create control points.
            unsigned VertCount(mesh.getVertices().size());
            fbx_mesh->InitControlPoints(VertCount);
            FbxVector4* ControlPoints = fbx_mesh->GetControlPoints();

            // Set the normals on Layer 0.
            FbxLayer* Layer = fbx_mesh->GetLayer(0);
            if (Layer == nullptr) {
                fbx_mesh->CreateLayer();
                Layer = fbx_mesh->GetLayer(0);
            }

            FbxLayerElementNormal* LayerElementNormal = FbxLayerElementNormal::Create(fbx_mesh, "");

            LayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);

            // Set the normal values for every control point.
            LayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);

            // Create UV for Diffuse channel.
//            FbxLayerElementUV* UVDiffuseLayer = FbxLayerElementUV::Create(fbx_mesh, "DiffuseUV");
//            UVDiffuseLayer->SetMappingMode(FbxLayerElement::eByControlPoint);
//            UVDiffuseLayer->SetReferenceMode(FbxLayerElement::eDirect);
//            Layer->SetUVs(UVDiffuseLayer, FbxLayerElement::eTextureDiffuse);

            std::vector<FbxGeometryElementUV*> UVs = {
                    fbx_mesh->CreateElementUV("DiffuseUV"),
                    fbx_mesh->CreateElementUV("UV2"),
                    fbx_mesh->CreateElementUV("UV3"),
                    fbx_mesh->CreateElementUV("UV4")
            };
            for (auto uv : UVs) {
                uv->SetMappingMode(FbxGeometryElement::eByControlPoint);
                uv->SetReferenceMode(FbxGeometryElement::eDirect);
            }

            for (unsigned VertexIdx = 0; VertexIdx < VertCount; ++VertexIdx) {
                const auto& vertex = mesh.getVertices()[VertexIdx];
                const auto& uv = mesh.getUV1()[VertexIdx];
                ControlPoints[VertexIdx] = FbxVector4(vertex.x, vertex.y, vertex.z);
                UVs.at(0)->GetDirectArray().Add(FbxVector2(uv.x, uv.y));
                UVs.at(1)->GetDirectArray().Add(FbxVector2(0, 0));
                UVs.at(2)->GetDirectArray().Add(FbxVector2(0, 0));

                const auto& src_uv4 = mesh.getUV4();
                if (VertexIdx < src_uv4.size()) {
                    auto& uv4 = src_uv4.at(VertexIdx);
                    UVs.at(3)->GetDirectArray().Add(FbxVector2(uv4.x, uv4.y));
                }

            }

            for (auto uv : UVs) {
                uv->GetIndexArray().SetCount(VertCount);
            }

            // 頂点カラーをセットします。
            auto dst_vertex_colors = fbx_mesh->CreateElementVertexColor();
            dst_vertex_colors->SetMappingMode(FbxGeometryElement::eByControlPoint);
            dst_vertex_colors->SetReferenceMode(FbxGeometryElement::eDirect);
            auto src_vert_colors = mesh.getVertexColors();
            for(auto src_color : src_vert_colors) {
                dst_vertex_colors->GetDirectArray().Add(FbxColor(src_color.r, src_color.g, src_color.b));
            }
            Layer->SetVertexColors(dst_vertex_colors);

            // Build list of Indices re-used multiple times to lookup Normals, UVs, other per face vertex information
            const auto& indices = mesh.getIndices();

            FbxLayerElementMaterial* MatLayer = FbxLayerElementMaterial::Create(fbx_mesh, "");
            MatLayer->SetMappingMode(FbxLayerElement::eByPolygon);
            MatLayer->SetReferenceMode(FbxLayerElement::eIndexToDirect);
            Layer->SetMaterials(MatLayer);

            const auto& sub_meshes = mesh.getSubMeshes();
            for (const auto& sub_mesh : sub_meshes) {
                FbxSurfaceMaterial* fbx_material;
                const auto& texture_path = sub_mesh.getTexturePath();
                if (texture_path.empty()) {
                    // マテリアル名の末尾は "-(gameMaterialID)" である必要があります。これはUnityの「Assetsへ変換」機能を動かすために必要です。
                    FbxString default_material_name = ("Default-Material-" + std::to_string(sub_mesh.getGameMaterialID())).c_str();
                    fbx_material = fbx_scene->GetMaterial(default_material_name);
                    if (!fbx_material) {
                        fbx_material = FbxSurfacePhong::Create(fbx_scene, default_material_name.Buffer());
                        ((FbxSurfacePhong*)fbx_material)->Diffuse.Set(FbxDouble3(0.72, 0.72, 0.72));
                        ((FbxSurfacePhong*)fbx_material)->DiffuseFactor.Set(1.);
                    }
                } else {
                    FbxString material_name = (fs::u8path(texture_path).filename().replace_extension("").u8string() + "-" + std::to_string(sub_mesh.getGameMaterialID())).c_str();
                    fbx_material = fbx_scene->GetMaterial(material_name); // 同じマテリアルがすでにあればそれを利用します。
                    if(!fbx_material) {
                        // 同じマテリアルがなければ生成します。
                        fbx_material = FbxSurfacePhong::Create(fbx_scene, material_name);
                        FbxProperty FbxColorProperty = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
                        if (FbxColorProperty.IsValid()) {
                            //Create a fbx property
                            FbxFileTexture* lTexture = FbxFileTexture::Create(fbx_scene, fs::u8path(texture_path).filename().u8string().c_str());
                            lTexture->SetFileName(texture_path.c_str());
                            lTexture->SetTextureUse(FbxTexture::eStandard);
                            lTexture->SetMappingType(FbxTexture::eUV);
                            lTexture->ConnectDstProperty(FbxColorProperty);
                            required_textures_.insert(texture_path);
                        }
                    }
                }

                auto material_index = fbx_node->GetMaterialIndex(fbx_material->GetName());
                if(material_index < 0) {
                    material_index = fbx_node->AddMaterial(fbx_material);
                }

                const unsigned triangle_count = (sub_mesh.getEndIndex() - sub_mesh.getStartIndex() + 1) / 3;

                // Copy over the index buffer into the FBX polygons set.
                for (unsigned triangle_index = 0; triangle_index < triangle_count; ++triangle_index) {
                    fbx_mesh->BeginPolygon(material_index);
                    for (unsigned point_index = 0; point_index < 3; point_index++) {
                        const auto vert_index = indices[sub_mesh.getStartIndex() + triangle_index * 3 + point_index];
                        fbx_mesh->AddPolygon(vert_index);
                    }
                    fbx_mesh->EndPolygon();
                }

                fbx_node->SetNodeAttribute(fbx_mesh);
            }
        }

    private:
        FbxManager* manager_;
        std::set<std::string> required_textures_;

    };

    bool FbxWriter::write(const std::string& fbx_file_path, const plateau::polygonMesh::Model& model, const FbxWriteOptions& options) {
        return FbxWriterImpl().write(fbx_file_path, model, options);
    }
}
