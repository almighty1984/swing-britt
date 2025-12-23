module;
#include <SFML/Graphics/Shader.hpp>
#include <filesystem>
#include <string>

export module shader;
import types;

export class Shader {
    Vec2F m_position;
    U16 m_w, m_h;
    sf::Shader m_sfml_shader;
public:
    const sf::Shader* get_sfml_shader() { return &m_sfml_shader; }

    Shader() : m_w(0), m_h(0), m_position(0.0F, 0.0F) {
        m_sfml_shader.setUniform("texture", sf::Shader::CurrentTexture);
    }        
    void color(Color c) {
        m_sfml_shader.setUniform("color", sf::Vector3i(c.r, c.g, c.b));
    }
    void set_position(cVec2F p) {
        m_position = p;
    }
    void set_width(cU16 w) { m_w = w; }
    void set_height(cU16 h) { m_h = h; }

    bool load(const std::filesystem::path& path_0, const std::filesystem::path& path_1) {
        if (path_0.empty() || path_1.empty()) return false;

        std::filesystem::path path_vert, path_frag;
        if (!path_0.empty() && path_0.string().size() > 4) {
            if (path_0.string().substr(path_0.string().size() - 4, 4) == "vert") {
                path_vert = path_0;
            } else if (path_0.string().substr(path_0.string().size() - 4, 4) == "frag") {
                path_frag = path_0;
            }
        }
        if (!path_1.empty() && path_1.string().size() > 4) {
            if (path_1.string().substr(path_1.string().size() - 4, 4) == "vert") {
                path_vert = path_1;
            } else if (path_1.string().substr(path_1.string().size() - 4, 4) == "frag") {
                path_frag = path_1;
            }
        }
        return m_sfml_shader.loadFromFile(path_vert, path_frag);
    }        

};