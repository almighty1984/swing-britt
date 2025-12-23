module;
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <sstream>

export module particle;
import console;
import entity;
import entity.particle;
import entity.particle.brick;
import entity.particle.bubble;
import entity.particle.down_thrust;
import entity.particle.health;
import entity.particle.interact;
import entity.particle.melee;
import entity.particle.sense;
import entity.particle.shot;
import entity.particle.drop;
import sprite;
import transform;
import types;
import window;

struct Spawn {
    entity::Object* parent = nullptr;
    particle::Type  type = particle::Type::NONE;
    Vec2F           position = { 0.0F, 0.0F },
                    velocity = { 0.0F, 0.0F };
};
using cSpawn = const Spawn;

std::vector<Spawn>           s_to_spawn;
std::list<entity::Particle*> s_particle_entities;

export namespace particle {
    void draw(std::unique_ptr<Window>& window, cU8 layer) {
        for (auto& i : s_particle_entities) {
            if (i && i->start_layer() == layer && !i->is_to_erase()) {
                i->draw(window);
            }
        }
    }
    void draw_debug(std::unique_ptr<Window>& window, cU8 layer) {
        for (auto& i : s_particle_entities) {
            if (i && i->start_layer() == layer && !i->is_to_erase()) {
                i->draw_debug(window);                
            }
        }
    }
    void clear() {
        s_to_spawn.clear();        
        for (auto it = s_particle_entities.begin(); it != s_particle_entities.end(); ++it) {
            if (!(*it)) continue;
            delete (*it);
        }
        s_particle_entities.clear();
    }
    void spawn(cSpawn to_spawn) {
        s_to_spawn.emplace_back(to_spawn);
    }        
    void update() {
        for (auto& i : s_particle_entities) {
            if (i) i->update();
        }        
    }
    void check_to_erase() {            
        for (auto it = s_particle_entities.begin(); it != s_particle_entities.end(); ++it) {
            if (!(*it)) continue;
            if ((*it)->is_to_erase()) {
                delete (*it);
                it = s_particle_entities.erase(it);
                if (it == s_particle_entities.end()) {
                    break;
                }
            }
        }
    }
    void check_to_spawn() {
        //console::log("particles: ", s_particle_entities.size(), "\n");

        for (auto& to_spawn : s_to_spawn) {
            Vec2F position = to_spawn.position;
            Vec2F velocity = to_spawn.velocity;

            if (to_spawn.type == Type::BUBBLE) {
                console::log("spawn bubble\n");
                s_particle_entities.emplace_back(new entity::ParticleBubble);
                s_particle_entities.back()->load_config("res/entity/particle/bubble.cfg");
                //m_entity_objects.back()->direction({ (F32)random::number(0, 1), 0.0F });
            } else if (to_spawn.type == Type::HEALTH) {
                s_particle_entities.emplace_back(new entity::ParticleHealth);
                s_particle_entities.back()->load_config("res/entity/particle/health.cfg");
            } else if (to_spawn.type == Type::HIT) {
                s_particle_entities.emplace_back(new entity::Particle);
                s_particle_entities.back()->load_config("res/entity/particle/hit.cfg");
                velocity = {};
            } else if (to_spawn.type == Type::INTERACT) {
                s_particle_entities.emplace_back(new entity::ParticleInteract);
                s_particle_entities.back()->load_config("res/entity/particle/interact_" + entity::to_string(to_spawn.parent->type()) + ".cfg");
            } else if (to_spawn.type == Type::MELEE) {
                s_particle_entities.emplace_back(new entity::ParticleMelee);
                s_particle_entities.back()->load_config("res/entity/particle/melee_" + entity::to_string(to_spawn.parent->type()) + ".cfg");
                //console::log("time left dead: ", s_particle_entities.back()->time_left_dead(), "\n");
                console::log("particle::check_to_spawn Melee velocity: ", velocity.x, " ", velocity.y, "\n");
            } else if (to_spawn.type == Type::SENSE) {
                s_particle_entities.emplace_back(new entity::ParticleSense);
                s_particle_entities.back()->load_config("res/entity/particle/sense_" + entity::to_string(to_spawn.parent->type()) + ".cfg");
                s_particle_entities.back()->start_layer(NUM_VISIBLE_LAYERS - 1);
            } else if (to_spawn.type == Type::SHOT) {
                s_particle_entities.emplace_back(new entity::ParticleShot);
                s_particle_entities.back()->load_config("res/entity/particle/shot_" + entity::to_string(to_spawn.parent->type()) + ".cfg");                                
            } else if (to_spawn.type == Type::BRICK       ||
                       to_spawn.type == Type::DOWN_THRUST ||
                       to_spawn.type == Type::DROP        ||
                       to_spawn.type == Type::DROP_BLOOD  ||
                       to_spawn.type == Type::DUST        ||
                       to_spawn.type == Type::DUST_L      ||
                       to_spawn.type == Type::DUST_R) {
                if (to_spawn.type == Type::BRICK) {
                    s_particle_entities.emplace_back(new entity::ParticleBrick);
                } else if (to_spawn.type == Type::DOWN_THRUST) {
                    s_particle_entities.emplace_back(new entity::ParticleDownThrust);
                } else if (to_spawn.type == Type::DROP_BLOOD || to_spawn.type == Type::DROP) {
                    s_particle_entities.emplace_back(new entity::ParticleDrop);
                } else {
                    s_particle_entities.emplace_back(new entity::Particle);
                }
                s_particle_entities.back()->load_config("res/entity/particle/" + particle::to_string(to_spawn.type) + ".cfg");
            }

            if (s_particle_entities.back()->transform_id() != -1) {
                transform::position(s_particle_entities.back()->transform_id(), position);
                transform::velocity(s_particle_entities.back()->transform_id(), velocity);
            }

            if (to_spawn.parent) {
                s_particle_entities.back()->parent(to_spawn.parent);
                sprite::layer(s_particle_entities.back()->sprite_id(), NUM_VISIBLE_LAYERS - 1);
                s_particle_entities.back()->start_layer(NUM_VISIBLE_LAYERS - 1);                    
            }
            //s_particle_entities.back()->update();
        }
        s_to_spawn.clear();
    }    
}
