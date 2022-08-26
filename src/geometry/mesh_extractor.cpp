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
    auto model = new Model();
    model->addNode(Node(std::string("ModelRoot")));
    auto& rootNode = model->getRootNodeAt(0);
    switch(options.meshGranularity) {
        case MeshGranularity::PerCityModelArea: {
            // 次のような階層構造を作ります:
            // rootNode -> (複数の子を所有) -> グリッドごとのノード

            // 都市をグリッドに分け、グリッドごとにメッシュをマージします。
            auto result = GridMerger::gridMerge(cityModel, options);
            int i = 0;
            for (auto &mesh: result) {
                auto node = Node("grid" + std::to_string(i), mesh); // TODO meshの渡し方、コピーになってるけど効率良い渡し方があるのでは？
                rootNode.addChildNode(node); // TODO nodeの渡し方、コピーになってるけど効率良い渡し方があるのでは？
                i++;
            }
        }
            break;
        case MeshGranularity::PerPrimaryFeatureObject:{
            // 次のような階層構造を作ります：
            // rootNode -> 主要地物ごとのノード

            auto& primaryCityObjs = cityModel.getAllCityObjectsOfType(PrimaryCityObjectTypes::getPrimaryTypeMask());

            // 主要地物ごとにメッシュを結合
            for(auto primaryObj : primaryCityObjs){
                // 主要地物の結合
                auto mesh = Mesh(primaryObj->getId());
                mesh.mergePolygonsInCityObject(*primaryObj, options, TVec2f{0, 0}, TVec2f{0, 0});
                if(options.maxLOD >= 2){
                    // 主要地物の子である最小値物の結合
                    auto atomicObjs = GeometryUtils::getChildCityObjectsRecursive(*primaryObj);
                    mesh.mergePolygonsInCityObjects(atomicObjs, TVec2f{0,0}, TVec2f{0,0}, options);
                }
                rootNode.addChildNode(Node(primaryObj->getId(), mesh)); // TODO メッシュ、ノードの渡し方？
            }
        }
            break;
        case MeshGranularity::PerAtomicFeatureObject:{
            // 次のような階層構造を作ります：
            // rootNode -> 主要地物ごとのノード -> 主要地物の子に、その子の最小地物ごとのノード
            auto& primaryCityObjs = cityModel.getAllCityObjectsOfType(PrimaryCityObjectTypes::getPrimaryTypeMask());
            for(auto primaryObj : primaryCityObjs){
                // 主要地物のノードを作成します。
                // 主要地物のノードに主要地物のメッシュを含むべきかどうかは状況により異なります。
                // LOD2以上である建物は、子の最小地物に必要なメッシュが入ります。主要地物にもメッシュを含めるとダブるため含めません。
                auto primaryMesh = std::optional<Mesh>(std::nullopt);
                bool shouldContainPrimaryMesh =
                        options.maxLOD < 2 ||
                                static_cast<std::underlying_type<CityObject::CityObjectsType>::type>(primaryObj->getType() | CityObject::CityObjectsType::COT_Building) != 0;
                if(shouldContainPrimaryMesh) {
                    primaryMesh = Mesh(primaryObj->getId());
                    primaryMesh->mergePolygonsInCityObject(*primaryObj, options, TVec2f{0, 0}, TVec2f{0, 0});
                }
                auto primaryNode = Node(primaryObj->getId(), primaryMesh);
                // 最小地物ごとにノードを作成します。
                auto atomicObjs = GeometryUtils::getChildCityObjectsRecursive(*primaryObj);
                for(auto atomicObj : atomicObjs){
                    // 最小地物のノードを作成
                    auto atomicMesh = Mesh(atomicObj->getId());
                    atomicMesh.mergePolygonsInCityObject(*atomicObj, options, TVec2f{0, 0}, TVec2f{0, 0});
                    auto atomicNode = Node(atomicObj->getId(), atomicMesh);
                    primaryNode.addChildNode(atomicNode);
                }
                rootNode.addChildNode(primaryNode);
            }

        }
            break;
        default:
            throw std::exception("Unknown enum type of options.meshGranularity .");
    }
    return model;
}

CityObjectWithImportID::CityObjectWithImportID(const CityObject *const cityObject, int primaryImportID, int secondaryImportID) :
    cityObject(cityObject),
    primaryImportID(primaryImportID),
    secondaryImportID(secondaryImportID) {
}
