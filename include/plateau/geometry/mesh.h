#pragma once
#include "citygml/polygon.h"
#include "plateau_dll_logger.h"
#include "sub_mesh.h"
#include "mesh_extract_options.h"
#include "citygml/cityobject.h"
#include <libplateau_api.h>
#include <optional>

namespace plateau::geometry {
    using namespace citygml;
    using UV = std::vector<TVec2f>;

    /**
     * メッシュ情報です。
     * Unity や Unreal Engine でメッシュを生成するために必要な情報が含まれるよう意図されています。
     * 具体的には 頂点リスト、Indicesリスト、UV、サブメッシュ（含テクスチャ）があります。
     *
     * 詳しくは Model クラスのコメントをご覧ください。
     *
     * このメッシュ情報がどのように生成されるかというと、
     * 空のMeshから Mesh::mergeWithTexture() 関数で citygml::Polygon を渡すことで Mesh に情報が追加され、
     * Polygon が複数あれば Polygonごとに複数回 Mergeが実行されることで複数個のSubMeshを含んだMeshが構築されるようになっています。
     *
     * 保持する頂点の座標系について、
     * citygml::Polygon は極座標系ですが、merge() メソッド実行時にデカルト座標系に変換されて保持します。
     */
    class LIBPLATEAU_EXPORT Mesh {
    public:
       explicit Mesh(const std::string &id);

       /// コピーを禁止します。メッシュは重いので。
       /// ムーブのみ許可します。
       Mesh(const Mesh& mesh) = delete;
       Mesh& operator=(const Mesh&) = delete;
       Mesh(Mesh&& mesh) = default;
       Mesh& operator=(Mesh&& mesh) = default;

        [[nodiscard]] std::vector<TVec3d> & getVertices();
        [[nodiscard]] const std::vector<TVec3d>& getVertices() const;

        [[nodiscard]] const std::vector<int> & getIndices() const;
        void setUV2(const UV& uv2);
        [[nodiscard]] const UV &getUV1() const;
        [[nodiscard]] const UV &getUV2() const;
        [[nodiscard]] const UV &getUV3() const;
        [[nodiscard]] const std::vector<SubMesh>& getSubMeshes() const;

        /**
         * citygml::Polygon をマージします。
         * 引数で与えられたポリゴンのうち、次の情報を自身に追加します。
         * ・頂点リスト、インデックスリスト、UV1、テクスチャ。
         * なおその他の情報のマージには未対応です。例えば LinearRing は考慮されません。
         * options.exportAppearance の値によって、 mergeWithTexture または mergeWithoutTexture を呼び出します。
         */
        void merge(const Polygon &otherPoly, MeshExtractOptions options, const TVec2f &UV2Element, const TVec2f &UV3Element);

        /**
         * merge関数を CityObject の各 Polygon に対して実行します。
         */
        void mergePolygonsInCityObject(const CityObject &cityObject, int LOD, const MeshExtractOptions &options,
                                       const TVec2f &UV2Element, const TVec2f &UV3Element);

        /**
         * merge関数を cityObjects の 各 CityObject の 各 Polygon に対して実行します。
         */
        void
        mergePolygonsInCityObjects(const std::list<const CityObject *> &cityObjects, int LOD, const TVec2f &UV3Element,
                                   const MeshExtractOptions &options, const TVec2f &UV2Element);

    private:
        std::vector<TVec3d> vertices_;
        std::vector<int> indices_;
        UV uv1_;
        UV uv2_;
        UV uv3_;
        std::vector<SubMesh> subMeshes_;

        /**
         * merge関数 のテクスチャあり版です。
         * テクスチャについては、マージした結果、範囲とテクスチャを対応付ける SubMesh が追加されます。
         */
        void mergeWithTexture(const Polygon &otherPoly, const MeshExtractOptions &options, const TVec2f &UV2Element,
                              const TVec2f &UV3Element);

        /**
         * merge関数 のテクスチャ無し版です。
         * SubMesh はただ1つであり、そのテクスチャパスは空文字列となります。
         */
        void mergeWithoutTexture(const Polygon &otherPoly, const TVec2f &UV2Element, const TVec2f &UV3Element,
                                 const MeshExtractOptions &options);

        /// 形状情報をマージします。merge関数における SubMesh を扱わない版です。
        void mergeShape(const Polygon &otherPoly, const TVec2f &UV2Element, const TVec2f &UV3Element,
                        const MeshExtractOptions &options);
        /// 頂点リストの末尾に追加します。
        void addVerticesList(const std::vector<TVec3d> &otherVertices, const MeshExtractOptions &options);
        void addIndicesList(const std::vector<unsigned>& otherIndices, unsigned prevNumVertices);
        void addUV1(const Polygon& otherPoly);
        void addUV2WithSameVal(const TVec2f& UV2Val, unsigned numAddingVertices);
        void addUV3WithSameVal(const TVec2f &UV3Val, unsigned numAddingVertices);

        /**
         * SubMesh を追加し、そのテクスチャパスには otherPoly のものを指定します。
         * 利用すべき状況 : 形状を追加したので、追加分を新しいテクスチャに設定したいという状況で利用できます。
         *
         * ただし、直前の SubMesh のテクスチャとパスが同じであれば、
         * 代わりに extendLastSubMesh を実行します。
         * なぜなら、同じテクスチャであればサブメッシュを分けるのは無意味で描画負荷を増やすだけと思われるためです。
         */
        void addSubMesh(const Polygon &otherPoly);

        /**
         * 直前の SubMesh の範囲を拡大し、範囲の終わりがindicesリストの最後を指すようにします。
         * 利用すべき状況 : 形状を追加したけど、テクスチャは前と同じものにしたいとう状況で利用できます。
         * SubMeshがない場合は最初の1つをテクスチャなしで追加します。
         */
        void extendLastSubMesh();
        static bool isValidPolygon(const Polygon& otherPoly);
    };
}