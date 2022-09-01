#include <citygml/texturetargetdefinition.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

DLL_VALUE_FUNC(plateau_texture_target_definition_get_texture_coordinates_count,
               TextureTargetDefinition,
               int,
               handle->getTextureCoordinatesCount())

DLL_PTR_FUNC_WITH_INDEX_CHECK(plateau_texture_target_definition_get_texture_coordinates,
             TextureTargetDefinition,
             TextureCoordinates,
             handle->getTextureCoordinates(index).get(),
             index >= handle->getTextureCoordinatesCount())
}
