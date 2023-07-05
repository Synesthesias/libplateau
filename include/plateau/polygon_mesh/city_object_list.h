#pragma once

#include <libplateau_api.h>
#include <map>

namespace plateau::polygonMesh {


    class CityObjectIndex {
    private:
        TVec2f vec;

    public:
        //コンストラクタ
        CityObjectIndex(TVec2f _vec) : vec(_vec) {
        }

        TVec2f getIndex() {
            return(vec);
        }

        CityObjectIndex(int primaryNum, int atomicNum) {
            vec = TVec2f(primaryNum, atomicNum);
        }

        bool operator<(const CityObjectIndex& obj) const {
            return (this->vec.x < obj.vec.x ? true : (this->vec.y < obj.vec.y));
        }

        bool operator>(const CityObjectIndex& obj) const {
            return (this->vec.x > obj.vec.x ? true : (this->vec.y > obj.vec.y));
        }

        bool operator==(const CityObjectIndex& obj) const {
            return (this->vec.x == obj.vec.x ? true : (this->vec.y == obj.vec.y));
        }
    };

    /**
     * 
     * @brief CityObjectListは、地物インデックスと地物IDの対応関係を保持するために、Modelに含まれる地物のリストを保持する目的で設計されています。
     * 
     * plateau::polygonMesh::Meshに次のメンバー変数を追加します。
     * 上記のUVに記録したID ( CityObjectIndex ) と、 gml:id を対応付けるデータ構造  CityObjectListを作成します。
     * CityObjectListは、 std::map<CityObjectIndex, std::string> city_object_index_to_gml_idから構成されます。
     * なお、std::map のキーに自作の構造体をとるには、比較演算子< を定義する必要があることに注意してください。
     * 参考: 逆引き！ C++でMapのキーとして、自分で定義した構造体を利用する
     * 「https://programming-tips.info/use_struct_as_key_of_map/cpp/index.html」
     *
     * mesh_extractor の処理において、上記の city_object_indexが構築されるようにし、ゲームエンジンから読めるようにします。
     */
    class LIBPLATEAU_EXPORT CityObjectList {

    public:
        CityObjectList();

        CityObjectList(const CityObjectList& city_object_list) = delete;
        CityObjectList& operator=(const CityObjectList&) = delete;
        CityObjectList(CityObjectList&& city_object_list) = default;
        //CityObjectList& operator=(CityObjectList&& city_object_list) = default;
        CityObjectList& operator=(CityObjectList&& city_object_list);

        CityObjectIndex createPrimaryId();
        CityObjectIndex createPrimaryAtomicId();

        const std::string& getAtomicGmlId(CityObjectIndex CityObjectIndex);
        const std::string& getPrimaryGmlId(int id);

        void add(CityObjectIndex key, std::string value);

    private:
        const int none = -1;
        int primary_id = 0;
        int primary_atomic_id = 0;
        std::map<CityObjectIndex, std::string> city_object_index_to_gml_id;
    };

}
