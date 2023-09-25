#include <plateau/granularity_convert/granularity_converter.h>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    namespace {
        Model convertToAtomic(const Model& src) {
            // TODO
            Model dst = Model();
            return dst;
        }

        /// srcが最小地物単位であることを前提に、optionに沿ってポリゴンをマージします。
        Model merge(const Model& src, GranularityConvertOption option) {
            // TODO
            Model dst = Model();
            return dst;
        }
    }

    Model GranularityConverter::convert(const Model& src, GranularityConvertOption option) {
        // 組み合わせの数を減らすため、まず最小地物に変換してから望みの粒度に変換します。
        auto atomic = convertToAtomic(src);
        auto converted = merge(atomic, option);
        return converted;
    }
}
