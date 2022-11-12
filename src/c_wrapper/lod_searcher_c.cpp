#include "libplateau_c.h"
#include <filesystem>
#include <plateau/udx/lod_searcher.h>

extern "C"{
    using namespace plateau::udx;
    using namespace libplateau;
    namespace fs = std::filesystem;
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_lod_searcher_search_lods_in_file(char* file_path_utf8, unsigned* out_lod_flags){
        API_TRY{
            auto path = fs::u8path(file_path_utf8);
            auto lods = LodSearcher::searchLodsInFile(path);
            *out_lod_flags = lods.getFlag();
            return APIResult::Success;
        }API_CATCH
        return APIResult::ErrorUnknown;
    }
}
