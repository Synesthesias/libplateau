#include <citygml/featureobject.h>
#include "libplateau_c.h"

using namespace citygml;

extern "C"{
    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_feature_object_get_envelop(
            const FeatureObject* featureObject,
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
}