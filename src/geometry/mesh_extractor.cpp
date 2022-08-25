#include <plateau/geometry/mesh_extractor.h>
#include <plateau/io/primary_city_object_types.h>
#include <plateau/io/polar_to_plane_cartesian.h>
#include "citygml/tesselator.h"
#include "citygml/texture.h"
#include "plateau/geometry/grid_merger.h"
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
    // TODO optionsに応じた処理の切り替えは未実装
    switch(options.meshGranularity) {
        case MeshGranularity::PerCityModelArea:

            // 都市をグリッドに分け、グリッドごとにメッシュをマージします。
            auto result = GridMerger::gridMerge(cityModel, options);

            // 次のような階層構造を作ります:
            // rootNode -> 子(複数) -> グリッドごとのノード -> 所有 -> グリッドメッシュ
            int i = 0;
            for (auto &mesh: result) {
                auto node = Node("grid" + std::to_string(i), mesh); // TODO meshの渡し方、コピーになってるけど効率良い渡し方があるのでは？
                rootNode.addChildNode(node); // TODO nodeの渡し方、コピーになってるけど効率良い渡し方があるのでは？
                i++;
            }
            break;
    }
    return model;
}

CityObjectWithImportID::CityObjectWithImportID(const CityObject *const cityObject, int primaryImportID, int secondaryImportID) :
    cityObject(cityObject),
    primaryImportID(primaryImportID),
    secondaryImportID(secondaryImportID) {
}
