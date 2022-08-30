#include <plateau/geometry/mesh.h>
#include <memory>
#include "citygml/texture.h"
#include "citygml/cityobject.h"
#include "plateau/geometry/geometry_utils.h"
#include "plateau/io/polar_to_plane_cartesian.h"
#include "plateau/io/obj_writer.h"

using namespace plateau::geometry;

Mesh::Mesh(const std::string &id) :
    vertices_(),
    uv1_(UV()),
    uv2_(UV()),
    uv3_(UV()),
    subMeshes_(){
}

std::vector<TVec3d>& Mesh::getVertices() {
    return vertices_;
}

const std::vector<TVec3d>& Mesh::getVertices() const {
    return vertices_;
}

const std::vector<int>& Mesh::getIndices() const {
    return indices_;
}

void Mesh::setUV2(const UV& uv2) {
    if(uv2.size() != vertices_.size()){
        throw std::exception("Size of uv2 does not match num of vertices.");
    }
    uv2_ = uv2;
}

const UV& Mesh::getUV1() const{
    return uv1_;
}

const UV& Mesh::getUV2() const{
    return uv2_;
}

const UV& Mesh::getUV3() const{
    return uv3_;
}

const std::vector<SubMesh>& Mesh::getSubMeshes() const {
    return subMeshes_;
}

void Mesh::merge(const Polygon &otherPoly, MeshExtractOptions options, const TVec2f &UV2Element, const TVec2f &UV3Element){
    if(!isValidPolygon(otherPoly)) return;
    if(options.exportAppearance){
        mergeWithTexture(otherPoly, options, UV2Element, UV3Element);
    }else{
        mergeWithoutTexture(otherPoly, UV2Element, UV3Element, options);
    }
}

void Mesh::mergePolygonsInCityObject(const CityObject &cityObject, int LOD, const MeshExtractOptions &options,
                                     const TVec2f &UV2Element, const TVec2f &UV3Element) {
    auto polygons = GeometryUtils::findAllPolygons(cityObject, LOD);
    for(auto poly : polygons){
        this->merge(*poly, options, UV2Element, UV3Element);
    }
}

void
Mesh::mergePolygonsInCityObjects(const std::list<const CityObject *> &cityObjects, int LOD, const TVec2f &UV3Element,
                                 const MeshExtractOptions &options, const TVec2f &UV2Element) {
    for(auto obj : cityObjects){
        mergePolygonsInCityObject(*obj, LOD, options, UV2Element, UV3Element);
    }
}

void Mesh::mergeWithTexture(const Polygon &otherPoly, const MeshExtractOptions &options, const TVec2f &UV2Element,
                            const TVec2f &UV3Element) {
    if(!isValidPolygon(otherPoly)) return;
    mergeShape(otherPoly, UV2Element, UV3Element, options);
    addSubMesh(otherPoly);
}

void Mesh::mergeWithoutTexture(const Polygon &otherPoly, const TVec2f &UV2Element, const TVec2f &UV3Element,
                               const MeshExtractOptions &options) {
    if(!isValidPolygon(otherPoly)) return;
    mergeShape(otherPoly, UV2Element, UV3Element, options);
    extendLastSubMesh();

}

void Mesh::mergeShape(const Polygon &otherPoly, const TVec2f &UV2Element, const TVec2f &UV3Element,
                      const MeshExtractOptions &options) {
    unsigned prevNumVertices = vertices_.size();
    auto& otherVertices = otherPoly.getVertices();
    auto& otherIndices = otherPoly.getIndices();

    addVerticesList(otherVertices, options);
    addIndicesList(otherIndices, prevNumVertices);
    addUV1(otherPoly);
    addUV2WithSameVal(UV2Element, otherVertices.size());
    addUV3WithSameVal(UV3Element, otherVertices.size());
}

void Mesh::addVerticesList(const std::vector<TVec3d> &otherVertices, const MeshExtractOptions &options) {
    // 各頂点を座標変換しながら追加します。
    for(const auto & otherPos : otherVertices){
        // otherPos は極座標系です。
        auto pos = otherPos;
        // デカルト座標系に直します。
        // FIXME 変換部分だけ ObjWriterの機能を拝借していますが、本質的には ObjWriter である必要はないです。変換を別クラスに書き出した方が良いです。
        // TODO 座標変換については、今後は自作せずライブラリを利用する方針になったのでライブラリ導入後にここを直します。
        polar_to_plane_cartesian().convert(pos);
        // オプションに応じて座標系を変更します。
        pos = ObjWriter::convertPosition(pos, options.referencePoint, options.meshAxes, options.unitScale);
        vertices_.push_back(pos);
    }
}

void Mesh::addIndicesList(const std::vector<unsigned>& otherIndices, unsigned prevNumVertices){
    auto prevNumIndices = indices_.size();

    // インデックスリストの末尾に追加します。
    for(unsigned int otherIndex : otherIndices){
        indices_.push_back((int)otherIndex);
    }
    // 追加分のインデックスを新しい値にします。以前の頂点の数だけインデックスの数値を大きくすれば良いです。
    for(unsigned i=prevNumIndices; i<indices_.size(); i++){
        indices_.at(i) += (int)prevNumVertices;
    }
}

void Mesh::addUV1(const Polygon& otherPoly){
    // UV1を追加します。
    auto& otherUV1 = otherPoly.getTexCoordsForTheme("rgbTexture", true);
    for(const auto & vec : otherUV1){
        uv1_.push_back(vec);
    }
    // otherUV1 の数が頂点数に足りなければ 0 で埋めます。
    auto otherVerticesSize = otherPoly.getVertices().size();
    for(size_t i = otherUV1.size(); i < otherVerticesSize; i++){
        uv1_.emplace_back(0,0);
    }
}

void Mesh::addUV2WithSameVal(const TVec2f &UV2Val, unsigned numAddingVertices) {
    for(int i=0; i<numAddingVertices; i++){
        uv2_.push_back(UV2Val);
    }
}

void Mesh::addUV3WithSameVal(const TVec2f &UV3Val, unsigned numAddingVertices){
    for(int i=0; i<numAddingVertices; i++){
        uv3_.push_back(UV3Val);
    }
}

void Mesh::addSubMesh(const Polygon &otherPoly) {
    // テクスチャが異なる場合は追加します。
    // TODO テクスチャありのポリゴン と なしのポリゴン が交互にマージされることで、テクスチャなしのサブメッシュが大量に生成されるので描画負荷に改善の余地ありです。
    //      テクスチャなしのサブメッシュは1つにまとめたいところです。テクスチャなしのポリゴンを連続してマージすることで1つにまとまるはずです。
    auto otherTexture = otherPoly.getTextureFor("rgbTexture");
    std::string otherTexturePath;
    if(otherTexture == nullptr){
        otherTexturePath = std::string("");
    }else{
        otherTexturePath = otherTexture->getUrl();
    }

    // 前と同じテクスチャかどうか判定します。
    bool isDifferentTex;
    if(subMeshes_.empty()){
        isDifferentTex = true;
    }else{
        auto& lastTexturePath = subMeshes_.rbegin()->getTexturePath();
        isDifferentTex = otherTexturePath != lastTexturePath;
    }

    if( isDifferentTex ){
        // テクスチャが違うなら、サブメッシュを追加します。
        unsigned prevNumIndices = indices_.size() - otherPoly.getIndices().size();
        SubMesh::addSubMesh(prevNumIndices, indices_.size()-1, otherTexturePath, subMeshes_);
    }else{
        // テクスチャが同じなら、最後のサブメッシュの範囲を延長して新しい部分の終わりに合わせます。
        extendLastSubMesh();
    }
}

void Mesh::extendLastSubMesh() {
    if(subMeshes_.empty()){
        subMeshes_.emplace_back(0, indices_.size()-1, "");
    }else {
        subMeshes_.at(subMeshes_.size() - 1).setEndIndex((int) indices_.size() - 1);
    }
}

bool Mesh::isValidPolygon(const Polygon& otherPoly){
    return !(otherPoly.getVertices().empty() || otherPoly.getIndices().empty());
}
