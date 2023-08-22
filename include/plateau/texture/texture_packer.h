
#pragma once

#include <plateau/polygon_mesh/model.h>
#include <plateau/texture/image_reader.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>

using namespace plateau::polygonMesh;
using namespace image_reader;

namespace plateau::texture {

class AtlasInfo {
public:

    explicit AtlasInfo() : valid(false), left(0), top(0), width(0), height(0), uPos(0), vPos(0), uFactor(0), vFactor(0) {}
    ~AtlasInfo() {}

    size_t getLeft() const { return left; }
    size_t getTop() const { return top; }
    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }
    double getUPos() const { return uPos; }
    double getVPos() const { return vPos; }
    double getUFactor() const { return uFactor; }
    double getVFactor() const { return vFactor; }

    bool getValid() const;
    void clear();
    void set_atlas_info(const bool _valid, const size_t _left, const size_t _top, const size_t _width, const size_t _height,
        double uPos, double vPos, double uFactor, double vFactor);

private:
    bool valid;     // �p�b�L���O�������A���s�̃t���O�iTRUE:�����AFALSE:���s�j
    size_t left;    // �p�b�L���O���ꂽ�摜�̍����X���W
    size_t top;     // �p�b�L���O���ꂽ�摜�̍����Y���W
    size_t width;   // �p�b�L���O���ꂽ�摜�̕�
    size_t height;  // �p�b�L���O���ꂽ�摜�̍���
    double uPos;    // uv���W�̍���u���W
    double vPos;    // uv���W�̍���v���W
    double uFactor; // u���W�̔{��
    double vFactor; // v���W�̔{��
};

class AtlasContainer {
public:

    explicit AtlasContainer(const size_t _gap, const size_t _horizontal_range, const size_t _vertical_range);
    ~AtlasContainer() {}

    size_t getGap() const { return gap; }
    size_t getRootHorizontalRange() const { return root_horizontal_range; }
    size_t getHorizontalRange() const { return horizontal_range; }
    size_t getVerticalRange() const { return vertical_range; }
    void add(const size_t _length);

private:
    size_t gap;                     // �摜���i�[����R���e�i�̍���
    size_t root_horizontal_range;   // �R���e�i���Ŗ��g�p�̗̈��X���W
    size_t horizontal_range;        // �Ō�̉摜���p�b�L���O���邽�߂̗̈��X���W
    size_t vertical_range;          // �p�b�L���O�̑ΏۂƂȂ�e�̉摜�̃R���e�i���z�u����Ă��鍶���Y���W
};

class TexturePacker {
public:
    const int defaultResolution = 2048;
    const unsigned char gray = 80;

    explicit TexturePacker() : canvas_width(0), canvas_height(0), vertical_range(0), capacity(0), coverage(0) {
    }
    explicit TexturePacker(size_t width, size_t height) : vertical_range(0), capacity(0), coverage(0) {
        canvas_width = width;
        canvas_height = height;
        canvas.init(width, height, gray);
    }
    ~TexturePacker() {
    }

    void setSaveFilePath(std::string fileName);

    void init(size_t width, size_t height);
    void clear();
    void flush();

    void process(Model& model);
    void processNodeRecursive(const Node& node);

    double getCoverage() const {
        return coverage;
    } // �Ăяo�����̃e�N�X�`���S�̂ɑ΂��Ă̊��Ƀp�b�N���ꂽ�摜�̐�L���i100%�j
    void update(const size_t _width, const size_t _height, const bool _is_new_container); // �摜�̃p�b�L���O�������̏����A��3�����iTRUE:�V�K�R���e�i���쐬�AFALSE:�����R���e�i�ɒǉ��j
    const AtlasInfo insert(const size_t _width, const size_t _height); // �w�肳�ꂽ�摜�̈�iwidth x height�j�̗̈悪�m�ۂł��邩���؁A�߂�lAtrasInfo�́uvalid�v�u�[���l�itrue:�����Afalse:���s�j�Ŕ���\

private:
    static int serialNumber;
    std::vector<AtlasContainer> container_list;
    AtlasInfo atlas_info;
    size_t canvas_width;
    size_t canvas_height;
    size_t vertical_range;
    size_t capacity;
    double coverage;
    TextureImage canvas;
    std::filesystem::path saveFilePath;
};
} // namespace plateau::texture


