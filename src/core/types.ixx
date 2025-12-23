module;
#include <concepts>
#include <string>
#include <map>

export module types;

export using   U8 =       unsigned char;
export using  cU8 = const unsigned char;
export using  U16 =       unsigned short;
export using cU16 = const unsigned short;
export using  U32 =       unsigned int;
export using cU32 = const unsigned int;
export using  U64 =       unsigned long;
export using cU64 = const unsigned long;

export using   I8 =       char;
export using  cI8 = const char;
export using  I16 =       short;
export using cI16 = const short;
export using  I32 =       int;
export using cI32 = const int;
export using  I64 =       long;
export using cI64 = const long;

export using  F32 =       float;
export using cF32 = const float;
export using  F64 =       double;
export using cF64 = const double;

export constexpr U16 WINDOW_W = 320;
export constexpr U16 WINDOW_H = 180;

export constexpr U16 U16_MAX = -1;
//export constexpr cU8 NUM_LAYERS = 2;

export constexpr U8 NUM_VISIBLE_LAYERS = 15;
export constexpr U8 NUM_LEVEL_LAYERS = 10;

export namespace aabb {
    enum class Name {
        NONE = 0,
        ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
        UP, DOWN, LEFT, RIGHT,
        BODY, BODY_SWIM,
        BONE,
        HIT_GROUND,   
        TRACK
    };
    using cName = const Name;
    const std::string to_string(cName n) {
        switch (n) {
        case Name::ONE:        return "ONE";        break;
        case Name::TWO:        return "TWO";        break;
        case Name::THREE:      return "THREE";      break;
        case Name::FOUR:       return "FOUR";       break;
        case Name::FIVE:       return "FIVE";       break;
        case Name::SIX:        return "SIX";        break;
        case Name::SEVEN:      return "SEVEN";      break;
        case Name::EIGHT:      return "EIGHT";      break;
        case Name::NINE:       return "NINE";       break;

        case Name::UP:         return "UP";         break;
        case Name::DOWN:       return "DOWN";       break;
        case Name::LEFT:       return "LEFT";       break;
        case Name::RIGHT:      return "RIGHT";      break;

        case Name::BODY:       return "BODY";       break;
        case Name::BODY_SWIM:  return "BODY_SWIM";  break;
        case Name::BONE:       return "BONE";       break;
        case Name::HIT_GROUND: return "HIT_GROUND"; break;
        case Name::TRACK:      return "TRACK";      break;
        default:               return "";           break;
        }
    }

    const std::map<std::string, Name> string_to_aabb_name_map{
        { "ONE"        , Name::ONE        },
        { "TWO"        , Name::TWO        },
        { "THREE"      , Name::THREE      },
        { "FOUR"       , Name::FOUR       },
        { "FIVE"       , Name::FIVE       },
        { "SIX"        , Name::SIX        },
        { "SEVEN"      , Name::SEVEN      },
        { "EIGHT"      , Name::EIGHT      },
        { "NINE"       , Name::NINE       },        
        { "UP"         , Name::UP         },
        { "DOWN"       , Name::DOWN       },
        { "LEFT"       , Name::LEFT       },
        { "RIGHT"      , Name::RIGHT      },
        { "BODY"       , Name::BODY       },
        { "BODY_SWIM"  , Name::BODY_SWIM  },
        { "BONE"       , Name::BONE       },
        { "HIT_GROUND" , Name::HIT_GROUND },
        { "TRACK"      , Name::TRACK      }        
    };
    cName string_to_name(const std::string& s) {        
        return string_to_aabb_name_map.find(s) == string_to_aabb_name_map.end() ? Name::NONE : string_to_aabb_name_map.at(s);
    }
}

export namespace anim {
    enum class Type {
        NONE = 0,
        BOUNCE,
        CRAWL,
        DEAD, DOWN_THRUST, DUCK,
        FALL,
        HURT, HIT_GROUND, HOVER,
        IDLE,
        JUMP, JUMP_SPIN, JUMP_SKID,
        LEDGE_GRAB, LEDGE_CLIMB, LEVER,
        MELEE,
        RISE, RUN,
        SKID, SLIDE_GROUND, SLIDE_WALL, STUNNED, SWIM, SWING,
        WALK,
        UPENDED
    };
    using cType = const Type;
    const std::string to_string(cType t) {
        switch (t) {
        case Type::BOUNCE:       return "BOUNCE";
        case Type::CRAWL:        return "CRAWL";
        case Type::DEAD:         return "DEAD";
        case Type::DOWN_THRUST:  return "DOWN_THRUST";
        case Type::DUCK:         return "DUCK";
        case Type::FALL:         return "FALL";
        case Type::HURT:         return "HURT";
        case Type::HIT_GROUND:   return "HIT_GROUND";
        case Type::HOVER:        return "HOVER";
        case Type::IDLE:         return "IDLE";
        case Type::JUMP:         return "JUMP";
        case Type::JUMP_SPIN:    return "JUMP_SPIN";
        case Type::JUMP_SKID:    return "JUMP_SKID";
        case Type::LEDGE_GRAB:   return "LEDGE_GRAB";
        case Type::LEDGE_CLIMB:  return "LEDGE_CLIMB";
        case Type::LEVER:        return "LEVER";
        case Type::MELEE:        return "MELEE";
        case Type::RISE:         return "RISE";
        case Type::RUN:          return "RUN";
        case Type::SKID:         return "SKID";
        case Type::SLIDE_GROUND: return "SLIDE_GROUND";
        case Type::SLIDE_WALL:   return "SLIDE_WALL";
        case Type::STUNNED:      return "STUNNED";
        case Type::SWIM:         return "SWIM";
        case Type::SWING:        return "SWING";
        case Type::WALK:         return "WALK";
        case Type::UPENDED:      return "UPENDED";
        default:                 return "";
        }
    }
    const std::map<std::string, Type> string_to_anim_type_map{
        { "BOUNCE"       , Type::BOUNCE       },
        { "CRAWL"        , Type::CRAWL        },
        { "DEAD"         , Type::DEAD         },
        { "DOWN_THRUST"  , Type::DOWN_THRUST  },
        { "DUCK"         , Type::DUCK         },
        { "FALL"         , Type::FALL         },
        { "HURT"         , Type::HURT         },
        { "HIT_GROUND"   , Type::HIT_GROUND   },
        { "HOVER"        , Type::HOVER        },
        { "IDLE"         , Type::IDLE         },
        { "JUMP"         , Type::JUMP         },
        { "JUMP_SPIN"    , Type::JUMP_SPIN    },
        { "JUMP_SKID"    , Type::JUMP_SKID    },
        { "LEDGE_GRAB"   , Type::LEDGE_GRAB   },
        { "LEDGE_CLIMB"  , Type::LEDGE_CLIMB  },
        { "LEVER"        , Type::LEVER        },
        { "MELEE"        , Type::MELEE        },
        { "RISE"         , Type::RISE         },
        { "RUN"          , Type::RUN          },
        { "SKID"         , Type::SKID         },
        { "SLIDE_GROUND" , Type::SLIDE_GROUND },
        { "SLIDE_WALL"   , Type::SLIDE_WALL   },
        { "STUNNED"      , Type::STUNNED      },
        { "SWIM"         , Type::SWIM         },
        { "SWING"        , Type::SWING        },
        { "WALK"         , Type::WALK         },
        { "UPENDED"      , Type::UPENDED      }
    };
    cType string_to_type(const std::string& s) {
        if (string_to_anim_type_map.find(s) != string_to_anim_type_map.end()) {
            return string_to_anim_type_map.at(s);
        }
        return Type::NONE;
    }
}

export namespace entity {
    enum class Type {
        NONE = 0,
        ARCH_L_1X1, ARCH_R_1X1, ARCH_L_2X1_0, ARCH_L_2X1_1, ARCH_R_2X1_0, ARCH_R_2X1_1,
        BRICK, BUG,
        CLIP, CLIP_D, CLIP_LD, CLIP_RD, CLIP_L, CLIP_R, CLIP_LEDGE, CLIP_U,

        CONDUIT_UD, CONDUIT_LR,
        CONDUIT_CORNER_UL, CONDUIT_CORNER_UR, CONDUIT_CORNER_DL, CONDUIT_CORNER_DR,
        CONDUIT_L_1X1_0, CONDUIT_L_1X1_1, CONDUIT_R_1X1_0, CONDUIT_R_1X1_1,
        CONDUIT_L_1X2_0, CONDUIT_L_1X2_1, CONDUIT_R_1X2_0, CONDUIT_R_1X2_1,
        CONDUIT_L_2X1_0, CONDUIT_L_2X1_1, CONDUIT_R_2X1_0, CONDUIT_R_2X1_1,
        
        FROG,
        GRASS_0, GRASS_0_L_1X1, GRASS_0_R_1X1, GRASS_0_L_2X1_0, GRASS_0_L_2X1_1, GRASS_0_R_2X1_0, GRASS_0_R_2X1_1,
        GRASS_1, GRASS_1_L_1X1, GRASS_1_R_1X1, GRASS_1_L_2X1_0, GRASS_1_L_2X1_1, GRASS_1_R_2X1_0, GRASS_1_R_2X1_1,
        GRASS_2, GRASS_2_L_1X1, GRASS_2_R_1X1, GRASS_2_L_2X1_0, GRASS_2_L_2X1_1, GRASS_2_R_2X1_0, GRASS_2_R_2X1_1,
        LEVEL_CENTER, LEVEL_L_0, LEVEL_R_0,
        LOGIC_AND_UD, LOGIC_AND_LR,
        LOGIC_NOT_U, LOGIC_NOT_D, LOGIC_NOT_L, LOGIC_NOT_R,
        LOGIC_OR_UD, LOGIC_OR_LR,
        MOLE,
        PARTICLE_BRICK, PARTICLE_BUBBLE,
        PARTICLE_DOWN_THRUST, PARTICLE_DROP,
        PARTICLE_HEALTH,
        PARTICLE_INTERACT,
        PARTICLE_MELEE,
        PARTICLE_SENSE, PARTICLE_SHOT,
        PLAYER,
        SLOPE_L_1X1, SLOPE_R_1X1, SLOPE_L_2X1_0, SLOPE_L_2X1_1, SLOPE_R_2X1_0, SLOPE_R_2X1_1, SLOPE_U,
        SPRING_U,
        SWING,
        
        TRACK_UD, TRACK_LR,
        TRACK_CORNER_UL, TRACK_CORNER_UR, TRACK_CORNER_DL, TRACK_CORNER_DR,
        TRACK_L_1X1_0, TRACK_L_1X1_1, TRACK_R_1X1_0, TRACK_R_1X1_1,
        TRACK_L_1X2_0, TRACK_L_1X2_1, TRACK_R_1X2_0, TRACK_R_1X2_1,
        TRACK_L_2X1_0, TRACK_L_2X1_1, TRACK_R_2X1_0, TRACK_R_2X1_1,

        TRAIN,
        TRIGGER, CONDUIT_TRIGGER_UL, CONDUIT_TRIGGER_UR, TRACK_TRIGGER_UL, TRACK_TRIGGER_UR,
        WATER_LINE, WATER_LINE_L, WATER_LINE_R
    };
    using cType = const Type;

    bool is_arch(cType t) {
        return t == Type::ARCH_L_1X1 || t == Type::ARCH_L_2X1_0 || t == Type::ARCH_L_2X1_1 ||
            t == Type::ARCH_R_1X1 || t == Type::ARCH_R_2X1_0 || t == Type::ARCH_R_2X1_1;
    }
    bool is_slope(cType t) {
        return t == Type::SLOPE_L_1X1 || t == Type::SLOPE_L_2X1_0 || t == Type::SLOPE_L_2X1_1 ||
            t == Type::SLOPE_R_1X1 || t == Type::SLOPE_R_2X1_0 || t == Type::SLOPE_R_2X1_1 ||
            t == Type::SLOPE_U;
    }
    bool is_clip(cType t) {
        return t == Type::CLIP || t == Type::CLIP_U || t == Type::CLIP_D ||
            t == Type::CLIP_L || t == Type::CLIP_R || t == Type::CLIP_LD || t == Type::CLIP_RD ||
            t == Type::CLIP_LEDGE;
    }
    bool is_conduit(cType t) {
        return t == Type::CONDUIT_TRIGGER_UL || t == Type::CONDUIT_TRIGGER_UR ||
            t == Type::CONDUIT_UD || t == Type::CONDUIT_LR ||            
            t == Type::CONDUIT_CORNER_UL || t == Type::CONDUIT_CORNER_UR || t == Type::CONDUIT_CORNER_DL || t == Type::CONDUIT_CORNER_DR ||
            t == Type::CONDUIT_L_1X1_0 || t == Type::CONDUIT_L_1X1_1 || t == Type::CONDUIT_R_1X1_0 || t == Type::CONDUIT_R_1X1_1 ||
            t == Type::CONDUIT_L_1X2_0 || t == Type::CONDUIT_L_1X2_1 || t == Type::CONDUIT_R_1X2_0 || t == Type::CONDUIT_R_1X2_1 ||
            t == Type::CONDUIT_L_2X1_0 || t == Type::CONDUIT_L_2X1_1 || t == Type::CONDUIT_R_2X1_0 || t == Type::CONDUIT_R_2X1_1
            ;
    }

    bool is_grass_0(cType t) { return t == Type::GRASS_0 || t == Type::GRASS_0_L_1X1 || t == Type::GRASS_0_R_1X1 || t == Type::GRASS_0_L_2X1_0 || t == Type::GRASS_0_L_2X1_1 || t == Type::GRASS_0_R_2X1_0 || t == Type::GRASS_0_R_2X1_1; }
    bool is_grass_1(cType t) { return t == Type::GRASS_1 || t == Type::GRASS_1_L_1X1 || t == Type::GRASS_1_R_1X1 || t == Type::GRASS_1_L_2X1_0 || t == Type::GRASS_1_L_2X1_1 || t == Type::GRASS_1_R_2X1_0 || t == Type::GRASS_1_R_2X1_1; }
    bool is_grass_2(cType t) { return t == Type::GRASS_2 || t == Type::GRASS_2_L_1X1 || t == Type::GRASS_2_R_1X1 || t == Type::GRASS_2_L_2X1_0 || t == Type::GRASS_2_L_2X1_1 || t == Type::GRASS_2_R_2X1_0 || t == Type::GRASS_2_R_2X1_1; }
    bool is_grass(cType t) { return is_grass_0(t) || is_grass_1(t) || is_grass_2(t); }
    bool is_logic_and(cType t) { return t == Type::LOGIC_AND_UD || t == Type::LOGIC_AND_LR; }
    bool is_logic_not(cType t) { return t == Type::LOGIC_NOT_U || t == Type::LOGIC_NOT_D || t == Type::LOGIC_NOT_L || t == Type::LOGIC_NOT_R; }
    bool is_logic_or(cType t) { return t == Type::LOGIC_OR_UD || t == Type::LOGIC_OR_LR; }
    bool is_logic(cType t) { return is_logic_and(t) || is_logic_not(t) || is_logic_or(t); }
    bool is_particle(cType t) { return t == Type::PARTICLE_SENSE; }
    bool is_track(cType t) {
        return t == Type::TRACK_TRIGGER_UL || t == Type::TRACK_TRIGGER_UR ||
               t == Type::TRACK_UD || t == Type::TRACK_LR ||
               t == Type::TRACK_CORNER_UL || t == Type::TRACK_CORNER_UR || t == Type::TRACK_CORNER_DL || t == Type::TRACK_CORNER_DR ||
               t == Type::TRACK_L_1X1_0 || t == Type::TRACK_L_1X1_1 || t == Type::TRACK_R_1X1_0 || t == Type::TRACK_R_1X1_1 ||
               t == Type::TRACK_L_1X2_0 || t == Type::TRACK_L_1X2_1 || t == Type::TRACK_R_1X2_0 || t == Type::TRACK_R_1X2_1 ||
               t == Type::TRACK_L_2X1_0 || t == Type::TRACK_L_2X1_1 || t == Type::TRACK_R_2X1_0 || t == Type::TRACK_R_2X1_1;
    }
    bool is_water_line(cType t) { return t == Type::WATER_LINE || t == Type::WATER_LINE_L || t == Type::WATER_LINE_R; }

    const std::map<std::string, Type> string_to_entity_type_map{
        { "ARCH_L_1X1"           , Type::ARCH_L_1X1           },
        { "ARCH_R_1X1"           , Type::ARCH_R_1X1           },
        { "ARCH_L_2X1_0"         , Type::ARCH_L_2X1_0         },
        { "ARCH_L_2X1_1"         , Type::ARCH_L_2X1_1         },
        { "ARCH_R_2X1_0"         , Type::ARCH_R_2X1_0         },
        { "ARCH_R_2X1_1"         , Type::ARCH_R_2X1_1         },
        { "BRICK"                , Type::BRICK                },
        { "BUG"                  , Type::BUG                  },
        { "CLIP"                 , Type::CLIP                 },
        { "CLIP_L"               , Type::CLIP_L               },
        { "CLIP_R"               , Type::CLIP_R               },
        { "CLIP_D"               , Type::CLIP_D               },
        { "CLIP_LD"              , Type::CLIP_LD              },
        { "CLIP_RD"              , Type::CLIP_RD              },
        { "CLIP_LEDGE"           , Type::CLIP_LEDGE           },
        { "CLIP_U"               , Type::CLIP_U               },

        { "CONDUIT_TRIGGER_UL"     , Type::CONDUIT_TRIGGER_UL     },
        { "CONDUIT_TRIGGER_UR"     , Type::CONDUIT_TRIGGER_UR     },

        { "CONDUIT_UD"           , Type::CONDUIT_UD           },        
        { "CONDUIT_LR"           , Type::CONDUIT_LR           },
        { "CONDUIT_CORNER_UL"    , Type::CONDUIT_CORNER_UL    },
        { "CONDUIT_CORNER_UR"    , Type::CONDUIT_CORNER_UR    },
        { "CONDUIT_CORNER_DL"    , Type::CONDUIT_CORNER_DL    },
        { "CONDUIT_CORNER_DR"    , Type::CONDUIT_CORNER_DR    },
        { "CONDUIT_L_1X1_0"    , Type::CONDUIT_L_1X1_0    },
        { "CONDUIT_L_1X1_1"    , Type::CONDUIT_L_1X1_1    },
        { "CONDUIT_R_1X1_0"    , Type::CONDUIT_R_1X1_0    },
        { "CONDUIT_R_1X1_1"    , Type::CONDUIT_R_1X1_1    },
        { "CONDUIT_L_1X2_0"    , Type::CONDUIT_L_1X2_0    },
        { "CONDUIT_L_1X2_1"    , Type::CONDUIT_L_1X2_1    },
        { "CONDUIT_R_1X2_0"    , Type::CONDUIT_R_1X2_0    },
        { "CONDUIT_R_1X2_1"    , Type::CONDUIT_R_1X2_1    },
        { "CONDUIT_L_2X1_0"    , Type::CONDUIT_L_2X1_0    },
        { "CONDUIT_L_2X1_1"    , Type::CONDUIT_L_2X1_1    },
        { "CONDUIT_R_2X1_0"    , Type::CONDUIT_R_2X1_0    },
        { "CONDUIT_R_2X1_1"    , Type::CONDUIT_R_2X1_1    },
        { "FROG"                 , Type::FROG                 },
        { "GRASS_0"              , Type::GRASS_0              },
        { "GRASS_0_L_1X1"        , Type::GRASS_0_L_1X1        },
        { "GRASS_0_R_1X1"        , Type::GRASS_0_R_1X1        },
        { "GRASS_0_L_2X1_0"      , Type::GRASS_0_L_2X1_0      },
        { "GRASS_0_L_2X1_1"      , Type::GRASS_0_L_2X1_1      },
        { "GRASS_0_R_2X1_0"      , Type::GRASS_0_R_2X1_0      },
        { "GRASS_0_R_2X1_1"      , Type::GRASS_0_R_2X1_1      },
        { "GRASS_1"              , Type::GRASS_1              },
        { "GRASS_1_L_1X1"        , Type::GRASS_1_L_1X1        },
        { "GRASS_1_R_1X1"        , Type::GRASS_1_R_1X1        },
        { "GRASS_1_L_2X1_0"      , Type::GRASS_1_L_2X1_0      },
        { "GRASS_1_L_2X1_1"      , Type::GRASS_1_L_2X1_1      },
        { "GRASS_1_R_2X1_0"      , Type::GRASS_1_R_2X1_0      },
        { "GRASS_1_R_2X1_1"      , Type::GRASS_1_R_2X1_1      },
        { "GRASS_2"              , Type::GRASS_2              },
        { "GRASS_2_L_1X1"        , Type::GRASS_2_L_1X1        },
        { "GRASS_2_R_1X1"        , Type::GRASS_2_R_1X1        },
        { "GRASS_2_L_2X1_0"      , Type::GRASS_2_L_2X1_0      },
        { "GRASS_2_L_2X1_1"      , Type::GRASS_2_L_2X1_1      },
        { "GRASS_2_R_2X1_0"      , Type::GRASS_2_R_2X1_0      },
        { "GRASS_2_R_2X1_1"      , Type::GRASS_2_R_2X1_1      },
        { "LEVEL_CENTER"         , Type::LEVEL_CENTER         },
        { "LEVEL_L_0"            , Type::LEVEL_L_0            },
        { "LEVEL_R_0"            , Type::LEVEL_R_0            },
        { "LOGIC_AND_UD"         , Type::LOGIC_AND_UD         },
        { "LOGIC_AND_LR"         , Type::LOGIC_AND_LR         },
        { "LOGIC_NOT_U"          , Type::LOGIC_NOT_U          },
        { "LOGIC_NOT_D"          , Type::LOGIC_NOT_D          },
        { "LOGIC_NOT_L"          , Type::LOGIC_NOT_L          },
        { "LOGIC_NOT_R"          , Type::LOGIC_NOT_R          },
        { "LOGIC_OR_UD"          , Type::LOGIC_OR_UD          },
        { "LOGIC_OR_LR"          , Type::LOGIC_OR_LR          },
        { "MOLE"                 , Type::MOLE                 },
        { "PARTICLE_BRICK"       , Type::PARTICLE_BRICK       },
        { "PARTICLE_BUBBLE"      , Type::PARTICLE_BUBBLE      },
        { "PARTICLE_DOWN_THRUST" , Type::PARTICLE_DOWN_THRUST },
        { "PARTICLE_DROP"        , Type::PARTICLE_DROP        },
        { "PARTICLE_HEALTH"      , Type::PARTICLE_HEALTH      },
        { "PARTICLE_INTERACT"    , Type::PARTICLE_INTERACT    },
        { "PARTICLE_MELEE"       , Type::PARTICLE_MELEE       },
        { "PARTICLE_SENSE"       , Type::PARTICLE_SENSE       },
        { "PARTICLE_SHOT"        , Type::PARTICLE_SHOT        },
        { "PLAYER"               , Type::PLAYER               },
        { "SLOPE_L_1X1"          , Type::SLOPE_L_1X1          },
        { "SLOPE_L_2X1_0"        , Type::SLOPE_L_2X1_0        },
        { "SLOPE_L_2X1_1"        , Type::SLOPE_L_2X1_1        },
        { "SLOPE_R_1X1"          , Type::SLOPE_R_1X1          },
        { "SLOPE_R_2X1_0"        , Type::SLOPE_R_2X1_0        },
        { "SLOPE_R_2X1_1"        , Type::SLOPE_R_2X1_1        },
        { "SPRING_U"             , Type::SPRING_U             },
        { "SLOPE_U"              , Type::SLOPE_U              },
        { "SWING"                , Type::SWING                },
        { "TRAIN"                , Type::TRAIN                },


        { "TRACK_TRIGGER_UL"     , Type::TRACK_TRIGGER_UL     },
        { "TRACK_TRIGGER_UR"     , Type::TRACK_TRIGGER_UR     },
        
        { "TRACK_UD"       , Type::TRACK_UD },
        { "TRACK_LR"       , Type::TRACK_LR },
        { "TRACK_CORNER_UL"      , Type::TRACK_CORNER_UL },
        { "TRACK_CORNER_UR"      , Type::TRACK_CORNER_UR },
        { "TRACK_CORNER_DL"      , Type::TRACK_CORNER_DL },
        { "TRACK_CORNER_DR"      , Type::TRACK_CORNER_DR },
        { "TRACK_L_1X1_0"      , Type::TRACK_L_1X1_0 },
        { "TRACK_L_1X1_1"      , Type::TRACK_L_1X1_1 },
        { "TRACK_R_1X1_0"      , Type::TRACK_R_1X1_0 },
        { "TRACK_R_1X1_1"      , Type::TRACK_R_1X1_1 },
        { "TRACK_L_1X2_0"      , Type::TRACK_L_1X2_0 },
        { "TRACK_L_1X2_1"      , Type::TRACK_L_1X2_1 },
        { "TRACK_R_1X2_0"      , Type::TRACK_R_1X2_0 },
        { "TRACK_R_1X2_1"      , Type::TRACK_R_1X2_1 },
        { "TRACK_L_2X1_0"      , Type::TRACK_L_2X1_0 },
        { "TRACK_L_2X1_1"      , Type::TRACK_L_2X1_1 },
        { "TRACK_R_2X1_0"      , Type::TRACK_R_2X1_0 },
        { "TRACK_R_2X1_1"      , Type::TRACK_R_2X1_1 },

        { "TRIGGER"              , Type::TRIGGER              },

        { "WATER_LINE"           , Type::WATER_LINE           },
        { "WATER_LINE_L"         , Type::WATER_LINE_L         },
        { "WATER_LINE_R"         , Type::WATER_LINE_R         }
    };
    cType string_to_type(const std::string& s) {
        if (string_to_entity_type_map.find(s) != string_to_entity_type_map.end()) {
            return string_to_entity_type_map.at(s);
        }
        return Type::NONE;
    }
    const std::string to_string(cType type) {        
        switch (type) {
            case Type::ARCH_L_1X1:           return "ARCH_L_1X1";
            case Type::ARCH_R_1X1:           return "ARCH_R_1X1";
            case Type::ARCH_L_2X1_0:         return "ARCH_L_2X1_0";
            case Type::ARCH_L_2X1_1:         return "ARCH_L_2X1_1";
            case Type::ARCH_R_2X1_0:         return "ARCH_R_2X1_0";
            case Type::ARCH_R_2X1_1:         return "ARCH_R_2X1_1";
            case Type::BRICK:                return "BRICK";
            case Type::BUG:                  return "BUG";
            case Type::CLIP:                 return "CLIP";
            case Type::CLIP_L:               return "CLIP_L";
            case Type::CLIP_R:               return "CLIP_R";
            case Type::CLIP_D:               return "CLIP_D";
            case Type::CLIP_LD:              return "CLIP_LD";
            case Type::CLIP_RD:              return "CLIP_RD";
            case Type::CLIP_LEDGE:           return "CLIP_LEDGE";
            case Type::CLIP_U:               return "CLIP_U";

            case Type::CONDUIT_TRIGGER_UL:     return "CONDUIT_TRIGGER_UL";
            case Type::CONDUIT_TRIGGER_UR:     return "CONDUIT_TRIGGER_UR";

            case Type::CONDUIT_UD:           return "CONDUIT_UD";
            case Type::CONDUIT_LR:           return "CONDUIT_LR";
            case Type::CONDUIT_CORNER_UL:    return "CONDUIT_CORNER_UL";
            case Type::CONDUIT_CORNER_UR:    return "CONDUIT_CORNER_UR";
            case Type::CONDUIT_CORNER_DL:    return "CONDUIT_CORNER_DL";
            case Type::CONDUIT_CORNER_DR:    return "CONDUIT_CORNER_DR";
            case Type::CONDUIT_L_1X1_0:      return "CONDUIT_L_1X1_0";
            case Type::CONDUIT_L_1X1_1:      return "CONDUIT_L_1X1_1";
            case Type::CONDUIT_R_1X1_0:      return "CONDUIT_R_1X1_0";
            case Type::CONDUIT_R_1X1_1:      return "CONDUIT_R_1X1_1";
            case Type::CONDUIT_L_1X2_0:      return "CONDUIT_L_1X2_0";
            case Type::CONDUIT_L_1X2_1:      return "CONDUIT_L_1X2_1";
            case Type::CONDUIT_R_1X2_0:      return "CONDUIT_R_1X2_0";
            case Type::CONDUIT_R_1X2_1:      return "CONDUIT_R_1X2_1";
            case Type::CONDUIT_L_2X1_0:      return "CONDUIT_L_2X1_0";
            case Type::CONDUIT_L_2X1_1:      return "CONDUIT_L_2X1_1";
            case Type::CONDUIT_R_2X1_0:      return "CONDUIT_R_2X1_0";
            case Type::CONDUIT_R_2X1_1:      return "CONDUIT_R_2X1_1";

            case Type::FROG:                 return "FROG";
            case Type::GRASS_0:              return "GRASS_0";
            case Type::GRASS_0_L_1X1:        return "GRASS_0_L_1X1";
            case Type::GRASS_0_R_1X1:        return "GRASS_0_R_1X1";
            case Type::GRASS_0_L_2X1_0:      return "GRASS_0_L_2X1_0";
            case Type::GRASS_0_L_2X1_1:      return "GRASS_0_L_2X1_1";
            case Type::GRASS_0_R_2X1_0:      return "GRASS_0_R_2X1_0";
            case Type::GRASS_0_R_2X1_1:      return "GRASS_0_R_2X1_1";
            case Type::GRASS_1:              return "GRASS_1";
            case Type::GRASS_1_L_1X1:        return "GRASS_1_L_1X1";
            case Type::GRASS_1_R_1X1:        return "GRASS_1_R_1X1";
            case Type::GRASS_1_L_2X1_0:      return "GRASS_1_L_2X1_0";
            case Type::GRASS_1_L_2X1_1:      return "GRASS_1_L_2X1_1";
            case Type::GRASS_1_R_2X1_0:      return "GRASS_1_R_2X1_0";
            case Type::GRASS_1_R_2X1_1:      return "GRASS_1_R_2X1_1";
            case Type::GRASS_2:              return "GRASS_2";
            case Type::GRASS_2_L_1X1:        return "GRASS_2_L_1X1";
            case Type::GRASS_2_R_1X1:        return "GRASS_2_R_1X1";
            case Type::GRASS_2_L_2X1_0:      return "GRASS_2_L_2X1_0";
            case Type::GRASS_2_L_2X1_1:      return "GRASS_2_L_2X1_1";
            case Type::GRASS_2_R_2X1_0:      return "GRASS_2_R_2X1_0";
            case Type::GRASS_2_R_2X1_1:      return "GRASS_2_R_2X1_1";
            case Type::LEVEL_CENTER:         return "LEVEL_CENTER";
            case Type::LEVEL_L_0:            return "LEVEL_L_0";
            case Type::LEVEL_R_0:            return "LEVEL_R_0";
            case Type::LOGIC_AND_UD:         return "LOGIC_AND_UD";
            case Type::LOGIC_AND_LR:         return "LOGIC_AND_LR";
            case Type::LOGIC_NOT_U:          return "LOGIC_NOT_U";
            case Type::LOGIC_NOT_D:          return "LOGIC_NOT_D";
            case Type::LOGIC_NOT_L:          return "LOGIC_NOT_L";
            case Type::LOGIC_NOT_R:          return "LOGIC_NOT_R";
            case Type::LOGIC_OR_UD:          return "LOGIC_OR_UD";
            case Type::LOGIC_OR_LR:          return "LOGIC_OR_LR";
            case Type::MOLE:                 return "MOLE";
            case Type::PARTICLE_BRICK:       return "PARTICLE_BRICK";
            case Type::PARTICLE_BUBBLE:      return "PARTICLE_BUBBLE";
            case Type::PARTICLE_DOWN_THRUST: return "PARTICLE_DOWN_THRUST";
            case Type::PARTICLE_DROP:        return "PARTICLE_DROP";
            case Type::PARTICLE_HEALTH:      return "PARTICLE_HEALTH";
            case Type::PARTICLE_INTERACT:    return "PARTICLE_INTERACT";
            case Type::PARTICLE_MELEE:       return "PARTICLE_MELEE";
            case Type::PARTICLE_SENSE:       return "PARTICLE_SENSE";
            case Type::PARTICLE_SHOT:        return "PARTICLE_SHOT";
            case Type::PLAYER:               return "PLAYER";
            case Type::SLOPE_L_1X1:          return "SLOPE_L_1X1";
            case Type::SLOPE_R_1X1:          return "SLOPE_R_1X1";
            case Type::SLOPE_L_2X1_0:        return "SLOPE_L_2X1_0";
            case Type::SLOPE_L_2X1_1:        return "SLOPE_L_2X1_1";
            case Type::SLOPE_R_2X1_0:        return "SLOPE_R_2X1_0";
            case Type::SLOPE_R_2X1_1:        return "SLOPE_R_2X1_1";
            case Type::SLOPE_U:              return "SLOPE_U";
            case Type::SPRING_U:             return "SPRING_U";
            case Type::SWING:                return "SWING";


            case Type::TRACK_TRIGGER_UL:     return "TRACK_TRIGGER_UL";
            case Type::TRACK_TRIGGER_UR:     return "TRACK_TRIGGER_UR";

            case Type::TRACK_UD:       return "TRACK_UD";
            case Type::TRACK_LR:       return "TRACK_LR";
            case Type::TRACK_CORNER_UL:       return "TRACK_CORNER_UL";
            case Type::TRACK_CORNER_UR:       return "TRACK_CORNER_UR";
            case Type::TRACK_CORNER_DL:       return "TRACK_CORNER_DL";
            case Type::TRACK_CORNER_DR:       return "TRACK_CORNER_DR";
            case Type::TRACK_L_1X1_0:       return "TRACK_L_1X1_0";
            case Type::TRACK_L_1X1_1:       return "TRACK_L_1X1_1";
            case Type::TRACK_R_1X1_0:       return "TRACK_R_1X1_0";
            case Type::TRACK_R_1X1_1:       return "TRACK_R_1X1_1";
            case Type::TRACK_L_1X2_0:       return "TRACK_L_1X2_0";
            case Type::TRACK_L_1X2_1:       return "TRACK_L_1X2_1";
            case Type::TRACK_R_1X2_0:       return "TRACK_R_1X2_0";
            case Type::TRACK_R_1X2_1:       return "TRACK_R_1X2_1";
            case Type::TRACK_L_2X1_0:       return "TRACK_L_2X1_0";
            case Type::TRACK_L_2X1_1:       return "TRACK_L_2X1_1";
            case Type::TRACK_R_2X1_0:       return "TRACK_R_2X1_0";
            case Type::TRACK_R_2X1_1:       return "TRACK_R_2X1_1";

            case Type::TRAIN:                return "TRAIN";
            case Type::TRIGGER:              return "TRIGGER";
            case Type::WATER_LINE:           return "WATER_LINE";
            case Type::WATER_LINE_L:         return "WATER_LINE_L";
            case Type::WATER_LINE_R:         return "WATER_LINE_R";
            default:                         return "";
        }
    }
    enum class State {
        NONE = 0,
        BLOCKED, BOUNCE,
        CARRIED,
        DEAD,
        ENTER, EXIT,
        HURT,
        IDLE,
        JUMP,
        MELEE,
        RUN,
        SHOOT, STUNNED, SWIM, SWING,
        TOSSED,
        UPENDED,
        WALK
    };
    using cState = const State;
    const std::string to_string(cState s) {
        switch (s) {
        case State::BLOCKED:    return "BLOCKED";
        case State::BOUNCE:     return "BOUNCE";
        case State::CARRIED:    return "CARRIED";
        case State::DEAD:       return "DEAD";
        case State::ENTER:      return "ENTER";
        case State::EXIT:       return "EXIT";
        case State::HURT:       return "HURT";
        case State::IDLE:       return "IDLE";
        case State::JUMP:       return "JUMP";
        case State::MELEE:      return "MELEE";
        case State::RUN:        return "RUN";
        case State::SHOOT:      return "SHOOT";
        case State::STUNNED:    return "STUNNED";
        case State::SWIM:       return "SWIM";
        case State::SWING:      return "SWING";
        case State::TOSSED:     return "TOSSED";
        case State::UPENDED:    return "UPENDED";
        case State::WALK:       return "WALK";
        default:                return "";
        }
    }
}

export namespace input {
    enum class Key : I32 {
        NONE = -1,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, NUM_0,
        ENTER = 36, ESC, BACKSPACE, TAB, SPACE, SUBTRACT,
        GRAVE = 48, COMMA, PERIOD, SLASH,
        F1 = 52, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        DEL = 83,
        RIGHT = 86, LEFT, DOWN, UP,
        CTRL = 127, SHIFT, ALT
    };
    enum class Button : I32 {
        NONE = -1,
        LEFT, RIGHT, MIDDLE
    };
    I32 alphabet_begin() { return (I32)Key::A;         }
    I32 alphabet_end()   { return (I32)Key::Z + 1;     }
    I32 numbers_begin()  { return (I32)Key::NUM_1;     }
    I32 numbers_end()    { return (I32)Key::NUM_0 + 1; }
    I32 inputs_begin()   { return (I32)Key::A;         }
    I32 inputs_end()     { return (I32)Key::ALT + 1;   }

    const std::string string_from(Key key, bool is_shift) {
        switch (key) {
        case Key::A:        return is_shift ? "A" : "a";
        case Key::B:        return is_shift ? "B" : "b";
        case Key::C:        return is_shift ? "C" : "c";
        case Key::D:        return is_shift ? "D" : "d";
        case Key::E:        return is_shift ? "E" : "e";
        case Key::F:        return is_shift ? "F" : "f";
        case Key::G:        return is_shift ? "G" : "g";
        case Key::H:        return is_shift ? "H" : "h";
        case Key::I:        return is_shift ? "I" : "i";
        case Key::J:        return is_shift ? "J" : "j";
        case Key::K:        return is_shift ? "K" : "k";
        case Key::L:        return is_shift ? "L" : "l";
        case Key::M:        return is_shift ? "M" : "m";
        case Key::N:        return is_shift ? "N" : "n";
        case Key::O:        return is_shift ? "O" : "o";
        case Key::P:        return is_shift ? "P" : "p";
        case Key::Q:        return is_shift ? "Q" : "q";
        case Key::R:        return is_shift ? "R" : "r";
        case Key::S:        return is_shift ? "S" : "s";
        case Key::T:        return is_shift ? "T" : "t";
        case Key::U:        return is_shift ? "U" : "u";
        case Key::V:        return is_shift ? "V" : "v";
        case Key::W:        return is_shift ? "W" : "w";
        case Key::X:        return is_shift ? "X" : "x";
        case Key::Y:        return is_shift ? "Y" : "y";
        case Key::Z:        return is_shift ? "Z" : "z";
        case Key::NUM_0:    return is_shift ? ")" : "0";
        case Key::NUM_1:    return is_shift ? "!" : "1";
        case Key::NUM_2:    return is_shift ? "@" : "2";
        case Key::NUM_3:    return is_shift ? "#" : "3";
        case Key::NUM_4:    return is_shift ? "$" : "4";
        case Key::NUM_5:    return is_shift ? "%" : "5";
        case Key::NUM_6:    return is_shift ? "^" : "6";
        case Key::NUM_7:    return is_shift ? "&" : "7";
        case Key::NUM_8:    return is_shift ? "*" : "8";
        case Key::NUM_9:    return is_shift ? "(" : "9";
        case Key::SUBTRACT: return is_shift ? "_" : "-";
        case Key::COMMA:    return is_shift ? "<" : ",";
        case Key::PERIOD:   return is_shift ? ">" : ".";
        case Key::SLASH:    return is_shift ? "?" : "/";
        default:            return "";
        }
    }
    const std::string string_from(Key key) {
        return string_from(key, false);
    }
}

export namespace state {
    enum class Type {
        NONE = 0, EDIT, GAME, OVERLAY
    };
    using cType = const Type;
    Type type_from(const std::string_view& s) {
        if      (s == "EDIT")    return Type::EDIT;
        else if (s == "GAME")    return Type::GAME;
        else if (s == "OVERLAY") return Type::OVERLAY;
        return Type::NONE;
    }
    const std::string to_string(cType t) {
        switch (t) {
        case Type::EDIT:    return "EDIT";    break;
        case Type::GAME:    return "GAME";    break;
        case Type::OVERLAY: return "OVERLAY"; break;
        default:            return "";        break;
        }
    }
}
export namespace start {
    enum class Type { NONE = 0, CENTER, L_0, R_0 };
    using cType = const Type;
    struct Info {
        Type type   = Type::CENTER;
        U8   number = 0;
    };
    using cInfo = const Info;
    Type from_string(const char* s) {
        if      (s == "CENTER") return Type::CENTER;
        else if (s == "L_0")    return Type::L_0;
        else if (s == "R_0")    return Type::R_0;
        else                    return Type::NONE;
    }
    const std::string to_string(cType type) {
        switch (type) {
        case Type::CENTER:  return "CENTER";
        case Type::L_0:     return "L_0";
        case Type::R_0:     return "R_0";
        default:            return ""; }
    }
}

export namespace particle {
    enum class Type {
        NONE = 0,
        BRICK, BUBBLE,
        DOWN_THRUST, DROP, DROP_BLOOD, DUST, DUST_L, DUST_R,
        HEALTH, HIT,
        INTERACT,
        MELEE,
        SENSE, SHOT,
        TILE_LINE
    };
    using cType = const Type;
    const std::string to_string(cType t) {
        switch (t) {
        case Type::BRICK:       return "BRICK";
        case Type::BUBBLE:      return "BUBBLE";
        case Type::DOWN_THRUST: return "DOWN_THRUST";
        case Type::DROP:        return "DROP";
        case Type::DROP_BLOOD:  return "DROP_BLOOD";
        case Type::DUST:        return "DUST";
        case Type::DUST_L:      return "DUST_L";
        case Type::DUST_R:      return "DUST_R";
        case Type::HEALTH:      return "HEALTH";
        case Type::HIT:         return "HIT";
        case Type::INTERACT:    return "INTERACT";
        case Type::MELEE:       return "MELEE";
        case Type::SENSE:       return "SENSE";
        case Type::SHOT:        return "SHOT";
        case Type::TILE_LINE:   return "TILE_LINE";
        default:                return "";
        }
    }
}

export struct Color {
    U8 r = 127, g = 127, b = 127;
    bool operator ==(const Color& other) { return (r == other.r && g == other.g && b == other.b); }
    bool operator !=(const Color& other) { return !operator==(other); }
    Color& operator =(const Color& other) {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }
};
using cColor = const Color;

export template<typename T> requires std::integral<T> || std::floating_point<T>
struct Rect {
    T x = 0, y = 0, w = 0, h = 0;
    Rect() {}
    Rect(T in_x, T in_y, T in_w, T in_h) : x(in_x), y(in_y), w(in_w), h(in_h) {}
    Rect& operator =(const Rect& other) { x = other.x, y = other.y, w = other.w, h = other.h; return *this; }    
    template<typename T>
    Rect& operator *=(const T scalar) { x *= scalar, y *= scalar, w *= scalar, h *= scalar; return *this; }
    bool operator ==(const Rect& other) const { return (x == other.x && y == other.y && w == other.w && h == other.h); }
    bool operator !=(const Rect& other) const { return !operator==(other); }

    Rect operator +(const Rect& other) const { return { x + other.x, y + other.y, w + other.w, h + other.h }; }
};

export template<typename T> requires std::integral<T> || std::floating_point<T>
struct Vec2 {
    T x = 0, y = 0;
    Vec2() {}
    Vec2(T in_x, T in_y) : x(in_x), y(in_y) {}
    Vec2& operator =(const Vec2& other) { x = other.x, y = other.y;   return *this; }
    Vec2& operator =(const T value) { x = value, y = value;   return *this; }
    Vec2& operator +=(const Vec2& other) { x += other.x, y += other.y; return *this; }
    Vec2& operator -=(const Vec2& other) { x -= other.x, y -= other.y; return *this; }
    Vec2& operator *=(const Vec2& other) { x *= other.x, y *= other.y; return *this; }
    Vec2 operator +(const Vec2& other)  const { return { x + other.x, y + other.y }; }
    Vec2 operator -(const Vec2& other)  const { return { x - other.x, y - other.y }; }
    Vec2 operator -()                   const { return { -x, -y }; }
    Vec2 operator *(const Vec2& other)  const { return { x * other.x, y * other.y }; }
    template<typename T>
    Vec2 operator *(const T scalar)     const { return { x * scalar, y * scalar }; }
    template<typename T>
    Vec2 operator /(const T scalar)     const { return { x / scalar, y / scalar }; }
    bool operator ==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator !=(const Vec2& other) const { return !operator==(other); }
    template<typename T>
    bool operator ==(const T value)     const { return x == value && y == value; }
    template<typename T>
    bool operator !=(const T value)     const { return x != value && y != value; }
    bool operator <(const Vec2& other)  const { return x < other.x && y < other.y; }
};

export template<typename T> requires std::integral<T> || std::floating_point<T>
struct Vec3 {
    T x = 0, y = 0, z = 0;
    Vec3() {}
    Vec3(T in_x, T in_y, T in_z) : x(in_x), y(in_y), z(in_z) {}
    Vec3& operator  =(const Vec3& other) { x = other.x, y = other.y, z = other.z;    return *this; }
    Vec3& operator  =(const T value) { x = value, y = value, z = value;          return *this; }
    Vec3& operator +=(const Vec3& other) { x += other.x, y += other.y, z += other.z; return *this; }
    Vec3& operator -=(const Vec3& other) { x -= other.x, y -= other.y, z -= other.z; return *this; }
    Vec3& operator *=(const Vec3& other) { x *= other.x, y *= other.y, z *= other.z; return *this; }
    Vec3  operator  +(const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
    Vec3  operator  -(const Vec3& other) const { return { x - other.x, y - other.y, z - other.z }; }
    Vec3  operator  -()                  const { return { -x, -y, -z }; }
    Vec3  operator  *(const Vec3& other) const { return { x * other.x, y * other.y, z * other.z }; }
    template<typename T>
    Vec3  operator  *(const T scalar)    const { return { x * scalar, y * scalar, z * scalar }; }
    template<typename T>
    Vec3  operator  /(const T scalar)    const { return { x / scalar, y / scalar, z / scalar }; }
    bool  operator ==(const Vec3& other) const { return x == other.x && y == other.y && z == other.z; }
    bool  operator !=(const Vec3& other) const { return !operator==(other); }
    template<typename T>
    bool  operator ==(const T value)     const { return x == value && y == value && z == value; }
    template<typename T>
    bool  operator !=(const T value)     const { return !operator==(value); }
    bool  operator  <(const Vec3& other) const { return x < other.x && y < other.y && z < other.z; }
};

export using  Vec2I =       Vec2<I32>;
export using cVec2I = const Vec2<I32>;
export using  Vec2U =       Vec2<U32>;
export using cVec2U = const Vec2<U32>;
export using  Vec2F =       Vec2<F32>;
export using cVec2F = const Vec2<F32>;

export using  Vec3I =       Vec3<I32>;
export using cVec3I = const Vec3<I32>;
export using  Vec3U =       Vec3<U32>;
export using cVec3U = const Vec3<U32>;
export using  Vec3F =       Vec3<F32>;
export using cVec3F = const Vec3<F32>;

export using  RectI =       Rect<I32>;
export using cRectI = const Rect<I32>;
export using  RectU =       Rect<U32>;
export using cRectU = const Rect<U32>;
export using  RectF =       Rect<F32>;
export using cRectF = const Rect<F32>;