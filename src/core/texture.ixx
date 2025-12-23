module;
#include <SFML/Graphics.hpp>
#include <map>
#include <filesystem>

export module texture;
import console;

std::map<std::filesystem::path, sf::Texture*> s_textures;

export namespace texture {    
    sf::Texture* load(const std::filesystem::path& path) {
        if (path.empty()) {
            return nullptr;
        }
        auto it = s_textures.find(path);
        sf::Texture* temp_texture;
        if (it == s_textures.end()) {
            temp_texture = new sf::Texture;
            if (!temp_texture->loadFromFile(path)) {
                console::error("texture::load: ", path, " not found\n");
                delete temp_texture;
                return nullptr;
            }
            temp_texture->setSmooth(false);
            s_textures.emplace(path, temp_texture);
            return s_textures.at(path);
        }
        return it->second;
    }
    void clear() {
        for (auto& i : s_textures) {
            if (i.second) delete i.second;
        }
        s_textures.clear();
    }
}