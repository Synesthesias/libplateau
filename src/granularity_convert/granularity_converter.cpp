#include <plateau/granularity_convert/granularity_converter.h>
#include <plateau/granularity_convert/convert_to_atomic.h>
#include <plateau/granularity_convert/convert_from_atomic_to_primary.h>
#include <plateau/granularity_convert/merge_primary_node_and_children.h>
#include "plateau/granularity_convert/convert_from_atomic_to_area.h"
#include <plateau/granularity_convert/convert_from_atomic_to_material_in_primary.h>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;


    Model GranularityConverter::convert(const Model& src, GranularityConvertOption option) const {
        // 組み合わせの数を減らすため、まず最小地物に変換してから望みの粒度に変換します。
        auto to_atomic = std::make_shared<ConvertToAtomic>();
        auto converters = std::vector<std::shared_ptr<IModelConverter>>{to_atomic};

        switch(option.granularity_) {
            case ConvertGranularity::PerAtomicFeatureObject:
                break;
            case ConvertGranularity::PerPrimaryFeatureObject: {
                auto to_primary = std::make_shared<ConvertFromAtomicToPrimary>();
                converters.push_back(to_primary);
                break;
            }
            case ConvertGranularity::PerCityModelArea: {
                auto to_area = std::make_shared<ConvertFromAtomicToArea>();
                converters.push_back(to_area);
                break;
            }
            case ConvertGranularity::MaterialInPrimary: {
                auto to_material_in_primary = std::make_shared<ConvertFromAtomicToMaterialInPrimary>();
                converters.push_back(to_material_in_primary);
            }
        }

        // convertersを順番に実行します。
        auto* next_src = &src;
        auto next_dst = Model();
        for(const auto& converter : converters) {
            // ここで変換
            next_dst = converter->convert(next_src);
            next_src = &next_dst;
        }
        next_dst.assignNodeHierarchy();
        return next_dst;
    }
}
