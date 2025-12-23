module;
#include <SFML/Audio.hpp>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <memory>

export module sound;
import console;
import types;

std::unordered_map<std::filesystem::path, std::unique_ptr<sf::SoundBuffer>> sound_buffers;

sf::SoundBuffer& load_sound_buffer(const std::filesystem::path& path) {
    if (sound_buffers.find(path) == sound_buffers.end()) {
        sound_buffers[path] = std::make_unique<sf::SoundBuffer>(path);
    }
    return *sound_buffers[path];
}

class Sound {
    std::unique_ptr<sf::Sound> m_sound;
    std::filesystem::path m_path;
    Vec2F m_position;
public:
    I32 id = -1;
    //Sound() {}
    Sound(const std::filesystem::path& path) {
        load(path);
    }
    void load(const std::filesystem::path& path) {
        m_path = path;
        m_sound = std::make_unique<sf::Sound>(load_sound_buffer(path));        
    }
    ~Sound() {
        m_sound.release();
    }
    cVec2F position() const { return m_position; }
    void position(cVec2F p) {
        if (!m_sound) return;
        m_position = p;
        m_sound->setPosition(sf::Vector3f{ p.x, p.y, 1.0F });
        
        //m_sound->setMinDistance(1.0F);
        //m_sound->setRelativeToListener(true);
    }
    cF32 offset() const { return m_sound->getPlayingOffset().asSeconds(); }
    bool is_looped() const { return m_sound->isLooping(); } void is_looped(const bool b) { m_sound->setLooping(b); }

    void play()  { if (m_sound) m_sound->play();  } bool is_playing() const { return m_sound->getStatus() == sf::SoundSource::Status::Playing; }
    void stop()  { if (m_sound) m_sound->stop();  } bool is_stopped() const { return m_sound->getStatus() == sf::SoundSource::Status::Stopped; }
    void pause() { if (m_sound) m_sound->pause(); } bool is_paused()  const { return m_sound->getStatus() == sf::SoundSource::Status::Paused; }
    
    cF32 pitch() const { return m_sound ? m_sound->getPitch() : 0.0F; } void pitch(cF32 p) { if (m_sound) m_sound->setPitch(p); }
};

std::vector<Sound*> s_sounds;
std::vector<I32>    s_unused_ids;

export namespace sound {
    bool     is_valid(size_t i) { return (i >= 0 && i < s_sounds.size() && s_sounds.at(i)) ? true : false; }
    size_t   size() { return s_sounds.size(); }

    cI32 make(const std::filesystem::path& path) {
        //s_sounds.emplace_back(std::make_unique<Sound>(path));
        //return s_sounds.size() - 1;

        Sound* object = new Sound(path);
        if (!s_unused_ids.empty()) {
            object->id = s_unused_ids.back();
            s_unused_ids.pop_back();
            if (!s_sounds.empty() && object->id >= 0 && object->id < s_sounds.size() && s_sounds.at(object->id)) {
                delete s_sounds.at(object->id);
                s_sounds.at(object->id) = nullptr;
            }
        } else {
            object->id = s_sounds.size();
            s_sounds.emplace_back(nullptr);
        }
        s_sounds.at(object->id) = object;
        return object->id;
    }
    void play(cI32 i) {
        if (i >= 0 && i < s_sounds.size() && s_sounds.at(i)) {
            s_sounds.at(i)->play();
        }
    }
    void stop(cI32 i) {
        if (i >= 0 && i < s_sounds.size() && s_sounds.at(i)) {
            s_sounds.at(i)->stop();
        }
    }
    void pause(cI32 i) {
        if (i >= 0 && i < s_sounds.size() && s_sounds.at(i)) {
            s_sounds.at(i)->pause();
        }
    }
    bool is_looped(cI32 i) {
        return i >= 0 && i < s_sounds.size() && s_sounds.at(i) && s_sounds.at(i)->is_looped();
    }
    void is_looped(cI32 i, bool is) {
        if (i >= 0 && i < s_sounds.size() && s_sounds.at(i)) {
            s_sounds.at(i)->is_looped(is);
        }
    }
    bool is_playing(cI32 i) {
        return i >= 0 && i < s_sounds.size() && s_sounds.at(i) && s_sounds.at(i)->is_playing();
    }
    bool is_stopped(cI32 i) {
        return i >= 0 && i < s_sounds.size() && s_sounds.at(i) && s_sounds.at(i)->is_stopped();
    }
    bool is_paused(cI32 i) {
        return i >= 0 && i < s_sounds.size() && s_sounds.at(i) && s_sounds.at(i)->is_paused();
    }
    cF32 pitch(cI32 i) {
        return i >= 0 && i < s_sounds.size() && s_sounds.at(i) ? s_sounds.at(i)->pitch() : 0.0F;
    }
    void pitch(cI32 i, cF32 v) {
        if (i >= 0 && i < s_sounds.size() && s_sounds.at(i)) {
            s_sounds.at(i)->pitch(v);
        }
    }
    void position(cI32 i, cVec2F p) {
        if (i >= 0 && i < s_sounds.size() && s_sounds.at(i)) {
            s_sounds.at(i)->position(p);
        }
    }
    bool erase(cI32 i) {
        if (i < 0) return false;
        if (s_sounds.empty() || i > s_sounds.size() - 1 || !s_sounds.at(i)) {
            console::log("sound::erase ", i, " can't do it!\n");
            return false;
        }            
        //console::log("sound::erase ", i, "\n");
            
        if (s_sounds.at(i)->is_playing()) {
            s_sounds.at(i)->stop();
        }

        delete s_sounds.at(i);
        s_sounds.at(i) = nullptr;
        s_unused_ids.emplace_back(i);
        return true;
    }
    void clear() {
        for (I32 i = 0; i < s_sounds.size(); ++i) {
            //erase(i);
            if (s_sounds.at(i)) {
                erase(i);
            }
        }
        s_sounds.clear();
        s_unused_ids.clear();
    }    
}