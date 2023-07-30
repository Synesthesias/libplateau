
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>

#include <plateau/texture/jpeg_image_reader.h>
//#include <plateau/texture/png_image_reader.h>

using namespace std::filesystem;
using namespace jpeg;
//using namespace png;

namespace imageReader
{

class TextureImage
{
public:
    enum class TextureType { Jpeg, Png, Tiff };

public:

    explicit TextureImage() {}
    explicit TextureImage(const std::string& fileName);
    explicit TextureImage(size_t width, size_t height, size_t gray);
    ~TextureImage() {}

    void init(size_t width, size_t height, unsigned char gray);

    size_t getWidth() const { return imageWidth; }
    size_t getHeight() const { return imageHeight; }
    TextureType getTextureType() { return textureType; }
    std::string getImageFilePath() { return imageFilePath; }
    JpegTextureImage& getJpegImage() { return jpegImage; }
    //PngTextureImage& getPngImage() { return pngImage; }

    void save(const std::string& fileName) const;
    void pack(size_t xdelta, size_t ydelta, TextureImage& image);
    void pack(size_t xdelta, size_t ydelta, JpegTextureImage& image, JpegTextureImage& targetImage);

    void DisplayPathInfo(std::string fileName);

private:
    TextureType textureType;
    JpegTextureImage jpegImage;
    //PngTextureImage pngImage;
    size_t imageWidth;
    size_t imageHeight;
    unsigned char imageColor;

    const std::string pngExtension = ".png";
    const std::string jpegExtension = ".jpg";

    std::string imageFilePath;

private:
    bool checkFileExtension(const std::string& fileName, const std::string& fileExtension);
};

} // namespace texture


