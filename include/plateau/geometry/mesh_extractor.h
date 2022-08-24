#pragma once
#include <libplateau_api.h>
#include <memory>
#include <plateau/geometry/mesh.h>
#include <plateau/geometry/mesh_extract_options.h>
#include "citygml/citymodel.h"
#include "model.h"

namespace plateau::geometry {
/**
 * @brief
 * CityModelからModel(メッシュ等)を取り出します。
 * GridMerge関数に CityModel を渡すと、配下のジオメトリをすべて検索して グリッド上のポリゴンにまとめます。
 */
    class LIBPLATEAU_EXPORT MeshExtractor {
    public:

        // TODO 以下のModelの設計意図に関するコメントはModelに書いた方が良さそう
        /**
         * cityModel から ジオメトリを取り出します。
         * 戻り値の Model は子に階層構造の Node を持ち、 Node の中にメッシュ・テクスチャ・サブメッシュ・名前情報があります。
         * Modelの設計意図は、GMLファイルのパーサーの世界とゲームエンジンの世界を橋渡しする、ジオメトリを表現する中間データ構造として設計されています。
         * 具体的には、Nodeの階層構造はゲームエンジンにおいて表現したいゲームオブジェクトの階層構造と対応します。
         * Nodeの持つ Mesh の情報は、ゲームエンジン側で頂点座標、UVの配列をコピーすれば必要なメッシュをゲームエンジン側で生成できるようになっています。
         */
        [[nodiscard]] std::shared_ptr<Model> extract(const CityModel &cityModel, const MeshExtractOptions &options) const;

        /**
         * extract関数について、戻り値がスマートポインタの代わりに生ポインタになった版です。
         * DLL利用者とModelをやりとりするには生ポインタである必要があるための措置です。
         * 生ポインタのdeleteはDLLの利用者の責任です。
         */
        [[nodiscard]] Model *extract_to_row_pointer(const CityModel &cityModel, const MeshExtractOptions &options) const;

    private:
    };

    class CityObjectWithImportID {
    public:
        CityObjectWithImportID(const CityObject * cityObject, int primaryImportID, int secondaryImportID);

        [[nodiscard]] const CityObject *getCityObject() const { return cityObject; }

        [[nodiscard]] int getPrimaryImportID() const { return primaryImportID; }

        [[nodiscard]] int getSecondaryImportID() const { return secondaryImportID; }

    private:
        const CityObject *const cityObject;
        int primaryImportID;
        int secondaryImportID;
    };
}