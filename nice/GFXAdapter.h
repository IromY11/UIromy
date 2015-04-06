#ifndef _ITF_GFX_ADAPTER_H_
#define _ITF_GFX_ADAPTER_H_

#ifndef _ITF_MATRIX44_H_
#include "core/math/Matrix44/Matrix44.h"
#endif

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_AFTERFX_H_
#include "engine/display/AfterFx.h"
#endif //_ITF_AFTERFX_H_

#ifndef _ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_VertexBufferManager.h"
#endif //_ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_COLORINTEGER_H_
#include "core/ColorInteger.h"
#endif //_ITF_COLORINTEGER_H_

#ifndef _ITF_SPLINE_H_
#include "core/math/spline.h"
#endif //_ITF_SPLINE_H_

#ifndef ITF_GFX_VECTOR4_H_
#include "engine/display/GFXVector4.h"
#endif // ITF_GFX_VECTOR4_H_

#ifndef ITF_GFX_MATERIAL_H_
#include "engine/display/material/GFXMaterial.h"
#endif //ITF_GFX_MATERIAL_H_

#ifndef ITF_GFX_ZLIST_H_
#include "engine/display/GFXZList.h"
#endif //ITF_GFX_ZLIST_H_

#ifndef ITF_GFX_RENDER_PASS_H_
#include "engine/display/GFXRenderPass.h"
#endif //ITF_GFX_RENDER_PASS_H_

#ifndef _ITF_FOG_H_
#include "engine/display/Fog.h"
#endif //_ITF_FOG_H_

#ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
#endif //_ITF_GFXADAPTER_SHADERMANAGER_H_

#ifndef ITF_WORLD_VIEW_PROJ_H_
#include "WorldViewProjAdapter.h"
#endif //ITF_WORLD_VIEW_PROJ_H_

#ifndef ITF_ENGINE_QUAD3DPRIM_H_
#include "engine/display/Primitives/Quad3DPrim.h"
#endif //ITF_ENGINE_QUAD3DPRIM_H_

#ifndef ITF_ENGINE_QUADFRAMEBLENDPRIM_H_
#include "engine/display/Primitives/QuadFrameBlendPrim.h"
#endif //ITF_ENGINE_QUADFRAMEBLENDPRIM_H_

#ifndef _ITF_GFX_ADAPTER_CONSTANT_H_
#include "engine\AdaptersInterfaces\GFXAdapter_Constant.h"
#endif //_ITF_GFX_ADAPTER_CONSTANT_H_

#ifndef _ITF_GFXADAPTER_SCREENTRANSITION_MANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_ScreenTransitionManager.h"
#endif //_ITF_GFXADAPTER_SHADERMANAGER_H_

#define float4 ITF::GFX_Vector4
#include "../bin/Shaders_dev/Unified/ShaderParameters.fxh" 
#undef float4

#include "engine/AdaptersInterfaces/GFXAdapter_AsyncScreenshot.h"

#ifdef ITF_WINDOWS 
#define ITF_PICKING_SUPPORT
#endif

#ifndef ITF_CONSOLE
#define ITF_DEBUG_DRAWCALL_STATES
#endif

#if !defined(ITF_FINAL)
#define ITF_GFX_USE_DEBUG_RENDER_TARGET
#endif

namespace ITF
{

class ITF_Mesh;
class FontText;
class Camera;

class AnimMeshScene;
class Font;
class FontText;
class TrailComponent;
class GFXAdapter_shaderManager;
class ITF_shader;
class Trail;
class MaskResolvePrimitive;
class Path;
class String8;

class GFX_LightManager;
class RenderParamManager;
class GFX_GridFluidManager;

struct GfxAdapterCallbackProxy // used to prevent link issue for CLR wrappers
{
    typedef bbool (*boolVoid) (void);
    typedef void (*voidPath) (const Path& _path);
    typedef void (*drcEmulatorPluginSendRenderData) (void * _p, u32 _size,const String8& _buffername);
    typedef void (*engineMonitorPluginSendFile) (const Path& _path, char* _buffer, u32 _bufferSize, bbool _append, bbool _binary);

    static voidPath                         m_pluginLauncherOnSnapShot;
	static voidPath                         m_pluginCrashDumpSenderOnSnapShot;
    static boolVoid                         m_drcEmulatorPluginIsConnected;
    static engineMonitorPluginSendFile      m_engineMonitorPluginSendFile;
    static drcEmulatorPluginSendRenderData  m_drcEmulatorPluginSendRenderData;
};

//=================================================================================================
// Simple structures
//

//-------------------------------------------------------------------------------------------------
// integer rectangle
//
struct GFX_RECT
{
    i32    left;
    i32    top;
    i32    right;
    i32    bottom;
};

//-------------------------------------------------------------------------------------------------
// float rectangle
//
struct GFX_RECT_F32
{
	f32    left;
	f32    top;
	f32    right;
	f32    bottom;
};

//=================================================================================================
// Vertex format definition
//

//-------------------------------------------------------------------------------------------------
// Vertex format : position
//
const int VertexFormat_P = VF_XYZ;

struct VertexP 
{
    void    setData (const Vec3d &_pos) { m_pos   = _pos; }
    void    setData (const Vec2d &_pos) { m_pos = Vec3d(_pos.x(), _pos.y(), 0.0f); }
    Vec3d   m_pos;

    enum { NbInput = 1 };
};

//-------------------------------------------------------------------------------------------------
// Vertex format : position, color
//
const int VertexFormat_PC = VF_XYZ | VF_DIFFUSE;

struct VertexPC
{
public:
    void    setData (const Vec3d &_pos, u32 _color)
    {
        m_pos   = _pos;
        m_color = _color;
    }

    void    setData (const Vec3d &_pos, Float4 SIMD_ARG _color)
    {
        m_pos   = _pos;
        ITFCOLOR_FLOAT4_TO_U32(&m_color, _color);
    }

    void    setData (const Vec2d &_pos, u32 _color)
    {
        m_pos = Vec3d(_pos.x(), _pos.y(), 0.0f);
        m_color = _color;
    }

    void    setData (const Vec2d &_pos, Float4 SIMD_ARG _color)
    {
        m_pos = Vec3d(_pos.x(), _pos.y(), 0.0f);
        ITFCOLOR_FLOAT4_TO_U32(&m_color, _color);
    }

    enum { NbInput = 2 };

    Vec3d   m_pos;
    u32     m_color;
};

//-------------------------------------------------------------------------------------------------
// Vertex format : position, color, uv1
//
const int VertexFormat_PCT = VF_XYZ | VF_DIFFUSE | VF_TEX1;

class   VertexPCT
{
    DECLARE_SERIALIZE()

public:
    void    setData (const Vec3d& _pos, const Vec2d& _uv, u32 _color)
    {
        m_pos   = _pos;
        m_color = _color;
        m_uv    = _uv;
    }
    ITF_INLINE void    setData (const Vec2d& _pos, const Vec2d& _uv, u32 _color, float _z = 0.f)
    {
        m_pos.set(_pos.x(), _pos.y(), _z);
        m_color = _color;
        m_uv = _uv;
    }

    void    setData (const Vec3d& _pos, const Vec2d& _uv, Float4 SIMD_ARG _color)
    {
        m_pos   = _pos;
        ITFCOLOR_FLOAT4_TO_U32(&m_color, _color);
        m_uv    = _uv;
    }


    ITF_INLINE void setColor(u32 _color) { m_color = _color; }
    ITF_INLINE u32 getColor() const {return m_color;}
    ITF_INLINE void setPosition(const Vec3d& _pos) { m_pos = _pos; }
    ITF_INLINE void setUV(const Vec2d& _uv) { m_uv = _uv; }

    enum { NbInput = 3 };

    Vec3d   m_pos;
    u32     m_color;
    Vec2d   m_uv;
};

//-------------------------------------------------------------------------------------------------
// Vertex format : position, color, uv1, uv4
//
const int VertexFormat_PC2T = VF_XYZ | VF_DIFFUSE | VF_TEX1 | VF_TEX4;

class   VertexPC2T
{
public:
    void    setData (const Vec3d& _pos, const Vec2d& _uv, const Vec2d& _uv2, u32 _color)
    {
        m_pos = _pos;
        m_uv = _uv;
        m_uv2 = _uv2;
        m_color = _color;
    }

    void    setData (const Vec2d& _pos, const Vec2d& _uv, const Vec2d& _uv2, u32 _color, float _z = 0.f)
    {
        m_pos.set(_pos.x(), _pos.y(), _z);
        m_uv = _uv;
        m_uv2 = _uv2;
        m_color = _color;
    }

    void    setData (const Vec3d& _pos, const Vec2d& _uv, const Vec2d& _uv2, Float4 SIMD_ARG _color)
    {
        m_pos = _pos;
        m_uv = _uv;
        m_uv2 = _uv2;
        ITFCOLOR_FLOAT4_TO_U32(&m_color, _color);
    }


    ITF_INLINE void setColor(u32 _color) { m_color = _color; }
    ITF_INLINE u32 getColor() const {return m_color;} 
    ITF_INLINE void setPosition(const Vec3d& _pos) { m_pos = _pos; }
    ITF_INLINE void setUV(const Vec2d& _uv) { m_uv = _uv; }
    ITF_INLINE void setUV2(const Vec2d& _uv2) { m_uv2 = _uv2; }

    enum { NbInput = 4 };

    Vec3d   m_pos;
    u32     m_color;
    Vec2d   m_uv;
    Vec2d   m_uv2;
};

//-------------------------------------------------------------------------------------------------
// Vertex format : position, color, uv1, blend indices, blend weight (skinning)
//
const int VertexFormat_PCTBIBW = VF_XYZ | VF_DIFFUSE | VF_TEX1 | VF_BLENDINDICES | VF_BLENDWEIGHT;
class   VertexPCTBIBW
{
public:
    void    setData (const Vec3d& _pos, const Vec2d& _uv, u32 _color)
    {
        m_pos   = _pos;
        m_uv    = _uv;
        m_color = _color;
        m_blendIndices = 0;
        m_blendWeight = 0xFF;
    }

    ITF_INLINE void    setData (const Vec2d& _pos, const Vec2d& _uv, u32 _color, float _z = 0.f)
    {
        m_pos.set(_pos.x(), _pos.y(), _z);
        m_uv = _uv;
        m_color = _color;
        m_blendIndices = 0;
        m_blendWeight = 0xFF;
    }

    ITF_INLINE void setColor(u32 _color) { m_color = _color; }
    ITF_INLINE u32 getColor() const {return m_color;}
    ITF_INLINE void setPosition(const Vec3d& _pos) { m_pos = _pos; }
    ITF_INLINE void setUV(const Vec2d& _uv) { m_uv = _uv; }

    ITF_INLINE void setBlend( u32 _i0, f32 _w0 = 1.0f, u32 _i1 = 0, f32 _w1 = 0.0f, u32 _i2 = 0, f32 _w2 = 0.0f, u32 _i3 = 0, f32 _w3 = 0.0f )
    {
        m_blendIndices = _i0 & 0xFF;
        m_blendIndices |= (_i1 & 0xFF) << 8;
        m_blendIndices |= (_i2 & 0xFF) << 16;
        m_blendIndices |= (_i3 & 0xFF) << 24;

        u8 W1 = (u8) (_w1 * 255.0f);
        u8 W2 = (u8) (_w2 * 255.0f);
        u8 W3 = (u8) (_w3 * 255.0f);
        u8 W0 = 255 - W1 - W2 - W3;
        m_blendWeight = W0 & 0xFF;
        m_blendWeight |= (W1 & 0xFF) << 8;
        m_blendWeight |= (W2 & 0xFF) << 16;
        m_blendWeight |= (W3 & 0xFF) << 24;
    }

    enum { NbInput = 5 };

    Vec3d   m_pos;
    u32     m_color;
    u32     m_blendWeight;
    u32     m_blendIndices;
    Vec2d   m_uv;
};

const int VertexFormat_PNCTBIBW = VF_XYZ | VF_NORMAL | VF_DIFFUSE | VF_TEX1 | VF_BLENDINDICES | VF_BLENDWEIGHT;
class   VertexPNCTBIBW
{
public:
    void    setData (const Vec3d& _pos, u32 _norm, u32 _tan, const Vec2d& _uv, u32 _color)
    {
        m_pos   = _pos;
        m_normal  = _norm;
        m_tangent  = _tan;
        m_uv    = _uv;
        m_color = _color;
        m_blendIndices = 0;
        m_blendWeight = 0xFF;
    }

    ITF_INLINE void    setData (const Vec2d& _pos, u32 _norm, u32 _tan, const Vec2d& _uv, u32 _color, float _z = 0.f)
    {
        m_pos.set(_pos.x(), _pos.y(), _z);
        m_normal  = _norm;
        m_tangent  = _tan;
        m_uv = _uv;
        m_color = _color;
        m_blendIndices = 0;
        m_blendWeight = 0xFF;
    }

    ITF_INLINE void setColor(u32 _color)
    {
        m_color = _color;
    }

    ITF_INLINE u32 getColor() const {return m_color;}

    ITF_INLINE void setPosition(const Vec3d& _pos)
    {
        m_pos = _pos;
    }

    ITF_INLINE void setUV(const Vec2d& _uv)
    {
        m_uv = _uv;
    }

    ITF_INLINE void setBlend( u32 _i0, f32 _w0 = 1.0f, u32 _i1 = 0, f32 _w1 = 0.0f, u32 _i2 = 0, f32 _w2 = 0.0f, u32 _i3 = 0, f32 _w3 = 0.0f )
    {
        m_blendIndices = _i0 & 0xFF;
        m_blendIndices |= (_i1 & 0xFF) << 8;
        m_blendIndices |= (_i2 & 0xFF) << 16;
        m_blendIndices |= (_i3 & 0xFF) << 24;

        u8 W1 = (u8) (_w1 * 255.0f);
        u8 W2 = (u8) (_w2 * 255.0f);
        u8 W3 = (u8) (_w3 * 255.0f);
        u8 W0 = 255 - W1 - W2 - W3;
        m_blendWeight = W0 & 0xFF;
        m_blendWeight |= (W1 & 0xFF) << 8;
        m_blendWeight |= (W2 & 0xFF) << 16;
        m_blendWeight |= (W3 & 0xFF) << 24;
    }

    enum { NbInput = 7 };

    Vec3d   m_pos;
    u32		m_normal;
    u32		m_tangent;
    u32     m_color;
    u32     m_blendWeight;
    u32     m_blendIndices;
    Vec2d   m_uv;
};

//-------------------------------------------------------------------------------------------------
// Vertex format : position, uv1
//
const int VertexFormat_PT = VF_XYZ | VF_TEX1;

class   VertexPT
{
public:
    void    setData (const Vec3d& _pos, const Vec2d& _uv)
    {
        m_pos   = _pos;
        m_uv    = _uv;
    }

    void    setData (const Vec2d& _pos, const Vec2d& _uv ,f32 _z = 0.f)
    {
        m_pos = Vec3d(_pos.x(), _pos.y(), _z);
        m_uv = _uv;
      
    }

    enum { NbInput = 2 };

    Vec3d   m_pos;
    Vec2d   m_uv;
};

//VLTODO a virer
typedef VertexPCT Vertex;


//-------------------------------------------------------------------------------------------------
// Vertex format : position, color, uv1, blend indices
//
const int VertexFormat_PCBT = VF_XYZ | VF_TEX1 | VF_DIFFUSE | VF_DWORD;

// According to the vertex buffer ubyte4 format
inline u32 BuildUByte4(u8 _x, u8 _y, u8 _z, u8 _w)
{
#ifdef ITF_PS3
    return u32(_w) + (u32(_z) << 8u) + (u32(_y) << 16u) + (u32(_x) << 24u);
#else
    return u32(_x) + (u32(_y) << 8u) + (u32(_z) << 16u) + (u32(_w) << 24u);
#endif
}

class VertexPCBT
{
    DECLARE_SERIALIZE()

public:
    void setData(const Vec3d& _pos, const Vec3d& _normal,const Vec2d& _uv, u32 _color, u32 _blendindices)
    {
        m_pos           = _pos;
        m_uv            = _uv;
        m_color         = _color;
        m_blendindices  = BuildUByte4(u8(_blendindices), 0, 0, 0);
    }

    void setData(const Vec2d& _pos, DWORD _color, u32 _blendindices, const Vec2d& _uv)
    {
        m_pos.x()       = _pos.x();
        m_pos.y()       = _pos.y();
        m_pos.z()       = 0.f;
        m_color         = _color;
        m_blendindices  = BuildUByte4(u8(_blendindices), 0, 0, 0);
        m_uv            = _uv;
    }

    enum { NbInput = 4 };

    Vec3d   m_pos;
    u32     m_color;
    u32     m_blendindices;
    Vec2d   m_uv;
};

//-------------------------------------------------------------------------------------------------
// Vertex format : position, color, uv1, uv4, blend indices
//
const int VertexFormat_PCB2T = VF_XYZ | VF_TEX1 | VF_TEX4 | VF_DIFFUSE | VF_DWORD;

class VertexPCB2T
{
    DECLARE_SERIALIZE()

public:
    void setData(const Vec3d& _pos,const Vec2d& _uv, const Vec2d& _uv2,DWORD _color,DWORD _blendindices)
    {
        m_pos           = _pos;
        m_uv            = _uv;
        m_uv2           = _uv2;
        m_color         = _color;
        m_blendindices  = BuildUByte4(u8(_blendindices), 0, 0, 0);
    }

    void setData(const Vec2d& _pos, DWORD _color, DWORD _blendindices, const Vec2d& _uv, const Vec2d& _uv2)
    {
        m_pos.x()       = _pos.x();
        m_pos.y()       = _pos.y();
        m_pos.z()       = 0.f;
        m_color         = _color;
        m_blendindices  = BuildUByte4(u8(_blendindices), 0, 0, 0);
        m_uv            = _uv;
        m_uv2           = _uv2;
    }

    enum { NbInput = 5 };

    Vec3d   m_pos;
    DWORD   m_color;
    DWORD   m_blendindices;
    Vec2d   m_uv;
    Vec2d   m_uv2;
};

//-------------------------------------------------------------------------------------------------
// Vertex format : position, color, uv1, normal
//
const int VertexFormat_PNCT = VF_XYZ | VF_TEX1 | VF_NORMAL | VF_DIFFUSE;

class VertexPNCT
{
public:
    void    setData(const Vec3d& _pos, u32 _norm, u32 _tan, const Vec2d& _uv, u32 _color)
    {
        m_pos           = _pos;
        m_uv            = _uv;
        m_normal		= _norm;
        m_tangent		= _tan;
        m_color         = _color;
    }


    ITF_INLINE void setColor(u32 _color)
    {
        m_color = _color;
    }

    enum { NbInput = 5 };

    Vec3d   m_pos;
    u32		m_normal;
    u32		m_tangent;
    u32     m_color;
    Vec2d   m_uv;
};

//-------------------------------------------------------------------------------------------------
// Vertex format : position, color, uv1, uv2, uv3, uv4
//
const int VertexFormat_PNC3T = VF_XYZ | VF_DIFFUSE | VF_TEX1 | VF_TEX2 | VF_TEX3 | VF_TEX4;

class VertexPNC3T
{
    DECLARE_SERIALIZE()

public:
    void  setData(const Vec3d& _pos, u32 _color, const Vec2d& _uv, const GFX_Vector4& _uv2, const GFX_Vector4& _uv3, const Vec2d& _uv4 )
    {
        m_pos           = _pos;       
        m_color         = _color;
        m_uv            = _uv;
        m_uv2           = _uv2;
        m_uv3           = _uv3;        
        m_uv4           = _uv4;
    }

    void  setData(const Vec3d& _pos, u32 _color, const Vec2d& _uv)
    {
        m_pos           = _pos;       
        m_color         = _color;
        m_uv            = _uv;
        m_uv4           = Vec2d::Zero;
    }

    ITF_INLINE void setPos(const Vec3d& _pos) { m_pos = _pos; }
    ITF_INLINE void setColor(u32 _color) { m_color = _color; }
    ITF_INLINE void setUv(const Vec2d& _uv) { m_uv = _uv; }

    enum { NbInput = 5 };

    Vec3d   m_pos;
    u32     m_color;
    Vec2d   m_uv;
    GFX_Vector4 m_uv2;
    GFX_Vector4 m_uv3;
    Vec2d       m_uv4;
};


//-------------------------------------------------------------------------------------------------
// VertexQuadInstancingPSC   : 2 streams -> indexStream (Float2) & instanceStream(float2, float2, color)
// Vertex format : quad instance, uv3, uv5, color
//
const int VertexFormat_QuadInstance_2TC = VF_QUAD_INSTANCE | VF_TEX4 | VF_TEX5 | VF_DIFFUSE;

class   VertexQI_2TC
{
    DECLARE_SERIALIZE()

public:
    void    setData (const Vec2d& _uv, const Vec2d& _uv2, u32 _color)
    {
#ifdef GFX_NO_INSTANCING
		m_vertices[0].m_pos.set(-1.0f, -1.0f);
        m_vertices[0].m_uv    = _uv;
        m_vertices[0].m_uv2   = _uv2;
        m_vertices[0].m_color = _color;

		m_vertices[1].m_pos.set(-1.0f, 1.0f);
        m_vertices[1].m_uv    = _uv;
        m_vertices[1].m_uv2   = _uv2;
        m_vertices[1].m_color = _color;

		m_vertices[2].m_pos.set(1.0f, 1.0f);
        m_vertices[2].m_uv    = _uv;
        m_vertices[2].m_uv2   = _uv2;
        m_vertices[2].m_color = _color;

		m_vertices[3].m_pos.set(1.0f, -1.0f);
        m_vertices[3].m_uv    = _uv;
        m_vertices[3].m_uv2   = _uv2;
        m_vertices[3].m_color =  _color;
#else
        m_uv    = _uv;
        m_uv2   = _uv2;
        m_color = _color;
#endif
    }

    void    setData (const Vec2d& _uv, const Vec2d& _uv2, Float4 SIMD_ARG _color)
    {
#ifdef GFX_NO_INSTANCING
		u32 color;
        ITFCOLOR_FLOAT4_TO_U32(&color, _color);
		setData( _uv, _uv2, color );
#else
        m_uv   = _uv;
        m_uv2    = _uv2;
        ITFCOLOR_FLOAT4_TO_U32(&m_color, _color);
#endif
    }

#ifdef GFX_NO_INSTANCING
	struct VertexElem
	{
		Vec2d   m_pos;
		Vec2d   m_uv;
		Vec2d   m_uv2;
		u32     m_color;
	};
	VertexElem m_vertices[4];
#else
    Vec2d   m_uv;
    Vec2d   m_uv2;
    u32     m_color;
#endif

    enum { NbInput = 3 };
};

//-------------------------------------------------------------------------------------------------
// Vertex format : uv1
//
const int VertexFormat_T = VF_TEX1;

class   VertexT
{
    DECLARE_SERIALIZE()

public:
    void    setData (const Vec2d& _uv) { m_uv    = _uv; }
    Vec2d   m_uv;

    enum { NbInput = 1 };
};

//-------------------------------------------------------------------------------------------------
// Vertex format : blend weight
//
const int VertexFormat_I = VF_BLENDWEIGHT;

class   VertexI
{
    DECLARE_SERIALIZE()

public:
    void    setData (const u32 _int) { m_int = _int; }
    u32   m_int;

    enum { NbInput = 1 };
};

struct VertexPatchAnim
{
    void    setData (const Vec2d& _pos, const Vec2d& _posNrm, const Vec2d& _uv, const Vec2d& _uvNrm, u8 _boneIx)
    {
        m_pos       = _pos;
        m_posNrm    = _posNrm;
        m_uv        = _uv;
        m_uvNrm     = _uvNrm;
        m_boneIdx   = _boneIx;
    }

    Vec2d   m_pos;
    Vec2d   m_posNrm;
    Vec2d   m_uv;
    Vec2d   m_uvNrm;
    f32     m_boneIdx;

    enum { NbInput = 5 };
};


const int VertexFormat_PTa = VF_XYZ | VF_TEX1 | VF_AMBIANT;

class PolyLine;
class SpriteAtomicObject;
class SpriteWorld;
class AnimScene;

/////////////////////////////////////////////////////////////

    //  vertexs buffer

struct Triangle
{
    Triangle(const ITF_VECTOR<Vertex>* _vertexList, u32 _i, u32 _ta, const ITF_VECTOR<u32>* _indexes=NULL) : m_vertexs(_vertexList)
    {
        index       = _i;
        taille      = _ta;
        m_indexes   = _indexes;
    }

    const ITF_VECTOR<Vertex>*   m_vertexs;
    const ITF_VECTOR<u32>*      m_indexes;
    u32     index;
    u32     taille;

    const Triangle& operator = (const Triangle& _p)
    {
        m_vertexs   = _p.m_vertexs;
        m_indexes   = _p.m_indexes;
        index       = _p.index;
        taille      = _p.taille;
        return *this;
    }    
};

struct Strip
{
    Strip(const ITF_VECTOR<Vertex>* _vertexList, u32 _i, u32 _ta, const ITF_VECTOR<u32>* _indexes=NULL) : m_vertexs(_vertexList)
    {
        index       = _i;
        taille      = _ta;
        m_indexes   = _indexes;
    }
    
    const ITF_VECTOR<Vertex>*   m_vertexs;
    const ITF_VECTOR<u32>*      m_indexes;
    u32     index;
    u32     taille;

    const Strip& operator = (const Strip& _p)
    {
        m_vertexs   = _p.m_vertexs;
        m_indexes   = _p.m_indexes;
        index       = _p.index;
        taille      = _p.taille;
        return *this;
    }    
};

struct Fan
{
    Fan(const ITF_VECTOR<Vertex>* _t, u32 _i, u32 _ta, const ITF_VECTOR<u32>* _indexes=NULL) : m_vertexs(_t)
    {
        index       = _i;
        taille      = _ta;
        m_indexes   = _indexes;
    }

    const ITF_VECTOR<Vertex>* m_vertexs;
    const ITF_VECTOR<u32>*    m_indexes;
    u32                 index;
    u32                 taille;

    const Fan& operator = (const Fan& _p)
    {
        m_vertexs   = _p.m_vertexs;
        m_indexes   = _p.m_indexes;
        index       = _p.index;
        taille      = _p.taille;
        return *this;
    }    
};

struct Quad2DInfo
{
    Texture*            m_texture;
    f32                 m_width;
    f32                 m_height;
    u32                 m_color;
    Vec2d               m_uvStart;
    Vec2d               m_uvEnd;
    Vec3d               m_pos2D;
    f32                 m_angle;
    Vec2d               m_pivot;
};

#define DEFAULT_HDIV            2
#define DEFAULT_VDIV            2

#ifndef PATCH32_WORKINGBUFF
# define PATCH32_WORKINGBUFF    0
#endif // PATCH32_WORKINGBUFF

#ifdef VBMANAGER_USE_DYNAMICRING_VB
// Must force this with ring buffer since AnimManager::computeAnimMeshVertex doesn't handle the case
# undef PATCH32_WORKINGBUFF
# define PATCH32_WORKINGBUFF    1
#endif // VBMANAGER_USE_DYNAMICRING_VB

class Patch32Data
{
public:
    Patch32Data(int _vertexFormat, u32 _maxVertex = MAX_VERTEX_BY_MESH);
    ~Patch32Data();

    ITF_Mesh    *mesh;
    union
    {
        void         *vertexList;
        VertexPT     *vertexListPT;
        VertexPCT     *vertexListPCT;
        VertexPatchAnim   *vertexListPatch;
    };
    u32         vertexCount;
    u32         maxVertexListSize;
    int         vertexFormat;
    bbool       paused;

    void init(ITF_Mesh    *_mesh, int _vertexType);
    void start();
    void pause(bbool _pause);
    void stop();
};

/////////////////////////////////////////////////////////////

class Size
{
public:
    Size(void) : d_width(0), d_height(0) {}
    Size(u32 width, u32 height) : d_width(width), d_height(height) {}

    bool operator==(const Size& other) const;
    bool operator!=(const Size& other) const;

    u32 d_width, d_height;
};

struct LOCKED_TEXTURE
{
    i32     m_pitch;
    void*   mp_Bits;
};

struct FluidSimu_Info
{
    bbool   m_useGlow;
    f32     m_glowSize;
    bbool   m_useEmboss;

    FluidSimu_Info()
        :m_useGlow(bfalse)
        ,m_glowSize(16.0f)
        ,m_useEmboss(bfalse)
    {}
};

struct Spline_Info
{
    Spline          m_spline;
    Matrix44        m_mat;
    Texture *       m_texture;
    f32             m_height;
    Vec3d           m_pos;
    f32             m_angle;
    Vec2d           m_scale;
    Color           m_color;

    Spline_Info()
        : m_texture(NULL)
        , m_height(0.1f)
    {
        m_mat.setIdentity();
    }
};

#ifdef WRAPPER_C
#pragma unmanaged
#endif


class FluidToDraw
{
public:
    FluidToDraw()
        : m_heightTabSize(0)
        , m_hdivCount(0)
        , m_pMaterial(NULL)
        , m_delta1(1.f)
        , m_delta2(1.f)
        , m_stepCount(0)
    {
        m_matrix.setIdentity();
    }

    GFX_Vector4 *   getBufferVect() const           { return (GFX_Vector4 *)m_bufferVect; }
    GFX_Vector4 *   getColorVect() const            { return (GFX_Vector4 *)m_colorVect; }

    u32             getHeighTabSize() const         { return m_heightTabSize; }
    void            setHeighTabSize(u32 _size)      { m_heightTabSize = _size; }

    u32             getHDivCount() const            { return m_hdivCount; }
    void            setHDivCount(u32 _size)         { m_hdivCount = _size; }

    f32             getDelta1() const               { return m_delta1; }
    void            setDelta1(f32 _val)             { m_delta1 = _val; }

    f32             getDelta2() const               { return m_delta2; }
    void            setDelta2(f32 _val)             { m_delta2 = _val; }

    f32             getStepCount() const            { return m_stepCount; }
    void            setStepCount(f32 _val)          { m_stepCount = _val; }

     const Matrix44 &getMatrix() const               { return m_matrix; }
     void            setMatrix(const Matrix44 & _m)  { m_matrix = _m; }

    const GFX_MATERIAL  * getMaterialPtr() const    { return m_pMaterial; }
    void            setMaterialPtr(GFX_MATERIAL* _m){ m_pMaterial = _m; }

    GFX_Vector4   & getUV1()                        { return m_uv1; }
    GFX_Vector4   & getUV2()                        { return m_uv2; }
    
    const GFX_Vector4   & getUV1() const            { return m_uv1; }
    const GFX_Vector4   & getUV2() const            { return m_uv2; }

private:
    GFX_MATERIAL *  m_pMaterial;
    GFX_Vector4     m_bufferVect[64 + 2];
    GFX_Vector4     m_colorVect[GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT];
    GFX_Vector4     m_uv1;
    GFX_Vector4     m_uv2;

    u32             m_heightTabSize;
    u32             m_hdivCount;

    Matrix44    m_matrix;
    f32         m_delta1;
    f32         m_delta2;
    f32         m_stepCount;
};
#ifdef WRAPPER_C
#pragma managed
#endif

//=================================================================================================
// Debug structures / classes
//

#define DBGPrim3D_Box    0
#define DBGPrim3D_Arrow  1
#define DBGPrim3D_Circle 2

#ifdef ITF_SUPPORT_DBGPRIM_BOX
struct DBGBox
{
    Vec2d pos, extent;
    f32 angle, r, g, b, duration, z;
    bbool fill;
};
#endif // ITF_SUPPORT_DBGPRIM_BOX

#ifdef ITF_SUPPORT_DBGPRIM_MISC
struct DBGCircle
{
    float x, y, rad, r, g, b, duration, z;
    u32 segsCount;
};

struct DBGSphere
{
    float x, y, rad, r, g, b, duration, z;
    u32 segsCount;
    bbool volume;
    bbool xaxis;
    bbool yaxis;
    bbool zaxis;
};

struct DBGLine
{
    Vec2d p1, p2;
    float r, g, b, duration, size, z, alpha;
};

struct DBG3DLine
{
    Vec3d p1, p2;
    float r, g, b, duration, size, alpha;
};

/**/
struct DBGPrim3D
{
    i32         Type;   
    Matrix44    M;
    f32         r, g, b;
    f32         duration, size;
    bbool       fill;
};

struct DBGTriangle
{
    Vec2d p1, p2, p3;
    float r, g, b, a, duration, z; 

};

struct DBGTriangle3D
{
    Vec3d p1, p2, p3;
    float r, g, b;
};

struct DBGContextInternalCircle
{
    Vec3d   pos;
    u32     color;
    f32     radius;
};

class DBGContextCircle
{
    public:

        static const u32 DefaultSegmentCount = 16;

        DBGContextCircle():segmentCount(DefaultSegmentCount) {};
        void init(u32 _countEstimated,u32 _segmentCount = DefaultSegmentCount);

        ITF_INLINE const DBGContextInternalCircle& getInternalCircle(u32 index)  {return m_InternalCircle[index];}
        ITF_INLINE u32 getSegmentCount()           {return segmentCount;}

        ITF_INLINE u32 getCircleCount()            {return m_InternalCircle.size();}

        void add(const Vec3d& _pos,f32 _radius,const Color& color);


    protected:
        ITF_VECTOR<DBGContextInternalCircle> m_InternalCircle;
    
        u32 segmentCount;

};

struct DBGTextureTriangle
{
    ITF_VECTOR<Vertex>* pVtx;
    float duration, z;
    Texture* t;
};

struct DBGTextureQuad
{
    Vertex mVtx[4];
    float duration, z;
    const Texture* t;
};

struct DBGSpline
{
    Matrix44 M;
    Texture* t;
    const Spline *s;
    float h;
    float duration;
    Color c;
};
#endif // ITF_SUPPORT_DBGPRIM_MISC

#ifdef ITF_SUPPORT_DBGPRIM_TEXT
struct DBGText
{
    String8  m_text;
    float   m_x;
    float   m_y;
    f32     m_w;
    f32     m_h;
    f32     m_r;
    f32     m_g;
    f32     m_b;
    bbool   m_useBigFont;
    bbool	m_useMonospaceFont;
    Color   m_backgroundColor;
    FONT_ALIGNMENT m_alignment;

    DBGText()
        : m_useBigFont(bfalse)
        , m_useMonospaceFont(bfalse)
        , m_x(F32_INFINITY)
        , m_y(F32_INFINITY)
        , m_r(1.0f)
        , m_g(1.0f)
        , m_b(1.0f)
        , m_w(0.0f)
        , m_h(0.0f)
        , m_alignment(FONT_ALIGN_LEFT)
        , m_backgroundColor(0, 0, 0, 0)
    {
    }
};
#endif // ITF_SUPPORT_DBGPRIM_TEXT

struct Circle2D
{
    static const u32 vertexCount = 64 + 1; // duplicate the first point
    
    Circle2D() { m_rank = 0; m_vertexCount = vertexCount; }
    VertexPC m_vertexes[vertexCount];
    u32     m_vertexCount;
    i32     m_rank;
};

struct Box2D
{
    Box2D() {m_rank = 0;}
    VertexPC v[4];
    i32     m_rank;
};

struct Triangle2D
{
    Triangle2D() {m_rank = 0;}
    VertexPC v[3];
    i32     m_rank;
};

typedef u32 UVSubdiv[2];

struct AnimBoneRenderable;

#define GFX_MaxRenderTarget 4

/*
	Render context, described all target used at a time with a viewport.
*/
class RenderContext
{
public:
	RenderContext(RenderTarget *_colorBuffer, RenderTarget *_depthBuffer = nullptr);
	void setViewport(u32 _x, u32 _y, u32 _width, u32 _height, f32 _minZ, f32 _maxZ);

	RenderTarget *m_depthBuffer;
	RenderTarget *m_colorBuffers[GFX_MaxRenderTarget];
	GFX_Viewport  m_viewport;
	u8			  m_colorBufferCount;
	u8			  m_useViewport;
};

class GFXAdapter : public TemplateSingleton<GFXAdapter>
{
    GFXAdapter&operator=(const GFXAdapter&);
public:

    // init/destroy
    virtual void                    init                    () = 0;
    virtual void                    preInitDriver           ();
    virtual void                    postInitDriver          ();
    virtual void                    destroy                 ();
    virtual                         ~GFXAdapter             ();

    float           m_gameUICameraTop;
    float           m_gameUICameraLeft;
    float           m_gameUICameraBottom;
    float           m_gameUICameraRight;
    WorldViewProj   m_worldViewProj;
    Matrix44        m_gameUIProjMatrix;

    bbool           m_requestRestoreGameUI2DCamera;

    // Mask buffer stuff
    void createMaskBuffer();
    void destroyMaskBuffer();

    // Common Matrix
    void                setObjectMatrix(const Matrix44 & _matrix) { m_worldViewProj.setWorld(_matrix); }
    const Matrix44 *    getViewMatrix() { return &m_worldViewProj.getView(); }
	const Matrix44 *    getGameUIProjMatrix() { return &m_gameUIProjMatrix; }

    // engine interface to command render steps
    // These methods are effectively sending draw commands to the gfx device
    virtual void            setCamera               (Camera* _cam);
    virtual void            clear                   (u32 _buffer, float _r=0.f, float _g=0.f, float _b=0.f, float _a=0.f) {ITF_ASSERT(0);}
    void                    drawOccludePrimitives   (View &_view); // draw regular primitives with an alpha test and a specific color code for each pickable
    void                    drawPrimitives          (View &_view); // draw 3D then ZList primitives
    void                    drawSubViewPrimitives   (View &_view);
	void					renderViewportMask      (View &_mainView, View &_subView);
    void                    clearViewportColor      (View &_mainView, View &_subView);
    void                    drawAfterFX             (View &_view); 
    void                    drawBrightnessAfterFX   (View &_view); 
    void                    draw2DPrimitives        (View &_view);
    void                    drawPickingPrimitives   (View &_view, const Vec2d & _pickingXY, u8 _alphaPickRef); // pickable if alpha > _alphaPickRef
    void                    resolvePickingList      (SafeArray<u32> & _pickIdxResult, ux _nbMax);

    virtual bbool           createDevice            (bbool _fullscreen, void *_hwnd) { ITF_ASSERT(0); return bfalse; }
    virtual void *          getRenderingDevice      () { ITF_ASSERT(0); return NULL; }
    virtual void            setFullScreenMode       (bbool _bMode) { ITF_ASSERT(0); }

#ifdef ITF_SUPPORT_DBGPRIM_ANY
    void                    drawDBGPrimitives       ();
    void                    cleanAllDebugPrimitives ();
#endif // ITF_SUPPORT_DBGPRIM_ANY

    RenderTarget*				m_maskBuffer;
	bbool						m_maskBufferFilled;

	RenderTarget* createTarget(GFXAdapter *_gfxAdapter, u32 _width, u32 height, Texture::PixFormat _pixformat, u32 _flags = 0, u32 _shiftSurfSize = 0, RenderTarget *_pSrcBaseTexture = NULL, RenderTarget *_pLinkedTexture = NULL );

#ifdef GFX_COMMON_POSTEFFECT
    static const u32    LIGHTS_BFR_RATIO;
    static const u32    QRT_LIGHTS_BFR_RATIO;
    static const u32    REFLECTION_BFR_RATIO;
    static const u32    REFRACTION_BFR_RATIO;
    static const u32    FLUID_BLUR_BFR_RATIO;
    static const u32    FLUID_GLOW_BFR_RATIO;

    enum eAdditionnalTexBuffer
    {
        TEX_BFR_FULL_1 = 0,
        TEX_BFR_FULL_2,
        TEX_BFR_QRT_1,
        TEX_BFR_FRONT_LIGHT,
        TEX_BFR_BACK_LIGHT,
        TEX_BFR_QRT_FRONT_LIGHT,
        TEX_BFR_QRT_BACK_LIGHT,
        TEX_BFR_QRT_TMP_LIGHT,
        TEX_BFR_REFLECTION,
        TEX_BFR_FLUID_BLUR,
        TEX_BFR_FLUID_GLOW,

        NB_TEX_BFR
    };
    static const u32 TEX_BFR_REFRACTION = TEX_BFR_REFLECTION;
    static const u32 TEX_BFR_QRT_AFX_1  = TEX_BFR_FRONT_LIGHT;
    static const u32 TEX_BFR_QRT_AFX_2  = TEX_BFR_BACK_LIGHT;

    enum eAdditionnalExternalTexBuffer
    {
        TEX_EXT_BFR_IMPOSTOR_COPY,
        TEX_EXT_BFR_SCENE_COPY,

        NB_TEX_EXT_BFR
    };

	//  Additional RT
    RenderTarget*				m_AdditionnalRenderingBuffer[NB_TEX_BFR];
    RenderTarget*				m_AdditionnalExternalRenderingBuffer[2][NB_TEX_EXT_BFR];
    RenderTarget*				m_DebugRenderingBuffer;

    RenderTarget*				m_MediumZBuffer;

	RenderTarget				*m_currentAFXBlurTexture;
    RenderTarget				*m_currentAFXSceneTexture;
    f64                         m_AFXTimer;

	f32							m_addGlobalContrast;
	f32							m_addGlobalBright;
	f32							m_addGlobalSaturation;
    f32							m_addSceneAndUIBright;

	#ifndef ITF_FINAL
	protected:
		String8 m_dbgTxtAfterFX;
	public:
	#endif // ITF_FINAL

	void createAdditionalRenderTarget();
    void releaseAdditionalRenderTarget();
	virtual void getBackBufferAfterOffset(u32 &_afterBackEDRAMOffset, u32 &_afterLightEDRAMOffset);
	virtual u32 getBufferSizeOffset(RenderTarget *_target) {return 0;}

#ifdef ITF_X360
    virtual void setShaderGPRAllocation(u32 VertexShaderCount, u32 PixelShaderCount) {}
#else
    void setShaderGPRAllocation(u32 VertexShaderCount, u32 PixelShaderCount) {}
#endif

    virtual void                setBackBuffer(bbool _setZbuffer = btrue, bbool _restoreBackBuffer = bfalse) { ITF_ASSERT(0); }

    void BeginAFXPostProcess(bbool /*_nocopy*/);
    void EndAFXPostProcess(bbool /*_nocopy*/, GFX_BLENDMODE /*_finalBlend*/);
    void AFXPostProcessProcessAllowCombinatory(AFXPostProcess &_postProcess);
    bbool isBlendingValue(AFXPostProcess &_postProcess);
	virtual RenderTarget*getSceneTexture(u32 _rt = 0, f32 _scale = 1.0f, bbool _use2x2Target = bfalse) {ITF_ASSERT(0);return NULL;}
    virtual void        PrepareBlurTarget(RenderPassContext & _rdrCtxt, f32 _pixelBlurSize, u32 _quality);
    void                AFXpostProcessPass(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess);
    void                AFXPostProcessKaleiPass(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess);
    void                BlurRenderBuffer(RenderPassContext & _rdrCtxt, f32 _pixelBlurSize, u32 _quality, RenderTarget* _srcTex,
                                         RenderTarget* _dstTex, RenderTarget* _tmpTex = NULL);
    void                fillHexagoneVertexBuffer(VertexPCT* _data, u32 _startIndex, Vec2d _position, f32 _hexagoneSize, const Vec2d& _uv1, const Vec2d& _uv2, const Vec2d& _uv3 );

	virtual void        DrawAFXPostProcess(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess );
	virtual void        blurFluidRenderBuffer( RenderPassContext & _rdrCtxt );
    virtual void        blurFrontLightRenderBuffer( RenderPassContext & _rdrCtxt, u32 _quality, u32 _size );
    virtual void        blurBackLightRenderBuffer( RenderPassContext & _rdrCtxt, u32 _quality, u32 _size );
    virtual void        restoreBackLightRenderBuffer( RenderPassContext & _rdrCtxt, eLightTexBuffer _currentLightBufferType);

    RenderTarget*		getBufferTexture(u32 _id) const
	{
		return m_AdditionnalRenderingBuffer[_id];
	}
    RenderTarget*		getExternalBufferTexture(u32 _view, u32 _id) const
	{
		return m_AdditionnalExternalRenderingBuffer[_view][_id];
	}
	virtual void copyTexture( RenderPassContext & _rdrCtxt, RenderTarget *_source, RenderTarget *_destination);
    RenderTarget*		getFrontLightRenderBuffer(eLightTexBuffer _currentLightBufferType) const;
    RenderTarget*		getBackLightRenderBuffer(eLightTexBuffer _currentLightBufferType) const;
#else
    virtual void        DrawAFXPostProcess(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess ) {ITF_ASSERT(0);}

	virtual void        blurFluidRenderBuffer( RenderPassContext & _rdrCtxt ) {ITF_ASSERT(0);}
    virtual void        blurFrontLightRenderBuffer( RenderPassContext & _rdrCtxt, u32 _quality, u32 _size ) {ITF_ASSERT(0);}
    virtual void        blurBackLightRenderBuffer( RenderPassContext & _rdrCtxt, u32 _quality, u32 _size ) {ITF_ASSERT(0);}
    virtual void        restoreBackLightRenderBuffer( RenderPassContext & _rdrCtxt, eLightTexBuffer _currentLightBufferType) {ITF_ASSERT(0);}
#endif
    virtual void        resolve(RenderTarget* _target) {}

#ifdef ITF_USE_REMOTEVIEW
    virtual Vec2d&          transformScreenToRemoteViewCordinate(Vec2d& _pos) { ITF_ASSERT(0); return _pos; }
#endif // ITF_USE_REMOTEVIEW

    // the default clear color is restored at each frame
    void                    setDefaultClearColor(const Color & _clearColor) { m_defaultClearColor = _clearColor; }

    // the default clear color is restored at each frame
    const Color &           getDefaultClearColor() const { return m_defaultClearColor; }

    // the clear color can be changed each frame (for instance by a component)
    void                    setClearColor(const Color & _clearColor);

    // state stuff
    i32                     getDebugFontSize        () const;
    i32                     getBigDebugFontSize     () const;

    // 
    virtual void            coord2DTo3D(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool _noTranslation = btrue) {ITF_ASSERT(0);}
	virtual void            compute3DTo2DViewport(const Vec3d& _in, Vec3d& _out) { compute3DTo2D(_in, _out); }
    virtual void            compute3DTo2D(const Vec3d& _in, Vec3d& _out) {ITF_ASSERT(0);}
    virtual void            compute2DTo3D(const Vec3d& _in, Vec3d& _out) {ITF_ASSERT(0);}
    virtual WorldViewProj*  getWorldViewProjMatrix() {return NULL;}
    virtual void            updateShaderScreenDimension(u32 _width, u32 _height){ITF_ASSERT(0);}

    // methods to perform direct rendering
    // -> not supposed to be used except by specific modules
    // -> will probably be refactored.
    virtual void                    drawScreenQuad          (DrawCallContext &_drawCallCtx, f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex = bfalse)   {ITF_ASSERT(0);}

    // Helpers/tools
    // -> should be moved somewhere else
    static void                            GFX_computeMatrixLookAtRH                   (   Matrix44* _view, Vec3d* _vEyePt, Vec3d* _vLookatPt, Vec3d* _vUpVec );
    static void                            GFX_computeMatrixPerspectiveFovRH           (   Matrix44* _proj, f32 _fov, f32 _aspect, f32 _zn, f32 _zf   );
    static void                            GFX_computeMatrixOrthoOffCenterRH           (   Matrix44* _proj, f32 _l, f32 _r, f32 _b, f32 _t, f32 _zn, f32 _zf );

// protected :

    // methods over ridable per platform

#if !defined(ITF_WII) || !defined(ITF_FINAL)
    virtual void                    project                 (   const Vec3d& /*_in3d*/, Vec2d & /*_out2d*/) {ITF_ASSERT(0);}
#endif // !defined(ITF_WII) || !defined(ITF_FINAL)

#ifdef ITF_SUPPORT_DBGPRIM_BOX
    virtual void                    drawBox                 (   DrawCallContext &_drawCallCtx, const Vec2d& /*_pos*/, f32 /*_angle*/, const Vec2d& /*_extent*/, f32 /*_r*/=1, f32 /*_g*/=1, f32 /*_b*/=1, float /*_z*/ = 0.f, bbool /* _fill */ = bfalse ) {ITF_ASSERT(0);}
#endif // ITF_SUPPORT_DBGPRIM_BOX

#ifdef ITF_SUPPORT_DBGPRIM_MISC
    // immediate draw (do not call)
    virtual void                    drawSphere              (   DrawCallContext &_drawCallCtx, float /*_x*/, float /*_y*/, float /*_z*/, float /*_radius*/, int /*lats*/, int /*longs*/, int /*_wireFrame*/, float /*_r*/=1, float /*_g*/=1, float /*_b*/=1  ) {ITF_ASSERT(0);}
    virtual void                    drawSphere              (   DrawCallContext &_drawCallCtx, float /*_x*/, float /*_y*/, float /*_radius*/, float /*_r*/=1, float /*_g*/=1, float /*_b*/=1, float /*_z*/ = 0.f, u32 /*_segsCount*/ = 16, bbool _volume = btrue, bbool _xaxis = btrue, bbool _yaxis = btrue, bbool _zaxis = btrue ){ITF_ASSERT(0);}
    virtual void                    drawCircle              (   DrawCallContext &_drawCallCtx, float /*_x*/, float /*_y*/, float /*_radius*/, float /*_r*/=1, float /*_g*/=1, float /*_b*/=1, float /*_z*/ = 0.f, u32 /*_segsCount*/ = 16  ){ITF_ASSERT(0);}
    virtual void                    drawCircleContext       (   DrawCallContext &_drawCallCtx, DBGContextCircle* /*_pContext*/  ){ITF_ASSERT(0);}
    virtual void                    drawPrim3D              (   DrawCallContext &_drawCallCtx, i32 /*_type*/, const Matrix44& /*M*/, float /*_r*/=1, float /*_g*/=1, float /*_b*/=1, float /*_alpha */ = 1.f, bbool /*_fill*/ = btrue ) {};
    DBGContextCircle*               getDBGContextCircle();
#endif // ITF_SUPPORT_DBGPRIM_MISC

    virtual void                    drawTriangle            (   DrawCallContext &_drawCallCtx, const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, const Vec2d& /*_p3*/, float /*_r*/=1, float /*_g*/=1, float /*_b*/=1, float /*_a*/=1, float /*_z*/ = 0.f    ) {ITF_ASSERT(0);}
    virtual void                    drawTriangle            (   DrawCallContext &_drawCallCtx, const Vertex& /*_v1*/, const Vertex& /*_v2*/, const Vertex& /*_v3*/, const Texture* /*_tex*/) {ITF_ASSERT(0);}
    virtual void                    drawQuad2D              (   DrawCallContext &_drawCallCtx, f32 /*_px*/, f32 /*_py*/, f32 /*_width*/, f32 /*_height*/, f32 /*_z*/, f32 /*_angle */ = 0.f, const Vec2d & _pivot = Vec2d::Zero, u32 /*_color*/ = COLOR_WHITE , const Texture* /*_tex*/ = NULL, const Vec2d* /*_uvStart */= NULL,  const Vec2d* /*_uvEnd */= NULL) {ITF_ASSERT(0);}
    virtual void                    draw3DQuad              (   DrawCallContext &_drawCallCtx, Quad3DPrim* /*_t_Quad*/ ) {ITF_ASSERT(0);}
    virtual void                    drawFrameBlendQuad      (   DrawCallContext &_drawCallCtx, QuadFrameBlendPrim* /*_t_Quad*/ ) {ITF_ASSERT(0);}

    //////////////////////////////////////////////////////////////////////////

    virtual bbool                   loadTexture             (   Texture* /*_tex*/, const Path& /*_path*/) {ITF_ASSERT(0);return bfalse;}

	virtual void					restoreGameUI2DCamera	() {};
	virtual void					setGameUI2DCamera		( float _left, float _right, float _top, float _bottom ) {}

	virtual void                    destroyResources();
    virtual void                    init2DRender            (   )   {ITF_ASSERT(0);}
    virtual void                    end2DRender             (   )   {}
	virtual void					initGameUI2DRender		(   )   {ITF_ASSERT(0);}


    virtual void                    setOrthoView            (   f32 /*_l*/, f32 /*_r*/, f32 /*_b*/, f32 /*_t*/  );

    virtual void                    applyDrawCallState      (   DrawCallStateCache & _statecache, DrawCallStateCache & _restoreState, bbool _force){ITF_ASSERT(0);}
    virtual void                    restoreDrawCallState    (   DrawCallStateCache & _statecache, DrawCallStateCache & _restoreState, bbool _force){ITF_ASSERT(0);}
    virtual void                    SetDrawCallState        (   DrawCallStateCache & _statecache, u32 _stateFlags, bbool _force){ITF_ASSERT(0);}

    virtual void                    setAlphaBlend           (   GFX_BLENDMODE _blend, GFX_ALPHAMASK _alphaMask  )   {ITF_ASSERT(0);}
    virtual void                    setCullMode             (   GFX_CULLMODE /*_cull*/    )  {ITF_ASSERT(0);}
    virtual void                    setFillMode             (   GFX_FILLMODE /*_fillmode*/  ) {ITF_ASSERT(0);}

    virtual void                    depthSet                (   u32 _depthTest, u32 _depthMask )   {depthTest(_depthTest);depthMask(_depthMask);}
    virtual void                    depthSet                (   u32 _depthTest, u32 _depthMask, f32 _biasOffset, f32 _biasSlope )   {depthTest(_depthTest);depthMask(_depthMask);depthBias(_biasOffset,_biasSlope);}
    virtual void                    depthSet                (   u32 _depthTest, u32 _depthMask, u32 _depthFunc, f32 _biasOffset, f32 _biasSlope )   {depthTest(_depthTest);depthMask(_depthMask);depthFunc(_depthFunc);depthBias(_biasOffset,_biasSlope);}
    virtual void                    depthMask               (   u32 /*_depthMask*/    )   {ITF_ASSERT(0);}
    virtual void                    depthFunc               (   u32 /*_func*/    )   {ITF_ASSERT(0);}
    virtual void                    depthTest               (   u32 /*_test*/    )   {ITF_ASSERT(0);}
    virtual void                    colorMask               (   GFX_COLOR_MASK_FLAGS _colorMask = GFX_COL_MASK_ALL )   {ITF_ASSERT(0);}
    virtual void                    depthBias               (   f32 _offset , f32 slope )   {}

    float                           getSizeNextPOT          (   float sz    ) const;
    virtual void                    setScissorRect          (   GFX_RECT* /*_clipRect*/    )   {ITF_ASSERT(0);}
    virtual void                    lockTexture             (   Texture* /*_tex*/, LOCKED_TEXTURE* /*_lockTex*/, u32 /*_flag*/ = 0 )   {ITF_ASSERT(0);}
    virtual void                    unlockTexture           (   Texture* /*_tex*/   )   {ITF_ASSERT(0);}
    virtual void                    setClipPlane            (   bbool _enable, f32 _ax = 0.0f, f32 _by = 0.0f, f32 _cz = 0.0f, f32 _dw = 0.0f ) {ITF_ASSERT(0);}
    virtual void                    cleanupTexture          (   Texture* /*_texture*/   ) {ITF_ASSERT(0);}

    virtual void                    DrawPrimitive           (   DrawCallContext &_drawCallCtx, PRIMITIVETYPE /*_type*/, const void * /*_p_Vertex*/, u32 /*_NumberVertex*/    ){ITF_ASSERT(0);}

    // Matrix.
    virtual void                    setMatrixTransform      (const Matrix44 & /*_matrix*/    ){ITF_ASSERT(0);} // set world matrix
    virtual void                    setupViewport           (    GFX_RECT* /*_Viewport*/    ) {ITF_ASSERT(0);}
#if !defined(ITF_WII) || !defined(ITF_FINAL)
    virtual void                    unprojectPoint          (    const Vec2d& /*_in*/, float /*_viewDistance*/, const Matrix44* /*_MatrixModel*/, const Matrix44* /*_MatrixProj*/, const int* /*_View*/, Vec2d& /*_out*/) {ITF_ASSERT(0);}
    virtual void                    setLookAtMatrix         (    Matrix44* /*_MatrixOut*/, double /*_midx*/, double /*_midy*/, float /*_viewDistance*/, float /*_aspect*/) {ITF_ASSERT(0);}
#endif // !defined(ITF_WII) || !defined(ITF_FINAL)
    virtual void                    getViewport             (    struct GFX_Viewport & _viewPort) {ITF_ASSERT(0);}

	virtual void					setGlobalLighting		(View &_view)  {}

    /// GfxMaterials.
    virtual bbool                   setGfxMaterial          (   DrawCallContext &_drawCtx, const GFX_MATERIAL& /*_gfxMat*/ ) {ITF_ASSERT(0); return bfalse; }
    virtual bbool                   setGfxMatDefault        (   DrawCallContext &_drawCtx  ) { ITF_ASSERT(0); return bfalse; }
    void                            prepareGfxMatAlphaFade      (   const GFX_MATERIAL& _gfxMat );

    /// Texture.
    virtual void                    createTexture           (   Texture* /*_texture*/, u32 /*_sizeX*/, u32 /*_sizeY*/, u32 /*_mipLevel*/, Texture::PixFormat /*_pixformat*/, u32 /*_pool*/ = 1, bbool /*_dynamic*/ = bfalse )  {ITF_ASSERT(0);}
    virtual void                    cleanupBaseTexture      (   Texture* /*_texture*/   ) {ITF_ASSERT(0);}
    virtual void                    setTextureSize          (   Texture* _texture, const Size& _size   );
    virtual unsigned char*          grabTexture             (   Texture* /*_texture*/   ) {ITF_ASSERT(0);return NULL;}
    virtual u32                     getMaxTextureSize       (   ) {ITF_ASSERT(0);return 0;}
    virtual void                    loadFromMemory          (   const void* /*_buffer*/, const Size& /*_texture_size*/, Texture* /*_texture*/, Texture::PixFormat /*_pixel_format*/, bbool /*bCooked*/,u32 _mipmapCount)  {ITF_ASSERT(0);}
    virtual bbool                   loadFromFileInMemory    (   Texture *_texture, const void* _buffer, u32 _contentSize) {ITF_ASSERT(0);return bfalse;}
    virtual void                    SetTexture              (   u32 _sampler, const Texture * _texture, bbool _linearFiltering = btrue) {ITF_ASSERT(0);}
    virtual void                    saveToMemory            (    void* /*_buffer*/, Texture* /*_Tex*/ ) {ITF_ASSERT(0);}
    const   Vec2d&                  getTexelScaling         (   ) const;
    void                            updateCachedScaleValues (   Texture* _Tex  );
    virtual float                   getTexelOffset          (   ) {ITF_ASSERT(0);return 0.0f;}

	// create/release generic textures, like white, black, etc.
    void							createTextureMonoChrome(Texture & _tex, const Color & _color);
    void							createGenericTextures();
    void							releaseGenericTextures();

	Texture*						getWhiteOpaqueTexture() const { return m_whiteOpaqueTexture; }
    Texture*						getBlackOpaqueTexture() const { return m_blackOpaqueTexture; }
    Texture*						getBlackTranspTexture() const { return m_blackTranspTexture; }
    Texture*						getGreyOpaqueTexture()  const { return m_greyOpaqueTexture; }

protected:
	Texture*	m_whiteOpaqueTexture;
	Texture*	m_blackOpaqueTexture;
	Texture*	m_blackTranspTexture;
	Texture*	m_greyOpaqueTexture;

public:
    virtual void                    setSamplerState         (   u32 /*_samp*/, GFX_SAMPLERSTATETYPE /*_type*/, i32 /*_value*/ ) {ITF_ASSERT(0);}
    virtual void                    fillTexture             (   Texture* _Tex, u8 * _buffer, u32 _pitch, u32 _height ) {}

    // New texture Target.   
    virtual RenderTarget*           createRenderTarget      (   RenderTargetInfo &_info )  {ITF_ASSERT(0); return NULL;}
    virtual void					setRenderContext		(   RenderContext &_context )  {ITF_ASSERT(0);}
    virtual void					prepareLockableRTAccess     (	RenderTarget* /*_target*/)  {ITF_ASSERT(0);}
    virtual void					lockRenderTarget        (	RenderTarget* /*_target*/, LOCKED_TEXTURE * /*_lockTex*/)  {ITF_ASSERT(0);}
    virtual void					unlockRenderTarget      (	RenderTarget* /*_target*/)  {ITF_ASSERT(0);}


    virtual void                    present                 (   ) {ITF_ASSERT(0);}

    virtual void*                   enableTextureDyn        ( TextureDyn* /*_texture*/ ) {ITF_ASSERT(0);return NULL;}
    virtual void                    disableTextureDyn       ( TextureDyn* /*_texture*/, void* /*_oldSurf*/ ) {ITF_ASSERT(0);}
    virtual void                    getTextureDynSurfaceLevel(TextureDyn* _texture ) {ITF_ASSERT(0);}
    virtual void                    releaseTextureDynSurfaceLevel(TextureDyn* _texture) {ITF_ASSERT(0);}

    /*
    // The rendering part of the game loop should look like that :
    GFX_ADAPTER->beginDisplay();
    ... addPrimitive ...  // fill primitives list
    for all view
        GFX_ADAPTER->beginViewDisplay();
            GFX_ADAPTER->beginViewportRendering(); // start kicking primitives to GPU)
                GFX_ADAPTER->drawPrimitives(); 
                GFX_ADAPTER->drawAfterFX();
                GFX_ADAPTER->drawDBGPrimitives();
                GFX_ADAPTER->draw2DPrimitives();
            GFX_ADAPTER->endViewportRendering();
        GFX_ADAPTER->endViewDisplay();
    GFX_ADAPTER->endDisplay();
    GFX_ADAPTER->present();
    */

    virtual void                    beginDisplay          ( f32 _dt );
    virtual void                    endDisplay            (   );

    virtual void                    beginViewDisplay      (   );
    virtual void                    endViewDisplay        (   );

    virtual void                    beginViewportRendering  ( View &_view  );
    virtual void                    endViewportRendering    (  View &_view );

    virtual BasePlatformTexture *	getFrontLightTexture(eLightTexBuffer _currentLightBufferType)  const { return NULL; }
    virtual BasePlatformTexture *	getBackLightTexture(eLightTexBuffer _currentLightBufferType)  const { return NULL; }

private:
    virtual void                    beginSceneRendering   ( View &_view  ) {};
    virtual void                    endSceneRendering     ( View &_view  ) {};
    
public:
    virtual bbool                   waitPreviousGPUframeEnd (   ) {ITF_ASSERT(0); return bfalse;}
    virtual void                    syncGPU                 (   ) {ITF_ASSERT(0);}

    virtual bbool                   setScreenProtection(bbool _enable) { return btrue; }

    GFXAdapter();
    bbool                           getSceneResolvedFlag()const         { return m_SceneAlreadyResolve; }
    void                            setSceneResolvedFlag(bbool _val)    { m_SceneAlreadyResolve = _val; }
    void                            resetSceneResolvedFlag()            { m_SceneAlreadyResolve = bfalse; }

    // tool/helper for platform specific code
    void                            vec3dProject            (   Vec3d* _out, const Vec3d* _v, const GFX_Viewport* _viewp, const Matrix44* _ViewProj);
    void                            vec3dUnProject          (   Vec3d* _out, const Vec3d* _v, const GFX_Viewport* _viewp, const Matrix44* _ViewProj_InvT);

#ifdef ITF_SUPPORT_DBGPRIM_TEXT
    bbool                           drawDBGText             (   const String8 & _text, float _x = ITF_POS2D_AUTO, float _y = ITF_POS2D_AUTO, f32 _r = 1.f, f32 _g = 1.f, f32 _b = 1.f, bbool _useBigFont = bfalse, FONT_ALIGNMENT _alignment=FONT_ALIGN_LEFT, bbool _useMonospaceFont = bfalse, Color _backgroundColor = Color(0,0,0,0), f32 _w = -1.f, f32 _h = -1.f);
    bbool                           drawDBGTextFixed        (   const String8 & _text, float _x = ITF_POS2D_AUTO, float _y = ITF_POS2D_AUTO, f32 _r = 1.f, f32 _g = 1.f, f32 _b = 1.f, FONT_ALIGNMENT _alignment=FONT_ALIGN_LEFT );
    ITF_INLINE bbool                drawDBGText             (   const String8 & _text, const Color& _color, float _x = ITF_POS2D_AUTO, float _y = ITF_POS2D_AUTO, FONT_ALIGNMENT _alignment = FONT_ALIGN_LEFT, f32 _w = -1.f, f32 _h = -1.f)
        { return drawDBGText(_text, _x, _y, _color.m_r, _color.m_g, _color.m_b, bfalse, _alignment, bfalse, Color(0,0,0,0), _w, _h); }
    virtual bbool                   drawDBGTextSize         (   const String8 &_text, f32 &_width, f32 &_height, bbool _useBigFont = bfalse, bbool _useFixedFont = bfalse ) { return bfalse; }
#endif // ITF_SUPPORT_DBGPRIM_TEXT

#ifdef ITF_SUPPORT_DBGPRIM_BOX
    void                            drawDBGBox              (   const Vec2d& _pos, f32 _angle, const Vec2d& _extent, float _r=1, float _g=1, float _b=1, float _duration = 0.f, float _z = 0.f, bbool _fill = bfalse );
    ITF_INLINE void                 drawDBGBox              (   const Vec2d& _pos, f32 _angle, const Vec2d& _extent, const Color& _color, float _duration = 0.f, float _z = 0.f, bbool _fill = bfalse ) { drawDBGBox(_pos, _angle, _extent, _color.m_r, _color.m_g, _color.m_b, _duration, _z, _fill); }
    void                            drawDBG2dBox            (   const Vec2d& _p, f32 _width, f32 _height, u32 _color0 = COLOR_WHITE, u32 _color1 = COLOR_WHITE, u32 _color2 = COLOR_WHITE, u32 _color3 = COLOR_WHITE, i32 _rank = 0);
#endif // ITF_SUPPORT_DBGPRIM_BOX

#ifdef ITF_SUPPORT_DBGPRIM_MISC
    void                            drawDBGCircle           (   float _x, float _y, float _radius, float _r=1, float _g=1, float _b=1, float _duration = 0.f, float _z = 0.f, u32 _segsCount = 16);
    ITF_INLINE void                 drawDBGCircle           (   const Vec2d& _pos, float _radius, float _r=1, float _g=1, float _b=1, float _duration = 0.f, u32 _segsCount = 16) { drawDBGCircle(_pos.x(), _pos.y(), _radius, _r, _g, _b, _duration, 0.f, _segsCount); }
    ITF_INLINE void                 drawDBGCircle           (   const Vec3d& _pos, float _radius, float _r=1, float _g=1, float _b=1, float _duration = 0.f, u32 _segsCount = 16) { drawDBGCircle(_pos.x(), _pos.y(), _radius, _r, _g, _b, _duration, _pos.z(), _segsCount); }
    void                            drawDBGSphere           (   float _x, float _y, float _radius, float _r=1, float _g=1, float _b=1, float _duration = 0.f, float _z = 0.f, u32 _segsCount = 16, bbool _volume = btrue, bbool _xaxis = btrue, bbool _yaxis = btrue, bbool _zaxis = btrue );
    void                            drawDBGPrim3D           (   i32 _type, const Matrix44 &M, float _r, float _g, float _b, float _duration = 0.f, bbool _fill = true);
    void                            drawDBGLine             (   const Vec2d& _p1, const Vec2d& _p2, float _r = 1, float _g = 1, float _b = 1, float _duration = 0.f, float _size = 1.f, float _z =0.f, float _alpha = 1.f );
    void                            drawDBG3DLine           (   const Vec3d& _p1, const Vec3d& _p2, float _r = 1, float _g = 1, float _b = 1, float _duration = 0.f, float _size = 1.f, float _alpha = 1.f);
    void                            drawDBGTriangle         (   const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r=1, float _g=1, float _b=1, float _a=1, float _duration = 1.f, float _z = 0.0f );
    void                            drawDBGAABB             (   const AABB& _aabb, float _r = 1, float _g = 1, float _b = 1, float _duration = 0.f, float _size = 1.f, float _z = 0.f );
    void                            drawDBGSpline           (   const Spline_Info* _splineInfo, float _duration = 0.f);

    void                            drawDBGArrow            (   const Vec2d& pos1, const Vec2d& pos2, float _r, float _g, float _b, float _rad, float _z = 0.0f, float _duration = 0.f, bbool _forceRad = bfalse  );
    void                            drawDBGArrow            (   const Vec3d& pos1, const Vec3d& pos2, float _r, float _g, float _b, float _rad, float _duration = 0.f, bbool _forceRad = bfalse  );
    ITF_INLINE void                 drawDBGArrow            (   const Vec2d& pos1, const Vec2d& pos2, Color _color = Color::white(), float _rad = 0.1f, float _z = 0.f, float _duration = 0.f, bbool _forceRad = bfalse ) { drawDBGArrow(pos1, pos2, _color.m_r, _color.m_g, _color.m_b, _rad, _z, _duration, _forceRad ); }
    ITF_INLINE void                 drawDBGArrow            (   const Vec3d& pos1, const Vec3d& pos2, Color _color = Color::white(), float _rad = 0.1f, float _duration = 0.f, bbool _forceRad = bfalse ) { drawDBGArrow(pos1, pos2, _color.m_r, _color.m_g, _color.m_b, _rad, _duration, _forceRad ); }

    
    // Not very efficient debugging functions
    void                            drawDBGTextureTriangle  (   ITF_VECTOR<Vertex>* _pVtx, Texture* _tex, float _duration = 0.f, float _z = 0.0f );
    void                            drawDBGQuad             (   const Vec2d& _pos, f32 _width, f32 _height, const Texture * _tex, f32 _angle = 0.0f, float _duration = 0.f, float _z = 0.0f, int _indexInAtlas=0, bbool _flipX=bfalse, bbool _flipY=bfalse, u32 _color = COLOR_WHITE  );
    
    DBGLine*                        getDBGLineBuffer        (   u32 _count );
    DBG3DLine*                      getDBG3DLineBuffer      (   u32 _count );

    // Color overloads
    ITF_INLINE void                 drawDBGCircle           (   float _x, float _y, float _radius, const Color& _color, float _duration = 0.f, float _z = 0.f, u32 _segsCount = 16) { drawDBGCircle(_x, _y, _radius, _color.m_r, _color.m_g, _color.m_b, _duration, _z, _segsCount); }
    ITF_INLINE void                 drawDBGCircle           (   const Vec2d& _pos, float _radius, const Color& _color, float _duration = 0.f, u32 _segsCount = 16) { drawDBGCircle(_pos.x(), _pos.y(), _radius, _color.m_r, _color.m_g, _color.m_b, _duration, 0.f, _segsCount); }
    ITF_INLINE void                 drawDBGCircle           (   const Vec3d& _pos, float _radius, const Color& _color, float _duration = 0.f, u32 _segsCount = 16) { drawDBGCircle(_pos.x(), _pos.y(), _radius, _color.m_r, _color.m_g, _color.m_b, _duration, _pos.z(), _segsCount); }
    ITF_INLINE void                 drawDBGLine             (   const Vec2d& _p1, const Vec2d& _p2, const Color& _color, float _duration = 0.f, float _size = 1.f, float _z = 0.f ) { drawDBGLine(_p1, _p2, _color.m_r, _color.m_g, _color.m_b, _duration, _size, _z, _color.m_a); }
    ITF_INLINE void                 drawDBG3DLine           (   const Vec3d& _p1, const Vec3d& _p2, const Color& _color, float _duration = 0.f, float _size = 1.f) { drawDBG3DLine(_p1, _p2, _color.m_r, _color.m_g, _color.m_b, _duration, _size, _color.m_a); }
    ITF_INLINE void                 drawDBGTriangle         (   const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, const Color& _color, float _duration = 0.f, float _z = 0.0f ) { drawDBGTriangle(_p1, _p2, _p3, _color.m_r, _color.m_g, _color.m_b, _color.m_a, _duration, _z); }
    ITF_INLINE void                 drawDBGAABB             (   const AABB& _aabb, const Color& _color, float _duration = 0.f, float _size = 1.f, float _z = 0.f ) { drawDBGAABB(_aabb, _color.m_r, _color.m_g, _color.m_b, _duration, _size, _z); }
    
    // batched 2D primitives
    void                            drawDBG2dLine           (   const Vec2d& _p1, const Vec2d& _p2, float _size, u32 _startColor, u32 _endColor);
    void                            drawDBG2dLine           (   const Vec2d& _p1, const Vec2d& _p2, float _size=1.f, u32 _color = COLOR_WHITE);
    void                            drawDBG2dTriangle       (   const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, u32 _color = COLOR_WHITE, i32 _rank = 0);
    void                            drawDBG2dCircle         (   const Vec2d& _center, f32 _radius, u32 _color0 = COLOR_WHITE, i32 _rank = 0);
#endif // ITF_SUPPORT_DBGPRIM_MISC

    /// extended primitives.

    bbool                           drawPatch32PT(Patch32Data *pData, Texture *texture, const Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, f32 _z);
    bbool                           drawPatch32PCT(Patch32Data *_pData, Texture *texture, Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, ColorInteger *_color, f32 _z);
   // bbool                           drawPatch32PCTSIMD(Patch32Data *_pData, Texture *texture, Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, Color *_color, f32 _z);
    bbool                           drawPatch32PCTOneColor(Patch32Data *_pData, Texture *texture, Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, u32 _generalColor, f32 _z);

    bbool                           drawBezierPatch32PT(Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, const f32* _widths, const f32* _widths2, const f32* _alphas, u32 _hdiv, u32 _vdiv, f32 _z);
    bbool                           drawBezierPatch32PCT(Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, const f32* _widths, const f32* _widths2, const f32* _alphas, u32 _hdiv, u32 _vdiv, const ColorInteger *_color, f32 _z);
    bbool                           drawBezierPatch32PCTOneColor(Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, const f32* _widths, const f32* _widths2, const f32* _alphas, u32 _hdiv, u32 _vdiv, u32 _generalColor, f32 _z);

    //  The size of the current View
    ITF_INLINE u32                  getScreenWidth          (   ) const                 {   return m_screenWidth;}
    ITF_INLINE u32                  getScreenHeight         (   ) const                 {   return m_screenHeight;}
   
    //  The size of the Final Windows/TV
    ITF_INLINE u32                  getWindowWidth          (   ) const                 {   return m_windowWidth;}
    ITF_INLINE u32                  getWindowHeight         (   ) const                 {   return m_windowHeight;}

	//  The offset of View port to screen
	u32								getViewPortOffsetX      (   ) const;
	u32								getViewPortOffsetY      (   ) const;

    ITF_INLINE const AABB &         getScreenAABB           (   ) const                 {   return m_screenAABB; }         
    void                            setResolution           (   u32 _windowWidth, u32 _windowHeight, bbool _forceReset = bfalse	);
    void                            setWaitVBL              (   bbool _waitVBL  ) {m_waitVBL = _waitVBL;}
    void                            forceScreenSize         (   u32 _screenWidth, u32 _screenHeight ) {m_screenWidth = _screenWidth; m_screenHeight = _screenHeight;}               

#ifdef ITF_WINDOWS
    virtual void		            getAvailableResolutions (   vector<pair<u32, u32>>& _resolutions ) {}
#endif //ITF_WINDOWS

#ifndef ITF_WII
    virtual void                    createDebugFonts        (   u32 _screenwidth, u32 _screenHeight ) {}

    virtual bbool                   isDeviceLost            (   ) {ITF_ASSERT(0);return bfalse;}
    virtual bbool                   resetDevice             (   ) { return btrue; }
#endif // ITF_WII

#ifdef ITF_USE_REMOTEVIEW

    void                            setCurrentEngineView    (   u32 _viewId  ) { m_currentEngineView = _viewId; }
    u32                             getCurrentEngineView    (   ) const { return m_currentEngineView; }
    void                            setCurrentDisplayingView(   u32 _viewFlag  ) { m_displayingEngineView = _viewFlag; }
    u32                             getCurrentDisplayingView(   ) { return m_displayingEngineView; }
    void                            setDisplayingView       ( engineView _view);
#endif

    virtual ITF_INLINE u32          getRemoteScreenWidth    (   ) const { return getScreenWidth(); }
    virtual ITF_INLINE u32          getRemoteScreenHeight   (   ) const { return getScreenHeight();}
    virtual ITF_INLINE u32          getLightPassWidth     (   ) const {ITF_ASSERT(0);return 0;}
    virtual ITF_INLINE u32          getLightPassHeight     (   ) const {ITF_ASSERT(0);return 0;}
    virtual ITF_INLINE u32          getQrtLightPassWidth     (   ) const {ITF_ASSERT(0);return 0;}
    virtual ITF_INLINE u32          getQrtLightPassHeight     (   ) const {ITF_ASSERT(0);return 0;}
    virtual ITF_INLINE u32          getReflectionPassWidth     (   ) const {ITF_ASSERT(0);return 0;}
    virtual ITF_INLINE u32          getReflectionPassHeight     (   ) const {ITF_ASSERT(0);return 0;}
    virtual ITF_INLINE u32          getRefractionPassWidth     (   ) const {ITF_ASSERT(0);return 0;}
    virtual ITF_INLINE u32          getRefractionPassHeight     (   ) const {ITF_ASSERT(0);return 0;}
    virtual ITF_INLINE u32          getFluidPassWidth     (   ) const {ITF_ASSERT(0);return 0;}
    virtual ITF_INLINE u32          getFluidPassHeight     (   ) const {ITF_ASSERT(0);return 0;}

    bbool                           bCanRenderToCurrentView (   u32 _renderFlag );

    // skip selected DrawPass for view
    void                            hideDrawPass            ( engineView _view, bbool _hideMainPass, bbool _hide2DPass );
    bbool                           getDrawPassHide         ( engineView _view );

    // Take a screenshoot of main drawPass and after draw the copy and skip main drawPass.
    void                            freezeMainDrawPass      ( View &_view, bbool _freeze );

    virtual void                    copyScene               (   ) {}
    virtual void                    drawScreenTransition    ( f32 _fadeVal ) {}
    GFXAdapter_screenTransitionManager& getScreenTransitionManager() { return m_screenTransitionManager; }

    // text
    void                            initFont();
    
    /// fps from system information.
    ITF_INLINE f32                  getfPs                  (   )   {  return m_fPs;};
    ITF_INLINE void                 setfPs                  (   f32 _fps) {  m_fPs = _fps;  };

    ITF_INLINE void                 setPause                (   bbool _pause  ) {  m_paused = _pause;  };
    ITF_INLINE bbool                isGFXPaused             (   ) {  return m_paused;  };

    /// Vertex declaration.
    virtual void                    setVertexFormat         (   u32 /*_vformat*/    ) {ITF_ASSERT(0);}

    virtual String8                 getRenderName           (   ) = 0;

    ITF_INLINE GFX_ZlistManager& getZListManager() {return m_ZlistManager;}

    /// Shaders.
#ifndef ITF_WII
    void                            initShaderConstant      ();
    virtual void                    releaseShader           (   ITF_shader* /*_shader*/ ) {ITF_ASSERT(0);}
    virtual bbool                   loadShader              (   ITF_shader* /*_shader*/ ) {ITF_ASSERT(0);return bfalse;}
    virtual void                    beginShader             (   ITF_shader* /*_shader*/ ) {ITF_ASSERT(0);}
    virtual void                    endShader               (   ITF_shader* /*_shader*/ ) {ITF_ASSERT(0);}

    virtual void*                   loadBinVShader          (   u8* _binShader, ux _binSize ) {return NULL;}
    virtual void*                   loadBinPShader          (   u8* _binShader, ux _binSize ) {return NULL;}
    virtual Path                    getShaderCompiledPath   () {ITF_ASSERT(0); return Path();}
    virtual bbool                   supportShaderCompilation() { return bfalse; }  // override per platform if the compileVertexShader/compilePixelShader can be called
    virtual bbool                   supportExternalShaderCompilation() { return bfalse; }  // override per platform if invokeExternalShaderCompiler can be called

    virtual void                    invokeExternalShaderCompiler(const char* _dataRootPath) { ITF_ASSERT(0); }
    virtual void*                   compileVertexShader     (const char* _fxFileName, const char* _functionName, const char* _define[] = NULL, u32 _numDef = 0) { return NULL;}
    virtual void*                   compilePixelShader      (const char* _fxFileName, const char* _functionName, const char* _define[] = NULL, u32 _numDef = 0) { return NULL;}

    virtual void                    setVertexShader         ( void * _shaderBin ) { ITF_ASSERT(0);}
    virtual void                    setPixelShader          ( void * _shaderBin ) { ITF_ASSERT(0);}

    virtual void                    setShaderRepartition    ( f32 _psPercentage ) { }
#endif // ITF_WII

	f32								m_currentDT;

    /// Shaders.
    GFXAdapter_shaderManager        mp_shaderManager;

    // Shaders register Constant

    // main shader (renderPCT)
    shaderGroupKey     mc_shader_Main;
    shaderEntryKey     mc_entry_main_VS_Default;
    shaderEntryKey     mc_entry_main_VS_PTAmb;
    shaderEntryKey     mc_entry_main_VS_PNC3T;
    shaderEntryKey     mc_entry_main_VS_Patch;
    shaderEntryKey     mc_entry_main_VS_BezierPatch;
    shaderEntryKey     mc_entry_main_VS_Fluid;
    shaderEntryKey     mc_entry_main_VS_Fluid2;
    shaderEntryKey     mc_entry_main_VS_Trail;
    shaderEntryKey     mc_entry_main_VS_Spline;
    shaderEntryKey     mc_entry_main_VS_Refraction;
    shaderEntryKey     mc_entry_main_VS_Overlay;
    shaderEntryKey     mc_entry_main_VS_QuadInstancing;
    shaderEntryKey     mc_entry_main_PS_Default;
    shaderEntryKey     mc_entry_main_PS_Debug;
    shaderEntryKey     mc_entry_main_PS_OverDraw;
    shaderEntryKey     mc_entry_main_PS_Refraction;
    shaderEntryKey     mc_entry_main_PS_Fluid;
    shaderEntryKey     mc_entry_main_PS_RenderInTexture;

    shaderDefineKey     mc_define_FogBox1;
    shaderDefineKey     mc_define_FogBox2;
    shaderDefineKey     mc_define_Texture;
    shaderDefineKey     mc_define_VertexNormal;
    shaderDefineKey     mc_define_NormalMap;
    shaderDefineKey     mc_define_UV2;
    shaderDefineKey     mc_define_Light3D;
    shaderDefineKey     mc_define_Skin;
    shaderDefineKey     mc_define_Light;
    shaderDefineKey     mc_define_StaticFog;
    shaderDefineKey     mc_define_Debug;
    shaderDefineKey     mc_define_SeparateAlpha;
    shaderDefineKey     mc_define_BlendTexture;
    shaderDefineKey     mc_define_TextureUv2;
    shaderDefineKey     mc_define_ZInject;
    shaderDefineKey     mc_define_UseBackLight;
    shaderDefineKey     mc_define_Reflection;
    shaderDefineKey     mc_define_fluidGlow;
    shaderDefineKey     mc_define_fluidEmboss;
    shaderDefineKey     mc_define_fluidVPos;
    shaderDefineKey     mc_define_AlphaTest;
    shaderDefineKey     mc_define_Outline;
	shaderDefineKey     mc_define_ColorMask;
    shaderDefineKey     mc_define_Saturation;

    //movies fx
    shaderGroupKey     mc_shader_Movie;
    shaderEntryKey     mc_entry_movies_VS;
    shaderEntryKey     mc_entry_movies_PS;
    shaderDefineKey    mc_define_movies_use_alpha_texture;

    //font fx
    shaderGroupKey     mc_shader_Font;
    shaderEntryKey     mc_entry_font_VS;
    shaderEntryKey     mc_entry_font_PS;
    shaderDefineKey    mc_define_font_PS_outline;
    shaderDefineKey    mc_define_font_PS_gradient;
    shaderDefineKey    mc_define_font_ZInject;

    //afterFx fx
    shaderGroupKey     mc_shader_AfterFx;
    shaderEntryKey     mc_entry_afterFx_VS_blur;
    shaderEntryKey     mc_entry_afterFx_VS_PCT;
    shaderEntryKey     mc_entry_afterFx_PS_bigBlur;
    shaderEntryKey     mc_entry_afterFx_PS_addMul;
    shaderEntryKey     mc_entry_afterFx_PS_colorOverDraw;
    shaderEntryKey     mc_entry_afterFx_PS_copyOverDraw;
    shaderEntryKey     mc_entry_afterFx_PS_mergedEffect;
    shaderEntryKey     mc_entry_afterFx_PS_Kalei;
    shaderEntryKey     mc_entry_afterFx_PS_DebugPass;
    shaderDefineKey    mc_define_afterFx_Refraction;
    shaderDefineKey    mc_define_afterFx_Glow;
    shaderDefineKey    mc_define_afterFx_Tonemap;
    shaderDefineKey    mc_define_afterFx_Blur;
    shaderDefineKey    mc_define_afterFx_Saturation;
    shaderDefineKey    mc_define_afterFx_ColorCorrection;
    shaderDefineKey    mc_define_afterFx_Tile;
    shaderDefineKey    mc_define_afterFx_Mosaic;
    shaderDefineKey    mc_define_afterFx_Negatif;
    shaderDefineKey    mc_define_afterFx_EyeFish;
    shaderDefineKey    mc_define_afterFx_Mirror;
    shaderDefineKey    mc_define_afterFx_OldTV;
    shaderDefineKey    mc_define_afterFx_Noise;
    shaderDefineKey    mc_define_afterFx_BlendValue;
    shaderDefineKey    mc_define_afterFx_EdgeDetection;
    shaderDefineKey    mc_define_afterFx_Gauss;
    shaderDefineKey    mc_define_afterFx_Radial;

    // Impostor fx
    shaderGroupKey     mc_shader_Impostor;
    shaderEntryKey     mc_entry_impostor_VS;
    shaderEntryKey     mc_entry_impostor_PS;
    shaderGroupKey   m_defaultShaderGroup; // default shader for basic renderer.

    shaderGroupKey     mc_shader_MaskMaterials;
    shaderEntryKey     mc_entry_mask_VS;
    shaderEntryKey     mc_entry_mask_frize_PNC3T_VS;
    shaderEntryKey     mc_entry_maskedMat_PS;
    shaderEntryKey     mc_entry_maskMat_PS;
    shaderDefineKey    mc_define_Mask_AlphaTest;

    // class to store states that need to be kept until the final drawcall
    // can be overload by platforms specific (some platforms may need to keep states until final drawcall, while others can directly push commands)
    class DrawCallState
    {
    public:
        DrawCallState()
            :   renderingMaterialType(GFX_MAT_INVALID)
            ,   vertexFormat(VertexFormat_PCT)
            ,   vertexBuffer(NULL)
            ,   indexBuffer(NULL)
            ,   shaderGroup(shaderGroupKey_invalid)
            ,   vertexSize(0)
            ,   nbVertices(0)
            ,   zOffset(0.0f)
            ,   useUV2(-1)
            ,   useTextureBlend(-1)
            ,   useInstancing(false)
            ,   useSelfIllum(-1)
			,   useColorMask(0)
            ,   vsEntryVariant(shaderEntryKey_invalid)
            ,   psEntryVariant(shaderEntryKey_invalid)
            ,   vsDefineVariant(0)
            ,   psDefineVariant(0)
			,   alphaTestRef(0.0f)
            ,   shouldSkipDrawCall(bfalse)
        {
        }

        void invalidate()
        {
            renderingMaterialType = GFX_MAT_INVALID;
            zOffset = 0.0f;
            useUV2 = -1;
            useTextureBlend = -1;
            useSelfIllum = -1;
            useInstancing = false;
			useColorMask = 0;
            shaderGroup = shaderGroupKey_invalid;
            shouldSkipDrawCall = bfalse;
        }

        void reset()
        {
            *this = DrawCallState();
        }

        bbool        shouldSkipDrawCall;
        GFX_MATERIAL_TYPE   renderingMaterialType; // can change ps variant (and sometime also vs variant)
        i32                 vertexFormat;
        ITF_VertexBuffer* vertexBuffer;
        ITF_IndexBuffer*  indexBuffer;
        shaderGroupKey   shaderGroup;
        u32         vertexSize;
        u32         nbVertices;
        f32         zOffset;
		f32			alphaTestRef;
			
        i8			useUV2;
        i8			useTextureBlend;
        i8			useSelfIllum;
        bool    	useInstancing;
		i8          useColorMask;
		GFX_Vector4	colorForMask[3];
		GFX_Vector4 refractionParam;
		GFX_Vector4 reflectionParam;
		shaderEntryKey vsEntryVariant; // determine vs entry
        shaderEntryKey psEntryVariant; // determine ps entry
        shaderDefineKey vsDefineVariant; // determine vs define variant
        shaderDefineKey psDefineVariant; // determine ps define variant
    };

    DrawCallState m_drawCallStates;

    void setShaderGroup( shaderGroupKey _shaderKey, shaderEntryKey _vsEntry = shaderEntryKey_invalid, shaderEntryKey _psEntry = shaderEntryKey_invalid,
						 shaderDefineKey _vsDefine = 0, shaderDefineKey _psDefine = 0 )
    {
        m_drawCallStates.shaderGroup = _shaderKey;
        // set variants
        setVertexShaderVariants(_vsEntry, _vsDefine);
        setPixelShaderVariants(_psEntry, _psDefine);
    }

    inline void setVertexShaderVariants(shaderEntryKey _vsEntry, shaderDefineKey _vsDefine = 0)
    {
        m_drawCallStates.vsEntryVariant = _vsEntry;
        m_drawCallStates.vsDefineVariant = _vsDefine;
    }

    inline void setPixelShaderVariants(shaderEntryKey _psEntry, shaderDefineKey _psDefine = 0)
    {
        m_drawCallStates.psEntryVariant = _psEntry;
        m_drawCallStates.psDefineVariant = _psDefine;    
    }

    inline void addVertexShaderDefines(shaderDefineKey _vsDefine)
    {
        m_drawCallStates.vsDefineVariant |= _vsDefine;
    }

    inline void addPixelShaderDefines(shaderDefineKey _psDefine)
    {
        m_drawCallStates.psDefineVariant |= _psDefine;    
    }


    /// Patch shader.
    virtual void                    prepareShaderPatch          (   PrimitiveContext &_primCtx, Matrix44* /*_matrix*/, f32 /*_z*/, u32 &/*_hdiv*/, u32 &/*_vdiv*/) {ITF_ASSERT(0);}    
    virtual void                    shaderPatchSendBuffer       (   DrawCallContext &_drawcallCtx, GFXAdapter_shaderBuffer * _shaderBuffer, const GFX_MATERIAL & _material, u32 _hdiv, u32 _vdiv) { ITF_ASSERT(0); }


    /// Bezier Patch shader.
    virtual void                    prepareShaderBezierPatch      (   PrimitiveContext &_primCtx, Matrix44* /*_matrix*/, f32 /*_z*/, u32 &/*_hdiv*/, u32 &/*_vdiv*/) {ITF_ASSERT(0);}    
    virtual void                    shaderBezierPatchSendBuffer (   DrawCallContext &_drawcallCtx, GFXAdapter_shaderBuffer * _shaderBuffer, const GFX_MATERIAL & _material, u32 _hdiv, u32 _vdiv, bbool rotate, u32 * _vdivToDraw) { ITF_ASSERT(0); }

    /// Spline Shader
    virtual void                    drawSpline              (   PrimitiveContext &_primCtx, Matrix44* _matrix, Texture* _texture, const Spline * _spline, f32 _height ) {ITF_ASSERT(0);}

    /// Fluid Shader.
    virtual void                    drawFluid               ( PrimitiveContext &_primCtx, const FluidToDraw & _fluidToDraw, u32 mode) {ITF_ASSERT(0);}
    virtual void                    drawFluid               ( PrimitiveContext &_primCtx, const ITF_VECTOR<FluidToDraw> & _allFluidToDraw, u32 _mode) {ITF_ASSERT(0);}

    /// Trail 3d Shader.
    virtual void                    drawTrail3D             (   PrimitiveContext &_primCtx, const GFX_MATERIAL* _material, Color _color, const SafeArray<Vec3d> & points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength) {ITF_ASSERT(0);}

    //Movie    
    virtual void                    drawMovie               ( PrimitiveContext &_primCtx, Matrix44* _matrix,f32 _alpha,ITF_VertexBuffer* _pVertexBuffer,Texture** _pTextureArray,u32 _countTexture) {ITF_ASSERT(0);}

    /// VertexBuffer.
    virtual void                    createVertexBuffer      (   ITF_VertexBuffer* /*_vertexBuffer*/ ) {ITF_ASSERT(0);}
    virtual void                    releaseVertexBuffer     (   ITF_VertexBuffer* /*_vertexBuffer*/ ) {ITF_ASSERT(0);}
    virtual void                    LockVertexBuffer        (   ITF_VertexBuffer* /*_vertexBuffer*/, void** /*_data*/, u32 /*_offset*/, u32 /*_size*/, u32 /*_flag*/ ) {ITF_ASSERT(0);}
    virtual void                    UnlockVertexBuffer      (   ITF_VertexBuffer* /*_vertexBuffer*/ ) {ITF_ASSERT(0);}
    virtual void                    UnlockVertexBuffer      ( ITF_VertexBuffer* _vertexBuffer, u32 _offset, u32 _size) {ITF_ASSERT(0);}

    /// IndexBuffer.
    virtual void                    setIndexBuffer          (   ITF_IndexBuffer* _indexBuffer ) { ITF_ASSERT(0); }
    virtual void                    createIndexBuffer       (   ITF_IndexBuffer* /*_indexBuffer*/ ) {ITF_ASSERT(0);}
    virtual void                    releaseIndexBuffer      (   ITF_IndexBuffer* /*_indexBuffer*/ ) {ITF_ASSERT(0);}
    virtual void                    LockIndexBuffer         (   ITF_IndexBuffer* /*_indexBuffer*/, void** /*_data*/ )  {ITF_ASSERT(0);}
    virtual void                    UnlockIndexBuffer       (   ITF_IndexBuffer* /*_indexBuffer*/ )  {ITF_ASSERT(0);}
    
    /// Vertex/Index Buffer Draw
    virtual void                    setVertexBuffer         (   ITF_VertexBuffer* _vertexBuffer ) { ITF_ASSERT(0); }
    virtual void                    DrawVertexBuffer        (   DrawCallContext &_drawCallCtx, PRIMITIVETYPE _type, u32 _vertexStart, u32 _vertexNumber )  {ITF_ASSERT(0);}
    virtual void                    drawMesh                (   const PrimitiveContext & _context, ITF_Mesh& _mesh){ITF_ASSERT(0);}

    virtual void                    setShaderForFont(bbool bOutline, bbool bGradient, GFX_Vector4 &_vFontParam, const Color& _gradientColor)  {ITF_ASSERT(0);}
    virtual void                    setShaderBonesMatrices( ITF_VECTOR<Matrix44> &_gizmos )   {ITF_ASSERT(0);}

#ifndef ITF_CONSOLE_FINAL
    virtual void                    reloadShaders() {}
#endif

    virtual void					setVertexShaderConstantF(u32 location, const f32 *floats, u32 count) = 0;
    virtual void					setPixelShaderConstantF(u32 location, const f32 *floats, u32 count) = 0;

    void cleanBufferEndOfFrame();

    /// Matrix.
    ITF_INLINE void                 setMatrixIdentity       (   Matrix44& _matrix  ) { _matrix.setIdentity(); };

    int                             FBTexture[10];

    virtual void        setGammaRamp(Vec3d gamma,f32 brigthnessValue,f32 contrastValue) {ITF_ASSERT(0);}

    // afterfx options:
    void                setInternalRTcolor(const Color& _color) {m_internRTColor = _color;}

    virtual bbool       isSupportingZBuffer() const { ITF_ASSERT(0); return bfalse; } // overload per platform
    
    void                computeUsingZPrepass();
    bbool               isUsingZPrepass() const {return m_useZPrepassCurrently;}

    bbool               useZPrepassByDefault() const {return m_useZPrepassByDefault;}
    void                setUseZPrepassByDefault(bbool _zPrepassOn)	{m_useZPrepassByDefault = _zPrepassOn;}

    bbool               isUsingFluids() const { return (m_isUsingFluids && m_isUsingPostEffects); }
    void                setUsingFluids(bbool _fluidsOn)
	{
		m_isUsingFluids = _fluidsOn;
	}

    bbool               isUsingZInject() const { return m_isUsingZInject; }
    void                setUsingZInject(bbool _zInject)
	{
		m_isUsingZInject = _zInject;
	}

    bbool               isUsingLighting() const { return m_isUsingLighting; }
    void                setUsingLighting(bbool _usingLighting)
	{
		m_isUsingLighting = _usingLighting;
	}

    bbool               isUsingPostEffects() const { return m_isUsingPostEffects; }
    void                setUsingPostEffects(bbool _usingPostEffects)
	{
		m_isUsingPostEffects = _usingPostEffects;
	}

	virtual bbool		isForceWriteAlpha()  { return bfalse; }
	virtual	void		seForceWriteAplha(bbool _usingForceWriteAplha) {};

    const Color&        getInternalRTcolor(   )const { return m_internRTColor; }
    Color               m_internRTColor;

    virtual void        beginGPUEvent(const char* _name, GPU_MARKER __marker = GFX_GPUMARKER_Default){}
    virtual void        endGPUEvent(GPU_MARKER __marker = GFX_GPUMARKER_Default){}

    struct OccluderStat
    {
        ObjectRef objRef;
        f32 screenRatio;
    };
    void                getLastOccluderStats(vector<OccluderStat> & _stats);
    void                setOccluderMapRenderingMode(ux _bMode) { m_renderOccluderMap = _bMode; }
    ux                  getRenderingOccluderMapMode() const { return m_renderOccluderMap; }

    //No required on some adapters
#ifdef ITF_PICKING_SUPPORT
    virtual void        setRenderTarget_Picking() {};
    virtual void        setRenderTarget_BackBuffer() {};

    virtual bbool       resolvePicking() {return bfalse;}

    virtual void        copyPickingToDebugRenderTarget() {}

    bbool               isPickingMode()  const           {return m_bPickingMode;}



    virtual         ux  getPickingWidth() const { return 0; }
    virtual         ux  getPickingHeight() const { return 0; }

    bbool               isPickingDebugActive() const { return m_bPickingDebug; }
    void                setPickingDebug(bbool _activate) { m_bPickingDebug = _activate; }
    virtual u32*        lockPickingBuffer(u32& _pitch) {_pitch = 0;return NULL;}
    virtual void        unlockPickingBuffer() { ITF_ASSERT(0); };

#else
    bbool               isPickingMode() const             {return bfalse;}
#endif  //ITF_WINDOWS

    virtual void        setRenderTargetForPass(GFX_ZLIST_PASS_TYPE _passType, bbool _restoreContext = bfalse)   { ITF_ASSERT(0); }
    virtual void        resolveRenderTargetForPass(GFX_ZLIST_PASS_TYPE _passType)                               { ITF_ASSERT(0); }
    virtual void        copyDebugRenderTarget(f32 _alpha = 0.f, const Vec2d & _pixelOffset = Vec2d::Zero)       {}
    virtual void        copyPassToDebugRenderTarget(GFX_ZLIST_PASS_TYPE _passType)                              {}
    virtual u32*        lockDebugRenderTarget(u32 & _width, u32 & _height, u32 & _pixelPitch)                        { _width = _height = _pixelPitch = 0; return nullptr; }
    virtual void        unlockDebugRenderTarget()                                                               {}

#ifndef ITF_FINAL
    virtual void        startRasterForPass(GFX_ZLIST_PASS_TYPE _passType)   {}
    virtual void        endRasterForPass(GFX_ZLIST_PASS_TYPE _passType)     {}
#endif // ITF_FINAL

    virtual Color       getBufferValue(u32* _pPixels,u32 _pitch,u32 _posX, u32 _posY) {return Color::black();}

    virtual void        doScreenCapture() {};
    virtual void        setAlphaRef(int /*_ref*/ =1) {}
    virtual void        setAlphaTest(bbool _enabled) { ITF_ASSERT(0); }

    void                setSeparateAlpha(bbool _val) { m_useSeparateAlpha =_val; setBlendSeparateAlpha(); }
    virtual void        setBlendSeparateAlpha() {}
    void                toggleSeparateAlpha() {setSeparateAlpha(!m_useSeparateAlpha);}
    bbool               getSeparateAlpha() {return m_useSeparateAlpha;}

    void                setOverDrawMode(u32 _val) {m_overDrawMode =_val;if (m_overDrawMode>3) {m_overDrawMode = 0;} if (m_overDrawMode==1) {m_overDrawMode = 2;}}
    void                switchOverDrawMode() {setOverDrawMode(++m_overDrawMode);}
#ifndef ITF_FINAL
    u32                 getOverDrawMode() {return m_overDrawMode;}
#else
    u32                 getOverDrawMode() {return 0;}
#endif
	bbool				getOverDrawRender() {return m_overDrawRender;}
	void				startOverDrawRender() {if (m_overDrawMode) m_overDrawRender = true;}
	void				endOverDrawRender() {m_overDrawRender = false;}
    virtual void        showOverDrawRecord(RenderPassContext &_rdrCtx) {ITF_ASSERT(0);}

	//-----------------------------------------------------------------------------------------------------------------
	// safe frame 
#ifndef ITF_CONSOLE_FINAL
    bbool           m_drawSafeFrameRequested;
    bbool           safeFrameDrawRequested() { return m_drawSafeFrameRequested; }
    void            hideSafeFrame() { m_drawSafeFrameRequested = bfalse; }
    void            showSafeFrame() { m_drawSafeFrameRequested = btrue; }
    virtual void    drawSafeFrame() {}
#endif // ITF_CONSOLE_FINAL

    void                setWireFrameMode(WireFrameMode _mode) { m_showWireFrame = _mode; }
    void                switchWireFrameMode() { m_showWireFrame =  WireFrameMode(ux(m_showWireFrame)+1u); if (m_showWireFrame == WIRE_FRAME_LAST) m_showWireFrame = WIRE_FRAME_FIRST; }
    WireFrameMode       getWireFrameMode() const { return m_showWireFrame; }

    void                setZListPassDebugMode(GFX_ZLIST_PASS_TYPE _zlistPass) { m_debugZListPass = _zlistPass; }
    GFX_ZLIST_PASS_TYPE getZListPassDebugMode() const { return (GFX_ZLIST_PASS_TYPE)m_debugZListPass; }
    void                setZListPassDebugModeObjectRef(GFX_ZLIST_PASS_TYPE _zlistPass, const ObjectRef & _objRef);

    void                setDebugDrawMode(u32 _val) {m_debugDrawMode =_val;if (m_debugDrawMode>3) {m_debugDrawMode = 0;}}
    void                switchDebugDrawMode() {setDebugDrawMode(++m_debugDrawMode);}
    u32                 getDebugDrawMode() {return m_debugDrawMode;}

    virtual void        startScreenCapture(const Path *_directory) {};
    void                stopScreenCapture();
    void                cancelScreenCapture();

    virtual void        setThreadUsageScreenCapture(u32 _uThreadUsage,bbool _bStepByStep);

#ifndef ITF_FINAL
    void     toggleRaster    (   );
    ITF_INLINE void     toggleAfterFx   (   )   {   m_showAfterFx = !m_showAfterFx;};
    ITF_INLINE  bbool   isShowRasters   (   )   {   return m_showRasters;   };
    ITF_INLINE  bbool   isShowAfterFx   (   )   {   return m_showAfterFx;   };
    
    ITF_INLINE void     toggleDebugInfo (   )   {   m_showDebugInfo = !m_showDebugInfo;};
    ITF_INLINE  bbool   isShowDebugInfo (   )   {   return m_showDebugInfo;   };

    ITF_INLINE void     setShowDebugColor(bbool _val)      {  m_showDebugColor = _val;}
    ITF_INLINE void     toggleDebugColor (   )   {   m_showDebugColor = !m_showDebugColor;};
    ITF_INLINE bbool    isShowDebugColor (   )const   {   return m_showDebugColor;   } ;
    ITF_INLINE void     toggleDebugRendering (   )   {   m_showDebugRendering = !m_showDebugRendering;};
    ITF_INLINE bbool    isShowDebugRendering (   )const   {   return m_showDebugRendering;   } ;
    ITF_INLINE void     setDebugColor    ( const Color& _color )   {   m_debugColor = _color;};
    ITF_INLINE Color&   getDebugColor    (   )   {   return m_debugColor;} ;
    ITF_INLINE void     setDebugBorder    ( const float _Border )   {   m_debugBorder = _Border;};
    ITF_INLINE float    getDebugBorder    (   )   {   return m_debugBorder;} ;

    // set individually at each Pickable drawn if it's marked as being displayed for debug
    ITF_INLINE bbool    isShowDebugColorObject() const          { return m_showDebugColorObject; }
    ITF_INLINE void     setShowDebugColorObject(bbool _showDebugColorObject) { m_showDebugColorObject = _showDebugColorObject; }

#else
    ITF_INLINE  bbool   isShowAfterFx   (   )   {   return btrue;   };
#endif // ITF_FINAL

    ITF_INLINE void     toggleLighting()            { m_LightingEnable = !m_LightingEnable; }
    ITF_INLINE void     setLightingEnable(bbool _enable)            { m_LightingEnable = _enable; }
    ITF_INLINE bbool    isLightingEnable() const
    {
        return m_LightingEnable && isUsingLighting() && ( m_overDrawMode == 0 )
#ifdef ITF_PICKING_SUPPORT
            && !m_bPickingMode
    #ifndef ITF_FINAL
            && m_showAfterFx
    #endif
#endif
            ;
    }

#ifdef ITF_SUPPORT_DBGPRIM_MISC
    void fillTriangleList(const DBGLine& l);
    void fillTriangleList(const DBG3DLine& l);
    void fillTriangleList(const DBGTriangle3D& t);
#endif // ITF_SUPPORT_DBGPRIM_MISC

    void drawFontText(const ITF_VECTOR <class View*>& _views, FontText* _fontText, f32 _depthOffset,  const ConstObjectRef & _objectRef);

    ITF_INLINE  bbool isRenderSceneInTarget() {return m_RenderSceneInTarget;};
    ITF_INLINE  void setRenderSceneInTarget(bbool _val) {m_RenderSceneInTarget = _val;};

    ITF_INLINE  u8   getStencilOverlapRef() {return m_stencilCurrentRef;}
    ITF_INLINE  u8   nextStencilOverlapRef() { m_stencilCurrentRef = (m_stencilCurrentRef >= STENCIL_MASK_OVERLAP)?1:m_stencilCurrentRef+1; return m_stencilCurrentRef;}
    ITF_INLINE  u8   resetStencilOverlapRef() { m_stencilCurrentRef = 1; return m_stencilCurrentRef;}

    /// static vb for patches
    static const UVSubdiv       patchGridBufferTypes[];
    static const u32            patchGridBufferTypesNb;

    static String8              getAllowedPatchGridBufferTypesStr();

    ITF_IndexBuffer*            createPatchGridInternalIB(u32 _hdiv, u32 _vdiv);
    ITF_VertexBuffer*           createPatchGridInternalVB(u32 _hdiv, u32 _vdiv);

    void                        createPatchGridInternalBuffers();
    void                        removePatchGridInternalBuffers();

    ITF_IndexBuffer*            getPatchGridIndexBuffer(u32 & _hdiv, u32 & _vdiv);
    ITF_VertexBuffer*           getPatchGridVertexBuffer(u32 & _hdiv, u32 & _vdiv);

    /// Fog.
    ITF_INLINE void             setUseFog(bbool _v) { m_useFog = _v;}
    ITF_INLINE bbool            isUseFog() { return m_useFog;}
    FogManager                  m_fogManager;
    ITF_INLINE FogManager&      getFogManager() { return m_fogManager; }
    void                        computeFogBoxObjectVisibility(const AABB& _aabb) {m_fogManager.computeBVVisibility(_aabb);}
    void                        clearFogBoxObjectVisibility() {m_fogManager.clearObjectFogBoxList();}

	// Normal map light manager.
    GFX_LightManager*			   m_lightManager;
    ITF_INLINE GFX_LightManager& getLightManager() { return *m_lightManager; }

	// Render param manager.
    RenderParamManager*			   m_renderParamManager;
    ITF_INLINE RenderParamManager& getRenderParamManager() { return *m_renderParamManager; }

    GFX_GridFluidManager*		     m_gridFluidManager;
    ITF_INLINE GFX_GridFluidManager& getGridFluidManager() { return *m_gridFluidManager; }

    //Reflection
    ITF_INLINE void             setUseReflection(bbool _v) { m_useReflection = _v;}
    ITF_INLINE bbool            isUseReflection() { return m_useReflection;}
    void                        beginReflectionRendering();
    void                        endReflectionRendering();

	// Set texture
	virtual void        bindTexture(ux _samplerIndex, Texture * _texture, bbool _linearFiltering  = btrue,
		                            GFX_TEXADRESSMODE _addressModeU = GFX_TEXADRESSMODE_CLAMP,
									GFX_TEXADRESSMODE _addressModeV = GFX_TEXADRESSMODE_CLAMP ) {ITF_ASSERT(0);}
	virtual void        bindTexture(ux _samplerIndex, BasePlatformTexture * _texture, bbool _linearFiltering  = btrue,
		                            GFX_TEXADRESSMODE _addressModeU = GFX_TEXADRESSMODE_CLAMP,
									GFX_TEXADRESSMODE _addressModeV = GFX_TEXADRESSMODE_CLAMP ) {ITF_ASSERT(0);}

	/// Global Color.
    virtual void        setGlobalColor(const Color& ) {ITF_ASSERT(0);}
    virtual void        setFogColor(const Color& ) {ITF_ASSERT(0);}
    virtual void        setUseStaticfog( bbool _useStaticFog) { m_useStaticFog = _useStaticFog;}
    virtual void        setDefaultGFXPrimitiveParam() {ITF_ASSERT(0);}
            void        setGFXPrimitiveParameters( const GFXPrimitiveParam* _primitiveParam, const RenderPassContext & _rdrPassCtxt);
    ITF_INLINE void     setGFXGlobalColor(const Color& _color) {m_globalColor = _color;}
    ITF_INLINE Color    getGFXGlobalColor() {return m_globalColor;};

	// Global color blended with the instance color.
	Color m_globalColorBlend;
	// Global fog blended with instance static fog.
    Color m_globalStaticFog;
	// Fog opacity added to the instance static fog opacity.
	f32 m_globalFogOpacity;
	// Global brightness added to the instance brightness.
	f32 m_globalBrightness;

	// Reset global color modifiers.
	void resetGlobalParameters();

	f32      getFrontLightBrightness();
    ITF_INLINE f32      getFrontLightContrast() {return m_currentPrimitiveParam->m_frontLightContrast;}
    ITF_INLINE f32      getBackLightBrightness() {return m_currentPrimitiveParam->m_backLightBrigthness;}
    ITF_INLINE f32      getBackLightContrast() {return m_currentPrimitiveParam->m_backLightContrast;}
    ITF_INLINE NormalLightingParam *getNormalMapLightParam()
	{
		return m_currentPrimitiveParam->m_normalLightParam;
	}
    ITF_INLINE f32      getOutlineWidth() {return m_currentPrimitiveParam->m_outlineWidth;}
    ITF_INLINE f32      getOutlineGlow() {return m_currentPrimitiveParam->m_outlineGlow;}
    ITF_INLINE const Color& getOutlineColor() {return m_currentPrimitiveParam->m_outlineColor;}
    ITF_INLINE bbool    getUseRenderInReflection() {return m_currentPrimitiveParam->m_renderInReflection;}
    ITF_INLINE f32      getSaturation() {return m_currentPrimitiveParam->m_saturation;}
    ITF_INLINE GFXMaskInfo      getMaskInfo() {return m_currentPrimitiveParam->m_gfxMaskInfo;}


    ITF_VertexBuffer* createVertexBuffer        (   u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, bbool _dynamic, dbEnumType _type = VB_T_NOTDEFINED );
    ITF_IndexBuffer* createIndexBuffer          (   u32 _indexNumber, bbool _dynamic    );
    void removeVertexBuffer                     (   ITF_VertexBuffer* _VertexBuffer     );
    void removeVertexBufferDelay                (   ITF_VertexBuffer* _VertexBuffer ,u32 _frameCount    );
    void removeIndexBuffer                      (   ITF_IndexBuffer* _IndexBuffer   );
    void removeIndexBufferDelay                (   ITF_IndexBuffer* _IndexBuffer ,u32 _frameCount     );

    ITF_INLINE GFXAdapter_VertexBufferManager& getVertexBufferManager() {return m_VertexBufferManager;};

    ITF_INLINE  void setUseDepthBuffer(bbool _val) {m_useDepthBuffer = _val;};
    ITF_INLINE  bbool isUseDepthBuffer() {return m_useDepthBuffer;};

    ITF_INLINE  void  setDisableRender2d(bbool _val) { m_disableRendering2d = _val; };
    ITF_INLINE  bbool isDisableRender2d() {return m_disableRendering2d;};

    ITF_INLINE  void  setDisableRenderAfterFx(bbool _val) { m_disableAfterFx = _val; }
    ITF_INLINE  bbool isDisableRenderAfterFx() { return m_disableAfterFx; }

    ITF_INLINE  u32     getSpriteIndexBufferSize() {return m_spriteIndexBufferSize;};
    ITF_IndexBuffer*    needSpriteIndexBuffer(u32 _numberIndex);
    ITF_VertexBuffer*   needSpriteInstancingVertexBuffer();
    ITF_VertexBuffer*   needSpriteInstancingIndexAsVertexBuffer();

    virtual void        acquireDeviceOwnerShip()         {ITF_ASSERT(0);}
    virtual void        releaseDeviceOwnerShip()         {ITF_ASSERT(0);}
    virtual void        minimal_present()                {ITF_ASSERT(0);}

    virtual void        lockDeviceLost()         {}
    virtual void        unlockDeviceLost()         {}

#if defined(ITF_DEBUG_DRAWCALL_STATES)
    virtual void debugBeginDrawZListNode() {}
    virtual void debugEndDrawZListNode() {}
#else
    void debugBeginDrawZListNode() {}
    void debugEndDrawZListNode() {}
#endif

    void                addHideFlags(const u32 _flags) {m_HideFlags |= _flags;}
    void                removeHideFlags(const u32 _flags) {m_HideFlags &= ~_flags;}
    bbool               hasHideFlags(const u32 _flags) const {return (m_HideFlags & _flags) != 0;}
    u32                 getHideFlags() const {return m_HideFlags;}
    void                setHideFlags(u32 _flags) {m_HideFlags = _flags;}

    f32                 getMonitorRefreshRate() const { return m_monitorRefreshRate; }

    void                setDisplayDebugStats(bbool _val)    {m_displaydebugstats = _val;}
    bbool               getDisplayDebugStats();
    void                setAllowShaderCompil(bbool _val)	{m_allowShaderCompil = _val;}

#ifndef ITF_DISABLE_DEBUGINFO
    void                setDebugExtraInfo(const String8& _extraInfo)         {m_extraInfo = _extraInfo;}
    void                setEngineDataVersion(const String8& _extraInfo)      {m_engineDataVersion = _extraInfo;}
protected:
    String8            m_extraInfo;
    String8            m_engineDataVersion;
public:
#endif //ITF_DISABLE_DEBUGINFO

    void                extractFrustumPlanes(const Matrix44 & _proj, Camera* _cam);

	// In-game Screenshot 
	class AsyncScreenshotConfig
	{
	public:
		AsyncScreenshotConfig(ux _width, ux _height, AsyncScreenshotRenderStep _rdrStep, engineView _view)
            : m_width(_width)
            , m_height(_height)
            , m_rdrStep(_rdrStep)
			, m_engineView(_view) 
		{
		}

		AsyncScreenshotConfig()
            : m_rdrStep(AsyncScreenshotRenderStep_BeforePostFX)			
			, m_engineView(e_engineView_MainScreen) 
            , m_width(0)
            , m_height(0)
		{
		}

		AsyncScreenshotConfig(const AsyncScreenshotConfig& _config)
		{
			m_width         = _config.m_width;
            m_height        = _config.m_height;
			m_rdrStep		= _config.m_rdrStep;
			m_engineView	= _config.m_engineView;
		}

		ux m_width;
        ux m_height;
        AsyncScreenshotRenderStep m_rdrStep;
		engineView m_engineView;
	};

	// In-game asynchronous screen shots (for facebook, saves, etc...)
    // Designed to get a minimal impact on game frame rate (ie. ).
    // Use case :
    // - call requestAsyncScreenshot
    // - add 2D primitives you want to be added to the GFX_ZLIST_SCREENSHOT_2D primitive list (at batch primitive step)
    // - after a few frames (probably 2 frames), you can call askAsyncScreenshotData that will provide you with a ScreenShotResultData object.
    // - askAsyncScreenshotData can return false if the screenshot is not yet ready.
	bbool requestAsyncScreenshot(const AsyncScreenshotConfig& _config);

    // Request of the async screenshot. If an async screenshot has been requested, the screenshot has to be retrieve.
    // return btrue if a screenshot is available, and in this case _result is filled and internal screenshot resources are released.
    // Any askAsyncScreenshotData without a corresponding requestScreenshot will fail.
	bbool askAsyncScreenshotData(class GFX_AsyncScreenShotResultData & _result);
    bbool isAsyncScreenshotDataAvailable() const; // return btrue if askAsyncScreenshotData will succeed

protected:
	struct AsyncScreenshotState
	{
		AsyncScreenshotState(): m_renderPushedFrame(0), m_requested(bfalse) {}

		AsyncScreenshotConfig m_config;
		bbool m_requested;
		u32 m_renderPushedFrame;  // the frame number of the asked screenshot
	};

	void renderAsyncScreenshot(View &_view);
    bbool isAsyncScreenshotRequested(AsyncScreenshotRenderStep _renderStep) const;
	AsyncScreenshotState m_asyncScreenshotState;

    // platform specific interface for screenshot rendering
    virtual void copyRenderTargetToScreenshot(RenderPassContext & _rdrCtxt, ux _width, ux _height) { ITF_ASSERT(0); }
    virtual void copyScreenshotSurfaceToRGBABuffer(u32 * _buffer, ux _width, ux _height) { ITF_ASSERT(0); }
    virtual void releaseScreenshotSurface() { ITF_ASSERT(0); }
    
// take a screenshot [begin]
private:
    const char* m_strSceenshotResquest;
    

public:
		VS_Global	m_vsGlobal;

#ifndef ITF_FINAL
    void requestProfilerFPScaptureMode() { m_strSceenshotResquest = strScreenshotProfileFPS(); }
#endif //ITF_FINAL

    void requestScreenshotUHDMode() { m_strSceenshotResquest = strScreenshotUHD(); }
    void requestScreenshotUHDNo2dMode() { m_strSceenshotResquest = strScreenshotUHDNo2d(); }
    void requestScreenshotNormalMode() { m_strSceenshotResquest = strScreenshotNormal(); }

    void resetScreenShotRequest() { m_strSceenshotResquest = NULL; }
    const char* getStrSceenshotResquest() const { return m_strSceenshotResquest; }
    virtual void  startScreenshotMode() {};
    virtual void  endScreenshotMode() {};

	virtual Texture* getScreenShotTexture() { return NULL; }
	virtual bbool isScreenShotTextureAvailable() { return bfalse; }
    virtual bbool isScreenShotTextureValid() { return bfalse; }
    virtual void invalidateScreenShotTexture() { }

	// Global color blended with the instance color.
	ITF_INLINE void setGlobalColorBlend(Color _color)
	{
		m_globalColorBlend = _color;
	}
	// Global fog blended with instance static fog.
	ITF_INLINE void setGlobalStaticFog(Color _color)
	{
		m_globalStaticFog = _color;
	}
	// Fog opacity added to the instance static fog opacity.
	ITF_INLINE void setGlobalFogOpacity(f32 _globalFogOpacity)
	{
		m_globalFogOpacity = _globalFogOpacity;
	}
	// Global brightness added to the instance brightness.
	ITF_INLINE void setGlobalBrightness(f32 _globalBrightness)
	{
		m_globalBrightness = _globalBrightness;
	}

	static const char* strScreenshotNormal() {return "ScreenshotNormal";}
    static const char* strScreenshotUHD() {return "ScreenshotUHD";}
    static const char* strScreenshotUHDNo2d() {return "ScreenshotUHDNo2d";}
#ifndef ITF_FINAL
    void requestScreenshotMapTiler() { m_strSceenshotResquest = strScreenshotMapTiler(); }
    static const char* strScreenshotMapTiler() {return "ScreenshotMapTiler";}
    static const char* strScreenshotProfileFPS() {return "ScreenshotProfileFPS";}
    static String8 m_screenshotMapTilerPath;
    static u32     m_screenshotMapTilerSize;
#endif //ITF_FINAL

// take a screenshot [end]

#ifdef ITF_USE_REMOTEVIEW
    u32                 m_currentEngineView;    //the current view for the current graphic pass
    u32                 m_displayingEngineView; //the final displaying view
#endif

private:

    void                    drawPrimitives2d(View &_view );

    ITF_VertexBuffer*   m_internalSpritesInstancingVertexBuffer;
    ITF_VertexBuffer*   m_internalSpritesInstancingIndexAsVertexBuffer;
    ITF_IndexBuffer*    m_internalSpritesIndexBuffer;
    u32                 m_spriteIndexBufferSize;

    bbool               m_useDepthBuffer;
    bbool               m_SceneAlreadyResolve;
    bbool               m_disableRendering2d;
	bbool				m_disableAfterFx;

    bbool               m_RenderSceneInTarget;
    u8                  m_stencilCurrentRef;

    AABB                            m_screenAABB;
    f32                             m_fPs;
    bbool                           m_paused;

#ifndef ITF_FINAL
    bbool                           m_showRasters;
    bbool                           m_showAfterFx;
    bbool                           m_showDebugInfo;
    bbool                           m_showDebugColor;
    bbool                           m_showDebugRendering;
    bbool                           m_showDebugColorObject;

    Color                           m_debugColor;
    float                           m_debugBorder;
#endif // ITF_FINAL

    ///  vertexs buffer.
    Texture                             *m_fontTexture;

#ifdef ITF_SUPPORT_DBGPRIM_BOX
    SafeArray<DBGBox, 16>               m_DBGBoxes;
#endif // ITF_SUPPORT_DBGPRIM_BOX

#ifdef ITF_SUPPORT_DBGPRIM_MISC
    SafeArray<DBGSphere, 16>            m_DBGSpheres;
    SafeArray<DBGCircle, 16>            m_DBGCircles;

    SafeArray<DBGPrim3D, 8>             m_DBGPrim3D;
    SafeArray<DBGLine, 256>             m_DBGLines;
    SafeArray<DBG3DLine, 16>            m_DBG3DLines;
    SafeArray<DBGLine, 8>               m_DBGLinesDuration;
    SafeArray<DBGTriangle, 8>           m_DBGTriangles;
    SafeArray<DBGTextureTriangle, 8>    m_DBGTextureTriangles;
    SafeArray<VertexPC, 8>              m_DBGTriangleList;
    SafeArray<DBGSpline, 8>             m_DBGSpline;

    ITF_VECTOR<DBGContextCircle*>       m_DBGCircleContext;
#endif

public:
#ifdef ITF_SUPPORT_DBGPRIM_TEXT
    ITF_VECTOR<DBGText>                 m_DBGTexts;
    ITF_VECTOR<DBGText>                 m_DBGLeftStackedTexts;
    ITF_VECTOR<DBGText>                 m_DBGRightStackedTexts;
#endif

	f32								m_centroidOffset;

    ObjectRef m_zlistPassObjRef[GFX_ZLIST_PASS_COUNT];
    u32       m_debugZListPass;

protected:

#ifdef ITF_SUPPORT_DBGPRIM_BOX
    SafeArray<Box2D, 10>                m_DBGBox2DList;
#endif // ITF_SUPPORT_DBGPRIM_BOX

#ifdef ITF_SUPPORT_DBGPRIM_MISC
    SafeArray<DBGTextureQuad, 8>        m_DBGTextureQuads;
    SafeArray<Triangle2D, 10>           m_DBGTriangle2DList;
    SafeArray<Circle2D, 10>             m_DBGCircle2DList;
#endif // ITF_DEBUG_LEVEL > 0


    virtual void    drawViewFrame(View &_view) = 0;

    /// Manager Vertex/Index
    GFXAdapter_VertexBufferManager  m_VertexBufferManager;

    /// static vb.
    ITF_IndexBuffer**   m_internalIdxBufferTab;
    ITF_VertexBuffer**  m_internalVtxBufferTab;

    class VideoCapture*       m_videoCapture;

    //GroupMeshManager*               m_groupMeshManager;

#ifndef ITF_FINAL
    u32 m_fenceFrameParity;
#endif

    u32                             m_drawFlag;
    f32                             m_currentDrawZCam;
    f32                             m_currentDrawZCamAbs;
    f32                             m_currentDrawZWorld;

    u32                             m_HideFlags;

    // hide drawPass
    bbool                           m_hideMainDrawPass[e_engineView_Count];
    bbool                           m_hide2DDrawPass[e_engineView_Count];

    //The size of the current View
    u32                             m_screenWidth;
    u32                             m_screenHeight;

    //  The size of the Final Windows/TV
    u32                             m_windowWidth;
    u32                             m_windowHeight;

#ifdef ITF_PICKING_SUPPORT
    bbool                           m_bPickingMode;
    bbool                           m_bPickingDebug;
    u8                              m_alphaPickRef;
    Vec2d                           m_PickingPos;
    Vec2d                           m_deltaPicking;
    GFX_RECT                        m_pickingScissor;
#endif //ITF_WINDOWS

    friend class PickingNextObjectPrim;
    void                            advancePickingToNextPixel();

    GFXAdapter_screenTransitionManager m_screenTransitionManager;

protected:

    GFX_ZlistManager                m_ZlistManager;

    friend class RenderPassAlgo;
    RenderPassAlgo *                m_renderPassAlgo[GFX_ZLIST_PASS_COUNT];
    RenderPassAlgo                  m_defaultRenderPassAlgo;

    class ZPrepassRenderPassAlgo : public RenderPassAlgo
    {
        virtual void prepareDrawCallState();
    };
    ZPrepassRenderPassAlgo          m_zprepassRenderPassAlgo;

    class RegularRenderPassAlgo : public RenderPassAlgo
    {
        virtual void begin(class RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
        virtual void prepareDrawCallState();
    protected:
        virtual void unpause(RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
        virtual void pause(RenderPassContext & _rdrCtxt, bbool _resolveTarget = bfalse);
    };
    RegularRenderPassAlgo           m_regularRenderPassAlgo;

    class ThreeDRenderPassAlgo : public RenderPassAlgo
    {
        virtual void prepareDrawCallState();
    };
    ThreeDRenderPassAlgo           m_3DRenderPassAlgo;

    class LightingRenderPassAlgo : public RenderPassAlgo
    {
        virtual void prepareDrawCallState();
    };
    LightingRenderPassAlgo           m_lightRenderPassAlgo;

    class ReflectionPrepassRenderPassAlgo : public RenderPassAlgo
    {
        virtual void prepareDrawCallState();
    };
    ReflectionPrepassRenderPassAlgo     m_reflectionPrepassRenderPassAlgo;

    class ReflectionRenderPassAlgo : public RenderPassAlgo
    {
        virtual void prepareDrawCallState();
    protected:
        virtual void unpause(RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
    };
    ReflectionRenderPassAlgo           m_reflectionRenderPassAlgo;

    class RefractionRenderPassAlgo : public RenderPassAlgo
    {
        virtual void prepareDrawCallState();
    };
    RefractionRenderPassAlgo           m_RefractionRenderPassAlgo;

    class FluidRenderPassAlgo : public RenderPassAlgo
    {
        virtual void prepareDrawCallState();
    protected:
        virtual void begin(RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
        virtual void end(RenderPassContext & _rdrCtxt, bbool _resolveTarget);
    };
    FluidRenderPassAlgo           m_FluidRenderPassAlgo;

    class MaskRenderPassAlgo : public RenderPassAlgo
    {
        virtual void prepareDrawCallState();
    protected:
        virtual void begin(RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
        virtual void end(RenderPassContext & _rdrCtxt, bbool _resolveTarget);
    };
    MaskRenderPassAlgo           m_MaskRenderPassAlgo;

    class OverridenRenderPassAlgo : public RenderPassAlgo
    {
    public:
        OverridenRenderPassAlgo(RenderPassAlgo * _overAlgo = nullptr) : m_overridenAlgo(_overAlgo) {}
        void setOverridenAlgo(RenderPassAlgo * _overAlgo) { m_overridenAlgo = _overAlgo; }
        RenderPassAlgo * getOverridenAlgo() const { return m_overridenAlgo; }
        virtual void prepareDrawCallState();
        virtual void begin(RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
        virtual void end(RenderPassContext & _rdrCtxt, bbool _resolveTarget);
    private:
        RenderPassAlgo * m_overridenAlgo;
    };

    class PickingRenderPassAlgo : public OverridenRenderPassAlgo
    {
    public:
        typedef OverridenRenderPassAlgo Super;

        PickingRenderPassAlgo(RenderPassAlgo * _overAlgo = nullptr) : Super(_overAlgo) {}
        virtual void prepareDrawCallState();
        virtual void begin(RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
    };

    class ScreenshotRenderPassAlgo : public OverridenRenderPassAlgo
    {
    public:
        typedef OverridenRenderPassAlgo Super;

        ScreenshotRenderPassAlgo(RenderPassAlgo * _overAlgo = nullptr) : Super(_overAlgo) {}
        virtual void prepareDrawCallState();
    };

    class OccluderMapRenderPassAlgo : public OverridenRenderPassAlgo
    {
    public:
        typedef OverridenRenderPassAlgo Super;

        OccluderMapRenderPassAlgo(RenderPassAlgo * _overAlgo = nullptr) : Super(_overAlgo) {}
        virtual void prepareDrawCallState();
    };

    typedef ITF_HASHMAP< u32, ObjectRef> OccluderColorMap;
    vector<OccluderStat> m_occluderStats;
    ux                  m_renderOccluderMap;

    void                computeOccluderStats(const OccluderColorMap & _occluderMap);

public :
    GFX_ZLIST_PASS_TYPE getCurPassType() const { return m_curPassType; }

	void				prerenderPass(u32 _zListViewID);
	void				preRenderViews(View & _view);

protected :
    GFX_ZLIST_PASS_TYPE m_curPassType;
    bbool               m_useZPrepassByDefault;
    bbool               m_useZPrepassCurrently;
	bbool				m_isUsingLighting;          // Project/platform constraint.
	bbool				m_isUsingPostEffects;       // Project/platform constraint.
	bbool				m_isUsingFluids;
	bbool				m_isUsingZInject;
    bbool               m_LightingEnable;			// User toggle

    Color               m_globalColor;
    Color               m_colorFog;
    bbool               m_useStaticFog;
    const GFXPrimitiveParam* m_currentPrimitiveParam;
    const GFXPrimitiveParam  m_defaultPrimitiveParam;

    bbool               m_useReflection;
    bbool               m_useFog;
    bbool               m_waitVBL;
    u32                 m_overDrawMode;
	bbool				m_overDrawRender;

	u32                 m_debugDrawMode;
    WireFrameMode       m_showWireFrame;
    bbool               m_useSeparateAlpha;
    
    f32                 m_monitorRefreshRate;
    bbool               m_displaydebugstats;
    bbool               m_frameHaveRefractionPass;

	bbool				m_allowShaderCompil;

public:
    void    DebugCopyZListPass();
    void    drawZListNode(const class RenderPassContext & _rdrCtxt, const ZList_Node_Prim & _curNode, f32 zCamRef);
    void    drawRenderPass(class RenderPassContext & _rdrCtxt);
    // following methods are called by drawRenderPass
    void    startRenderPass(class RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
    void    stopRenderPass(class RenderPassContext & _rdrCtxt);
    void    pauseRenderPass(class RenderPassContext & _rdrCtxt);
    void    unpauseRenderPass(class RenderPassContext & _rdrCtxt, bbool _restoreContext = bfalse);
    Color m_defaultClearColor;
    // Todo -> handle clear colors per view

	Color m_clearColor;

protected:
	GFX_Vector4 m_viewportDimensions;

    FluidSimu_Info m_fluidSimuInfo;
#ifndef ITF_FINAL
public :
    struct dbgCurShaderNfo
    {
        dbgCurShaderNfo() : m_shader(0), m_shaderID(0), m_entryID(0), m_defineID(0) {}
        dbgCurShaderNfo(class ITF_shader *_shader, shaderGroupKey _shaderID,
                        shaderEntryKey _entryID, shaderDefineKey _defineID)
            : m_shader(_shader)
            , m_shaderID(_shaderID)
            , m_entryID(_entryID)
            , m_defineID(_defineID)
        {}
        class ITF_shader *m_shader;
        shaderGroupKey m_shaderID;
        shaderEntryKey m_entryID;
        shaderDefineKey m_defineID;
        #ifdef ITF_DEBUG
        String8 m_entryFullName;
        #endif
    };
    dbgCurShaderNfo m_curentVS;
    dbgCurShaderNfo m_curentPS;
#endif // ITF_FINAL
};

#define GFX_ADAPTER         GFXAdapter::getptr()

    #define ITF_DECLARE_VS_ATTRIB_REG(attribType, attribName, attribFirstReg) \
        VS_Attrib_##attribName = attribFirstReg,

    #define ITF_DECLARE_VS_ATTRIB_ARRAY_REG(attribType, attribName, arraySize, attribFirstReg) \
        VS_Attrib_##attribName = attribFirstReg,

    #define ITF_DECLARE_VS_ATTRIB_STRUCT_REG(structType, attribName, structSize, attribFirstReg) \
        VS_Attrib_##attribName = attribFirstReg,

    enum VertexShaderAttrib
    {
        #include "../bin/Shaders_dev/Unified/ParameterListVS_Macros.h"
    };

#ifndef ITF_PS3
    // ignore PS reg on PS3
    #define ITF_DECLARE_PS_ATTRIB_REG(attribType, attribName, attribFirstReg) \
        PS_Attrib_##attribName = attribFirstReg,

    #define ITF_DECLARE_PS_ATTRIB_ARRAY_REG(attribType, attribName, arraySize, attribFirstReg) \
        PS_Attrib_##attribName = attribFirstReg,

    #define ITF_DECLARE_PS_ATTRIB_STRUCT_REG(structType, attribName, structSize, attribFirstReg) \
        PS_Attrib_##attribName = attribFirstReg,

    enum PixelShaderAttrib
    {
        #include "../bin/Shaders_dev/Unified/ParameterListPS_Macros.h"
    };
#else
    // ignore PS reg on PS3
    #define ITF_DECLARE_PS_ATTRIB_REG(attribType, attribName, attribFirstReg) \
        PS_Attrib_##attribName,

    #define ITF_DECLARE_PS_ATTRIB_ARRAY_REG(attribType, attribName, arraySize, attribFirstReg) \
        PS_Attrib_##attribName,

    #define ITF_DECLARE_PS_ATTRIB_STRUCT_REG(structType, attribName, structSize, attribFirstReg) \
        PS_Attrib_##attribName,

    enum PixelShaderAttrib
    {
        #include "../bin/Shaders_dev/Unified/ParameterListPS_Macros.h"
        PS_Attrib_Count,
    };
#endif

} // namespace ITF


#endif // _ITF_GFX_ADAPTER_H_
