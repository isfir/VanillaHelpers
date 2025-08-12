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

enum TypeMask {
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
    void *unk0, *unk1, *unk2, *unk3, *unk4;
    C3Vector *(__thiscall *GetPosition)(CGObject_C *, C3Vector *);
};

struct CGObject_C {
    CGObject_C_VfTable *vftable;
    uint64_t guid;
    uint32_t objectType;
};

struct TexCoord {
    C2Vector bottomLeft;  // Coordinate as a fraction of the image's width from the left
    C2Vector bottomRight; // Coordinate as a fraction of the image's width from the left
    C2Vector topLeft;     // Coordinate as a fraction of the image's height from the top
    C2Vector topRight;    // Coordinate as a fraction of the image's height from the top
};

struct CGMinimapFrame_FrameScriptPart;

struct CGMinimapFrame_FrameScriptPartVfTable {
    void *unk0, *unk1, *unk2, *unk3, *unk4, *unk5, *unk6;
    float(__thiscall *GetUnkScale)(CGMinimapFrame_FrameScriptPart *);
};

struct CGMinimapFrame_LayoutPart {
    struct CGMinimapFrame_CLayoutFramePartVfTable *vftable;
    void *unk0;
    void *unk1;
    void *unk2;
    void *unk3;
    void *unk4;
    void *unk5;
    void *unk6;
    void *unk7;
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
    uint32_t unk; // Seems to always be set to 8
    TSLink head;
    STATUS_TYPE maxSeverity;

    CStatus();
    ~CStatus();

    bool ok() const { return maxSeverity < STATUS_TYPE::STATUS_ERROR; }
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
typedef void(__fastcall *lua_pushnil_t)(void *L);
typedef bool(__fastcall *lua_isnumber_t)(void *L, int index);
typedef double(__fastcall *lua_tonumber_t)(void *L, int index);
typedef void(__fastcall *lua_pushnumber_t)(void *L, double value);
typedef bool(__fastcall *lua_isstring_t)(void *L, int index);
typedef const char *(__fastcall *lua_tostring_t)(void *L, int index);
typedef void(__fastcall *lua_pushstring_t)(void *L, const char *);
typedef void(__cdecl *lua_error_t)(void *L, const char *);

extern lua_pushnil_t PushNil;
extern lua_isnumber_t IsNumber;
extern lua_tonumber_t ToNumber;
extern lua_pushnumber_t PushNumber;
extern lua_isstring_t IsString;
extern lua_tostring_t ToString;
extern lua_pushstring_t PushString;
extern lua_error_t Error;
} // namespace Lua

typedef void(__fastcall *FrameScript_RegisterFunction)(const char *name, uintptr_t func);
typedef void(__stdcall *LoadScriptFunctions_t)();
typedef uint64_t(__fastcall *GetGUIDFromName_t)(const char *unitName);
typedef Game::CGObject_C *(__fastcall *ClntObjMgrObjectPtr_t)(TypeMask typeMask,
                                                              const char *debugMessage,
                                                              uint64_t guid, int debugCode);
typedef void(__thiscall *RenderObjectBlips_t)(CGMinimapFrame *thisptr, DNInfo *dnInfo);
typedef CGxTex *(__fastcall *TextureGetGxTex_t)(HTEXTURE__ *texture, int, CStatus *status);
typedef void(__fastcall *GxRsSet_t)(EGxRenderState, CGxTex *);
typedef void(__fastcall *GxPrimLockVertexPtrs_t)(uint32_t, C3Vector *, uint32_t, C3Vector *,
                                                 uint32_t, const CImVector *, uint32_t,
                                                 unsigned char *, uint32_t, C2Vector *, uint32_t,
                                                 C2Vector *, uint32_t);
typedef void(__fastcall *GxPrimDrawElements_t)(EGxPrim, uint32_t, unsigned short *);
typedef void(__fastcall *GxPrimUnlockVertexPtrs_t)();
typedef void(__thiscall *CStatus_Destructor_t)(CStatus *thisptr);
typedef uint32_t *(__thiscall *CGxTexFlags_Constructor_t)(
    void *thisptr, uint32_t filter, uint32_t wrapU, uint32_t wrapV, uint32_t forceMipTracking,
    uint32_t generateMipMaps, uint32_t renderTarget, uint32_t maxAnisotropy, uint32_t unknownFlag);
typedef HTEXTURE__ *(__fastcall *TextureCreate_t)(const char *filename, CStatus *status,
                                                  CGxTexFlags texFlags, int unkParam1,
                                                  int unkParam2);
typedef C2Vector *(__fastcall *WorldPosToMinimapFrameCoords_t)(C2Vector *out, void *edx,
                                                               C3Vector cur, float worldRadius,
                                                               float worldX, float worldY,
                                                               float layoutScale, float unkScale);
typedef int(__fastcall *ObjectEnumProc_t)(MINIMAPINFO *info, uint64_t guid);
typedef int(__thiscall *ClntObjMgrEnumVisibleObjectsCallback_t)(void *context, uint64_t guid);
typedef int(__fastcall *ClntObjMgrEnumVisibleObjects_t)(
    ClntObjMgrEnumVisibleObjectsCallback_t callback, void *context);

extern GetGUIDFromName_t GetGUIDFromName;
extern ClntObjMgrObjectPtr_t ClntObjMgrObjectPtr;
extern RenderObjectBlips_t RenderObjectBlips;
extern TextureGetGxTex_t TextureGetGxTex;
extern GxRsSet_t GxRsSet;
extern GxPrimLockVertexPtrs_t GxPrimLockVertexPtrs;
extern GxPrimDrawElements_t GxPrimDrawElements;
extern GxPrimUnlockVertexPtrs_t GxPrimUnlockVertexPtrs;
extern TextureCreate_t TextureCreate;
extern WorldPosToMinimapFrameCoords_t WorldPosToMinimapFrameCoords;

void RegisterLuaFunction(const char *name, int(__cdecl *function)(void *L), uintptr_t caveAddress);
void DrawMinimapTexture(HTEXTURE__ *texture, C2Vector minimapPosition, float scale);

extern C3Vector *s_blipVertices;
extern TexCoord &texCoords;
extern C3Vector &normal;
extern unsigned short *vertIndices;

} // namespace Game
