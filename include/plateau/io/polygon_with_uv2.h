#pragma once
#include "citygml/polygon.h"
#include "plateau_dll_logger.h"
#include <libplateau_api.h>
using namespace citygml;
using UV2 = std::vector<TVec2f>;

class LIBPLATEAU_EXPORT PolygonWithUV2 : public Polygon{
public:
    PolygonWithUV2(const std::string& id, std::shared_ptr<PlateauDllLogger> logger);
    void setUV2(std::unique_ptr<UV2> uv2);
    const UV2& getUV2();
    void Merge(const Polygon &otherPoly);

private:
    std::unique_ptr<UV2> uv2_;
    std::shared_ptr<PlateauDllLogger> logger_;
};
