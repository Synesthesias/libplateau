#pragma once
#include <libplateau_api.h>
#include <memory>
#include <plateau/geometry/mesh.h>
#include <plateau/geometry/mesh_extract_options.h>
#include "citygml/citymodel.h"
#include "model.h"

namespace plateau::geometry {

    /**
     * CityModelからModel(メッシュ等)を構築します。
     * このクラスの利用者である各ゲームエンジンは、このクラスから受け取った Model を元に
     * ゲームオブジェクト、メッシュ、テクスチャを生成することが期待されます。
     *
     * 詳しくは Model クラスのコメントを参照してください。
     */
    class LIBPLATEAU_EXPORT MeshExtractor {
    public:

        /**
         * CityModel から Modelを取り出します。
         */
        [[nodiscard]] static std::shared_ptr<Model> extract(const CityModel &cityModel, const MeshExtractOptions &options) ;

        /**
         * extract関数について、戻り値がスマートポインタの代わりに生ポインタになった版です。
         * DLL利用者との間でModelをやりとりするには生ポインタである必要があるための措置です。
         * 生ポインタのdeleteはDLLの利用者の責任です。
         */
        [[nodiscard]] static Model *extract_to_row_pointer(const CityModel &cityModel, const MeshExtractOptions &options) ;

    };

    /**
     * インポートした各CityObject についてインポート番号を記憶するためのクラスです。
     * ID情報をUV2に埋め込む時に利用します。
     * TODO UV2からデータを取り出す機能は未実装です。
     */
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