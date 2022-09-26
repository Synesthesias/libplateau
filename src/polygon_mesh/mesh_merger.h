#pragma once

#include <plateau/polygon_mesh/mesh.h>
#include <plateau/polygon_mesh/mesh_extract_options.h>
#include <citygml/polygon.h>
#include <citygml/cityobject.h>
#include <list>
#include "citygml/polygon.h"
#include "plateau/geometry/geo_reference.h"
#include "citygml/cityobject.h"

namespace plateau::polygonMesh {

    /**
     * citygml::Polygon (GMLパーサーの成果物の一部) から変換して polygonMesh::Mesh (3Dモデル中間データ構造) を生成します。
     */
    class LIBPLATEAU_EXPORT MeshMerger {
    public:
        /**
         * citygml::Polygon の情報を Mesh 向けに変換し、 引数の mesh に書き加えます。
         * 引数で与えられたポリゴンのうち、次の情報を追加します。
         * ・頂点リスト、インデックスリスト、UV1、テクスチャ。
         * なおその他の情報のマージには未対応です。例えば LinearRing は考慮されません。
         * options.export_appearance の値によって、 mergeWithTexture または mergeWithoutTexture を呼び出します。
         */
        static void merge(Mesh& mesh, const citygml::Polygon& other_poly, const MeshExtractOptions& mesh_extract_options,
                          const GeoReference& geo_reference, const TVec2f& uv_2_element,
                          const TVec2f& uv_3_element);

        /**
         * merge関数を 引数 city_object_ の各 Polygon に対して実行します。
         */
        static void mergePolygonsInCityObject(Mesh& mesh, const citygml::CityObject& city_object, unsigned int lod,
                                              const MeshExtractOptions& mesh_extract_options, const GeoReference& geo_reference,
                                              const TVec2f& uv_2_element, const TVec2f& uv_3_element);

        /**
         * merge関数を 引数 city_objects の 各 CityObject の 各 Polygon に対して実行します。
         */
        static void
        mergePolygonsInCityObjects(Mesh& mesh, const std::list<const citygml::CityObject*>& city_objects, unsigned int lod,
                                   const MeshExtractOptions& mesh_extract_options, const GeoReference& geo_reference,
                                   const TVec2f& uv_3_element, const TVec2f& uv_2_element);

        /**
         * city_obj に含まれるポリゴンをすべて検索し、リストで返します。
         * 子の CityObject は検索しません。
         * 子の Geometry は再帰的に検索します。
         */
        static std::list<const citygml::Polygon*> findAllPolygons(const citygml::CityObject& city_obj, unsigned lod);
    };
}
