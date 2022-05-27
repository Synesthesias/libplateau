#include <pybind11/pybind11.h>
#include <string>


// TODO この処理はまとめたい
#if __has_include(<filesystem>)
#include <filesystem>
namespace filesystem = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
  namespace filesystem = std::experimental::filesystem;
#endif

#include <citygml/citygml.h>
#include <citygml/citygmllogger.h>

#include <obj_writer.h>

namespace py = pybind11;
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

PYBIND11_MODULE(python_wrapper, m)
{
    py::enum_<AxesConversion>(m, "AxesConversion")
        .value("WNU", AxesConversion::WNU)
        .value("RUF", AxesConversion::RUF);

    py::class_<ObjWriter>(m, "ObjWriter", py::dynamic_attr())
        .def(py::init())
        .def("write",
            [](ObjWriter& self, const std::string& gml_file_path, const std::string& obj_file_path ) {
                try {
                    const auto logger = std::make_shared<StdLogger>();                 
                    citygml::ParserParams params;
                    params.optimize = true;
                    const auto city_model = load(gml_file_path, params, logger);            
                    self.write(obj_file_path, *city_model, gml_file_path);
                }
                catch (std::exception& e) {
                    std::cout << e.what() << std::endl;
                }
                catch (...) {
                    std::cout << "Unknown error occurred. GML file might not exist." << std::endl;
                }
            }
        )
        .def("setValidReferencePoint",
            [](ObjWriter& self, const std::string& gml_file_path) {
                try {
                    const auto logger = std::make_shared<StdLogger>();
                    citygml::ParserParams params;
                    params.optimize = true;               
                    const auto city_model = load(gml_file_path, params, logger);
                    self.setValidReferencePoint(*city_model);
                }
                catch (std::exception& e) {
                    std::cout << e.what() << std::endl;
                }
                catch (...) {
                    std::cout << "Unknown error occurred. GML file might not exist." << std::endl;
                }
            }
        )
        .def("setDestAxes", &ObjWriter::setDestAxes)
        .def("setMergeMeshFlg", &ObjWriter::setMergeMeshFlg);
}
