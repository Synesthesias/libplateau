#pragma once

#include <plateau/polygon_mesh/mesh.h>
#include <plateau/polygon_mesh/mesh_extract_options.h>
#include <citygml/polygon.h>
#include <citygml/cityobject.h>

namespace plateau::polygonMesh {

    class MeshMerger {
    public:
        /**
         * citygml::Polygon をマージします。
         * 引数で与えられたポリゴンのうち、次の情報を自身に追加します。
         * ・頂点リスト、インデックスリスト、UV1、テクスチャ。
         * なおその他の情報のマージには未対応です。例えば LinearRing は考慮されません。
         * options.export_appearance の値によって、 mergeWithTexture または mergeWithoutTexture を呼び出します。
         */
        static void merge(Mesh& mesh, const citygml::Polygon& other_poly, MeshExtractOptions options,
                          const TVec2f& uv_2_element,
                          const TVec2f& uv_3_element);

        /**
         * merge関数を 引数 city_object_ の各 Polygon に対して実行します。
         */
        static void mergePolygonsInCityObject(Mesh& mesh, const citygml::CityObject& city_object, unsigned int lod,
                                              const MeshExtractOptions& options, const TVec2f& uv_2_element,
                                              const TVec2f& uv_3_element);

        /**
         * merge関数を 引数 city_objects の 各 CityObject の 各 Polygon に対して実行します。
         */
        static void mergePolygonsInCityObjects(Mesh& mesh, const std::list<const citygml::CityObject*>& city_objects,
                                               unsigned int lod,
                                               const TVec2f& uv_3_element, const MeshExtractOptions& options,
                                               const TVec2f& uv_2_element);

        /**
         * city_obj に含まれるポリゴンをすべて検索し、リストで返します。
         * 子の CityObject は検索しません。
         * 子の Geometry は再帰的に検索します。
         */
        static std::list<const citygml::Polygon*> findAllPolygons(const citygml::CityObject& city_obj, unsigned lod);

    private:
        // TODO 無名名前空間に移動
        /**
         * merge関数 のテクスチャあり版です。
         * テクスチャについては、マージした結果、範囲とテクスチャを対応付ける SubMesh が追加されます。
         */
        static void mergeWithTexture(Mesh& mesh, const citygml::Polygon& other_poly, const MeshExtractOptions& options,
                                     const TVec2f& uv_2_element, const TVec2f& uv_3_element);

        /**
         * merge関数 のテクスチャ無し版です。
         * 生成される Mesh の SubMesh はただ1つであり、そのテクスチャパスは空文字列となります。
         */
        static void mergeWithoutTexture(Mesh& mesh, const citygml::Polygon& other_poly,
                                        const TVec2f& uv_2_element,
                                        const TVec2f& uv_3_element, const MeshExtractOptions& options);

        /// 形状情報をマージします。merge関数における SubMesh を扱わない版です。
        static void mergeShape(Mesh& mesh, const citygml::Polygon& other_poly, const TVec2f& uv_2_element,
                               const TVec2f& uv_3_element,
                               const MeshExtractOptions& options);

        static bool isValidPolygon(const citygml::Polygon& other_poly);

        /**
         * findAllPolygons のジオメトリを対象とする版です。
         * 結果は引数の polygons に格納します。
         */
        static void findAllPolygons(const citygml::Geometry& geom,
                                    std::list<const citygml::Polygon*>& polygons,
                                    unsigned lod);
    };
}
