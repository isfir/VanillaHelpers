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

#include "Game.h"
#include "Offsets.h"

namespace Game {

namespace Lua {
const lua_pushnil_t PushNil = reinterpret_cast<lua_pushnil_t>(Offsets::LUA_PUSH_NIL);
const lua_isnumber_t IsNumber = reinterpret_cast<lua_isnumber_t>(Offsets::LUA_IS_NUMBER);
const lua_tonumber_t ToNumber = reinterpret_cast<lua_tonumber_t>(Offsets::LUA_TO_NUMBER);
const lua_pushnumber_t PushNumber = reinterpret_cast<lua_pushnumber_t>(Offsets::LUA_PUSH_NUMBER);
const lua_isstring_t IsString = reinterpret_cast<lua_isstring_t>(Offsets::LUA_IS_STRING);
const lua_tostring_t ToString = reinterpret_cast<lua_tostring_t>(Offsets::LUA_TO_STRING);
const lua_pushstring_t PushString = reinterpret_cast<lua_pushstring_t>(Offsets::LUA_PUSH_STRING);
const lua_error_t Error = reinterpret_cast<lua_error_t>(Offsets::LUA_ERROR);
} // namespace Lua

const FrameScript_RegisterFunction_t FrameScript_RegisterFunction =
    reinterpret_cast<FrameScript_RegisterFunction_t>(Offsets::FUN_REGISTER_LUA_FUNCTION);
const GetGUIDFromName_t GetGUIDFromName =
    reinterpret_cast<GetGUIDFromName_t>(Offsets::FUN_GET_GUID_FROM_NAME);
const ClntObjMgrObjectPtr_t ClntObjMgrObjectPtr =
    reinterpret_cast<ClntObjMgrObjectPtr_t>(Offsets::FUN_CLNT_OBJ_MGR_OBJECT_PTR);
const RenderObjectBlips_t RenderObjectBlips =
    reinterpret_cast<RenderObjectBlips_t>(Offsets::FUN_RENDER_OBJECT_BLIP);
const TextureGetGxTex_t TextureGetGxTex =
    reinterpret_cast<TextureGetGxTex_t>(Offsets::FUN_TEXTURE_GET_GX_TEX);
const GxRsSet_t GxRsSet = reinterpret_cast<GxRsSet_t>(Offsets::FUN_GX_RS_SET);
const GxPrimLockVertexPtrs_t GxPrimLockVertexPtrs =
    reinterpret_cast<GxPrimLockVertexPtrs_t>(Offsets::FUN_GX_PRIM_LOCK_VERTEX_PTRS);
const GxPrimDrawElements_t GxPrimDrawElements =
    reinterpret_cast<GxPrimDrawElements_t>(Offsets::FUN_GX_PRIM_DRAW_ELEMENTS);
const GxPrimUnlockVertexPtrs_t GxPrimUnlockVertexPtrs =
    reinterpret_cast<GxPrimUnlockVertexPtrs_t>(Offsets::FUN_GX_PRIM_UNLOCK_VERTEX_PTRS);
const TextureCreate_t TextureCreate =
    reinterpret_cast<TextureCreate_t>(Offsets::FUN_TEXTURE_CREATE);
const WorldPosToMinimapFrameCoords_t WorldPosToMinimapFrameCoords =
    reinterpret_cast<WorldPosToMinimapFrameCoords_t>(
        Offsets::FUN_WORLD_POS_TO_MINIMAP_FRAME_COORDS);
const SStrPack_t SStrPack = reinterpret_cast<SStrPack_t>(Offsets::FUN_S_STR_PACK);
const CWorld_QueryMapObjIDs_t CWorld_QueryMapObjIDs =
    reinterpret_cast<CWorld_QueryMapObjIDs_t>(Offsets::FUN_CWORLD_QUERY_MAP_OBJ_IDS);
const ClntObjMgrGetActivePlayer_t ClntObjMgrGetActivePlayer =
    reinterpret_cast<ClntObjMgrGetActivePlayer_t>(Offsets::FUN_CLNT_OBJ_MGR_GET_ACTIVE_PLAYER);
const CGUnit_C_CanAssist_t CGUnit_C_CanAssist =
    reinterpret_cast<CGUnit_C_CanAssist_t>(Offsets::FUN_CGUNIT_C_CAN_ASSIST);
const FrameScript_Execute_t FrameScript_Execute =
    reinterpret_cast<FrameScript_Execute_t>(Offsets::FUN_FRAME_SCRIPT_EXECUTE);
const CVar_Register_t CVar_Register = reinterpret_cast<CVar_Register_t>(Offsets::FUN_CVAR_REGISTER);
const ClntObjMgrEnumVisibleObjects_t ClntObjMgrEnumVisibleObjects =
    reinterpret_cast<ClntObjMgrEnumVisibleObjects_t>(
        Offsets::FUN_CLNT_OBJ_MGR_ENUM_VISIBLE_OBJECTS);
const CGUnit_C_RefreshMount_t CGUnit_C_RefreshMount =
    reinterpret_cast<CGUnit_C_RefreshMount_t>(Offsets::FUN_CGUNIT_C_REFRESH_MOUNT);
const SFile_Open_t SFile_Open = reinterpret_cast<SFile_Open_t>(Offsets::FUN_SFILE_OPEN);
const SFile_Read_t SFile_Read = reinterpret_cast<SFile_Read_t>(Offsets::FUN_SFILE_READ);
const SFile_Close_t SFile_Close = reinterpret_cast<SFile_Close_t>(Offsets::FUN_SFILE_CLOSE);

CStatus::CStatus()
    : vftable(reinterpret_cast<void *>(Offsets::VFTABLE_CSTATUS)), m_unk(8),
      m_head{&m_head, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(&m_head) | 1U)},
      m_maxSeverity(STATUS_TYPE::STATUS_INFO) {}

CStatus::~CStatus() {
    reinterpret_cast<CStatus_Destructor_t>(Offsets::FUN_CSTATUS_DESTRUCTOR)(this);
}

CGxTexFlags::CGxTexFlags(EGxTexFilter filter, uint32_t wrapU, uint32_t wrapV,
                         uint32_t forceMipTracking, uint32_t generateMipMaps, uint32_t renderTarget,
                         uint32_t maxAnisotropy, uint32_t unknownFlag) {
    reinterpret_cast<CGxTexFlags_Constructor_t>(Offsets::FUN_CGX_TEX_FLAGS_CONSTRUCTOR)(
        this, filter, wrapU, wrapV, forceMipTracking, generateMipMaps, renderTarget, maxAnisotropy,
        unknownFlag);
}

C3Vector *s_blipVertices = reinterpret_cast<C3Vector *>(Offsets::CONST_BLIP_VERTICES);
TexCoord &texCoords = *reinterpret_cast<TexCoord *>(Offsets::CONST_TEX_COORDS);
C3Vector &normal = *reinterpret_cast<C3Vector *>(Offsets::CONST_NORMAL_VEC3);
unsigned short *vertIndices = reinterpret_cast<unsigned short *>(Offsets::CONST_VERT_INDICES);
const float &BLIP_HALF = *reinterpret_cast<float *>(Offsets::CONST_BLIP_HALF);
const WowClientDB<FactionTemplate> &g_factionTemplateDB =
    *reinterpret_cast<const WowClientDB<FactionTemplate> *>(Offsets::CONST_FACTION_TEMPLATE_DB);

void DrawMinimapTexture(HTEXTURE__ *texture, C2Vector minimapPosition, float scale, bool gray) {
    CImVector color = {0xFF, 0xFF, 0xFF, 0xFF}; // White
    if (gray) {
        color = {0xFF, 0xB0, 0xB0, 0xB0}; //  Dark Gray
    }

    C3Vector vertices[4];
    for (auto i = 0; i < 4; i++) {
        vertices[i].x = minimapPosition.x + scale * s_blipVertices[i].x;
        vertices[i].y = minimapPosition.y + scale * s_blipVertices[i].y;
        vertices[i].z = scale * s_blipVertices[i].z;
    }

    CStatus status;
    CGxTex *gxTex = TextureGetGxTex(texture, 1, &status);
    if (!status.ok())
        return;

    GxRsSet(GxRs_Texture0, gxTex);

    GxPrimLockVertexPtrs(4, vertices, 12, &normal, 0, &color, 0, nullptr, 0,
                         reinterpret_cast<C2Vector *>(&texCoords), 8, nullptr, 0);

    GxPrimDrawElements(EGxPrim::GxPrim_TriangleStrip, 4, vertIndices);

    GxPrimUnlockVertexPtrs();
}

} // namespace Game
