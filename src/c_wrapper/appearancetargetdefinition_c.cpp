#include <citygml/appearancetargetdefinition.h>
#include <citygml/texture.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

// AppearanceTargetDefinition<T> について、T は Texture と Material の2通りあります。
// しかし extern "C" 内でテンプレートを扱うのは難しそうなので、
// とりあえず今は T = Texture として決め打って実装します。
// 今後 T = Material の場合に対処する必要が出てきたら、同じく T = Material と決め打った関数を実装する必要があります。

DLL_STRING_PTR_FUNC(plateau_appearance_target_definition_tex_get_target_id,
                    AppearanceTargetDefinition<Texture>,
                    handle->getTargetID())

DLL_PTR_FUNC(plateau_appearance_target_definition_tex_get_appearance,
             AppearanceTargetDefinition<Texture>,
             Texture,
             handle->getAppearance().get())
}
