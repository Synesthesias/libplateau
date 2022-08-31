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
         * Model を new して shared_ptr で返します。
         */
        static std::shared_ptr<Model> extract(const CityModel &cityModel, const MeshExtractOptions &options) ;

        /**
         * extract関数について、戻り値がスマートポインタの代わりに、引数にデータを追加するようになった版です。
         * DLL利用者との間でModelをやりとりするには生ポインタである必要があるための措置です。
         * 別途 初期化されたばかりのModelを引数で受け取り、そのModelに対して結果を格納します。
         * 生ポインタのdeleteはDLLの利用者の責任です。
         */
        static void extract(Model &outModel, const CityModel &cityModel, const MeshExtractOptions &options) ;

    private:
        static void extractInner(Model& outModel, const CityModel& cityModel, const MeshExtractOptions& options);
    };

    /**
     * インポートした各CityObject についてインポート番号を記憶するためのクラスです。
     * ID情報をUV2に埋め込む時に利用します。
     * TODO UV2からデータを取り出す機能は未実装です。
     */
    class CityObjectWithImportID {
    public:
        CityObjectWithImportID(const CityObject * cityObject, int primaryImportID, int secondaryImportID);

        const CityObject *getCityObject() const { return cityObject; }

        int getPrimaryImportID() const { return primaryImportID; }

        int getSecondaryImportID() const { return secondaryImportID; }

    private:
        const CityObject *const cityObject;
        int primaryImportID;
        int secondaryImportID;
    };
}
