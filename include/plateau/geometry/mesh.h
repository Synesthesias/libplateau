#pragma once
#include "citygml/polygon.h"
#include "plateau_dll_logger.h"
#include "sub_mesh.h"
#include <libplateau_api.h>
#include <optional>
using namespace citygml;
using UV = std::vector<TVec2f>;
///**
// * keyを Indicesリストのインデックス（つまり面情報）、 value を テクスチャURLとする map です。
// * keyであるIndicesリストのインデックス以降（つまりこれ以降の面）は value のURLのテクスチャを使う、という意です。
// * 「この場所にはテクスチャがない」というケースでは、valueは 空文字列 になります。
// */
//using MultiTextureURL = std::map<int, std::string>;

namespace plateau::geometry {

/**
 * メッシュ情報です。
 * Unity や Unreal Engine でメッシュを生成するために必要な情報が含まれるよう意図されています。
 * 具体的には 頂点リスト、Indicesリスト、UV、サブメッシュ（含テクスチャ）があります。
 *
 * 詳しくは Model クラスのコメントをご覧ください。
 *
 * このメッシュ情報がどのように生成されるかというと、
 * 空のMeshから Mesh::Merge() 関数で citygml::Polygon を渡すことで Mesh に情報が追加され、
 * Polygon が複数あれば Polygonごとに複数回 Mergeが実行されることで複数個のSubMeshを含んだMeshが構築されるようになっています。
 */
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

//        [[nodiscard]] const MultiTextureURL &getMultiTexture() const;
        [[nodiscard]] const std::vector<SubMesh>& getSubMeshes() const;

        /**
         * ポリゴンをマージします。
         * 引数で与えられたポリゴンのうち、次の情報を自身に追加します。
         * ・頂点リスト、インデックスリスト、UV1、テクスチャ
         * なおその他の情報のマージには未対応です。例えば LinearRing は考慮されません。
         * テクスチャについては、マージした結果、範囲とテクスチャを対応付ける SubMesh が追加されます。
         */
        void Merge(const Polygon &otherPoly, const TVec2f &UV2Element, const TVec2f &UV3Element);

    private:
        std::vector<TVec3d> vertices_;
        std::vector<int> indices_;
        UV uv1_;
        UV uv2_;
        UV uv3_;
        std::vector<SubMesh> subMeshes_;
//        /**
//         * MultiTextureURL は、IndicesリストのインデックスとTexture の URLを対応付けるmapです。
//         * その面以降、次の番号まではそのURLのTextureであるとします。
//         */
//        MultiTextureURL multiTexture_;
    };
}