module;
#include <filesystem>
#include <fstream>

export module config;
import console;
import types;

export class Config {
    static inline U8          s_scale      = 1;
    static inline state::Type s_state_type = state::Type::GAME;

public:
    static U8 scale() { return s_scale; }
    static state::Type state_type() { return s_state_type; }

    static bool load(const std::filesystem::path& path) {
        std::ifstream in_file(path);
        if (!in_file) {
            console::error("config::load ", path, " not found\n");
            return false;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();
        std::string text = oss.str();
        std::string layer_string = state::to_string(s_state_type);

        size_t start_tag = text.find("start_state", 0);
        size_t i = 0;
        if (start_tag != std::string::npos) {
            size_t end_line = text.find("\n", start_tag);
            size_t equals = text.find("=", start_tag);
            if (equals < end_line) {
                i = equals + 1;
                while (i < end_line && (text.at(i) == '=' || text.at(i) == ' ' || text.at(i) == '	')) {
                    ++i;
                }
                layer_string = text.substr(i, end_line - i);
                console::log("config::load found start: ", layer_string.c_str(), "\n");
            }
        }
        s_state_type = state::type_from(layer_string);

        size_t scale_label = text.find("scale", 0);
        i = 0;
        if (scale_label != std::string::npos) {
            size_t end_line = text.find("\n", scale_label);
            size_t equals = text.find("=", scale_label);
            if (equals < end_line) {
                i = equals + 1;
                while (i < end_line && (text.at(i) == '=' || text.at(i) == ' ' || text.at(i) == '	')) {
                    ++i;
                }
                s_scale = std::stoi(text.substr(i, end_line - i));                
                console::log("config::load found scale: ", (int)s_scale, "\n");
            }
        }

        /*const size_t tag_start = text.find(state_string, 0);
        if (tag_start == std::string::npos) {
            console::error("config::load ", state_string.c_str(), " not found\n");
            return false;
        }
        const size_t bracket_open = text.find("{", tag_start);
        if (tag_start == std::string::npos) {
            console::error("config::load ", state_string.c_str(), " { not found\n");
            return false;
        }
        const size_t body_end = text.find("}", bracket_open);
        if (body_end == std::string::npos) {
            console::error("config::load ", state_string.c_str(), " } not found\n");
            return false;
        }

        i = bracket_open + 1;
        while (i < body_end && (text.at(i) == '{' || text.at(i) == ' ' || text.at(i) == '	' || text.at(i) == '\n')) {
            ++i;
        }
        const size_t body_start = i;

        size_t end_line = text.find("\n", i);

        console::log("text:", text.substr(i, end_line - i).c_str(), "\n");

        size_t var_tag = text.find("scale", i);
        if (var_tag > body_end) {
            var_tag = body_start;
        } else {
            size_t equals = text.find("=", i);
            if (var_tag < end_line) {
                i = equals + 1;
                while (i < end_line && (text.at(i) == '=' || text.at(i) == ' ' || text.at(i) == '	')) {
                    ++i;
                }
                cI32 scale = std::stoi(text.substr(i, end_line - i));
                console::log("Config::load scale: ", scale, "\n");
                if (scale > 0 && scale < 5) {
                    m_scale = (U8)scale;
                }
            }
        }*/





        //m_scale = 4;
        //return Config{ .scale = 4 };

        return true;
    }

};