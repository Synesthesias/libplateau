#include <string>
#include <filesystem>

#include <citygml/citygml.h>
#include <citygml/citygmllogger.h>

#include <plateau/io/mesh_converter.h>
#include <plateau/io/mesh_convert_options_factory.h>

namespace fs = std::filesystem;

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
        fs::path test_data_root_path = "../data/udx/bldg";
        test_data_root_path.make_preferred();
        citygml::ParserParams params;
        params.optimize = true;
        MeshConverter converter;
        MeshConvertOptions options;
        options.mesh_axes = AxesConversion::WUN;
        bool first_gml = true;
        options.mesh_granularity = MeshGranularity::PerPrimaryFeatureObject;
        options.mesh_file_format = MeshFileFormat::GLTF;

        for (const auto& entry : fs::directory_iterator(test_data_root_path)) {
            if (entry.path().extension() != ".gml") {
                continue;
            }
            const auto city_model = load(entry.path().string(), params, logger);
            if (first_gml) {
                MeshConvertOptionsFactory::setValidReferencePoint(options, *city_model);
                converter.setOptions(options);
                first_gml = false;
            }
            auto gml_path = entry.path().string();
            converter.convert(".", gml_path, city_model);
        }
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
