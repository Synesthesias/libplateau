#include <plateau/geometry/mesh_extractor.h>
#include <plateau/io/primary_city_object_types.h>
#include <plateau/io/polar_to_plane_cartesian.h>
#include "citygml/tesselator.h"
#include "citygml/texture.h"
#include "plateau/geometry/grid_merger.h"
#include <plateau/io/obj_writer.h>

using namespace plateau::geometry;


std::shared_ptr<Model> MeshExtractor::extract(const CityModel &cityModel, const MeshExtractOptions &options) const {
    auto modelPtr = extract_to_row_pointer(cityModel, options);
    auto sharedModel = std::unique_ptr<Model>(modelPtr);
    return sharedModel; // TODO これはmoveにしたほうが良いのか？
}

Model *MeshExtractor::extract_to_row_pointer(const CityModel &cityModel, const MeshExtractOptions &options) const {
    auto model = new Model();
    auto& rootNode = model->addNode(Node(std::string("ModelRoot")));
    // TODO optionsに応じた処理の切り替えは未実装
    switch(options.meshGranularity) {
        case MeshGranularity::PerCityModelArea:
            auto result = GridMerger::gridMerge(cityModel, options);

            int i = 0;
            for (auto &mesh: result) {
                auto node = Node("grid" + std::to_string(i), mesh); // TODO 本当は move で meshを渡したい
                rootNode.addChildNode(node); // TODO 本当は move でnodeを渡したい
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
