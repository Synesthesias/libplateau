#include <citygml/featureobject.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C"{
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_feature_object_get_envelope(
            const FeatureObject* const feature_object,
            double* const out_array
            ){
        API_TRY{
            const auto& envelope = feature_object->getEnvelope();
            const auto& lower = envelope.getLowerBound().xyz;
            const auto& upper = envelope.getUpperBound().xyz;

            for(int i=0; i<3; i++){
                out_array[i] = lower[i];
            }
            for(int i=0; i<3; i++){
                out_array[i + 3] = upper[i];
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_feature_object_set_envelope(
            FeatureObject* const feature_object,
            const double lower_x, const double lower_y, const double lower_z, const double upper_x, const double upper_y, const double upper_z
    ){
        API_TRY{
            // この動的確保領域は setEnvelope の中で unique_ptr に渡ります。
            const auto e = new Envelope();
            const auto lower = TVec3d(lower_x, lower_y, lower_z);
            const auto upper = TVec3d(upper_x, upper_y, upper_z);
            e->setLowerBound(lower);
            e->setUpperBound(upper);
            feature_object->setEnvelope(e);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}