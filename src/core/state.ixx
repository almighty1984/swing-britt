module;
#include <fstream>
#include <filesystem>
#include <string>
#include <set>

export module state;
import console;
import input;
import types;
import window;

export namespace state {
    std::filesystem::path tile_set_texture_path(cU8 tile_set_number) { return "res/texture/set_" + std::to_string((int)tile_set_number) + ".png"; }

    class Object {
    protected:
        I32 m_input_id = -1;

        Type m_next    = Type::GAME,
             m_current = Type::NONE;

        std::set<U8> m_visible_layers;

        start::Info m_start_info = { start::Type::CENTER, 0 };

        RectF m_view = { 0.0F, 0.0F, 320.0F, 180.0F };

        start::Type m_next_level = start::Type::CENTER;

    public:
        bool is_drawing_debug = false;
        bool is_to_write_save = false;
        bool is_to_transition = false;
        bool is_to_change_view = false;

        virtual std::filesystem::path level_path_to_save() { return {}; }
        virtual ~Object() {
            m_visible_layers.clear();
        }
        virtual void update(cF32 ts) {}
        virtual void update_unlocked() {}

        std::set<U8>& get_visible_layers() { return m_visible_layers; }
        
        Type next()    const { return m_next;    } void next(cType n)    { m_next = n; }
        Type current() const { return m_current; } void current(cType c) { m_current = m_next = c; }

        cRectF view() const { return m_view; } void view(cRectF v) { m_view = v; }

        start::cType next_level() const { return m_next_level; } void next_level(start::cType n) { m_next_level = n; }
        start::cInfo start_info() const { return m_start_info; } void start_info(start::cInfo i) { m_start_info = i; }

        //entity::Type next_level_start() const { return m_next_level_start; }  void next_level_start(entity::Type t) { m_next_level_start = t; }        


        virtual void draw(std::unique_ptr<Window>& window, cU8 layer) {}
    };
}