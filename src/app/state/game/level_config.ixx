module;
#include <fstream>
#include <filesystem>
#include <map>
#include <string>

export module level_config;
import console;
import music;
import types;

export class LevelConfig {
    static inline std::filesystem::path m_level_path_L_0,
                                        m_level_path_R_0,
                                        m_level_music_path,
                                        m_prev_level_music_path;

    //static inline std::map<U8, std::filesystem::path> m_trigger_paths;

    static inline I32 m_music_id = -1;

public:
    ~LevelConfig() {
        music::erase(m_music_id);        
    }
    static void clear_music() {
        music::erase(m_music_id);
        m_prev_level_music_path.clear();
    }
    /*static void clear_paths() {
        m_trigger_paths.clear();
    }*/
    static void stop_music() {
        music::stop(m_music_id);
    }
    static std::filesystem::path level_path(start::Type start) {
        switch (start) {
        case start::Type::L_0:    return m_level_path_L_0;
        case start::Type::R_0:    return m_level_path_R_0; }
        return {};
    }
    /*static std::filesystem::path trigger_path(cU8 number) {
        if (m_trigger_paths.find(number) != m_trigger_paths.cend()) {
            return m_trigger_paths.at(number);
        }
        return {};
    }*/
    static void load(const std::filesystem::path& path) {
        std::string level_name = path.string();

        if (level_name.substr(0, 10) != "res/level/") {
            return;
        }
        level_name.erase(0, 10);

        console::log("state::Game::load_level_config: ", level_name, "\n");

        std::ifstream in_file("res/levels.cfg");
        if (!in_file) {
            console::error("state::Game::load_level_config levels.cfg not found!\n");
            return;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();

        const std::string text = oss.str();

        size_t level_open = 0;
        size_t level_name_start = 0;
        while (text.at(level_open) != '{') {
            level_name_start = text.find(level_name, level_name_start);
            if (level_name_start == std::string::npos) {
                console::error("state::Game::load_level_config ", level_name, " not found!\n");
                return;
            }
            size_t level_name_end = text.find(".bin", level_name_start);
            level_name_end += 4;

            level_open = level_name_end;
            while (text.at(level_open) == ' ' || text.at(level_open) == '	' || text.at(level_open) == '\n') {
                ++level_open;
            }
            console::log("level_open char: ", text.at(level_open), "\n");
            ++level_name_start;
        }
        ++level_open;
        console::log("level_open pos: ", level_open, "\n");
        size_t level_close = text.find("\n}", level_open);
        if (level_close != std::string::npos) {
            const size_t L_0_label = text.find("L_0", level_open);
            if (L_0_label < level_close) {
                const size_t end_line = text.find("\n", L_0_label);
                size_t L_0_start = text.find("=", L_0_label);
                if (L_0_start < end_line) {
                    ++L_0_start;
                    while (text.at(L_0_start) == '	' || text.at(L_0_start) == ' ') ++L_0_start;

                    m_level_path_L_0 = "res/level/" + text.substr(L_0_start, end_line - L_0_start);

                    console::log("state::Game::load_level_config L_0: ", m_level_path_L_0, "\n");
                }
            }
            const size_t R_0_label = text.find("R_0", level_open);
            if (R_0_label < level_close) {
                const size_t end_line = text.find("\n", R_0_label);
                size_t R_0_start = text.find("=", R_0_label);
                if (R_0_start < end_line) {
                    ++R_0_start;
                    while (text.at(R_0_start) == '	' || text.at(R_0_start) == ' ') ++R_0_start;
                    m_level_path_R_0 = "res/level/" + text.substr(R_0_start, end_line - R_0_start);

                    console::log("state::Game::load_level_config R_0: ", m_level_path_R_0, "\n");
                }
            }
            const size_t music_label = text.find("music", level_open);
            if (music_label < level_close) {
                const size_t end_line = text.find("\n", music_label);
                size_t music_start = text.find("=", music_label);
                if (music_start < end_line) {
                    ++music_start;
                    while (text.at(music_start) == '	' || text.at(music_start) == ' ') ++music_start;
                    m_level_music_path = "res/music/" + text.substr(music_start, end_line - music_start);
                    console::log("state::Game::load_level_config music path: ", m_level_music_path, " prev: ", m_prev_level_music_path, "\n");

                    if (m_level_music_path != m_prev_level_music_path) {
                        music::erase(m_music_id);
                        m_prev_level_music_path = m_level_music_path;

                        m_music_id = music::make(m_level_music_path);

                        music::is_looped(m_music_id, true);
                        music::volume(m_music_id, 5.0F);
                        music::play(m_music_id);
                    }
                }
            }
            /*const size_t trigger_label = text.find("trigger", level_open);
            if (trigger_label < level_close) {
                const size_t end_line = text.find("\n", trigger_label);
                size_t number_start = text.find("_", trigger_label);
                ++number_start;
                size_t number_end = number_start;
                while (text.at(number_end) != '	' && text.at(number_end) != ' ') ++number_end;
                cU8 number = std::stoi(text.substr(number_start, number_end - number_start));
                size_t value_start = text.find("=", trigger_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' || text.at(value_start) == ' ') ++value_start;
                    m_trigger_paths.emplace(number, "res/entity/" + text.substr(value_start, end_line - value_start));
                    console::log("state::Game::load_level_config m_trigger_paths[0]: ", m_trigger_paths.at(number), "\n");
                }
            }*/
        }
    }
};