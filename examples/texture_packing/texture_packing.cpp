
#include <plateau/texture/atlas_manager.h>
#include <plateau/texture/jpeg.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>


using namespace plateau::texture;

int
main( int argc, char* argv[] )
{
    std::chrono::system_clock::time_point timer1;
    std::chrono::system_clock::time_point timer2;
    std::chrono::system_clock::time_point timer3;

    const int range = 1024 * 2; // 画像を詰め込む領域(range x range)の指定

    AtlasManager manager(range, range); // 画像パッキングクラスの初期化

    if ( argc < 2 ) {
        return 1;
    }
    try {

        // テクスチャをパックする領域の大きさ(例:2048x2048)を指定、第3引数の整数は領域を塗りつぶす色(例: R:255、G:255、B:255)のグレースケール値
        TextureImage canvas(range, range, 255);

        // コマンドラインの第1引数に画像が格納されているフォルダのパスが必要
        auto path = std::filesystem::path(argv[1]);
        auto directory = path;

        std::vector<TextureImage> imageList;

        timer1 = std::chrono::system_clock::now(); // 処理時間計測用タイマーの設定

        auto index = 0;

        for (const auto& file : std::filesystem::directory_iterator(directory)) {
            auto fileName = file.path().generic_string();
            auto image = TextureImage(fileName);
            auto imageWidth = image.getWidth();
            auto imageHeight = image.getHeight();

            if (imageHeight > 2048)
            {
                // 大きな画像(例: 4096x4096)はスキップ
                continue;
            }

            // パッキング領域の検索、「info」構造体の「valid」ブール値(true:成功、false:失敗)で判定可能
            auto info = manager.insert(imageWidth, imageHeight);

            std::cout << ++index << " : " << "COVERAGE: " << manager.getCoverage() << " LEFT: " << info.getLeft() << " TOP: " << info.getTop() << " WIDTH: " << info.getWidth() << " HEIGHT: " << info.getHeight() << " : " << file.path() << std::endl;

            if (info.getValid())
            {
                // 見つかったパッキングの領域に画像をコピー
                canvas.pack(info.getLeft(), info.getTop(), image);
            }
            else {
                // パッキングする十分な空間が見つからないので終了
                break;
            }
        }
        timer2 = std::chrono::system_clock::now();
        canvas.save("canvas.jpg");
        timer3 = std::chrono::system_clock::now();

        //std::cout << "Packing Time     : " << std::format("{:5.3f}", std::chrono::duration_cast<std::chrono::milliseconds>(timer2 - timer1).count() / 1000.0) << " sec" << std::endl;
        //std::cout << "Canvas Save Time : " << std::format("{:5.3f}", std::chrono::duration_cast<std::chrono::milliseconds>(timer3 - timer2).count() / 1000.0) << " sec" << std::endl;
        //std::cout << std::format("{:=^30}", "") << std::endl;
        //std::cout << "Total Time       : " << std::format("{:5.3f}", std::chrono::duration_cast<std::chrono::milliseconds>(timer3 - timer1).count() / 1000.0) << " sec" << std::endl;
    }
    catch( const std::exception& e ) {
        std::cout << "ERROR: " <<  e.what() << std::endl;
        return 1;
    }

    return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          