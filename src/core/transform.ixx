module;
#include <vector>

export module transform;
import console;
import types;

struct Transform {
    I32   id;
    Vec2F position, start_position,
        velocity, moved_velocity,
        acceleration, deceleration,
        max_velocity, start_max_velocity;

    Transform() : id(-1),
        position({ 0.0F, 0.0F }), start_position({ 0.0F, 0.0F }),
        velocity({ 0.0F, 0.0F }), moved_velocity({ 0.0F, 0.0F }),
        acceleration({ 0.0F, 0.0F }), deceleration({ 0.0F, 0.0F }),
        max_velocity({ 10.0F, 10.0F }), start_max_velocity({ 10.0F, 10.0F }) {
    }
    /*Transform(cVec2F p) : Transform() {
        start_position = p;
    }*/
    void update() {
        //moved_velocity = decelerate(moved_velocity, deceleration);
        if (moved_velocity != Vec2F(0.0F, 0.0F)) {
            moved_velocity = clamp(moved_velocity, max_velocity);
        }
        if (velocity != Vec2F(0.0F, 0.0F)) {
            velocity = decelerate(velocity, deceleration);
            velocity = clamp(velocity, max_velocity);
        }
        position += velocity + moved_velocity;
    }
private:
    Vec2F clamp(Vec2F v, Vec2F limit) {
        if (limit == Vec2F{ 0.0F, 0.0F }) return v;
        if (v.x < -limit.x) {
            v.x = -limit.x;
            //v.x += acceleration.x;                
            /*if (v.x > -limit.x) {
                v.x = -limit.x;
            }*/
        } else if (v.x > limit.x) {
            v.x = limit.x;
            //v.x -= acceleration.x;                
            /*if (v.x < limit.x) {
                v.x = limit.x;
            }*/
        }
        if (v.y < -limit.y) {
            v.y = -limit.y;
            //v.y += acceleration.y;                
            /*if (v.y > -limit.y) {
                v.y = -limit.y;
            }*/
        } else if (v.y > limit.y) {
            v.y = limit.y;
            //v.y -= acceleration.y;                
            /*if (v.y < limit.y) {
                v.y = limit.y;
            }*/
        }
        return v;
    }
    Vec2F decelerate(Vec2F v, Vec2F amount) {
        if (v.x != 0.0F && amount.x != 0.0F) {
            if (v.x > 0.0F && v.x < amount.x || v.x < 0.0F && v.x > -amount.x) {
                v.x = 0.0F;
            } else {
                if (v.x > 0.0F) v.x -= amount.x;
                else if (v.x < 0.0F) v.x += amount.x;
            }
        }
        if (v.y != 0.0F && amount.y != 0.0F) {
            if (v.y > 0.0F && v.y < amount.y || v.y < 0.0F && v.y > -amount.y) {
                v.y = 0.0F;
            } else {
                if (v.y > 0.0F) v.y -= amount.y;
                else if (v.y < 0.0F) v.y += amount.y;
            }
        }
        return v;
    }
};

std::vector<Transform*> s_transforms;
std::vector<size_t>     s_unused_ids;

I32 s_level_transform_id = -1;

#define IS_VALID(i) i >= 0 && i < s_transforms.size() && s_transforms.at(i)

export namespace transform {
    bool    is_valid(size_t i) { return (i >= 0 && i < s_transforms.size() && s_transforms.at(i)) ? true : false; }
    size_t  size()             { return s_transforms.size(); }
    size_t  unused_size()      { return s_unused_ids.size(); }

    I32  level_id()       { return s_level_transform_id; }
    void level_id(cI32 i) { s_level_transform_id = i; }
    bool is_level(cI32 i) { return IS_VALID(i) && i == s_level_transform_id; }

    I32 make() {
        Transform* object = new Transform;
        if (!s_unused_ids.empty()) {
            object->id = s_unused_ids.back();
            s_unused_ids.pop_back();
            //console::log("transform::make unused: ", object->id, "\n");
            if (!s_transforms.empty() && object->id >= 0 && object->id < s_transforms.size() && s_transforms.at(object->id)) {
                delete s_transforms.at(object->id);
                //s_transforms.at(object->id) = nullptr;
            }
            s_transforms.at(object->id) = object;
        } else {
            object->id = s_transforms.size();
            //console::log("transform::make id: ", object->id, "\n");
            s_transforms.emplace_back(object);
        }
        return object->id;
    }

    Vec2F position(cI32 i)           { return IS_VALID(i) ? s_transforms.at(i)->position           : Vec2F{}; }    
    Vec2F start_position(cI32 i)     { return IS_VALID(i) ? s_transforms.at(i)->start_position     : Vec2F{}; }
    Vec2F velocity(cI32 i)           { return IS_VALID(i) ? s_transforms.at(i)->velocity           : Vec2F{}; }
    Vec2F moved_velocity(cI32 i)     { return IS_VALID(i) ? s_transforms.at(i)->moved_velocity     : Vec2F{}; }
    Vec2F acceleration(cI32 i)       { return IS_VALID(i) ? s_transforms.at(i)->acceleration       : Vec2F{}; }
    Vec2F deceleration(cI32 i)       { return IS_VALID(i) ? s_transforms.at(i)->deceleration       : Vec2F{}; }
    Vec2F max_velocity(cI32 i)       { return IS_VALID(i) ? s_transforms.at(i)->max_velocity       : Vec2F{}; }
    Vec2F start_max_velocity(cI32 i) { return IS_VALID(i) ? s_transforms.at(i)->start_max_velocity : Vec2F{}; }

    void position(cI32 i, cVec2F p)           { if (IS_VALID(i)) s_transforms.at(i)->position             = p; }
    void position_x(cI32 i, cF32 x)           { if (IS_VALID(i)) s_transforms.at(i)->position.x           = x; }    
    void position_y(cI32 i, cF32 y)           { if (IS_VALID(i)) s_transforms.at(i)->position.y           = y; }
    void add_position(cI32 i, cVec2F p)       { if (IS_VALID(i)) s_transforms.at(i)->position            += p; }
    void add_position_x(cI32 i, cF32 x)       { if (IS_VALID(i)) s_transforms.at(i)->position.x          += x; }
    void add_position_y(cI32 i, cF32 y)       { if (IS_VALID(i)) s_transforms.at(i)->position.y          += y; }
    void start_position(cI32 i, cVec2F p)     { if (IS_VALID(i)) s_transforms.at(i)->start_position       = p; }
    void velocity(cI32 i, cVec2F v)           { if (IS_VALID(i)) s_transforms.at(i)->velocity             = v; }
    void velocity_x(cI32 i, cF32 x)           { if (IS_VALID(i)) s_transforms.at(i)->velocity.x           = x; }
    void velocity_y(cI32 i, cF32 y)           { if (IS_VALID(i)) s_transforms.at(i)->velocity.y           = y; }
    void add_velocity(cI32 i, cVec2F v)       { if (IS_VALID(i)) s_transforms.at(i)->velocity            += v; }
    void add_velocity_x(cI32 i, cF32 x)       { if (IS_VALID(i)) s_transforms.at(i)->velocity.x          += x; }
    void add_velocity_y(cI32 i, cF32 y)       { if (IS_VALID(i)) s_transforms.at(i)->velocity.y          += y; }
    void moved_velocity(cI32 i, cVec2F v)     { if (IS_VALID(i)) s_transforms.at(i)->moved_velocity       = v; }
    void moved_velocity_x(cI32 i, cF32 x)     { if (IS_VALID(i)) s_transforms.at(i)->moved_velocity.x     = x; }
    void moved_velocity_y(cI32 i, cF32 y)     { if (IS_VALID(i)) s_transforms.at(i)->moved_velocity.y     = y; }
    void add_moved_velocity(cI32 i, cVec2F v) { if (IS_VALID(i)) s_transforms.at(i)->moved_velocity      += v; }
    void add_moved_velocity_x(cI32 i, cF32 x) { if (IS_VALID(i)) s_transforms.at(i)->moved_velocity.x    += x; }
    void add_moved_velocity_y(cI32 i, cF32 y) { if (IS_VALID(i)) s_transforms.at(i)->moved_velocity.y    += y; }
    void acceleration(cI32 i, cVec2F a)       { if (IS_VALID(i)) s_transforms.at(i)->acceleration         = a; }
    void acceleration_x(cI32 i, cF32 x)       { if (IS_VALID(i)) s_transforms.at(i)->acceleration.x       = x; }
    void acceleration_y(cI32 i, cF32 y)       { if (IS_VALID(i)) s_transforms.at(i)->acceleration.y       = y; }
    void deceleration(cI32 i, cVec2F d)       { if (IS_VALID(i)) s_transforms.at(i)->deceleration         = d; }
    void deceleration_x(cI32 i, cF32 x)       { if (IS_VALID(i)) s_transforms.at(i)->deceleration.x       = x; }
    void deceleration_y(cI32 i, cF32 y)       { if (IS_VALID(i)) s_transforms.at(i)->deceleration.y       = y; }
    void max_velocity(cI32 i, cVec2F v)       { if (IS_VALID(i)) s_transforms.at(i)->max_velocity         = v; }
    void max_velocity_x(cI32 i, cF32 x)       { if (IS_VALID(i)) s_transforms.at(i)->max_velocity.x       = x; }
    void max_velocity_y(cI32 i, cF32 y)       { if (IS_VALID(i)) s_transforms.at(i)->max_velocity.y       = y; }
    void start_max_velocity(cI32 i, cVec2F v) { if (IS_VALID(i)) s_transforms.at(i)->start_max_velocity   = v; }
    void start_max_velocity_x(cI32 i, cF32 x) { if (IS_VALID(i)) s_transforms.at(i)->start_max_velocity.x = x; }
    void start_max_velocity_y(cI32 i, cF32 y) { if (IS_VALID(i)) s_transforms.at(i)->start_max_velocity.y = y; }

    bool erase(const size_t i) {
        if (IS_VALID(i)) {
            //console::log("transform::erase ", i, "\n");
            delete s_transforms.at(i);
            s_transforms.at(i) = nullptr;
            s_unused_ids.emplace_back(i);
            return true;
        }
        //console::log("transform::erase ", i, " can't do it! size: ", s_transforms.size(), "\n");
        return false;
    }
    void update() {
        for (auto& i : s_transforms) {
            if (i) i->update();
        }
    }
    void clear() {
        for (size_t i = 0; i < s_transforms.size(); ++i) {            
            if (s_transforms.at(i)) delete s_transforms.at(i);            
        }
        s_transforms.clear();
        s_unused_ids.clear();
    }
}