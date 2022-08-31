#include <citygml/texturecoordinates.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

DLL_VALUE_FUNC_WITH_INDEX_CHECK(plateau_texture_coordinates_get_coordinate,
               TextureCoordinates,
               TVec2f,
               handle->getCoords().at(index),
               index >= handle->getCoords().size())

DLL_VALUE_FUNC(plateau_texture_coordinates_count,
               TextureCoordinates,
               int,
               handle->getCoords().size())

DLL_STRING_PTR_FUNC(plateau_texture_coordinates_get_target_linear_ring_id,
                    TextureCoordinates,
                    handle->getTargetLinearRingID())

/// 与えられた LinearRing がターゲットかどうか判定してboolを返します。
DLL_VALUE_FUNC(plateau_texture_coordinates_is_ring_target,
               TextureCoordinates,
               bool,
               handle->targets(*ring),
               ,const LinearRing* ring)
}
