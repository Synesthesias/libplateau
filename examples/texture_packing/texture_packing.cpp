#include <string>
#include <filesystem>

#include <citygml/citygml.h>
#include <citygml/citygmllogger.h>

#include <plateau/polygon_mesh/mesh_extractor.h>

#include <plateau/mesh_writer/obj_writer.h>

namespace fs = std::filesystem;
using namespace citygml;
namespace {
    class StdLogger : public citygml::CityGMLLogger {
    public:

        StdLogger(LOGLEVEL level = LOGLEVEL::LL_ERROR)
            : CityGMLLogger(level) {
        }

        void log(LOGLEVEL level, const std::string& message, const char* file, int line) const override {
            std::ostream& stream = level == LOGLEVEL::LL_ERROR ? std::cerr : std::cout;

            switch (level) {
            case LOGLEVEL::LL_DEBUG:
                stream << "DEBUG";
                break;
            case LOGLEVEL::LL_WARNING:
                stream << "WARNING";
                break;
            case LOGLEVEL::LL_TRACE:
                stream << "TRACE";
                break;
            case LOGLEVEL::LL_ERROR:
                stream << "ERROR";
                break;
            case LOGLEVEL::LL_INFO:
                stream << "INFO";
                break;
            }

            if (file) {
                stream << " [" << file;
                if (line > -1) {
                    stream << ":" << line;
                }
                stream << "]";
            }

            stream << " " << message << std::endl;
        }
    };
}

int main() {
    try {
        const auto logger = std::make_shared<StdLogger>();
        ParserParams params_;
        //const std::string gml_file_path = "../data/日本語パステスト/udx/bldg/53392642_bldg_6697_op2.gml";
        const std::string gml_file_path = "../data/dataset/udx/bldg/53392642_bldg_6697_op2.gml";
        //const std::string gml_file_path = "../data/日本語パステスト/udx/bldg/52350420_bldg_6697.gml";
        plateau::polygonMesh::MeshExtractOptions mesh_extract_options_;
        std::shared_ptr<const CityModel> city_model_ = load(gml_file_path, params_);

        mesh_extract_options_.enable_texture_packing = true;

        auto model = plateau::polygonMesh::MeshExtractor::extract(*city_model_, mesh_extract_options_);

        const auto destination = fs::path(".").string();
        const auto gml_file_name = fs::path(gml_file_path).filename().string();
        const auto base_obj_name = fs::path(gml_file_name).replace_extension(".obj").string();
        const auto obj_file_path = fs::path(destination).append(base_obj_name).make_preferred().string();

        auto result = plateau::meshWriter::ObjWriter().write(obj_file_path, *model);
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cout << "Unknown error occurred. GML file might not exist." << std::endl;
        return 1;
    }

    return 0;
}
