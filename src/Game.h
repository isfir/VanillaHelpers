// This file is part of VanillaHelpers.
//
// VanillaHelpers is free software: you can redistribute it and/or modify it under the terms of the
// GNU Lesser General Public License as published by the Free Software Foundation, either version 3
// of the License, or (at your option) any later version.
//
// VanillaHelpers is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lessed General Public License along with
// VanillaHelpers. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>

namespace Game {

struct HTEXTURE__; // Don't know real struct
struct CGxTex;     // Don't know real struct
struct CWorld;     // don't know real struct
struct SFile;      // don't know real struct

enum TYPE_MASK {
    TYPEMASK_OBJECT = 0x1,
    TYPEMASK_ITEM = 0x2,
    TYPEMASK_CONTAINER = 0x4,
    TYPEMASK_UNIT = 0x8,
    TYPEMASK_PLAYER = 0x10,
    TYPEMASK_GAMEOBJECT = 0x20,
    TYPEMASK_DYNAMICOBJECT = 0x40,
    TYPEMASK_CORPSE = 0x80,
};

enum EGxRenderState { GxRs_Texture0 = 23 };

enum EGxPrim {
    GxPrim_Points = 0,
    GxPrim_Lines = 1,
    GxPrim_LineStrip = 2,
    GxPrim_Triangles = 3,
    GxPrim_TriangleStrip = 4,
    GxPrim_TriangleFan = 5,
    GxPrims_Last = 6
};

enum EGxTexFilter {
    GxTex_Nearest = 0,
    GxTex_Linear = 1,
    GxTex_LinearMipNearest = 2,
    GxTex_LinearMipLinear = 3,
    GxTex_Anisotropic = 4,
    GxTexFilters_Last = 5
};

enum STATUS_TYPE : int32_t {
    STATUS_INFO = 0,
    STATUS_WARNING = 1,
    STATUS_ERROR = 2,
    STATUS_FATAL = 3,
    STATUS_NUMTYPES = 4
};

enum OBJECT_TYPE {
    OBJECT = 0,
    ITEM = 1,
    CONTAINER = 2,
    UNIT = 3,
    PLAYER = 4,
    GAMEOBJECT = 5,
    DYNAMICOBJECT = 6,
    CORPSE = 7
};

enum NPC_FLAGS {
    UNIT_NPC_FLAG_NONE = 0x00000000,
    UNIT_NPC_FLAG_GOSSIP = 0x00000001,
    UNIT_NPC_FLAG_QUESTGIVER = 0x00000002,
    UNIT_NPC_FLAG_VENDOR = 0x00000004,
    UNIT_NPC_FLAG_FLIGHTMASTER = 0x00000008,
    UNIT_NPC_FLAG_TRAINER = 0x00000010,
    UNIT_NPC_FLAG_SPIRITHEALER = 0x00000020,
    UNIT_NPC_FLAG_SPIRITGUIDE = 0x00000040,
    UNIT_NPC_FLAG_INNKEEPER = 0x00000080,
    UNIT_NPC_FLAG_BANKER = 0x00000100,
    UNIT_NPC_FLAG_PETITIONER = 0x00000200,
    UNIT_NPC_FLAG_TABARDDESIGNER = 0x00000400,
    UNIT_NPC_FLAG_BATTLEMASTER = 0x00000800,
    UNIT_NPC_FLAG_AUCTIONEER = 0x00001000,
    UNIT_NPC_FLAG_STABLEMASTER = 0x00002000,
    UNIT_NPC_FLAG_REPAIR = 0x00004000,
    UNIT_NPC_FLAG_SUMMONING_RITUAL = 0x00008000,
};

enum GAMEOBJECT_TYPES {
    GAMEOBJECT_TYPE_DOOR = 0,
    GAMEOBJECT_TYPE_BUTTON = 1,
    GAMEOBJECT_TYPE_QUESTGIVER = 2,
    GAMEOBJECT_TYPE_CHEST = 3,
    GAMEOBJECT_TYPE_BINDER = 4,
    GAMEOBJECT_TYPE_GENERIC = 5,
    GAMEOBJECT_TYPE_TRAP = 6,
    GAMEOBJECT_TYPE_CHAIR = 7,
    GAMEOBJECT_TYPE_SPELL_FOCUS = 8,
    GAMEOBJECT_TYPE_TEXT = 9,
    GAMEOBJECT_TYPE_GOOBER = 10,
    GAMEOBJECT_TYPE_TRANSPORT = 11,
    GAMEOBJECT_TYPE_AREADAMAGE = 12,
    GAMEOBJECT_TYPE_CAMERA = 13,
    GAMEOBJECT_TYPE_MAP_OBJECT = 14,
    GAMEOBJECT_TYPE_MO_TRANSPORT = 15,
    GAMEOBJECT_TYPE_DUEL_ARBITER = 16,
    GAMEOBJECT_TYPE_FISHINGNODE = 17,
    GAMEOBJECT_TYPE_SUMMONING_RITUAL = 18,
    GAMEOBJECT_TYPE_MAILBOX = 19,
    GAMEOBJECT_TYPE_AUCTIONHOUSE = 20,
    GAMEOBJECT_TYPE_GUARDPOST = 21,
    GAMEOBJECT_TYPE_SPELLCASTER = 22,
    GAMEOBJECT_TYPE_MEETINGSTONE = 23,
    GAMEOBJECT_TYPE_FLAGSTAND = 24,
    GAMEOBJECT_TYPE_FISHINGHOLE = 25,
    GAMEOBJECT_TYPE_FLAGDROP = 26,
    GAMEOBJECT_TYPE_MINI_GAME = 27,
    GAMEOBJECT_TYPE_LOTTERY_KIOSK = 28,
    GAMEOBJECT_TYPE_CAPTURE_POINT = 29,
    GAMEOBJECT_TYPE_AURA_GENERATOR = 30,
};

enum CVAR_CATEGORY {
    DEBUG = 0,
    GRAPHICS = 1,
    CONSOLE = 2,
    COMBAT = 3,
    GAME = 4,
    DEFAULT = 5,
    NET = 6,
    SOUND = 7,
    GM = 8,
    NONE = 9,
    LAST = 10
};

enum EObjectFields {
    OBJECT_FIELD_GUID = 0x0,    // 0x000 - Size: 2 - Type: GUID
    OBJECT_FIELD_TYPE = 0x2,    // 0x002 - Size: 1 - Type: INT
    OBJECT_FIELD_ENTRY = 0x3,   // 0x003 - Size: 1 - Type: INT
    OBJECT_FIELD_SCALE_X = 0x4, // 0x004 - Size: 1 - Type: FLOAT
    OBJECT_FIELD_PADDING = 0x5, // 0x005 - Size: 1 - Type: INT
    OBJECT_END = 0x6
};

enum EItemFields {
    ITEM_FIELD_OWNER = OBJECT_END + 0x0,                 // 0x006 - Size: 2 - Type: GUID
    ITEM_FIELD_CONTAINED = OBJECT_END + 0x2,             // 0x008 - Size: 2 - Type: GUID
    ITEM_FIELD_CREATOR = OBJECT_END + 0x4,               // 0x00A - Size: 2 - Type: GUID
    ITEM_FIELD_GIFTCREATOR = OBJECT_END + 0x6,           // 0x00C - Size: 2 - Type: GUID
    ITEM_FIELD_STACK_COUNT = OBJECT_END + 0x8,           // 0x00E - Size: 1 - Type: INT
    ITEM_FIELD_DURATION = OBJECT_END + 0x9,              // 0x00F - Size: 1 - Type: INT
    ITEM_FIELD_SPELL_CHARGES = OBJECT_END + 0xA,         // 0x010 - Size: 5 - Type: INT
    ITEM_FIELD_FLAGS = OBJECT_END + 0xF,                 // 0x015 - Size: 1 - Type: INT
    ITEM_FIELD_ENCHANTMENT = OBJECT_END + 0x10,          // 0x016 - Size: 21 - Type: INT
    ITEM_FIELD_PROPERTY_SEED = OBJECT_END + 0x25,        // 0x02B - Size: 1 - Type: INT
    ITEM_FIELD_RANDOM_PROPERTIES_ID = OBJECT_END + 0x26, // 0x02C - Size: 1 - Type: INT
    ITEM_FIELD_ITEM_TEXT_ID = OBJECT_END + 0x27,         // 0x02D - Size: 1 - Type: INT
    ITEM_FIELD_DURABILITY = OBJECT_END + 0x28,           // 0x02E - Size: 1 - Type: INT
    ITEM_FIELD_MAXDURABILITY = OBJECT_END + 0x29,        // 0x02F - Size: 1 - Type: INT
    ITEM_END = OBJECT_END + 0x2A                         // 0x030
};

enum EContainerFields {
    CONTAINER_FIELD_NUM_SLOTS = ITEM_END + 0x0, // 0x02A - Size: 1 - Type: INT
    CONTAINER_ALIGN_PAD = ITEM_END + 0x1,       // 0x02B - Size: 1 - Type: BYTES
    CONTAINER_FIELD_SLOT_1 = ITEM_END + 0x2,    // 0x02C - Size: 72 - Type: GUID
    CONTAINER_END = ITEM_END + 0x4A             // 0x074
};

enum EUnitFields {
    UNIT_FIELD_CHARM = OBJECT_END + 0x0,                     // 0x006 - Size: 2 - Type: GUID
    UNIT_FIELD_SUMMON = OBJECT_END + 0x2,                    // 0x008 - Size: 2 - Type: GUID
    UNIT_FIELD_CHARMEDBY = OBJECT_END + 0x4,                 // 0x00A - Size: 2 - Type: GUID
    UNIT_FIELD_SUMMONEDBY = OBJECT_END + 0x6,                // 0x00C - Size: 2 - Type: GUID
    UNIT_FIELD_CREATEDBY = OBJECT_END + 0x8,                 // 0x00E - Size: 2 - Type: GUID
    UNIT_FIELD_TARGET = OBJECT_END + 0xA,                    // 0x010 - Size: 2 - Type: GUID
    UNIT_FIELD_PERSUADED = OBJECT_END + 0xC,                 // 0x012 - Size: 2 - Type: GUID
    UNIT_FIELD_CHANNEL_OBJECT = OBJECT_END + 0xE,            // 0x014 - Size: 2 - Type: GUID
    UNIT_FIELD_HEALTH = OBJECT_END + 0x10,                   // 0x016 - Size: 1 - Type: INT
    UNIT_FIELD_POWER1 = OBJECT_END + 0x11,                   // 0x017 - Size: 1 - Type: INT
    UNIT_FIELD_POWER2 = OBJECT_END + 0x12,                   // 0x018 - Size: 1 - Type: INT
    UNIT_FIELD_POWER3 = OBJECT_END + 0x13,                   // 0x019 - Size: 1 - Type: INT
    UNIT_FIELD_POWER4 = OBJECT_END + 0x14,                   // 0x01A - Size: 1 - Type: INT
    UNIT_FIELD_POWER5 = OBJECT_END + 0x15,                   // 0x01B - Size: 1 - Type: INT
    UNIT_FIELD_MAXHEALTH = OBJECT_END + 0x16,                // 0x01C - Size: 1 - Type: INT
    UNIT_FIELD_MAXPOWER1 = OBJECT_END + 0x17,                // 0x01D - Size: 1 - Type: INT
    UNIT_FIELD_MAXPOWER2 = OBJECT_END + 0x18,                // 0x01E - Size: 1 - Type: INT
    UNIT_FIELD_MAXPOWER3 = OBJECT_END + 0x19,                // 0x01F - Size: 1 - Type: INT
    UNIT_FIELD_MAXPOWER4 = OBJECT_END + 0x1A,                // 0x020 - Size: 1 - Type: INT
    UNIT_FIELD_MAXPOWER5 = OBJECT_END + 0x1B,                // 0x021 - Size: 1 - Type: INT
    UNIT_FIELD_LEVEL = OBJECT_END + 0x1C,                    // 0x022 - Size: 1 - Type: INT
    UNIT_FIELD_FACTIONTEMPLATE = OBJECT_END + 0x1D,          // 0x023 - Size: 1 - Type: INT
    UNIT_FIELD_BYTES_0 = OBJECT_END + 0x1E,                  // 0x024 - Size: 1 - Type: BYTES
    UNIT_VIRTUAL_ITEM_SLOT_DISPLAY = OBJECT_END + 0x1F,      // 0x025 - Size: 3 - Type: INT
    UNIT_VIRTUAL_ITEM_INFO = OBJECT_END + 0x22,              // 0x028 - Size: 6 - Type: BYTES
    UNIT_FIELD_FLAGS = OBJECT_END + 0x28,                    // 0x02E - Size: 1 - Type: INT
    UNIT_FIELD_AURA = OBJECT_END + 0x29,                     // 0x02F - Size: 48 - Type: INT
    UNIT_FIELD_AURAFLAGS = OBJECT_END + 0x59,                // 0x05F - Size: 6 - Type: BYTES
    UNIT_FIELD_AURALEVELS = OBJECT_END + 0x5F,               // 0x065 - Size: 12 - Type: BYTES
    UNIT_FIELD_AURAAPPLICATIONS = OBJECT_END + 0x6B,         // 0x071 - Size: 12 - Type: BYTES
    UNIT_FIELD_AURASTATE = OBJECT_END + 0x77,                // 0x07D - Size: 1 - Type: INT
    UNIT_FIELD_BASEATTACKTIME = OBJECT_END + 0x78,           // 0x07E - Size: 2 - Type: INT
    UNIT_FIELD_RANGEDATTACKTIME = OBJECT_END + 0x7A,         // 0x080 - Size: 1 - Type: INT
    UNIT_FIELD_BOUNDINGRADIUS = OBJECT_END + 0x7B,           // 0x081 - Size: 1 - Type: FLOAT
    UNIT_FIELD_COMBATREACH = OBJECT_END + 0x7C,              // 0x082 - Size: 1 - Type: FLOAT
    UNIT_FIELD_DISPLAYID = OBJECT_END + 0x7D,                // 0x083 - Size: 1 - Type: INT
    UNIT_FIELD_NATIVEDISPLAYID = OBJECT_END + 0x7E,          // 0x084 - Size: 1 - Type: INT
    UNIT_FIELD_MOUNTDISPLAYID = OBJECT_END + 0x7F,           // 0x085 - Size: 1 - Type: INT
    UNIT_FIELD_MINDAMAGE = OBJECT_END + 0x80,                // 0x086 - Size: 1 - Type: FLOAT
    UNIT_FIELD_MAXDAMAGE = OBJECT_END + 0x81,                // 0x087 - Size: 1 - Type: FLOAT
    UNIT_FIELD_MINOFFHANDDAMAGE = OBJECT_END + 0x82,         // 0x088 - Size: 1 - Type: FLOAT
    UNIT_FIELD_MAXOFFHANDDAMAGE = OBJECT_END + 0x83,         // 0x089 - Size: 1 - Type: FLOAT
    UNIT_FIELD_BYTES_1 = OBJECT_END + 0x84,                  // 0x08A - Size: 1 - Type: BYTES
    UNIT_FIELD_PETNUMBER = OBJECT_END + 0x85,                // 0x08B - Size: 1 - Type: INT
    UNIT_FIELD_PET_NAME_TIMESTAMP = OBJECT_END + 0x86,       // 0x08C - Size: 1 - Type: INT
    UNIT_FIELD_PETEXPERIENCE = OBJECT_END + 0x87,            // 0x08D - Size: 1 - Type: INT
    UNIT_FIELD_PETNEXTLEVELEXP = OBJECT_END + 0x88,          // 0x08E - Size: 1 - Type: INT
    UNIT_DYNAMIC_FLAGS = OBJECT_END + 0x89,                  // 0x08F - Size: 1 - Type: INT
    UNIT_CHANNEL_SPELL = OBJECT_END + 0x8A,                  // 0x090 - Size: 1 - Type: INT
    UNIT_MOD_CAST_SPEED = OBJECT_END + 0x8B,                 // 0x091 - Size: 1 - Type: FLOAT
    UNIT_CREATED_BY_SPELL = OBJECT_END + 0x8C,               // 0x092 - Size: 1 - Type: INT
    UNIT_NPC_FLAGS = OBJECT_END + 0x8D,                      // 0x093 - Size: 1 - Type: INT
    UNIT_NPC_EMOTESTATE = OBJECT_END + 0x8E,                 // 0x094 - Size: 1 - Type: INT
    UNIT_TRAINING_POINTS = OBJECT_END + 0x8F,                // 0x095 - Size: 1 - Type: TWO_SHORT
    UNIT_FIELD_STAT0 = OBJECT_END + 0x90,                    // 0x096 - Size: 1 - Type: INT
    UNIT_FIELD_STAT1 = OBJECT_END + 0x91,                    // 0x097 - Size: 1 - Type: INT
    UNIT_FIELD_STAT2 = OBJECT_END + 0x92,                    // 0x098 - Size: 1 - Type: INT
    UNIT_FIELD_STAT3 = OBJECT_END + 0x93,                    // 0x099 - Size: 1 - Type: INT
    UNIT_FIELD_STAT4 = OBJECT_END + 0x94,                    // 0x09A - Size: 1 - Type: INT
    UNIT_FIELD_RESISTANCES = OBJECT_END + 0x95,              // 0x09B - Size: 7 - Type: INT
    UNIT_FIELD_BASE_MANA = OBJECT_END + 0x9C,                // 0x0A2 - Size: 1 - Type: INT
    UNIT_FIELD_BASE_HEALTH = OBJECT_END + 0x9D,              // 0x0A3 - Size: 1 - Type: INT
    UNIT_FIELD_BYTES_2 = OBJECT_END + 0x9E,                  // 0x0A4 - Size: 1 - Type: BYTES
    UNIT_FIELD_ATTACK_POWER = OBJECT_END + 0x9F,             // 0x0A5 - Size: 1 - Type: INT
    UNIT_FIELD_ATTACK_POWER_MODS = OBJECT_END + 0xA0,        // 0x0A6 - Size: 1 - Type: TWO_SHORT
    UNIT_FIELD_ATTACK_POWER_MULTIPLIER = OBJECT_END + 0xA1,  // 0x0A7 - Size: 1 - Type: FLOAT
    UNIT_FIELD_RANGED_ATTACK_POWER = OBJECT_END + 0xA2,      // 0x0A8 - Size: 1 - Type: INT
    UNIT_FIELD_RANGED_ATTACK_POWER_MODS = OBJECT_END + 0xA3, // 0x0A9 - Size: 1 - Type: TWO_SHORT
    UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = OBJECT_END + 0xA4, // 0x0AA - Size: 1 - Type: FLOAT
    UNIT_FIELD_MINRANGEDDAMAGE = OBJECT_END + 0xA5,                // 0x0AB - Size: 1 - Type: FLOAT
    UNIT_FIELD_MAXRANGEDDAMAGE = OBJECT_END + 0xA6,                // 0x0AC - Size: 1 - Type: FLOAT
    UNIT_FIELD_POWER_COST_MODIFIER = OBJECT_END + 0xA7,            // 0x0AD - Size: 7 - Type: INT
    UNIT_FIELD_POWER_COST_MULTIPLIER = OBJECT_END + 0xAE,          // 0x0B4 - Size: 7 - Type: FLOAT
    UNIT_FIELD_PADDING = OBJECT_END + 0xB5,                        // 0x0BB - Size: 1 - Type: INT
    UNIT_END = OBJECT_END + 0xB6                                   // 0x0BC
};

enum EPlayerFields {
    PLAYER_DUEL_ARBITER = UNIT_END + 0x0,                 // 0x0B6 - Size: 2 - Type: GUID
    PLAYER_FLAGS = UNIT_END + 0x2,                        // 0x0B8 - Size: 1 - Type: INT
    PLAYER_GUILDID = UNIT_END + 0x3,                      // 0x0B9 - Size: 1 - Type: INT
    PLAYER_GUILDRANK = UNIT_END + 0x4,                    // 0x0BA - Size: 1 - Type: INT
    PLAYER_BYTES = UNIT_END + 0x5,                        // 0x0BB - Size: 1 - Type: BYTES
    PLAYER_BYTES_2 = UNIT_END + 0x6,                      // 0x0BC - Size: 1 - Type: BYTES
    PLAYER_BYTES_3 = UNIT_END + 0x7,                      // 0x0BD - Size: 1 - Type: BYTES
    PLAYER_DUEL_TEAM = UNIT_END + 0x8,                    // 0x0BE - Size: 1 - Type: INT
    PLAYER_GUILD_TIMESTAMP = UNIT_END + 0x9,              // 0x0BF - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_1_1 = UNIT_END + 0xA,                // 0x0C0 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_1_2 = UNIT_END + 0xB,                // 0x0C1 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_2_1 = UNIT_END + 0xD,                // 0x0C3 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_2_2 = UNIT_END + 0xE,                // 0x0C4 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_3_1 = UNIT_END + 0x10,               // 0x0C6 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_3_2 = UNIT_END + 0x11,               // 0x0C7 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_4_1 = UNIT_END + 0x13,               // 0x0C9 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_4_2 = UNIT_END + 0x14,               // 0x0CA - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_5_1 = UNIT_END + 0x16,               // 0x0CC - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_5_2 = UNIT_END + 0x17,               // 0x0CD - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_6_1 = UNIT_END + 0x19,               // 0x0CF - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_6_2 = UNIT_END + 0x1A,               // 0x0D0 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_7_1 = UNIT_END + 0x1C,               // 0x0D2 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_7_2 = UNIT_END + 0x1D,               // 0x0D3 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_8_1 = UNIT_END + 0x1F,               // 0x0D5 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_8_2 = UNIT_END + 0x20,               // 0x0D6 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_9_1 = UNIT_END + 0x22,               // 0x0D8 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_9_2 = UNIT_END + 0x23,               // 0x0D9 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_10_1 = UNIT_END + 0x25,              // 0x0DB - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_10_2 = UNIT_END + 0x26,              // 0x0DC - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_11_1 = UNIT_END + 0x28,              // 0x0DE - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_11_2 = UNIT_END + 0x29,              // 0x0DF - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_12_1 = UNIT_END + 0x2B,              // 0x0E1 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_12_2 = UNIT_END + 0x2C,              // 0x0E2 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_13_1 = UNIT_END + 0x2E,              // 0x0E4 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_13_2 = UNIT_END + 0x2F,              // 0x0E5 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_14_1 = UNIT_END + 0x31,              // 0x0E7 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_14_2 = UNIT_END + 0x32,              // 0x0E8 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_15_1 = UNIT_END + 0x34,              // 0x0EA - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_15_2 = UNIT_END + 0x35,              // 0x0EB - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_16_1 = UNIT_END + 0x37,              // 0x0ED - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_16_2 = UNIT_END + 0x38,              // 0x0EE - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_17_1 = UNIT_END + 0x3A,              // 0x0F0 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_17_2 = UNIT_END + 0x3B,              // 0x0F1 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_18_1 = UNIT_END + 0x3D,              // 0x0F3 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_18_2 = UNIT_END + 0x3E,              // 0x0F4 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_19_1 = UNIT_END + 0x40,              // 0x0F6 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_19_2 = UNIT_END + 0x41,              // 0x0F7 - Size: 2 - Type: INT
    PLAYER_QUEST_LOG_20_1 = UNIT_END + 0x43,              // 0x0F9 - Size: 1 - Type: INT
    PLAYER_QUEST_LOG_20_2 = UNIT_END + 0x44,              // 0x0FA - Size: 2 - Type: INT
    PLAYER_VISIBLE_ITEM_1_CREATOR = UNIT_END + 0x46,      // 0x0FC - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_1_0 = UNIT_END + 0x48,            // 0x0FE - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_1_PROPERTIES = UNIT_END + 0x50,   // 0x106 - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_1_PAD = UNIT_END + 0x51,          // 0x107 - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_2_CREATOR = UNIT_END + 0x52,      // 0x108 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_2_0 = UNIT_END + 0x54,            // 0x10A - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_2_PROPERTIES = UNIT_END + 0x5C,   // 0x112 - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_2_PAD = UNIT_END + 0x5D,          // 0x113 - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_3_CREATOR = UNIT_END + 0x5E,      // 0x114 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_3_0 = UNIT_END + 0x60,            // 0x116 - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_3_PROPERTIES = UNIT_END + 0x68,   // 0x11E - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_3_PAD = UNIT_END + 0x69,          // 0x11F - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_4_CREATOR = UNIT_END + 0x6A,      // 0x120 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_4_0 = UNIT_END + 0x6C,            // 0x122 - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_4_PROPERTIES = UNIT_END + 0x74,   // 0x12A - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_4_PAD = UNIT_END + 0x75,          // 0x12B - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_5_CREATOR = UNIT_END + 0x76,      // 0x12C - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_5_0 = UNIT_END + 0x78,            // 0x12E - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_5_PROPERTIES = UNIT_END + 0x80,   // 0x136 - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_5_PAD = UNIT_END + 0x81,          // 0x137 - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_6_CREATOR = UNIT_END + 0x82,      // 0x138 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_6_0 = UNIT_END + 0x84,            // 0x13A - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_6_PROPERTIES = UNIT_END + 0x8C,   // 0x142 - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_6_PAD = UNIT_END + 0x8D,          // 0x143 - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_7_CREATOR = UNIT_END + 0x8E,      // 0x144 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_7_0 = UNIT_END + 0x90,            // 0x146 - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_7_PROPERTIES = UNIT_END + 0x98,   // 0x14E - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_7_PAD = UNIT_END + 0x99,          // 0x14F - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_8_CREATOR = UNIT_END + 0x9A,      // 0x150 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_8_0 = UNIT_END + 0x9C,            // 0x152 - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_8_PROPERTIES = UNIT_END + 0xA4,   // 0x15A - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_8_PAD = UNIT_END + 0xA5,          // 0x15B - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_9_CREATOR = UNIT_END + 0xA6,      // 0x15C - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_9_0 = UNIT_END + 0xA8,            // 0x15E - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_9_PROPERTIES = UNIT_END + 0xB0,   // 0x166 - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_9_PAD = UNIT_END + 0xB1,          // 0x167 - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_10_CREATOR = UNIT_END + 0xB2,     // 0x168 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_10_0 = UNIT_END + 0xB4,           // 0x16A - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_10_PROPERTIES = UNIT_END + 0xBC,  // 0x172 - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_10_PAD = UNIT_END + 0xBD,         // 0x173 - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_11_CREATOR = UNIT_END + 0xBE,     // 0x174 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_11_0 = UNIT_END + 0xC0,           // 0x176 - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_11_PROPERTIES = UNIT_END + 0xC8,  // 0x17E - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_11_PAD = UNIT_END + 0xC9,         // 0x17F - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_12_CREATOR = UNIT_END + 0xCA,     // 0x180 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_12_0 = UNIT_END + 0xCC,           // 0x182 - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_12_PROPERTIES = UNIT_END + 0xD4,  // 0x18A - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_12_PAD = UNIT_END + 0xD5,         // 0x18B - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_13_CREATOR = UNIT_END + 0xD6,     // 0x18C - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_13_0 = UNIT_END + 0xD8,           // 0x18E - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_13_PROPERTIES = UNIT_END + 0xE0,  // 0x196 - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_13_PAD = UNIT_END + 0xE1,         // 0x197 - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_14_CREATOR = UNIT_END + 0xE2,     // 0x198 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_14_0 = UNIT_END + 0xE4,           // 0x19A - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_14_PROPERTIES = UNIT_END + 0xEC,  // 0x1A2 - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_14_PAD = UNIT_END + 0xED,         // 0x1A3 - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_15_CREATOR = UNIT_END + 0xEE,     // 0x1A4 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_15_0 = UNIT_END + 0xF0,           // 0x1A6 - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_15_PROPERTIES = UNIT_END + 0xF8,  // 0x1AE - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_15_PAD = UNIT_END + 0xF9,         // 0x1AF - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_16_CREATOR = UNIT_END + 0xFA,     // 0x1B0 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_16_0 = UNIT_END + 0xFC,           // 0x1B2 - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_16_PROPERTIES = UNIT_END + 0x104, // 0x1BA - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_16_PAD = UNIT_END + 0x105,        // 0x1BB - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_17_CREATOR = UNIT_END + 0x106,    // 0x1BC - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_17_0 = UNIT_END + 0x108,          // 0x1BE - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_17_PROPERTIES = UNIT_END + 0x110, // 0x1C6 - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_17_PAD = UNIT_END + 0x111,        // 0x1C7 - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_18_CREATOR = UNIT_END + 0x112,    // 0x1C8 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_18_0 = UNIT_END + 0x114,          // 0x1CA - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_18_PROPERTIES = UNIT_END + 0x11C, // 0x1D2 - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_18_PAD = UNIT_END + 0x11D,        // 0x1D3 - Size: 1 - Type: INT
    PLAYER_VISIBLE_ITEM_19_CREATOR = UNIT_END + 0x11E,    // 0x1D4 - Size: 2 - Type: GUID
    PLAYER_VISIBLE_ITEM_19_0 = UNIT_END + 0x120,          // 0x1D6 - Size: 8 - Type: INT
    PLAYER_VISIBLE_ITEM_19_PROPERTIES = UNIT_END + 0x128, // 0x1DE - Size: 1 - Type: TWO_SHORT
    PLAYER_VISIBLE_ITEM_19_PAD = UNIT_END + 0x129,        // 0x1DF - Size: 1 - Type: INT
    PLAYER_FIELD_INV_SLOT_HEAD = UNIT_END + 0x12A,        // 0x1E0 - Size: 46 - Type: GUID
    PLAYER_FIELD_PACK_SLOT_1 = UNIT_END + 0x158,          // 0x20E - Size: 32 - Type: GUID
    PLAYER_FIELD_BANK_SLOT_1 = UNIT_END + 0x178,          // 0x22E - Size: 48 - Type: GUID
    PLAYER_FIELD_BANKBAG_SLOT_1 = UNIT_END + 0x1A8,       // 0x25E - Size: 12 - Type: GUID
    PLAYER_FIELD_VENDORBUYBACK_SLOT_1 = UNIT_END + 0x1B4, // 0x26A - Size: 24 - Type: GUID
    PLAYER_FIELD_KEYRING_SLOT_1 = UNIT_END + 0x1CC,       // 0x282 - Size: 64 - Type: GUID
    PLAYER_FARSIGHT = UNIT_END + 0x20C,                   // 0x2C2 - Size: 2 - Type: GUID
    PLAYER_FIELD_COMBO_TARGET = UNIT_END + 0x20E,         // 0x2C4 - Size: 2 - Type: GUID
    PLAYER_XP = UNIT_END + 0x210,                         // 0x2C6 - Size: 1 - Type: INT
    PLAYER_NEXT_LEVEL_XP = UNIT_END + 0x211,              // 0x2C7 - Size: 1 - Type: INT
    PLAYER_SKILL_INFO_1_1 = UNIT_END + 0x212,             // 0x2C8 - Size: 384 - Type: TWO_SHORT
    PLAYER_CHARACTER_POINTS1 = UNIT_END + 0x392,          // 0x448 - Size: 1 - Type: INT
    PLAYER_CHARACTER_POINTS2 = UNIT_END + 0x393,          // 0x449 - Size: 1 - Type: INT
    PLAYER_TRACK_CREATURES = UNIT_END + 0x394,            // 0x44A - Size: 1 - Type: INT
    PLAYER_TRACK_RESOURCES = UNIT_END + 0x395,            // 0x44B - Size: 1 - Type: INT
    PLAYER_BLOCK_PERCENTAGE = UNIT_END + 0x396,           // 0x44C - Size: 1 - Type: FLOAT
    PLAYER_DODGE_PERCENTAGE = UNIT_END + 0x397,           // 0x44D - Size: 1 - Type: FLOAT
    PLAYER_PARRY_PERCENTAGE = UNIT_END + 0x398,           // 0x44E - Size: 1 - Type: FLOAT
    PLAYER_CRIT_PERCENTAGE = UNIT_END + 0x399,            // 0x44F - Size: 1 - Type: FLOAT
    PLAYER_RANGED_CRIT_PERCENTAGE = UNIT_END + 0x39A,     // 0x450 - Size: 1 - Type: FLOAT
    PLAYER_EXPLORED_ZONES_1 = UNIT_END + 0x39B,           // 0x451 - Size: 64 - Type: BYTES
    PLAYER_REST_STATE_EXPERIENCE = UNIT_END + 0x3DB,      // 0x491 - Size: 1 - Type: INT
    PLAYER_FIELD_COINAGE = UNIT_END + 0x3DC,              // 0x492 - Size: 1 - Type: INT
    PLAYER_FIELD_POSSTAT0 = UNIT_END + 0x3DD,             // 0x493 - Size: 1 - Type: INT
    PLAYER_FIELD_POSSTAT1 = UNIT_END + 0x3DE,             // 0x494 - Size: 1 - Type: INT
    PLAYER_FIELD_POSSTAT2 = UNIT_END + 0x3DF,             // 0x495 - Size: 1 - Type: INT
    PLAYER_FIELD_POSSTAT3 = UNIT_END + 0x3E0,             // 0x496 - Size: 1 - Type: INT
    PLAYER_FIELD_POSSTAT4 = UNIT_END + 0x3E1,             // 0x497 - Size: 1 - Type: INT
    PLAYER_FIELD_NEGSTAT0 = UNIT_END + 0x3E2,             // 0x498 - Size: 1 - Type: INT
    PLAYER_FIELD_NEGSTAT1 = UNIT_END + 0x3E3,             // 0x499 - Size: 1 - Type: INT
    PLAYER_FIELD_NEGSTAT2 = UNIT_END + 0x3E4,             // 0x49A - Size: 1 - Type: INT
    PLAYER_FIELD_NEGSTAT3 = UNIT_END + 0x3E5,             // 0x49B - Size: 1 - Type: INT
    PLAYER_FIELD_NEGSTAT4 = UNIT_END + 0x3E6,             // 0x49C - Size: 1 - Type: INT
    PLAYER_FIELD_RESISTANCEBUFFMODSPOSITIVE = UNIT_END + 0x3E7, // 0x49D - Size: 7 - Type: INT
    PLAYER_FIELD_RESISTANCEBUFFMODSNEGATIVE = UNIT_END + 0x3EE, // 0x4A4 - Size: 7 - Type: INT
    PLAYER_FIELD_MOD_DAMAGE_DONE_POS = UNIT_END + 0x3F5,        // 0x4AB - Size: 7 - Type: INT
    PLAYER_FIELD_MOD_DAMAGE_DONE_NEG = UNIT_END + 0x3FC,        // 0x4B2 - Size: 7 - Type: INT
    PLAYER_FIELD_MOD_DAMAGE_DONE_PCT = UNIT_END + 0x403,        // 0x4B9 - Size: 7 - Type: INT
    PLAYER_FIELD_BYTES = UNIT_END + 0x40A,                      // 0x4C0 - Size: 1 - Type: BYTES
    PLAYER_AMMO_ID = UNIT_END + 0x40B,                          // 0x4C1 - Size: 1 - Type: INT
    PLAYER_SELF_RES_SPELL = UNIT_END + 0x40C,                   // 0x4C2 - Size: 1 - Type: INT
    PLAYER_FIELD_PVP_MEDALS = UNIT_END + 0x40D,                 // 0x4C3 - Size: 1 - Type: INT
    PLAYER_FIELD_BUYBACK_PRICE_1 = UNIT_END + 0x40E,            // 0x4C4 - Size: 12 - Type: INT
    PLAYER_FIELD_BUYBACK_TIMESTAMP_1 = UNIT_END + 0x41A,        // 0x4D0 - Size: 12 - Type: INT
    PLAYER_FIELD_SESSION_KILLS = UNIT_END + 0x426,              // 0x4DC - Size: 1 - Type: TWO_SHORT
    PLAYER_FIELD_YESTERDAY_KILLS = UNIT_END + 0x427,            // 0x4DD - Size: 1 - Type: TWO_SHORT
    PLAYER_FIELD_LAST_WEEK_KILLS = UNIT_END + 0x428,            // 0x4DE - Size: 1 - Type: TWO_SHORT
    PLAYER_FIELD_THIS_WEEK_KILLS = UNIT_END + 0x429,            // 0x4DF - Size: 1 - Type: TWO_SHORT
    PLAYER_FIELD_THIS_WEEK_CONTRIBUTION = UNIT_END + 0x42A,     // 0x4E0 - Size: 1 - Type: INT
    PLAYER_FIELD_LIFETIME_HONORBALE_KILLS = UNIT_END + 0x42B,   // 0x4E1 - Size: 1 - Type: INT
    PLAYER_FIELD_LIFETIME_DISHONORBALE_KILLS = UNIT_END + 0x42C, // 0x4E2 - Size: 1 - Type: INT
    PLAYER_FIELD_YESTERDAY_CONTRIBUTION = UNIT_END + 0x42D,      // 0x4E3 - Size: 1 - Type: INT
    PLAYER_FIELD_LAST_WEEK_CONTRIBUTION = UNIT_END + 0x42E,      // 0x4E4 - Size: 1 - Type: INT
    PLAYER_FIELD_LAST_WEEK_RANK = UNIT_END + 0x42F,              // 0x4E5 - Size: 1 - Type: INT
    PLAYER_FIELD_BYTES2 = UNIT_END + 0x430,                      // 0x4E6 - Size: 1 - Type: BYTES
    PLAYER_FIELD_WATCHED_FACTION_INDEX = UNIT_END + 0x431,       // 0x4E7 - Size: 1 - Type: INT
    PLAYER_FIELD_COMBAT_RATING_1 = UNIT_END + 0x432,             // 0x4E8 - Size: 20 - Type: INT
    PLAYER_END = UNIT_END + 0x446                                // 0x4FC
};

enum EGameObjectFields {
    OBJECT_FIELD_CREATED_BY = OBJECT_END + 0x0,  // 0x006 - Size: 2 - Type: GUID
    GAMEOBJECT_DISPLAYID = OBJECT_END + 0x2,     // 0x008 - Size: 1 - Type: INT
    GAMEOBJECT_FLAGS = OBJECT_END + 0x3,         // 0x009 - Size: 1 - Type: INT
    GAMEOBJECT_ROTATION = OBJECT_END + 0x4,      // 0x00A - Size: 4 - Type: FLOAT
    GAMEOBJECT_STATE = OBJECT_END + 0x8,         // 0x00E - Size: 1 - Type: INT
    GAMEOBJECT_POS_X = OBJECT_END + 0x9,         // 0x00F - Size: 1 - Type: FLOAT
    GAMEOBJECT_POS_Y = OBJECT_END + 0xA,         // 0x010 - Size: 1 - Type: FLOAT
    GAMEOBJECT_POS_Z = OBJECT_END + 0xB,         // 0x011 - Size: 1 - Type: FLOAT
    GAMEOBJECT_FACING = OBJECT_END + 0xC,        // 0x012 - Size: 1 - Type: FLOAT
    GAMEOBJECT_DYN_FLAGS = OBJECT_END + 0xD,     // 0x013 - Size: 1 - Type: INT
    GAMEOBJECT_FACTION = OBJECT_END + 0xE,       // 0x014 - Size: 1 - Type: INT
    GAMEOBJECT_TYPE_ID = OBJECT_END + 0xF,       // 0x015 - Size: 1 - Type: INT
    GAMEOBJECT_LEVEL = OBJECT_END + 0x10,        // 0x016 - Size: 1 - Type: INT
    GAMEOBJECT_ARTKIT = OBJECT_END + 0x11,       // 0x017 - Size: 1 - Type: INT
    GAMEOBJECT_ANIMPROGRESS = OBJECT_END + 0x12, // 0x018 - Size: 1 - Type: INT
    GAMEOBJECT_PADDING = OBJECT_END + 0x13,      // 0x019 - Size: 1 - Type: INT
    GAMEOBJECT_END = OBJECT_END + 0x14           // 0x01A
};

enum EDynamicObjectFields {
    DYNAMICOBJECT_CASTER = OBJECT_END + 0x0,  // 0x006 - Size: 2 - Type: GUID
    DYNAMICOBJECT_BYTES = OBJECT_END + 0x2,   // 0x008 - Size: 1 - Type: BYTES
    DYNAMICOBJECT_SPELLID = OBJECT_END + 0x3, // 0x009 - Size: 1 - Type: INT
    DYNAMICOBJECT_RADIUS = OBJECT_END + 0x4,  // 0x00A - Size: 1 - Type: FLOAT
    DYNAMICOBJECT_POS_X = OBJECT_END + 0x5,   // 0x00B - Size: 1 - Type: FLOAT
    DYNAMICOBJECT_POS_Y = OBJECT_END + 0x6,   // 0x00C - Size: 1 - Type: FLOAT
    DYNAMICOBJECT_POS_Z = OBJECT_END + 0x7,   // 0x00D - Size: 1 - Type: FLOAT
    DYNAMICOBJECT_FACING = OBJECT_END + 0x8,  // 0x00E - Size: 1 - Type: FLOAT
    DYNAMICOBJECT_PAD = OBJECT_END + 0x9,     // 0x00F - Size: 1 - Type: BYTES
    DYNAMICOBJECT_END = OBJECT_END + 0xA      // 0x010
};

enum ECorpseFields {
    CORPSE_FIELD_OWNER = OBJECT_END + 0x0,          // 0x006 - Size: 2 - Type: GUID
    CORPSE_FIELD_FACING = OBJECT_END + 0x2,         // 0x008 - Size: 1 - Type: FLOAT
    CORPSE_FIELD_POS_X = OBJECT_END + 0x3,          // 0x009 - Size: 1 - Type: FLOAT
    CORPSE_FIELD_POS_Y = OBJECT_END + 0x4,          // 0x00A - Size: 1 - Type: FLOAT
    CORPSE_FIELD_POS_Z = OBJECT_END + 0x5,          // 0x00B - Size: 1 - Type: FLOAT
    CORPSE_FIELD_DISPLAY_ID = OBJECT_END + 0x6,     // 0x00C - Size: 1 - Type: INT
    CORPSE_FIELD_ITEM = OBJECT_END + 0x7,           // 0x00D - Size: 19 - Type: INT
    CORPSE_FIELD_BYTES_1 = OBJECT_END + 0x1A,       // 0x020 - Size: 1 - Type: BYTES
    CORPSE_FIELD_BYTES_2 = OBJECT_END + 0x1B,       // 0x021 - Size: 1 - Type: BYTES
    CORPSE_FIELD_GUILD = OBJECT_END + 0x1C,         // 0x022 - Size: 1 - Type: INT
    CORPSE_FIELD_FLAGS = OBJECT_END + 0x1D,         // 0x023 - Size: 1 - Type: INT
    CORPSE_FIELD_DYNAMIC_FLAGS = OBJECT_END + 0x1E, // 0x024 - Size: 1 - Type: INT
    CORPSE_FIELD_PAD = OBJECT_END + 0x1F,           // 0x025 - Size: 1 - Type: INT
    CORPSE_END = OBJECT_END + 0x20                  // 0x026
};

struct C2Vector {
    float x;
    float y;
};

struct C3Vector {
    float x;
    float y;
    float z;
};

struct C4Quaternion {
    float x;
    float y;
    float z;
    float w; // Unlike Quaternions elsewhere, the scalar part ('w') is the last element in the
             // struct instead of the first
};

struct CImVector {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};

struct Point {
    uint32_t x;
    uint32_t y;
};

struct Rectangle {
    uint32_t width;
    uint32_t heigth;
};

struct TSLink {
    void *prevLink;
    void *next;
};

struct CGObject_C;

struct CGObject_Data {
    uint64_t m_guid;
    uint32_t m_type;
    uint32_t m_entryID;
    uint32_t m_scale;
    uint32_t m_padding;
};

struct CGObject_C_VfTable {
    void *unk0[5];
    C3Vector *(__thiscall *GetPosition)(CGObject_C *, C3Vector *);
    void *unk1[22];
    const char *(__thiscall *GetName)(CGObject_C *);
};

struct CGObject_C {
    CGObject_C_VfTable *vftable;
    void *m_unk0;
    CGObject_Data *m_data;
    void *m_unk1[2];
    uint32_t m_objectType;
    void *m_unk2[6];
    uint64_t m_guid; // 0x30
    void *m_unk3[42];
    CWorld *m_worldData; // 0xE0
};

struct ItemEnchantment {
    int id;
    int expiration;
    int chargesRemaining;
};

struct CGItemData {
    CGObject_Data m_objectData;
    uint64_t m_owner;
    uint64_t m_containedIn;
    uint64_t m_creator;
    uint64_t m_giftCreator;
    uint32_t m_stackCount;
    int32_t m_duration;
    int32_t m_spellCharges[5];
    uint32_t m_flags;
    ItemEnchantment m_enchantment[7];
    uint32_t m_propertySeed;
    int32_t m_randomPropertiesId;
    uint32_t m_itemTextId;
    uint32_t m_durability;
    uint32_t m_maxDurability;
};

struct CGContainerData {
    CGItemData m_itemData;
    uint32_t m_numSlots;
    uint32_t m_padding;
    uint64_t m_slots[36];
};

struct VirtualItemInfo {
    unsigned char m_classID;
    unsigned char m_subclassID;
    unsigned char m_material;
    unsigned char m_inventoryType;
    unsigned char m_sheatheType;
    unsigned char m_padding0;
    unsigned char m_padding1;
    unsigned char m_padding2;
};

struct CGUnit_C;

struct CGUnitData {
    CGObject_Data m_objectData;
    uint64_t m_charm;
    uint64_t m_summon;
    uint64_t m_charmedBy;
    uint64_t m_summonedBy;
    uint64_t m_createdBy;
    uint64_t m_target;
    uint64_t m_persuaded;
    uint64_t m_channelObject;
    uint32_t m_health;
    int32_t m_power[5];
    uint32_t m_maxHealth;
    int32_t m_maxPower[5];
    uint32_t m_level;
    uint32_t m_factionTemplate;
    uint32_t m_bytes0;
    uint32_t m_virtualItemDisplay[3];
    VirtualItemInfo m_virtualItemInfo[3];
    uint32_t m_flags;
    int32_t m_auras[48];
    uint32_t m_auraFlags[6];
    uint8_t m_auraLevels[48];
    uint8_t m_auraApplications[48];
    uint32_t m_auraState;
    uint32_t m_baseAttackTime[2];
    uint32_t m_rangedAttackTime;
    float m_boundingRadius;
    float m_combatReach;
    uint32_t m_displayId;
    uint32_t m_nativeDisplayId;
    uint32_t m_mountDisplayId;
    float m_minDamage;
    float m_maxDamage;
    float m_minOffHandDamage;
    float m_maxOffHandDamage;
    uint32_t m_bytes1;
    uint32_t m_petNumber;
    uint32_t m_petNameTimestamp;
    uint32_t m_petExperience;
    uint32_t m_petNextLevelExperience;
    uint32_t m_dynamicFlags;
    uint32_t m_channelSpell;
    float m_modCastSpeed;
    uint32_t m_createdBySpell;
    uint32_t m_npcFlags;
    uint32_t m_npcEmoteState;
    uint32_t m_trainingPoints;
    int32_t m_stats[5];
    int32_t m_resistances[7];
    uint32_t m_baseMana;
    uint32_t m_baseHealth;
    uint32_t m_bytes2;
    int32_t m_attackPower;
    int32_t m_attackPowerMods;
    float m_attackPowerMultiplier;
    int32_t m_rangedAttackPower;
    int32_t m_rangedAttackPowerMods;
    float m_rangedAttackPowerMultiplier;
    float m_minRangedDamage;
    float m_maxRangedDamage;
    int32_t m_powerCostModifier[7];
    float m_powerCostMultiplier[7];
    uint32_t m_padding;
};

struct CGUnit_C {
    CGObject_C_VfTable *vftable;
    void *m_unk0;
    CGUnitData *m_data;
    void *m_unk1[2];
    uint32_t m_objectType;
    void *m_unk2[6];
    uint64_t m_guid; // 0x30
    void *m_unk3[42];
    CWorld *m_worldData; // 0xE0
};

struct QuestLogEntry {
    int32_t id;
    int32_t state;
    uint32_t pad;
};

struct VisibleItem {
    uint64_t creator;
    uint32_t data[8];
    uint32_t properties;
    uint32_t pad;
};

struct CGPlayerData {
    CGUnitData m_unitData;
    uint64_t m_duelArbiter;
    uint32_t m_flags;
    uint32_t m_guildID;
    uint32_t m_guildRank;
    uint32_t m_bytes;
    uint32_t m_bytes2;
    uint32_t m_bytes3;
    uint32_t m_duelTeam;
    uint32_t m_guildTimestamp;
    QuestLogEntry m_questLog[20];
    VisibleItem m_visibleItems[19];
    uint64_t m_invSlotHead[23];
    uint64_t m_packSlots[16];
    uint64_t m_bankSlots[24];
    uint64_t m_bankBagSlots[6];
    uint64_t m_vendorBuybackSlots[12];
    uint64_t m_keyringSlots[32];
    uint64_t m_farsight;
    uint64_t m_comboTarget;
    int32_t m_xp;
    int32_t m_nextLevelXp;
    uint8_t m_skillInfo[0x600];
    int32_t m_characterPoints1;
    int32_t m_characterPoints2;
    uint32_t m_trackCreatures;
    uint32_t m_trackResources;
    float m_blockPercentage;
    float m_dodgePercentage;
    float m_parryPercentage;
    float m_critPercentage;
    float m_rangedCritPercentage;
    uint32_t m_exploredZones[64];
    int32_t m_restStateExperience;
    uint32_t m_coinage;
    int32_t m_posStat[5];
    int32_t m_negStat[5];
    int32_t m_resistanceBuffModsPositive[7];
    int32_t m_resistanceBuffModsNegative[7];
    int32_t m_modDamageDonePos[7];
    int32_t m_modDamageDoneNeg[7];
    int32_t m_modDamageDonePct[7];
    uint32_t m_fieldBytes;
    uint32_t m_ammoId;
    uint32_t m_selfResSpell;
    uint32_t m_pvpMedals;
    uint32_t m_buybackPrice[12];
    uint32_t m_buybackTimestamp[12];
    uint32_t m_sessionKills;
    uint32_t m_yesterdayKills;
    uint32_t m_lastWeekKills;
    uint32_t m_thisWeekKills;
    uint32_t m_thisWeekContribution;
    uint32_t m_lifetimeHonorableKills;
    uint32_t m_lifetimeDishonorableKills;
    uint32_t m_yesterdayContribution;
    uint32_t m_lastWeekContribution;
    uint32_t m_lastWeekRank;
    uint32_t m_bytes_2_tail;
    uint32_t m_watchedFactionIndex;
    uint32_t m_combatRating[20];
};

struct CGGameObjectData {
    CGObject_Data m_objectData;
    uint64_t m_unk;
    int m_displayID;
    unsigned int m_flags;
    C4Quaternion m_rotation;
    int m_state;
    C3Vector m_position;
    float m_facing;
    uint32_t m_dynamicFlags;
    int m_factionTemplate;
    GAMEOBJECT_TYPES m_type;
    int m_level;
    int m_artKit;
    int m_animProgress;
    int m_padding;
};

struct CGDynamicObjectData {
    CGObject_Data m_objectData;
    uint64_t m_caster;
    uint32_t m_bytes;
    int32_t m_spellID;
    float m_radius;
    C3Vector m_position;
    float m_facing;
    int32_t m_padding;
};

struct CGCorpseData {
    CGObject_Data m_objectData;
    uint64_t m_owner;
    float m_facing;
    C3Vector m_position;
    uint32_t m_displayID;
    uint32_t m_items[19];
    uint32_t m_bytes1;
    uint32_t m_bytes2;
    uint32_t m_guild;
    uint32_t m_flags;
    uint32_t m_dynamicFlags;
    uint32_t m_padding;
};

struct TexCoord {
    C2Vector bottomLeft;  // Coordinate as a fraction of the image's width from the left
    C2Vector bottomRight; // Coordinate as a fraction of the image's width from the left
    C2Vector topLeft;     // Coordinate as a fraction of the image's height from the top
    C2Vector topRight;    // Coordinate as a fraction of the image's height from the top
};

struct CGMinimapFrame_FrameScriptPart;

struct CGMinimapFrame_FrameScriptPartVfTable {
    void *unk[7];
    float(__thiscall *GetUnkScale)(CGMinimapFrame_FrameScriptPart *);
};

struct CGMinimapFrame_LayoutPart {
    struct CGMinimapFrame_CLayoutFramePartVfTable *vftable;
    void *m_unk[8];
};

struct CGMinimapFrame_FrameScriptPart {
    CGMinimapFrame_FrameScriptPartVfTable *vftable;
};

struct CGMinimapFrame {
    CGMinimapFrame_LayoutPart layoutPart;
    CGMinimapFrame_FrameScriptPart FrameScriptPart;
};

struct DNInfo {
    unsigned int time;
    float dayProgression;
    float day;
    C3Vector playerPos;
    C3Vector cameraPos;
    C3Vector cameraDir;
    float faceAngle;
    float nearClipScaled;
    float farClip;
    float elapsedSec;
    // There's more to it
};

struct MINIMAPINFO {
    void *unk0;
    uint32_t wmoID;
    uint32_t mapObjID;
    C3Vector currentPos;
    float radius;
    float layoutScale;
    CGMinimapFrame *minimapFrame;
};

struct STATUSENTRY {
    const char *text;
    STATUS_TYPE severity;
    TSLink link;
};

struct CStatus {
    void *vftable;
    uint32_t m_unk; // Seems to always be set to 8
    TSLink m_head;
    STATUS_TYPE m_maxSeverity;

    CStatus();
    ~CStatus();

    [[nodiscard]] bool ok() const { return m_maxSeverity < STATUS_TYPE::STATUS_ERROR; }
};

struct CGxTexFlags {
    uint32_t m_filter : 3;
    uint32_t m_wrapU : 1;
    uint32_t m_wrapV : 1;
    uint32_t m_forceMipTracking : 1;
    uint32_t m_generateMipMaps : 1;
    uint32_t m_renderTarget : 1;
    uint32_t m_unknownFlag : 1;
    uint32_t m_maxAnisotropy : 5;

    CGxTexFlags(EGxTexFilter filter, uint32_t wrapU, uint32_t wrapV, uint32_t forceMipTracking,
                uint32_t generateMipMaps, uint32_t renderTarget, uint32_t maxAnisotropy,
                uint32_t unknownFlag);
};

struct FactionTemplate {
    int m_ID;
    int m_faction;
    int m_flags;
    int m_factionGroup;
    int m_friendGroup;
    int m_enemyGroup;
    int m_enemies[4];
    int m_friend[4];
};

template <typename T> struct WowClientDB {
    T *m_records;
    int m_numRecords;
    T **m_recordsById;
    int m_maxId;
    int m_loaded;
};

namespace Lua {
using lua_pushnil_t = void(__fastcall *)(void *L);
using lua_isnumber_t = bool(__fastcall *)(void *L, int index);
using lua_tonumber_t = double(__fastcall *)(void *L, int index);
using lua_pushnumber_t = void(__fastcall *)(void *L, double value);
using lua_isstring_t = bool(__fastcall *)(void *L, int index);
using lua_tostring_t = const char *(__fastcall *)(void *L, int index);
using lua_pushstring_t = void(__fastcall *)(void *L, const char *);
using lua_gettable_t = void(__fastcall *)(void *L, int index);
using lua_type_t = int(__fastcall *)(void *L, int index);
using lua_next_t = int(__fastcall *)(void *L, int index);
using lua_settop_t = void(__fastcall *)(void *L, int index);
using lua_error_t = void(__cdecl *)(void *L, const char *);

extern const lua_pushnil_t PushNil;
extern const lua_isnumber_t IsNumber;
extern const lua_tonumber_t ToNumber;
extern const lua_pushnumber_t PushNumber;
extern const lua_isstring_t IsString;
extern const lua_tostring_t ToString;
extern const lua_pushstring_t PushString;
extern const lua_gettable_t GetTable;
extern const lua_type_t Type;
extern const lua_next_t Next;
extern const lua_settop_t SetTop;
extern const lua_error_t Error;

inline bool IsTable(void *L, int index) { return Type(L, index) == 5; }
inline void Pop(void *L, int index) { SetTop(L, -(index)-1); }
} // namespace Lua

using FrameScript_RegisterFunction_t = void(__fastcall *)(const char *name, uintptr_t func);
using LoadScriptFunctions_t = void(__fastcall *)();
using GetGUIDFromName_t = uint64_t(__fastcall *)(const char *unitName);
using ClntObjMgrObjectPtr_t = CGObject_C *(__fastcall *)(uint32_t typeMask,
                                                         const char *debugMessage, uint64_t guid,
                                                         int debugCode);
using RenderObjectBlips_t = void(__thiscall *)(CGMinimapFrame *thisptr, DNInfo *dnInfo);
using TextureGetGxTex_t = CGxTex *(__fastcall *)(HTEXTURE__ * texture, int, CStatus *status);
using GxRsSet_t = void(__fastcall *)(EGxRenderState, CGxTex *);
using GxPrimLockVertexPtrs_t = void(__fastcall *)(uint32_t, C3Vector *, uint32_t, C3Vector *,
                                                  uint32_t, const CImVector *, uint32_t,
                                                  unsigned char *, uint32_t, C2Vector *, uint32_t,
                                                  C2Vector *, uint32_t);
using GxPrimDrawElements_t = void(__fastcall *)(EGxPrim, uint32_t, unsigned short *);
using GxPrimUnlockVertexPtrs_t = void(__fastcall *)();
using CStatus_Destructor_t = void(__thiscall *)(CStatus *thisptr);
using CGxTexFlags_Constructor_t =
    uint32_t *(__thiscall *)(void *thisptr, uint32_t filter, uint32_t wrapU, uint32_t wrapV,
                             uint32_t forceMipTracking, uint32_t generateMipMaps,
                             uint32_t renderTarget, uint32_t maxAnisotropy, uint32_t unknownFlag);
using TextureCreate_t = HTEXTURE__ *(__fastcall *)(const char *filename, CStatus *status,
                                                   CGxTexFlags texFlags, int unkParam1,
                                                   int unkParam2);
using WorldPosToMinimapFrameCoords_t = C2Vector *(__fastcall *)(C2Vector * out, void *edx,
                                                                C3Vector cur, float worldRadius,
                                                                float worldX, float worldY,
                                                                float layoutScale, float unkScale);
using ObjectEnumProc_t = int(__fastcall *)(MINIMAPINFO *info, uint64_t guid);
using ClntObjMgrEnumVisibleObjectsCallback_t = bool(__fastcall *)(void *context, void *edx,
                                                                  uint64_t guid);
using ClntObjMgrEnumVisibleObjects_t =
    bool(__fastcall *)(ClntObjMgrEnumVisibleObjectsCallback_t callback, void *context);
using SStrPack_t = int(__stdcall *)(char *dst, const char *src, int cap);
using InvalidFunctionPtrCheck_t = void(__fastcall *)(void *target);
using CWorld_QueryMapObjIDs_t = bool(__fastcall *)(CWorld *thisptr, uint32_t *outWmoID,
                                                   uint32_t *outMapObjID, uint32_t *outGroupNum);
using ClntObjMgrGetActivePlayer_t = uint64_t(__fastcall *)();
using CGUnit_C_CanAssist_t = bool(__thiscall *)(CGUnit_C *thisptr, CGUnit_C *other);
using FrameScript_Initialize_t = bool(__fastcall *)();
using FrameScript_Execute_t = bool(__fastcall *)(const char *script, const char *scriptName);
using CVar_Callback_t = bool(__fastcall *)(void *CVar, const char *oldValue, const char *newValue,
                                           void *userData);
using CVar_Register_t = void(__fastcall *)(const char *name, const char *help, uint32_t flags,
                                           const char *defaultValue, CVar_Callback_t callback,
                                           CVAR_CATEGORY category, bool consoleOnly,
                                           void *userData);
using InitializeGlobal_t = bool(__fastcall *)();
using CGUnit_C_CreateUnitMount_t = void(__thiscall *)(CGUnit_C *thisptr);
using CGUnit_C_RefreshMount_t = void(__thiscall *)(CGUnit_C *thisptr, bool reset);
using TextureBlit_t = void(__stdcall *)(void *, void *, Point *offset0, Point *offset1,
                                        Rectangle *copy, Rectangle *extent);
using SFile_Open_t = uint64_t(__stdcall *)(const char *filename, SFile **outFile);
using SFile_Read_t = uint64_t(__stdcall *)(SFile *file, void *buffer, uint32_t bytesToRead,
                                           uint32_t *bytesRead, void *reserved1, void *reserved2);
using SFile_Close_t = uint64_t(__stdcall *)(SFile *file);
using CGObject_C_SetBlock_t = int(__thiscall *)(Game::CGObject_C *thisptr, uint32_t fieldIndex,
                                                uint32_t value);
using CGUnit_C_UpdateDisplayInfo_t = void(__thiscall *)(Game::CGUnit_C *thisptr);
using CGUnit_C_Destructor_t = void(__thiscall *)(Game::CGUnit_C *thisptr);

extern const FrameScript_RegisterFunction_t FrameScript_RegisterFunction;
extern const GetGUIDFromName_t GetGUIDFromName;
extern const ClntObjMgrObjectPtr_t ClntObjMgrObjectPtr;
extern const RenderObjectBlips_t RenderObjectBlips;
extern const TextureGetGxTex_t TextureGetGxTex;
extern const GxRsSet_t GxRsSet;
extern const GxPrimLockVertexPtrs_t GxPrimLockVertexPtrs;
extern const GxPrimDrawElements_t GxPrimDrawElements;
extern const GxPrimUnlockVertexPtrs_t GxPrimUnlockVertexPtrs;
extern const TextureCreate_t TextureCreate;
extern const WorldPosToMinimapFrameCoords_t WorldPosToMinimapFrameCoords;
extern const SStrPack_t SStrPack;
extern const CWorld_QueryMapObjIDs_t CWorld_QueryMapObjIDs;
extern const ClntObjMgrGetActivePlayer_t ClntObjMgrGetActivePlayer;
extern const CGUnit_C_CanAssist_t CGUnit_C_CanAssist;
extern const FrameScript_Execute_t FrameScript_Execute;
extern const CVar_Register_t CVar_Register;
extern const ClntObjMgrEnumVisibleObjects_t ClntObjMgrEnumVisibleObjects;
extern const CGUnit_C_RefreshMount_t CGUnit_C_RefreshMount;
extern const SFile_Open_t SFile_Open;
extern const SFile_Read_t SFile_Read;
extern const SFile_Close_t SFile_Close;
extern const CGUnit_C_UpdateDisplayInfo_t CGUnit_C_UpdateDisplayInfo;

void DrawMinimapTexture(HTEXTURE__ *texture, C2Vector minimapPosition, float scale, bool gray);

template <typename T> inline const T *DbLookupById(const WowClientDB<T> &db, int id) {
    if (!db.m_loaded || !db.m_recordsById)
        return nullptr;

    if (id < 0 || id > db.m_maxId)
        return nullptr;

    return db.m_recordsById[id];
}

extern C3Vector *s_blipVertices;
extern TexCoord &texCoords;
extern C3Vector &normal;
extern unsigned short *vertIndices;
extern const float &BLIP_HALF;
extern const WowClientDB<FactionTemplate> &g_factionTemplateDB;

} // namespace Game
