#pragma once
#include "citygml/polygon.h"
#include "plateau_dll_logger.h"
#include <libplateau_api.h>
#include <optional>
using namespace citygml;
using UV = std::vector<TVec2f>;
/**
 * keyを Indicesリストのインデックス（つまり面情報）、 value を テクスチャURLとする map です。
 * keyであるIndicesリストのインデックス以降（つまりこれ以降の面）は value のURLのテクスチャを使う、という意です。
 * 「この場所にはテクスチャがない」というケースでは、valueは 空文字列 になります。
 */
using MultiTextureURL = std::map<int, std::string>;

namespace plateau::geometry {

/**
 * PLATEAU向けに機能を拡張した citygml::Polygon です。
 * 空の状態から Merge() によって頂点・面を追加していく使い方を想定しています。
 * 拡張した機能:
 * ・ポリゴンマージ機能 : 軽量化のために都市3Dモデルをグリッド状にまとめる MeshExtractor::gridMerge で利用します。
 * ・複数テクスチャ機能
 */
    // TODO Polygonを継承する必要はないかも
    class LIBPLATEAU_EXPORT Mesh {
    public:
        Mesh(const std::string &id);

        // TODO const関数とconstでない関数で2つ同じ実装があるけど、まとめられそうな気がする
        [[nodiscard]] std::vector<TVec3d> & getVertices();
        [[nodiscard]] const std::vector<TVec3d>& getVerticesConst() const;

        [[nodiscard]] const std::vector<int> & getIndices() const;

        void setUV2(const UV& uv2);

        [[nodiscard]] const UV &getUV1() const;

        [[nodiscard]] const UV &getUV2() const;

        [[nodiscard]] const UV &getUV3() const;

        [[nodiscard]] const MultiTextureURL &getMultiTexture() const;

        /**
         * ポリゴンをマージします。
         * 引数で与えられたポリゴンのうち、次の情報を自身に追加します。
         * ・頂点リスト、インデックスリスト、UV1、テクスチャ
         * なおその他の情報のマージには未対応です。例えば LinearRing は変化しません。
         * テクスチャについては、マージした結果、範囲とテクスチャを対応付ける MultiTextureURL が構築されます。
         * テクスチャがない範囲では、それに対応する MultiTextureURL の Texture は ID と URL が "noneTexture" である特別なテクスチャになります。
         */
        void Merge(const Polygon &otherPoly, const TVec2f &UV2Element, const TVec2f &UV3Element);

    private:
        std::vector<TVec3d> vertices_;
        std::vector<int> indices_;
        UV uv1_;
        UV uv2_;
        UV uv3_;
        /**
         * MultiTextureURL は、IndicesリストのインデックスとTexture の URLを対応付けるmapです。
         * その面以降、次の番号まではそのURLのTextureであるとします。
         */
        MultiTextureURL multiTexture_;
    };
}