#pragma once
#include "citygml/polygon.h"
#include "plateau_dll_logger.h"
#include <libplateau_api.h>
using namespace citygml;
using UV = std::vector<TVec2f>;
using MultiTexture = std::map<int, std::shared_ptr<const Texture>>;

/**
 * PLATEAU向けに機能を拡張した citygml::Polygon です。
 * 空の状態から Merge() によって頂点・面を追加していく使い方を想定しています。
 * 拡張した機能:
 * ・ポリゴンマージ機能 : 軽量化のために都市3Dモデルをグリッド状にまとめる MeshMerger::gridMerge で利用します。
 * ・複数テクスチャ機能
 */
class LIBPLATEAU_EXPORT PlateauPolygon : public Polygon{
public:
    PlateauPolygon(const std::string& id, std::shared_ptr<PlateauDllLogger> logger);
    void setUV2(std::unique_ptr<UV> uv2);
    [[nodiscard]] const UV& getUV1() const;
    [[nodiscard]] const UV& getUV2() const;
    [[nodiscard]] const MultiTexture& getMultiTexture() const;
    /**
     * ポリゴンをマージします。
     * 引数で与えられたポリゴンのうち、次の情報を自身に追加します。
     * ・頂点リスト、インデックスリスト、UV1
     * その他の情報のマージには未対応です。例えば LinearRing は変化しません。
     */
    void Merge(const Polygon &otherPoly);

private:
    std::unique_ptr<UV> uv1_;
    std::unique_ptr<UV> uv2_;
    /**
     * MultiTexture は、頂点番号とTextureを対応付けるmapです。
     * その頂点番号以降、次の頂点番号まではそのTextureであるとします。
     */
    std::unique_ptr<MultiTexture> multiTexture_;
    std::shared_ptr<PlateauDllLogger> logger_;
};
