#include <citygml/texturecoordinates.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

DLL_VALUE_FUNC(plateau_texture_coordinates_get_coordinate,
               TextureCoordinates,
               TVec2f,
               handle->getCoords().at(index),
               ,int index)

DLL_VALUE_FUNC(plateau_texture_coordinates_count,
               TextureCoordinates,
               int,
               handle->getCoords().size())

}