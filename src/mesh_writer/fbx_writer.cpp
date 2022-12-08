#include <stdexcept>
#include <vector>
#include <filesystem>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/polygon.h>
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

            const auto& mesh = node.getMesh();
            if (mesh.has_value())
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
            FbxLayerElementUV* UVDiffuseLayer = FbxLayerElementUV::Create(fbx_mesh, "DiffuseUV");
            UVDiffuseLayer->SetMappingMode(FbxLayerElement::eByControlPoint);
            UVDiffuseLayer->SetReferenceMode(FbxLayerElement::eDirect);
            Layer->SetUVs(UVDiffuseLayer, FbxLayerElement::eTextureDiffuse);

            for (unsigned VertexIdx = 0; VertexIdx < VertCount; ++VertexIdx) {
                const auto& vertex = mesh.getVertices()[VertexIdx];
                const auto& uv = mesh.getUV1()[VertexIdx];
                ControlPoints[VertexIdx] = FbxVector4(vertex.x, vertex.y, vertex.z);
                UVDiffuseLayer->GetDirectArray().Add(FbxVector2(uv.x, uv.y));
            }

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
                    FbxString default_material_name = "Default-Material";
                    fbx_material = fbx_scene->GetMaterial(default_material_name);
                    if (!fbx_material) {
                        fbx_material = FbxSurfacePhong::Create(fbx_scene, default_material_name.Buffer());
                        ((FbxSurfacePhong*)fbx_material)->Diffuse.Set(FbxDouble3(0.72, 0.72, 0.72));
                        ((FbxSurfacePhong*)fbx_material)->DiffuseFactor.Set(1.);
                    }
                } else {
                    FbxString material_name = fs::u8path(texture_path).filename().replace_extension("").u8string().c_str();
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

                const int material_index = fbx_node->AddMaterial(fbx_material);

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
