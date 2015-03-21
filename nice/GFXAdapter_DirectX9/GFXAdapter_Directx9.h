#if defined (ITF_WINDOWS) || defined (ITF_X360)

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#define _ITF_DIRECTX9_DRAWPRIM_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

#ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
#endif //_ITF_GFXADAPTER_SHADERMANAGER_H_

#ifndef ITF_GFX_ADAPTER_COMMON_H_
#include "engine/AdaptersInterfaces/GFXAdapter_Common.h"
#endif // ITF_GFX_ADAPTER_COMMON_H_
 
#ifndef ITF_RENDERINGBUFFER_DX9_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_RenderingBuffer_Directx9.h"
#endif //ITF_RENDERINGBUFFER_DX9_H_

#ifndef GPUCOUNTER_X360_H
#include "adapters\GFXAdapter_DirectX9\X360\GPUCounter_X360.h"
#endif //GPUCOUNTER_X360_H

#include <d3dx9.h>

#ifdef ITF_WINDOWS
#include <windows.h>
#endif

#ifndef ITF_DISABLE_WARNING
#define CHECK_SHADERS_VALIDITY
#endif // ITF_DISABLE_WARNING

#if !defined(ITF_FINAL) && defined(ITF_WINDOWS)
  #define USE_D3D_MEMORY_TRACKER
#endif

#ifdef ITF_X360
//#define _USE_ZBUFFER_TILING
#define _USE_ZBUFFER_FLOAT
#ifdef ITF_SUPPORT_DBGPRIM_TEXT
#include "x360/FontDebug_x360.h"
#endif
#endif

#ifdef ITF_WINDOWS
  #define ALLOW_DIRECTXEX_USE
#endif

namespace ITF
{

#define MAX_SAMPLER             16

const D3DVERTEXELEMENT9 VElem_PCT[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,   0 },
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PC2T[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,   0 },
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PCTBIBW[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,   0 },
    { 0, 16, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
    { 0, 20, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PNCTBIBW[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,       0 },
    { 0, 12, D3DDECLTYPE_UBYTE4N,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,			0 },
    { 0, 16, D3DDECLTYPE_UBYTE4N,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,		0 },
    { 0, 20, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,          0 },
    { 0, 24, D3DDECLTYPE_UBYTE4N,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,    0 },
    { 0, 28, D3DDECLTYPE_UBYTE4,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES,   0 },
    { 0, 32, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,       0 },
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PC[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,   0 },
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PT[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PCBT[] =
{
    {0,  0, D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION    , 0},
    {0, 12, D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR       , 0},
    {0, 16, D3DDECLTYPE_UBYTE4  , 0, D3DDECLUSAGE_BLENDINDICES, 0},
    {0, 20, D3DDECLTYPE_FLOAT2  , 0, D3DDECLUSAGE_TEXCOORD    , 0},
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PCB2T[] =
{
    {0,  0, D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION    , 0},
    {0, 12, D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR       , 0},
    {0, 16, D3DDECLTYPE_UBYTE4  , 0, D3DDECLUSAGE_BLENDINDICES, 0},
    {0, 20, D3DDECLTYPE_FLOAT2  , 0, D3DDECLUSAGE_TEXCOORD    , 0},
    {0, 28, D3DDECLTYPE_FLOAT2  , 0, D3DDECLUSAGE_TEXCOORD    , 1},
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PNCT[] =
{
    {0,  0, D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION    , 0},
    {0, 12, D3DDECLTYPE_UBYTE4N , 0, D3DDECLUSAGE_NORMAL      , 0},
    {0, 16, D3DDECLTYPE_UBYTE4N , 0, D3DDECLUSAGE_TANGENT     , 0},
    {0, 20, D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR       , 0},
    {0, 24, D3DDECLTYPE_FLOAT2  , 0, D3DDECLUSAGE_TEXCOORD    , 0},
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PNC3T[] =
{
    { 0,  0, D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION    , 0},
    { 0, 12, D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR       , 0},
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0},
    { 0, 24, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1},
    { 0, 40, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  2},
    { 0, 56, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  3},
    D3DDECL_END()
};

#ifdef GFX_NO_INSTANCING
const D3DVERTEXELEMENT9 VElem_QI_2TC[] =
{
    { 0,  0, D3DDECLTYPE_FLOAT2     , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    { 0,  8, D3DDECLTYPE_FLOAT2     , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
    { 0, 16, D3DDECLTYPE_FLOAT2     , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
    { 0, 24, D3DDECLTYPE_D3DCOLOR   , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR   , 0},
    D3DDECL_END()
};
#else
const D3DVERTEXELEMENT9 VElem_QI_2TC[] =
{
    { 0,  0, D3DDECLTYPE_FLOAT2     , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    { 1,  0, D3DDECLTYPE_FLOAT2     , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
    { 1,  8, D3DDECLTYPE_FLOAT2     , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
    { 1, 16, D3DDECLTYPE_D3DCOLOR   , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR   , 0},
#ifdef ITF_X360
    { 2,  0, D3DDECLTYPE_UBYTE4     , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
#endif
    D3DDECL_END()
};
#endif

const D3DVERTEXELEMENT9 VElem_T[] =
{
    { 0,  0, D3DDECLTYPE_FLOAT2     , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_I[] =
{
    { 0,  0, D3DDECLTYPE_UBYTE4     , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
    D3DDECL_END()
};

typedef struct PredicatedTilingParam_
{
    bbool   usePredicatedTiling;
    u32     backBufferFullWidth;
    u32     backBufferFullHeight;
    u32     tileCount;
    u32     TilingFlags;
    D3DRECT surfaceRect[2];
    u32     constantOffset;

} PredicatedTilingParam;

#ifdef USE_D3D_MEMORY_TRACKER

#define USE_D3D_MEMORY_CALLSTACK_TRACKER_FOR_NOTMANAGE
//#define USE_D3D_MEMORY_CALLSTACK_TRACKER_FOR_MANAGE
class D3DMemTracker
{
public:
    enum memTraceType
    {
        MEMTRACE_UNKNOW = 0,
        MEMTRACE_SURFACE,
        MEMTRACE_TEXTURE,
        MEMTRACE_VERTEXBUFFER,
        MEMTRACE_INDEXBUFFER,
        MEMTRACE_VERTEXDECLARATION,
        MEMTRACE_SHADER,

        MEMTRACE_COUNT,
    };

    #define MAX_STACK_DEPTH 20
    typedef struct memTrace_
    {
        memTrace_(uPtr _ptr = 0, memTraceType _type = MEMTRACE_UNKNOW, u32 _allocNum = 0)
        : ptr(_ptr), type(_type), allocNum(_allocNum), stackDepth(0)
        {
            ITF_MemSet(stack, 0, sizeof(stack));
        }

        bool operator == (const memTrace_ & _trace) { return (ptr == _trace.ptr);}

        uPtr ptr;
        memTraceType  type;
        u32 allocNum;
        uPtr stack[MAX_STACK_DEPTH];
        u32 stackDepth;
    } memTrace;


    D3DMemTracker();
    ~D3DMemTracker();
    void addRessource(uPtr _ptr, memTraceType _type, bbool _managed = bfalse);
    void releaseRessource(uPtr _ptr);
    void dump(bbool _onlyManagedRessource);

private:
    SafeArray<memTrace> m_traceArray;
    SafeArray<memTrace> m_traceNotManagedArray;
    u32                 m_globalNumAlloc;
    u32                 m_currentNumAlloc;
    Mutex               m_mxChangeArray;
};

//For not managed D3D ressource
  #define D3D_NM_RELEASE(p) if (p) { static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->getD3DMemTracker().releaseRessource((uPtr)(p)); /*u32 refLeft =*/ (p)->Release(); (p)=NULL; }

#else

  #define D3D_NM_RELEASE(p) if (p) { (p)->Release(); (p)=NULL;}

#endif

#ifdef ITF_SUPPORT_GPU_METRICS
class GFXAdapter_metrics_DX9
{
public :
    GFXAdapter_metrics_DX9();
    ~GFXAdapter_metrics_DX9();

    void     init(IDirect3DDevice9 * _pD3DDevice);

    void     enable()    { m_Enabled = btrue;  }
    void     disable()   { m_Enabled = bfalse; }

    void     activateGpuProfile(bbool _on);
    void     endDraw();

    bbool    isMetricsReady() const          { return m_Enabled; }
    bbool    isMetricsJustReady() const      { return isMetricsReady(); }

    void     getNewMetricsTag(u32 *_tag);

    void     startTag(u32 _tag);
    void     endTag(u32 _tag);

    u64      getMetricU64(u32 _metric, u32 _tag) const;
    f32      getMetricF32(u32 _metric, u32 _tag) const;

private:

    const static u32 c_metricsTagMax = 2048;

    bbool                           m_Enabled;
    ITF_VECTOR<PerfCounterManager*> m_TagArray;
    u32                             m_tagArrayIdx;

    IDirect3DDevice9*               m_pD3DDevice;
};
#endif //ITF_SUPPORT_GPU_METRICS

#define GFX_ADAPTER_DX9		(static_cast< GFXAdapter_Directx9* > (GFX_ADAPTER))

class GFXAdapter_Directx9 : public GFXAdapter_Common<GFXAdapter_Directx9>
{
public:
    typedef GFXAdapter_Common<GFXAdapter_Directx9> Super;

    enum
    {
#ifdef ITF_X360
        supportWireFrame = 0
#else
        supportWireFrame = 1
#endif
    };

    // type needed by GFXAdapter_Common
    typedef IDirect3DSurface9 PlatformSurface;

    void        init                    (   );
    void        destroy                 (   );
    void        destroyResources();
    void        project                 (   const Vec3d& _in3d, Vec2d & _out2d);


    void        clear                   (   u32 _buffer, u32 _Col = 0  );
    void        clear                   (   u32 _buffer, float _r=0.f, float _g=0.f, float _b=0.f, float _a=0.f);
    void        setAlphaBlend           (   GFX_BLENDMODE _Blend, GFX_ALPHAMASK _alphaMask    );
    void        setBlendSeparateAlpha   (   );
    void        setAlphaRef             (   int _ref=1 );
    void        setAlphaTest            (   bbool _enable );

    void        setCullMode             (   GFX_CULLMODE _cull    );
    void        setFillMode             (   GFX_FILLMODE _fillmode  );

    f32         internalDepth           (   f32 _z  );
    void        depthMask               (   u32 _depthMask    );
    void        depthFunc               (   u32 _func    );
    void        depthTest               (   u32 _test    );
    void        depthBias               (   f32 _offset, f32 slope);
    void        depthStencil            (   bbool stencilTestEnable, GFX_CMPFUNC stencilFunc,
                                            GFX_STENCILFUNC stencilZPass, GFX_STENCILFUNC stencilZFail, GFX_STENCILFUNC stencilFail,
                                            u8 ref, u8 preMask=0xff, u8 writeMask=0xff);

    void        colorMask               (   GFX_COLOR_MASK_FLAGS _colorMask = GFX_COL_MASK_ALL );

    void        setScissorRect          (   GFX_RECT* _clipRect);
    void        SetTextureBind          (   ux _Sampler, BasePlatformTexture *_Bind, bbool _linearFiltering = btrue);
	void		unbindAllSampler		(PlatformTexture *_texture);
    void        cleanupTexture          (   Texture* _texture);
    void        setClipPlane            (   bbool _enable, f32 _ax = 0.0f, f32 _by = 0.0f, f32 _cz = 0.0f, f32 _dw = 0.0f );


    void        setupViewport           (    GFX_RECT* _Viewport);
    void        unprojectPoint          (    const Vec2d& _in, float _viewDistance, const Matrix44* _MatrixModel, const Matrix44* _MatrixProj, const int* _View, Vec2d& _out);
    void        setLookAtMatrix         (    Matrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect);
    void        getViewport             (    GFX_Viewport & _vp);

    Matrix44    convertM44ToSpecificPlatformGPU(const Matrix44 &_in);

    /// GfxMaterials.
    void        prepareGfxMatReflection     (   const GFX_MATERIAL& _gfxMat );
    void        prepareGfxMatFluid          (   const GFX_MATERIAL& _gfxMat );

    /// Texture.
    bbool       loadTexture             (   Texture* _tex, const Path& _path); // overloaded from common to handle HRes tex
    bbool       loadTextureFromPlatformRawData(Texture* _t, const u8 * _rawData);

    void        createTexture           (   Texture* _texture, u32 _sizeX, u32 _sizeY, u32 _mipLevel, Texture::PixFormat _pixformat, u32 _pool = 1, bbool _dynamic = bfalse);
	RenderTarget* createRenderTarget	(   RenderTargetInfo &_info );
    void		setRenderContext		(   RenderContext &_context );
    void		prepareLockableRTAccess     (	RenderTarget* _target);
    void		lockRenderTarget        (	RenderTarget* _target, LOCKED_TEXTURE *_lockTex);
    void		unlockRenderTarget      (	RenderTarget* _target);
    void        cleanupBaseTexture      (   Texture* _texture);
    void        updateTextureSize       (   Texture* _Tex   );
    unsigned char* grabTexture          (   Texture* _texture   );
    u32         getMaxTextureSize       (   );
    void        loadFromMemory          (   const void * _buffer, const Size & _texture_size, Texture* _texture, Texture::PixFormat _pixel_format, bbool bCooked, u32 _dwMipMapCount);
    void        lockTexture             (   Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag );
    void        unlockTexture           (   Texture* _tex   );
    void        saveToMemory            (   void* _buffer, Texture* _Tex    );
    Size        getAdjustedSize         (   const Size& sz  );
    ITF_INLINE  float getTexelOffset    (   ) {return 0.5f;};
    void        setSamplerState         ( u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value );
    void        setTextureAdressingMode(u32 _samp, GFX_TEXADRESSMODE _texAdressModeU, GFX_TEXADRESSMODE _texAdressModeV);
    template <D3DSAMPLERSTATETYPE samplerAddressModeState>
    void        setTextureAdressingMode(u32 _samp, GFX_TEXADRESSMODE _texAdressMode);
    void        fillTexture             (   Texture* _Tex, u8 * _buffer, u32 _pitch, u32 _height );

    void        setInternalRenderTarget (   LPDIRECT3DSURFACE9 _RT, LPDIRECT3DSURFACE9 _ZRT);

    void*       enableTextureDyn        ( TextureDyn* _texture );
    void        disableTextureDyn       ( TextureDyn* _texture, void* _oldSurf );
    void        getTextureDynSurfaceLevel(TextureDyn* _texture );
    void        releaseTextureDynSurfaceLevel(TextureDyn* _texture);

    void        ApplyGamma              (   );
    void        ComputeGammaRamp        (float _fGamma, float _bright);
    void        drawDebug();

    /// Renderer.
    void        present                 (   );

    void        beginDisplay          (f32 _dt);
    void        endDisplay            (   );
    void        beginViewportRendering( View &_view  );
    void        endViewportRendering  ( View &_view  );
    void        beginSceneRendering   ( View &_view  );
    void        endSceneRendering     ( View &_view  );

    u32         isRender;
    void        showOverDrawRecord      (   RenderPassContext &_rdrCtx );
    void        OverDraw_ColorRemap     (   RenderPassContext &_rdrCtx );
    void        OverDraw_ComputeDepth   (   RenderPassContext &_rdrCtx );
    void        createOverDrawRamp      (   u32 _greenPos, u32 _yellowPos, u32 _redPos);
    void        showDebugDraw           (   RenderPassContext &_rdrCtx );
    void        applyRemoteScreenToFinalBackBuffer( RenderPassContext &_rdrCtx );

    /// Device.
    bbool       isDeviceLost            (   );
    void        lockDeviceLost          (   ) {m_mxDeviceLost.lock();}
    void        unlockDeviceLost        (   ) {m_mxDeviceLost.unlock();}
    bbool       resetDevice             (   );
    void        postD3DReset            (   );
    void        preD3DReset             (   );
    void        BuildPresentParams      (   );
    void        BuildPresentParamsWin32 (   );
    void        BuildPresentParamsX360  (   );

    /// Shaders.
    void        setVertexShader( void * _shaderBin );
    void        setPixelShader( void * _shaderBin );

    void        releaseShader           (   ITF_shader* _shader );
    bbool       loadShader              (   ITF_shader* _shader );
    void        constructBinShaderName  (   char* _shaderName, const char* _entryName, const char **_defineArray, u32 _numDef, bbool _isPixelShader);
    void*       loadBinVShader          (   u8* _binShader, ux _binSize);
    void*       loadBinPShader          (   u8* _binShader, ux _binSize);
    Path        getShaderCompiledPath   (   )
    {
#ifdef ITF_WINDOWS
        String8 sPath = mp_shaderManager.getShaderPath() + String8("compiled/win32/");
        return Path(sPath);
#else
        String8 sPath = mp_shaderManager.getShaderPath() + String8("compiled/x360/");
        return Path(sPath);
#endif
    }

    void        invokeExternalShaderCompiler(const char* _dataRootPath);
    void*       compilePixelShader(const char*_fxFileName, const char* _functionName, const char* _define[] = NULL, u32 _numDef = 0);
    void*       compileVertexShader(const char* _fxFileName, const char* _functionName, const char* _define[] = NULL, u32 _numDef = 0);
    void        computeD3DMacro(vector<D3DXMACRO> & _d3dmacro, const char* _define[], u32 _numDef, bbool _isPixel);
    bbool       supportShaderCompilation()
    { 
#ifdef ITF_WINDOWS
        return m_allowShaderCompil;
#else
        return bfalse;
#endif
    }

    bbool      supportExternalShaderCompilation() { return supportShaderCompilation(); }

    void        SetVertexShaderConstantF(u32 location, const f32 *floats, u32 count);
    void        SetPixelShaderConstantF(u32 location, const f32 *floats, u32 count);

#ifndef ITF_CONSOLE_FINAL
    virtual void reloadShaders();
#endif

    // Surface / RT
    void                getBackBuffer();
    void                setBackBuffer(bbool _setZbuffer = btrue, bbool _restoreBackBuffer = bfalse);

	RenderTarget*		getCurrentBackBuffer();
    LPDIRECT3DSURFACE9  getCurrentBackBufferSurf();
    LPDIRECT3DSURFACE9  getCurrentDepthStencilSurf();

    virtual bbool       isSupportingZBuffer() const { return btrue; }

    GFX_RenderingBuffer_DX9 *getBufferTextureDX(u32 _id) const
	{
		RenderTarget* buffer = getBufferTexture(_id);
		return buffer ? (GFX_RenderingBuffer_DX9 *)buffer->m_adapterimplementationData : NULL;
	}
    GFX_RenderingBuffer_DX9 *getExternalBufferTextureDX(u32 _view, u32 _id) const
	{
		return (GFX_RenderingBuffer_DX9 *)getExternalBufferTexture(_view, _id)->m_adapterimplementationData;
	}
    BasePlatformTexture *   getFrontLightTexture(eLightTexBuffer _currentLightBufferType) const;
    BasePlatformTexture *   getBackLightTexture(eLightTexBuffer _currentLightBufferType) const;
    GFX_RenderingBuffer_DX9 *   getFrontLightTextureDX(eLightTexBuffer _currentLightBufferType);
    GFX_RenderingBuffer_DX9 *   getBackLightTextureDX(eLightTexBuffer _currentLightBufferType);
    GFX_RenderingBuffer_DX9 *   getAdditionnalZBufferDX()
	{
		RenderTarget* rt = m_MediumZBuffer;
		return (GFX_RenderingBuffer_DX9 *)rt->m_adapterimplementationData;
	}
	GFX_RenderingBuffer_DX9 * getTargetDX(RenderTarget *target)
	{
		return (GFX_RenderingBuffer_DX9 *)target->m_adapterimplementationData;
	}

	void copyTexture( RenderPassContext & _rdrCtxt, RenderTarget *_source, RenderTarget *_destination);

/*    PlatformTexture *   getBufferTexture(u32 _id) const
	{
		return (PlatformTexture *) &m_AdditionnalRenderingBuffer[_id];
	}
    PlatformSurface *   getBufferSurface(u32 _id)
	{
		return m_AdditionnalRenderingBuffer[_id].getSurface();
	}
    PlatformTexture *   getExternalBufferTexture(u32 _view, u32 _id) const
	{
		return (PlatformTexture *)&m_AdditionnalExternalRenderingBuffer[_view][_id];
	}
    PlatformSurface *   getExternalBufferSurface(u32 _view, u32 _id)
	{
		return m_AdditionnalExternalRenderingBuffer[_view][_id].getSurface();
	}

    PlatformSurface *   getAdditionnalZBufferSurface( ) { return m_MediumZBuffer.getSurface(); }*/

    //Movie
    // should move to common at a tme...
    void        drawMovie               (PrimitiveContext &_primCtx, Matrix44* _matrix,f32 _alpha,ITF_VertexBuffer* _pVertexBuffer,Texture** _pTextureArray,u32 _countTexture);

    /// VertexBuffer.
    void        createVertexBuffer              (   ITF_VertexBuffer* _vertexBuffer );
    void        releaseVertexBuffer             (   ITF_VertexBuffer* _vertexBuffer );
    virtual void setVertexBuffer                (   ITF_VertexBuffer* _vertexBuffer );
    void        LockVertexBuffer                (   ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag );
    void        UnlockVertexBuffer              (   ITF_VertexBuffer* _vertexBuffer );

#ifdef ITF_SUPPORT_RESET_DEVICE
    void        VertexBufferManagerOnLostDevice (   );
    void        VertexBufferManagerOnResetDevice(   );
    void        VertexBufferOnResetDevice       (   ITF_VertexBuffer* _vertexBuffer );
    void        VertexBufferOnLostDevice        (   ITF_VertexBuffer* _vertexBuffer );
    void        IndexBufferManagerOnLostDevice  (   );
    void        IndexBufferManagerOnResetDevice (   );
    void        IndexBufferOnResetDevice        (   ITF_IndexBuffer* _indexBuffer );
    void        IndexBufferOnLostDevice         (   ITF_IndexBuffer* _indexBuffer );
#endif //ITF_SUPPORT_RESET_DEVICE

    /// IndexBuffer.
    void        createIndexBuffer               (   ITF_IndexBuffer* _indexBuffer );
    void        releaseIndexBuffer              (   ITF_IndexBuffer* _indexBuffer );
    void        setIndexBuffer                  (   ITF_IndexBuffer* _indexBuffer );
    void        LockIndexBuffer                 (   ITF_IndexBuffer* _indexBuffer, void** _data );
    void        UnlockIndexBuffer               (   ITF_IndexBuffer* _indexBuffer );

    void        setVertexFormat                 (   u32 _vformat    );


    // Vertex/Index Buffer Draw command.
    //  SetDraw*Command can be called only between beginDrawCall/endDrawcall.
    //  It can be called several times but it's only valid to change vertex/pixel shader constants between calls (cpu instancing).
    void                    SetDrawVertexBufferCommand(PRIMITIVETYPE _type, u32 _vertexStart, u32 _vertexNumber);
    void                    SetDrawIndexedVertexBufferCommand(PRIMITIVETYPE _type, u32 _indexNumber, u32 _startVertex = 0);
    void                    SetDrawPrimitiveCommand(PRIMITIVETYPE _type, const void * _p_Vertex, u32 _NumberVertex);
    void                    SetDrawInstancedVertexBufferCommand(PRIMITIVETYPE _type, u32 _indexNumber, u32 _startVertex = 0);

    void                        createRenderTarget(LPDIRECT3DTEXTURE9* _ptex, LPDIRECT3DSURFACE9* _psurf, u32 _width, u32 height, D3DFORMAT _format, LPDIRECT3DSURFACE9 _shiftSurf = NULL, IDirect3DTexture9 *_pSrcBaseTexture = NULL, IDirect3DTexture9 *_pLinkedTexture = NULL);
    void                        copyCurrentColorBuffer(u32 _rt);


    // AFX PostProcess function
    static const f32            cRadialBlurScaleSize;


public:
    virtual  u32                getRemoteScreenWidth    (   ) const;
    virtual  u32                getRemoteScreenHeight   (   ) const;
    virtual  u32                getLightPassWidth     (   ) const { return getScreenWidth() / LIGHTS_BFR_RATIO;}
    virtual  u32                getLightPassHeight     (   ) const { return getScreenHeight() / LIGHTS_BFR_RATIO;}
    virtual  u32                getQrtLightPassWidth     (   ) const { return getScreenWidth() / QRT_LIGHTS_BFR_RATIO;}
    virtual  u32                getQrtLightPassHeight     (   ) const { return getScreenHeight() / QRT_LIGHTS_BFR_RATIO;}
    virtual  u32                getReflectionPassWidth     (   ) const { return getScreenWidth() / REFLECTION_BFR_RATIO;}
    virtual  u32                getReflectionPassHeight     (   ) const { return getScreenHeight() / REFLECTION_BFR_RATIO;}
    virtual  u32                getRefractionPassWidth     (   ) const {return getScreenWidth() / REFRACTION_BFR_RATIO;}
    virtual  u32                getRefractionPassHeight     (   ) const { return getScreenHeight() / REFRACTION_BFR_RATIO;}
    virtual  u32                getFluidPassWidth     (   ) const {return getScreenWidth() / FLUID_GLOW_BFR_RATIO;}
    virtual  u32                getFluidPassHeight     (   ) const { return getScreenHeight() / FLUID_GLOW_BFR_RATIO;}


#ifdef ITF_USE_REMOTEVIEW
    LPDIRECT3DSURFACE9          getBackBufferSurf(u32 _viewID);
    Vec2d&                      transformScreenToRemoteViewCordinate(Vec2d& _pos);
    void                        computeEngineViewRect(RECT& _mainOrigin, RECT& _mainTop, RECT& _remoteOrigin, RECT& _remoteBottom);
#ifndef ITF_FINAL
    void DRCThread();
#endif // ITF_FINAL
    private:
        ITF_THREAD_CRITICAL_SECTION m_csDRC;
        class Thread * m_drcThread;
        bbool m_quitDRCThread;
        static const u32 maxRingSurface = 3;
        u32 m_currentIndexRing;
        bbool m_newBufferSet;
        ITF_THREAD_EVENT                m_RemoteExitEvent;
        IDirect3DSurface9* m_RingSurface[maxRingSurface];
#endif //ITF_USE_REMOTEVIEW

    void                        resolve(RenderTarget* _target);

    void                        beginGPUEvent(const char* _name, GPU_MARKER _marker = GFX_GPUMARKER_Default);
    void                        endGPUEvent(GPU_MARKER _marker = GFX_GPUMARKER_Default);
    f32                         m_GPURasters[2][GFX_GPUMARKER_COUNT];
    __int64                     m_CPURasterStart;
    __int64                     m_CPURasterStop;
    f32                         m_CPURaster;

#ifdef ITF_PICKING_SUPPORT
    void                        setRenderTarget_BackBuffer();
/*    void                        setRenderTarget_Picking();
    Color                       getBufferValue(u32* _Pixels,u32 _Pitch,u32 _posX, u32 _posY);
    void*                       resolvePicking();
    u32*                        lockPickingBuffer(void* pSurface,u32& _pitch);
    void                        unlockPickingBuffer(void* pSurface);*/
#endif //ITF_PICKING_SUPPORT


	/// Tools / Debug
    void            startScreenshotMode() override;
    void            endScreenshotMode() override;
    void            startProfilerFPScaptureMode();
    void            endProfilerFPScaptureMode();
#ifdef ITF_SUPPORT_VIDEOCAPTURE
    void            doScreenCapture();
    void            startScreenCapture(const Path *_directory);
#endif// ITF_SUPPORT_VIDEOCAPTURE

    virtual void copyRenderTargetToScreenshot(RenderPassContext & _rdrCtxt, ux _width, ux _height);
    virtual void copyScreenshotSurfaceToRGBABuffer(u32 * _buffer, ux _width, ux _height);
    virtual void releaseScreenshotSurface();

public:
#ifdef ITF_SUPPORT_DBGPRIM_TEXT
    void            createDebugFonts(u32 _screenwidth, u32 _screenHeight);
#endif

#ifdef ITF_SUPPORT_DBGPRIM_TEXT
    bbool           drawDBGTextSize( const String8 &_text, f32 &_width, f32 &_height, bbool _useBigFont = bfalse, bbool _useFixedFont = bfalse );
    u32             displayDebugText( const DBGText &_text );
#endif // ITF_SUPPORT_DBGPRIM_TEXT

    void            displayDebugInfoWin32(DrawCallContext &_drawCallCtx);
    void            displayDebugInfoX360(DrawCallContext &_drawCallCtx);

    void *          getRenderingDevice() { return m_pd3dDevice; }
    
    bbool       createDevice                    ( bbool _fullscreen, void* _hwnd);
    bbool       createDXDeviceWin32             (   );
    bbool       createDXDeviceX360              (   );

                GFXAdapter_Directx9             (   );
                ~GFXAdapter_Directx9            (   ) {};

    String8     getRenderName                   (   )   { return "DX9";};

    void  setFullScreenMode(bbool _bMode)       {m_fullscreen = _bMode;}

#ifdef ITF_X360
    void        buildGammaRamp( D3DGAMMARAMP& scaledRamp, Vec3d gamma, f32 brigthnessValue, f32 contrastValue );
    void        setGammaRamp(Vec3d gamma,f32 brigthnessValue=1.0f,f32 contrastValue=1.0f);

    void        acquireDeviceOwnerShip();
    void        releaseDeviceOwnerShip();
    void        minimal_present();
    void        beginTiling(Color &_clearColor);
    void        endTiling(LPDIRECT3DTEXTURE9 _ptex = NULL);
    void        forceRenderToAllTile(bbool _force);
    void        copyTextureToEDRAM(RenderTarget* _pSrcTex, RenderTarget* surfDst, bbool _blurTarget = bfalse, f32 _blurSize = 1.0f);
#endif //ITF_X360

#ifdef USE_D3D_MEMORY_TRACKER
    D3DMemTracker& getD3DMemTracker() { return m_D3DMemTracker; }
#endif

#ifdef ITF_SUPPORT_GPU_METRICS
    GFXAdapter_metrics_DX9 m_Metrics;

    void     activateGpuProfile(bbool _on)                   { m_Metrics.activateGpuProfile(_on); }

    bbool    isMetricsJustReady() const                       { return m_Metrics.isMetricsJustReady(); }

    void     getNewMetricsTag(u32 *_tag)                      { m_Metrics.getNewMetricsTag(_tag); }

    void     startMetricsTag(u32 _tag)                        { m_Metrics.startTag(_tag); }
    void     endMetricsTag(u32 _tag)                          { m_Metrics.endTag(_tag); }

    u64      getMetricU64(u32 _metric, u32 _tag) const        { return m_Metrics.getMetricU64(_metric, _tag); }
    f32      getMetricF32(u32 _metric, u32 _tag) const        { return m_Metrics.getMetricF32(_metric, _tag); }

#endif // ITF_SUPPORT_GPU_METRICS

 protected:
    DWORD fontAlignmentToDirectXFormat(FONT_ALIGNMENT _alignment);
    void updateCameraFrustumPlanes(Camera* _cam);

#if defined(ITF_X360) && defined(GFX_COMMON_POSTEFFECT)
	virtual void getBackBufferAfterOffset(u32 &_afterBackEDRAMOffset, u32 &_afterLightEDRAMOffset);
	virtual u32 getBufferSizeOffset(RenderTarget *_target);
    virtual void setShaderGPRAllocation(u32 VertexShaderCount, u32 PixelShaderCount);
//    virtual void PrepareBlurTarget(RenderPassContext & _rdrCtxt, u32 _pixelBlurSize, u32 _quality);
    void blur4x4(RenderPassContext & _rdrCtxt, RenderTarget* _Src, RenderTarget* _Dst, RenderTarget* _QrtIntermediateBuffer, f32 _pixelSize = 4.0f, bbool _resolveDst = bfalse );
#endif

#ifndef GFX_COMMON_POSTEFFECT
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

	RenderTarget* createTarget(GFXAdapter_Directx9 *_gfxAdapter, u32 _width, u32 height, Texture::PixFormat _pixformat, u32 _flags = 0, u32 _shiftSurfSize = 0, RenderTarget *_pSrcBaseTexture = NULL, RenderTarget *_pLinkedTexture = NULL );

	//  Additional RT
    RenderTarget*				m_AdditionnalRenderingBuffer[NB_TEX_BFR];
    RenderTarget*				m_AdditionnalExternalRenderingBuffer[2][NB_TEX_EXT_BFR];
    RenderTarget*				m_DebugRenderingBuffer;

    RenderTarget*				m_MediumZBuffer;

    RenderTarget				*m_currentAFXBlurTexture;
    RenderTarget				*m_currentAFXSceneTexture;

	void createAdditionalRenderTarget();
    void releaseAdditionalRenderTarget();

    void                        PrepareBlurTarget(RenderPassContext & _rdrCtxt, u32 _pixelBlurSize, u32 _quality);
    void                        AFXpostProcessPass(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess);
    void                        AFXPostProcessKaleiPass(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess);
    void                        BlurRenderBuffer(RenderPassContext & _rdrCtxt, u32 _pixelBlurSize, u32 _quality, RenderTarget* _srcTex,
                                                 RenderTarget* _dstTex, RenderTarget* _tmpTex = NULL);
    void                        fillHexagoneVertexBuffer(VertexPCT* _data, u32 _startIndex, Vec2d _position, f32 _hexagoneSize, const Vec2d& _uv1, const Vec2d& _uv2, const Vec2d& _uv3 );

    RenderTarget*		getBufferTexture(u32 _id) const
	{
		return m_AdditionnalRenderingBuffer[_id];
	}
    RenderTarget*		getExternalBufferTexture(u32 _view, u32 _id) const
	{
		return m_AdditionnalExternalRenderingBuffer[_view][_id];
	}

    f64                         m_AFXTimer;

	#ifndef ITF_FINAL
		String8 m_dbgTxtAfterFX;
	#endif // ITF_FINAL

    void                        DrawAFXPostProcess(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess);

    void                        blurFluidRenderBuffer(RenderPassContext & _rdrCtxt);
    void                        blurFrontLightRenderBuffer(RenderPassContext & _rdrCtxt, u32 _quality, u32 _size);
    void                        blurBackLightRenderBuffer(RenderPassContext & _rdrCtxt, u32 _quality, u32 _size);
    void                        restoreBackLightRenderBuffer(RenderPassContext & _rdrCtxt, eLightTexBuffer _currentLightBufferType);
#endif

	RenderTarget*				getSceneTexture(u32 _rt = 0, f32 _scale = 1.0f, bbool _use2x2Target = bfalse);

#ifdef ITF_X360
    void                        downSample4x4(RenderPassContext & _rdrCtxt, RenderTarget* _Src, RenderTarget* _QrtBuffer);
#endif

public:
    f32                         getCentroidOffset() const { return 0.5f; }


#ifdef ITF_USE_REMOTEVIEW
    void createRemoteRenderTargetSurfaces();
    void releaseRemoteRenderTargetSurfaces();
#endif

    virtual void        setRenderTargetForPass(GFX_ZLIST_PASS_TYPE _passType, bbool _restoreContext = bfalse);
    virtual void        resolveRenderTargetForPass(GFX_ZLIST_PASS_TYPE _passType);
    virtual void        copyDebugRenderTarget(f32 _alpha, const Vec2d & _pixelOffset);
    virtual void        copyPassToDebugRenderTarget(GFX_ZLIST_PASS_TYPE _passType);
    virtual void        copyPickingToDebugRenderTarget();
            void        copySurfaceToDebugRenderTarget(RenderTarget *_sourceRT);
    virtual u32*        lockDebugRenderTarget(u32 & _width, u32 & _height, u32 & _pitch);
    virtual void        unlockDebugRenderTarget();

	RenderTarget*		createRenderTarget( IDirect3DSurface9 *_surf );

    virtual void        copyScene();
    virtual void        drawScreenTransition(f32 _fadeVal);

#ifdef ITF_WINDOWS
    virtual void	    getAvailableResolutions(vector<pair<u32, u32>>& _resolutions);
#endif //ITF_WINDOWS
   
public:
    // D3DX function redefinition
    HRESULT DXCreateTexture(u32 _Width, u32 _Height, u32 _level, u32 _usage, D3DFORMAT _d3dFormat, D3DPOOL Pool, IDirect3DTexture9 **ppTexture);
    void    DXReleaseTexture(IDirect3DTexture9 **ppTexture);
    HRESULT DXCreateLinkTexture(IDirect3DTexture9 *_pSrcBaseTexture, IDirect3DTexture9 *_pLinkedTexture, u32 _Width, u32 _Height, u32 _level, u32 _usage, D3DFORMAT _d3dFormat, D3DPOOL _Pool, IDirect3DTexture9 **_ppTexture);
    void    DXReleaseLinkTexture(IDirect3DTexture9 **ppTexture);
    HRESULT DXGetSurfaceLevel(IDirect3DTexture9* _pTexture, u32 _level, IDirect3DSurface9 **_ppSurfaceLevel);
    HRESULT DXCreateVertexDeclaration(const D3DVERTEXELEMENT9 *_pVertexElements, IDirect3DVertexDeclaration9 **_ppDecl);
    HRESULT DXCreateVertexShader(const DWORD *pFunction, IDirect3DVertexShader9 **ppShader);
    HRESULT DXCreatePixelShader(const DWORD *pFunction, IDirect3DPixelShader9 **ppShader);
    HRESULT DXCreateVertexBuffer(u32 _Lenght, u32 _usage, u32 _FVF, D3DPOOL _pool, IDirect3DVertexBuffer9 **_ppVertexBuffer, HANDLE *_pSharedHandle = NULL);
    HRESULT DXCreateIndexBuffer(u32 _Lenght, u32 _usage, D3DFORMAT Format, D3DPOOL _pool, IDirect3DIndexBuffer9 **_ppIndexBuffer, HANDLE *_pSharedHandle = NULL);
#ifndef ITF_X360
    HRESULT DXCreateSprite(LPD3DXSPRITE * _spriteInterface);
    HRESULT DXCreateRenderTarget(u32 _Width, u32 _Height, D3DFORMAT _d3dFormat, D3DMULTISAMPLE_TYPE _MultiSample, u32 _MultisampleQuality, bbool _Lockable, IDirect3DSurface9 **_ppSurface, HANDLE *_pSharedHandle = NULL);
    HRESULT DXCreateDepthStencilSurface(u32 _Width, u32 _Height, D3DFORMAT _d3dFormat, D3DMULTISAMPLE_TYPE _MultiSample, u32 _MultisampleQuality, bbool _Discard, IDirect3DSurface9 **_ppSurface, HANDLE *_pSharedHandle = NULL);
    HRESULT DXCreateOffscreenPlainSurface(u32 _Width, u32 _Height, D3DFORMAT _d3dFormat, D3DPOOL Pool, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle = NULL);
#else
    HRESULT DXCreateRenderTarget(u32 _Width, u32 _Height, D3DFORMAT _d3dFormat, D3DMULTISAMPLE_TYPE _MultiSample, u32 _MultisampleQuality, bbool _Lockable, IDirect3DSurface9 **_ppSurface, CONST D3DSURFACE_PARAMETERS *pParameters);
    HRESULT DXCreateDepthStencilSurface(u32 _Width, u32 _Height, D3DFORMAT _d3dFormat, D3DMULTISAMPLE_TYPE _MultiSample, u32 _MultisampleQuality, bbool _Discard, IDirect3DSurface9 **_ppSurface, CONST D3DSURFACE_PARAMETERS *pParameters = NULL);
#endif
protected:
    LPDIRECT3D9             m_pD3D; // Used to create the D3DDevice
    LPDIRECT3DDEVICE9       m_pd3dDevice; // Our rendering device
    bbool                   m_useD3D9Ex;

    /// Parameters.
    D3DPRESENT_PARAMETERS   m_ppars;
    D3DFORMAT               m_depthStencilFormat;
    HWND                    m_hwnd;
    bbool                   m_fullscreen;

#ifdef ITF_WINDOWS
    bbool                   m_previousDisableRendering2d;
#endif

    D3DXVECTOR3             m_lastTransformedRotationCenter;

    RenderTarget*			m_BackBuffer;
    RenderTarget*			m_backBufferNoTile;

#ifdef ITF_USE_REMOTEVIEW
    RenderTarget*			m_BackBufferScreen[e_engineView_Count];
    RenderTarget*			m_DepthStencilScreen[e_engineView_Count];
#else
    RenderTarget*			m_DepthStencil;
#endif

	static const ux		nbrBufOD = 6;
	GFX_RenderingBuffer_DX9 m_overDrawBuffers[nbrBufOD];
	f32					m_overDrawDepth;

#ifdef ITF_X360
    LPDIRECT3DTEXTURE9          m_pFrontBufferTexture[2];
    u32                         m_currFrontBuffer;
    PredicatedTilingParam       m_TilingParam;
    bbool                       m_isPredicatedTilingPass;
    u8*                         m_shaderMemoryPoolBuffer;
    u8*                         m_shaderMemoryPoolPtr;
    static const u32            c_shaderMemoryPoolSize;

#endif

#ifdef ITF_WINDOWS
#ifdef ALLOW_DIRECTXEX_USE
    D3DDISPLAYMODEEX*           m_displayModeEx;
#endif
    D3DDISPLAYMODE*             m_displayMode;

    u32                         m_displayModeCount;
    u32                         m_displayModeIndex;
    LPD3DXSPRITE                m_spriteInterface;
#endif //ITF_WINDOWS

    u32                     m_preUHDScreenWidth, m_preUHDScreenHeight;

#ifdef ITF_X360
    bbool						m_askToReleaseOwnerShip;
    ITF_THREAD_EVENT            m_AcquireOwnerShipEvent;
    ITF_THREAD_EVENT            m_ReleaseOwnerShipEvent;
#endif

    LPDIRECT3DSURFACE9			m_debugBufferOffscreenSurf[2];
    u32							m_debugBufferOffscreenSwap;

    RenderTarget*				m_RendereringBufferPicking;
    LPDIRECT3DSURFACE9          m_pPickingOffScreenBufferSurf[2];
    u32                         m_PickingBufferSwap;

    RenderTarget*				m_asyncScreenshotBuffer;
    LPDIRECT3DSURFACE9          m_asyncScreenshotOffscreenSurf;

	GFX_RenderingBuffer_DX9*    m_currentRenderTarget[4];
	GFX_RenderingBuffer_DX9*    m_currentDepthBuffer;

    LPDIRECT3DSURFACE9          m_currentRenderingSurface;

    LPDIRECT3DTEXTURE9          m_overDrawRamptexture;

    SafeArray<PlatformTexture*, MAX_SAMPLER> m_textureBinded;
    SafeArray<bbool, MAX_SAMPLER>          m_textureBindedFiltering;

    ITF_THREAD_CRITICAL_SECTION   m_csPresent;//CS present
    Mutex                         m_mxDeviceLost;

#ifdef ITF_SUPPORT_DBGPRIM_TEXT
#ifdef ITF_X360
    Fontdebug               m_FontDebug;
#else
    /// Fonts Debug.
    ID3DXFont*              mp_FontDebug;
    ID3DXFont*              mp_FontDebug_Monospace;
    ID3DXFont*              mp_FontDebug_Big;
#endif
#endif

    // Vertex Declarations.
    bbool			createVertexDeclaration (   );
    void			releaseVertexDeclaration(   );

    IDirect3DVertexDeclaration9* mp_VDcl_PCT;
    IDirect3DVertexDeclaration9* mp_VDcl_PC2T;
    IDirect3DVertexDeclaration9* mp_VDcl_PCTBIBW;
    IDirect3DVertexDeclaration9* mp_VDcl_PNCTBIBW;
    IDirect3DVertexDeclaration9* mp_VDcl_PC;
    IDirect3DVertexDeclaration9* mp_VDcl_PT;
    IDirect3DVertexDeclaration9* mp_VDcl_PCBT;
    IDirect3DVertexDeclaration9* mp_VDcl_PCB2T;
    IDirect3DVertexDeclaration9* mp_VDcl_PNCT;
    IDirect3DVertexDeclaration9* mp_VDcl_PNC3T;
    IDirect3DVertexDeclaration9* mp_VDcl_QI_2TC;
    IDirect3DVertexDeclaration9* mp_VDcl_T;
    IDirect3DVertexDeclaration9* mp_VDcl_I;
    
    ///! What the renderer thinks the max texture size is.
    u32         m_maxTextureSize;
    ///! whether the hardware supports non-power of two textures
    bbool       m_supportNPOTTex;
    ///! whether the hardware supports non-square textures.
    bbool       m_supportNonSquareTex;

    void messageD3DError(HRESULT hr);
    
#ifdef USE_D3D_MEMORY_TRACKER
    D3DMemTracker m_D3DMemTracker;
#endif

    SafeArray<ShaderFogParamVS,2> fogBindedVS;
    SafeArray<ShaderFogParamPS,2> fogBindedPS;

    // Gamma
    float                       mf_Gamma;           // requested gamma. gamma is changed at the end of Swap.
    float                       mf_CurrentGamma;    // currently set gamma.
    float                       mf_Bright;          // for gamma rmap computation.
    float                       mf_CurrentBright;
    _D3DGAMMARAMP               mtt_GammaRamp;
};

} // namespace ITF



#endif // _ITF_DIRECTX9_DRAWPRIM_H_

#endif //!defined (ITF_WINDOWS) || defined (ITF_X360)
