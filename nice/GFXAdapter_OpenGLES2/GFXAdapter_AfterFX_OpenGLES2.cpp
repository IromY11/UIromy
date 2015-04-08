#include "precompiled_GFXAdapter_OpenGLES2.h"

#ifndef _ITF_OPENGLES2_DRAWPRIM_H_
#include "adapters/GFXAdapter_OpenGLES2/GFXAdapter_OpenGLES2.h"
#endif //_ITF_OPENGLES2_DRAWPRIM_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_



namespace ITF
{
	void GFXAdapter_OpenGLES2::CopySurface(RenderPassContext & _rdrCtxt, TextureBuffer* _dstTex, TextureBuffer* _srcTex, GFX_BLENDMODE _blend, bbool _invalidateDst, f32 _scale)
	{
		/*if((_blend == GFX_BLEND_COPY) && (_scale == 1.0f))
		{
			if(    (_srcTex->getWidth()  == _dstTex->getWidth() )
				&& (_srcTex->getHeight() == _dstTex->getHeight())
				//DEAC && (_srcTex->getFormat() == _dstTex->getFormat())
				)
			{
				_srcTex->Invalidate();
				//GX2_CopySurface(&_srcTex->getColorBuffer()->surface, 0, 0, &_dstTex->getColorBuffer()->surface, 0, 0);
			}
			else
			{
				//GX2UT_CopySurface(&_srcTex->getColorBuffer()->surface, 0, 0, &_dstTex->getColorBuffer()->surface, 0, 0);
			}
		}
		else*/
		{
			PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
			DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

			f32 Width, Height;
			
			if (!_dstTex)
			{
#ifdef ITF_IOS 
                // get Objective C layer real size, do NOT guess
				Width  = (f32)SYSTEM_ADAPTER->getDeviceInfo().getFrameBufferWidth(); 
				Height = (f32)SYSTEM_ADAPTER->getDeviceInfo().getFrameBufferHeight();
#else
				Width  = (f32)m_windowWidth;
				Height = (f32)m_windowHeight;
#endif
			}
			else
			{
				Width  = (f32)_dstTex->getWidth();
				Height = (f32)_dstTex->getHeight();
			}

			const f32 WidthScaled  = Width * _scale;
			const f32 HeightScaled = Height * _scale;


			setRenderBuffers(_dstTex, NULL/*getRenderDepthBuffer()*/);
			setShaderGroup(m_defaultShaderGroup);
			setDefaultGFXPrimitiveParam();
			setGfxMatDefault(drawCallCtx);

			getWorldViewProjMatrix()->push();
			setOrthoView(0.f, Width, 0.f, Height); // Width and Height of the DEST (not str)
			setupViewport(0, 0, (i32)WidthScaled, (i32)HeightScaled);
            
			_srcTex->Invalidate();
			SetTextureBind(0, _srcTex->getTextureSamp());
            setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
            
			drawCallCtx.getStateCache().setAlphaBlend(_blend).setDepthTest(bfalse);

            colorMask(GFX_COL_MASK_ALL);
			//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			drawScreenQuad(drawCallCtx, 0.f, 0.f, Width, Height, 0.f, COLOR_WHITE);

			// restore camera
			View::getCurrentView()->apply();
			getWorldViewProjMatrix()->pop();
		}

		if(_invalidateDst)
			_dstTex->Invalidate();
	}

#ifndef ITF_FINAL
	void GFXAdapter_OpenGLES2::copyDebugRenderTarget( f32 _alpha, const Vec2d & _pixelOffset)
	{
#if 0 //DEAC
		if (!m_currentTextureBuffer[TEX_BFR_DBG_COPY])
			return;

		ITF_ASSERT(m_currentTextureBuffer[TEX_BFR_MAIN]);
		if(m_currentTextureBuffer[TEX_BFR_MAIN])
		{
			RenderPassContext rdrCtxt;
			rdrCtxt.getStateCache().setDepthTest(bfalse);
			SetDrawCallState(rdrCtxt.getStateCache(), 0xFFffFFff, btrue);
			CopySurface(rdrCtxt, m_currentTextureBuffer[TEX_BFR_MAIN], m_currentTextureBuffer[TEX_BFR_DBG_COPY], GFX_BLEND_COPY, bfalse, 1.f);
		}
#endif //DEAC
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::copyPassToDebugRenderTarget(GFX_ZLIST_PASS_TYPE _passType)
	{
#if 0 //DEAC
		if (!m_currentTextureBuffer[TEX_BFR_DBG_COPY])
			return;

		TextureBuffer *src = NULL;
		switch(_passType)
		{
		case GFX_ZLIST_PASS_FRONT_LIGHT:
			if(m_currentFrontLightBufferType == LIGHT_BFR_FULL)
				src =  m_currentTextureBuffer[TEX_BFR_FRONT_LIGHT];
			else
				src =  m_currentTextureBuffer[TEX_BFR_QRT_FRONT_LIGHT];
			break;
		case GFX_ZLIST_PASS_BACK_LIGHT:
			if(m_currentBackLightBufferType == LIGHT_BFR_FULL)
				src =  m_currentTextureBuffer[TEX_BFR_BACK_LIGHT];
			else
				src =  m_currentTextureBuffer[TEX_BFR_QRT_BACK_LIGHT];
			break;
		case GFX_ZLIST_PASS_REFLECTION:
			src = m_currentTextureBuffer[TEX_BFR_REFLECTION];
			break;
		case GFX_ZLIST_PASS_FLUID:
			src = m_currentTextureBuffer[TEX_BFR_FLUID_2];
			break;
		case GFX_ZLIST_PASS_REGULAR:
			src = m_currentTextureBuffer[TEX_BFR_MAIN];
			break;
		}

		ITF_ASSERT(src);
		if(src)
		{
			RenderPassContext rdrCtxt;
			rdrCtxt.getStateCache().setDepthTest(bfalse);
			SetDrawCallState(rdrCtxt.getStateCache(), 0xFFffFFff, btrue);
			CopySurface(rdrCtxt, m_currentTextureBuffer[TEX_BFR_DBG_COPY], src, GFX_BLEND_COPY, btrue, 1.f);
		}
#endif //DEAC
	}
#endif // ITF_FINAL

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::DrawAFXPostProcess(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess)
	{
		if(!_postProcess.isActived() || !isUsingPostEffects())
			return;
        
        if( m_direct2BackBuffer )
            return; // speed things up
        
		AFXPostProcessProcessAllowCombinatory(_postProcess);

		m_copyCurrentTextureBuffer = NULL;
		m_aftexFXTextureBlur       = NULL;

		f32 copyScale = _postProcess.m_tile.m_use?f32_Lerp(1.f, 1.f/_postProcess.m_tile.m_repetition, _postProcess.m_tile.m_fade):1.f;
		u32 blurQuality = _postProcess.m_blur.m_use?_postProcess.m_blur.m_quality: _postProcess.m_glow.m_quality;
		bbool b_needBlur = _postProcess.m_blur.m_use || _postProcess.m_glow.m_use;
        
		if(b_needBlur)
		{
			m_aftexFXTextureBlur = m_currentTextureBuffer[TEX_BFR_FIRST_QS+(blurQuality%2)];
			CopySurface(_rdrCtxt, m_aftexFXTextureBlur, m_currentTextureBuffer[TEX_BFR_MAIN], GFX_BLEND_COPY, bfalse, copyScale);
		}

       
		// Prepare copy if needed
		if(    _postProcess.m_colorSetting.m_use
			|| _postProcess.m_refraction.m_use
			|| _postProcess.m_tile.m_use
			|| _postProcess.m_mosaic.m_use
			|| _postProcess.m_negatif.m_use
			|| _postProcess.m_kalei.m_use
			|| _postProcess.m_eyeFish.m_use
			|| _postProcess.m_mirror.m_use
			|| _postProcess.m_oldTV.m_use
			|| _postProcess.m_noise.m_use)
		{
            
             // Looks broken now... don't really understand why
             // Jira UAF-22659
             // for a  flare effect, it uses the wrong texture output
             // before : the same working effect was using glow and blur. Now only colorEffect. Could be related
             // optim disabled then
			if((copyScale == 1.f) && !_postProcess.m_kalei.m_use && (_postProcess.m_blur.m_use ||  _postProcess.m_glow.m_use ) )
			{
				// Switch texture buffer to avoid CopySurface
				TextureBuffer* tmp = m_currentTextureBuffer[TEX_BFR_FIRST_FS];
				m_currentTextureBuffer[TEX_BFR_FIRST_FS] = m_currentTextureBuffer[TEX_BFR_MAIN];
				m_currentTextureBuffer[TEX_BFR_MAIN] = tmp;
			}
			else
            
			{
				CopySurface(_rdrCtxt, m_currentTextureBuffer[TEX_BFR_FIRST_FS], m_currentTextureBuffer[TEX_BFR_MAIN], GFX_BLEND_COPY, bfalse, copyScale);
			}
			m_copyCurrentTextureBuffer = m_currentTextureBuffer[TEX_BFR_FIRST_FS];
			m_copyCurrentTextureBuffer->Invalidate();
		}

		BeginAFXPostProcess(btrue);

		if(b_needBlur)
		{
			const f32 screenFactor = (f32)getScreenHeight() / (f32)getWindowHeight();
			// Draw blurred scene in additional renderTarget 0
			if(_postProcess.m_blur.m_use)
				PrepareBlurTarget(_rdrCtxt, f32(_postProcess.m_blur.m_pixelSize * screenFactor), _postProcess.m_blur.m_quality);
			else
				PrepareBlurTarget(_rdrCtxt, f32(_postProcess.m_glow.m_pixelSize * screenFactor), _postProcess.m_glow.m_quality);
		}

		AFXpostProcessPass(_rdrCtxt, _postProcess);

		if(_postProcess.m_kalei.m_use)
		{
			AFXPostProcessKaleiPass(_rdrCtxt, _postProcess);
		}

		EndAFXPostProcess(btrue, GFX_BLEND_ADDALPHA);
	}

	// ------------------------------------------------------------------------------
	// Draw blur Scene in Additional RenderTarget 0
	// ------------------------------------------------------------------------------
	void GFXAdapter_OpenGLES2::PrepareBlurTarget(RenderPassContext & _rdrCtxt, f32 _pixelBlurSize, u32 _quality)
	{
		ITF_ASSERT(m_aftexFXTextureBlur);
		ITF_ASSERT(MAX_AFTER_FX_QUART >= 2);

		//DEAC GFX_METRICS_START_TAG(GFX_MAIN_PASS_AFX_BLUR);

		PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
		DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

		u32 dstId = (_quality+1)%2;
		u32 srcId = _quality%2;

		TextureBuffer* texDst = m_currentTextureBuffer[TEX_BFR_FIRST_QS+dstId];
		TextureBuffer* texSrc = m_aftexFXTextureBlur;

		const f32 W = (f32)texDst->getWidth();
		const f32 H = (f32)texDst->getHeight();

		getWorldViewProjMatrix()->push();
		setOrthoView(0.f, W, 0.f, H);
		setupViewport(0, 0, (i32)W, (i32)H);

		setShaderGroup(mc_shader_AfterFx, mc_entry_afterFx_VS_blur, mc_entry_afterFx_PS_bigBlur);
		setDefaultGFXPrimitiveParam();
		setGfxMatDefault(drawCallCtx);

		drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
			.setDepthTest(bfalse);

		const f32 ratiopixelX = 1.f/W;
		const f32 ratiopixelY = 1.f/H;

		f32 displaceX = (_pixelBlurSize / 2.0f) * ratiopixelX; //4 pixel Displace
		f32 displaceY = (_pixelBlurSize / 2.0f) * ratiopixelY;
		const f32 qualityDisplaceX = displaceX / _quality;
		const f32 qualityDisplaceY = displaceY / _quality;

		u32 pass = 0;

		while ((displaceX >= ratiopixelX + MTH_EPSILON) || (pass==0))
		{
			setRenderBuffers(texDst, getRenderDepthBuffer());
			m_aftexFXTextureBlur = texDst;

			texSrc->Invalidate();
			SetTextureBind(0, texSrc->getTextureSamp());
			setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);

			/// shader const.
			GFX_Vector4 vconst1(displaceX, displaceY, displaceX, displaceY);
			SetVertexShaderConstantF(VS_Attrib_afx, (f32*)&vconst1, 1);

			drawScreenQuadC(drawCallCtx, 0.f, 0.f, W, H, 0.f, COLOR_WHITE, getCentroidOffset(), bfalse);

			//swap target
			dstId = (dstId+1)%2;
			srcId = (srcId+1)%2;
			texDst = m_currentTextureBuffer[TEX_BFR_FIRST_QS+dstId];
			texSrc = m_currentTextureBuffer[TEX_BFR_FIRST_QS+srcId];

			displaceX -= qualityDisplaceX;
			displaceY -= qualityDisplaceY;
			pass++;
			if (_quality && pass >= _quality) 
				break;
		}

		// restore camera
		View::getCurrentView()->apply();
		getWorldViewProjMatrix()->pop();

		//DEAC GFX_METRICS_END_TAG(GFX_MAIN_PASS_AFX_BLUR);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::AFXpostProcessPass(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess)
	{
		//DEAC GFX_METRICS_START_TAG(GFX_MAIN_PASS_AFX_APPLY);

		PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
		DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

		// restore main render buffer.
		setRenderTargetForPass(m_curPassType);
    
		setDefaultGFXPrimitiveParam();
		setGfxMatDefault(drawCallCtx);

		PS_PostEffect psConsts;
		ITF_MemSet(&psConsts, 0, sizeof(psConsts));

		GFX_BLENDMODE mode = GFX_BLEND_ALPHA;
		shaderDefineKey psDefine = 0;

		if(_postProcess.m_blur.m_use || _postProcess.m_glow.m_use)
		{
			// blurred texture
			ITF_ASSERT(m_aftexFXTextureBlur);
			m_aftexFXTextureBlur->Invalidate();
			SetTextureBind(0, m_aftexFXTextureBlur->getTextureSamp(), btrue);
			setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
			psDefine |= mc_define_afterFx_Blur;
		}

		if(_postProcess.m_refraction.m_use || _postProcess.m_colorSetting.m_use)
		{
			// normal scene before AFX
			ITF_ASSERT(m_copyCurrentTextureBuffer);
			SetTextureBind(1, m_copyCurrentTextureBuffer->getTextureSamp());
			setTextureAdressingMode(1, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);
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
			SetTextureBind(2, getRefractionBuffer(), btrue);
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
			SetTextureBind( 1, m_copyCurrentTextureBuffer->getTextureSamp(), btrue );

			mode = GFX_BLEND_COPY;
		}

		if(_postProcess.m_mosaic.m_use)
		{
			SetTextureBind( 1, m_copyCurrentTextureBuffer->getTextureSamp(), btrue );
			setTextureAdressingMode(1, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);

			const f32 factor = (f32)getScreenHeight() / (f32)getWindowHeight();
			const f32 ratio = (f32)getScreenHeight() / (f32)getScreenWidth();
			const f32 pixSize = f32_Lerp(1.0f, _postProcess.m_mosaic.m_pixelSize * factor, _postProcess.m_mosaic.m_fade);
			const f32 pixShaderSize = pixSize / getScreenWidth();

			psConsts.mosaic.set(pixShaderSize, 1.0f/pixShaderSize, (pixSize * 0.5f) / getScreenWidth(), ratio);

			psDefine |= mc_define_afterFx_Mosaic;

			mode = GFX_BLEND_COPY;
		}

		if(_postProcess.m_eyeFish.m_use)
		{
			SetTextureBind( 1, m_copyCurrentTextureBuffer->getTextureSamp(), btrue );
			setTextureAdressingMode(1, GFX_TEXADRESSMODE_MIRROR, GFX_TEXADRESSMODE_MIRROR);
			setTextureAdressingMode(0, GFX_TEXADRESSMODE_MIRROR, GFX_TEXADRESSMODE_MIRROR); //if effect is merged with blur

			psConsts.eyeFish.set(_postProcess.m_eyeFish.m_height, _postProcess.m_eyeFish.m_fade, _postProcess.m_eyeFish.m_scale, 0.0f);

			psDefine |= mc_define_afterFx_EyeFish;

			mode = GFX_BLEND_COPY;
		}

		if(_postProcess.m_mirror.m_use)
		{
			SetTextureBind( 1, m_copyCurrentTextureBuffer->getTextureSamp(), btrue );
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
			SetTextureBind( 1, m_copyCurrentTextureBuffer->getTextureSamp(), btrue );
			setTextureAdressingMode(1, GFX_TEXADRESSMODE_WRAP, GFX_TEXADRESSMODE_WRAP);

			psConsts.tile.set(f32_Lerp(1.0f, 1.0f/_postProcess.m_tile.m_repetition, _postProcess.m_tile.m_fade), 0.0f, 0.0f, 0.0f);

			psDefine |= mc_define_afterFx_Tile;

			mode = GFX_BLEND_COPY;
		}

		if(_postProcess.m_negatif.m_use)
		{
			SetTextureBind( 1, m_copyCurrentTextureBuffer->getTextureSamp(), btrue );
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
			f32 offsetTime = f32(1.0f - ((time - m_AFXTimer) / _postProcess.m_oldTV.m_scanLineSpeed));

			SetTextureBind( 1, m_copyCurrentTextureBuffer->getTextureSamp(), btrue );
			setTextureAdressingMode(1, GFX_TEXADRESSMODE_WRAP, GFX_TEXADRESSMODE_WRAP);
			Texture* oldTVTex = _postProcess.m_oldTV.getTexture();
			if(oldTVTex && oldTVTex->m_adapterimplementationData)
			{
				SetTextureBind( 3, (PlatformTexture*) oldTVTex->m_adapterimplementationData);
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
			SetTextureBind( 1, m_copyCurrentTextureBuffer->getTextureSamp(), btrue );
			psConsts.noise.set( offsetTime, _postProcess.m_noise.m_blend, sizeParam, 1.0f/sizeParam);
			psDefine |= mc_define_afterFx_Noise;
			mode = GFX_BLEND_COPY;
		}

		if(psDefine != 0)
		{
			//check if need to blend value in shader
			if(isBlendingValue(_postProcess))
				psDefine |= mc_define_afterFx_BlendValue;

			setShaderGroup(mc_shader_AfterFx, mc_entry_afterFx_VS_PCT, mc_entry_afterFx_PS_mergedEffect, 0, psDefine);

			psConsts.AFXParam.set(_postProcess.m_glow.m_factor, _postProcess.m_glow.m_fade, _postProcess.m_blur.m_fade, _postProcess.m_colorSetting.m_fade);
			psConsts.AFXParam2.set(_postProcess.m_refraction.m_fade / 255.0f, _postProcess.m_negatif.m_fade, _postProcess.m_oldTV.m_fade, _postProcess.m_noise.m_fade);

			SetPixelShaderConstantF(PS_Attrib_postEffect, (f32*)&psConsts, sizeof(psConsts)/16);

			drawCallCtx.getStateCache().setAlphaBlend(mode)
				.setDepthTest(bfalse);

			drawScreenQuad(drawCallCtx, 0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE);
		}

		#ifndef ITF_FINAL
/*		if(psDefine)
		{
			if(psDefine & mc_define_afterFx_Blur)   strcat(m_dbgTxtAfterFX,"Blur ");
			if(psDefine & mc_define_afterFx_Bright) strcat(m_dbgTxtAfterFX,"Bright ");
			if(psDefine & mc_define_afterFx_ColorCorrection) strcat(m_dbgTxtAfterFX,"ColCorrec ");
			if(psDefine & mc_define_afterFx_Contrast)strcat(m_dbgTxtAfterFX,"Contr ");
			if(psDefine & mc_define_afterFx_EyeFish) strcat(m_dbgTxtAfterFX,"EyeF ");
			if(psDefine & mc_define_afterFx_EdgeDetection) strcat(m_dbgTxtAfterFX,"EdgeD ");
			if(psDefine & mc_define_afterFx_Gauss)   strcat(m_dbgTxtAfterFX,"Gauss ");
			if(psDefine & mc_define_afterFx_Glow)    strcat(m_dbgTxtAfterFX,"Glow ");
			if(psDefine & mc_define_afterFx_Mirror)  strcat(m_dbgTxtAfterFX,"Mirr ");
			if(psDefine & mc_define_afterFx_Negatif) strcat(m_dbgTxtAfterFX,"Neg ");
			if(psDefine & mc_define_afterFx_Mosaic)  strcat(m_dbgTxtAfterFX,"Mos ");
			if(psDefine & mc_define_afterFx_Refraction) strcat(m_dbgTxtAfterFX,"Refr ");
			if(psDefine & mc_define_afterFx_Saturation) strcat(m_dbgTxtAfterFX,"Sat ");
			if(psDefine & mc_define_afterFx_Tile)       strcat(m_dbgTxtAfterFX,"Tile ");
			if(psDefine & mc_define_afterFx_Tonemap)    strcat(m_dbgTxtAfterFX,"Tonemap ");
		}*/
		#endif // ITF_FINAL
		//DEAC GFX_METRICS_END_TAG(GFX_MAIN_PASS_AFX_APPLY);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::fillHexagoneVertexBuffer(VertexPCT* _data, u32 _startIndex, Vec2d _position, f32 _hexagoneSize, const Vec2d& _uv1, const Vec2d& _uv2, const Vec2d& _uv3 )
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

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::AFXPostProcessKaleiPass(RenderPassContext & _rdrCtxt, AFXPostProcess &_postProcess)
	{
#if 0 //DEAC
		//DEAC GFX_METRICS_START_TAG(GFX_MAIN_PASS_AFX_KALEI);

		PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
		DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

		setRenderTargetForPass(m_curPassType);

		u32 W = getScreenWidth();
		u32 H = getScreenHeight();

		setDefaultGFXPrimitiveParam();
		setGfxMatDefault(drawCallCtx);
		setVertexFormat(VertexFormat_PCT);

		drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY);

		shaderDefineKey psDefine = 0;
		setShaderGroup( mc_shader_AfterFx, mc_entry_afterFx_VS_PCT, mc_entry_afterFx_PS_Kalei, 0, 0);

		SetTextureBind( 1, m_copyCurrentTextureBuffer->getTextureSamp(), btrue );
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

		//DEAC GFX_METRICS_END_TAG(GFX_MAIN_PASS_AFX_KALEI);
#endif	
	}

	// ------------------------------------------------------------------------------
	// Blur RenderTarget.
	// ------------------------------------------------------------------------------
	void GFXAdapter_OpenGLES2::BlurRenderBuffer(RenderPassContext & _rdrCtxt, u32 _pixelBlurSize, u32 _quality, u32 _src, u32 _dst, u32 _tmp)
	{
		if(_quality <= 0)
			return;
#if 0
		PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
		DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

		GFX_ZLIST_PASS_TYPE oldPassType = getCurPassType();
		m_curPassType = GFX_ZLIST_PASS_REGULAR;

		u32 destId = (_quality+1)%2;
		u32 sourceId = _quality%2;

		u32 texDest = (destId==0)?_dst:_tmp;
		u32 texSource = _src;

		TextureBuffer* texDst = m_currentTextureBuffer[texDest];
		TextureBuffer* texSrc = m_currentTextureBuffer[texSource];

		const f32 W = (f32)texDst->getWidth();
		const f32 H = (f32)texDst->getHeight();

		getWorldViewProjMatrix()->push();
		setOrthoView(0.f, W, 0.f, H);
		setupViewport(0, 0, (i32)W, (i32)H);

		setShaderGroup(mc_shader_AfterFx, mc_entry_afterFx_VS_blur, mc_entry_afterFx_PS_bigBlur);
		setDefaultGFXPrimitiveParam();
		setGfxMatDefault(drawCallCtx);

		drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY);

		const f32 ratiopixelX = 1.f/W;
		const f32 ratiopixelY = 1.f/H;

		f32 displaceX = u32(_pixelBlurSize / 2.0f) * ratiopixelX; //4 pixel Displace
		f32 displaceY = u32(_pixelBlurSize / 2.0f) * ratiopixelY;
		const f32 qualityDisplaceX = displaceX / _quality;
		const f32 qualityDisplaceY = displaceY / _quality;

		u32 pass = 0;

		while ((_quality && pass < _quality))
		{
			setRenderBuffers(texDst, getRenderDepthBuffer());

			texSrc->Invalidate();
			SetTextureBind(0, texSrc->getTextureSamp());
			setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);

			/// shader const.
			GFX_Vector4 vconst1(displaceX, displaceY, displaceX, displaceY);
			SetVertexShaderConstantF(VS_Attrib_afx, (f32*)&vconst1, 1);

			drawScreenQuadC(drawCallCtx, 0.f, 0.f, W, H, 0.f, COLOR_WHITE, getCentroidOffset(), bfalse);

			//swap target
			destId = (destId+1)%2;
			sourceId = (sourceId+1)%2;

			texDest = (destId==0)?_dst:_tmp;
			texSource = (sourceId==0)?_dst:_tmp;

			texDst = m_currentTextureBuffer[texDest];
			texSrc = m_currentTextureBuffer[texSource];

			displaceX -= qualityDisplaceX;
			displaceY -= qualityDisplaceY;
			pass++;
		}

		// restore camera
		View::getCurrentView()->apply();
		getWorldViewProjMatrix()->pop();

		m_curPassType = oldPassType;
#endif
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::blurFrontLightRenderBuffer(RenderPassContext & _rdrCtxt, u32 _quality, u32 _size)
	{
		u32 srcBuf = (_quality%2==0)?TEX_BFR_QRT_FRONT_LIGHT:TEX_BFR_QRT_TMP_LIGHT;
		u32 dstBuf = TEX_BFR_QRT_FRONT_LIGHT;
		u32 tmpBuf = TEX_BFR_QRT_TMP_LIGHT;
		BlurRenderBuffer(_rdrCtxt, _size, _quality, srcBuf, dstBuf, tmpBuf);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::blurBackLightRenderBuffer(RenderPassContext & _rdrCtxt, u32 _quality, u32 _size)
	{
		u32 srcBuf = (_quality%2==0)?TEX_BFR_QRT_BACK_LIGHT:TEX_BFR_QRT_TMP_LIGHT;
		u32 dstBuf = TEX_BFR_QRT_BACK_LIGHT;
		u32 tmpBuf = TEX_BFR_QRT_TMP_LIGHT;
		BlurRenderBuffer( _rdrCtxt, _size, _quality, srcBuf, dstBuf, tmpBuf);
	}

	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::restoreBackLightRenderBuffer(RenderPassContext & _rdrCtxt, eLightTexBuffer _currentLightBufferType)
	{
		CopySurface(_rdrCtxt, m_currentTextureBuffer[TEX_BFR_QRT_TMP_LIGHT], m_currentTextureBuffer[TEX_BFR_QRT_BACK_LIGHT], GFX_BLEND_COPY, btrue, 1.0f);
		CopySurface(_rdrCtxt, m_currentTextureBuffer[TEX_BFR_BACK_LIGHT], m_currentTextureBuffer[TEX_BFR_QRT_TMP_LIGHT], GFX_BLEND_COPY, btrue, 1.0f);
	}

	// ------------------------------------------------------------------------------
	// Draw blur Scene in Additional RenderTarget 0
	// ------------------------------------------------------------------------------
	void GFXAdapter_OpenGLES2::blurFluidRenderBuffer(RenderPassContext & _rdrCtxt)
	{
		GFX_ZLIST_PASS_TYPE oldPassType = getCurPassType();
		m_curPassType = GFX_ZLIST_PASS_REGULAR;

		PrimitiveContext primCtx = PrimitiveContext(&_rdrCtxt);
		DrawCallContext drawCallCtx = DrawCallContext(&primCtx);

		TextureBuffer* texDst = m_currentTextureBuffer[TEX_BFR_FLUID_BLUR];
		TextureBuffer* texSrc = m_currentTextureBuffer[TEX_BFR_FLUID_2];

		f32 W = (f32)texDst->getWidth();
		f32 H = (f32)texDst->getHeight();

		f32 ratiopixelX = 1.f/(f32)W;
		f32 ratiopixelY = 1.f/(f32)H;
		f32 displaceX = 8.0f * ratiopixelX;
		f32 displaceY = 8.0f * ratiopixelY;

		getWorldViewProjMatrix()->push();
		setOrthoView(0.f, W, 0.f, H);
		setupViewport(0, 0, (i32)W, (i32)H);

		setShaderGroup(mc_shader_AfterFx, mc_entry_afterFx_VS_blur, mc_entry_afterFx_PS_bigBlur, 0, mc_define_afterFx_Gauss);
		setDefaultGFXPrimitiveParam();
		setGfxMatDefault(drawCallCtx);

		drawCallCtx.getStateCache().setAlphaBlend(GFX_BLEND_COPY)
			.setColorWrite(btrue)
			.setAlphaWrite(btrue);

		//First Blur for fluid
		setRenderBuffers(texDst, getRenderDepthBuffer());
		clear(GFX_CLEAR_COLOR, 0.0f, 0.0f, 0.0f, 0.0f);

		// Set Texture
		texSrc->Invalidate();
		SetTextureBind(0, texSrc->getTextureSamp());
		setTextureAdressingMode(0, GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE_CLAMP);

		// shader const.
		GFX_Vector4 vconst1(displaceX, displaceY, displaceX, displaceY);
		SetVertexShaderConstantF(VS_Attrib_afx, (f32*)&vconst1, 1);

		drawScreenQuad(drawCallCtx, 0.0f, 0.0f, W, H, 0.f, COLOR_WHITE);

		//Second blur in small Buffer for possible glow effect
		if(m_fluidSimuInfo.m_useGlow)
		{
			TextureBuffer* texDst = m_currentTextureBuffer[TEX_BFR_FLUID_2];
			TextureBuffer* texSrc = m_currentTextureBuffer[TEX_BFR_FLUID_BLUR];
			f32 WGlow = (f32)texDst->getWidth();
			f32 HGlow = (f32)texDst->getHeight();

			float displaceSize = m_fluidSimuInfo.m_glowSize;
			displaceX = displaceSize * ratiopixelX;
			displaceY = displaceSize * ratiopixelY;

			setRenderBuffers(texDst, getRenderDepthBuffer());
			clear(0, 0.0f, 0.0f, 0.0f, 0.0f);

			// Set Texture
			texSrc->Invalidate();
			SetTextureBind(0, texSrc->getTextureSamp());

			// shader const.
			GFX_Vector4 vconst1(displaceX, displaceY, displaceX, displaceY);
			SetVertexShaderConstantF(VS_Attrib_afx, (f32*)&vconst1, 1);

			setOrthoView(0.f, WGlow, 0.f, HGlow);
			setupViewport(0, 0, (i32)WGlow, (i32)HGlow);
			setShaderGroup(mc_shader_AfterFx, mc_entry_afterFx_VS_blur, mc_entry_afterFx_PS_bigBlur, 0, mc_define_afterFx_Gauss);

			drawScreenQuad(drawCallCtx, 0.f, 0.f, WGlow, HGlow, 0.f, COLOR_WHITE);
		}

		// restore camera
		View::getCurrentView()->apply();
		getWorldViewProjMatrix()->pop();
		m_curPassType = oldPassType;
	}

}