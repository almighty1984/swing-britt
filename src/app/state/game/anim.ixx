module;
#include <vector>
#include <sstream>
#include <string>
#include <map>

export module anim;
import console;
import types;

struct Anim {
private:
    F32 m_timer;
public:
    I32 id;
    anim::Type type;
    F32 speed;
    U16 loop, loops;
    U16 first_frame, last_frame;
    RectI source;
    Vec2U texture_size;
    bool is_reverse;

    Anim() : m_timer(0.0F),
        id(-1),
        type(anim::Type::NONE),
        speed(1.0F),
        loop(0), loops(0),
        first_frame(0),
        last_frame(0),
        source(0, 0, 16, 16),
        texture_size(16, 16),
        is_reverse(false) {
    }

    cU16 num_frames()    const { return texture_size.x / source.w; }
    cU16 current_frame() const { return source.x / source.w; }

    bool is_last_frame() const { return (current_frame() == last_frame); }

    void update() {
        m_timer += speed;
        if (m_timer < 1.0F) {
            return;
        }
        m_timer = 0.0F;

        if (is_reverse) {
            source.x -= source.w;

            //console::log("Anim::update() source.x: ", source.x, "\n");
            if (source.x < 0) {
                if (loops == 0) {
                    source.x = first_frame * source.w;
                } else {
                    loop += 1;
                    if (loop >= loops) {
                        source.x = 0;
                        loop = loops;
                    } else {
                        source.x = first_frame * source.w;
                    }
                }
            }
            return;
        }

        source.x += source.w;

        if (source.x >= texture_size.x) {
            if (loops == 0) {
                source.x = first_frame * source.w;
            } else {
                loop += 1;
                if (loop >= loops) {
                    source.x = texture_size.x - source.w;
                    loop = loops;
                } else {
                    source.x = 0;
                }
            }
        }
        //console::log("source: ", source.x, "\n");
    }
};

std::vector<Anim*>  s_anims;
std::vector<I32>    s_unused_ids;

#define IS_VALID(i) i >= 0 && i < s_anims.size() && s_anims.at(i)

export namespace anim {
    Anim* get(cI32 i)      { return IS_VALID(i) ? s_anims.at(i) : nullptr; }
    size_t  size()           { return s_anims.size(); }

    Type  type(cI32 i)          { return IS_VALID(i) ? s_anims.at(i)->type             :       Type::NONE; }
    F32   speed(cI32 i)         { return IS_VALID(i) ? s_anims.at(i)->speed            :             0.0F; }
    U16   loop(cI32 i)          { return IS_VALID(i) ? s_anims.at(i)->loop             :                0; }
    U16   loops(cI32 i)         { return IS_VALID(i) ? s_anims.at(i)->loops            :                0; }
    U16   first_frame(cI32 i)   { return IS_VALID(i) ? s_anims.at(i)->first_frame      :                0; }
    U16   last_frame(cI32 i)    { return IS_VALID(i) ? s_anims.at(i)->last_frame       :                0; }
    RectI source(cI32 i)        { return IS_VALID(i) ? s_anims.at(i)->source           : RectI{0,0,16,16}; }
    Vec2U texture_size(cI32 i)  { return IS_VALID(i) ? s_anims.at(i)->texture_size     :          Vec2U{}; }
    U32   texture_w(cI32 i)     { return IS_VALID(i) ? s_anims.at(i)->texture_size.x   :                0; }
    U32   texture_h(cI32 i)     { return IS_VALID(i) ? s_anims.at(i)->texture_size.y   :                0; }
    cU16  num_frames(cI32 i)    { return IS_VALID(i) ? s_anims.at(i)->num_frames()     :                0; }
    cU16  current_frame(cI32 i) { return IS_VALID(i) ? s_anims.at(i)->current_frame()  :                0; }
    bool  is_last_frame(cI32 i) { return (current_frame(i) == last_frame(i));                              }
    bool  is_reverse(cI32 i)     { return IS_VALID(i) ? s_anims.at(i)->is_reverse : false; }

    void type(cI32 i, cType t)          { if (IS_VALID(i)) s_anims.at(i)->type           = t;  }
    void speed(cI32 i, cF32 s)          { if (IS_VALID(i)) s_anims.at(i)->speed          = s;  }
    void loop(cI32 i, cU16 l)           { if (IS_VALID(i)) s_anims.at(i)->loop           = l;  }
    void loops(cI32 i, cU16 l)          { if (IS_VALID(i)) s_anims.at(i)->loops          = l;  }
    void first_frame(cI32 i, cU16 f)    { if (IS_VALID(i)) s_anims.at(i)->first_frame    = f;  }
    void last_frame(cI32 i, cU16 f)     { if (IS_VALID(i)) s_anims.at(i)->last_frame     = f;  }
    void source(cI32 i, cRectI s)       { if (IS_VALID(i)) s_anims.at(i)->source         = s;  }
    void source_x(cI32 i, cI32 x)       { if (IS_VALID(i)) s_anims.at(i)->source.x       = x;  }
    void source_y(cI32 i, cI32 y)       { if (IS_VALID(i)) s_anims.at(i)->source.y       = y;  }
    void source_w(cI32 i, cI32 w)       { if (IS_VALID(i)) s_anims.at(i)->source.w       = w;  }
    void source_h(cI32 i, cI32 h)       { if (IS_VALID(i)) s_anims.at(i)->source.h       = h;  }
    void texture_size(cI32 i, cVec2U s) { if (IS_VALID(i)) s_anims.at(i)->texture_size   = s;  }
    void texture_w(cI32 i, cU32 w)      { if (IS_VALID(i)) s_anims.at(i)->texture_size.x = w;  }
    void texture_h(cI32 i, cU32 h)      { if (IS_VALID(i)) s_anims.at(i)->texture_size.y = h;  }
    void is_reverse(cI32 i, bool is)    { if (IS_VALID(i)) s_anims.at(i)->is_reverse     = is; }

    I32 make() {
        Anim* object = new Anim;
        if (!s_unused_ids.empty()) {
            object->id = s_unused_ids.back();
            s_unused_ids.pop_back();                
            if (!s_anims.empty() && object->id >= 0 && object->id < s_anims.size() && s_anims.at(object->id)) {
                delete s_anims.at(object->id);
                //s_anims.at(object->id) = nullptr;
            }
            s_anims.at(object->id) = object;
        } else {
            object->id = s_anims.size();                
            s_anims.emplace_back(object);
        }
        return object->id;
    }
    bool erase(cI32 i) {
        if (i < 0) return false;
        if (s_anims.empty() || i > s_anims.size() -1 || !s_anims.at(i)) {
            console::log("anim::erase ", i, " can't do it!\n");
            return false;
        }
        delete s_anims.at(i);
        s_anims.at(i) = nullptr;
        s_unused_ids.emplace_back(i);
        return true;
    }
    void update() {
        for (const auto& i : s_anims) {
            if (i) i->update();
        }
    }
    void clear() {
        for (size_t i = 0; i < s_anims.size(); ++i) {
            erase(i);
        }
        s_anims.clear();
        s_unused_ids.clear();
    }
}