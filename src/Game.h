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
struct CWorld;     // Don't know real struct

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

namespace Lua {
using lua_pushnil_t = void(__fastcall *)(void *L);
using lua_isnumber_t = bool(__fastcall *)(void *L, int index);
using lua_tonumber_t = double(__fastcall *)(void *L, int index);
using lua_pushnumber_t = void(__fastcall *)(void *L, double value);
using lua_isstring_t = bool(__fastcall *)(void *L, int index);
using lua_tostring_t = const char *(__fastcall *)(void *L, int index);
using lua_pushstring_t = void(__fastcall *)(void *L, const char *);
using lua_error_t = void(__cdecl *)(void *L, const char *);

extern const lua_pushnil_t PushNil;
extern const lua_isnumber_t IsNumber;
extern const lua_tonumber_t ToNumber;
extern const lua_pushnumber_t PushNumber;
extern const lua_isstring_t IsString;
extern const lua_tostring_t ToString;
extern const lua_pushstring_t PushString;
extern const lua_error_t Error;
} // namespace Lua

using FrameScript_RegisterFunction_t = void(__fastcall *)(const char *name, uintptr_t func);
using LoadScriptFunctions_t = void(__fastcall *)();
using GetGUIDFromName_t = uint64_t(__fastcall *)(const char *unitName);
using ClntObjMgrObjectPtr_t = Game::CGObject_C *(__fastcall *)(uint32_t typeMask,
                                                               const char *debugMessage,
                                                               uint64_t guid, int debugCode);
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
using ClntObjMgrEnumVisibleObjectsCallback_t = int(__thiscall *)(void *context, uint64_t guid);
using ClntObjMgrEnumVisibleObjects_t =
    int(__fastcall *)(ClntObjMgrEnumVisibleObjectsCallback_t callback, void *context);
using SStrPack_t = int(__stdcall *)(char *dst, const char *src, int cap);
using InvalidFunctionPtrCheck_t = void(__fastcall *)(void *target);
using CWorld_QueryMapObjIDs_t = bool(__fastcall *)(CWorld *thisptr, uint32_t *outWmoID,
                                                   uint32_t *outMapObjID, uint32_t *outGroupNum);

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

void DrawMinimapTexture(HTEXTURE__ *texture, C2Vector minimapPosition, float scale, bool gray);

extern C3Vector *s_blipVertices;
extern TexCoord &texCoords;
extern C3Vector &normal;
extern unsigned short *vertIndices;
extern const float &BLIP_HALF;

} // namespace Game
