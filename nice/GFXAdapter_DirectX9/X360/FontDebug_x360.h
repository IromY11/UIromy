#pragma once
#ifndef FONTDEBUG_X360_H
#define FONTDEBUG_X360_H

#include "Resource_x360.h"

namespace ITF
{

#define FONTDBG_LEFT       0x00000000
#define FONTDBG_RIGHT      0x00000001
#define FONTDBG_CENTER_X   0x00000002
#define FONTDBG_CENTER_Y   0x00000004
#define FONTDBG_TRUNCATED  0x00000008

//#define GENERATE_FONTSHADER //enable it to generate the shader [todo] implement in the normal shaders build process
 
//----------------------------------------------------------------------------//

typedef struct GLYPH_ATTR
{
    WORD m_tu1, m_tv1, m_tu2, m_tv2;
    SHORT m_Offset;
    SHORT m_Width;
    SHORT m_Advance;
    WORD m_Mask;
} GLYPH_ATTR;

//----------------------------------------------------------------------------//

enum SavedStates
{
    SAVEDSTATE_D3DRS_ALPHABLENDENABLE,
    SAVEDSTATE_D3DRS_SRCBLEND,
    SAVEDSTATE_D3DRS_DESTBLEND,
    SAVEDSTATE_D3DRS_BLENDOP,
    SAVEDSTATE_D3DRS_ALPHATESTENABLE,
    SAVEDSTATE_D3DRS_ALPHAREF,
    SAVEDSTATE_D3DRS_ALPHAFUNC,
    SAVEDSTATE_D3DRS_FILLMODE,
    SAVEDSTATE_D3DRS_CULLMODE,
    SAVEDSTATE_D3DRS_ZENABLE,
    SAVEDSTATE_D3DRS_STENCILENABLE,
    SAVEDSTATE_D3DRS_VIEWPORTENABLE,
    SAVEDSTATE_D3DSAMP_MINFILTER,
    SAVEDSTATE_D3DSAMP_MAGFILTER,
    SAVEDSTATE_D3DSAMP_ADDRESSU,
    SAVEDSTATE_D3DSAMP_ADDRESSV,

    SAVEDSTATE_COUNT,
    ENUM_FORCE_SIZE_32(SavedStates)
};

//----------------------------------------------------------------------------//

class Fontdebug
{
public:
    PackedResource m_xprResource;

    f32 m_FontHeight;
    f32 m_FontTopPadding;
    f32 m_FontBottomPadding;
    f32 m_FontYAdvance;

    f32 m_XScaleFactor;
    f32 m_YScaleFactor;
    f32 m_SlantFactor;
    bbool m_Rotate;
    DOUBLE m_RotCos;
    DOUBLE m_RotSin;

    D3DRECT m_rcWindow;
    f32 m_CursorX;
    f32 m_CursorY;

    DWORD m_MaxGlyph;
    WCHAR* m_TranslatorTable;

    DWORD m_NumGlyphs;
    const GLYPH_ATTR* m_Glyphs;

    BOOL m_bSaveState;
    DWORD m_SavedState[ SAVEDSTATE_COUNT ];
    DWORD m_NestedBeginCount;

    D3DTexture* m_pFontTexture;

public:
    ITF_INLINE D3DTexture* GetTexture() const
    {
        return m_pFontTexture;
    }
    ITF_INLINE f32 GetFontHeight() const
    {
        return m_FontYAdvance;
    }
    ITF_INLINE void SetSlantFactor( f32 _SlantFactor )
    {
        m_SlantFactor = _SlantFactor;
    }

    ITF_INLINE void SetScaleFactors( f32 _XScaleFactor, f32 _YScaleFactor )
    {
        m_XScaleFactor = _XScaleFactor;
        m_YScaleFactor = _YScaleFactor;
    }

public:
    Fontdebug();
    ~Fontdebug();

    HRESULT Create( const Path& fontFileName, LPDIRECT3DDEVICE9 _device );
    HRESULT Create( D3DTexture* pFontTexture, const VOID* pFontData );
    void    Destroy();

    void    GetTextExtent( const WCHAR* strText, f32* _pWidth,
                           f32* pHeight, bbool _FirstLineOnly = bfalse ) const;
    f32     GetTextWidth( const WCHAR* strText ) const;

    void    SetWindow(const D3DRECT &rcWindow );
    void    SetWindow( LONG x1, LONG y1, LONG x2, LONG y2 );
    void    GetWindow(D3DRECT &rcWindow) const;
    void    SetCursorPosition( f32 _CursorX, f32 _CursorY );
    void    SetRotationFactor( f32 _RotationFactor );

    D3DTexture* CreateTexture( const WCHAR* strText,
                               D3DCOLOR _BackgroundColor = 0x00000000,
                               D3DCOLOR _TextColor = 0xffffffff,
                               D3DFORMAT _d3dFormat = D3DFMT_A8R8G8B8 );

    void    Begin();
    void    DrawText( DWORD _Color, const WCHAR* strText, DWORD _Flags=0L,
                      f32 _MaxPixelWidth = 0.0f );
    void    DrawText( f32 _sx, f32 _sy, DWORD _Color, const WCHAR* strText,
                      DWORD _Flags=0L, f32 _MaxPixelWidth = 0.0f );
    void    End();

    D3DDevice *m_pD3dDevice;

private:

#ifdef GENERATE_FONTSHADER
    HRESULT CreateFontShaders();
#endif //GENERATE_FONTSHADER

    HRESULT loadFontShaders();
    void ReleaseFontShaders();
    void RotatePoint( f32* _X, f32* _Y, DOUBLE _OriginX, DOUBLE _OriginY ) const;
};


//----------------------------------------------------------------------------//

#define GLYPH_A_BUTTON      L"\400"
#define GLYPH_B_BUTTON      L"\401"
#define GLYPH_X_BUTTON      L"\402"
#define GLYPH_Y_BUTTON      L"\403"
#define GLYPH_WHITE_BUTTON  L"\404"
#define GLYPH_BLACK_BUTTON  L"\405"
#define GLYPH_LEFT_BUTTON   L"\406"
#define GLYPH_RIGHT_BUTTON  L"\407"
#define GLYPH_START_BUTTON  L"\410\411"
#define GLYPH_BACK_BUTTON   L"\412\413"

#define GLYPH_LEFT_HAND     L"\414"
#define GLYPH_RIGHT_HAND    L"\415"
#define GLYPH_UP_HAND       L"\416"
#define GLYPH_DOWN_HAND     L"\417"

#define GLYPH_BULLET        L"\500"
#define GLYPH_STAR_1        L"\501"
#define GLYPH_STAR_2        L"\502"
#define GLYPH_STAR_3        L"\503"
#define GLYPH_STAR_4        L"\504"
#define GLYPH_STAR_5        L"\505"

#define GLYPH_BIG_X         L"\506"
#define GLYPH_HAND          L"\507"
#define GLYPH_CHECK_MARK    L"\510"
#define GLYPH_X_MARK        L"\511"
#define GLYPH_SKULL         L"\512"

#define GLYPH_LEFT_TICK     L"\514"
#define GLYPH_RIGHT_TICK    L"\515"
#define GLYPH_UP_TICK       L"\516"
#define GLYPH_DOWN_TICK     L"\517"

#define GLYPH_HOLLOW_CIRCLE L"\600"
#define GLYPH_FILLED_CIRCLE L"\601"

#define GLYPH_LR_ARROW      L"\612"
#define GLYPH_UD_ARROW      L"\613"
#define GLYPH_LEFT_ARROW    L"\614"
#define GLYPH_RIGHT_ARROW   L"\615"
#define GLYPH_UP_ARROW      L"\616"
#define GLYPH_DOWN_ARROW    L"\617"

} // namespace ITF

#endif // FONTDEBUG_H
