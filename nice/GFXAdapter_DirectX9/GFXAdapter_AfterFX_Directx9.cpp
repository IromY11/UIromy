#include "precompiled_GFXAdapter_Directx9.h"

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

///////////////////////////////////////////////////////////////////////////////
///////////////////////       implementation of various shapes drawing
///////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
    #include <windows.h>        // otherwise GL.h and GLU.h won't compile on PC
#endif // WIN32

#ifdef ITF_X360
#include <fxl.h>
#include <xgraphics.h>
#endif

#include <math.h>
#include <d3dx9.h>
#include <algorithm>

#ifdef ITF_WINDOWS
#include "glfw/include/gl/glfw.h"
#endif //ITF_WINDOWS

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_
#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/RastersManager.h"
#endif //_ITF_RASTERSMANAGER_H_

namespace ITF
{
#ifndef GFX_COMMON_POSTEFFECT
    DECLARE_RASTER(afterFX, RasterGroup_Misc, Color::violet());

    const f32   GFXAdapter_Directx9::cRadialBlurScaleSize = 0.5f;


void GFXAdapter_Directx9::DrawAFXPostProcess(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess)
{
    if(!_postProcess.isActived())
    {
        //if call drawAFXPostProcess with no AFX, we need to restore backBuffer
        setBackBuffer(bfalse, btrue);
        return;
    }

#ifdef ITF_X360
    m_pd3dDevice->SetShaderGPRAllocation(0, 16, 112);
#endif

    setBackBuffer(bfalse, bfalse);

    AFXPostProcessProcessAllowCombinatory(_postProcess);

    //Prepare AFX pass
    f32 copyScale = 1.0f;
    if(_postProcess.m_tile.m_use)
    {
        copyScale = f32_Lerp(1.0f, 1.0f/_postProcess.m_tile.m_repetition, _postProcess.m_tile.m_fade);
    }
    else if(_postProcess.m_radial.m_use && !(_postProcess.m_blur.m_use || _postProcess.m_glow.m_use))
    {
        copyScale = cRadialBlurScaleSize;
    }

    u32 rtIdx = 0;
    bbool b_needBlur = _postProcess.m_blur.m_use || _postProcess.m_glow.m_use;
    if(b_needBlur)
    {
        rtIdx = _postProcess.m_blur.m_use?_postProcess.m_blur.m_quality:_postProcess.m_glow.m_quality;
        rtIdx = rtIdx%2;
        m_currentAFXBlurTexture = getSceneTexture(rtIdx, copyScale, btrue);
    }

    if(_postProcess.m_colorSetting.m_use || _postProcess.m_refraction.m_use
        || _postProcess.m_tile.m_use || _postProcess.m_mosaic.m_use
        || _postProcess.m_negatif.m_use || _postProcess.m_kalei.m_use
        || _postProcess.m_eyeFish.m_use || _postProcess.m_mirror.m_use
        || _postProcess.m_oldTV.m_use || _postProcess.m_noise.m_use
        || _postProcess.m_radial.m_use)
    {
        //Prepare copy if needed
        m_currentAFXSceneTexture = getSceneTexture(rtIdx, copyScale, bfalse);
    }

    GFX_ADAPTER->beginGPUEvent("AFX postProcess", GFX_GPUMARKER_AFX);
    BeginAFXPostProcess(btrue);

    if(b_needBlur)
    {
        //draw blurred scene in additionnal renderTarget 0
        GFX_ADAPTER->beginGPUEvent("Blur");
        if(_postProcess.m_blur.m_use)
        {
            PrepareBlurTarget(_rdrCtxt, _postProcess.m_blur.m_pixelSize, _postProcess.m_blur.m_quality);
        }
        else
        {
            PrepareBlurTarget(_rdrCtxt, _postProcess.m_glow.m_pixelSize, _postProcess.m_glow.m_quality);
        }
        GFX_ADAPTER->endGPUEvent();
    }

    AFXpostProcessPass(_rdrCtxt, _postProcess);

    if(_postProcess.m_kalei.m_use)
    {
        AFXPostProcessKaleiPass(_rdrCtxt, _postProcess);
    }

    EndAFXPostProcess(btrue, GFX_BLEND_ADDALPHA);
    GFX_ADAPTER->endGPUEvent(GFX_GPUMARKER_AFX);
}

//Blur RenderTarget
void GFXAdapter_Directx9::BlurRenderBuffer(RenderPassContext & _rdrCtxt, u32 _pixelBlurSize, u32 _quality, RenderTarget* _srcTex,
                                           RenderTarget* _dstTex, RenderTarget* _tmpTex)
{
    if(_quality <= 0)
        return;

    PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

    GFX_ZLIST_PASS_TYPE oldPassType = getCurPassType();
    m_curPassType = GFX_ZLIST_PASS_REGULAR;

	GFX_RenderingBuffer_DX9 *dstTex = getTargetDX(_dstTex);

    u32 W = dstTex->getWidth();
    u32 H = dstTex->getHeight();

    if(_tmpTex == NULL)
        _quality = 1;

    u32 destId = (_quality+1)%2;
    u32 sourceId = _quality%2;

    getWorldViewProjMatrix()->push();
    setOrthoView(0.f, (f32)W, 0.f, (f32)H);

    setShaderGroup(mc_shader_AfterFx, mc_entry_afterFx_VS_blur, mc_entry_afterFx_PS_bigBlur);
    setGfxMatDefault(drawCallCtx);

    drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
        .setDepthTest(bfalse);

    const f32 ratiopixelX = 1.f/(f32)W;
    const f32 ratiopixelY = 1.f/(f32)H;
    const f32 centrof = 0.5f;
    f32 displaceX = u32(_pixelBlurSize / 2.0f) * ratiopixelX; //4 pixel Displace
    f32 displaceY = u32(_pixelBlurSize / 2.0f) * ratiopixelY;
    const f32 qualityDisplaceX = displaceX / _quality;
    const f32 qualityDisplaceY = displaceY / _quality;

    u32 pass = 0;

    RenderTarget* texDest = (destId==0)?_dstTex:_tmpTex;
    RenderTarget* texSource = _srcTex;

    while ( (_quality && pass < _quality))
    {
		RenderContext rtContext(texDest);
		setRenderContext(rtContext);

        bindTexture(0, texSource, btrue, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
        setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);

        /// shader const.
        GFX_Vector4 vconst1(displaceX, displaceY, displaceX, displaceY);
        SetVertexShaderConstantF(VS_Attrib_afx, (f32*)&vconst1, 1);

        drawScreenQuadC(drawCallCtx, 0.f, 0.f, (f32)W, (f32)H, 0.f, COLOR_WHITE, centrof);

        resolve( texDest );

        //swap target
        destId = (destId+1)%2;
        sourceId = (sourceId+1)%2;

        texDest = (destId==0)?_dstTex:_tmpTex;
        texSource = (sourceId==0)?_dstTex:_tmpTex;

        displaceX -= qualityDisplaceX;
        displaceY -= qualityDisplaceY;
        pass++;
    }

    getWorldViewProjMatrix()->pop();

    m_curPassType = oldPassType;
    bindTexture(0, NULL);
}

// Draw blur Scene in Additionnal RenderTarget 0
void GFXAdapter_Directx9::PrepareBlurTarget(RenderPassContext & _rdrCtxt, u32 _pixelBlurSize, u32 _quality)
{
#ifdef ITF_X360
    u32 currentDst = TEX_BFR_QRT_AFX_1;
    if(_quality > 0 && _pixelBlurSize > 1.0f)
    {
        u32 currentSrc = TEX_BFR_FULL_1;
        if(_pixelBlurSize > 4.0f)
        {
            u32 pass = 1;
            bbool lastPass = (pass >= _quality);
            copyTextureToEDRAM(m_currentAFXBlurTexture, m_AdditionnalRenderingBuffer[currentSrc]);
            blur4x4(_rdrCtxt, m_AdditionnalRenderingBuffer[currentSrc], m_AdditionnalRenderingBuffer[currentDst], m_AdditionnalRenderingBuffer[TEX_BFR_QRT_1], (f32)_pixelBlurSize, lastPass );
            while (lastPass == bfalse)
            {
                _pixelBlurSize /= 2;
                lastPass = (pass >= _quality) || (_pixelBlurSize <= 2);
                currentSrc = (currentSrc == TEX_BFR_QRT_AFX_1)?TEX_BFR_QRT_AFX_2:TEX_BFR_QRT_AFX_1;
                currentDst = (currentDst == TEX_BFR_QRT_AFX_1)?TEX_BFR_QRT_AFX_2:TEX_BFR_QRT_AFX_1;
                blur4x4(_rdrCtxt, m_AdditionnalRenderingBuffer[currentSrc], m_AdditionnalRenderingBuffer[currentDst], m_AdditionnalRenderingBuffer[TEX_BFR_QRT_FRONT_LIGHT], (f32)_pixelBlurSize, lastPass );

                pass++;
            }
        }
        else
        {
            copyTextureToEDRAM(m_currentAFXBlurTexture, m_AdditionnalRenderingBuffer[currentDst]);
            resolve( m_AdditionnalRenderingBuffer[currentDst]);
        }
    }
    m_currentAFXBlurTexture = getBufferTexture(currentDst);
#else
    BlurRenderBuffer(_rdrCtxt, _pixelBlurSize, _quality,
        m_currentAFXBlurTexture,
        getBufferTexture(TEX_BFR_QRT_AFX_1),
        getBufferTexture(TEX_BFR_QRT_AFX_2) );

    m_currentAFXBlurTexture = getBufferTexture(TEX_BFR_QRT_AFX_1);
#endif  
}

void GFXAdapter_Directx9::AFXpostProcessPass(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess)
{
    bbool needRestore = !_postProcess.m_colorSetting.m_use;
    setBackBuffer(bfalse, needRestore);
    
    u32 W = getScreenWidth();
    u32 H = getScreenHeight();

    PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

    setGfxMatDefault(drawCallCtx);

	PS_PostEffect psConsts;
	ITF_MemSet(&psConsts, 0, sizeof(psConsts));

//     bbool haveAFXPass = bfalse;
    GFX_BLENDMODE mode = GFX_BLEND_ALPHA;
    shaderDefineKey psDefine = 0;
    if(_postProcess.m_blur.m_use || _postProcess.m_glow.m_use)
    {
        // blurred texture
        bindTexture( 0, m_currentAFXBlurTexture, btrue );
        setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
        psDefine |= mc_define_afterFx_Blur;
   }
    
    if(_postProcess.m_glow.m_use && !_postProcess.m_blur.m_use)
    {
        psDefine |= mc_define_afterFx_Glow;
        bbool bUseToneMap = !(_postProcess.m_glow.m_threshold == 0 && _postProcess.m_glow.m_thresholdScale == 1);
        if(_postProcess.m_glow.m_useTonemap && bUseToneMap)
        {
            psDefine |= mc_define_afterFx_Tonemap;
            float thresholdScale = (f32_Clamp(_postProcess.m_glow.m_thresholdScale, 1.0f, 4.0f) - 1.0f) / 3.0f;
            psConsts.glowParam.set(_postProcess.m_glow.m_threshold, thresholdScale, 0.0f, 0.0f);
        }
        mode = GFX_BLEND_ADDALPHA;
    }

    if(_postProcess.m_refraction.m_use)
    {
        bindTexture( 1, m_currentAFXSceneTexture, btrue );
        bindTexture( 2, getBufferTexture(TEX_BFR_REFRACTION), btrue );
        setTextureAdressingMode(1, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
        setTextureAdressingMode(2, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
        psDefine |= mc_define_afterFx_Refraction;
        mode = GFX_BLEND_COPY;
    }

    if(_postProcess.m_colorSetting.m_use)
    {
		// Factorisation, transform following code :
		// f3_NewColor = lerp( dot(f3_NewColor, float3(0.3f, 0.59f, 0.11f)).xxx, f3_NewColor, Saturation );
		// f3_NewColor = (f3_NewColor + Brightness);
		// f3_NewColor = ( f3_NewColor * ContrasteScale + Contraste );
		// f3_NewColor = ( f3_NewColor * colorCorrection.rgb ) * colorCorrection.a;
		// into
		// float grey = dot(f3_NewColor, float3(0.3f, 0.59f, 0.11f));
		// f3_NewColor = f3_NewColor * ps_colorSetting[0].rgb + grey * ps_colorSetting[1].rgb + ps_colorSetting[2].rgb;
		GFX_Vector4 vColorSetting[3];
		GFX_Vector4 mulVec, addVec;
		f32 red = _postProcess.m_colorSetting.m_colorCorrection.getRed();
		f32 green = _postProcess.m_colorSetting.m_colorCorrection.getGreen();
		f32 blue = _postProcess.m_colorSetting.m_colorCorrection.getBlue();
		f32 alpha = _postProcess.m_colorSetting.m_colorCorrection.getAlpha();
		f32 contrastScale = _postProcess.m_colorSetting.m_contrastScale;
		f32 contrast = _postProcess.m_colorSetting.m_contrast;
		f32 bright = _postProcess.m_colorSetting.m_bright;
		f32 saturation = _postProcess.m_colorSetting.m_saturation;
		f32 contrastBrightScale = contrast + bright * contrastScale;
		mulVec.m_x = red * alpha * contrastScale;
		mulVec.m_y = green * alpha * contrastScale;
		mulVec.m_z = blue * alpha * contrastScale;
		addVec.m_x = contrastBrightScale * red * alpha;
		addVec.m_y = contrastBrightScale * green * alpha;
		addVec.m_z = contrastBrightScale * blue * alpha;
		psConsts.colorSetting[0].m_x = saturation * mulVec.m_x;
		psConsts.colorSetting[0].m_y = saturation * mulVec.m_y;
		psConsts.colorSetting[0].m_z = saturation * mulVec.m_z;
		psConsts.colorSetting[0].m_w = 0.0f;
		psConsts.colorSetting[1].m_x = (1.0f-saturation) * mulVec.m_x;
		psConsts.colorSetting[1].m_y = (1.0f-saturation) * mulVec.m_y;
		psConsts.colorSetting[1].m_z = (1.0f-saturation) * mulVec.m_z;
		psConsts.colorSetting[1].m_w = 0.0f;
		psConsts.colorSetting[2].m_x = addVec.m_x;
		psConsts.colorSetting[2].m_y = addVec.m_y;
		psConsts.colorSetting[2].m_z = addVec.m_z;
		psConsts.colorSetting[2].m_w = 0.0f;

		if( (_postProcess.m_colorSetting.m_saturation != 1.0f) || (_postProcess.m_colorSetting.m_bright != 0.0f) ||
			(_postProcess.m_colorSetting.m_contrast != 0.0f || _postProcess.m_colorSetting.m_contrastScale != 1.0f) ||
			(_postProcess.m_colorSetting.m_colorCorrection != Color::white()) )
		{
			psDefine |= mc_define_afterFx_ColorCorrection;
		}

		// normal scene before AFX
		bindTexture( 1, m_currentAFXSceneTexture );

        mode = GFX_BLEND_COPY;
    }

    if(_postProcess.m_mosaic.m_use)
    {
        bindTexture( 1, m_currentAFXSceneTexture, btrue );
        setTextureAdressingMode(1, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);

        const f32 ratio = (f32)getScreenHeight() / (f32)getScreenWidth();
        f32 pixSize = f32_Lerp(1.0f, _postProcess.m_mosaic.m_pixelSize, _postProcess.m_mosaic.m_fade);
        f32 pixShaderSize = pixSize / getScreenWidth();
        psConsts.mosaic.set(pixShaderSize, 1.0f/pixShaderSize, (pixSize * 0.5f) / getScreenWidth(), ratio);

        psDefine |= mc_define_afterFx_Mosaic;

        mode = GFX_BLEND_COPY;
    }

    if(_postProcess.m_eyeFish.m_use)
    {
        bindTexture( 1, m_currentAFXSceneTexture, btrue );
        setTextureAdressingMode(1, GFX_TEXADRESSMODE_MIRROR, GFX_TEXADRESSMODE_MIRROR);
        setTextureAdressingMode(0, GFX_TEXADRESSMODE_MIRROR, GFX_TEXADRESSMODE_MIRROR); //if effect is merged with blur

        psConsts.eyeFish.set(_postProcess.m_eyeFish.m_height, _postProcess.m_eyeFish.m_fade, _postProcess.m_eyeFish.m_scale, 0.0f);

        psDefine |= mc_define_afterFx_EyeFish;

        mode = GFX_BLEND_COPY;
    }

    if(_postProcess.m_mirror.m_use)
    {
        bindTexture( 1, m_currentAFXSceneTexture, btrue );
        setTextureAdressingMode(1, GFX_TEXADRESSMODE_MIRROR, GFX_TEXADRESSMODE_MIRROR);

        bbool isInvertX = (_postProcess.m_mirror.m_offsetX < 0.0f);
        bbool isInvertY = (_postProcess.m_mirror.m_offsetY < 0.0f);
        const f32 signX = isInvertX?1.0f:-1.0f;
        const f32 signY = isInvertY?1.0f:-1.0f;
        f32 offsetX = f32_Abs(_postProcess.m_mirror.m_fade * _postProcess.m_mirror.m_offsetX);
        f32 offsetY = f32_Abs(_postProcess.m_mirror.m_fade * _postProcess.m_mirror.m_offsetY);
        if(!isInvertX)
            offsetX -= 1.0f;
        if(!isInvertY)
            offsetY -= 1.0f;

        psConsts.mirror.set(offsetX, offsetY, signX, signY);
	
        psDefine |= mc_define_afterFx_Mirror;

        mode = GFX_BLEND_COPY;
    }

    if(_postProcess.m_tile.m_use)
    {
        bindTexture( 1, m_currentAFXSceneTexture, btrue );
        setTextureAdressingMode(1, GFX_TEXADRESSMODE_WRAP, GFX_TEXADRESSMODE_WRAP);

        psConsts.tile.set(f32_Lerp(1.0f, 1.0f/_postProcess.m_tile.m_repetition, _postProcess.m_tile.m_fade), 0.0f, 0.0f, 0.0f);

        psDefine |= mc_define_afterFx_Tile;

        mode = GFX_BLEND_COPY;
    }

    if(_postProcess.m_negatif.m_use)
    {
        bindTexture( 1, m_currentAFXSceneTexture, btrue );
        psDefine |= mc_define_afterFx_Negatif;
        mode = GFX_BLEND_COPY;
    }

    if(_postProcess.m_oldTV.m_use)
    {
        //compute ScanLine offset (between 0-1)
        f64 time = SYSTEM_ADAPTER->getTime();
        if( time - m_AFXTimer > (f64)_postProcess.m_oldTV.m_scanLineSpeed )
        {
            m_AFXTimer = time;
        }
        f32 offsetTime = 1.0f - ((f32)(time - m_AFXTimer) / _postProcess.m_oldTV.m_scanLineSpeed);
        
        bindTexture( 1, m_currentAFXSceneTexture, btrue, GFX_TEXADRESSMODE_WRAP, GFX_TEXADRESSMODE_WRAP );
        Texture * oldTvTex = _postProcess.m_oldTV.getTexture();
        if(oldTvTex && oldTvTex->m_adapterimplementationData)
        {
            bindTexture( 3, oldTvTex);
            setTextureAdressingMode(3, oldTvTex->getUVAddressModeX(), oldTvTex->getUVAddressModeY());
        }

        psConsts.oldTV.set( _postProcess.m_oldTV.m_lineFade * _postProcess.m_oldTV.m_fade,
							_postProcess.m_oldTV.m_scanLineFade * _postProcess.m_oldTV.m_fade,
							_postProcess.m_oldTV.m_scanLineSize, offsetTime);  //1 - abs((1-uv.y) * S) - S + offset

        psDefine |= mc_define_afterFx_OldTV;
        mode = GFX_BLEND_COPY;
    }

    if(_postProcess.m_noise.m_use)
    {
        //compute ScanLine offset (between 0-1)
        f64 time = SYSTEM_ADAPTER->getTime();
        f32 offsetTime = 1.0f - (f32)fmod(time, (f64)1.0f);
        f32 sizeParam = _postProcess.m_noise.m_size / 10.0f;

        bindTexture( 1, m_currentAFXSceneTexture, btrue );
        psConsts.noise.set( offsetTime, _postProcess.m_noise.m_blend, sizeParam, 1.0f/sizeParam);
        psDefine |= mc_define_afterFx_Noise;
        mode = GFX_BLEND_COPY;
    }

    if(_postProcess.m_radial.m_use)
    {
        bindTexture( 1, m_currentAFXSceneTexture, btrue );

        const f32 fSampleStepSize = _postProcess.m_radial.m_size / getScreenWidth() / 6.0f * cRadialBlurScaleSize * _postProcess.m_radial.m_fade;
        const f32 fPower = (f32)_postProcess.m_radial.m_fade * _postProcess.m_radial.m_strength;
        psConsts.radial.set( _postProcess.m_radial.m_centerOffset.x(), _postProcess.m_radial.m_centerOffset.y(), fSampleStepSize, fPower);
        psDefine |= mc_define_afterFx_Radial;
        mode = GFX_BLEND_ALPHA;
    }

    if(psDefine != 0)
    {
        //check if need to blend value in shader
        if(isBlendingValue(_postProcess))
            psDefine |= mc_define_afterFx_BlendValue;

        setShaderGroup( mc_shader_AfterFx, mc_entry_afterFx_VS_PCT, mc_entry_afterFx_PS_mergedEffect, 0, psDefine);

        psConsts.AFXParam.set(_postProcess.m_glow.m_factor, _postProcess.m_glow.m_fade, _postProcess.m_blur.m_fade, _postProcess.m_colorSetting.m_fade);
        psConsts.AFXParam2.set(_postProcess.m_refraction.m_fade / 255.0f, _postProcess.m_negatif.m_fade, _postProcess.m_oldTV.m_fade, _postProcess.m_noise.m_fade);

        SetPixelShaderConstantF(PS_Attrib_postEffect, (f32*)&psConsts, sizeof(psConsts)/16);

        drawCallCtx.getStateCache().setAlphaBlend(mode);

        drawScreenQuad(drawCallCtx, 0.f, 0.f, (f32)W, (f32)H, 0.f, COLOR_WHITE);
    }

#ifndef ITF_FINAL
    if(psDefine)
    {
		m_dbgTxtAfterFX = "";
        if(psDefine & mc_define_afterFx_Blur)   m_dbgTxtAfterFX += "Blur ";
        if(psDefine & mc_define_afterFx_ColorCorrection) m_dbgTxtAfterFX += "ColCorrec ";
        if(psDefine & mc_define_afterFx_EyeFish) m_dbgTxtAfterFX += "EyeF ";
        if(psDefine & mc_define_afterFx_EdgeDetection) m_dbgTxtAfterFX += "EdgeD ";
        if(psDefine & mc_define_afterFx_Gauss)   m_dbgTxtAfterFX += "Gauss ";
        if(psDefine & mc_define_afterFx_Glow)    m_dbgTxtAfterFX += "Glow ";
        if(psDefine & mc_define_afterFx_Mirror)  m_dbgTxtAfterFX += "Mirr ";
        if(psDefine & mc_define_afterFx_Negatif) m_dbgTxtAfterFX += "Neg ";
        if(psDefine & mc_define_afterFx_Mosaic)  m_dbgTxtAfterFX += "Mos ";
        if(psDefine & mc_define_afterFx_Refraction) m_dbgTxtAfterFX += "Refr ";
        if(psDefine & mc_define_afterFx_Saturation) m_dbgTxtAfterFX += "Sat ";
        if(psDefine & mc_define_afterFx_Tile)       m_dbgTxtAfterFX += "Tile ";
        if(psDefine & mc_define_afterFx_Tonemap)    m_dbgTxtAfterFX += "Tonemap ";
    }
#endif // ITF_FINAL

}

void GFXAdapter_Directx9::fillHexagoneVertexBuffer(VertexPCT* _data, u32 _startIndex, Vec2d _position, f32 _hexagoneSize, const Vec2d& _uv1, const Vec2d& _uv2, const Vec2d& _uv3 )
{

    //      A  B
    //    F   .  C
    //      E  D
    f32 screenSize = f32_Min(static_cast<f32>(getScreenWidth()), static_cast<f32>(getScreenHeight()));
    const f32 rayon = screenSize * _hexagoneSize * 0.5f;
    const f32 apotheme = 0.8660254f * rayon;
    Vec3d offsetPos = _position.to3d();
    u32 color = 0xFFFFFFFF;

    u32 index = _startIndex;
    //cAB
    _data[index++].setData(offsetPos + Vec3d(0.0f, 0.0f, 0.0f), Vec2d(_uv1.x(), _uv1.y()), color);
    _data[index++].setData(offsetPos + Vec3d(-rayon/2.0f, -apotheme, 0.0f), Vec2d(_uv2.x(), _uv2.y()), color);
    _data[index++].setData(offsetPos + Vec3d(rayon/2.0f, -apotheme, 0.0f), Vec2d(_uv3.x(), _uv3.y()), color);
    //cBC
    _data[index++].setData(offsetPos + Vec3d(0.0f, 0.0f, 0.0f), Vec2d(_uv1.x(), _uv1.y()), color);
    _data[index++].setData(offsetPos + Vec3d(rayon/2.0f, -apotheme, 0.0f), Vec2d(_uv3.x(), _uv3.y()), color);
    _data[index++].setData(offsetPos + Vec3d(rayon, 0.0f, 0.0f), Vec2d(_uv2.x(), _uv2.y()), color);
    //cCD
    _data[index++].setData(offsetPos + Vec3d(0.0f, 0.0f, 0.0f), Vec2d(_uv1.x(), _uv1.y()), color);
    _data[index++].setData(offsetPos + Vec3d(rayon, 0.0f, 0.0f), Vec2d(_uv2.x(), _uv2.y()), color);
    _data[index++].setData(offsetPos + Vec3d(rayon/2.0f, apotheme, 0.0f), Vec2d(_uv3.x(), _uv3.y()), color);
    //cDE
    _data[index++].setData(offsetPos + Vec3d(0.0f, 0.0f, 0.0f), Vec2d(_uv1.x(), _uv1.y()), color);
    _data[index++].setData(offsetPos + Vec3d(rayon/2.0f, apotheme, 0.0f), Vec2d(_uv3.x(), _uv3.y()), color);
    _data[index++].setData(offsetPos + Vec3d(-rayon/2.0f, apotheme, 0.0f), Vec2d(_uv2.x(), _uv2.y()), color);
    //cEF
    _data[index++].setData(offsetPos + Vec3d(0.0f, 0.0f, 0.0f), Vec2d(_uv1.x(), _uv1.y()), color);
    _data[index++].setData(offsetPos + Vec3d(-rayon/2.0f, apotheme, 0.0f), Vec2d(_uv2.x(), _uv2.y()), color);
    _data[index++].setData(offsetPos + Vec3d(-rayon, 0.0f, 0.0f), Vec2d(_uv3.x(), _uv3.y()), color);
    //cEF
    _data[index++].setData(offsetPos + Vec3d(0.0f, 0.0f, 0.0f), Vec2d(_uv1.x(), _uv1.y()), color);
    _data[index++].setData(offsetPos + Vec3d(-rayon, 0.0f, 0.0f), Vec2d(_uv3.x(), _uv3.y()), color);
    _data[index++].setData(offsetPos + Vec3d(-rayon/2.0f, -apotheme, 0.0f), Vec2d(_uv2.x(), _uv2.y()), color);

}

void GFXAdapter_Directx9::AFXPostProcessKaleiPass(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess)
{
    setBackBuffer(bfalse, bfalse);

    u32 W = getScreenWidth();
    u32 H = getScreenHeight();

    PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

    setGfxMatDefault(drawCallCtx);
    setVertexFormat(VertexFormat_PCT);

    drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY);

//     shaderDefineKey psDefine = 0;
    setShaderGroup( mc_shader_AfterFx, mc_entry_afterFx_VS_PCT, mc_entry_afterFx_PS_Kalei, 0, 0);

    bindTexture( 1, m_currentAFXSceneTexture, btrue );
    setTextureAdressingMode(1, GFX_TEXADRESSMODE_MIRROR, GFX_TEXADRESSMODE_MIRROR);

    Matrix44 World;
    World.setIdentity();
    setObjectMatrix(World);

    // fill vertex buffer.
    const Vec2d p1UV = _postProcess.m_kalei.m_uv1;
    const Vec2d p2UV = _postProcess.m_kalei.m_uv2;
    const Vec2d p3UV = _postProcess.m_kalei.m_uv3;

    static const u32 maxArraySize = 1024;
    VertexPCT v[maxArraySize];
    Vec2d offsetPos = Vec2d(W * 0.5f, H * 0.5f);
    f32 fade = _postProcess.m_kalei.m_fade;
    f32 hexagoneSize = _postProcess.m_kalei.m_size;
    f32 screenSize = f32_Min(static_cast<f32>(getScreenWidth()), static_cast<f32>(getScreenHeight()));
    const f32 rayon = screenSize * hexagoneSize * 0.5f;
    const f32 apotheme = 0.8660254f * rayon;

    u32 index = 0;
    const i32 nbHexagoneX = (static_cast<i32>(W / (1.5f * rayon))+1) / 2;
    const i32 nbHexagoneY = (static_cast<i32>(H / apotheme)+1) / 2;
    bbool pair = btrue;
    for(i32 i = -nbHexagoneX; i<=nbHexagoneX; i++)
    {
        f32 delta = 0.0f;
        if(pair)
            delta = apotheme;
        for(i32 j = -nbHexagoneY; j<=nbHexagoneY; j++)
        {
            if(index + 18 >= maxArraySize)
                return;
            fillHexagoneVertexBuffer(v, index, offsetPos + Vec2d(i * rayon * 1.5f, j * apotheme*2.0f + delta), hexagoneSize * fade, p1UV, p2UV, p3UV );
            index += 18;
        }
        pair = !pair;
    }
    
    DrawPrimitive(drawCallCtx, GFX_TRIANGLES, (void*)v, index);
}

void GFXAdapter_Directx9::blurFrontLightRenderBuffer(RenderPassContext & _rdrCtxt, u32 _quality, u32 _size)
{
    u32 srcBuf = (_quality%2==0)?TEX_BFR_QRT_FRONT_LIGHT:TEX_BFR_QRT_TMP_LIGHT;
    u32 dstBuf = TEX_BFR_QRT_FRONT_LIGHT;
    u32 tmpBuf = TEX_BFR_QRT_TMP_LIGHT;

    BlurRenderBuffer(_rdrCtxt, _size, _quality, getBufferTexture(srcBuf), getBufferTexture(dstBuf), getBufferTexture(tmpBuf) );
}

void GFXAdapter_Directx9::blurBackLightRenderBuffer(RenderPassContext & _rdrCtxt, u32 _quality, u32 _size)
{
    u32 srcBuf = (_quality%2==0)?TEX_BFR_QRT_BACK_LIGHT:TEX_BFR_QRT_TMP_LIGHT;
    u32 dstBuf = TEX_BFR_QRT_BACK_LIGHT;
    u32 tmpBuf = TEX_BFR_QRT_TMP_LIGHT;

    BlurRenderBuffer(_rdrCtxt, _size, _quality, getBufferTexture(srcBuf), getBufferTexture(dstBuf), getBufferTexture(tmpBuf) );
}

void GFXAdapter_Directx9::restoreBackLightRenderBuffer(RenderPassContext & _rdrCtxt, eLightTexBuffer _currentLightBufferType)
{
    // need to restore backLight only if previous and next target are different.
    bbool bRestoreFullLight = (_currentLightBufferType != LIGHT_BFR_FULL);

    RenderTarget* srcSurf = bRestoreFullLight?getBufferTexture(TEX_BFR_QRT_BACK_LIGHT):getBufferTexture(TEX_BFR_BACK_LIGHT);
    RenderTarget* dstSurf = bRestoreFullLight?getBufferTexture(TEX_BFR_BACK_LIGHT):getBufferTexture(TEX_BFR_QRT_BACK_LIGHT);
	copyTexture(_rdrCtxt, srcSurf, dstSurf);
}

// Draw blur Scene in Additionnal RenderTarget 0
void GFXAdapter_Directx9::blurFluidRenderBuffer(RenderPassContext & _rdrCtxt)
{
    GFX_ZLIST_PASS_TYPE oldPassType = getCurPassType();
    m_curPassType = GFX_ZLIST_PASS_REGULAR;

    PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
    DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

    u32 W = getScreenWidth();
    u32 H = getScreenHeight();

    const f32 ratiopixelX = 1.f/(f32)W;
    const f32 ratiopixelY = 1.f/(f32)H;
    f32 displaceX = 8.0f * ratiopixelX;
    f32 displaceY = 8.0f * ratiopixelY;

    setGfxMatDefault(drawCallCtx);
    setShaderGroup(mc_shader_AfterFx, mc_entry_afterFx_VS_blur, mc_entry_afterFx_PS_bigBlur, 0, mc_define_afterFx_Gauss);
    
    drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
        .setColorWrite(btrue)
        .setAlphaWrite(btrue);

    //First Blur for fluid
    RenderTarget* texDest = getBufferTexture(TEX_BFR_FLUID_BLUR);
    RenderTarget* texSource = getBufferTexture(TEX_BFR_FLUID_GLOW);

    bindTexture(0, texSource, btrue);

	RenderContext rtContext(texDest);
	setRenderContext(rtContext);

    /// shader const.
    GFX_Vector4 vconst1(displaceX, displaceY, displaceX, displaceY);
    SetVertexShaderConstantF(VS_Attrib_afx, (f32*)&vconst1, 1);

    drawScreenQuad(drawCallCtx, 0.f, 0.f, (f32)W, (f32)H, 0.f, COLOR_WHITE, bfalse);
    resolve( getBufferTexture(TEX_BFR_FLUID_BLUR) );

    //Second blur in small Buffer for possible glow effect
    if(m_fluidSimuInfo.m_useGlow)
    {
        float displaceSize = m_fluidSimuInfo.m_glowSize;

        displaceX = displaceSize * ratiopixelX;
        displaceY = displaceSize * ratiopixelY;

        texDest = getBufferTexture(TEX_BFR_FLUID_GLOW);
        texSource = getBufferTexture(TEX_BFR_FLUID_BLUR);

		bindTexture(0, texSource, btrue);

		RenderContext rtContext(texDest);
		setRenderContext(rtContext);

        /// shader const.
        vconst1 = GFX_Vector4(displaceX, displaceY, displaceX, displaceY);
        SetVertexShaderConstantF(VS_Attrib_afx, (f32*)&vconst1, 1);

        setShaderGroup(mc_shader_AfterFx, mc_entry_afterFx_VS_blur, mc_entry_afterFx_PS_bigBlur, 0, mc_define_afterFx_Gauss);

        drawScreenQuad(drawCallCtx, 0.f, 0.f, (f32)W, (f32)H, 0.f, COLOR_WHITE, bfalse);
        resolve( texDest );
    }

    bindTexture(0, NULL);
    m_curPassType = oldPassType;
}

#endif

// Get (can copy) scene texture
RenderTarget* GFXAdapter_Directx9::getSceneTexture(u32 _rt, f32 _scale, bbool _use2x2Target)
{
    bindTexture(0, NULL);
    bindTexture(1, NULL);
    bindTexture(2, NULL);

    //Prepare First Source
    RenderTarget* sceneTex;

#ifndef ITF_X360
    if(_use2x2Target)
    {
        sceneTex = getBufferTexture(TEX_BFR_QRT_AFX_1 + _rt);

        RECT rectDest;
        rectDest.left   = 0;
        rectDest.right  = static_cast<u32>(_scale * sceneTex->getSizeX());
        rectDest.top    = 0;
        rectDest.bottom = static_cast<u32>(_scale * sceneTex->getSizeY());

        HRESULT hr = m_pd3dDevice->StretchRect( m_currentRenderingSurface, NULL, getTargetDX(sceneTex)->getSurface(), &rectDest, D3DTEXF_LINEAR);
        ITF_VERIFY(D3D_OK == hr);
    }
    else
    {
        sceneTex = getBufferTexture(TEX_BFR_FULL_1 + _rt);
        RECT rectDest;
        rectDest.left   = 0;
        rectDest.right  = static_cast<u32>(_scale * sceneTex->getSizeX());
        rectDest.top    = 0;
        rectDest.bottom = static_cast<u32>(_scale * sceneTex->getSizeY());

        HRESULT hr = m_pd3dDevice->StretchRect( m_currentRenderingSurface, NULL, getTargetDX(sceneTex)->getSurface(), &rectDest, D3DTEXF_LINEAR);
        ITF_VERIFY(D3D_OK == hr);
    }
#else
    if(_scale == 1.0f)
    {
        sceneTex = m_backBufferNoTile;
    }
    else
    {
        RenderPassContext rdrCtx;
        PrimitiveContext primCtx = PrimitiveContext(&rdrCtx);
        DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

        u32 W = getScreenWidth();
        u32 H = getScreenHeight();

        getWorldViewProjMatrix()->push();
        setOrthoView(0.f, (f32)W, 0.f, (f32)H);

        setShaderGroup(m_defaultShaderGroup);
        setGfxMatDefault(drawCallCtx);

        drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
            .setDepthTest(bfalse);

        RenderTarget* texDest = getBufferTexture(TEX_BFR_FULL_1);
        RenderTarget* texSource = m_backBufferNoTile;

		RenderContext rtContext(texDest);
		setRenderContext(rtContext);

        bindTexture(0, texSource);
        setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);

        drawScreenQuad(drawCallCtx, 0.f, (f32)H * (1.0f-_scale), (f32)W * _scale, (f32)H * _scale, 0.0f, COLOR_WHITE);
        resolve(texDest);
        sceneTex = getBufferTexture(TEX_BFR_FULL_1);

        getWorldViewProjMatrix()->pop();
    }
#endif

    return sceneTex;
}

} // namespace ITF