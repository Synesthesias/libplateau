#include <citygml/featureobject.h>
#include "libplateau_c.h"

using namespace citygml;

extern "C"{
    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_feature_object_get_envelope(
            const FeatureObject* const featureObject,
            double* const outArray
            ){
        API_TRY{
            const auto& envelope = featureObject->getEnvelope();
            const auto& lower = envelope.getLowerBound().xyz;
            const auto& upper = envelope.getUpperBound().xyz;

            for(int i=0; i<3; i++){
                outArray[i] = lower[i];
            }
            for(int i=0; i<3; i++){
                outArray[i+3] = upper[i];
            }
        }
        API_CATCH;
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_feature_object_set_envelope(
            FeatureObject* const featureObject,
            double lowerX, double lowerY, double lowerZ, double upperX, double upperY, double upperZ
    ){
        API_TRY{
            // この動的確保領域は setEnvelope の中で unique_ptr に渡ります。
            auto e = new Envelope();
            auto lower = TVec3d(lowerX, lowerY, lowerZ);
            auto upper = TVec3d(upperX, upperY, upperZ);
            e->setLowerBound(lower);
            e->setUpperBound(upper);
            featureObject->setEnvelope(e);
        }
        API_CATCH;
    }
}