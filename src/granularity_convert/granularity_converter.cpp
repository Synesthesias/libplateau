#include <plateau/granularity_convert/granularity_converter.h>
#include <plateau/granularity_convert/convert_to_atomic.h>
#include <plateau/granularity_convert/convert_from_atomic_to_primary.h>
#include <plateau/granularity_convert/merge_primary_node_and_children.h>
#include "plateau/granularity_convert/convert_from_atomic_to_area.h"

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;


    Model GranularityConverter::convert(Model& src, GranularityConvertOption option) {
        // 組み合わせの数を減らすため、まず最小地物に変換してから望みの粒度に変換します。

        // 例：入力のNode構成が次のようだったとして、以下にその変化を例示します。
        // 入力： gml_node <- lod_node <- group_node

        auto atomic = ConvertToAtomic().convert(&src);

        // 例：上の行の実行後、次のようなNode構成になります。
        // gml_node <- lod_node <- primary_node <- atomic_node

        atomic.eraseEmptyNodes();

        switch (option.granularity_) {
            case MeshGranularity::PerAtomicFeatureObject:
                return atomic;
            case MeshGranularity::PerPrimaryFeatureObject:
                return ConvertFromAtomicToPrimary().convert(&atomic);
            case MeshGranularity::PerCityModelArea:
                return ConvertFromAtomicToArea().convert(&atomic);
            default:
                throw std::runtime_error("unknown argument");
        }
    }
}
