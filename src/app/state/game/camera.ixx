module;
#include <set>
export module camera;
import console;
import random;
import transform;
import types;

std::set<I32> s_transform_ids;

Vec2F shake_min, shake_max;
F32 shake_dec = 0.0F;
F32 sum_shake_dec = 0.0F;

U16 time_left_shaking = 0;
U16 time_to_shake = 0;

export namespace camera {
    I32   focus_transform = -1;
    Vec2F focal_point = { WINDOW_W / 2.0F, WINDOW_H / 2.0F };
    Vec2F speed = { 0.0625f, 0.0625f };
    Vec2F position = { 0.0F, 0.0F };
    Vec2F difference = { 0.0F, 0.0F };

    Vec2I scroll_limit = { INT_MAX, INT_MAX };

    Vec2F focus_offset = { 0.0F, 0.0F };

    void add_transform_id(cI32 id) {
        //console::log("camera::add_transform_id: ", id, "\n");
        s_transform_ids.insert(id);
    }
    void remove_transform_id(cI32 id) {
        s_transform_ids.erase(id);
    }
    void clear() {
        focus_transform = -1;
        s_transform_ids.clear();
        position = { 0.0F, 0.0F };
        difference = { 0.0F, 0.0F };
        scroll_limit = { INT_MAX, INT_MAX };
    }
    void set_position(cVec2F p) {
        position = p;
        if (position.x < 0.0F) {
            position.x = 0.0F;
        } else if (position.x > scroll_limit.x) {
            position.x = scroll_limit.x;
        }
        if (position.y < 0.0F) {
            position.y = 0.0F;
        } else if (position.y > scroll_limit.y) {
            position.y = scroll_limit.y;
        }

        for (auto& i : s_transform_ids) {
            transform::position(i, transform::position(i) - position);
            transform::start_position(i, transform::start_position(i) - position);
        }
    }
    void init_shake(cU16 time, cVec2F min, cVec2F max, F32 dec) {
        time_left_shaking = time;
        time_to_shake = time;
        shake_min = min;
        shake_max = max;
        shake_dec = dec;
        sum_shake_dec = 0.0F;
    }
    void shake() {
        //console::log("camera::shake()\n");
        //if (shake_dec >= 1.0F) return;

        if (time_left_shaking == time_to_shake) {
            focus_offset.x = random::number(0, 1) ? random::number((U16)shake_min.x, (U16)shake_max.x) : -random::number(shake_min.x, shake_max.x);
            focus_offset.y = random::number((U16)shake_min.y, (U16)shake_max.y);
        } else {
            focus_offset.x = random::number(0, 1) ? random::number((U16)shake_min.x, (U16)shake_max.x) : -random::number((U16)shake_min.x, (U16)shake_max.x);
            focus_offset.y = random::number(0, 1) ? random::number((U16)shake_min.y, (U16)shake_max.y) : -random::number((U16)shake_min.y, (U16)shake_max.y);
        }

        shake_min.x *= (1.0F - sum_shake_dec);
        shake_max.x *= (1.0F - sum_shake_dec);
        shake_min.y *= (1.0F - sum_shake_dec);
        shake_max.y *= (1.0F - sum_shake_dec);

        console::log("sum_shake_dec: ", sum_shake_dec, "\n");
        sum_shake_dec += shake_dec;
        if (sum_shake_dec > 1.0F) {
            sum_shake_dec = 1.0F;
        }


        //focus_offset.x = random::number(0, 1) ? -16.0F : 16.0F;
        //focus_offset.y = random::number(0, 1) ? -16.0F : 16.0F;
        //focus_offset.x = random::number(0, 1) ? focus_offset.x - 16.0F : focus_offset.x + 16.0F;
        //focus_offset.y = random::number(0, 1) ? focus_offset.y - 16.0F : focus_offset.y + 16.0F;
        //focus_offset.x = focus_offset.x > 0.0F ? -16.0F : 16.0F;
        //focus_offset.y = focus_offset.y > 0.0F ? -16.0F : 16.0F;
    }
    void update() {
        if (focus_transform == -1) return;
        //console::log(focus_transform, " position: ", position.x, " ", position.y, "\n");

        if (time_left_shaking > 0) {
            //console::log("camera::update() time left shaking: ", time_left_shaking, " ", shake_min.x, " ", shake_max.x, "\n");
            if (time_left_shaking % 2 == 0) {
                shake();
            }
            --time_left_shaking;

            
        } else {
            focus_offset *= {0.5F, 0.5F};
        }

        if (focal_point.x < 16.0F) {
            focal_point.x = 16.0F;
        } else if (focal_point.x > WINDOW_W - 16.0F) {
            focal_point.x = WINDOW_W - 16.0F;
        }
        if (focal_point.y < 0.0F) {
            focal_point.y = 0.0F;
        } else if (focal_point.y > WINDOW_H - 16.0F) {
            focal_point.y = WINDOW_H - 16.0F;
        }

        //difference = { 0.0F, 0.0F };
        if (transform::position(focus_transform).x + focus_offset.x > focal_point.x ||
            transform::position(focus_transform).x + focus_offset.x < focal_point.x) {
            difference.x = (transform::position(focus_transform).x + focus_offset.x - focal_point.x) * speed.x;

            if (difference.x < 0 && position.x <= 0.0F) {
                difference.x = -position.x;
            } else if (difference.x > 0 && position.x >= scroll_limit.x) {
                difference.x = scroll_limit.x - position.x;
            } else {
                // slow down at level edges
                if (difference.x < 0.0F && position.x < difference.x * -10.0F) {
                    difference.x = -position.x / 20.0F;
                } else if (difference.x > 0.0F && position.x > scroll_limit.x - difference.x * 10.0F) {
                    difference.x = -(position.x - scroll_limit.x) / 20.0F;
                }
            }
            for (auto& i : s_transform_ids) {
                transform::position(i, transform::position(i) - Vec2F{ difference.x, 0.0F });
                transform::start_position(i, transform::start_position(i) - Vec2F{ difference.x, 0.0F });
            }
            position.x += difference.x;
        }
        if (transform::position(focus_transform).y + focus_offset.y > focal_point.y ||
            transform::position(focus_transform).y + focus_offset.y < focal_point.y) {
            difference.y = (transform::position(focus_transform).y + focus_offset.y - focal_point.y) * speed.y;

            if (difference.y < 0.0F && position.y <= 0.0F) {
                difference.y = -position.y;
            } else if (difference.y > 0.0F && position.y >= scroll_limit.y) {
                difference.y = scroll_limit.y - position.y;
            } else {
                // slow down at level edges
                if (difference.y < 0.0F && position.y < difference.y * -10.0F) {
                    difference.y = -position.y / 20.0F;
                } else if (difference.y > 0.0F && position.y > scroll_limit.y - difference.y * 10.0F) {
                    difference.y = -(position.y - scroll_limit.y) / 20.0F;
                }
            }
            for (auto& i : s_transform_ids) {
                transform::position(i, transform::position(i) - Vec2F{ 0.0F, difference.y });
                transform::start_position(i, transform::start_position(i) - Vec2F{ 0.0F, difference.y });
            }
            position.y += difference.y;
        }
    }
}