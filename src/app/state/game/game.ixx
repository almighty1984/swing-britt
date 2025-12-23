module;
#include <filesystem>
#include <sstream>
#include <vector>
#include <map>
#include <list>

export module state.game;

import state;
import bitmap_text;
import console;
import input;
import line;
import sprite;
import transform;
import types;

import camera;
import collider.aabb;
import entity;
import entity.particle;
import entity.player;
import plane;
import quad_tree;

import particle;

export namespace state {
    class Game : public Object {
    private:        
        U16 m_window_w = 0,
            m_window_h = 0;

        I32 m_level_transform_id = -1;

        U32 m_timer = 0;
        entity::Player m_player;
        //entity::Player m_player2;
        //entity::Player m_player3;

        Vec2F m_start_position = { 0.0F, 0.0F };
        //entity::Type m_level_start = entity::Type::level_start;

        std::vector<I32> m_level_sprite_ids;

        std::vector<entity::Object*> m_entity_objects,
                                     m_trigger_entity_objects,
                                     m_water_entity_objects,
                                     m_unlocked_entity_objects;

        std::vector<std::pair<Vec2I, QuadTreeNode*>> m_level_quad_trees;

        std::filesystem::path m_level_path = "res/level/test.bin";
        
        Multiplane m_bg_planes;

    public:
        Game() = delete;
        Game(cU16 window_w, cU16 window_h, const std::filesystem::path level_path, start::Info start_info);
        ~Game() {
            clear();
        }        
        void clear() {
            console::log("state::Game::clear()\n");
            camera::remove_transform_id(m_level_transform_id);
            camera::remove_transform_id(m_player.get_transform_id());
            camera::difference = 0.0F;
            camera::clear();

            m_visible_layers.clear();

            for (auto& i : m_level_quad_trees) {
                delete i.second;
            }
            m_level_quad_trees.clear();
            
            for (auto& i : m_entity_objects) {
                delete i;
                i = nullptr;
            }
            m_entity_objects.clear();
            m_water_entity_objects.clear();
            m_unlocked_entity_objects.clear();
            m_trigger_entity_objects.clear();

            input::erase(m_input_id);            

            console::log("sprite::size: ", sprite::size(), " unused size: ", sprite::unused_size(), "\n");
            for (auto& i : m_level_sprite_ids) {
                sprite::erase(i);
            }
            m_level_sprite_ids.clear();

            aabb::clear();            
            particle::clear();

            transform::erase(m_level_transform_id);
        }
        void check_to_add_input_from(entity::Object* trigger_entity);

        virtual void update(cF32 ts) override;
        virtual void update_unlocked() override;

        void draw(std::unique_ptr<Window>& window, cU8 layer) override;

        std::filesystem::path level_path_to_save() override {
            return m_level_path;
        }

        std::map<U16, std::string> load_types_from_bin();

        Vec2F level_velocity() { return transform::velocity(m_level_transform_id); }
        void level_velocity(cVec2F v) {
            transform::velocity(m_level_transform_id, v);
        }
        cF32 level_x() { return transform::position(m_level_transform_id).x; }
        void level_x(cF32 x) { transform::position_x(m_level_transform_id, x); }

        cF32 level_y() { return transform::position(m_level_transform_id).y; }
        void level_y(cF32 y) { transform::position_y(m_level_transform_id, y); }

        Vec2F level_position() { return transform::position(m_level_transform_id); }
        void level_position(cVec2F p) {
            transform::position(m_level_transform_id, p);
        }

        void load_level(const std::filesystem::path& path);

        void quad_trees_check_collision();
    };
}