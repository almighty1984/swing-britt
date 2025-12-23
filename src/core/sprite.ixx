module;
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <vector>
#include <set>

export module sprite;
import config;
import console;
import texture;
import transform;
import types;
import window;

struct Sprite {
    I32   id, transform_id;
    U8    layer, tile_set;
    bool  is_bg, is_debug, is_hidden, is_leftward, is_upended;
    Vec2F offset, prev_position, start, level, origin, center;
    Color color, start_color;
    F32   rotation, start_rotation;
    RectI source_rect;
    std::filesystem::path texture_path;
    sf::Sprite sf_sprite;
    sf::Texture* sf_texture;

    Sprite() = delete;
    Sprite(std::filesystem::path path) : //m_transformed_position(),
        id(-1), transform_id(-1),
        layer(0), tile_set(0),
        is_bg(false), is_debug(false), is_hidden(false), is_leftward(false), is_upended(false),
        offset(), prev_position(-1.0F, -1.0F),
        start(), level(), origin(), center(),
        color({ 127, 127, 127 }), start_color({ 127, 127, 127 }),
        rotation(0.0F), start_rotation(0.0F),
        texture_path(path),
        sf_sprite(*texture::load(path)),
        sf_texture(texture::load(path)),
        source_rect{ 0, 0, 16, 16 } {
    }
    ~Sprite() {
        //console::log("~Sprite layer: ", layer, " id: ", id, "\n");
        id = -1;
        transform_id = -1;
    }
    Sprite& operator=(const Sprite& other) {
        transform_id = other.transform_id;
        layer = other.layer;
        tile_set = other.tile_set;
        is_debug = other.is_debug;
        is_hidden = other.is_hidden;
        is_leftward = other.is_leftward;
        is_upended = other.is_upended;
        offset = other.offset;
        start = other.start;
        level = other.level;
        origin = other.origin;
        center = other.center;
        color = other.color;
        start_color = other.start_color;
        rotation = other.rotation;
        start_rotation = other.start_rotation;
        texture_path = other.texture_path;
        source_rect = other.source_rect;
        sf_sprite = other.sf_sprite;
        sf_texture = other.sf_texture;
        return *this;
    }
    cVec2U texture_size() {
        return { sf_sprite.getTexture().getSize().x, sf_sprite.getTexture().getSize().y };
    }
    void set_origin(cVec2F o) {
        origin = o;
        sf_sprite.setOrigin(sf::Vector2f(o.x, o.y));
    }
    void set_rotation(F32 degrees) {
        if (degrees > 360.0F) degrees -= 360.0F;
        rotation = degrees + start_rotation;
        if (rotation > 360.0F) rotation -= 360.0F;

        sf_sprite.setRotation(sf::degrees(rotation));
    }
    cF32 radians() const {
        return sf::degrees(rotation).asRadians();
    }
    void rotate(cF32 degrees) {
        rotation += degrees;
        if (rotation > 360.0F) rotation -= 360.0F;
        else if (rotation < -360.0F) rotation += 360.0F;
        sf_sprite.setRotation(sf_sprite.getRotation() + sf::degrees(degrees));
    }
    bool texture(const std::filesystem::path path) {
        if (texture_path == path) return false;
        texture_path = path;
        sf_texture = texture::load(path);
        if (!sf_texture) return false;
        sf_sprite.setTexture(*sf_texture, true);
        sf_sprite.setTextureRect(sf::IntRect({ (I32)source_rect.x, (I32)source_rect.y }, { (I32)source_rect.w, (I32)source_rect.h }));
        return true;
    }
    void texture_rect(cRectI rect) {
        source_rect = rect;
        sf_sprite.setTextureRect(sf::IntRect({ (I32)rect.x, (I32)rect.y }, { (I32)rect.w, (I32)rect.h }));
    }
    void update() {

        texture_rect(source_rect);
        //sf_sprite.setOrigin({ origin.x * Config::scale(), origin.y * Config::scale() });
        sf_sprite.setOrigin({ origin.x, origin.y });


        //cF32 flip_x = is_leftward ? source_rect.w * 1.0F : 0.0F;
        //cF32 flip_y = is_upended  ? source_rect.h * 1.0F : 0.0F;


        sf_sprite.setScale({ (is_leftward ? -1.0F : 1.0F) * Config::scale(), (is_upended ? -1.0F : 1.0F) * Config::scale() });


        //if (position != prev_position) {
            //m_transformed_position = position + position();
            //prev_position = position;
            //center = { position.x + source_rect.w / 2.0F, position.y + source_rect.h / 2.0F };
            //console::log("sprite::Sprite::update position: ", prev_position.x, " ", position().x, "  ", prev_position.y, " ", position().y, "\n");        
        sf_sprite.setPosition({ (transform::position(transform_id).x + offset.x + origin.x) * Config::scale(),
                                (transform::position(transform_id).y + offset.y + origin.y) * Config::scale() });
        //}
    }
};


std::vector<Sprite*> s_sprites;
std::vector<I32>     s_unused_ids;

#define IS_VALID(i) i >= 0 && i < s_sprites.size() && s_sprites.at(i)

export namespace sprite {
    struct Data {
        U8 tile_set, layer;
        U16 source_y, source_x, y, x;
        Data() : tile_set(0), layer(0), source_y(0), source_x(0), y(0), x(0) {}
    };

    std::vector<I32> ids_in_layer(cU8 layer) {
        std::vector<I32> same_layer_object_ids;
        std::for_each(s_sprites.cbegin(), s_sprites.cend(), [&](Sprite* i) {
                if (i && i->id != -1 && !i->is_bg && i->layer == layer && !i->is_hidden) {
                    same_layer_object_ids.emplace_back(i->id);
                }
            }
        );
        return same_layer_object_ids;
    }

    std::vector<I32> bg_ids_in_layer(cU8 layer) {
        std::vector<I32> same_layer_bg_ids;
        std::for_each(s_sprites.cbegin(), s_sprites.cend(), [&](Sprite* i) {
            if (i && i->id != -1 && i->is_bg && i->layer == layer && !i->is_hidden) {
                same_layer_bg_ids.emplace_back(i->id);
            }
            }
        );
        return same_layer_bg_ids;
    }

    size_t  size()        { return s_sprites.size();    }
    size_t  unused_size() { return s_unused_ids.size(); }

    Sprite* get(cI32 i) { return IS_VALID(i) ? s_sprites.at(i) : nullptr; }
        
    bool  is_bg(cI32 i)          { return IS_VALID(i) ? s_sprites.at(i)->is_bg          :  false;  }
    bool  is_debug(cI32 i)       { return IS_VALID(i) ? s_sprites.at(i)->is_debug       :  false;  }
    bool  is_hidden(cI32 i)      { return IS_VALID(i) ? s_sprites.at(i)->is_hidden      :  false;  }
    bool  is_leftward(cI32 i)    { return IS_VALID(i) ? s_sprites.at(i)->is_leftward    :  false;  }
    bool  is_upended(cI32 i)     { return IS_VALID(i) ? s_sprites.at(i)->is_upended     :  false;  }
    I32   transform_id(cI32 i)   { return IS_VALID(i) ? s_sprites.at(i)->transform_id   :     -1;  }
    U8    layer(cI32 i)          { return IS_VALID(i) ? s_sprites.at(i)->layer          :      0;  }
    U8    tile_set(cI32 i)       { return IS_VALID(i) ? s_sprites.at(i)->tile_set       :      0;  } 
    Vec2F offset(cI32 i)         { return IS_VALID(i) ? s_sprites.at(i)->offset         : Vec2F{}; }
    Vec2F prev_position(cI32 i)  { return IS_VALID(i) ? s_sprites.at(i)->prev_position  : Vec2F{}; }
    Vec2F start(cI32 i)          { return IS_VALID(i) ? s_sprites.at(i)->start          : Vec2F{}; }
    Vec2F level(cI32 i)          { return IS_VALID(i) ? s_sprites.at(i)->level          : Vec2F{}; }
    Vec2F origin(cI32 i)         { return IS_VALID(i) ? s_sprites.at(i)->origin         : Vec2F{}; }
    Vec2F center(cI32 i)         { return IS_VALID(i) ? s_sprites.at(i)->center         : Vec2F{}; }
    F32   rotation(cI32 i)       { return IS_VALID(i) ? s_sprites.at(i)->rotation       :    0.0F; }
    F32   start_rotation(cI32 i) { return IS_VALID(i) ? s_sprites.at(i)->start_rotation :    0.0F; }
    F32   radians(cI32 i)        { return IS_VALID(i) ? s_sprites.at(i)->radians()      :    0.0F; }
    Color color(cI32 i)          { return IS_VALID(i) ? s_sprites.at(i)->color          : Color{}; }
    Color start_color(cI32 i)    { return IS_VALID(i) ? s_sprites.at(i)->start_color    : Color{}; }
    RectI source_rect(cI32 i)    { return IS_VALID(i) ? s_sprites.at(i)->source_rect    : RectI{}; }
    Vec2U texture_size(cI32 i)   { return IS_VALID(i) ? s_sprites.at(i)->texture_size() : Vec2U{}; }

    void is_bg(cI32 i, bool q)           { if (IS_VALID(i)) s_sprites.at(i)->is_bg          = q;  }
    void is_debug(cI32 i, bool q)        { if (IS_VALID(i)) s_sprites.at(i)->is_debug       = q;  }
    void is_hidden(cI32 i, bool q)       { if (IS_VALID(i)) s_sprites.at(i)->is_hidden      = q;  }
    void is_leftward(cI32 i, bool q)     { if (IS_VALID(i)) s_sprites.at(i)->is_leftward    = q;  }
    void is_upended(cI32 i, bool q)      { if (IS_VALID(i)) s_sprites.at(i)->is_upended     = q;  }
    void transform_id(cI32 i, cI32 t)    { if (IS_VALID(i)) s_sprites.at(i)->transform_id   = t;  }
    void layer(cI32 i, cU8 l)            { if (IS_VALID(i)) s_sprites.at(i)->layer          = l;  }
    void tile_set(cI32 i, cU8 t)         { if (IS_VALID(i)) s_sprites.at(i)->tile_set       = t;  }    
    void offset(cI32 i, cVec2F o)        { if (IS_VALID(i)) s_sprites.at(i)->offset         = o;  }
    void offset_x(cI32 i, cF32 x)        { if (IS_VALID(i)) s_sprites.at(i)->offset.x       = x;  }
    void offset_y(cI32 i, cF32 y)        { if (IS_VALID(i)) s_sprites.at(i)->offset.y       = y;  }
    void add_offset(cI32 i, cVec2F o)    { if (IS_VALID(i)) s_sprites.at(i)->offset        += o;  }
    void origin(cI32 i, cVec2F o)        { if (IS_VALID(i)) s_sprites.at(i)->origin         = o;  }
    void prev_position(cI32 i, cVec2F p) { if (IS_VALID(i)) s_sprites.at(i)->prev_position  = p;  }
    void start(cI32 i, cVec2F s)         { if (IS_VALID(i)) s_sprites.at(i)->start          = s;  }
    void level(cI32 i, cVec2F l)         { if (IS_VALID(i)) s_sprites.at(i)->level          = l;  }
    void rotate(cI32 i, cF32 r)          { if (IS_VALID(i)) s_sprites.at(i)->rotate(r);           }
    void rotation(cI32 i, cF32 r)        { if (IS_VALID(i)) s_sprites.at(i)->set_rotation(r);     }
    void start_rotation(cI32 i, cF32 r)  { if (IS_VALID(i)) s_sprites.at(i)->start_rotation = r;  }
    void color(cI32 i, Color c)          { if (IS_VALID(i)) s_sprites.at(i)->color          = c;  }
    void start_color(cI32 i, Color c)    { if (IS_VALID(i)) s_sprites.at(i)->start_color    = c;  }
    void source_rect(cI32 i, RectI r)    { if (IS_VALID(i)) s_sprites.at(i)->source_rect    = r;  }
    void source_rect_x(cI32 i, cI32 x)   { if (IS_VALID(i)) s_sprites.at(i)->source_rect.x  = x;  }
    void source_rect_y(cI32 i, cI32 y)   { if (IS_VALID(i)) s_sprites.at(i)->source_rect.y  = y;  }
    void source_rect_w(cI32 i, cI32 w)   { if (IS_VALID(i)) s_sprites.at(i)->source_rect.w  = w;  }
    void source_rect_h(cI32 i, cI32 h)   { if (IS_VALID(i)) s_sprites.at(i)->source_rect.h  = h;  }
    void texture(cI32 i, const std::filesystem::path path) {
        if (IS_VALID(i)) s_sprites.at(i)->texture(path);
    }
    cI32 make(const std::filesystem::path& path) {
        Sprite* object = new Sprite(path);
        if (!s_unused_ids.empty()) {
            object->id = s_unused_ids.back();
            s_unused_ids.pop_back();
            //console::log("sprite::make unused: ", object->id, "\n");
            if (!s_sprites.empty() && object->id >= 0 && object->id < s_sprites.size() && s_sprites.at(object->id)) {
                delete s_sprites.at(object->id);
                //s_sprites.at(object->id) = nullptr;
            }
            s_sprites.at(object->id) = object;
        } else {
            object->id = s_sprites.size();
            //console::log("sprite::make id: ", object->id, "\n");
            s_sprites.emplace_back(object);
        }
        return object->id;
    }
    bool erase(cI32 i) {
        if (IS_VALID(i)) {
            //console::log("sprite::erase ", i, "\n");
            delete s_sprites.at(i);
            s_sprites.at(i) = nullptr;
            s_unused_ids.emplace_back(i);
            return true;
        }
        //console::log("sprite::erase ", i, " can't do it! size: ", s_sprites.size(), "\n");
        return false;
    }
    void clear() {
        for (I32 i = 0; i < s_sprites.size(); ++i) {
            erase(i);
        }
        s_sprites.clear();
        s_unused_ids.clear();
    }
    void update() {
        for (auto& i : s_sprites) {
            if (i) i->update();
        }
    }
    void draw(std::unique_ptr<Window>& window, cI32 i) {
        if (!window || !(IS_VALID(i)) || s_sprites.at(i)->is_hidden) return;

        if (transform::position(s_sprites.at(i)->transform_id).x + s_sprites.at(i)->offset.x < window->view().x - s_sprites.at(i)->source_rect.w ||
            transform::position(s_sprites.at(i)->transform_id).x + s_sprites.at(i)->offset.x > window->view().w ||
            transform::position(s_sprites.at(i)->transform_id).y + s_sprites.at(i)->offset.y < window->view().y - s_sprites.at(i)->source_rect.h ||
            transform::position(s_sprites.at(i)->transform_id).y + s_sprites.at(i)->offset.y > window->view().h) {
            return;
        }
        window->draw_sf_sprite(s_sprites.at(i)->sf_sprite);
    }    
    void save_level(const std::filesystem::path& path, std::vector<I32>& grid_sprite_ids) {
        console::log("sprite::save...");
        std::vector<Sprite*> to_save;
        for (auto& i : grid_sprite_ids) {
            cU16   rows    = s_sprites.at(i)->source_rect.h / 16;
            cU16   columns = s_sprites.at(i)->source_rect.w / 16;
            cVec2F offset  = s_sprites.at(i)->offset;

            for (U8 layer = 0; layer < NUM_LEVEL_LAYERS; ++layer) {
                for (U16 y = 0; y < rows; ++y) {
                    for (U16 x = 0; x < columns; ++x) {                                                        
                        /*std::for_each(m_objects.cbegin(), m_objects.cend(),
                        [&](Sprite* i) { if (i && i->offset == offset + Vec2F{ x * 16.0F, y * 16.0F }) sprites_to_save.emplace_back(i); }
                        );*/
                        for (auto& i : s_sprites) {
                            if (!i) continue;
                            if (i->layer == layer && i->offset == offset + Vec2F{ x * 16.0F, y * 16.0F }) {
                                to_save.emplace_back(i);
                            }
                        }
                    }
                }
            }
        }
        console::log("size: ", to_save.size(), "...");

        cU16 size = (U16)to_save.size();

        std::ofstream out_file(path, std::ios::out | std::ios::binary);
        out_file.write((I8*)&size, sizeof(U16));

        for (auto& i : to_save) {
            if (!i) continue;
            //console::log((int)i->layer, " ", i->offset.y, " ", i->offset.x, " ");

            cU8  tile_set = i->tile_set;
            cU8  layer = i->layer;
            cU16 source_y = (U16)i->source_rect.y,
                    source_x = (U16)i->source_rect.x;
            cU16 y = (U16)i->offset.y,
                    x = (U16)i->offset.x;

            out_file.write((I8*)&tile_set, sizeof(U8));
            out_file.write((I8*)&layer, sizeof(U8));
            out_file.write((I8*)&source_y, sizeof(U16));
            out_file.write((I8*)&source_x, sizeof(U16));
            out_file.write((I8*)&y, sizeof(U16));
            out_file.write((I8*)&x, sizeof(U16));
        }
        console::log("done\n");
    }
    std::vector<Data> load_level(const std::filesystem::path& path) {
        std::ifstream in_file(path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) return {};

        U16 size = 0;
        in_file.read((I8*)&size, sizeof(U16));

        console::log("sprite::open size: ", size, "\n");

        std::vector<Data> sprite_data_vec;
        for (U16 i = 0; i < size; ++i) {
            Data sprite_data{};
            in_file.read((I8*)&sprite_data.tile_set, sizeof(U8));
            in_file.read((I8*)&sprite_data.layer, sizeof(U8));
            in_file.read((I8*)&sprite_data.source_y, sizeof(U16));
            in_file.read((I8*)&sprite_data.source_x, sizeof(U16));
            in_file.read((I8*)&sprite_data.y, sizeof(U16));
            in_file.read((I8*)&sprite_data.x, sizeof(U16));
            sprite_data_vec.emplace_back(sprite_data);
        }

        in_file.close();

        return sprite_data_vec;
    }    
}