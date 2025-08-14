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

struct C2Vector {
    float x;
    float y;
};

struct C3Vector {
    float x;
    float y;
    float z;
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

struct CGObject_C_VfTable {
    void *unk0[5];
    C3Vector *(__thiscall *GetPosition)(CGObject_C *, C3Vector *);
    void *unk1[22];
    const char *(__thiscall *GetName)(CGObject_C *);
};

struct CGObject_C {
    CGObject_C_VfTable *vftable;
    uint64_t m_guid;
    uint32_t m_objectType;
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
    void *unk1;
    void *unk2;
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

using FrameScript_RegisterFunction = void(__fastcall *)(const char *name, uintptr_t func);
using LoadScriptFunctions_t = void(__fastcall *)();
using GetGUIDFromName_t = uint64_t(__fastcall *)(const char *unitName);
using ClntObjMgrObjectPtr_t = Game::CGObject_C *(__fastcall *)(TYPE_MASK typeMask,
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

void RegisterLuaFunction(const char *name, int(__fastcall *function)(void *L),
                         uintptr_t caveAddress);
void DrawMinimapTexture(HTEXTURE__ *texture, C2Vector minimapPosition, float scale);

extern C3Vector *s_blipVertices;
extern TexCoord &texCoords;
extern C3Vector &normal;
extern unsigned short *vertIndices;
extern const float &BLIP_HALF;

} // namespace Game
