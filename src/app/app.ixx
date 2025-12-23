module;
#include <chrono>
#include <fstream>
#include <filesystem>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <sstream>
#include <thread>
#include <mutex>

export module app;
import anim;
import console;
import bitmap_text;
import health;
import input;
import level_config;
import line;
import music;
import sprite;
import state;
import state.edit;
import state.game;
import state.overlay;
import transform;
import types;
import window;


std::mutex transition_mutex;

export class App {
    std::unique_ptr<Window>   m_window;
    state::Type               m_state_type;

    std::list<state::Object*> m_state_objects;

    std::filesystem::path     m_level_path,
                              m_level_path_to_save;

    U8 m_time_left_saving = 0,
       m_time_to_save = 50;
        
public:
    App() = delete;
    App(cU16 w, cU16 h, cU8 scale, const char* title) {
        m_window = std::make_unique<Window>(w, h, scale, title);
    }
    ~App() {
        for (auto& i : m_state_objects) {
            if (i) delete i;
        }
        m_state_objects.clear();
    }

    void set_level_path_to_save(const std::filesystem::path& p) {
        m_level_path_to_save = p;
    }

    const std::string read_save(cU8 number) {
        std::ostringstream ss;
        ss << (int)number;
        std::ifstream in_file("res/save/" + ss.str() + ".bin", std::ios::in | std::ios::binary);
        char saved_path[32];
        in_file.read((I8*)&saved_path, sizeof(char) * 32);
        return std::string(saved_path);
    }
    void write_save(const std::filesystem::path& level_path_to_save) {
        if (level_path_to_save.empty()) {
            console::log("level_path_to_save empty\n");
            return;
        }
        std::ofstream out_file("res/save/0.bin", std::ios::out | std::ios::binary);
        //char save_path[32] = m_level_path_to_save;
        out_file.write((I8*)level_path_to_save.string().c_str(), sizeof(char) * 32);
        console::log("write_save ", level_path_to_save.string(), "\n");
    }

    void add_state(state::cType state) {
        if (state == state::Type::EDIT) {
            m_state_objects.emplace_back(new state::Edit(m_window->w(), m_window->h(), m_level_path));
        } else if (state == state::Type::GAME) {
            start::Info start_info;

            if (m_level_path.empty()) {
                m_level_path = read_save(0);
                console::log("App::set_state() read_save(): ", m_level_path, "\n");
                if (m_level_path.empty()) {
                    console::log("App::set_state() level path empty!\n");
                    m_level_path = "res/level/test.bin";
                }
                LevelConfig::load(m_level_path);
                LevelConfig::stop_music();
            }
            m_state_objects.emplace_back(new state::Game(m_window->w(), m_window->h(), m_level_path, start_info));
        } else if (state == state::Type::OVERLAY) {            
            m_state_objects.emplace_back(new state::Overlay(m_window->w(), m_window->h()));
        }
    }

    bool transition(state::Object* state_object, state::cType to) {
        if (!state_object) return false;
        for (auto it = m_state_objects.begin(); it != m_state_objects.end(); ++it) {            
            if ((*it) && state_object == (*it)) {
                console::log("App::transition from: ", state::to_string(state_object->current()), " to: ", state::to_string(to), "\n");

                delete (*it);
                if (to == state::Type::GAME) {
                    m_level_path = LevelConfig::level_path(state_object->start_info().type);

                    if (m_level_path.empty()) {
                        m_level_path = read_save(0);
                        console::log("App::set_state() read_save(): ", m_level_path, "\n");
                        if (m_level_path.empty()) {
                            console::log("App::set_state() level path empty!\n");
                            m_level_path = "res/level/test.bin";
                        }
                    }
                    LevelConfig::load(m_level_path);
                    LevelConfig::stop_music();
                    
                    (*it) = new state::Game(m_window->w(), m_window->h(), m_level_path, state_object->start_info());
                } else if (to == state::Type::EDIT) {
                    (*it) = new state::Edit(m_window->w(), m_window->h(), m_level_path);
                }
                console::log("num states: ", m_state_objects.size(), "\n");
                return true;
            }
        }

        return false;
    }
    void run() {
        /*if (!m_state) {
            m_window.close();
        }*/

        //U32 highest_fps = 0;

        //U64 seconds_timer = 0;
        cI64 frames_until_update = 1000000000 / 60;
        //cF32 frames_until_update = 1.0F / 60.0F;
        I64  frames = frames_until_update;

        auto last_time = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration   delta_time;


        while (m_window->is_open()) {
            //seconds_timer += std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count();

            //if (seconds_timer >= 1000000) {
                //console::log("hello ", std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count(), "\n");
                //seconds_timer = 0;
            //}

            std::set<U8> layers_to_draw;

            for (auto& state : m_state_objects) {
                if (!state) continue;
                state->update_unlocked();

                layers_to_draw.insert(state->get_visible_layers().begin(), state->get_visible_layers().end());
            }

            if (frames >= frames_until_update) {
                frames = 0;
                cF32 time_step = std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time).count() / 1000000000.0F;

                input::handle_events(m_window);
                
                std::vector<std::thread> threads;

                for (auto& state : m_state_objects) {
                    if (!state) continue;

                    threads.emplace_back(std::thread([&]() {
                        state->update(time_step);
                        
                        if (state->is_to_change_view) {
                            state->is_to_change_view = false;
                            m_window->view(state->view());
                        } else {
                            state->view(m_window->view());
                        }
                        if (state->is_to_write_save) {
                            state->is_to_write_save = false;
                            if (m_time_left_saving == 0) {
                                m_time_left_saving = m_time_to_save;
                                write_save(state->level_path_to_save());
                            }
                        }

                        if (state->is_to_transition) {
                            state->is_to_transition = false;

                            std::unique_lock<std::mutex> transition_lock(transition_mutex);
                            transition(state, state->next());
                        }
                    })
                    );
                }
                for (auto& i : threads) {
                    i.join();
                }
                threads.clear();
                threads.emplace_back(std::thread(&transform::update));
                threads.emplace_back(std::thread(&health::update));
                threads.emplace_back(std::thread(&anim::update));
                threads.emplace_back(std::thread(&sprite::update));
                threads.emplace_back(std::thread(&line::update));
                for (auto& i : threads) {
                    i.join();
                }

                if (m_time_left_saving > 0) {
                    --m_time_left_saving;
                }                
            }

            m_window->clear();
            for (U8 layer = 0; layer < 3; ++layer) {
                const auto bg_sprite_ids_in_layer = sprite::bg_ids_in_layer(layer);
                for (const auto& i : bg_sprite_ids_in_layer) {                    
                    sprite::draw(m_window, i);                    
                }
            }
            for (auto& layer : layers_to_draw) {
                for (auto& state : m_state_objects) {
                    if (!state) continue;                    
                    state->draw(m_window, layer);                    
                }
            }
            m_window->display();


            auto current_time = std::chrono::steady_clock::now();
            delta_time = current_time - last_time;

            frames += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time).count();
            last_time = current_time;
        }
    }
};