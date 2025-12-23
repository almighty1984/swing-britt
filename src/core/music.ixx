module;
#include <SFML/Audio.hpp>
#include <filesystem>

export module music;
import console;
import types;

class Music {
    sf::Music m_sf_music;

public:
    Music(const std::filesystem::path& path) { open(path); }
    bool open(const std::filesystem::path& path) { return m_sf_music.openFromFile(path); }
    bool is_looped() const { return m_sf_music.isLooping(); } void is_looped(bool is) { m_sf_music.setLooping(is); }
    bool is_playing() const { return m_sf_music.getStatus() == sf::Music::Status::Playing; }
    cF32 volume()    const { return m_sf_music.getVolume(); } void volume(cF32 v) { m_sf_music.setVolume(v); }
    void play()  { m_sf_music.play();  }
    void stop()  { m_sf_music.stop();  }
    void pause() { m_sf_music.pause(); }
};

std::vector<std::unique_ptr<Music>> s_music;

#define IS_VALID(i) i >= 0 && i < s_music.size() && s_music.at(i)

export namespace music {
    //std::unique_ptr<Music>& get(size_t i) { return s_music.at(i); }
    bool    is_valid(size_t i) { return (i >= 0 && i < s_music.size() && s_music.at(i)) ? true : false; }
    size_t  size() { return s_music.size(); }

    cI32 make(const std::filesystem::path& path) {
        s_music.emplace_back(std::make_unique<Music>(path));
        return s_music.size() - 1;
    }
    F32 volume(cI32 i) { return IS_VALID(i) ? s_music.at(i)->volume() : 0.0F; }

    void play(cI32 i)               { if (IS_VALID(i))  s_music.at(i)->play();           }
    void stop(cI32 i)               { if (IS_VALID(i)) s_music.at(i)->stop();            }
    void pause(cI32 i)              { if (IS_VALID(i)) s_music.at(i)->pause();           }
    bool is_looped(cI32 i)          { return IS_VALID(i) && s_music.at(i)->is_looped();  }
    void is_looped(cI32 i, bool is) { if (IS_VALID(i)) s_music.at(i)->is_looped(is);     }
    bool is_playing(cI32 i)         { return IS_VALID(i) && s_music.at(i)->is_playing(); }
    void volume(cI32 i, cF32 v)     { if (IS_VALID(i)) s_music.at(i)->volume(v);         }

    bool erase(cI32 i) {
        if (i < 0) return false;
        console::log("music::erase ", i, "\n");
        if (s_music.empty() || i > s_music.size() - 1 || !s_music.at(i)) {
            return false;
        }
        if (s_music.at(i) && s_music.at(i)->is_playing()) {
            s_music.at(i)->stop();
        }
        s_music.at(i).release();
        return true;
    }
    void clear() {  
        for (auto& i : s_music) {
            if (i && i->is_playing()) {
                i->stop();
            }
            i.release();
        }
        s_music.clear();
    }
}