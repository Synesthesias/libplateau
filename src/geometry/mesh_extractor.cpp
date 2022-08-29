#include <plateau/geometry/mesh_extractor.h>
#include <plateau/io/primary_city_object_types.h>
#include <plateau/io/polar_to_plane_cartesian.h>
#include "citygml/tesselator.h"
#include "citygml/texture.h"
#include "plateau/geometry/grid_merger.h"
#include "plateau/geometry/geometry_utils.h"
#include <plateau/io/obj_writer.h>

using namespace plateau::geometry;

std::shared_ptr<Model> MeshExtractor::extract(const CityModel &cityModel, const MeshExtractOptions &options) {
    auto modelPtr = extract_to_row_pointer(cityModel, options);
    auto sharedModel = std::unique_ptr<Model>(modelPtr);
    return sharedModel;
}

Model *MeshExtractor::extract_to_row_pointer(const CityModel &cityModel, const MeshExtractOptions &options) {
    if(options.maxLOD < options.minLOD) throw std::exception("Invalid LOD range.");

    auto model = new Model();
    model->addNode(Node(std::string("ModelRoot")));
    auto& rootNode = model->getRootNodeAt(0);
    // rootNode の子に LODノード を作ります。
    for(unsigned lod = options.minLOD; lod <= options.maxLOD; lod++){
        auto lodNode = Node("LOD" + std::to_string(lod));

        // LODノードの下にメッシュ配置用ノードを作ります。
        switch(options.meshGranularity) {
            case MeshGranularity::PerCityModelArea: {
                // 次のような階層構造を作ります:
                // rootNode -> LODノード -> グリッドごとのノード

                // 都市をグリッドに分け、グリッドごとにメッシュをマージします。
                auto result = GridMerger::gridMerge(cityModel, options, lod);
                int i = 0;
                for (auto &mesh: result) {
                    auto node = Node("grid" + std::to_string(i), std::move(mesh));
                    lodNode.addChildNode(std::move(node));
                    i++;
                }
            }
                break;
            case MeshGranularity::PerPrimaryFeatureObject:{
                // 次のような階層構造を作ります：
                // rootNode -> LODノード -> 主要地物ごとのノード

                auto& primaryCityObjs = cityModel.getAllCityObjectsOfType(PrimaryCityObjectTypes::getPrimaryTypeMask());

                // 主要地物ごとにメッシュを結合
                for(auto primaryObj : primaryCityObjs){
                    // 主要地物の結合
                    auto mesh = Mesh(primaryObj->getId());
                    mesh.mergePolygonsInCityObject(*primaryObj, lod, options, TVec2f{0, 0}, TVec2f{0, 0});
                    if(lod >= 2){
                        // 主要地物の子である最小値物の結合
                        auto atomicObjs = GeometryUtils::getChildCityObjectsRecursive(*primaryObj);
                        mesh.mergePolygonsInCityObjects(atomicObjs, lod, TVec2f{0, 0}, options, TVec2f{0, 0});
                    }
                    lodNode.addChildNode(Node(primaryObj->getId(), std::move(mesh)));
                }
            }
                break;
            case MeshGranularity::PerAtomicFeatureObject:{
                // 次のような階層構造を作ります：
                // rootNode -> LODノード -> 主要地物ごとのノード -> その子の最小地物ごとのノード
                auto& primaryCityObjs = cityModel.getAllCityObjectsOfType(PrimaryCityObjectTypes::getPrimaryTypeMask());
                for(auto primaryObj : primaryCityObjs){
                    // 主要地物のノードを作成します。
                    // 主要地物のノードに主要地物のメッシュを含むべきかどうかは状況により異なります。
                    // LOD2以上である建物は、子の最小地物に必要なメッシュが入ります。よって主要地物にもメッシュを含めるとダブるため含めません。
                    auto primaryMesh = std::optional<Mesh>(std::nullopt);
                    bool shouldContainPrimaryMesh =
                            !(lod >= 2 &&
                              (primaryObj->getType() & CityObject::CityObjectsType::COT_Building) != (CityObject::CityObjectsType)0);
                    if(shouldContainPrimaryMesh) {
                        primaryMesh = Mesh(primaryObj->getId());
                        primaryMesh->mergePolygonsInCityObject(*primaryObj, lod, options, TVec2f{0, 0}, TVec2f{0, 0});
                    }
                    auto primaryNode = Node(primaryObj->getId(), std::move(primaryMesh));
                    // 最小地物ごとにノードを作成します。
                    auto atomicObjs = GeometryUtils::getChildCityObjectsRecursive(*primaryObj);
                    for(auto atomicObj : atomicObjs){
                        // 最小地物のノードを作成
                        auto atomicMesh = Mesh(atomicObj->getId());
                        atomicMesh.mergePolygonsInCityObject(*atomicObj, lod, options, TVec2f{0, 0}, TVec2f{0, 0});
                        auto atomicNode = Node(atomicObj->getId(), std::move(atomicMesh));
                        primaryNode.addChildNode(std::move(atomicNode));
                    }
                    lodNode.addChildNode(std::move(primaryNode));
                }

            }
                break;
            default:
                throw std::exception("Unknown enum type of options.meshGranularity .");
        }

        rootNode.addChildNode(std::move(lodNode));
    }

    return model;
}

CityObjectWithImportID::CityObjectWithImportID(const CityObject *const cityObject, int primaryImportID, int secondaryImportID) :
    cityObject(cityObject),
    primaryImportID(primaryImportID),
    secondaryImportID(secondaryImportID) {
}
