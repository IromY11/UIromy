#include "precompiled_GFXAdapter_OpenGLES2.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif // _ITF_SINGLETONS_H_

#ifndef _ITF_OPENGLES2_DRAWPRIM_H_
#include "adapters/GFXAdapter_OpenGLES2/GFXAdapter_OpenGLES2.h"
#endif // _ITF_OPENGLES2_DRAWPRIM_H_

#ifndef __MINILZO_H
#include "core/lzo/minilzo.h"
#endif //__MINILZO_H

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifdef ITF_WINDOWS
#include "../../../extern/CaptureJpeg/Source/INTERFCE.h"
#include <shlobj.h>
#endif

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/RastersManager.h"
#endif //_ITF_RASTERSMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

// Fix pourri pour le recalcul des windComponent....
// Le WindComponent update ne devrait pas etre fait comme ça
#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_WINDCOMPONENT_H_
#include "gameplay/components/misc/WindComponent.h"
#endif //_ITF_WINDCOMPONENT_H_


// no guard -> should be included once only !
#include "engine/AdaptersInterfaces/GFXAdapter_Common_Impl.h"

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_


int gDefaultFrameBuffer=0;


#ifndef ITF_CORE_UNICODE_TOOLS_H_
#include "core/UnicodeTools.h"
#endif // ITF_CORE_UNICODE_TOOLS_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

namespace ITF
{
    // explicit instantiation.
    template class GFXAdapter_Common<GFXAdapter_OpenGLES2>;

    // Statics.

    const u32	GFXAdapter_OpenGLES2::LIGHTS_BFR_RATIO     = 4;
    const u32	GFXAdapter_OpenGLES2::QRT_LIGHTS_BFR_RATIO = 4;
    const u32	GFXAdapter_OpenGLES2::REFLECTION_BFR_RATIO = 2;
    const u32	GFXAdapter_OpenGLES2::REFRACTION_BFR_RATIO = 2;
    const u32	GFXAdapter_OpenGLES2::FLUID_BLUR_BFR_RATIO = 2;
    const u32	GFXAdapter_OpenGLES2::FLUID_GLOW_BFR_RATIO = 8;

	// ------------------------------------------------------------------------------

#ifdef _HARDCODED_SHADER

	static const char gVertexShaderPCT[] = 
		"attribute vec4 vPosition;\n"
		"attribute vec4	myUV;\n"
		"attribute vec4	COLOR;\n"
		"uniform mat4	myPMVMatrix;\n"
		"varying highp vec2	myTexCoord;\n"
		"varying lowp vec4	myColor;\n"
		"void main() {\n"
		"gl_Position = (myPMVMatrix * vPosition);\n"
		"myColor = vec4(COLOR.r,COLOR.g,COLOR.b,COLOR.a);\n" //b=0 a=1 r =1 g=0;
		"myTexCoord = myUV.st;\n"
		"}\n";

	static const char gFragmentShaderPCT[] = 
		"uniform sampler2D sampler2d;\n"
		"varying highp vec2	myTexCoord;\n"
		"varying lowp vec4	myColor;\n"
		"void main() {\n"
		//"gl_FragColor = texture2D(sampler2d, myTexCoord - vec2(0.5/320.0, 0.5/240.0)) * myColor;\n"
		"gl_FragColor = texture2D(sampler2d, myTexCoord ) * myColor;\n" 
        //"gl_FragColor = texture2D(sampler2d, myTexCoord ) + vec4(0.2,0.2,0,1);\n"
		//"gl_FragColor = myColor;\n"
		//"gl_FragColor = texture2D(sampler2d, myTexCoord );\n"
        //"gl_FragColor = vec4(gl_FragColor.b,gl_FragColor.g,gl_FragColor.r,gl_FragColor.w);\n"
		"}\n";

		static const char gVertexShaderPC[] = 
			"attribute vec4 vPosition;\n"
			"attribute vec4	COLOR;\n"
			"uniform mat4	myPMVMatrix;\n"
			"varying lowp vec4	myColor;\n"
			"void main() {\n"
			"gl_Position = (myPMVMatrix * vPosition);\n"
			"myColor = vec4(COLOR.r,COLOR.g,COLOR.b,COLOR.a);\n" //b=0 a=1 r =1 g=0;
			"}\n";

		static const char gFragmentShaderPC[] = 
			"uniform sampler2D sampler2d;\n"
			"varying highp vec4	myColor;\n"
			"void main() {\n"
			"gl_FragColor = myColor;\n"
			"}\n";

#endif

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::doBenchMark()
	{
		if (0 == nbBenchMarkPass)
		{
			curTimeBenchMark = 0.f;
			startTimeBenchMark = SYSTEM_ADAPTER->getTime();
		}
		else if (isBenchOver())
			return;

#if 1
		RenderPassContext rdrCtx;
		PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
		DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

		i32 Width = getScreenWidth(); 
		i32 Height = getScreenHeight();
			
		setShaderGroup(m_defaultShaderGroup);
		setDefaultGFXPrimitiveParam();
		setGfxMatDefault(drawCallCtx);

		getWorldViewProjMatrix()->push();
		setOrthoView(0.f, Width, 0.f, Height);
		setupViewport(0, 0, Width, Height);
            
		bindTexture(0, m_whiteOpaqueTexture, btrue);
        setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
            
		drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA).setDepthTest(bfalse);

        colorMask(GFX_COL_MASK_ALL);

		for (u32 i = 1; i < 10; i++)
		{
			double time = SYSTEM_ADAPTER->getEllaspedFromStart();
			drawScreenQuad(drawCallCtx, 0.f + sinf(time * 1.f) * 50.f * i, + sinf(time * 4.f) * 60.f * i, Width, Height, 0.f, COLOR_WHITE);
		}

		clear(GFX_CLEAR_COLOR, 0.f, 0.f, 0.f, 0.f);

		// restore camera.
		View::getCurrentView()->apply();
		getWorldViewProjMatrix()->pop();
	
		f64 getTime = SYSTEM_ADAPTER->getTime();
		curTimeBenchMark += (getTime - startTimeBenchMark) * 10;
		
		startTimeBenchMark = getTime;

		nbBenchMarkPass++;
#endif	
	}

	// ------------------------------------------------------------------------------
    void GFXAdapter_OpenGLES2::TextureBuffer::Init(u32 _width, u32 _height,/*GX2SurfaceFormat _format,*/ bbool _ftv)
    {
        m_surfWidth = _width;
        m_surfHeight = _height;
        
        glGenTextures(1, (GLuint*)&m_textureId);
        glBindTexture(GL_TEXTURE_2D, m_textureId);
        
        
        u32 mipIndex = 0;
        GLint internalFmt = GL_RGBA;
        GLenum format = GL_RGBA;
        GLenum type = GFX_ADAPTER_OPENGLES2->m_direct2BackBuffer ? GL_UNSIGNED_SHORT_4_4_4_4 : GL_UNSIGNED_BYTE;
//        u32 size = getWidth() * getHeight() * 4; // 4 = RGBA
        
        glTexImage2D(GL_TEXTURE_2D, mipIndex, internalFmt, getWidth(),  getHeight(), 0, format, type, 0/*(GLvoid*) image_data*/);
        // These 2 MUST BE set on IOS or the texture cannot be used
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
		m_textureSampled.hwdData = m_textureId;
    }
	// ------------------------------------------------------------------------------

	GLenum GFXAdapter_OpenGLES2::PrimitiveToGLES2(PRIMITIVETYPE _v)
	{
		switch(_v)
		{
			default:
				ITF_ASSERT_MSG(0, "Unknown primitive type");
			case GFX_TRIANGLES:
				return GL_TRIANGLES;
			case GFX_LINES:
				return GL_LINES;
			case GFX_LINE_STRIP:
				return GL_LINE_STRIP;
			case GFX_TRIANGLE_STRIP:
				return GL_TRIANGLE_STRIP;
			case GFX_TRIANGLE_FAN:
				return GL_TRIANGLE_FAN;
		}
	}

	// ------------------------------------------------------------------------------

	u32 GFXAdapter_OpenGLES2::VertexFormat_TO_StructSize(u32 _vformat)
	{
		switch(_vformat)
		{
		case VertexFormat_P:
			ITF_ASSERT(bfalse);
			return 0;
		case VertexFormat_PCT:
			return sizeof(VertexPCT);
		case VertexFormat_PC2T:
			return sizeof(VertexPC2T);
		case VertexFormat_PNCT:
			return sizeof(VertexPNCT);
		case VertexFormat_PNC3T:
			return sizeof(VertexPNC3T);
		case VertexFormat_PC:
			return sizeof(VertexPC);
		case VertexFormat_PT:
		case VertexFormat_PTa:
			return sizeof(VertexPT);
		case VertexFormat_PCBT:
			return sizeof(VertexPCBT);
		case VertexFormat_PCB2T:
			return sizeof(VertexPCB2T);
		case VertexFormat_PCTBIBW:
			return sizeof(VertexPCTBIBW);
		case VertexFormat_QuadInstance_2TC:
			return sizeof(VertexQI_2TC)/4;
		case VertexFormat_T:
			return sizeof(VertexT);

		default:
			ITF_ASSERT_MSG(0, "Primitive not recognized");
			return 0;
		}
	}

	// ------------------------------------------------------------------------------
#if 0
	void GFXAdapter_OpenGLES2::SamplerManager::create()
	{
	ITF_ASSERT_STATIC((u32)GX2_TEX_XY_FILTER_POINT == 0, "SamplerManager will not work");
		ITF_ASSERT_STATIC((u32)GX2_TEX_XY_FILTER_BILINEAR == 1, "SamplerManager will not work");

		ITF_ASSERT_STATIC((u32)GFX_TEXADRESSMODE_WRAP == 0, "SamplerManager will not work");
		ITF_ASSERT_STATIC((u32)GFX_TEXADRESSMODE_MIRROR == 1, "SamplerManager will not work");
		ITF_ASSERT_STATIC((u32)GFX_TEXADRESSMODE_CLAMP == 2, "SamplerManager will not work");
		ITF_ASSERT_STATIC((u32)GFX_TEXADRESSMODE_BORDER == 3, "SamplerManager will not work");

		u32 id = 0;
		for(u32 clampX=0 ; clampX<NB_CLAMP ; ++clampX)
		{
			for(u32 clampY=0 ; clampY<NB_CLAMP ; ++clampY)
			{
				for(u32 filter=0 ; filter<NB_FILTER ; ++filter)
				{
					//ITF_ASSERT_CRASH(id == getSamplerID((GFX_TEXADRESSMODE)clampX, (GFX_TEXADRESSMODE)clampY, (GX2TexXYFilterType)filter), "SamplerManager will not work");
					//GX2InitSampler(&m_samplers[id], GX2_TEX_CLAMP_CLAMP, GX2_TEX_XY_FILTER_POINT);
					//GX2InitSamplerClamping(&m_samplers[id], GFX_2_GX2_AddressModeConvert((GFX_TEXADRESSMODE)clampX), GFX_2_GX2_AddressModeConvert((GFX_TEXADRESSMODE)clampY), GX2_TEX_CLAMP_WRAP);
					//GX2InitSamplerXYFilter(&m_samplers[id], (GX2TexXYFilterType)filter, (GX2TexXYFilterType)filter, GX2_TEX_ANISO_1_TO_1);
					id++;
				}
			}
		}
	}
#endif

	// ------------------------------------------------------------------------------
	// GFXAdapter_OpenGLES2
	// ------------------------------------------------------------------------------

    GFXAdapter_OpenGLES2::~GFXAdapter_OpenGLES2()
    {
    }
        
	GFXAdapter_OpenGLES2::GFXAdapter_OpenGLES2()
	{
		#ifdef GFX_USE_LIGHTING
			setUsingLighting(btrue);
		#else
			setUsingLighting(bfalse);
		#endif

		#ifdef GFX_USE_POSTEFFECT
			setUsingPostEffects(btrue);
		#else
			setUsingPostEffects(bfalse);
		#endif

		#ifdef GFX_USE_FLUIDS
			setUsingFluids(btrue);
		#else
			setUsingFluids(bfalse);
		#endif


		m_StencilControl.depthEnable = GL_FALSE;
		m_StencilControl.depthWriteEnable = GL_FALSE;
		m_StencilControl.depthFunc = GL_ALWAYS;
		m_StencilControl.stencilTestEnable = GL_FALSE;
		m_StencilControl.frontStencilFunc = GL_NEVER;
		m_StencilControl.frontStencilZPass = GL_KEEP;
		m_StencilControl.frontStencilZFail = GL_KEEP;
		m_StencilControl.frontStencilFail = GL_KEEP;
		//m_alphaTest.alphaFunc = GX2_COMPARE_GEQUAL;

		Synchronize::createCriticalSection(&m_csRendering);
		Synchronize::createCriticalSection(&m_csGXOperation);

		m_AFXTimer = 0.0f;

		m_curBufferSizeX = 1280;
		m_curBufferSizeY = 720;
		m_RenderTextureBuffer = NULL;
		m_RenderDepthBuffer   = NULL;
		for(u32 i=0 ; i < NB_TEX_BFR ; ++i)
		{
			m_currentTextureBuffer[i] = NULL;
		}

		m_curProg = 0;
#ifdef ITF_SUPPORT_DBGPRIM_TEXT    
		GFX_gpDebugTextManager = 0;
#endif
        Synchronize::createCriticalSection(&m_csUnlockTexturePend);
		Synchronize::createCriticalSection(&m_csDeleteTexturePend);
        Synchronize::createCriticalSection(&m_csVBPend);
        Synchronize::createCriticalSection(&m_csIBPend);
        
		m_dirtyFlags = 0xFFFFffff;
		m_lastProgram = NULL;
        
        m_initCalled = false;
		m_usingForceWriteAplha = bfalse;
        
        m_currentColorMask = 0; // say it's dirty
        m_GL_scissorTestEnabled = -1; // means dirty
        m_GL_currentActiveTexture = 0xFFFFFFFF;
        m_GL_VP_X = 0;
        m_GL_VP_Y = 0;
        m_GL_VP_W = 0;
        m_GL_VP_H = 0;




#ifndef ITF_FINAL
		m_fPsNoVsync = 0;
		mf64_VBLSyncTime = 0.0f;
#endif

		for( int i=0; i<NB_TEX_ZBFR; i++ )
			m_currentDepthBuffer[i] = NULL;
		m_aftexFXTextureBlur = NULL;
		m_copyCurrentTextureBuffer = NULL;
		m_CurrentVertexBuffer = NULL;
		m_direct2BackBuffer = 0;
		m_vertexBufferId = 0;
		m_CurrentVertexShader = NULL;
		m_CurrentPixelShader = NULL;
		m_cachedVertexShader = NULL;
		m_cachedPixelShader = NULL;

#ifdef ITF_WINDOWS
		m_EGLDisplay	= NULL;
		m_EGLWindow		= NULL;
		m_EGLContext	= NULL;
		m_EGLConfig		= NULL;
		m_NDT			= 0;
		m_NPT			= 0;
		m_NWT			= 0;
		m_MajorVersion	= 0;
		m_MinorVersion	= 0;
		m_iConfig		= 0;
#endif

		nbBenchMarkPass = 0;
		startTimeBenchMark = 0.f;
		curTimeBenchMark = 0.f;
		benchEnable = bfalse;

		useDXTHardDecompression = bfalse;

		m_previousDisableRendering2d = bfalse;
		m_pScreenShotBuffer = NULL;
		m_screenShotAsTexture = NULL;
		m_screenShotReady = bfalse;
        m_screenShotValid = bfalse;
	}

	// ------------------------------------------------------------------------------
    void GFXAdapter_OpenGLES2::GL_enableScissorTest()
    {
        if( m_GL_scissorTestEnabled != 1 )
        {
            glEnable(GL_SCISSOR_TEST);
            m_GL_scissorTestEnabled = 1;
        }
    }
    
    void GFXAdapter_OpenGLES2::GL_disableScissorTest()
    {
        if( m_GL_scissorTestEnabled != 0 )
        {
            glDisable(GL_SCISSOR_TEST);
            m_GL_scissorTestEnabled = 0;
        }
    }
    
    void GFXAdapter_OpenGLES2::GL_activeTexture(u32 _text)
    {
        if( m_GL_currentActiveTexture != _text )
        {
            GL_CHECK( glActiveTexture( _text ) );
            m_GL_currentActiveTexture = _text;
        }
    }

    int m_scissorTestEnabled;
    
	void GFXAdapter_OpenGLES2::destroy()
	{
		// Cleanup 
		FlushAllPendingCreateUnlock();

		clearFrameBufferHashTable();

		Synchronize::destroyCriticalSection(&m_csRendering);
		Synchronize::destroyCriticalSection(&m_csGXOperation);
		Synchronize::destroyCriticalSection(&m_csUnlockTexturePend);
		Synchronize::destroyCriticalSection(&m_csDeleteTexturePend);
		Synchronize::destroyCriticalSection(&m_csVBPend);
		Synchronize::destroyCriticalSection(&m_csIBPend);

		releasePrograms();

		close();

		GFXAdapter::destroy();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::close()
	{
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::destroyResources()
	{
#ifdef _ENABLE_DEBUGFONT_
		endFontDebug();
#endif // _ENABLE_DEBUGFONT_

		releaseDynamicRingVB();
		releaseRenderBuffers();
#ifdef ITF_SUPPORT_DBGPRIM_TEXT
		SF_DEL(GFX_gpDebugTextManager);
#endif
		
		if (m_screenShotAsTexture)
		{
			GFX_ADAPTER->cleanupTexture(m_screenShotAsTexture);
			RESOURCE_MANAGER->releaseEmptyResource(m_screenShotAsTexture);
			m_screenShotAsTexture = NULL;
			m_screenShotReady = bfalse;
            m_screenShotValid = bfalse;
		}

        destroyScreenShotBuffer();

		GFXAdapter_Common<GFXAdapter_OpenGLES2>::destroyResources();
	}

	void GFXAdapter_OpenGLES2::clearFrameBufferHashTable()
	{
		for(ITF_HASHMAP<u64, GLuint>::const_iterator it = m_frameBuffersIds.begin(); it!=m_frameBuffersIds.end(); ++it)
		{
			GLuint fbo = it.getval();
			GL_CHECK( glDeleteFramebuffers(1, &fbo) );
		}
		m_frameBuffersIds.clear();
	}

	// ------------------------------------------------------------------------------
	void GFXAdapter_OpenGLES2::setRenderBuffers(TextureBuffer* _colorBuffer, DepthBuffer* _depthBuffer)
	{
        //TODO: !! delete assigment et hash map !!.

		if (m_RenderTextureBuffer != _colorBuffer)
			m_RenderTextureBuffer = _colorBuffer;
		if (m_RenderDepthBuffer != _depthBuffer)
			m_RenderDepthBuffer = _depthBuffer;

		if (!_colorBuffer)
		{
            // Copy to main frame buffer
			GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, gDefaultFrameBuffer) );
            m_RenderTextureBuffer = NULL;
            m_RenderDepthBuffer = NULL;
			m_curBufferSizeX = getWindowWidth();
			m_curBufferSizeY = getWindowHeight();

			return;
		}
		else
		{
			m_curBufferSizeX = _colorBuffer->getWidth();
			m_curBufferSizeY = _colorBuffer->getHeight();
		}
        
        // Here colorBuffer has to exist (see above)

		GLuint depthId = _depthBuffer ? (GLuint)_depthBuffer->getDepthBuffer() : 0;
        GLuint colorId = _colorBuffer->getColorBuffer();
		u64 key = ((u64)colorId)<<32 | (u64)depthId;
		
		GLuint* pclbf = m_frameBuffersIds.find(key);

		if( pclbf )
		{
			GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, *pclbf) );
		}
		else
		{
            GLuint fbo;
            GL_CHECK(glGenFramebuffers(1, &fbo));
            GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

            GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorId, 0));
            
			if (_depthBuffer)
			{
				GL_CHECK( glBindRenderbuffer(GL_RENDERBUFFER, depthId) );

                GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthId));
                GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthId));
			}
			else
			{
				GL_CHECK( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0) );
				GL_CHECK( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0) );
			}

			m_frameBuffersIds.insert(key, fbo);
		}
	}

    // ------------------------------------------------------------------------------
    void GFXAdapter_OpenGLES2::setForeground()
    {
        // recreate render buffers if they don't exists
        // use the screenshot buffer to test that
        if( m_pScreenShotBuffer == NULL )
        {
            loadCoreShaders();
            
            // Setup new render buffer.
            createRenderBuffers();
            createDynamicRingVB();
            createScreenShotBuffer();
            
#ifdef ITF_SUPPORT_DBGPRIM_TEXT
            GFX_gpDebugTextManager = newAlloc(mId_GfxAdapter, GFX_BitmapFont());
#endif
            
/*
             // not working 100% : crash when go to home screen then kill the app
 #ifdef ITF_SUPPORT_RESET_DEVICE
            m_VertexBufferManager.ReallocAllDynamicVB();
            m_VertexBufferManager.ReallocAllDynamicIB();
#endif
 */
            
            createGenericTextures();
        }
    }

    // ------------------------------------------------------------------------------
    void GFXAdapter_OpenGLES2::setBackground()
    {
        if( m_pScreenShotBuffer != NULL )
        {
#ifdef ITF_SUPPORT_DBGPRIM_TEXT
            SF_DEL( GFX_gpDebugTextManager );
            GFX_gpDebugTextManager = NULL;
#endif
            // Setup new render buffer.
            releaseDynamicRingVB();
            releaseRenderBuffers();
            clearFrameBufferHashTable();

            destroyScreenShotBuffer();
            
/*
 // not working 100% : crash when go to home screen then kill the app
 #ifdef ITF_SUPPORT_RESET_DEVICE
            m_VertexBufferManager.UnallocAllDynamicVB();
            m_VertexBufferManager.UnallocAllDynamicIB();
#endif
 */
            releaseGenericTextures();
            releaseCoreShaders();
        }
    }

    bbool GFXAdapter_OpenGLES2::resetDevice()
    {
        // Not now
        if( m_initCalled == false )
            return true;
        
        // Clean up
        releaseRenderBuffers();
        clearFrameBufferHashTable();
        
		// Setup new render buffer.
		createRenderBuffers();
        
        return btrue;
    }
    
	void GFXAdapter_OpenGLES2::createRenderBuffers()
	{
		//DEAC GX2SurfaceFormat renderDBFormat = GX2_SURFACE_FORMAT_D_D24_S8_UNORM;

		// Setup render buffer
        LOG( "Main render buffer size %u by %u", getWindowWidth(), getWindowHeight() );
		allocRenderBuffer(TEX_BFR_MAIN, getWindowWidth(), getWindowHeight(), btrue);

#if 0
		// Setup aux buffer
		u32 size, align;
		GX2CalcColorBufferAuxInfo(m_mainTextureBuffer[TEX_BFR_MAIN].getColorBuffer(), &size, &align);
		ptr = allocMem1(size, align);
		ITF_ASSERT_CRASH(ptr,"Not enough mem for GFX");
		GX2InitColorBufferAuxPtr(m_mainTextureBuffer[TEX_BFR_MAIN].getColorBuffer(), ptr);
		// Aux buffer must be cleared manually to this value once:
		ITF_MemSet(ptr, GX2_AUX_BUFFER_CLEAR_VALUE, size);
		ITF_ASSERT_CRASH(sb_IsRenderThread(), "GX2Invalidate must be called from main thread");
		GX2Invalidate(GX2_INVALIDATE_CPU, ptr, size);

#ifdef ITF_USE_REMOTEVIEW
		GX2InitColorBufferAuxPtr(m_remoteTextureBuffer[TEX_BFR_MAIN].getColorBuffer(), ptr);
#endif // ITF_USE_REMOTEVIEW
#endif

		// Setup Depth Buffer
		m_mainDepthBuffer.Init(getWindowWidth(), getWindowHeight());
		m_mainDepthBuffer.Invalidate();
  
		// Extra full screen render buffers
		if ( isUsingPostEffects() )
		{
			u32 FS_Width   = getWindowWidth();
			u32 FS_Height  = getWindowHeight();
			for(u32 i=TEX_BFR_FIRST_FS ; i<=TEX_BFR_LAST_FS; ++i)
			{
				allocRenderBuffer(i, FS_Width, FS_Height, btrue);
			}

			// Extra quart screen render buffers
			FS_Width /= 2; FS_Height /= 2;
			for(u32 i=TEX_BFR_FIRST_QS ; i<=TEX_BFR_LAST_QS ; ++i)
			{
				allocRenderBuffer(i, FS_Width, FS_Height, btrue);
			}
		}
   
		if ( isUsingFluids() )
		{
			// Set up Fluid render buffers
			const u32 FBCB_W  = (getWindowWidth()  / FLUID_BLUR_BFR_RATIO + 15) & 0xfffffff0;
			const u32 FBCB_H  = (getWindowHeight() / FLUID_BLUR_BFR_RATIO + 15) & 0xfffffff0;
			allocRenderBuffer(TEX_BFR_FLUID_BLUR, FBCB_W, FBCB_H, btrue);
			

			const u32 FGCB_W  = (getWindowWidth()  / FLUID_GLOW_BFR_RATIO + 15) & 0xfffffff0;
			const u32 FGCB_H  = (getWindowHeight() / FLUID_GLOW_BFR_RATIO + 15) & 0xfffffff0;
			allocRenderBuffer(TEX_BFR_FLUID_2, FGCB_W, FGCB_H, btrue);
		}

		// Set up lighting render buffers, linked to quart buffers
		if ( isUsingLighting() )
		{
			const u32 LCB_W  = getWindowWidth()  / LIGHTS_BFR_RATIO;
			const u32 LCB_H  = getWindowHeight() / LIGHTS_BFR_RATIO;
			allocRenderBuffer( TEX_BFR_FRONT_LIGHT, LCB_W, LCB_H, btrue );
			allocRenderBuffer( TEX_BFR_BACK_LIGHT, LCB_W, LCB_H, btrue );
		}

#if 0
		GX2SurfaceFormat qrtLightsurfaceFmt = renderCBFormat;
		const u32 QLCB_W  = getWindowWidth()  / QRT_LIGHTS_BFR_RATIO;
		const u32 QLCB_H  = getWindowHeight() / QRT_LIGHTS_BFR_RATIO;
		const u32 QRLCB_W = getRemoteScreenWidth()  / QRT_LIGHTS_BFR_RATIO;
		const u32 QRLCB_H = getRemoteScreenHeight() / QRT_LIGHTS_BFR_RATIO;
		linkRenderBuffer(TEX_BFR_QRT_FRONT_LIGHT, TEX_BFR_FIRST_QS, QLCB_W, QLCB_H, QRLCB_W, QRLCB_H, qrtLightsurfaceFmt);
		linkRenderBuffer(TEX_BFR_QRT_BACK_LIGHT, TEX_BFR_FLUID_2, QLCB_W, QLCB_H, QRLCB_W, QRLCB_H, qrtLightsurfaceFmt);
		linkRenderBuffer(TEX_BFR_QRT_TMP_LIGHT, TEX_BFR_BACK_LIGHT, QLCB_W, QLCB_H, QRLCB_W, QRLCB_H, qrtLightsurfaceFmt, btrue);

		const u32 RCB_W  = getWindowWidth()  / REFLECTION_BFR_RATIO;
		const u32 RCB_H  = getWindowHeight() / REFLECTION_BFR_RATIO;
		const u32 RRCB_W = getRemoteScreenWidth()  / REFLECTION_BFR_RATIO;
		const u32 RRCB_H = getRemoteScreenHeight() / REFLECTION_BFR_RATIO;
		linkZBuffer(RCB_W, RCB_H, RRCB_W, RRCB_H);

		//----

		// Now alloc buffers that can be always in Mem2

		if(!_mem1only)
		{
			allocRenderBuffer
			(
				TEX_BFR_SCENE_COPY,
				getWindowWidth(), getWindowHeight(), getRemoteScreenWidth(), getRemoteScreenHeight(),
				renderCBFormat, GFX_COLOR_BUFFER_SWIZZLE, bfalse,
				bfalse, btrue
			);

#ifndef ITF_FINAL
			// Alloc a debug render buffer in Mem2 for zlist pass debug
			allocRenderBuffer
			(
				TEX_BFR_DBG_COPY,
				getWindowWidth(), getWindowHeight(), getRemoteScreenWidth(), getRemoteScreenHeight(),
				renderCBFormat, GFX_COLOR_BUFFER_SWIZZLE, bfalse,
				bfalse, bfalse, MemoryId::mId_Debug
			);
#endif // ITF_FINAL
		}
#endif
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::allocRenderBuffer(u32 _texID, u32 _mainW, u32 _mainH, bbool _ftv)
	{
		m_mainTextureBuffer[_texID].Init(_mainW, _mainH, _ftv);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::linkRenderBuffer(u32 _dstID, u32 _srcID, u32 _mainW, u32 _mainH, u32 _remoteW, u32 _remoteH, /*DEAC GX2SurfaceFormat _format,*/ bbool _offseted)
	{
//#ifndef ITF_FINAL
//		char str[64];
//#endif // ITF_FINAL

		m_mainTextureBuffer[_dstID].Copy(&m_mainTextureBuffer[_srcID], _offseted ? m_mainTextureBuffer[_srcID].getSize() : 0);
		#ifdef TEX_BFR_USE_SUB_LIST
		m_mainTextureBuffer[_dstID].setMain(&m_mainTextureBuffer[_srcID]);
		#endif // TEX_BFR_USE_SUB_LIST
		m_mainTextureBuffer[_dstID].Resize(_mainW, _mainH, btrue);
		//DEAC m_mainTextureBuffer[_dstID].ChangeFormat(_format);
		m_mainTextureBuffer[_dstID].Invalidate();

#ifdef ITF_USE_REMOTEVIEW
		//DEAC m_remoteTextureBuffer[_dstID].Copy(&m_mainTextureBuffer[_dstID]);
#ifdef TEX_BFR_USE_SUB_LIST
		m_remoteTextureBuffer[_dstID].setMain(&m_mainTextureBuffer[_dstID]);
#endif // TEX_BFR_USE_SUB_LIST
		//DEAC m_remoteTextureBuffer[_dstID].Resize(_remoteW, _remoteH, btrue);
		//DEAC m_remoteTextureBuffer[_dstID].Invalidate();
#endif // ITF_USE_REMOTEVIEW
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::linkZBuffer(u32 _mainW, u32 _mainH, u32 _remoteW, u32 _remoteH)
	{
//#ifndef ITF_FINAL
//		char str[64];
//#endif // ITF_FINAL

		m_QSDepthBuffer.Copy(&m_mainDepthBuffer);
		m_QSDepthBuffer.Resize(_mainW, _mainH, btrue);
		m_QSDepthBuffer.Invalidate();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::releaseRenderBuffers(bbool _mem1only)
	{
        glBindTexture(GL_TEXTURE_2D, 0);
        
		m_mainTextureBuffer[TEX_BFR_MAIN].release();
		
        m_mainDepthBuffer.Release();

		for(u32 i=TEX_BFR_FIRST_FS ; i<=TEX_BFR_LAST_FS; ++i)
		{
			m_mainTextureBuffer[i].release();
		}
		for(u32 i=TEX_BFR_FIRST_QS ; i<=TEX_BFR_LAST_QS ; ++i)
		{
			m_mainTextureBuffer[i].release();
		}

		m_mainTextureBuffer[TEX_BFR_SCENE_COPY].release();

        if ( isUsingFluids() )
        {
            m_mainTextureBuffer[TEX_BFR_FLUID_BLUR].release();
            m_mainTextureBuffer[TEX_BFR_FLUID_2].release();
        }

        if ( isUsingLighting() )
        {
            m_mainTextureBuffer[TEX_BFR_FRONT_LIGHT].release();
            m_mainTextureBuffer[TEX_BFR_BACK_LIGHT].release();
        }

	}

	// ------------------------------------------------------------------------------

	BasePlatformTexture* GFXAdapter_OpenGLES2::getFrontLightTexture(eLightTexBuffer _currentLightBufferType) const
	{
		if(_currentLightBufferType == LIGHT_BFR_FULL)
			return m_currentTextureBuffer[TEX_BFR_FRONT_LIGHT]->getTextureSamp();
		else
			return m_currentTextureBuffer[TEX_BFR_QRT_FRONT_LIGHT]->getTextureSamp();
	}

	// ------------------------------------------------------------------------------

	BasePlatformTexture* GFXAdapter_OpenGLES2::getBackLightTexture(eLightTexBuffer _currentLightBufferType) const
	{
		if(_currentLightBufferType == LIGHT_BFR_FULL)
			return m_currentTextureBuffer[TEX_BFR_BACK_LIGHT]->getTextureSamp();
		else
			return m_currentTextureBuffer[TEX_BFR_QRT_BACK_LIGHT]->getTextureSamp();
	}

	// ------------------------------------------------------------------------------

	GFXAdapter_OpenGLES2::TextureBuffer* GFXAdapter_OpenGLES2::getFrontLightSurface(eLightTexBuffer _currentLightBufferType)
	{
		if(_currentLightBufferType == LIGHT_BFR_FULL)
			return m_currentTextureBuffer[TEX_BFR_FRONT_LIGHT];
		else if(_currentLightBufferType == LIGHT_BFR_QRT_MOD_0)
			return m_currentTextureBuffer[TEX_BFR_QRT_FRONT_LIGHT];
		else
			return m_currentTextureBuffer[TEX_BFR_QRT_TMP_LIGHT];
	}

	// ------------------------------------------------------------------------------

	GFXAdapter_OpenGLES2::TextureBuffer* GFXAdapter_OpenGLES2::getBackLightSurface(eLightTexBuffer _currentLightBufferType)
	{
		if(_currentLightBufferType == LIGHT_BFR_FULL)
			return m_currentTextureBuffer[TEX_BFR_BACK_LIGHT];
		else if(_currentLightBufferType == LIGHT_BFR_QRT_MOD_0)
			return m_currentTextureBuffer[TEX_BFR_QRT_BACK_LIGHT];
		else
			return m_currentTextureBuffer[TEX_BFR_QRT_TMP_LIGHT];
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::createDynamicRingVB()
	{
		u32 ringVBSize = 2 * 1024 * 1024;
#ifdef VBMANAGER_USE_DYNAMICRING_VB
#ifdef ITF_ANDROID
		ringVBSize = 512 * 1024;
#else
		ringVBSize = 1024 * 1024;
#endif
        
#endif // VBMANAGER_USE_DYNAMICRING_VB
		m_VertexBufferManager.createDynamicRingVB(ringVBSize, 4);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::releaseDynamicRingVB()
	{
		m_VertexBufferManager.deleteDynamicRingVB();
	}

	// ------------------------------------------------------------------------------

#ifdef _HARDCODED_SHADER

	GLuint loadShader(GLenum shaderType, const char* pSource)
	{
		GLuint shader = glCreateShader(shaderType);
		if (shader)
		{
			glShaderSource(shader, 1, &pSource, NULL);
			glCompileShader(shader);
			GLint compiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			if (!compiled)
			{
				GLint infoLen = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
				if (infoLen)
				{
					char* buf = (char*) malloc(infoLen);
					if (buf)
					{
						glGetShaderInfoLog(shader, infoLen, NULL, buf);
						//OGE("Could not compile shader %d:\n%s\n", shaderType, buf);
						free(buf);
					}
	                
					glDeleteShader(shader);
					shader = 0;
				}
			}
		}
		return shader;
	}

	GLuint createProgram(const char* pVertexSource, const char* pFragmentSource)
	{
		GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
		if (!vertexShader)
		{
			return 0;
		}

		GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
		if (!pixelShader)
		{
			return 0;
		}

		GLuint program = glCreateProgram();
		if (program)
		{
			glAttachShader(program, vertexShader);
			glAttachShader(program, pixelShader);
			glLinkProgram(program);
            glValidateProgram(program);
			GLint linkStatus = GL_FALSE;
			glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
			if (linkStatus != GL_TRUE)
			{
				GLint bufLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
				if (bufLength)
				{
					char* buf = (char*) malloc(bufLength);
					if (buf)
					{
						glGetProgramInfoLog(program, bufLength, NULL, buf);
						//LogE("Could not link program:\n%s\n", buf);
						free(buf);
					}
				}
	            
				glDeleteProgram(program);
            	program = 0;
			}
		}

		return program;
	}
#endif

    //
    // This is SGI sample code taken directly from OpenGL.org:
    // http://www.opengl.org/developers/code/features/OGLextensions/OGLextensions.html
    //
    int OpenGLExtensionIsSupported(const char* extension)
    {
        const GLubyte *extensions = glGetString(GL_EXTENSIONS);
        const GLubyte *start;
        GLubyte *where, *terminator;
        
        // Extension names should not have spaces.
        where = (GLubyte *) strchr(extension, ' ');
        if (where || *extension == '\0')
            return 0;
        
        // It takes a bit of care to be fool-proof about parsing the
        // OpenGL extensions string. Don't be fooled by sub-strings, etc.
        start = extensions;
        const char *extStr = (const char *)extensions;
        
        LOG( extStr );
        for (;;)
        {
            where = (GLubyte *)strstr( (const char *)start, extension );
            if( !where )
                break;
            terminator = where + strlen(extension);
            if (where == start || *(where - 1) == ' ')
                if (*terminator == ' ' || *terminator == '\0')
                    return 1;
            start = terminator;
        }
        return 0;
    }
    
	void GFXAdapter_OpenGLES2::init()
	{
        m_initCalled = true;
        
		Super::preInitDriver();
        

        // int supported = OpenGLExtensionIsSupported("GL_APPLE_client_storage");
        useDXTHardDecompression = (bbool)OpenGLExtensionIsSupported("GL_EXT_texture_compression_s3tc");

#ifdef _HARDCODED_SHADER
		gProgram[0] = createProgram(gVertexShaderPCT, gFragmentShaderPCT);
		if (!gProgram[0])
		{
			//TA MEME
			u32 tameme;
			tameme = 1;
		}

		gProgram[1] = createProgram(gVertexShaderPC, gFragmentShaderPC);
		if (!gProgram[0])
		{
			//TA MEME
			u32 tameme;
			tameme = 1;
		}

		gvPositionHandle[0] = glGetAttribLocation(gProgram[0], "vPosition");
		gvColorHandle[0] = glGetAttribLocation(gProgram[0], "COLOR");
		gvUVHandle[0] = glGetAttribLocation(gProgram[0], "myUV");

		gvPositionHandle[1] = glGetAttribLocation(gProgram[1], "vPosition");
		gvColorHandle[1] = glGetAttribLocation(gProgram[1], "COLOR");

		glDisableVertexAttribArray(gvPositionHandle[0]);
		glDisableVertexAttribArray(gvUVHandle[0]);
		glDisableVertexAttribArray(gvColorHandle[0]);

		glEnableVertexAttribArray(gvPositionHandle[0]);
		glEnableVertexAttribArray(gvColorHandle[0]);
		glEnableVertexAttribArray(gvUVHandle[0]);
#endif

		// Init shaders
#ifndef _HARDCODED_SHADER
        f64 timeStart = SYSTEM_ADAPTER->getTime();
		loadCoreShaders();
        f64 timeEnd = SYSTEM_ADAPTER->getTime() - timeStart;
        
        LOG( "Shader Loading Took %f milliseconds", f32(timeEnd*1000.0f) );
#endif

		invalidateSamplers();

        m_direct2BackBuffer = 0; // SYSTEM_ADAPTER->getDeviceInfo().getDeviceSpeed() == DeviceInfo::Device_SpeedLow;

		SetDefaultSettings();
		//m_SamplerManager.create();

		// Setup render buffer.
		createRenderBuffers();
        
		createDynamicRingVB();
		 
#ifdef ITF_SUPPORT_DBGPRIM_TEXT
		GFX_gpDebugTextManager = newAlloc(mId_GfxAdapter, GFX_BitmapFont());
#endif

		m_RenderStates.flush();

        Super::postInitDriver();
        

        createScreenShotBuffer();
	}
    // ------------------------------------------------------------------------------

    void GFXAdapter_OpenGLES2::createScreenShotBuffer()
    {
        m_ScreenShotWidth = 800;
        m_ScreenShotHeight = 600;

        m_pScreenShotBuffer = newAlloc(mId_Archive,u8[ 4 * m_ScreenShotWidth * m_ScreenShotHeight]);
    }

    // ------------------------------------------------------------------------------
    void GFXAdapter_OpenGLES2::destroyScreenShotBuffer()
    {
        if (m_pScreenShotBuffer)
        {
            SF_DEL_ARRAY(m_pScreenShotBuffer);
            //delete [] m_pScreenShotBuffer;
            m_pScreenShotBuffer = NULL;
        }
    }

	// ------------------------------------------------------------------------------

	bbool GFXAdapter_OpenGLES2::createDevice(bbool _fullscreen, void* _hwnd)
	{
		bbool res = btrue;

		m_hwnd = _hwnd;
		//m_fullscreen = _fullscreen;

#ifdef ITF_WINDOWS
		res = createGLES2DeviceWin32();
#else
        res = 1; // already done
#endif
		if(res)
		{
			/// Core Shaders.    
			// loadCoreShaders();

			//if (!createVertexDeclaration())
			//	return bfalse;

			slotAllocatorManager::enablePhysicalAllocation();
		}

		return res;
	}

	// ------------------------------------------------------------------------------
#ifdef ITF_WINDOWS
	bbool GFXAdapter_OpenGLES2::createGLES2DeviceWin32()
	{
		HDC	m_hDC = GetDC((HWND)m_hwnd);
		m_NDT = (EGLNativeDisplayType)m_hDC;
		m_NWT = (EGLNativeWindowType)m_hwnd;

		m_EGLContext = 0;
		m_EGLDisplay = eglGetDisplay(m_NDT);

		if(m_EGLDisplay == EGL_NO_DISPLAY)
		{
			m_EGLDisplay = eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY);
		}

		if(!eglInitialize(m_EGLDisplay, &m_MajorVersion, &m_MinorVersion))
		{
			//SYSTEM_ADAPTER->messageBox( "Create OpenGLES2 device Failed", ITF_MSG_ICONERROR | ITF_MSG_OK, ITF_IDOK );
			return bfalse;
		}

		// Find an EGL config.
#ifdef ITF_GLES2_POWER_VR
		m_EGLConfig = (void*)0x7;//SelectEGLConfiguration(m_pShell->m_pShellData);
#else
		m_EGLConfig = (void*)0x5;//SelectEGLConfiguration(m_pShell->m_pShellData);
#endif
		eglGetConfigAttrib(m_EGLDisplay, m_EGLConfig, EGL_CONFIG_ID, &m_iConfig);

		// Destroy the context if we already created one.
		if (m_EGLContext)
		{
			eglDestroyContext(m_EGLDisplay, m_EGLContext);
		}

		// Attempt to create a context.
		EGLint ai32ContextAttribs[32];
		int	i = 0;

#if defined(EGL_VERSION_1_3) && defined(GL_ES_VERSION_2_0)
		ai32ContextAttribs[i++] = EGL_CONTEXT_CLIENT_VERSION;
		ai32ContextAttribs[i++] = 2;
#endif

		ai32ContextAttribs[i] = EGL_NONE;

		m_EGLContext = eglCreateContext(m_EGLDisplay, m_EGLConfig, NULL, ai32ContextAttribs);

		EGLint		attrib_list[16];
		i = 0;
		attrib_list[i] = EGL_NONE;

		m_EGLWindow = eglCreateWindowSurface(m_EGLDisplay, m_EGLConfig, m_NWT, attrib_list);

		// If we have failed to create a surface then try using Null.
		if(m_EGLWindow == EGL_NO_SURFACE)
		{
			m_EGLWindow = eglCreateWindowSurface(m_EGLDisplay, m_EGLConfig, NULL, attrib_list);
		}

		if (!eglMakeCurrent(m_EGLDisplay, m_EGLWindow, m_EGLWindow, m_EGLContext))
		{
#ifdef EGL_VERSION_1_3
			//if((eglGetError() == EGL_CONTEXT_LOST))
#endif	
		}

#ifdef EGL_VERSION_1_1
		eglSwapInterval(m_EGLDisplay, 1);
#endif


#if 0 //DEAC		
		m_maxTextureSize = std::min(caps.MaxTextureHeight, caps.MaxTextureWidth);

		m_supportNonSquareTex = !(caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY);

		m_supportNPOTTex = !(caps.TextureCaps & D3DPTEXTURECAPS_POW2) ||
						   (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL);
#endif		
		return btrue;
	}
#endif //ITF_WINDOWS

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::SetDefaultSettings()
	{
		for ( ux i = 0; i < MaxSampler; ++i )
		{
			m_samplerMinFilterPS[i] = GL_LINEAR;
			m_samplerMaxFilterPS[i] = GL_LINEAR;
			m_samplerWrapUPS[i] = GL_CLAMP_TO_EDGE;
			m_samplerWrapVPS[i] = GL_CLAMP_TO_EDGE;
		}

		setShaderGroup(m_defaultShaderGroup);

		m_rasterizeControl.setDefault();
		m_rasterizeControl.sendRasterizeControl();

		RenderPassContext rdrCtx;
		PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
		DrawCallContext drawCallCtx = DrawCallContext(&primCtx);
		static GFXPrimitiveParam defaultPrimitiveParam;
		setGFXPrimitiveParameters(&defaultPrimitiveParam, rdrCtx);
		setGfxMatDefault(drawCallCtx);
		SetDrawCallState(drawCallCtx.getStateCache(), 0xFFffFFff, btrue);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::clear(u32 _buffer, float _r, float _g, float _b, float _a)
	{  
		if(_buffer)
		{
			GLbitfield bitfmask = 0;
			if(_buffer & GFX_CLEAR_COLOR)
			{
				GL_CHECK(glClearColor(_r, _g, _b, _a) );

				//glClearColor(1.f, 0, 0, 1.f);
                
				bitfmask |= GL_COLOR_BUFFER_BIT;
			}

#ifndef GLES2_NO_Z_BUFFER
			if(_buffer & GFX_CLEAR_ZBUFFER)
			{
				bitfmask |= GL_DEPTH_BUFFER_BIT;
				GL_CHECK(glDepthMask(GL_TRUE));
				GL_CHECK(glClearDepthf(1.0f));
			}
#endif
            
			if(_buffer & GFX_CLEAR_STENCIL)
			{
				bitfmask |= GL_STENCIL_BUFFER_BIT;
				GL_CHECK(glDisable(GL_STENCIL_TEST) );
				GL_CHECK(glStencilMask(0xFF));
				GL_CHECK(glClearStencil(0));
			}

			GL_CHECK(glClear(bitfmask));

			m_RenderStates.SetDepthStencilControl(m_StencilControl, true);
            colorMask(GFX_COL_MASK_RED | GFX_COL_MASK_GREEN | GFX_COL_MASK_BLUE);
			//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
		}
	}

	// ------------------------------------------------------------------------------

#if 0
#ifndef ITF_FINAL
// To flip endianess and store
	static void SetLeU32(u8* ptr, u32 data)
	{
		*ptr     = (u8)(data & 0x000000FF);
		*(ptr+1) = (u8)((data >> 8)  & 0x000000FF);
		*(ptr+2) = (u8)((data >> 16) & 0x000000FF);
		*(ptr+3) = (u8)((data >> 24) & 0x000000FF);
	}

	static void SetLeU16(u8* ptr, u16 data)
	{
		*ptr     = (u8)(data & 0x00FF);
		*(ptr+1) = (u8)((data >> 8) & 0x00FF);
	}
#endif // ITF_FINAL
#endif

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::drawDebug()
	{
#ifndef ITF_GLES2_RELEASE
        if( getStrSceenshotResquest() )
            return;
        
		RenderPassContext passCtx;
		passCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA)
			.setDepthTest(bfalse);
		SetDrawCallState(passCtx.getStateCache(), 0xFFffFFff, btrue);
		PrimitiveContext primitiveCxt = PrimitiveContext(&passCtx);
		primitiveCxt.setPrimitiveParam((GFXPrimitiveParam*)&m_defaultPrimitiveParam);
		DrawCallContext drawCallCtx = DrawCallContext(&primitiveCxt);

		setDefaultGFXPrimitiveParam();
		setVertexShader( NULL );

		/// set the wvp matrix.
		setOrthoView(0.f, (f32) getWindowWidth(), 0.f, (f32) getWindowHeight());

		/// set first the shader you want to use.
		setShaderGroup(m_defaultShaderGroup);
		setGfxMatDefault(drawCallCtx);

		/// set Vertex format
		setVertexFormat(VertexFormat_PC);

#ifdef ITF_SUPPORT_DBGPRIM_BOX
		for (u32 it = 0; it < m_DBGBox2DList.size(); ++it)
		{
			const Box2D& rect = m_DBGBox2DList[it];

			DrawPrimitive(drawCallCtx, GFX_TRIANGLE_STRIP, (void*)rect.v, 4);
		}
		m_DBGBox2DList.clear();
#endif // ITF_SUPPORT_DBGPRIM_BOX

#ifdef ITF_SUPPORT_DBGPRIM_MISC
		for (u32 it = 0; it < m_DBGTriangle2DList.size(); ++it)
		{
			const Triangle2D& tri = m_DBGTriangle2DList[it];

			DrawPrimitive(drawCallCtx, GFX_TRIANGLES, (void*)tri.v, 3);
		}
		m_DBGTriangle2DList.clear();

		for (u32 it = 0; it < m_DBGCircle2DList.size(); ++it)
		{
			const Circle2D& circle = m_DBGCircle2DList[it];

			DrawPrimitive(drawCallCtx, GFX_LINE_STRIP, (void*)circle.m_vertexes, circle.m_vertexCount);
		}
		m_DBGCircle2DList.clear();

#endif // ITF_SUPPORT_DBGPRIM_MISC

    
#ifndef ITF_FINAL
		if (isShowDebugInfo())
			getVertexBufferManager().drawDebugVBInfo();
#endif // ITF_FINAL

		 displayDebugInfoGLES2(drawCallCtx);
        
#endif
        
	}

	void GFXAdapter_OpenGLES2::present()
	{
        //clear(GFX_CLEAR_COLOR, 1.f,0.f,0.f,1.f);
#ifdef _DEMO_DE_LA_MORT
		// -----------------------------
		// Super Demo de la mort.
		// -----------------------------
		
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);clear
     
        //clear(GFX_CLEAR_COLOR, 1.f,0.f,0.f,1.f);
        
        GLint binding=666;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &binding);
        
		RenderPassContext rdrCtxt;
		rdrCtxt.getStateCache().setDepthTest(bfalse);
		SetDrawCallState(rdrCtxt.getStateCache(), 0xFFffFFff, btrue);


		PrimitiveContext primCtx = PrimitiveContext(&rdrCtxt);
		DrawCallContext drawCallCtx = DrawCallContext(&primCtx);
		drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
			.setColorWrite(btrue)
			.setAlphaWrite(btrue);

		// drawScreenQuad(drawCallCtx, 0.5f, 0.5f, 100+0.5f, 100+0.5f, 0.f, 0xFF8080FF,true);
		drawScreenQuad(drawCallCtx, 0.f, 0.f, 100+0.5f, 100+0.5f, 0.f, 0xFF8080FF,true);

   
        
		// -----------------------------
#endif

        if( m_direct2BackBuffer == bfalse )
            mainBufferToBackBuffer();

		// Unlock mutex
		UnlockRendering();

#ifdef ITF_WINDOWS
		// Flip.
		bool bRes;
		bRes = (eglSwapBuffers (m_EGLDisplay, m_EGLWindow) == EGL_TRUE);
#endif
        
        resetShaderConstantCache(); //
	}


	void GFXAdapter_OpenGLES2::setBackBuffer(bbool _setZbuffer, bbool _restoreBackBuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gDefaultFrameBuffer); // ? to check 
	}

	//----------------------------------------------------------------------------//

	void GFXAdapter_OpenGLES2::beginSceneRendering(View &_view)
	{
		setRenderTargetForPass(GFX_ZLIST_PASS_REGULAR);
		u32 buffers = GFX_CLEAR_ZBUFFER | GFX_CLEAR_STENCIL;
		if(_view.getViewType() != View::viewType_ViewportUI)
			buffers |= GFX_CLEAR_COLOR;
		clear(buffers, m_clearColor.m_r, m_clearColor.m_g, m_clearColor.m_b, 0.0f );
	}


	//----------------------------------------------------------------------------//

	void GFXAdapter_OpenGLES2::endSceneRendering(View &_view)
	{
	}
	
	//----------------------------------------------------------------------------//
	
	void GFXAdapter_OpenGLES2::startScreenshotMode()
	{
		m_preUHDScreenWidth = getWindowWidth();
		m_preUHDScreenHeight = getWindowHeight();
        m_screenShotReady = bfalse;
        m_screenShotValid = bfalse;

		m_previousDisableRendering2d = isDisableRender2d();

		setDisableRender2d(btrue);
		setDisableRenderAfterFx(btrue);

		// Limitation RAM on Mobile.
        setResolution(m_ScreenShotWidth, m_ScreenShotHeight);
        
        updateRenderBuffers();
        setupViewport(0,0,m_ScreenShotWidth,m_ScreenShotHeight);

        updateShaderScreenDimension(m_ScreenShotWidth, m_ScreenShotHeight);
        
		setSeparateAlpha(btrue);
	}

	void GFXAdapter_OpenGLES2::endScreenshotMode()
	{
		const char * request = getStrSceenshotResquest();

		if (request == strScreenshotNormal() || request == strScreenshotUHD() || request == strScreenshotUHDNo2d())
		{
			const i32 Width  = m_windowWidth;
			const i32 Height = m_windowHeight;

			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glReadPixels(0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, m_pScreenShotBuffer);

			// inverse Y.
			unsigned char	u8_Temp;

			u8* src = m_pScreenShotBuffer + (Height - 1) * Width * 4;
			u8* dst = m_pScreenShotBuffer;

			while(dst < src)
			{
				for(unsigned int i = 0; i < Width; i++)
				{
					u8_Temp = dst[4 * i + 0];
					dst[4 * i + 0] = src[4 * i + 0];
					src[4 * i + 0] = u8_Temp;

					u8_Temp = dst[4 * i + 1];
					dst[4 * i + 1] = src[4 * i + 1];
					src[4 * i + 1] = u8_Temp;
				
					u8_Temp = dst[4 * i + 2];
					dst[4 * i + 2] = src[4 * i + 2];
					src[4 * i + 2] = u8_Temp;
				
					// skip alpha...
				}

				dst += Width * 4;
				src -= Width * 4;
			}

            
            // make texture from screenShot.
			const u32 wtex = 256;
			const u32 htex = 256;
            
            const f32 WRatio = 0.9999f * (f32)Width / (f32)wtex;
            const f32 HRatio = 0.9999f * (f32)Height / (f32)htex;
            
			if (!m_screenShotAsTexture)
			{
				m_screenShotAsTexture = static_cast<Texture *>(RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture));
                
				GFX_ADAPTER->createTexture(
                                           m_screenShotAsTexture,
                                           wtex,
                                           htex,
                                           1,
                                           Texture::PF_RGBA,
                                           2,
                                           btrue
                                           );
			}
            
			if (m_screenShotAsTexture)
			{
				LOCKED_TEXTURE lockRect;
				u32 lock_flags = 0;
                
				GFX_ADAPTER->lockTexture(m_screenShotAsTexture, &lockRect, lock_flags);
                
				u8* pDst = (u8*)lockRect.mp_Bits;
                
				for(u32 j = 0 ; j < htex; j++)
				{
                    u8* srcLine = m_pScreenShotBuffer + 4 * Width * (u32)( j * HRatio);
                    u32 lineSampleCount = (u32)( (j+1) * HRatio) - (u32)(j * HRatio);
					for ( u32 i = 0 ; i < wtex; i++)
					{
                        u8* srcPixel = srcLine + 4 * (u32)(i*WRatio);
                        u32 pixelSampleCount = (u32)((i+1)*WRatio) - (u32)(i*WRatio);
	
                        // Filtering
                        u32 R = 0;
                        u32 G = 0;
                        u32 B = 0;
                        for (u32 k=0; k<lineSampleCount; k++)
                        {
                            for (u32 l=0; l<pixelSampleCount; l++)
                            {
                                src = srcPixel + 4 * (Width * k + l);
                                R += src[2];
                                G += src[1];
                                B += src[0];
                            }
                        }
                        u32 sampleCount = lineSampleCount * pixelSampleCount;
                        
                        pDst[0] = (u8)(R/sampleCount);
						pDst[1] = (u8)(G/sampleCount);
						pDst[2] = (u8)(B/sampleCount);
						pDst[3] = 255;
                        pDst += 4;
					}
				}
                
				GFX_ADAPTER->unlockTexture(m_screenShotAsTexture);
			}


			m_screenShotReady = btrue;
            m_screenShotValid = btrue;
		}

		// Restore previous state
		setResolution(m_preUHDScreenWidth, m_preUHDScreenHeight);
		setSeparateAlpha(bfalse);
		setDisableRender2d(m_previousDisableRendering2d);
		setDisableRenderAfterFx(bfalse);
        updateRenderBuffers();
        setupViewport(0,0,m_preUHDScreenWidth,m_preUHDScreenHeight);
        updateShaderScreenDimension(m_preUHDScreenWidth, m_preUHDScreenHeight);
	}
	
	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::beginDisplay(f32 _dt)
	{
		//DEAC cleanSetShaderCache();


#ifdef SUPPORT_STATS_MANAGER
#ifdef TRIPLE_BUFFERING
		ITF_ASSERT((m_VertexBufferManager.getDynamicRingVBusage()*2) <= m_VertexBufferManager.getDynamicRingVBSize());
#else // TRIPLE_BUFFERING
		//DEAC ITF_ASSERT(m_VertexBufferManager.getDynamicRingVBusage() <= m_VertexBufferManager.getDynamicRingVBSize());
#endif // TRIPLE_BUFFERING
#endif // SUPPORT_STATS_MANAGER
		
		GFXAdapter::beginDisplay(_dt);
#ifndef ITF_IOS 
		// Called in AppFramework iOS on iOS
		FlushAllPendingCreateUnlock();
#endif
        
		LockRendering();
		invalidateSamplers();

#ifndef TRIPLE_BUFFERING
		waitPreviousGPUframeEnd(0);
#endif // TRIPLE_BUFFERING
}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::endDisplay()
	{
		GFXAdapter::endDisplay();

		resetSceneResolvedFlag();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::FlushAllPendingCreateUnlock()
	{
		ITF_ASSERT_CRASH(Synchronize::getCurrentThreadId() == ThreadInfo::getGraphicThreadId(), "This function must be called from graphics thread");

        
		// Textures.
        {
        
            int maxCount = 4; // slice the loading to avoid choke points
            
			while( 1 )
			{
				textureGLES2 tex2D;
				bool hasTexture = false;

                

				// Try to get something and removed it asap, with the minimum lock time
				{
					csAutoLock cs(m_csUnlockTexturePend);

					const u32 count = (u32)m_pendingCreateUnlockTex2D.size();
					if( count > 0 )
					{
						tex2D = m_pendingCreateUnlockTex2D[count-1];
						m_pendingCreateUnlockTex2D.pop_back();
						hasTexture = true;
					}
				}
				// this takes time, must be OUTSIDE of lock
				if(hasTexture)
				{
					loadFromMemory(tex2D.mp_dataRaw, Size(tex2D.mp_texture->getSizeX(), tex2D.mp_texture->getSizeY()), tex2D.mp_texture, tex2D.mp_texture->m_pixFormat, 1, tex2D.mp_texture->m_mipMapLevels);
					Memory::alignedFree(tex2D.mp_dataRaw);
					tex2D.mp_dataRaw = 0;
				}
				else
					break; // we're done, list is empty
                
                // Stop if too many iteration, avoid locking the main thread for too long
                if( --maxCount <= 0 )
                    break;
			}
        }
        
            
        // texture delete
        {
            csAutoLock cs(m_csDeleteTexturePend);
            
            const u32 count = (u32)m_pendingTextureDelete.size();
            for (ux i = 0; i < count; ++i)
            {
                //LOG( "Flushing Pending called on %u over %u", i, count );
                BasePlatformTexture* tex = m_pendingTextureDelete[i];
                
                cleanupTextureInternal(tex);

            }
            m_pendingTextureDelete.clear();
        }
        
        
		//Vertex Buffer.
        {
            csAutoLock cs(m_csVBPend);
            const u32 countVBC = (u32)m_pendingCreateVerterBuffer.size();
            for (ux i = 0; i < countVBC; ++i)
            {
                ITF_VertexBuffer* vertxb = m_pendingCreateVerterBuffer[i];
                if(vertxb)
                {
                    createVertexBufferGL(vertxb);
                }
            }

            m_pendingCreateVerterBuffer.clear();

            // -----

            //Vertex Buffer.
            const u32 countVB = (u32)m_pendingCreateUnlockVerterBuffer.size();
            for (ux i = 0; i < countVB; ++i)
            {
                ITF_VertexBuffer* vertxb = m_pendingCreateUnlockVerterBuffer[i];
                if(vertxb)
                {
                    unlockVertexBufferGL(vertxb);
                }
            }

            m_pendingCreateUnlockVerterBuffer.clear();
            

            {
                const u32 countVB = (u32)m_pendingReleaseVertexBuffer.size();
                for (ux i = 0; i < countVB; ++i)
                {
                    releaseVertexBufferGL(m_pendingReleaseVertexBuffer[i]);
                }
            }
            m_pendingReleaseVertexBuffer.clear();
		}

		{   
			csAutoLock cs(m_csIBPend);

            const u32 countIBC = (u32)m_pendingCreateIndexBuffer.size();
            for (ux i = 0; i < countIBC; ++i)
            {
                ITF_IndexBuffer* indxb = m_pendingCreateIndexBuffer[i];
                if(indxb)
                {
                    createIndexBufferGL(indxb);
                }
            }

            m_pendingCreateIndexBuffer.clear();
            
            //Index Buffer.
            const u32 countIB = (u32)m_pendingCreateUnlockIndexBuffer.size();
            for (ux i = 0; i < countIB; ++i)
            {
                ITF_IndexBuffer* indxb = m_pendingCreateUnlockIndexBuffer[i];
                if(indxb)
                {
                    unlockIndexBufferGL(indxb);
                }
            }

            m_pendingCreateUnlockIndexBuffer.clear();
            
            
            //Index Buffer.
            {
                const u32 countIB = (u32)m_pendingReleaseIndexBuffer.size();
                for (ux i = 0; i < countIB; ++i)
                {
                    releaseIndexBufferGL(m_pendingReleaseIndexBuffer[i]);
                }
            }
            m_pendingReleaseIndexBuffer.clear();
            
		}
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::beginViewportRendering(View &_view)
	{
		//DEAC cleanSetShaderCache();

		// Set RenderBuffers
        updateRenderBuffers();
        
#ifndef ITF_PROJECT_RABBIDS
		// Clear Mask Buffers
        u32 buffers = GFX_CLEAR_ZBUFFER | GFX_CLEAR_STENCIL;
        if(_view.getViewType() != View::viewType_ViewportUI)
            buffers |= GFX_CLEAR_COLOR;
		clear(
			buffers,
			m_clearColor.getRed(),
			m_clearColor.getGreen(),
			m_clearColor.getBlue(),
			m_clearColor.getAlpha()
			);
#endif

        //readPixels();
        
		// Screen dimensions
		updateShaderScreenDimension(getScreenWidth(), getScreenHeight());
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::endViewportRendering(View &_view)
	{
		//DEAC cleanSetShaderCache();
		//DEAC SYSTEM_ADAPTER_CAFE->renderSpecific();

		//DEAC cleanSetShaderCache();

#if 0 //DEAC
		if(TRC_ADAPTER) 
			TRC_ADAPTER->render(getCurrentEngineView() == e_engineView_MainScreen);
#endif

#ifndef ITF_FINAL
		switch(getZListPassDebugMode())
		{
			case GFX_ZLIST_PASS_ZPREPASS:
			{
#if 0 //DEAC
				initDefault2DRender(passCtx);

				PrimitiveContext primitiveCxt = PrimitiveContext(&passCtx);
				primitiveCxt.setPrimitiveParam((GFXPrimitiveParam*)&m_defaultPrimitiveParam);
				DrawCallContext drawCallCtx = DrawCallContext(&primitiveCxt);
				setGfxMatDefault(drawCallCtx);
				drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY);

				getRenderDepthBuffer()->Expand();
				SetTextureBind(0, getRenderDepthBuffer()->getTextureSamp());

				drawScreenQuad(drawCallCtx, 0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE);
#endif
				end2DRender();
				break;
			}
		}
#endif // ITF_FINAL

		//DEAC cleanSetShaderCache();
#ifdef _DISPLAY_DEBUGINFO_
		// Debug Infos
		if(!(getOverDrawMode()&1) && !getStrSceenshotResquest())
			drawDebugInfos(passCtx);
		#ifdef ITF_SUPPORT_DBGPRIM_ANY
		else
			clearDebugInfos();
		#endif // ITF_SUPPORT_DBGPRIM_ANY
#endif // _DISPLAY_DEBUGINFO_
    
		//DEAC cleanSetShaderCache();

#if 0 //DEAC
#ifndef ITF_FINAL
		if(m_gb_ShowHelp)
		{
			showHelp(passCtx);
		}
		else
		{
#ifdef ITF_SUPPORT_GPU_METRICS
			// Metrics
			displayMetrics(passCtx);
#endif // ITF_SUPPORT_GPU_METRICS
		}
#endif // ITF_FINAL
#endif
		//DEAC cleanSetShaderCache();
#if 1
   {
    #ifndef ITF_CONSOLE_FINAL
        // safe Frame.
        if (safeFrameDrawRequested())
        {
            drawSafeFrame();
        }
    #endif // ITF_CONSOLE_FINAL
		
		if (isBenchEnable())
			doBenchMark();

#ifndef ITF_GLES2_RELEASE
        drawDebug();
#endif
   }
#endif

		GFXAdapter::endViewportRendering(_view);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::BeginSimpleFrame()
	{
#ifdef ERROR_HANDLER_GLES2 // DEAC <- todo
		gLastRenderTime = OSGetTick();
#endif // ERROR_HANDLER_CAFE

		LockRendering();

		// Set RenderBuffers
		updateRenderBuffers();

		// Clear
		clear(GFX_CLEAR_COLOR, 0.f, 0.f, 0.f, 1.f);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::EndSimpleFrame()
	{
		// Copy ColorBuffer to ScanBuffer

		//DEAC ?? GX2SwapScanBuffers();
#ifdef VSYNC_USE_CB
		++mu32_SwapCount;
#endif // VSYNC_USE_CB

		// Flush after swap, since swap goes into the FIFO
		glFlush();

		syncGPU();

		// Unlock mutex
		UnlockRendering();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::RenderSimpleFrame()
	{
		BeginSimpleFrame();

		// Render TRC error if some
		//DEAC if(TRC_ADAPTER) 
		//DEAC	TRC_ADAPTER->render(getCurrentEngineView() == e_engineView_MainScreen);

		EndSimpleFrame();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::updateRenderBuffers()
	{
		for(u32 i=TEX_BFR_MAIN ; i<NB_TEX_BFR ; ++i)
			m_currentTextureBuffer[i] = &m_mainTextureBuffer[i];

        m_currentDepthBuffer[TEX_ZBFR_MAIN] = &m_mainDepthBuffer;

		m_currentDepthBuffer[TEX_ZBFR_REFLECTION] = &m_QSDepthBuffer;

        if( m_direct2BackBuffer )
            setRenderBuffers(0 , m_currentDepthBuffer[TEX_ZBFR_MAIN]);
        else
            setRenderBuffers(m_currentTextureBuffer[TEX_BFR_MAIN], m_currentDepthBuffer[TEX_ZBFR_MAIN]);

	}

	// ------------------------------------------------------------------------------
	
	bbool GFXAdapter_OpenGLES2::waitPreviousGPUframeEnd(u32 _depth)
	{
#if 0
		if(GX2GetSwapInterval() == 0)
			return bfalse;

#ifndef ITF_FINAL
		f64 time = SYSTEM_ADAPTER->getEllaspedFromStart();
#endif // ITF_FINAL

#ifdef VSYNC_USE_CB
		Synchronize::resetEvent(&GFXAdapter_Cafe::me_FlipEvent);

		// Wait for "_depth" frames ago to post
		while (1)
		{
			if ((mu32_FlipCount+_depth) >= mu32_SwapCount)
				break;

			// If we've waited over 10 seconds for a flip, consider the GPU hung
			// and stop running.
			if(!Synchronize::waitEventWithTimeout(&GFXAdapter_Cafe::me_FlipEvent, 10000))
			{
				#if defined(_ENABLE_DEBUGFONT_) && defined(ERROR_HANDLER_CAFE)
				if(!ForceNoLockRendering)
				#endif // defined(_ENABLE_DEBUGFONT_) && defined(ERROR_HANDLER_CAFE)
				{
					#ifndef ITF_RETAIL
					GX2PrintGPUStatus();
					LOG("GFXAdapter_Cafe::waitPreviousGPUframeEnd timed out. Potential GPU hang detected?\n");
					OSDebugStrIfConnected(0);
					#endif // ITF_RETAIL
				}
				break;
			}
		}
#else // VSYNC_USE_CB
		u32 swapCount, flipCount, waitCount=0;
		OSTime tLastFlip, tLastVsync;

		// Note: must be careful about unsigned wrap-around!
		// Wait for "_depth" frames ago to post
		while (1)
		{
			GX2GetSwapStatus(&swapCount, &flipCount, &tLastFlip, &tLastVsync);
			if ((flipCount+_depth) >= swapCount)
				break;

			// If we've waited over 10 seconds for a flip, consider the GPU hung
			// and stop running.
			if (waitCount++ > 60*GX2GetGPUTimeout()/1000)
			{
#if defined(_ENABLE_DEBUGFONT_) && defined(ERROR_HANDLER_CAFE)
				if(!ForceNoLockRendering)
#endif // defined(_ENABLE_DEBUGFONT_) && defined(ERROR_HANDLER_CAFE)
				{
#ifndef ITF_RETAIL
					GX2PrintGPUStatus();
					LOG("GFXAdapter_Cafe::waitPreviousGPUframeEnd timed out. Potential GPU hang detected?\n");
					OSDebugStrIfConnected(0);
#endif // ITF_RETAIL
				}
				break;
			}

			// Call WaitForVsync instead of WaitForFlip due to possible
			// race condition of flip happening right after above test.
			// (There will always be more vsyncs, but not always more flips.)
			GX2WaitForVsync();
		}
#endif // VSYNC_USE_CB

#ifndef ITF_FINAL
		mf64_VBLSyncTime += SYSTEM_ADAPTER->getEllaspedFromStart() - time;
#endif // ITF_FINAL

#endif
		return btrue;
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::syncGPU(u64 _timeStamp)
	{
#if 0 //DEAC 
		LockRendering();

		ITF_ASSERT(_timeStamp != GX2_INVALID_COUNTER_VALUE_U64);

		if(GX2GetRetiredTimeStamp() < _timeStamp)
		{
			if(!GX2WaitTimeStamp(_timeStamp))
			{
				LOG("syncGPU timeout.");
			}
		}

		ITF_ASSERT(GX2GetRetiredTimeStamp() >= _timeStamp);

		UnlockRendering();
#endif
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::syncGPU()
	{
#if 0 //DEAC
#ifndef ITF_FINAL
		f64 time = SYSTEM_ADAPTER->getEllaspedFromStart();
#endif // ITF_FINAL

		GX2Flush();
		syncGPU(GX2GetLastSubmittedTimeStamp());

#ifndef ITF_FINAL
		mf64_VBLSyncTime += SYSTEM_ADAPTER->getEllaspedFromStart() - time;
#endif // ITF_FINAL
#endif
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::initDefault2DRender(RenderPassContext& _rdrCtx)
	{
		init2DRender();

		_rdrCtx.getStateCache().setCullMode(GFX_CULL_NONE)
			.setAlphaBlend(GFX_BLEND_ALPHA)
			.setDepthTest(bfalse);
		SetDrawCallState(_rdrCtx.getStateCache(), 0xFFffFFff, btrue);

		setShaderGroup(m_defaultShaderGroup);

		setGlobalColor(Color::white());
		setFogColor(Color::zero());
	}

	// ------------------------------------------------------------------------------

	bbool GFXAdapter_OpenGLES2::setScreenProtection(bbool _enable)
	{
		// ????? DEAC
		return 0;
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::removeForceScreenProtection()
	{
		// ?????? DEAC
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::forceSetScreenProtection()
	{
		// ???? DEAC
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::LockRendering()
	{
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::UnlockRendering()
	{
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setupViewport(GFX_RECT* _Viewport)
	{
		m_CurrentViewport.m_x = _Viewport->left;
		m_CurrentViewport.m_y = _Viewport->top;
		m_CurrentViewport.m_width = _Viewport->right  - _Viewport->left;
		m_CurrentViewport.m_height = _Viewport->bottom - _Viewport->top;
		m_CurrentViewport.m_minZ = 0.f;
		m_CurrentViewport.m_maxZ = 1.f;

		if(m_curPassType == GFX_ZLIST_PASS_REGULAR)
			m_RegularViewport = m_CurrentViewport;

		// GXSetViewport/GX2SetScissor can't be called if another thread is rendering
		LockRendering();
		{
            u32 x, y, w, h;
            
//			if ( getRenderTextureBuffer() )
			{
                x = m_CurrentViewport.m_x;
                // super crade
                if( m_curBufferSizeY > (m_CurrentViewport.m_y + m_CurrentViewport.m_height) )
                    y = m_curBufferSizeY - (m_CurrentViewport.m_y + m_CurrentViewport.m_height);
                else
                    y =0;
                
                w = m_CurrentViewport.m_width;
                h = m_CurrentViewport.m_height;
    
			}
/*            else
            {
                // We are in the back to main
 				x =  m_CurrentViewport.m_x;
                y = m_CurrentViewport.m_y;
                w =  m_CurrentViewport.m_width;
                h = m_CurrentViewport.m_height;

            }*/
            if( x != m_GL_VP_X || y != m_GL_VP_Y || w != m_GL_VP_W || h != m_GL_VP_H )
            {
                glViewport( x, y, w, h );
                if ( ( w != m_windowWidth ) || ( h != m_windowHeight ) )
                {
			        GL_enableScissorTest();
                    glScissor( x, y, w, h );
                }
                else
                {
                    GL_disableScissorTest();
                }
                m_GL_VP_X = x;
                m_GL_VP_Y = y;
                m_GL_VP_W = w;
                m_GL_VP_H = h;
			}


		}
		UnlockRendering();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setupViewport(i32 x, i32 y, i32 w, i32 h)
	{
		GFX_RECT viewport;
		viewport.left   = x;
		viewport.top    = y;
		viewport.right  = x + w;
		viewport.bottom = y + h;
		setupViewport(&viewport);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::getViewport(GFX_Viewport & _vp)
	{
		_vp = m_CurrentViewport;
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setScissorRect(GFX_RECT* _clipRect)
	{
		if (_clipRect)
		{
			GL_enableScissorTest();
			u32 height = (u32)((_clipRect->bottom - _clipRect->top));
			glScissor
			(
				(u32)(_clipRect->left),
                // (m_windowHeight - m_CurrentViewport.m_height) : takes into account difference between viewport and main screen
				(u32)( (m_windowHeight - m_CurrentViewport.m_height) + m_CurrentViewport.m_height - _clipRect->top - height),
				(u32)((_clipRect->right  - _clipRect->left)),
				height
			);

		}
		else
		{
			GL_disableScissorTest();
		}
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setAlphaTest(bbool _enabled)
	{
		m_alphaTest.alphaTestEnable = _enabled ? 1 : 0;
		if ( !_enabled )
			m_drawCallStates.alphaTestRef = 0.0f;
		else
			m_drawCallStates.alphaTestRef = m_alphaTest.ref;
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setAlphaRef(int _ref)
	{
		m_alphaTest.ref = _ref/255.f;
		if ( !m_alphaTest.alphaTestEnable )
			m_drawCallStates.alphaTestRef = 0.0f;
		else
			m_drawCallStates.alphaTestRef = m_alphaTest.ref;
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setAlphaBlend(GFX_BLENDMODE _blend, GFX_ALPHAMASK _alphaMask)
	{
		GLenum blendOp = GL_FUNC_ADD;
		GLenum srcCoef;
		GLenum dstCoef;

		switch(_blend)
		{
			case GFX_BLEND_COPY:
				srcCoef = GL_ONE;
				dstCoef = GL_ZERO;
				break;

			case GFX_BLEND_ALPHA:
				srcCoef = GL_SRC_ALPHA;
				dstCoef = GL_ONE_MINUS_SRC_ALPHA;
				break;

			case GFX_BLEND_ALPHAPREMULT:
				srcCoef = GL_ONE;
				dstCoef = GL_ONE_MINUS_SRC_ALPHA;
				break;

			case GFX_BLEND_ALPHADEST:
				srcCoef = GL_DST_ALPHA;
				dstCoef = GL_ONE_MINUS_DST_ALPHA;
				break;

			case GFX_BLEND_ALPHADESTPREMULT:
				srcCoef = GL_DST_ALPHA;
				dstCoef = GL_ONE;
				break;

			case GFX_BLEND_ADD:
				srcCoef = GL_ONE;
				dstCoef = GL_ONE;
				break;

			case GFX_BLEND_ADDSMOOTH:
				srcCoef = GL_ONE;
				dstCoef = GL_ONE_MINUS_SRC_COLOR;
				break;

			case GFX_BLEND_ADDALPHA:
				srcCoef = GL_SRC_ALPHA;
				dstCoef = GL_ONE;
				break;

			case GFX_BLEND_SUBALPHA:
				srcCoef = GL_ZERO;
				dstCoef = GL_ONE_MINUS_SRC_COLOR;
				blendOp = GL_FUNC_SUBTRACT;
				break;

			case GFX_BLEND_SUB:
				srcCoef = GL_ZERO;
				dstCoef = GL_ONE_MINUS_SRC_COLOR;
				break;

			case GFX_BLEND_MUL:
				srcCoef = GL_ZERO;
				dstCoef = GL_SRC_COLOR;
				break;

			case GFX_BLEND_ALPHAMUL:
				srcCoef = GL_ONE;
				dstCoef = GL_ZERO;
				break;

			case GFX_BLEND_IALPHAMUL:
				srcCoef = GL_ZERO;
				dstCoef = GL_ONE_MINUS_SRC_ALPHA;
				break;

			case GFX_BLEND_IALPHA:
				srcCoef = GL_ONE_MINUS_SRC_ALPHA;
				dstCoef = GL_SRC_ALPHA;
				break;

			case GFX_BLEND_IALPHAPREMULT:
				srcCoef = GL_ONE;
				dstCoef = GL_SRC_ALPHA;
				break;

			case GFX_BLEND_IALPHADEST:
				srcCoef = GL_ONE_MINUS_DST_ALPHA;
				dstCoef = GL_DST_ALPHA;
				break;

			case GFX_BLEND_IALPHADESTPREMULT:
				srcCoef = GL_ONE_MINUS_DST_ALPHA;
				dstCoef = GL_ZERO;
				break;

			case GFX_BLEND_MUL2X:
				srcCoef = GL_DST_COLOR;
				dstCoef = GL_SRC_COLOR;
				break;

			case GFX_BLEND_ALPHATOCOLOR:
				srcCoef = GL_DST_ALPHA;
				dstCoef = GL_ZERO;
				break;

			case GFX_BLEND_IALPHATOCOLOR:
				srcCoef = GL_ONE_MINUS_DST_ALPHA;
				dstCoef = GL_ZERO;
				break;

			case GFX_BLEND_SCREEN:
				srcCoef = GL_ONE;
				dstCoef = GL_ONE_MINUS_SRC_COLOR;
				break;

			default:
				LOG("Blend mode not supported");
				srcCoef = GL_ONE;
				dstCoef = GL_ZERO;
				break;
		}

		// blend active.
		GLenum alphablendOp = GL_FUNC_ADD;
		GLenum alphasrcCoef;
		GLenum alphadstCoef;
		bbool alphaisActive = bfalse;

		switch(_alphaMask)
		{

		case GFX_ALPHAMASK_NONE :
		default :
			alphasrcCoef = GL_ZERO;
			alphadstCoef = GL_ONE;
			alphaisActive = bfalse;
			break;

		case GFX_ALPHAMASK_ADDALPHA :
			alphasrcCoef = GL_ONE;
			alphadstCoef = GL_ONE;
			alphaisActive = btrue;
			break;

		case GFX_ALPHAMASK_SUBALPHA :
			alphasrcCoef = GL_ZERO;
			alphadstCoef = GL_ONE_MINUS_SRC_ALPHA;
			alphaisActive = btrue;
			break;
		}

		m_blendColorState.op = blendOp;
		m_blendColorState.src = srcCoef;
		m_blendColorState.dst = dstCoef;
		m_blendAlphaState.op = alphablendOp;
		m_blendAlphaState.src = alphasrcCoef;
		m_blendAlphaState.dst = alphadstCoef;
		m_blendAlphaState.enable = alphaisActive;

		BlendControl ctr;
		ctr.colorSrcBlend = srcCoef;
		ctr.colorDstBlend = dstCoef;
		ctr.colorCombine = blendOp;
		ctr.separateAlphaBlend = alphaisActive ? GL_TRUE : GL_FALSE;
		ctr.alphaSrcBlend = alphasrcCoef;
		ctr.alphaDstBlend = alphadstCoef;
		ctr.alphaCombine = alphablendOp;

		m_RenderStates.SetBlendControl(ctr);

#ifdef ASSERT_ENABLED
		if(m_curPassType == GFX_ZLIST_PASS_ZPREPASS)
		{
			switch(_blend)
			{
			case GFX_BLEND_ALPHA:
			case GFX_BLEND_ALPHAPREMULT:
			case GFX_BLEND_IALPHAMUL:
				break;

			case GFX_BLEND_ALPHADEST:
			case GFX_BLEND_ALPHADESTPREMULT:
			case GFX_BLEND_ADD:
			case GFX_BLEND_ADDALPHA:
			case GFX_BLEND_SUBALPHA:
			case GFX_BLEND_SUB:
			case GFX_BLEND_MUL:
			case GFX_BLEND_ALPHAMUL:
			case GFX_BLEND_IALPHA:
			case GFX_BLEND_IALPHAPREMULT:
			case GFX_BLEND_IALPHADEST:
			case GFX_BLEND_IALPHADESTPREMULT:
			case GFX_BLEND_MUL2X:
			case GFX_BLEND_ALPHATOCOLOR:
			case GFX_BLEND_IALPHATOCOLOR:
			case GFX_BLEND_SCREEN:
				// Can't be here since we early reject this case in isCurrentPassMaterial function
				ITF_ASSERT(0);
				break;

			case GFX_BLEND_COPY:
			default:
				// If this assert occurs we need to handle the case
				ITF_ASSERT(0);
				break;
			}
		}
	#endif // ASSERT_ENABLED
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setCullMode(GFX_CULLMODE _cull)
	{
		switch(_cull)
		{
		case GFX_CULL_NONE:
				m_rasterizeControl.setCullMode( GL_NONE );
				break;
			case GFX_CULL_CCW:
				m_rasterizeControl.setCullMode( GL_BACK );
				break;
			default:
			case GFX_CULL_CW:
				m_rasterizeControl.setCullMode( GL_FRONT );
				break;
		}
		m_rasterizeControl.sendRasterizeControl();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setFillMode(GFX_FILLMODE _fillmode)
	{
		// NOT IMPLEMENTED.
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::depthSet( u32 _depthTest, u32 _depthMask )
	{
		if(_depthMask)
			m_StencilControl.depthWriteEnable = GL_TRUE;
		else
			m_StencilControl.depthWriteEnable = GL_FALSE;

		if (_depthTest)
			m_StencilControl.depthEnable = GL_TRUE;
		else
			m_StencilControl.depthEnable = GL_FALSE;

		SendDepthParams();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::depthSet( u32 _depthTest, u32 _depthMask, f32 _biasOffset, f32 _biasSlope )
	{
		depthSet( _depthTest, _depthMask );
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::depthSet( u32 _depthTest, u32 _depthMask, u32 _depthFunc, f32 _biasOffset, f32 _biasSlope )
	{
		m_StencilControl.depthFunc = (GLuint)_depthFunc;

		depthSet( _depthTest, _depthMask );
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::depthMask(u32 _depthMask)
	{
		if(_depthMask)
			m_StencilControl.depthWriteEnable = GL_TRUE;
		else
			m_StencilControl.depthWriteEnable = GL_FALSE;

		SendDepthParams();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::depthFunc(u32 _func)
	{
		m_StencilControl.depthFunc = (GLuint)_func;
		SendDepthParams();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::depthTest(u32 _test)
	{
		if (_test)
			m_StencilControl.depthEnable = GL_TRUE;
		else
			m_StencilControl.depthEnable = GL_FALSE;

		SendDepthParams();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::depthStencil(
		bbool stencilTestEnable,
		GFX_CMPFUNC stencilFunc,
		GFX_STENCILFUNC stencilZPass,
		GFX_STENCILFUNC stencilZFail,
		GFX_STENCILFUNC stencilFail,
		u8 ref,
		u8 preMask,
		u8 writeMask
		)
	{

		m_StencilControl.stencilTestEnable = (GLboolean)stencilTestEnable;
		m_StencilControl.frontStencilFunc  = getCmpFunc(stencilFunc);
		m_StencilControl.frontStencilZPass = getStencilOp(stencilZPass);
		m_StencilControl.frontStencilZFail = getStencilOp(stencilZFail);
		m_StencilControl.frontStencilFail  = getStencilOp(stencilFail);
		m_StencilControl.ref = ref;
		m_StencilControl.preMask = preMask;
		m_StencilControl.writeMask = writeMask;

		SendDepthParams();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::colorMask(GFX_COLOR_MASK_FLAGS _colorMask)
	{
        if( _colorMask != m_currentColorMask )
        {
		    GLboolean redMask = ( _colorMask & GFX_COL_MASK_RED ) ? GL_TRUE : GL_FALSE;
		    GLboolean greenMask = ( _colorMask & GFX_COL_MASK_GREEN ) ? GL_TRUE : GL_FALSE;
		    GLboolean blueMask = ( _colorMask & GFX_COL_MASK_BLUE ) ? GL_TRUE : GL_FALSE;
		    GLboolean alphaMask = ( _colorMask & GFX_COL_MASK_ALPHA ) ? GL_TRUE : GL_FALSE;
            glColorMask(redMask, greenMask, blueMask, alphaMask);

            m_currentColorMask = _colorMask;
        }
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::prepareGfxMatReflection( const GFX_MATERIAL& _gfxMat )
	{
		//DEAC if(isUseReflection())
		//DEAC 	SetTextureBind(1, getReflectionBuffer());
		f32 reflectorFactor = _gfxMat.getTemplateMatParams().getfParamsAt(0) * _gfxMat.getMatParams().getfParamsAt(0);
		m_drawCallStates.reflectionParam.set(reflectorFactor, 0.0f, 0.0f, 0.0f);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::prepareGfxMatFluid( const GFX_MATERIAL& _gfxMat )
	{

		setShaderGroup(m_defaultShaderGroup);
		bbool useGlow = ( _gfxMat.getMatParams().getiParamsAt(0) != 0);

		/// try normal texture.
		if (getFluidBlurBuffer() != NULL)
		{
			SetTextureBind(1, getFluidBlurBuffer(), bfalse);
		}

		if (useGlow)
		{
			m_currentTextureBuffer[TEX_BFR_FLUID_2]->Invalidate();
			SetTextureBind(2, getFluidTmpBuffer(), bfalse);
		}

		PS_Fluid fluidConsts;

		fluidConsts.fluidParam.set(_gfxMat.getMatParams().getfParamsAt(0), _gfxMat.getMatParams().getfParamsAt(1), _gfxMat.getMatParams().getfParamsAt(2), _gfxMat.getMatParams().getfParamsAt(3));
		float ramp1 = 1.0f / (fluidConsts.fluidParam.m_x - fluidConsts.fluidParam.m_y);
		float ramp2 = fluidConsts.fluidParam.m_y / (fluidConsts.fluidParam.m_x - fluidConsts.fluidParam.m_y);
		float flowSizeInv = (1.0f / fluidConsts.fluidParam.m_z) * 2.0f;
		float ramp1MinusRamp2 = ramp1 - ramp2;
		fluidConsts.fluidParam2.set(ramp1, ramp2, flowSizeInv, ramp1MinusRamp2);

		fluidConsts.fluidColor1 = _gfxMat.getMatParams().getvParamsAt(0);
		fluidConsts.fluidColor2 = _gfxMat.getMatParams().getvParamsAt(1);
		fluidConsts.fluidColor3 = _gfxMat.getMatParams().getvParamsAt(2);
		fluidConsts.fluidColor4 = _gfxMat.getMatParams().getvParamsAt(3);
		fluidConsts.fluidColor5 = _gfxMat.getMatParams().getvParamsAt(4);
		SetPixelShaderConstantF( PS_Attrib_fluid, (f32*)&fluidConsts, sizeof(fluidConsts)/16 );

		m_fluidSimuInfo.m_useEmboss = (_gfxMat.getMatParams().getiParamsAt(1) != 0);
	}

#ifndef ITF_FINAL

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::RequestDisplayMessageBox(String8 *_title, String8 *_msg, u32 _type, volatile u32 *_res)
	{
#if 0 //DEAC UITLE ??
		GXOperation op(GX_DISPLAY_MESSAGE_BOX, _res,_title, _msg, _type, 0);

		Synchronize::enterCriticalSection(&m_csGXOperation);
		m_OpList.push_back(op);
		Synchronize::leaveCriticalSection(&m_csGXOperation);
#endif	
	}

#endif // ITF_FINAL

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::setRenderTargetForPass(GFX_ZLIST_PASS_TYPE _passType, bbool _restoreContext)
	{
		m_curPassType = _passType;
        
        
        if( (_passType == GFX_ZLIST_PASS_REGULAR) && GFX_ADAPTER->getStrSceenshotResquest() )
        {
            // We need to set the correct viewport during screenshot
            // do this here in case we're not in the regular pass when starting the screenshot
            setupViewport(0, 0, getWindowWidth(), getWindowHeight() );
        }
        
// 		eTexBfr TexId = TEX_BFR_MAIN;
// 		eTexZBfr ZBufferId = TEX_ZBFR_MAIN;
		TextureBuffer* rt = m_currentTextureBuffer[TEX_BFR_MAIN];
		DepthBuffer* zb = m_currentDepthBuffer[TEX_ZBFR_MAIN];
		switch(m_curPassType)
		{
		case GFX_ZLIST_PASS_FRONT_LIGHT:
			rt = getFrontLightSurface(m_lightManager->getFrontLightBufferType());
			zb = 0;
			break;
		case GFX_ZLIST_PASS_BACK_LIGHT:
			rt = getBackLightSurface(m_lightManager->getBackLightBufferType());
			zb = 0;
			break;
		case GFX_ZLIST_PASS_REFRACTION:
			rt = m_currentTextureBuffer[TEX_BFR_REFRACTION];
			break;
		case GFX_ZLIST_PASS_REFLECTION_PREPASS:
			rt = m_currentTextureBuffer[TEX_BFR_REFLECTION];
			zb = m_currentDepthBuffer[TEX_ZBFR_REFLECTION];
			break;
		case GFX_ZLIST_PASS_REFLECTION:
			rt = m_currentTextureBuffer[TEX_BFR_REFLECTION];
			zb = m_currentDepthBuffer[TEX_ZBFR_REFLECTION];
			break;
		case GFX_ZLIST_PASS_FLUID:
			rt = m_currentTextureBuffer[TEX_BFR_FLUID_2];
			zb = 0;
			break;
		case GFX_ZLIST_PASS_REGULAR:
		case GFX_ZLIST_PASS_ZPREPASS:
			break;
		}
        
        if( m_direct2BackBuffer )
            setRenderBuffers(0 , zb);
        else
        {
            setRenderBuffers(rt , zb);

            setupViewport
            (
                (i32)(m_RegularViewport.m_x),
                (i32)(m_RegularViewport.m_y),
                (i32)(m_RegularViewport.m_width),
                (i32)(m_RegularViewport.m_height)
             );
        }
    }

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::resolveRenderTargetForPass(GFX_ZLIST_PASS_TYPE _passType)
	{
		switch(_passType)
		{
			case GFX_ZLIST_PASS_FRONT_LIGHT:
				getFrontLightSurface(m_lightManager->getFrontLightBufferType())->Invalidate();
				break;
			case GFX_ZLIST_PASS_BACK_LIGHT:
				getBackLightSurface(m_lightManager->getBackLightBufferType())->Invalidate();
				break;
			case GFX_ZLIST_PASS_REFRACTION:
				m_currentTextureBuffer[TEX_BFR_REFRACTION]->Invalidate();
				break;
			case GFX_ZLIST_PASS_REFLECTION:
				m_currentTextureBuffer[TEX_BFR_REFLECTION]->Invalidate();
				break;
			case GFX_ZLIST_PASS_FLUID:
				m_currentTextureBuffer[TEX_BFR_FLUID_2]->Invalidate();
				m_currentTextureBuffer[TEX_BFR_FLUID_BLUR]->Invalidate();
				break;
		}
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::PrepareRenderTarget(TextureBuffer* _dstTex, DepthBuffer* _dstDepth)
	{
		const f32 Width  = (f32)_dstTex->getWidth();
		const f32 Height = (f32)_dstTex->getHeight();

		getWorldViewProjMatrix()->push();
		setOrthoView(0.f, Width, 0.f, Height);
		setupViewport(0, 0, (i32)Width, (i32)Height);

		setRenderBuffers(_dstTex, _dstDepth);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::RestoreRenderTarget()
	{
		getWorldViewProjMatrix()->pop();
		// restore camera
		View::getCurrentView()->apply();

		setRenderTargetForPass(m_curPassType);
	}

#ifndef ITF_FINAL
	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::startRasterForPass(GFX_ZLIST_PASS_TYPE _passType)
	{
		//DEAC GFX_METRICS_START_TAG(_passType);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::endRasterForPass(GFX_ZLIST_PASS_TYPE _passType)
	{
		//DEAC GFX_METRICS_END_TAG(_passType);
	}
#endif // ITF_FINAL

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::beginGPUEvent(const char* _name, GPU_MARKER _marker)
	{
#ifdef ITF_WIIU_PROFILER
		GX2UTDebugTagIndent(_name);
#endif
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::endGPUEvent( GPU_MARKER _marker)
	{
		// DEAC ???
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::mainBufferToBackBuffer()
	{
		RenderPassContext rdrCtx;

		CopySurface(rdrCtx, 0, m_currentTextureBuffer[TEX_BFR_MAIN], GFX_BLEND_COPY, btrue, 1.f);
	}


	void GFXAdapter_OpenGLES2::copyScene()
	{
		RenderPassContext rdrCtx;
		CopySurface(rdrCtx, m_currentTextureBuffer[TEX_BFR_SCENE_COPY], m_currentTextureBuffer[TEX_BFR_MAIN], GFX_BLEND_COPY, btrue, 1.f);
		
		//Force alpha value to 1
		PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
		DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

		const f32 Width  = (f32)m_currentTextureBuffer[TEX_BFR_SCENE_COPY]->getWidth();
		const f32 Height = (f32)m_currentTextureBuffer[TEX_BFR_SCENE_COPY]->getHeight();

		getWorldViewProjMatrix()->push();
		setOrthoView(0.f, Width, 0.f, Height);
		setupViewport(0, 0, (i32)Width, (i32)Height);

		setRenderBuffers(m_currentTextureBuffer[TEX_BFR_SCENE_COPY], getRenderDepthBuffer());
		setShaderGroup(m_defaultShaderGroup);
		setGfxMatDefault(drawCallCtx);

		drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
			.setAlphaWrite(btrue)
			.setColorWrite(bfalse)
			.setSeparateAlphaBlend(GFX_ALPHAMASK_ADDALPHA)
			.setDepthTest(bfalse);
		drawScreenQuad(drawCallCtx, 0.f, 0.f, Width, Height, 0.f, COLOR_WHITE, btrue);

		// restore camera
		View::getCurrentView()->apply();
		getWorldViewProjMatrix()->pop();
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::drawScreenTransition(f32 _fadeVal)
	{
		const screenTransition& transition = m_screenTransitionManager.getCurrentTransition();
		f32 fade = _fadeVal;

        init2DRender();

		u32 screenwidth;
		u32 screenHeight;
        screenwidth = getScreenWidth();
        screenHeight = getScreenHeight();

#ifdef ITF_USE_REMOTEVIEW
		engineView view = (engineView)getCurrentEngineView();
		if(transition.views != view && transition.views != e_engineView_Both)
			return;
		if(view != e_engineView_MainScreen)
		{
			screenwidth = getRemoteScreenWidth();
			screenHeight = getRemoteScreenHeight();
		}
#endif
        
		
        RenderPassContext rdrCtx;
		rdrCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
			.setAlphaTest(bfalse)
			.setDepthTest(bfalse);
		SetDrawCallState(rdrCtx.getStateCache(), 0xFFFFFFFF, btrue);
		PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
		DrawCallContext drawCallCtx = DrawCallContext(&primCtx);
		setShaderGroup(m_defaultShaderGroup);
		setGfxMatDefault(drawCallCtx);
		setGlobalColor(Color::white());

		SetTextureBind(0, getSceneCopyBuffer());

		if(transition.type == GFX_ST_TYPE_FADE)
		{
			fade *= 2.0f;
			Color fadeColor = transition.color;
			if(fade > 1.0f)
			{
				//Fade out : draw new scene
				fade = 2.0f - fade;
				fadeColor = Interpolate(Color(0.0f, 1.0f, 1.0f, 1.0f), fadeColor, fade);
				drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA)
					.setAlphaTest(bfalse)
					.setDepthTest(bfalse);
				drawScreenQuad(drawCallCtx, 0, 0, (f32)screenwidth, (f32)screenHeight, 0, fadeColor.getAsU32(), btrue);
			}
			else
			{
				//Fade in : draw old copy scene
				fadeColor = Interpolate(Color::white(), fadeColor, fade);
				drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
					.setAlphaTest(bfalse)
					.setDepthTest(bfalse);
				drawScreenQuad(drawCallCtx, 0, 0, (f32)screenwidth, (f32)screenHeight, 0, fadeColor.getAsU32(), bfalse);
			}
		}
		else if(transition.type == GFX_ST_TYPE_SLIDE_LEFT || transition.type == GFX_ST_TYPE_SLIDE_RIGHT)
		{
			f32 slide = fade * screenwidth;
			if(transition.type == GFX_ST_TYPE_SLIDE_LEFT)
				slide *= -1;
			drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY);
			drawScreenQuad(drawCallCtx, slide, 0, (f32)screenwidth, (f32)screenHeight, 0, 0xFFFFFFFF, bfalse);
		}
		else if(transition.type == GFX_ST_TYPE_SLIDE_UP || transition.type == GFX_ST_TYPE_SLIDE_DOWN)
		{
			f32 slide = fade * screenHeight;
			if(transition.type == GFX_ST_TYPE_SLIDE_DOWN)
				slide *= -1;
			drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY);
			drawScreenQuad(drawCallCtx, 0, slide, (f32)screenwidth, (f32)screenHeight, 0, 0xFFFFFFFF, bfalse);
		}
		else if(transition.type == GFX_ST_TYPE_ZOOM_OUT)
		{
			Color fadeAlpha = Color::white();
			fadeAlpha.m_a = (1.0f - fade);
			f32 scaleX = fade * screenwidth;
			f32 scaleY = fade * screenHeight;
			drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA);
			drawScreenQuad(drawCallCtx, scaleX/ 2.0f, scaleY/ 2.0f, (f32)screenwidth - scaleX, (f32)screenHeight - scaleY, 0, fadeAlpha.getAsU32(), bfalse);
		}
		else if(transition.type == GFX_ST_TYPE_ZOOM_IN)
		{
			const f32 zoomFactor = 1.0f;
			Color fadeAlpha = Color::white();
			fadeAlpha.m_a = (1.0f - fade);
			f32 scaleX = fade * screenwidth * zoomFactor;
			f32 scaleY = fade * screenHeight * zoomFactor;
			drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_ALPHA);
			drawScreenQuad(drawCallCtx, -scaleX/2.0f, -scaleY/2.0f, (f32)screenwidth + scaleX, (f32)screenHeight + scaleY, 0, fadeAlpha.getAsU32(), bfalse);
		}

		end2DRender();
	}

	// ------------------------------------------------------------------------------

	
void GFXAdapter_OpenGLES2::displayDebugInfoGLES2(DrawCallContext &_drawCallCtx)
{
#ifdef ITF_SUPPORT_DBGPRIM_MISC
    for (u32 it = 0; it < m_DBGTextureQuads.size();)
    {
        DBGTextureQuad& t = m_DBGTextureQuads[it];
        for (int i = 0; i < 4; i++)
        {
            t.mVtx[i].m_pos.y() = getWindowHeight() - t.mVtx[i].m_pos.y();
            t.mVtx[i].m_pos.z() = 0.f;
        }
        drawTriangle(_drawCallCtx, t.mVtx[0], t.mVtx[1], t.mVtx[2], t.t);
        drawTriangle(_drawCallCtx, t.mVtx[1], t.mVtx[2], t.mVtx[3], t.t);
        t.duration -= LOGICDT;
        if (t.duration <= 0.0f)
            m_DBGTextureQuads.eraseKeepOrder(it);
        else
            it++;
    }
#endif // ITF_SUPPORT_DBGPRIM_MISC

    /// Debug info.
    /// Show fps.
#ifdef ITF_SUPPORT_DBGPRIM_TEXT

    GFX_RECT rct;
    rct.left=10;
    rct.right=780;
#ifdef ITF_IOS
    rct.top=30; // skip the first line, iOS 7 displays its own stuff on it
#else
    rct.top=10;
#endif
    rct.bottom=rct.top+20;
    
    //m_spriteInterface->Begin(D3DXSPRITE_ALPHABLEND|D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);

#ifndef ITF_DISABLE_DEBUGINFO
//     if ( 1 ) // isShowDebugInfo())
    {
        // const f32 diffFPS = APPLOOPFPS - getfPs();

        char fps[1024];
#ifdef ITF_ANDROID
#ifdef PEL_PROFILER_ENABLED
		bbool isProfiling = Profiler::GetCapturingEvents();
		if (isProfiling)	
			sprintf( fps, "%0.02f fps -PROFILER-\0", getfPs());
		else
#endif
    	{ sprintf( fps, "%0.02f fps\0", getfPs()); }
		//{ sprintf( fps, "%0.02f fps bench: %.f\0", getfPs(), getBenchTime()); }
    	float ffps = getfPs();
    	float _r=0.f,_g=0.f,_b=0.f;
    	if(ffps > 25.0f) {_g = 1.f;} else if(ffps > 20.0f){_r = 1.f;_g = 1.f;} else{_r = 1.f;}
    	Color color = Color::black(); color.m_a = 0.66f;
    	const u32 uColor = color.getAsU32();
    	GFX_ADAPTER->drawDBG2dBox(Vec2d(5.0f, 10.f), 85.f, 25.f, uColor, uColor, uColor, uColor);
    	GFX_ADAPTER->drawDBGText(fps, 11.f, 15.f, _r, _g, _b, /*_useBigFont*/btrue );
#else

#ifdef ITF_W1W_MOBILE
        float ffps = getfPs();
        sprintf( fps, "%0.02f (%s) \0", ffps ,  m_engineDataVersion.cStr());

        if(ffps > 25.0f)
            GFX_ADAPTER->drawDBGText(fps, 0.1f ,float(rct.top), 0.0f, 1.0f, 0.0f );
        else if (ffps > 20.0f)
            GFX_ADAPTER->drawDBGText(fps, 0.1f ,float(rct.top), 0.0f, 0.0f, 1.0f );
        else
            GFX_ADAPTER->drawDBGText(fps, 0.1f ,float(rct.top), 1.0f, 0.0f, 0.0f );

#else


        /*u32 len =*/ _snprintf( fps, sizeof(fps), "%0.02f fps (%llu frame(s) dropped) %s - dataVersion %s", getfPs(), SINGLETONS.getLostFrameCount(), m_extraInfo.cStr(), m_engineDataVersion.cStr() );
        GFX_ADAPTER->drawDBGText(fps, 0.1f ,float(rct.top), 1.0f, 1.0f, 1.0f );
#endif
#endif





		

//         u32 color = 0x80000000;
        //GFX_ADAPTER->drawDBG2dBox( Vec2d(0.1f , (f32) rct.top), f32(len) * 8, 20, color, color, color, color);
    }
#endif // ITF_DISABLE_DEBUGINFO

    rct.top=getScreenHeight()-30;
    rct.bottom=rct.top+20;

    static ITF_VECTOR<DBGText> stackDBGTexts;

	GFX_BitmapFont* bmptxt = GFX_gpDebugTextManager;

    ITF_VECTOR<DBGText>::iterator it = m_DBGTexts.begin();
    ITF_VECTOR<DBGText>::iterator end = m_DBGTexts.end();

    stackDBGTexts.reserve(m_DBGTexts.size());
    for (; it != end; ++it)
    {
        const DBGText& dbgText = (*it);
        if (dbgText.m_x < (ITF_POS2D_AUTO+0.1f) && dbgText.m_y < (ITF_POS2D_AUTO+0.1f) && (it->m_alignment == FONT_ALIGN_LEFT))
        {
            stackDBGTexts.push_back(dbgText);
        }
        else
        {
            //UTF8ToUTF16 strUTF16Converter(dbgText.m_text.cStr());
            //LPCWSTR strUTF16 = (LPCWSTR)strUTF16Converter.get();

            GFX_RECT localRect;
//             DWORD alignment = 0;//fontAlignmentToDirectXFormat(it->m_alignment);
            localRect.left=(i32)((*it).m_x);
            localRect.right=getScreenWidth();
            localRect.top=(i32)((*it).m_y);
            localRect.bottom=rct.top+20;
 
			u32 color = ITFCOLOR_TO_U32( 0xFF, (u32)((*it).m_r*255.f), (u32)((*it).m_g*255.f), (u32)((*it).m_b*255.f) );
			bmptxt->AddTextToDrawF(
				float(localRect.left),
				float(localRect.top),
				false,
				color, 
				0xffff00ff,
				dbgText.m_text.cStr()
				);

			/*
			if (dbgText.m_useMonospaceFont)
                mp_FontDebug_Monospace->DrawTextW(m_spriteInterface, strUTF16, -1 ,&localRect, alignment, color);
            else
                if (dbgText.m_useBigFont)
                    mp_FontDebug_Big->DrawTextW(m_spriteInterface, strUTF16, -1 ,&localRect, alignment, color);
                else
                    mp_FontDebug->DrawTextW(m_spriteInterface, strUTF16, -1 ,&localRect, alignment, color);
			*/
		}
    }
    m_DBGTexts.clear();

    

    static int FontsOn = 1; // debug
    if( FontsOn )
    {
    if(stackDBGTexts.size())
    {
        const u32 uColor = ITFCOLOR_TO_U32(150, 100, 100, 100);

        it = stackDBGTexts.begin();
        end = stackDBGTexts.end();
        while(it != end)
        {
            const DBGText& txt = (*it);
//             const u32 color = 0xffffffff;//D3DCOLOR_ARGB(255, (u32)(txt.m_r*255.f), (u32)(txt.m_g*255.f), (u32)(txt.m_b*255.f));

            rct.top -= 15;
            rct.bottom = rct.top+15;

            //UTF8ToUTF16 strUTF16Converter(txt.m_text.cStr());
            //LPCWSTR strUTF16 = (LPCWSTR)strUTF16Converter.get();
            ux nbUniChar = getNbUnicodeChar(txt.m_text.cStr());

            drawDBG2dBox(Vec2d(f32(rct.left - 2), f32(rct.top)), nbUniChar * 7.0f + 2, 15.0f, uColor, uColor, uColor, uColor);
            //DEAC DWORD alignment = fontAlignmentToDirectXFormat(txt.m_alignment);

// 			const float height = bmptxt->getRelativeHeight();

			bmptxt->AddTextToDrawF(
				float(rct.left),
				float(rct.top),
				false,
				0xffffffff,
				0xffff00ff,
				txt.m_text.cStr()
				);

			/*
            if ((*it).m_useMonospaceFont)
                mp_FontDebug_Monospace->DrawTextW(m_spriteInterface, strUTF16, -1 ,&rct, alignment, color);
            else
                if ((*it).m_useBigFont)
                    mp_FontDebug_Big->DrawTextW(m_spriteInterface, strUTF16, -1 ,&rct, alignment, color);
                else
                    mp_FontDebug->DrawTextW(m_spriteInterface, strUTF16, -1 ,&rct, alignment, color);
			*/

            ++it;
        }
        stackDBGTexts.clear();
    
	}

	bmptxt->DrawAllTexts(_drawCallCtx, 0);
    }

    //m_spriteInterface->End();
#endif // ITF_SUPPORT_DBGPRIM_TEXT
}

#ifndef ITF_SUPPORT_EDITOR
	void GFXAdapter_OpenGLES2::waitEndOfProcessing()
	{
		if( Synchronize::getCurrentThreadId() != ThreadInfo::getGraphicThreadId() )
		{
            // no more
            /*
			while( ((GFXAdapter_OpenGLES2*)GFX_ADAPTER)->isAdapterStillProcessing() )
			{
				sleep(2);
			}
            */
		}
	}
#endif

	//*************************************
	// New texture Targets.   


	GFXAdapter_OpenGLES2::RenderTarget_GLES2::RenderTarget_GLES2(RenderTargetInfo &_info)
	{
		m_info = _info;

        glGenTextures(1, &hwdData);
        glBindTexture(GL_TEXTURE_2D, hwdData);
        
        
        u32 mipIndex = 0;
        GLint internalFmt = GL_RGBA;
        GLenum format = GL_RGBA;
        GLenum type = GFX_ADAPTER_OPENGLES2->m_direct2BackBuffer ? GL_UNSIGNED_SHORT_4_4_4_4 : GL_UNSIGNED_BYTE;
//        u32 size = getWidth() * getHeight() * 4; // 4 = RGBA
        
        glTexImage2D(GL_TEXTURE_2D, mipIndex, internalFmt, m_info.m_width, m_info.m_heigth, 0, format, type, 0);
        // These 2 MUST BE set on IOS or the texture cannot be used
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  	}
	GFXAdapter_OpenGLES2::RenderTarget_GLES2::~RenderTarget_GLES2()
	{
		if (hwdData)
		{
			glDeleteTextures(1, &hwdData);					
		}			
	}

	RenderTarget* GFXAdapter_OpenGLES2::createRenderTarget( RenderTargetInfo &_info )
	{
		RenderTarget *rt = newAlloc(mId_Textures, RenderTarget(_info));
		RenderTarget_GLES2 *platformRT = newAlloc(mId_Textures, RenderTarget_GLES2(_info));

		rt->m_adapterimplementationData = platformRT;

		return rt;
	}

	void GFXAdapter_OpenGLES2::setRenderContext( RenderContext &_context )
	{
		ITF_ASSERT(_context.m_colorBufferCount == 1 );
		ITF_ASSERT(_context.m_depthBuffer == NULL );
		RenderTarget *colorBuffer = _context.m_colorBuffers[0];
		RenderTarget_GLES2 *colorBufferGles2 = (RenderTarget_GLES2 *)colorBuffer->m_adapterimplementationData;
		
		GLuint depthId = 0;
        GLuint colorId = colorBufferGles2->hwdData;
		u64 key = ((u64)colorId)<<32 | (u64)depthId;
		
		GLuint* pclbf = m_frameBuffersIds.find(key);

		if( pclbf )
		{
			GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, *pclbf) );
		}
		else
		{
            GLuint fbo;
            GL_CHECK(glGenFramebuffers(1, &fbo));
            GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

            GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorId, 0));

            GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthId));
            GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthId));

			m_frameBuffersIds.insert(key, fbo);
		}

		m_curBufferSizeX = colorBuffer->getSizeX();
		m_curBufferSizeY = colorBuffer->getSizeY();
		setupViewport( 0, 0, colorBuffer->getSizeX(), colorBuffer->getSizeY() );
	}

	void GFXAdapter_OpenGLES2::prepareLockableRTAccess(	RenderTarget* _target)
	{

	}

	void GFXAdapter_OpenGLES2::lockRenderTarget( RenderTarget* _target, LOCKED_TEXTURE * _lockTex)
	{

	}

	void GFXAdapter_OpenGLES2::unlockRenderTarget( RenderTarget* _target)
	{

	}
} // namespace ITF
