#pragma once
#include "citygml/polygon.h"
#include "plateau_dll_logger.h"
#include <libplateau_api.h>
using namespace citygml;
using UV2 = std::vector<TVec2f>;

/**
 * PLATEAU向けに機能を拡張した citygml::Polygon です。
 * 拡張した機能:
 * ポリゴンマージ機能は、軽量化のために都市3Dモデルをグリッド状にまとめる MeshMerger::gridMerge で利用します。
 */
class LIBPLATEAU_EXPORT PolygonWithUV2 : public Polygon{
public:
    PolygonWithUV2(const std::string& id, std::shared_ptr<PlateauDllLogger> logger);
    void setUV2(std::unique_ptr<UV2> uv2);
    const UV2& getUV2();
    /**
     * ポリゴンをマージします。
     * 頂点リストとインデックスリストをそれぞれ結合します。
     * その他の情報のマージには未対応です。例えば LinearRing は変化しません。
     */
    void Merge(const Polygon &otherPoly);

private:
    std::unique_ptr<UV2> uv2_;
    std::shared_ptr<PlateauDllLogger> logger_;
};
