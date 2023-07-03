#pragma once

#include <libplateau_api.h>
#include <map>

namespace plateau::polygonMesh {


    struct CityObjectIndex {
        TVec2f vec;

        //コンストラクタ
        CityObjectIndex(TVec2f _vec) : vec(_vec) {
        }

        TVec2f getUV() {
            return(vec);
        }

        CityObjectIndex(int primaryNum, int atomicNum) {
            vec = TVec2f(primaryNum, atomicNum);
        }

        // `std::map` のキーとして `Node` オブジェクトを使用するための `<` 演算子をオーバーロードする
        //現在のオブジェクトが指定されたオブジェクトの前にある場合はtrueを返します
        bool operator<(const CityObjectIndex& obj) const {
            //return (obj.vec.x < obj.vec.y) ? true : (obj.vec.x == obj.vec.y) ? ((obj.vec.x < obj.vec.y) ? true : false) : false;
            return (obj.vec.x < obj.vec.y);
        }
    };


    /**
     * CityObjectListは、地物インデックスと地物IDの対応関係を保持するために、Modelに含まれる地物のリストを保持する目的で設計されています。
     * 
     * plateau::polygonMesh::Modelに次のメンバー変数を追加します。
     * 上記のUVに記録したID ( CityObjectIndex ) と、 gml:id を対応付けるデータ構造  CityObjectListを作成します。
     * CityObjectListは、 std::map<CityObjectIndex, std::string> city_object_index_to_gml_idから構成されます。
     * なお、std::map のキーに自作の構造体をとるには、比較演算子< を定義する必要があることに注意してください。
     * 参考: 逆引き！ C++でMapのキーとして、自分で定義した構造体を利用する
     * 「https://programming-tips.info/use_struct_as_key_of_map/cpp/index.html」
     *
     * mesh_extractor の処理において、上記の city_object_indexが構築されるようにし、ゲームエンジンから読めるようにしてください。
     * 
     * ■ メンバー関数
     * 
     * std::string& getAtomicGmlId(CityObjectIndexType cityObjectIndex)
     * std::string& getPrimaryGmlId(int id)
     * 
     * 主要地物のCityObjectIndexは、(n, -1)ですが、内部的な「-1」を隠匿するために
     * getAtomicGmlId()とgetPrimaryGmlId()を使い分けます。
     * 
     */
    class LIBPLATEAU_EXPORT CityObjectList {

    public:
        CityObjectList();

    public:
        CityObjectIndex createtPrimaryId();
        CityObjectIndex createtAtomicId();
        //CityObjectIndex createtPrimaryCityObjectIndex(int index);

    public:
        const std::string& getAtomicGmlId(CityObjectIndex CityObjectIndex);
        const std::string& getPrimaryGmlId(int id);

    public:
        void add(CityObjectIndex key, std::string value);

    private:
        const int none = -1;
        int primary_id = 0;
        int atomic_id = 0;
        std::map<CityObjectIndex, std::string> city_object_index_to_gml_id;

    };

}
