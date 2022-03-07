#include <string>

#include <citygml/citygml.h>
#include <citygml/citygmllogger.h>

#include <obj_writer.h>

class StdLogger : public citygml::CityGMLLogger {
public:

    StdLogger(LOGLEVEL level = LOGLEVEL::LL_ERROR)
    : citygml::CityGMLLogger(level) {}

    virtual void log(LOGLEVEL level, const std::string& message, const char* file, int line) const
    {
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


int main()
{
    std::shared_ptr<const citygml::CityModel> city_model;
    std::string gml_file_path = "../../data/53392642_bldg_6697_op2.gml";

    const citygml::ParserParams params;
    const auto logger = std::make_shared<StdLogger>();
    try {
        city_model = citygml::load(gml_file_path, params, logger);
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cout << "Unknown error occurred. GML file might not exist." << std::endl;
        return 1;
    }
    ObjWriter().write("test.obj", *city_model, gml_file_path);

    return 0;
}
