#include <citygml/material.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;
extern "C" {

        DLL_VALUE_FUNC(plateau_material_get_diffuse,
                    Material,
                    TVec3f,
                    handle->getDiffuse())

        DLL_VALUE_FUNC(plateau_material_get_emissive,
                    Material,
                    TVec3f,
                    handle->getEmissive())

        DLL_VALUE_FUNC(plateau_material_get_specular,
                    Material,
                    TVec3f,
                    handle->getSpecular())

        DLL_VALUE_FUNC(plateau_material_get_ambient_intensity,
                    Material,
                    float,
                    handle->getAmbientIntensity())

        DLL_VALUE_FUNC(plateau_material_get_shininess,
                    Material,
                    float,
                    handle->getShininess())

        DLL_VALUE_FUNC(plateau_material_get_transparency,
                    Material,
                    float,
                    handle->getTransparency())

        DLL_VALUE_FUNC(plateau_material_is_smooth,
                    Material,
                    bool,
                    handle->isSmooth())

}