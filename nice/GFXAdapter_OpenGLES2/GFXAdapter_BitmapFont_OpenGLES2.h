#ifndef _GFXADAPTER_BITMAPFONT_OPENGLES2_H_
#define _GFXADAPTER_BITMAPFONT_OPENGLES2_H_

namespace ITF
{

	class ITF_VertexBuffer;
	class ITF_IndexBuffer;
	class Texture;
	class DrawCallContext;
	/*
	class GRD_ConstantFBuffer;
	class GRD_SamplerState;
	class GRD_DepthStencilState;
	class GRD_BlendState;
	class GRD_StreamDecl;
	*/
	#if defined(_WIN32)
	#define GFX_BitmapFont_MaxTextVerticesCount (4096 * 3)
	#else
	#define GFX_BitmapFont_MaxTextVerticesCount (4096 * 3)
	#endif

	#define GFX_BitmapFont_MaxStringCount       2048
	#define GFX_BitmapFont_TabSize              8.0f

	struct GFX_CharRect
	{
		float fLeft;
		float fTop;
		float fRight;
		float fBottom;
	};

	struct GFX_Text
	{
		enum	{	GFX_BitmapFont_TextLengthMax = 1024 - 14	};
		u32     color;
		u32     bkgColor;
		u16     length;
		u8      autoAlignY;
		u8      unused;
		f32     x;
		f32     y;
		f32     offsetY;
		char    ac_Text[GFX_BitmapFont_TextLengthMax];
	};

	struct GFX_FontVertex
	{
		f32 posX;
		f32 posY;
		u32 color;
		//u32 backColor;
		u32 uv;
	};

	struct GFX_TextBatch
	{
		ITF_VertexBuffer *  po_VB;
		ITF_IndexBuffer *   po_IB;
		u16                 verticeCount;
		u16                 primitiveCount;
	};

	class GFX_BitmapFont
	{
	public :
								GFX_BitmapFont          ();
								~GFX_BitmapFont         ();

		void                    EnableDisplay          (bbool _b_Enabled)       { mb_EnableDisplay = _b_Enabled; }


		void                    vAddTextToDrawF        (float          _x,
														float          _y,
														bbool          _yIsRelativeScreen,
														u32				_foregroundColor,
														u32				_backgroundColor,
														const char *   _format,
														va_list        _list );

		void                    AddTextToDrawF         (float          _x,
														float          _y,
														bbool          _yIsRelativeScreen,
														u32				_foregroundColor,
														u32				_backgroundColor,
														const char *   _format,
														...);

		void                    AppendTextToDrawF      (float          _x,
														ux             _numLines,
														u32				_foregroundColor,
														u32				_backgroundColor,
														const char *   _format,
														...);

		void                    DrawAllTexts            (DrawCallContext &_drawCallCtx, bbool _b_FlipTexCoordV);
		void                    nextFrame               ();

		void                    setCurrentLineAndY      (ux _currentLine, f32 _y);
		void                    getCurrentLineAndY      (ux &_currentLine, f32 &_y) const;
		ux                      getCurrentLine          () const;
		f32                     getCurrentY             () const;
		f32                     getRelativeHeight       () const;

		void                    AddTextToDraw           (float          _fX,
														 float          _fY,
														 u32			_u32_ForegroundColor,
														 u32			_u32_BackgroundColor,
														 const char *   _pz_Text,
														 bbool          _autoAlignY = bfalse);

		void                    AppendTextToDraw        (float          _fX,
														 ux             _numLines,
														 u32			_u32_ForegroundColor,
														 u32			_u32_BackgroundColor,
														 const char *   _pz_Text);
		float                   getTextWidth            (const char* ptext, const size_t len = 0xFFFFFFFF);
		float                   getRelativeTextWidth    (const char* ptext, const size_t len);
	protected:

		bbool                   b_BuildBatch            (u16 * _puw_PrimitivesCount);
		const GFX_CharRect&		getCharUV               (int _cChar) const      { return mat_TexCoords[_cChar]; }
		Texture*				getTexture() const      { return mpo_Texture; }
		ux                      getHeight() const       { return mu16_Height; }

		bbool                   b_Init                  ();
		void                    Deinit                  ();
		void                    BuildFontFromHeader     ();
		void                    CreateVertexDecl        ();
		void                    CreateBlendState        ();
		void                    CreateDepthStencilState ();
		void                    CreateSamplerState      ();

	private :
		u16                     mu16_Width;
		u16                     mu16_Height;

		float                   mf_Scale;
		bbool                   mb_EnableDisplay;

		GFX_CharRect			mat_TexCoords[256];

		Texture*				mpo_Texture;

		float                   m_screenWidth;
		float                   m_screenHeight;
		float                   m_autoAlignY;
		float                   m_autoAlignYFactor;
		u16                     m_autoAlignYCurrLine;
		u16                     mu16_TextVertexCount;
		ITF_VertexBuffer*       mpo_VB;
		int                     mi_CurrentToDraw;

		ITF_VECTOR<GFX_Text>		mat_Strings[2];

		ux                      mux_CurrentText;
	};

	//extern GFX_BitmapFont*		GFX_gpDebugTextManager;

} // namespace ITF


#endif  // _GFXADAPTER_BITMAPFONT_OPENGLES2_H_
