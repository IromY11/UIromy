#include "precompiled_GFXAdapter_OpenGLES2.h"

#ifndef _GFXADAPTER_BITMAPFONT_OPENGLES2_H_
#include "adapters/GFXAdapter_OpenGLES2/GFXAdapter_BitmapFont_OpenGLES2.h"
#endif //_GFXADAPTER_BITMAPFONT_OPENGLES2_H_

#include "adapters/GFXAdapter_OpenGLES2/GFXAdapter_BitmapFontData_OpenGLES2.h"


#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_OpenGLES2/GFXAdapter_OpenGLES2.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_


#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_


#define GFX_OUTPUTTEXT_FIRSTLINE_Y    1

namespace ITF
{
	//GFX_BitmapFont* GRD_gpDebugTextManager = NULL; //EXTERN

	GFX_BitmapFont::GFX_BitmapFont() :
	mu16_Width(0),
	mu16_Height(0),
	mf_Scale(1.0f),
	mb_EnableDisplay(btrue),
	mpo_Texture(NULL),
	m_autoAlignY(0.0f),
	m_autoAlignYFactor(0.0f),
	m_autoAlignYCurrLine(0),
	mu16_TextVertexCount(0),
	mpo_VB(NULL),
	mi_CurrentToDraw(1),
	mux_CurrentText(0)
	{
		BuildFontFromHeader();

		b_Init();
	}

	//------------------------------------------------------------------------
	
	GFX_BitmapFont::~GFX_BitmapFont()
	{
		Deinit();
	}

	//------------------------------------------------------------------------
	bbool GFX_BitmapFont::b_Init()
	{
		ITF_ASSERT(!mpo_VB);

		//const ux ux_Size = sizeof(VertexPCT) *  GFX_BitmapFont_MaxTextVerticesCount;
 		mpo_VB = GFX_ADAPTER->createVertexBuffer(GFX_BitmapFont_MaxTextVerticesCount, VertexFormat_PCT, sizeof(VertexPCT), btrue, VB_T_INTERNAL);

		ITF_ASSERT(mpo_VB);

		mat_Strings[0].reserve(64);
		mat_Strings[1].reserve(64);

		mu16_TextVertexCount = 0;

		return btrue;
	}

	//------------------------------------------------------------------------
	
	void GFX_BitmapFont::Deinit()
	{
		if (mpo_VB)
		{
			GFX_ADAPTER->removeVertexBuffer(mpo_VB);
			mpo_VB = NULL;
		}

		if (mpo_Texture)
		{
			GFX_ADAPTER->cleanupTexture(mpo_Texture);
			RESOURCE_MANAGER->releaseEmptyResource(mpo_Texture);
			mpo_Texture = NULL;
		}

		for (ux i = 0; i < 2; ++i)
		{
			mat_Strings[i].clear();//Destroy(); ????
		}
	}

	//------------------------------------------------------------------------
	
	void GFX_BitmapFont::BuildFontFromHeader()
	{
		u32 ux_TexWidth, ux_TexHeight;

		//ITF_ASSERT(sizeof(fontgen::FONT_DATA) == fontgen::FONT_TEX_WIDTH * fontgen::FONT_TEX_HEIGHT, "bad input data");

		mu16_Width  = (u16)FONT_WIDTH;
		mu16_Height = (u16)FONT_HEIGHT;
		ux_TexWidth = FONT_TEX_WIDTH;
		ux_TexHeight= FONT_TEX_HEIGHT;

		// read font tex coords.
		ux nbrEntries = sizeof(mat_TexCoords) / sizeof(GFX_CharRect);
		for( ux c = 0; c < nbrEntries; ++c )
		{
			u32 tc = 4 * c;
			mat_TexCoords[c].fLeft =   FONT_TEXCOORD[tc + 0];
			mat_TexCoords[c].fTop =    FONT_TEXCOORD[tc + 1];
			mat_TexCoords[c].fRight =  FONT_TEXCOORD[tc + 2];
			mat_TexCoords[c].fBottom = FONT_TEXCOORD[tc + 3];
		}

		mpo_Texture = static_cast<Texture *>(RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture));
		// create texture and read pixel data.
		GFX_ADAPTER->createTexture(
			mpo_Texture,
			ux_TexWidth,
			ux_TexHeight,
			1,
			Texture::PF_RGBA,
			2,
			btrue
			);

		// lock the texture and fill it with content of gau8_Font.
		LOCKED_TEXTURE lockRect;
		u32 lock_flags = 0;

		GFX_ADAPTER->lockTexture(mpo_Texture, &lockRect, lock_flags);

		const u8* pSrc = &FONT_DATA[0];
		u32* pDst = (u32 *)lockRect.mp_Bits;
		for ( ux y = 0; y < ux_TexHeight; ++y )
		{
			for ( ux x = 0; x < ux_TexWidth; ++x )
			{
				const char c = pSrc[x];
				pDst[x] = c | (c<<8) | (c<<16) | (c<<24);
			}
			pDst += lockRect.m_pitch/4;
			pSrc += ux_TexWidth;
		}
		GFX_ADAPTER->unlockTexture(mpo_Texture);
	}

	void GFX_BitmapFont::DrawAllTexts(DrawCallContext &_drawCallCtx, bbool _b_FlipTexCoordV)
	{
		u32 textsCount = (u32)(mat_Strings[mi_CurrentToDraw].size());

		if ( 0 == textsCount )
		{
			mi_CurrentToDraw ^= 1;
			return;
		}

		m_screenWidth  = (f32)GFX_ADAPTER->getScreenWidth();//(f32)GFX_ADAPTER->getWindowWidth();
		m_screenHeight = (f32)GFX_ADAPTER->getScreenHeight();//(f32)GFX_ADAPTER->getWindowHeight();

		const float fCharHeight = (getCharUV('A').fBottom - getCharUV('A').fTop) * getTexture()->getSizeY();
		m_autoAlignYFactor = fCharHeight / m_screenHeight;

// 		const float fAspectRatio = m_screenWidth / m_screenHeight;

		/* 
		Matrix mViewProj;

		mViewProj.Ix = 1.0f / m_screenWidth * 2.0f;
		mViewProj.Iy = 0.0f;
		mViewProj.Iz = 0.0f;
		mViewProj.Sx = 0.0f;

		mViewProj.Jx = 0.0f;
		mViewProj.Jy = 1.0f / m_screenWidth * 2.0f * fAspectRatio;
		mViewProj.Jz = 0.0f;
		mViewProj.Sy = 0.0f;

		mViewProj.Kx = 0.0f;
		mViewProj.Ky = 0.0f;
		mViewProj.Kz = 1.0f;
		mViewProj.Sz = 0.0f;

		mViewProj.t.x = -1.0f;
		mViewProj.t.y = -1.0f;
		mViewProj.t.z = 0.0f;
		mViewProj.w   = 1.0f;

		Matrix44 mTransposeWorldViewProj;
		if ( _b_FlipTexCoordV )
		{
			Matrix mFlipMatrix;
			mFlipMatrix.setIdentity();
			mFlipMatrix.Jy *= -1;

			Matrix mRealViewProj;
			matrix44RealMul(&mRealViewProj, &mViewProj, &mFlipMatrix);
			matrix44Transpose(&mTransposeWorldViewProj, &mRealViewProj);
		}
		else
		{
			matrix44Transpose(&mTransposeWorldViewProj, &mViewProj);
		}

		//po_Driver->SetVSConstantF(C_f44ModelViewProj, (const float *)&mTransposeWorldViewProj, 4);
	*/

		GFX_ADAPTER->setVertexFormat(VertexFormat_PCT);
		GFX_ADAPTER->SetTexture(0, getTexture());
        

		u16 u16_PrimsCount;

		while ( mux_CurrentText < textsCount )
		{
			if ( b_BuildBatch(&u16_PrimsCount) && u16_PrimsCount > 0 )
			{
                // ?? ITF_ASSERT(mpo_VB->m_offset == 0);
                GFX_ADAPTER->setVertexBuffer(mpo_VB);
                

		        GFX_ADAPTER->DrawVertexBuffer(_drawCallCtx, GFX_TRIANGLES, 0, u16_PrimsCount  * 3);
                glBindBuffer(GL_ARRAY_BUFFER, 0 );
                
			}
			else
			{
				if ( 0xFFFF == u16_PrimsCount )
				{
					break;
				}
			}
		}

		m_autoAlignYCurrLine = GFX_OUTPUTTEXT_FIRSTLINE_Y;
		m_autoAlignY = 0.0f;
		mux_CurrentText = 0;
		nextFrame();
	}

	//------------------------------------------------------------------------

	bbool GFX_BitmapFont::b_BuildBatch(u16 * _puw_PrimitivesCount)
	{
		*_puw_PrimitivesCount = 0;

		union
		{
			VertexPCT* pst_Vertices;
			void * pVert;
		};
		pVert = NULL;

		GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(mpo_VB, (void**)&pVert);

		if (!pst_Vertices)
		{
			*_puw_PrimitivesCount = 0xFFFF;
			return false;
		}

		u32 u32_TextVerticesCount = 0;
		const float fCharHeight = (getCharUV('A').fBottom - getCharUV('A').fTop) * getTexture()->getSizeY();
		const float fInvScale = 1.0f / mf_Scale;
		const float fTabSize = GFX_BitmapFont_TabSize * mu16_Width;
		const float fInvTabSize = 1.0f / fTabSize;

		u32 textsCount = mat_Strings[mi_CurrentToDraw].size();
		for( ; mux_CurrentText < textsCount; ++mux_CurrentText )
		{
			GFX_Text * pst_Text = &mat_Strings[mi_CurrentToDraw].at(mux_CurrentText);

			

			if ( 0 == pst_Text->ac_Text[0] )
			{
				continue;
			}

			u32 u32_Length = (u32)pst_Text->length * 6;

			if( u32_Length + u32_TextVerticesCount > GFX_BitmapFont_MaxTextVerticesCount )
			{
				if ( u32_Length > GFX_BitmapFont_MaxTextVerticesCount )
				{
					// Text is too long.
					GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(mpo_VB);
					*_puw_PrimitivesCount = 0xFFFF;
					return bfalse;
				}

				// No more place. We'll need to restart from 0 for the next draw.
                if( u32_TextVerticesCount > 0 )
                    break;
			}

			float fX = pst_Text->x;
			float fY = pst_Text->y;
			if ( pst_Text->autoAlignY )
			{
				fY = fY * m_autoAlignYFactor + pst_Text->offsetY;
			}

			//fY += 32.0f/1280.0f;
			//fY = 1.0f - fY;

			//fX = fX * GFX_ADAPTER->getWindowWidth();
			//fY = fY * GFX_ADAPTER->getWindowHeight();

			float fStartX = fX;
			const char * pz_Text = pst_Text->ac_Text;

			while( *pz_Text )
			{
				char c = *pz_Text++;

				if( c == '\n' )
				{
					fX = fStartX;

					fY -= fCharHeight;
					continue;
				}

				if (c == '\t')
				{
					fX = (fX * fInvTabSize + 1.0F) * fTabSize;
					continue;
				}

				if( c < ' ' )
				{
					continue;
				}
                
                

				float tx1 = getCharUV(c).fLeft;
				float ty1 = getCharUV(c).fTop;
				float tx2 = getCharUV(c).fRight;
				float ty2 = getCharUV(c).fBottom;
				float w = (tx2-tx1) * getTexture()->getSizeX() * fInvScale;
				float h = (ty2-ty1) * getTexture()->getSizeY() * fInvScale;
				f32 u1 = tx1;
				f32 v1 = ty1;
				f32 u2 = tx2;
				f32 v2 = ty2;

				#define kf_Offset 0.f //0.5f demitexel

#if 0
#define setVertex(vertex, px, py, c0, c1, tu, tv) { vertex.m_pos.setX(px); vertex.m_pos.setY(py); vertex.m_pos.setZ(0.f); vertex.m_color = c0; /*vertex.backColor = c1;*/ vertex.m_uv.set(tu, tv);}

				setVertex(pst_Vertices[0], (fX+0-kf_Offset), (fY+h-kf_Offset), pst_Text->color, /*pst_Text->bkgColor*/, u1, v1);
				setVertex(pst_Vertices[1], (fX+0-kf_Offset), (fY+0-kf_Offset), pst_Text->color, /*pst_Text->bkgColor*/, u1, v2);
				setVertex(pst_Vertices[2], (fX+w-kf_Offset), (fY+h-kf_Offset), pst_Text->color, /*pst_Text->bkgColor*/, u2, v1);
				setVertex(pst_Vertices[3], (fX+w-kf_Offset), (fY+0-kf_Offset), pst_Text->color, /*pst_Text->bkgColor*/, u2, v2);
				setVertex(pst_Vertices[4], (fX+w-kf_Offset), (fY+h-kf_Offset), pst_Text->color, /*pst_Text->bkgColor*/, u2, v1);
				setVertex(pst_Vertices[5], (fX+0-kf_Offset), (fY+0-kf_Offset), pst_Text->color, /*pst_Text->bkgColor*/, u1, v2);
#else
				u32 TxTcolor = pst_Text->color; 
				pst_Vertices[0].m_pos.setX(fX+0-kf_Offset);
				pst_Vertices[0].m_pos.setY(fY+h-kf_Offset);
				pst_Vertices[0].m_pos.setZ(0.f);
				pst_Vertices[0].m_color = TxTcolor; // 0xffffffff;
				pst_Vertices[0].m_uv.set(u1, v1);

				pst_Vertices[1].m_pos.setX(fX+0-kf_Offset);
				pst_Vertices[1].m_pos.setY(fY+0-kf_Offset);
				pst_Vertices[1].m_pos.setZ(0.f);
				pst_Vertices[1].m_color = TxTcolor; // 0xffffffff;
				pst_Vertices[1].m_uv.set(u1, v2);

				pst_Vertices[2].m_pos.setX(fX+w-kf_Offset);
				pst_Vertices[2].m_pos.setY(fY+h-kf_Offset);
				pst_Vertices[2].m_pos.setZ(0.f);
				pst_Vertices[2].m_color = TxTcolor; // 0xffffffff;
				pst_Vertices[2].m_uv.set(u2, v1);

				pst_Vertices[3].m_pos.setX(fX+w-kf_Offset);
				pst_Vertices[3].m_pos.setY(fY+0-kf_Offset);
				pst_Vertices[3].m_pos.setZ(0.f);
				pst_Vertices[3].m_color = TxTcolor; // 0xffffffff;
				pst_Vertices[3].m_uv.set(u2, v2);

				pst_Vertices[4].m_pos.setX(fX+w-kf_Offset);
				pst_Vertices[4].m_pos.setY(fY+h-kf_Offset);
				pst_Vertices[4].m_pos.setZ(0.f);
				pst_Vertices[4].m_color = TxTcolor; // 0xffffffff;
				pst_Vertices[4].m_uv.set(u2, v1);

				pst_Vertices[5].m_pos.setX(fX+0-kf_Offset);
				pst_Vertices[5].m_pos.setY(fY+0-kf_Offset);
				pst_Vertices[5].m_pos.setZ(0.f);
				pst_Vertices[5].m_color = TxTcolor; // 0xffffffff;
				pst_Vertices[5].m_uv.set(u1, v2);

#endif

				pst_Vertices += 6;

				#undef M_InitVertex

				u32_TextVerticesCount  += 6;
				*_puw_PrimitivesCount += 2;

				fX += w;
			}
            
            ITF_ASSERT( *_puw_PrimitivesCount *3 <= GFX_BitmapFont_MaxTextVerticesCount );
            
            	

                    
                    
			// Free the text entry
			pst_Text->ac_Text[0] = 0;
		}
        ITF_ASSERT( *_puw_PrimitivesCount *3 <= GFX_BitmapFont_MaxTextVerticesCount );
        
		GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(mpo_VB);

		return true;
	}

	//------------------------------------------------------------------------
	
	void GFX_BitmapFont::AddTextToDraw(float _fX, float _fY, u32 _foregroundColor, u32 _backgroundColor, const char *_pz_Text, bbool _autoAlignY)
	{
		if ( NULL == _pz_Text || bfalse == mb_EnableDisplay )
		{
			return;
		}

		uSize ux_Length = (uSize)strlen(_pz_Text); // * 6;

		if ( 0 == ux_Length )
		{
			return;
		}

		GFX_Text tText;
		if ( ux_Length > (GFX_Text::GFX_BitmapFont_TextLengthMax - 1) )
			ux_Length = GFX_Text::GFX_BitmapFont_TextLengthMax - 1;
		memcpy(tText.ac_Text, _pz_Text, ux_Length);

		tText.ac_Text[ux_Length] = 0;
		tText.color     = _foregroundColor;//.getGraphicColor();
		tText.bkgColor  = _backgroundColor;//.getGraphicColor();
		tText.x         = _fX;
		tText.y         = GFX_ADAPTER->getScreenHeight() - _fY - 15; //magouille to match Directx version
		tText.length    = u16(ux_Length);

		if ( _autoAlignY == btrue )
		{
			tText.offsetY    = m_autoAlignY;
			tText.autoAlignY = 1;
		}
		else
		{
			tText.offsetY    = 0;
			tText.autoAlignY = 0;
		}

		mat_Strings[mi_CurrentToDraw].push_back(tText);
	}

	//------------------------------------------------------------------------
	
	void GFX_BitmapFont::AppendTextToDraw(float          _fX,
										  ux             _numLines,
										  u32			_foregroundColor,
										  u32				_backgroundColor,
										  const char *   _pz_Text)
	{
		float y = (float)m_autoAlignYCurrLine;
		m_autoAlignYCurrLine = (u16)(m_autoAlignYCurrLine + _numLines);
		AddTextToDraw(_fX, y, _foregroundColor, _backgroundColor, _pz_Text, btrue);
	}

	void GFX_BitmapFont::nextFrame()
	{
		mat_Strings[mi_CurrentToDraw].clear();
		mi_CurrentToDraw ^= 1;
	}

	void GFX_BitmapFont::setCurrentLineAndY(ux _currentLine, f32 _y)
	{
		m_autoAlignYCurrLine = (u16)_currentLine;
		m_autoAlignY = _y;
	}


	//------------------------------------------------------------------------
	ux GFX_BitmapFont::getCurrentLine() const
	{
		return m_autoAlignYCurrLine;
	}


	//------------------------------------------------------------------------
	f32 GFX_BitmapFont::getCurrentY() const
	{
		return m_autoAlignY;
	}


	//------------------------------------------------------------------------
	void GFX_BitmapFont::getCurrentLineAndY(ux &_currentLine, f32 &_y) const
	{
		_currentLine = getCurrentLine();
		_y = getCurrentY();
	}

	void GFX_BitmapFont::vAddTextToDrawF(float          _x,
										 float          _y,
										 bbool          _yIsRelativeScreen,
										 u32     _foregroundColor,
										 u32     _backgroundColor,
										 const char *   _format,
										 va_list        _list)
	{
#ifndef _FINAL_
		char az_buff[GFX_BitmapFont_MaxStringCount];
		vsnprintf(az_buff, sizeof(az_buff) - 1, _format, _list);

		AddTextToDraw(_x, _y, _foregroundColor, _backgroundColor, az_buff, _yIsRelativeScreen);
#endif
	}


	//------------------------------------------------------------------------
	void GFX_BitmapFont::AddTextToDrawF(float          _x,
										float          _y,
										bbool          _yIsRelativeScreen,
										u32     _foregroundColor,
										u32     _backgroundColor,
										const char *   _format,
										...)
	{
#ifndef _FINAL_
		va_list v;
		va_start(v, _format);
		char az_buff[GFX_BitmapFont_MaxStringCount];
		vsnprintf(az_buff, sizeof(az_buff) - 1, _format, v);
		va_end(v);

		AddTextToDraw(_x, _y, _foregroundColor, _backgroundColor, az_buff, _yIsRelativeScreen);
#endif
	}

	void GFX_BitmapFont::AppendTextToDrawF(float          _x,
										   ux             _numLines,
										   u32     _foregroundColor,
										   u32     _backgroundColor,
										   const char *   _format,
										   ...)
	{
#ifndef _FINAL_
		va_list v;
		va_start(v, _format);
		char az_buff[GFX_BitmapFont_MaxStringCount];
		vsnprintf(az_buff, sizeof(az_buff) - 1, _format, v);
		va_end(v);

		AppendTextToDraw(_x, _numLines, _foregroundColor, _backgroundColor, az_buff);
#endif
	}

	f32 GFX_BitmapFont::getRelativeHeight() const
	{
		const float fCharHeight = (getCharUV('A').fBottom - getCharUV('A').fTop) * getTexture()->getSizeY();
		return fCharHeight / (f32)m_screenHeight;
	}
	
	//------------------------------------------------------------------------
	
	float GFX_BitmapFont::getTextWidth(const char* ptext, const size_t len)
	{
		ITF_ASSERT(ptext);

		const float fInvScale = 1.0f / mf_Scale;
		float w = 0;
		size_t text_len = Min(len, strlen(ptext));
		for(size_t i=0;i<text_len;++i)
		{
			unsigned char c = ptext[i];
			if(c >= 127 || c <= 31) continue;
			float tx1 = getCharUV(c).fLeft;
			float tx2 = getCharUV(c).fRight;
			w += (tx2-tx1) * getTexture()->getSizeX()*fInvScale;
		}

		return w;
	}

	//------------------------------------------------------------------------
	
	float GFX_BitmapFont::getRelativeTextWidth(const char* ptext, const size_t len)
	{
		return getTextWidth(ptext, len) / (f32)m_screenWidth;
	}


} //namespace ITF

