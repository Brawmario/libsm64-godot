#include <sm64_surface_properties.hpp>

#include <godot_cpp/core/class_db.hpp>


#define SURFACE_TYPE_HINT_STRING "Default:0,Burning:1,0004:4,Hangable:5,Slow:9,Death Plane:10,Close Camera:11,Water:13,Flowing Water:14,Intangible:18,Very Slippery:19,Slippery:20,Not Slippery:21,Ttm Vines:22,Mgr Music:26,Instant Warp 1b:27,Instant Warp 1c:28,Instant Warp 1d:29,Instant Warp 1e:30,Shallow Quicksand:33,Deep Quicksand:34,Instant Quicksand:35,Deep Moving Quicksand:36,Shallow Moving Quicksand:37,Quicksand:38,Moving Quicksand:39,Wall Misc:40,Noise Default:41,Noise Slippery:42,Horizontal Wind:44,Instant Moving Quicksand:45,Ice:46,Look Up Warp:47,Hard:48,Warp:50,Timer Start:51,Timer End:52,Hard Slippery:53,Hard Very Slippery:54,Hard Not Slippery:55,Vertical Wind:56,Boss Fight Camera:101,Camera Free Roam:102,Thi3 Wallkick:104,Camera 8 Dir:105,Camera Middle:110,Camera Rotate Right:111,Camera Rotate Left:112,Camera Boundary:114,Noise Very Slippery 73:115,Noise Very Slippery 74:116,Noise Very Slippery:117,No Cam Collision:118,No Cam Collision 77:119,No Cam Col Very Slippery:120,No Cam Col Slippery:121,Switch:122,Vanish Cap Walls:123,Painting Wobble A6:166,Painting Wobble A7:167,Painting Wobble A8:168,Painting Wobble A9:169,Painting Wobble Aa:170,Painting Wobble Ab:171,Painting Wobble Ac:172,Painting Wobble Ad:173,Painting Wobble Ae:174,Painting Wobble Af:175,Painting Wobble B0:176,Painting Wobble B1:177,Painting Wobble B2:178,Painting Wobble B3:179,Painting Wobble B4:180,Painting Wobble B5:181,Painting Wobble B6:182,Painting Wobble B7:183,Painting Wobble B8:184,Painting Wobble B9:185,Painting Wobble Ba:186,Painting Wobble Bb:187,Painting Wobble Bc:188,Painting Wobble Bd:189,Painting Wobble Be:190,Painting Wobble Bf:191,Painting Wobble C0:192,Painting Wobble C1:193,Painting Wobble C2:194,Painting Wobble C3:195,Painting Wobble C4:196,Painting Wobble C5:197,Painting Wobble C6:198,Painting Wobble C7:199,Painting Wobble C8:200,Painting Wobble C9:201,Painting Wobble Ca:202,Painting Wobble Cb:203,Painting Wobble Cc:204,Painting Wobble Cd:205,Painting Wobble Ce:206,Painting Wobble Cf:207,Painting Warp D0:208,Painting Warp D1:209,Painting Warp D2:210,Painting Warp D3:211,Painting Warp D4:212,Painting Warp D5:213,Painting Warp D6:214,Painting Warp D7:215,Painting Warp D8:216,Painting Warp D9:217,Painting Warp Da:218,Painting Warp Db:219,Painting Warp Dc:220,Painting Warp Dd:221,Painting Warp De:222,Painting Warp Df:223,Painting Warp E0:224,Painting Warp E1:225,Painting Warp E2:226,Painting Warp E3:227,Painting Warp E4:228,Painting Warp E5:229,Painting Warp E6:230,Painting Warp E7:231,Painting Warp E8:232,Painting Warp E9:233,Painting Warp Ea:234,Painting Warp Eb:235,Painting Warp Ec:236,Painting Warp Ed:237,Painting Warp Ee:238,Painting Warp Ef:239,Painting Warp F0:240,Painting Warp F1:241,Painting Warp F2:242,Painting Warp F3:243,Ttc Painting 1:244,Ttc Painting 2:245,Ttc Painting 3:246,Painting Warp F7:247,Painting Warp F8:248,Painting Warp F9:249,Painting Warp Fa:250,Painting Warp Fb:251,Painting Warp Fc:252,Wobbling Warp:253,Trapdoor:255"
#define TERRAIN_TYPE_HINT_STRING "Grass,Stone,Snow,Sand,Spooky,Water,Slide,Mask"


void SM64SurfaceProperties::set_surface_type(SurfaceType value)
{
    surface_type = value;
}

SM64SurfaceProperties::SurfaceType SM64SurfaceProperties::get_surface_type() const
{
    return surface_type;
}

void SM64SurfaceProperties::set_terrain_type(TerrainType value)
{
    terrain_type = value;
}

SM64SurfaceProperties::TerrainType SM64SurfaceProperties::get_terrain_type() const
{
    return terrain_type;
}

void SM64SurfaceProperties::set_force(int value)
{
    force = value;
}

int SM64SurfaceProperties::get_force() const
{
    return force;
}

void SM64SurfaceProperties::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_surface_type", "value"), &SM64SurfaceProperties::set_surface_type);
    godot::ClassDB::bind_method(godot::D_METHOD("get_surface_type"), &SM64SurfaceProperties::get_surface_type);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "surface_type", godot::PROPERTY_HINT_ENUM, SURFACE_TYPE_HINT_STRING), "set_surface_type", "get_surface_type");
    godot::ClassDB::bind_method(godot::D_METHOD("set_terrain_type", "value"), &SM64SurfaceProperties::set_terrain_type);
    godot::ClassDB::bind_method(godot::D_METHOD("get_terrain_type"), &SM64SurfaceProperties::get_terrain_type);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "terrain_type", godot::PROPERTY_HINT_ENUM, TERRAIN_TYPE_HINT_STRING), "set_terrain_type", "get_terrain_type");
    godot::ClassDB::bind_method(godot::D_METHOD("set_force", "value"), &SM64SurfaceProperties::set_force);
    godot::ClassDB::bind_method(godot::D_METHOD("get_force"), &SM64SurfaceProperties::get_force);
    ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "force"), "set_force", "get_force");

    BIND_ENUM_CONSTANT(SURFACE_TYPE_DEFAULT);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_BURNING);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_0004);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_HANGABLE);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_SLOW);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_DEATH_PLANE);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_CLOSE_CAMERA);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_WATER);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_FLOWING_WATER);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_INTANGIBLE);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_VERY_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_NOT_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_TTM_VINES);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_MGR_MUSIC);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_INSTANT_WARP_1B);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_INSTANT_WARP_1C);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_INSTANT_WARP_1D);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_INSTANT_WARP_1E);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_SHALLOW_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_DEEP_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_INSTANT_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_DEEP_MOVING_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_SHALLOW_MOVING_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_MOVING_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_WALL_MISC);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_NOISE_DEFAULT);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_NOISE_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_HORIZONTAL_WIND);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_INSTANT_MOVING_QUICKSAND);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_ICE);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_LOOK_UP_WARP);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_HARD);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_WARP);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_TIMER_START);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_TIMER_END);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_HARD_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_HARD_VERY_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_HARD_NOT_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_VERTICAL_WIND);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_BOSS_FIGHT_CAMERA);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_CAMERA_FREE_ROAM);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_THI3_WALLKICK);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_CAMERA_8_DIR);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_CAMERA_MIDDLE);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_CAMERA_ROTATE_RIGHT);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_CAMERA_ROTATE_LEFT);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_CAMERA_BOUNDARY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_NOISE_VERY_SLIPPERY_73);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_NOISE_VERY_SLIPPERY_74);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_NOISE_VERY_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_NO_CAM_COLLISION);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_NO_CAM_COLLISION_77);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_NO_CAM_COL_VERY_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_NO_CAM_COL_SLIPPERY);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_SWITCH);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_VANISH_CAP_WALLS);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_A6);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_A7);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_A8);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_A9);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_AA);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_AB);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_AC);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_AD);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_AE);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_AF);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_B0);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_B1);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_B2);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_B3);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_B4);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_B5);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_B6);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_B7);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_B8);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_B9);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_BA);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_BB);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_BC);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_BD);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_BE);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_BF);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_C0);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_C1);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_C2);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_C3);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_C4);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_C5);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_C6);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_C7);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_C8);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_C9);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_CA);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_CB);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_CC);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_CD);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_CE);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_CF);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_D0);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_D1);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WOBBLE_D2);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_D3);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_D4);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_D5);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_D6);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_D7);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_D8);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_D9);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_DA);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_DB);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_DC);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_DD);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_DE);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_DF);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_E0);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_E1);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_E2);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_E3);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_E4);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_E5);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_E6);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_E7);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_E8);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_E9);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_EA);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_EB);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_EC);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_ED);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_EE);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_EF);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_F0);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_F1);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_F2);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_F3);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_TTC_PAINTING_1);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_TTC_PAINTING_2);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_TTC_PAINTING_3);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_F7);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_F8);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_F9);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_FA);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_FB);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_PAINTING_WARP_FC);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_WOBBLING_WARP);
    BIND_ENUM_CONSTANT(SURFACE_TYPE_TRAPDOOR);

    BIND_ENUM_CONSTANT(TERRAIN_TYPE_GRASS);
    BIND_ENUM_CONSTANT(TERRAIN_TYPE_STONE);
    BIND_ENUM_CONSTANT(TERRAIN_TYPE_SNOW);
    BIND_ENUM_CONSTANT(TERRAIN_TYPE_SAND);
    BIND_ENUM_CONSTANT(TERRAIN_TYPE_SPOOKY);
    BIND_ENUM_CONSTANT(TERRAIN_TYPE_WATER);
    BIND_ENUM_CONSTANT(TERRAIN_TYPE_SLIDE);
    BIND_ENUM_CONSTANT(TERRAIN_TYPE_MASK);
}
