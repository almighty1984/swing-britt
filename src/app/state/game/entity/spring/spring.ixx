export module entity.spring;
import anim;
import console;
import entity;
import collider.aabb;
import sound;
import types;

export namespace entity {
    class Spring : public Object {
    public:
        Spring() {
            //load_config("res/entity/spring_U.cfg");
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            cType other_type = other.owner->type();

            if ((other_type == Type::BRICK || other_type == Type::BUG) && !other.owner->parent() ||
                other_type == Type::FROG  ||
                other_type == Type::PLAYER) {
                //if (other.owner->parent()) return;
                
                reset_anim(m_dead_anim_id);
                m_time_left_dead = m_time_to_be_dead;
                
                sound::position(m_dead_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_dead_sound_id);
            }

        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}