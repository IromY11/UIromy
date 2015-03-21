#include "precompiled_GFXAdapter_Directx9.h"

#include "FontDebug_x360.h"
#include <xgraphics.h>

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_


#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_FILE_H_
# include "core/file/File.h"
#endif _ITF_FILE_H_

namespace ITF
{
    
#define CALCFONTFILEHEADERSIZE(x) ( sizeof(DWORD) + (sizeof(f32)*4) + sizeof(WORD) + (sizeof(WCHAR)*(x)) )
#define FONTFILEVERSION 5

    typedef struct FontFileHeaderImage_t
    {
        DWORD m_FileVersion;
        f32 m_FontHeight;
        f32 m_FontTopPadding;
        f32 m_FontBottomPadding;
        f32 m_FontYAdvance;
        WORD m_MaxGlyph;
        WCHAR m_TranslatorTable[1];

    } FontFileHeaderImage_t;

    typedef struct FontFileStrikesImage_t
    {
        DWORD m_NumGlyphs;
        GLYPH_ATTR m_Glyphs[1];
    } FontFileStrikesImage_t; 

#ifdef GENERATE_FONTSHADER
    static const CHAR g_strFontShader[] =
        "struct VS_IN\n"
        "{\n"
        "float2 Pos : POSITION;\n"
        "float2 Tex : TEXCOORD0;\n"
        "float4 ChannelSelector : TEXCOORD1;\n"
        "};\n"
        //  "\n"
        "struct VS_OUT\n"
        "{\n"
        "float4 Position : POSITION;\n"
        "float4 Diffuse : COLOR0_center;\n"
        "float2 TexCoord0 : TEXCOORD0;\n"
        "float4 ChannelSelector : TEXCOORD1;\n"
        "};\n"
        //  "\n"
        "uniform float4 Color : register(c1);\n"
        "uniform float2 TexScale : register(c2);\n"
        //  "\n"
        "sampler FontTexture : register(s0);\n"
        //  "\n"
        "VS_OUT FontVertexShader( VS_IN In )\n"
        "{\n"
        "VS_OUT Out;\n"
        "Out.Position.x  = (In.Pos.x-0.5);\n"
        "Out.Position.y  = (In.Pos.y-0.5);\n"
        "Out.Position.z  = ( 0.0 );\n"
        "Out.Position.w  = ( 1.0 );\n"
        "Out.Diffuse = Color;\n"
        "Out.TexCoord0.x = In.Tex.x * TexScale.x;\n"
        "Out.TexCoord0.y = In.Tex.y * TexScale.y;\n"
        "Out.ChannelSelector = In.ChannelSelector;\n"
        "return Out;\n"
        "}\n"
        // "\n"
        "float4 FontPixelShader( VS_OUT In ) : COLOR0\n"
        "{\n"
        //      "// Fetch a texel from the font texture\n"
        "float4 FontTexel = tex2D( FontTexture, In.TexCoord0 );\n"
        //      "\n"
        "if( dot( In.ChannelSelector, float4(1,1,1,1) ) )\n"
        "{\n"
        //          "// Select the color from the channel\n"
        "float value = dot( FontTexel, In.ChannelSelector );\n"
        //          "\n"
        //          "// For white pixels, the high bit is 1 and the low\n"
        //          "// bits are luminance, so r0.a will be > 0.5. For the\n"
        //          "// RGB channel, we want to lop off the msb and shift\n"
        //          "// the lower bits up one bit. This is simple to do\n"
        //          "// with the _bx2 modifier. Since these pixels are\n"
        //          "// opaque, we emit a 1 for the alpha channel (which\n"
        //          "// is 0.5 x2 ).\n"
        //          "\n"
        //          "// For black pixels, the high bit is 0 and the low\n"
        //          "// bits are alpha, so r0.a will be < 0.5. For the RGB\n"
        //          "// channel, we emit zero. For the alpha channel, we\n"
        //          "// just use the x2 modifier to scale up the low bits\n"
        //          "// of the alpha.\n"
        "float4 Color;\n"
        "Color.rgb = ( value > 0.5f ? 2*value-1 : 0.0f );\n"
        "Color.a = 2 * ( value > 0.5f ? 1.0f : value );\n"
        //          "\n"
        //          "// Return the texture color modulated with the vertex\n"
        //          "// color\n"
        "return Color * In.Diffuse;\n"
        "}\n"
        "else\n"
        "{\n"
        "return FontTexel * In.Diffuse;\n"
        "}\n"
        "}\n";
#endif //GENERATE_FONTSHADER

    typedef struct FontDebug_Locals_t {
        D3DVertexDeclaration* m_pFontVertexDecl;
        D3DVertexShader* m_pFontVertexShader;
        D3DPixelShader* m_pFontPixelShader;
    } FontDebug_Locals_t;

    static FontDebug_Locals_t s_FontDebugLocals;

    static const D3DVERTEXELEMENT9 decl[] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0,  8, D3DDECLTYPE_USHORT2,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        D3DDECL_END()
    };


    //----------------------------------------------------------------------------//
    HRESULT Fontdebug::loadFontShaders()
    {
          Path shaderDebug(ALIASMANAGER->getPath("fontdebugx360"));
          shaderDebug.setFlag(Path::FlagNonData, btrue);
          HRESULT Result;
     
          Result = m_pD3dDevice->CreateVertexDeclaration( decl, &s_FontDebugLocals.m_pFontVertexDecl );
          if (Result!=S_OK)
              return Result;


          File* pFile = FILEMANAGER->openFile(shaderDebug,  ITF_FILE_ATTR_READ);
          if (pFile)
          {
              u32 lenght = (u32) pFile->getLength();
              u8* pBuffer = newAlloc(mId_Temporary,u8[lenght]);

              bbool rdValid = pFile->read(pBuffer,lenght);
              FILEMANAGER->closeFile(pFile);
              pFile = NULL;

              ArchiveMemory rd(pBuffer,lenght);

              //vertex shader
              u32 shVertexLen;
              rd.serialize(shVertexLen);

              u8* pVertexShaderCode = newAlloc(mId_Temporary,u8[shVertexLen]);
              rd.serializeBlock8(pVertexShaderCode,shVertexLen);

              //pixel shader
              u32 shPixelLen;
              rd.serialize(shPixelLen);

              u8* pPixelShaderCode = newAlloc(mId_Temporary,u8[shPixelLen]);
              rd.serializeBlock8(pPixelShaderCode,shPixelLen);

              Result = static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXCreateVertexShader((DWORD*)pVertexShaderCode,&s_FontDebugLocals.m_pFontVertexShader);

              if (SUCCEEDED( Result ))
              {
                   Result =static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->DXCreatePixelShader((DWORD*)pPixelShaderCode,&s_FontDebugLocals.m_pFontPixelShader);
              }

              SF_DEL_ARRAY(pPixelShaderCode);
              SF_DEL_ARRAY(pVertexShaderCode);

              SF_DEL_ARRAY(pBuffer);
          }
     

          return Result;
    }


#ifdef GENERATE_FONTSHADER
    HRESULT Fontdebug::CreateFontShaders()
    {   
        ArchiveMemory shaderBin;
        
        HRESULT Result  =S_FALSE;
        if (!s_FontDebugLocals.m_pFontVertexDecl)
        {
            do {

                Result = m_pD3dDevice->CreateVertexDeclaration( decl, &s_FontDebugLocals.m_pFontVertexDecl );
               
                if ( SUCCEEDED( Result ) )
                {
                    ID3DXBuffer* pShaderCode;
                    Result = D3DXCompileShader( g_strFontShader, sizeof(g_strFontShader)-1 ,
                        NULL, NULL, "FontVertexShader", "vs.2.0", 0,&pShaderCode, NULL, NULL );
                    if ( SUCCEEDED( Result ) )
                    {
                     
                        u32 bufferSize = pShaderCode->GetBufferSize();
                        shaderBin.serialize(bufferSize);
                        shaderBin.serializeBlock8((u8*)pShaderCode->GetBufferPointer(),bufferSize);

                        pShaderCode->Release();


                        if( SUCCEEDED( Result ) )
                        {
                            Result = D3DXCompileShader( g_strFontShader, sizeof(g_strFontShader)-1 ,
                                NULL, NULL, "FontPixelShader", "ps.2.0", 0,&pShaderCode, NULL, NULL );
                            if ( SUCCEEDED( Result ) )
                            {
                                u32 bufferSize = pShaderCode->GetBufferSize();
                                shaderBin.serialize(bufferSize);
                                shaderBin.serializeBlock8((u8*)pShaderCode->GetBufferPointer(),bufferSize);

                                pShaderCode->Release();

                                if ( SUCCEEDED( Result ) ) 
                                {
                                    Result = S_OK;
                                    break;
                                }
                            }

                            s_FontDebugLocals.m_pFontVertexShader->Release();
                        }
                        s_FontDebugLocals.m_pFontVertexShader = NULL;
                    }
                    s_FontDebugLocals.m_pFontVertexDecl->Release();
                }
                s_FontDebugLocals.m_pFontVertexDecl = NULL;
            } while (0);

            //write the shader on the console ,need to be copy on the data folder
            if (Result == S_OK)
            {
                Path shaderDebug("e:\\fontShader.bin");
                shaderDebug.setFlag(Path::FlagNonData, btrue);

                File* pFile = FILEMANAGER->openFile(shaderDebug,  ITF_FILE_ATTR_CREATE_NEW |ITF_FILE_ATTR_WRITE);
                if (pFile)
                {
                    pFile->write(shaderBin.getData(),shaderBin.getSize());
                    FILEMANAGER->closeFile(pFile);
                }
            }

            return Result;
        }
         

        return Result;
    }

#endif //GENERATE_FONTSHADER
    //----------------------------------------------------------------------------//

    void Fontdebug::ReleaseFontShaders()
    {
        if( ( s_FontDebugLocals.m_pFontPixelShader != NULL ) && ( s_FontDebugLocals.m_pFontPixelShader->Release() == 0 ) )
            s_FontDebugLocals.m_pFontPixelShader = NULL;
        if( ( s_FontDebugLocals.m_pFontVertexShader != NULL ) && ( s_FontDebugLocals.m_pFontVertexShader->Release() == 0 ) )
            s_FontDebugLocals.m_pFontVertexShader = NULL;
        if( ( s_FontDebugLocals.m_pFontVertexDecl != NULL ) && ( s_FontDebugLocals.m_pFontVertexDecl->Release() == 0 ) )
            s_FontDebugLocals.m_pFontVertexDecl = NULL;
    }

    //----------------------------------------------------------------------------//

    Fontdebug::Fontdebug()
    {
        m_pFontTexture = NULL;

        m_NumGlyphs = 0L;
        m_Glyphs = NULL;

        m_CursorX = 0.0f;
        m_CursorY = 0.0f;

        m_XScaleFactor = 1.0f;
        m_YScaleFactor = 1.0f;
        m_SlantFactor = 0.0f;
        m_Rotate = FALSE;
        m_RotCos = cos( 0.0 );
        m_RotSin = sin( 0.0 );

        m_MaxGlyph = 0;
        m_TranslatorTable = NULL;

        m_NestedBeginCount = 0L;

        m_pD3dDevice = NULL;

        m_FontHeight = 0;
        m_FontTopPadding = 0;
        m_FontBottomPadding = 0;
        m_FontYAdvance = 0;

        m_bSaveState = bfalse;
        ITF_MemSet(m_SavedState,0,sizeof(m_SavedState));
        ITF_MemSet(&m_rcWindow,0,sizeof(m_rcWindow));
    }

    //----------------------------------------------------------------------------//

    Fontdebug::~Fontdebug()
    {
        Destroy();
    }

    //----------------------------------------------------------------------------//

    HRESULT Fontdebug::Create(const Path& fontFileName, LPDIRECT3DDEVICE9 _device )
    {
        m_pD3dDevice = (D3DDevice*)_device;
        if( FAILED( m_xprResource.Create( fontFileName ) ) )
            return E_FAIL;

        return Create( m_xprResource.GetTexture( "FontTexture" ),
            m_xprResource.GetData( "FontData" ) );
    }

    //----------------------------------------------------------------------------//

    HRESULT Fontdebug::Create( D3DTexture* pFontTexture, const void* pFontData )
    {
        m_pFontTexture = pFontTexture;

        const u8* pData = static_cast<const u8*>(pFontData);
        DWORD FileVersion = reinterpret_cast<const FontFileHeaderImage_t *>(pData)->m_FileVersion;

        if( FileVersion == FONTFILEVERSION )
        {
            m_FontHeight = reinterpret_cast<const FontFileHeaderImage_t *>(pData)->m_FontHeight;
            m_FontTopPadding = reinterpret_cast<const FontFileHeaderImage_t *>(pData)->m_FontTopPadding;
            m_FontBottomPadding = reinterpret_cast<const FontFileHeaderImage_t *>(pData)->m_FontBottomPadding;
            m_FontYAdvance = reinterpret_cast<const FontFileHeaderImage_t *>(pData)->m_FontYAdvance;

            m_MaxGlyph = reinterpret_cast<const FontFileHeaderImage_t *>(pData)->m_MaxGlyph;

            m_TranslatorTable = const_cast<FontFileHeaderImage_t*>(reinterpret_cast<const FontFileHeaderImage_t *>(pData))->m_TranslatorTable;

            pData += CALCFONTFILEHEADERSIZE( m_MaxGlyph + 1 );

            m_NumGlyphs = reinterpret_cast<const FontFileStrikesImage_t *>(pData)->m_NumGlyphs;
            m_Glyphs = reinterpret_cast<const FontFileStrikesImage_t *>(pData)->m_Glyphs;
        }
        else
        {
            return E_FAIL;
        }

        if( FAILED( loadFontShaders() ) )
        {
            return E_FAIL;
        }

        // Initialize the window
        D3DDISPLAYMODE DisplayMode;
        m_pD3dDevice->GetDisplayMode( 0, &DisplayMode );
        m_rcWindow.x1 = 0;
        m_rcWindow.y1 = 0;
        m_rcWindow.x2 = DisplayMode.Width;
        m_rcWindow.y2 = DisplayMode.Height;

        m_bSaveState = TRUE;

        return S_OK;
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::Destroy()
    {
        m_pFontTexture = NULL;
        m_NumGlyphs = 0L;
        m_Glyphs = NULL;
        m_MaxGlyph = 0;
        m_TranslatorTable = NULL;
        m_NestedBeginCount = 0L;

        ReleaseFontShaders();

        if( m_xprResource.Initialized() )
        {
            m_xprResource.Destroy();
        }
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::SetWindow(const D3DRECT &rcWindow )
    {
        m_rcWindow.x1 = rcWindow.x1;
        m_rcWindow.y1 = rcWindow.y1;
        m_rcWindow.x2 = rcWindow.x2;
        m_rcWindow.y2 = rcWindow.y2;

        m_CursorX = 0.0f;
        m_CursorY = 0.0f;
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::SetWindow( LONG x1, LONG y1, LONG x2, LONG y2 )
    {
        m_rcWindow.x1 = x1;
        m_rcWindow.y1 = y1;
        m_rcWindow.x2 = x2;
        m_rcWindow.y2 = y2;

        m_CursorX = 0.0f;
        m_CursorY = 0.0f;
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::GetWindow( D3DRECT &rcWindow ) const
    {
        rcWindow = m_rcWindow;
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::SetCursorPosition( f32 _CursorX, f32 _CursorY )
    {
        m_CursorX = floorf( _CursorX );
        m_CursorY = floorf( _CursorY );
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::SetRotationFactor( f32 _RotationFactor )
    {
        m_Rotate = _RotationFactor != 0;

        m_RotCos = cos( _RotationFactor );
        m_RotSin = sin( _RotationFactor );
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::GetTextExtent( const WCHAR* strText, f32* _pWidth,
        f32* _pHeight, bbool _FirstLineOnly ) const
    {
        ITF_ASSERT( _pWidth != NULL );
        ITF_ASSERT( _pHeight != NULL );

        int iWidth = 0;
        f32 fHeight = 0.0f;

        if( strText )
        {
            int ix = 0;
            f32 fy = m_FontHeight;
            if( fy > fHeight )
                fHeight = fy;

            DWORD letter;
            while( (letter = *strText) != 0 )
            {
                ++strText;

                if( letter == L'\n' )
                {
                    if( _FirstLineOnly )
                        break;
                    ix = 0;
                    fy += m_FontYAdvance;
                    if( fy > fHeight )
                        fHeight = fy;
                }

                if( letter == L'\r' )
                    continue;

                const GLYPH_ATTR* pGlyph;

                if( letter > m_MaxGlyph )
                    letter = 0;
                else
                    letter = m_TranslatorTable[letter];

                pGlyph = &m_Glyphs[letter]; 

                ix += pGlyph->m_Offset;
                ix += pGlyph->m_Advance;

                if( ix > iWidth )
                    iWidth = ix;
            }
        }

        f32 fWidth = static_cast<f32>(iWidth);
        fHeight *= m_YScaleFactor;
        *_pHeight = fHeight;

        fWidth *= m_XScaleFactor;
        *_pWidth = fWidth;
    }

    //----------------------------------------------------------------------------//

    f32 Fontdebug::GetTextWidth( const WCHAR* strText ) const
    {
        f32 fTextWidth;
        f32 fTextHeight;
        GetTextExtent( strText, &fTextWidth, &fTextHeight );
        return fTextWidth;
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::Begin()
    {
        if( 0 == m_NestedBeginCount )
        {
            ITF_ASSERT( m_pD3dDevice );
            // Save state
            if( m_bSaveState )
            {
                m_pD3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,
                    &m_SavedState[ SAVEDSTATE_D3DRS_ALPHABLENDENABLE ] );
                m_pD3dDevice->GetRenderState( D3DRS_SRCBLEND, &m_SavedState[ SAVEDSTATE_D3DRS_SRCBLEND ] );
                m_pD3dDevice->GetRenderState( D3DRS_DESTBLEND, &m_SavedState[ SAVEDSTATE_D3DRS_DESTBLEND ] );
                m_pD3dDevice->GetRenderState( D3DRS_BLENDOP, &m_SavedState[ SAVEDSTATE_D3DRS_BLENDOP ] );
                m_pD3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &m_SavedState[ SAVEDSTATE_D3DRS_ALPHATESTENABLE ] );
                m_pD3dDevice->GetRenderState( D3DRS_ALPHAREF, &m_SavedState[ SAVEDSTATE_D3DRS_ALPHAREF ] );
                m_pD3dDevice->GetRenderState( D3DRS_ALPHAFUNC, &m_SavedState[ SAVEDSTATE_D3DRS_ALPHAFUNC ] );
                m_pD3dDevice->GetRenderState( D3DRS_FILLMODE, &m_SavedState[ SAVEDSTATE_D3DRS_FILLMODE ] );
                m_pD3dDevice->GetRenderState( D3DRS_CULLMODE, &m_SavedState[ SAVEDSTATE_D3DRS_CULLMODE ] );
                m_pD3dDevice->GetRenderState( D3DRS_ZENABLE, &m_SavedState[ SAVEDSTATE_D3DRS_ZENABLE ] );
                m_pD3dDevice->GetRenderState( D3DRS_STENCILENABLE, &m_SavedState[ SAVEDSTATE_D3DRS_STENCILENABLE ] );
                m_pD3dDevice->GetRenderState( D3DRS_VIEWPORTENABLE, &m_SavedState[ SAVEDSTATE_D3DRS_VIEWPORTENABLE ] );
                m_pD3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &m_SavedState[ SAVEDSTATE_D3DSAMP_MINFILTER ] );
                m_pD3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER, &m_SavedState[ SAVEDSTATE_D3DSAMP_MAGFILTER ] );
                m_pD3dDevice->GetSamplerState( 0, D3DSAMP_ADDRESSU, &m_SavedState[ SAVEDSTATE_D3DSAMP_ADDRESSU ] );
                m_pD3dDevice->GetSamplerState( 0, D3DSAMP_ADDRESSV, &m_SavedState[ SAVEDSTATE_D3DSAMP_ADDRESSV ] );
            }

            D3DSURFACE_DESC TextureDesc;
            m_pFontTexture->GetLevelDesc( 0, &TextureDesc );

            static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->SetTextureBind( 0, NULL ); // Unbind previous texture first.
            m_pD3dDevice->SetTexture( 0, m_pFontTexture );

            f32 vTexScale[4];
            vTexScale[0] = 1.0f / TextureDesc.Width;
            vTexScale[1] = 1.0f / TextureDesc.Height;
            vTexScale[2] = 0.0f;
            vTexScale[3] = 0.0f;

            m_pD3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
            m_pD3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
            m_pD3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
            m_pD3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
            m_pD3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
            m_pD3dDevice->SetRenderState( D3DRS_ALPHAREF, 0x08 );
            m_pD3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
            m_pD3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
            m_pD3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
            m_pD3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
            m_pD3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
            m_pD3dDevice->SetRenderState( D3DRS_VIEWPORTENABLE, FALSE );
            m_pD3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
            m_pD3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
            m_pD3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
            m_pD3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

            m_pD3dDevice->SetVertexDeclaration( s_FontDebugLocals.m_pFontVertexDecl );
            m_pD3dDevice->SetVertexShader( s_FontDebugLocals.m_pFontVertexShader );
            m_pD3dDevice->SetPixelShader( s_FontDebugLocals.m_pFontPixelShader );

            m_pD3dDevice->SetVertexShaderConstantF( 2, vTexScale, 1 );
        }

        m_NestedBeginCount++;
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::DrawText( DWORD _Color, const WCHAR* strText, DWORD _Flags,
        f32 _MaxPixelWidth )
    {
        DrawText( m_CursorX, m_CursorY, _Color, strText, _Flags, _MaxPixelWidth );
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::DrawText( f32 _OriginX, f32 _OriginY, DWORD _Color,
        const WCHAR* strText, DWORD _Flags, f32 _MaxPixelWidth )
    {
        if( NULL == strText )    return;
        if( L'\0' == strText[0] ) return;

        f32 vColor[4];
        vColor[0] = ( ( _Color & 0x00ff0000 ) >> 16L ) / 255.0F;
        vColor[1] = ( ( _Color & 0x0000ff00 ) >> 8L ) / 255.0F;
        vColor[2] = ( ( _Color & 0x000000ff ) >> 0L ) / 255.0F;
        vColor[3] = ( ( _Color & 0xff000000 ) >> 24L ) / 255.0F;

        Begin();

        m_pD3dDevice->SetVertexShaderConstantF( 1, vColor, 1 );

        if( ( _OriginX < 0.0f ) || ( ( _Flags & FONTDBG_RIGHT ) && ( _OriginX <= 0.0f ) ) )
        {
            _OriginX += ( m_rcWindow.x2 - m_rcWindow.x1 );
        }
        if( _OriginY < 0.0f )
        {
            _OriginY += ( m_rcWindow.y2 - m_rcWindow.y1 );
        }

        m_CursorX = floorf( _OriginX );
        m_CursorY = floorf( _OriginY );

        // Adjust for padding
        _OriginY -= m_FontTopPadding;

        f32 EllipsesPixelWidth = m_XScaleFactor * 3.0f * ( m_Glyphs[m_TranslatorTable[L'.']].m_Offset +
            m_Glyphs[m_TranslatorTable[L'.']].m_Advance );

        if( _Flags & FONTDBG_TRUNCATED )
        {
            if( _MaxPixelWidth <= 0.0f )
            {
                _Flags &= ( ~FONTDBG_TRUNCATED );
            }
            else
            {
                f32 w, h;
                GetTextExtent( strText, &w, &h, TRUE );

                if( w <= _MaxPixelWidth )
                    _Flags &= ( ~FONTDBG_TRUNCATED );
            }
        }

        if( _Flags & FONTDBG_CENTER_Y )
        {
            f32 w, h;
            GetTextExtent( strText, &w, &h );
            m_CursorY = floorf( m_CursorY - (h * 0.5f) );
        }

        f32 Winx = static_cast<f32>(m_rcWindow.x1);
        f32 Winy = static_cast<f32>(m_rcWindow.y1);
        _OriginX += Winx;
        _OriginY += Winy;
        m_CursorX += Winx;
        m_CursorY += Winy;

        bbool StartingNewLine = TRUE;

        DWORD NumEllipsesToDraw = 0;

        volatile f32* pVertex;

        DWORD NumChars = wcslen( strText ) + ( _Flags & FONTDBG_TRUNCATED ? 3 : 0 );
        HRESULT hr = m_pD3dDevice->BeginVertices( D3DPT_QUADLIST, 4 * NumChars, sizeof( XMFLOAT4 ) ,
            ( void** )&pVertex );
        ITF_ASSERT(hr == S_OK);
        StartingNewLine = TRUE;

        while( *strText )
        {
            WCHAR letter;

            if( NumEllipsesToDraw )
            {
                letter = L'.';
            }
            else
            {
                if( StartingNewLine )
                {
                    if( _Flags & ( FONTDBG_RIGHT | FONTDBG_CENTER_X ) )
                    {
                        f32 w, h;
                        GetTextExtent( strText, &w, &h, TRUE );

                        if( _Flags & FONTDBG_RIGHT )
                            m_CursorX = floorf( _OriginX - w );
                        if( _Flags & FONTDBG_CENTER_X )
                            m_CursorX = floorf( _OriginX - w * 0.5f );
                    }
                    StartingNewLine = FALSE;
                }

                letter = *strText++;

                if( letter == L'\n' )
                {
                    m_CursorX = _OriginX;
                    m_CursorY += m_FontYAdvance * m_YScaleFactor;
                    StartingNewLine = TRUE;
                    continue;
                }

                if( letter == L'\r' )
                    continue;
            }

            const GLYPH_ATTR* pGlyph = &m_Glyphs[ ( letter <= m_MaxGlyph ) ? m_TranslatorTable[letter] : 0 ];

            f32 Offset = m_XScaleFactor * ( f32 )pGlyph->m_Offset;
            f32 Advance = m_XScaleFactor * ( f32 )pGlyph->m_Advance;
            f32 Width = m_XScaleFactor * ( f32 )pGlyph->m_Width;
            f32 Height = m_YScaleFactor * m_FontHeight;

            if( 0 == NumEllipsesToDraw )
            {
                if( _Flags & FONTDBG_TRUNCATED )
                {
                    if( m_CursorX + Offset + Width + EllipsesPixelWidth + m_SlantFactor > _OriginX + _MaxPixelWidth )
                    {
                        NumEllipsesToDraw = 3;
                        continue;
                    }
                }
            }

            m_CursorX += Offset;
            f32 X4 = m_CursorX;
            f32 X1 = X4 + m_SlantFactor;
            f32 X3 = X4 + Width;
            f32 X2 = X1 + Width;
            f32 Y1 = m_CursorY;
            f32 Y3 = Y1 + Height;
            f32 Y2 = Y1;
            f32 Y4 = Y3;

            if( m_Rotate )
            {
                RotatePoint( &X1, &Y1, _OriginX, _OriginY );
                RotatePoint( &X2, &Y2, _OriginX, _OriginY );
                RotatePoint( &X3, &Y3, _OriginX, _OriginY );
                RotatePoint( &X4, &Y4, _OriginX, _OriginY );
            }

            m_CursorX += Advance;

            DWORD ChannelSelector = pGlyph->m_Mask;
            ChannelSelector = ((ChannelSelector&0xF000)<<(24-12))|((ChannelSelector&0xF00)<<(16-8))|
                ((ChannelSelector&0xF0)<<(8-4))|(ChannelSelector&0xF);

            ChannelSelector *= 0x11;

            DWORD tu1 = pGlyph->m_tu1;
            DWORD tv1 = pGlyph->m_tv1;
            DWORD tu2 = pGlyph->m_tu2;
            DWORD tv2 = pGlyph->m_tv2;

            pVertex[0] = X1;
            pVertex[1] = Y1;
            reinterpret_cast<volatile DWORD *>(pVertex)[2] = (tu1<<16)|tv1;
            reinterpret_cast<volatile DWORD *>(pVertex)[3] = ChannelSelector;
            pVertex[4] = X2;
            pVertex[5] = Y2;
            reinterpret_cast<volatile DWORD *>(pVertex)[6] = (tu2<<16)|tv1;
            reinterpret_cast<volatile DWORD *>(pVertex)[7] = ChannelSelector;
            pVertex[8] = X3;
            pVertex[9] = Y3;
            reinterpret_cast<volatile DWORD *>(pVertex)[10] = (tu2<<16)|tv2;
            reinterpret_cast<volatile DWORD *>(pVertex)[11] = ChannelSelector;
            pVertex[12] = X4;
            pVertex[13] = Y4;
            reinterpret_cast<volatile DWORD *>(pVertex)[14] = (tu1<<16)|tv2;
            reinterpret_cast<volatile DWORD *>(pVertex)[15] = ChannelSelector;
            pVertex+=16;

            if( NumEllipsesToDraw )
            {
                if( --NumEllipsesToDraw == 0 )
                    break;
            }

            NumChars--;
        }

        while( NumChars )
        {
            pVertex[0] = 0;
            pVertex[1] = 0;
            pVertex[2] = 0;
            pVertex[3] = 0;
            pVertex[4] = 0;
            pVertex[5] = 0;
            pVertex[6] = 0;
            pVertex[7] = 0;
            pVertex[8] = 0;
            pVertex[9] = 0;
            pVertex[10] = 0;
            pVertex[11] = 0;
            pVertex[12] = 0;
            pVertex[13] = 0;
            pVertex[14] = 0;
            pVertex[15] = 0;
            pVertex+=16;
            NumChars--;
        }

        m_pD3dDevice->EndVertices();

        m_CursorX -= Winx;
        m_CursorY -= Winy;

        End();
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::End()
    {
        ITF_ASSERT( m_NestedBeginCount > 0 );
        if( --m_NestedBeginCount > 0 )
            return;

        if( m_bSaveState )
        {
            static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->SetTextureBind( 0, NULL ); // Unbind previous texture first.
            m_pD3dDevice->SetTexture( 0, NULL );
            m_pD3dDevice->SetVertexDeclaration( NULL );
            m_pD3dDevice->SetVertexShader( NULL );
            m_pD3dDevice->SetPixelShader( NULL );
            m_pD3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, m_SavedState[ SAVEDSTATE_D3DRS_ALPHABLENDENABLE ] );
            m_pD3dDevice->SetRenderState( D3DRS_SRCBLEND, m_SavedState[ SAVEDSTATE_D3DRS_SRCBLEND ] );
            m_pD3dDevice->SetRenderState( D3DRS_DESTBLEND, m_SavedState[ SAVEDSTATE_D3DRS_DESTBLEND ] );
            m_pD3dDevice->SetRenderState( D3DRS_BLENDOP, m_SavedState[ SAVEDSTATE_D3DRS_BLENDOP ] );
            m_pD3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, m_SavedState[ SAVEDSTATE_D3DRS_ALPHATESTENABLE ] );
            m_pD3dDevice->SetRenderState( D3DRS_ALPHAREF, m_SavedState[ SAVEDSTATE_D3DRS_ALPHAREF ] );
            m_pD3dDevice->SetRenderState( D3DRS_ALPHAFUNC, m_SavedState[ SAVEDSTATE_D3DRS_ALPHAFUNC ] );
            m_pD3dDevice->SetRenderState( D3DRS_FILLMODE, m_SavedState[ SAVEDSTATE_D3DRS_FILLMODE ] );
            m_pD3dDevice->SetRenderState( D3DRS_CULLMODE, m_SavedState[ SAVEDSTATE_D3DRS_CULLMODE ] );
            m_pD3dDevice->SetRenderState( D3DRS_ZENABLE, m_SavedState[ SAVEDSTATE_D3DRS_ZENABLE ] );
            m_pD3dDevice->SetRenderState( D3DRS_STENCILENABLE, m_SavedState[ SAVEDSTATE_D3DRS_STENCILENABLE ] );
            m_pD3dDevice->SetRenderState( D3DRS_VIEWPORTENABLE, m_SavedState[ SAVEDSTATE_D3DRS_VIEWPORTENABLE ] );
            m_pD3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, m_SavedState[ SAVEDSTATE_D3DSAMP_MINFILTER ] );
            m_pD3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, m_SavedState[ SAVEDSTATE_D3DSAMP_MAGFILTER ] );
            m_pD3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, m_SavedState[ SAVEDSTATE_D3DSAMP_ADDRESSU ] );
            m_pD3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, m_SavedState[ SAVEDSTATE_D3DSAMP_ADDRESSV ] );
        }
    }

    //----------------------------------------------------------------------------//

    LPDIRECT3DTEXTURE9 Fontdebug::CreateTexture( const WCHAR* strText, D3DCOLOR _BackgroundColor,
        D3DCOLOR _TextColor, D3DFORMAT _d3dFormat )
    {
        if( FALSE == XGIsTiledFormat( _d3dFormat ) )
        {
            return NULL;
        }

        f32 TexWidth, TexHeight;
        GetTextExtent( strText, &TexWidth, &TexHeight );
        DWORD Width = XGNextMultiple( ( DWORD )TexWidth, 32 );
        DWORD Height = XGNextMultiple( ( DWORD )TexHeight, 32 );

        LPDIRECT3DSURFACE9 pNewRenderTarget;
        if( FAILED( m_pD3dDevice->CreateRenderTarget( Width, Height, _d3dFormat, D3DMULTISAMPLE_NONE,
            0L, FALSE, &pNewRenderTarget, NULL ) ) )
        {
            return NULL;
        }

        LPDIRECT3DTEXTURE9 pNewTexture;
        if( FAILED( m_pD3dDevice->CreateTexture( Width, Height, 1, 0L, _d3dFormat,
            D3DPOOL_DEFAULT, &pNewTexture, NULL ) ) )
        {
            pNewRenderTarget->Release();
            return NULL;
        }

        LPDIRECT3DSURFACE9 pOldRenderTarget;
        m_pD3dDevice->GetRenderTarget( 0, &pOldRenderTarget );

        static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->setInternalRenderTarget(pNewRenderTarget, NULL);
        m_pD3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, _BackgroundColor, 1.0f, 0L );

        D3DRECT rcSaved;
        GetWindow(rcSaved);
        SetWindow( 0, 0, Width, Height );
        DrawText( Width * 0.5f, Height * 0.5f, _TextColor, strText, FONTDBG_CENTER_X | FONTDBG_CENTER_Y );

        m_pD3dDevice->Resolve( 0L, NULL, pNewTexture, NULL, 0, 0, NULL, 0, 0, NULL );

		static_cast<GFXAdapter_Directx9 *>(GFX_ADAPTER)->setInternalRenderTarget(pOldRenderTarget, NULL);
        pOldRenderTarget->Release();
        pNewRenderTarget->Release();
        SetWindow(rcSaved);

        return pNewTexture;
    }

    //----------------------------------------------------------------------------//

    void Fontdebug::RotatePoint( f32* _X, f32* _Y, DOUBLE _OriginX, DOUBLE _OriginY ) const
    {
        DOUBLE TempX = *_X - _OriginX;
        DOUBLE TempY = *_Y - _OriginY;
        DOUBLE Xprime = _OriginX + ( m_RotCos * TempX - m_RotSin * TempY );
        DOUBLE Yprime = _OriginY + ( m_RotSin * TempX + m_RotCos * TempY );

        *_X = static_cast<f32>( Xprime );
        *_Y = static_cast<f32>( Yprime );
    }

} // namespace ITF

