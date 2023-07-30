

#include <plateau/texture/png_image_reader.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3


namespace png
{


    void abort_(const char* s, ...)
    {
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
    }

    //size_t PngTextureImage::x;
    //size_t PngTextureImage::y;
    //size_t PngTextureImage::width;
    //size_t PngTextureImage::height;
    //png_byte PngTextureImage::color_type;
    //png_byte PngTextureImage::bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    size_t number_of_passes;
    png_bytep* row_pointers;

    void
    PngTextureImage::init(const std::string& fileName)
    {
        png_byte header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE* fp = fopen(fileName.c_str(), "rb");
        if (!fp)
            abort_("[read_png_file] File %s could not be opened for reading", fileName);
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8))
            abort_("[read_png_file] File %s is not recognized as a PNG file", fileName);


        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
            abort_("[read_png_file] png_create_read_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
            abort_("[read_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
            abort_("[read_png_file] Error during init_io");

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);


        /* read file */
        if (setjmp(png_jmpbuf(png_ptr)))
            abort_("[read_png_file] Error during read_image");


        bitmapData.clear();
        bitmapData.reserve(height);

        row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
        for (y = 0; y < height; y++) {
            auto size = png_get_rowbytes(png_ptr, info_ptr);
            row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
        }

        png_read_image(png_ptr, row_pointers);

        //auto row_stride = png_get_rowbytes(png_ptr, info_ptr);

        //for (y = 0; y < height; y++) {
        //    std::vector<uint8_t> vec(row_stride);
        //    bitmapData.push_back(vec);
        //}

        //row_pointers = (unsigned char**)malloc(sizeof(unsigned char) * height);
        //for (int y = 0; y < height; y++) {
        //    row_pointers[y] = (unsigned char*)malloc(png_get_rowbytes(png_ptr, info_ptr));

        //}
        //png_read_image(png_ptr, row_pointers);

        //unsigned char* image = (unsigned char*)malloc(3 * width * height);
        //int count = 0;
        //for (auto i = 0; i < height; i++)
        //{
        //    for (auto j = 0; j < 3 * width; j++)
        //    {
        //        image[count] = row_pointers[i][j];
        //        count += 1;
        //    }
        //}

        bitmapData.clear();
        bitmapData.reserve(height);

        auto row_stride = png_get_rowbytes(png_ptr, info_ptr);
        unsigned char* image = (unsigned char*)malloc(row_stride);
        std::vector<uint8_t> vec(row_stride);

        for (auto row = 0; row < height; row++) {
            auto index = 0;

            for (auto col = 0; col < row_stride; col++) {
                vec[index++] = row_pointers[row][col];
            }
            bitmapData.push_back(vec);
        }

        fclose(fp);
    }

    //row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    //for (y = 0; y < height; y++) {
    //    auto size = png_get_rowbytes(png_ptr, info_ptr);
    //    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
    //}

    //std::vector<std::vector<uint8_t>>&
    //PngTextureImage::getBitmapData() {

    //    //std::vector<std::vector<uint8_t>> bitmapData;


    //    size_t row_stride = png_get_rowbytes(png_ptr, info_ptr);

    //    bitmapData.clear();
    //    bitmapData.reserve(height);

    //    std::vector<uint8_t> vec(row_stride, 0);

    //    //int index = 0;
    //    //for (int column = 0; column < width; ++column) {
    //    //    vec[index++] = color;
    //    //    vec[index++] = color;
    //    //    vec[index++] = color;
    //    //}

    //    for (int row = 0; row < height; ++row) {

    //        auto a1 = row_pointers;
    //        auto a2 = row_pointers[row];
    //        auto a3 = &row_pointers[row];
    //        //auto a4 = *row_pointers[y];

    //        png_bytep* colPtr = &row_pointers[row];

    //        //for (int col = 0; col < row_stride; ++col) {
    //            //vec.push_back(&static_cast<uint8_t>(colPtr[col]));

    //            //memcpy(row_pointers[row], vec, row_stride);
    //        //}
    //        bitmapData.push_back(vec);

    //        png_bytep sss;
    //        uint8_t xxx;

    //        //xxx = static_cast<uint8_t>(sss);
    //    }

    //    return bitmapData;        
    //}


    //void
    //PngTextureImage::pack(size_t xdelta, size_t ydelta, const PngTextureImage& image, JpegTextureImage& targetImage) {
    //    auto srcHeight = image.getHeight();

    //    auto fromPtr = image.bitmapData.data();
    //    auto toPtr = targetImage.getBitmapData().data();

    //    for (auto y = 0; y < srcHeight; ++y) {
    //        std::copy(fromPtr[y].begin(), fromPtr[y].end(), toPtr[ydelta + y].begin() + xdelta * 3);
    //    }
    //}

    //void write_png_file(char* file_name)
    //{
    //    /* create file */
    //    FILE* fp = fopen(file_name, "wb");
    //    if (!fp)
    //        abort_("[write_png_file] File %s could not be opened for writing", file_name);


    //    /* initialize stuff */
    //    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    //    if (!png_ptr)
    //        abort_("[write_png_file] png_create_write_struct failed");

    //    info_ptr = png_create_info_struct(png_ptr);
    //    if (!info_ptr)
    //        abort_("[write_png_file] png_create_info_struct failed");

    //    if (setjmp(png_jmpbuf(png_ptr)))
    //        abort_("[write_png_file] Error during init_io");

    //    png_init_io(png_ptr, fp);


    //    /* write header */
    //    if (setjmp(png_jmpbuf(png_ptr)))
    //        abort_("[write_png_file] Error during writing header");

    //    png_set_IHDR(png_ptr, info_ptr, width, height,
    //        bit_depth, color_type, PNG_INTERLACE_NONE,
    //        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    //    png_write_info(png_ptr, info_ptr);


    //    /* write bytes */
    //    if (setjmp(png_jmpbuf(png_ptr)))
    //        abort_("[write_png_file] Error during writing bytes");

    //    png_write_image(png_ptr, row_pointers);


    //    /* end write */
    //    if (setjmp(png_jmpbuf(png_ptr)))
    //        abort_("[write_png_file] Error during end of write");

    //    png_write_end(png_ptr, NULL);

    //    /* cleanup heap allocation */
    //    for (y = 0; y < height; y++)
    //        free(row_pointers[y]);
    //    free(row_pointers);

    //    fclose(fp);
    //}


    //void process_file2(void)
    //{
    //    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
    //        abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
    //            "(lacks the alpha channel)");

    //    if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
    //        abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
    //            PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));

    //    for (y = 0; y < height; y++) {
    //        png_byte* row = row_pointers[y];
    //        for (x = 0; x < width; x++) {
    //            png_byte* ptr = &(row[x * 4]);
    //            printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
    //                x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

    //            /* set red value to 0 and green value to the blue one */
    //            ptr[0] = 0;
    //            ptr[1] = ptr[2];
    //        }
    //    }
    //}


    //int main(int argc, char **argv)
    //{
    //        if (argc != 3)
    //                abort_("Usage: program_name <file_in> <file_out>");
    //
    //        read_png_file(argv[1]);
    //        process_file();
    //        write_png_file(argv[2]);
    //
    //        return 0;
    //}
} // namespace png

