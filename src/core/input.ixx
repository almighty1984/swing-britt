module;
#include <map>
#include <string>
#include <sstream>
#include <optional>
#include <vector>

export module input;

import console;
import types;
import window;

struct Listener {
    I32 id = -1;
    std::map<input::Key, bool>       key_map,    key_is_locked_map;
    std::map<input::Button, bool>    button_map, button_is_locked_map;

    void press(input::Key k)      { key_map[k] = true;  }
    void release(input::Key k)    { key_map[k] = false; }
    bool is_pressed(input::Key k) { return key_map[k];  }
    void lock(input::Key k)       { key_is_locked_map[k] = true;  }
    void unlock(input::Key k)     { key_is_locked_map[k] = false; }
    bool is_locked(input::Key k)  { return key_is_locked_map[k];  }

    void press(input::Button b)      { button_map[b] = true;  }
    void release(input::Button b)    { button_map[b] = false; }
    bool is_pressed(input::Button b) { return button_map[b];  }
    void lock(input::Button b)       { button_is_locked_map[b] = true;  }
    void unlock(input::Button b)     { button_is_locked_map[b] = false; }
    bool is_locked(input::Button b)  { return button_is_locked_map[b];  }
};

std::vector<Listener*> s_listeners;
std::vector<size_t>    s_unused_ids;

#define IS_VALID(i) i >= 0 && i < s_listeners.size() && s_listeners.at(i)

export namespace input {
    Vec2F mouse{ 0.0F, 0.0F }, mouse_prev{ 0.0F, 0.0F };
    F32   scroll = 0.0F;

    void press(cI32 i, input::Key k)      { if (IS_VALID(i)) s_listeners.at(i)->press(k);                  }
    void release(cI32 i, input::Key k)    { if (IS_VALID(i)) s_listeners.at(i)->release(k);                }
    bool is_pressed(cI32 i, input::Key k) { return IS_VALID(i) ? s_listeners.at(i)->is_pressed(k) : false; }
    void lock(cI32 i, input::Key k)       { if (IS_VALID(i)) s_listeners.at(i)->lock(k);                   }
    void unlock(cI32 i, input::Key k)     { if (IS_VALID(i)) s_listeners.at(i)->unlock(k);                 }
    bool is_locked(cI32 i, input::Key k)  { return IS_VALID(i) ? s_listeners.at(i)->is_locked(k)  : false; }

    void press(cI32 i, input::Button b)      { s_listeners.at(i)->press(b);                                  }
    void release(cI32 i, input::Button b)    { s_listeners.at(i)->release(b);                                }
    bool is_pressed(cI32 i, input::Button b) { return s_listeners.at(i)->is_pressed(b);                      }
    void lock(cI32 i, input::Button b)       { if (IS_VALID(i)) s_listeners.at(i)->lock(b);                  }
    void unlock(cI32 i, input::Button b)     { if (IS_VALID(i)) s_listeners.at(i)->unlock(b);                }
    bool is_locked(cI32 i, input::Button b)  { return IS_VALID(i) ? s_listeners.at(i)->is_locked(b) : false; }

    void handle_events(std::unique_ptr<Window>& window) {
        while (const std::optional sf_event = window->poll_event()) {
            scroll = 0.0F;
            if (sf_event->is<sf::Event::Closed>()) {
                window->close();
            } else if (const auto* sf_key = sf_event->getIf<sf::Event::KeyPressed>()) {
                input::Key key_num = (input::Key)sf_key->scancode;
                //console::log("key pressed ", (I32)key_num, "\n");
                for (auto& i : s_listeners) {
                    if (i) {
                        i->press(key_num);
                    }
                }
            } else if (const auto* sf_key = sf_event->getIf<sf::Event::KeyReleased>()) {
                input::Key key_num = (input::Key)sf_key->scancode;
                //console::log("key released ", (I32)key_num, "\n");
                for (auto& i : s_listeners) {
                    if (i) {
                        i->release(key_num);
                    }
                }
            } else if (const auto* sf_mouse_moved = sf_event->getIf<sf::Event::MouseMoved>()) {
                cVec2F view_scale = { 320.0F / window->view().w, 180.0F / window->view().h };                    
                mouse = { (F32)(sf_mouse_moved->position.x / view_scale.x / window->scale()), (F32)(sf_mouse_moved->position.y / view_scale.y / window->scale())};

            } else if (const auto* sf_mouse_pressed = sf_event->getIf<sf::Event::MouseButtonPressed>()) {
                Button button = (Button)sf_mouse_pressed->button;
                //console::log("mouse pressed ", (I8)b, "\n");                
                for (auto& i : s_listeners) {
                    if (i) {
                        i->press(button);
                    }
                }
                if (button == Button::LEFT) {
                    mouse_prev = { (F32)(sf_mouse_pressed->position.x / window->scale()), (F32)(sf_mouse_pressed->position.y / window->scale()) };
                } else if (button == Button::RIGHT) {
                    //m_mouse_x = mouse_pressed->position.x;
                        //m_mouse_y = mouse_pressed->position.y;
                }
            } else if (const auto* sf_mouse_released = sf_event->getIf<sf::Event::MouseButtonReleased>()) {
                Button b = (Button)sf_mouse_released->button;
                //console::log("mouse released ", button_num, "\n");
                for (auto& i : s_listeners) {
                    if (i) {
                        i->release(b);
                    }
                }
            } else if (const auto* sf_mouse_scrolled = sf_event->getIf<sf::Event::MouseWheelScrolled>()) {
                scroll = sf_mouse_scrolled->delta;
                //console::log("input::handle_events mouse delta: ", mouse_scrolled->delta, "\n");
            } else if (sf_event->is<sf::Event::JoystickButtonPressed>()) {
                //console::log("ListenerHandler JoystickButtonPressed ", sfEvent.joystickButton.button, "\n");
                //state->joystickButtonDown(sfEvent.joystickButton.button);
            }
        }
    }
    cI32 make() {
        Listener* object = new Listener;
        if (!s_unused_ids.empty()) {
            object->id = s_unused_ids.back();
            s_unused_ids.pop_back();
            if (object->id >= 0 && object->id < s_listeners.size() && s_listeners.at(object->id)) {
                delete s_listeners.at(object->id);
            }
        } else {
            object->id = s_listeners.size();
            s_listeners.emplace_back(nullptr);
        }            
        s_listeners.at(object->id) = object;
        return object->id;
    }
    bool erase(cI32 i) {
        if (IS_VALID(i)) {
            delete s_listeners.at(i);
            s_listeners.at(i) = nullptr;
            s_unused_ids.emplace_back(i);
            return true;
        }        
        console::log("input::erase ", i, " can't do it\n");
        return false;
    }
    void clear() {
        for (size_t i = 0; i < s_listeners.size(); ++i) {
            erase(i);
        }
        s_listeners.clear();
        s_unused_ids.clear();
    }
}