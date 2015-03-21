#pragma once
#ifndef RESOURCE_X360_H
#define RESOURCE_X360_H

namespace ITF
{

class Path;

struct RESOURCE
{
    DWORD dwType;
    DWORD dwOffset;
    DWORD dwSize;
    CHAR* strName;
};

enum
{
    RESOURCETYPE_USERDATA       = ( ( 'U' << 24 ) | ( 'S' << 16 ) | ( 'E' << 8 ) | ( 'R' ) ),
    RESOURCETYPE_TEXTURE        = ( ( 'T' << 24 ) | ( 'X' << 16 ) | ( '2' << 8 ) | ( 'D' ) ),
    RESOURCETYPE_CUBEMAP        = ( ( 'T' << 24 ) | ( 'X' << 16 ) | ( 'C' << 8 ) | ( 'M' ) ),
    RESOURCETYPE_VOLUMETEXTURE  = ( ( 'T' << 24 ) | ( 'X' << 16 ) | ( '3' << 8 ) | ( 'D' ) ),
    RESOURCETYPE_VERTEXBUFFER   = ( ( 'V' << 24 ) | ( 'B' << 16 ) | ( 'U' << 8 ) | ( 'F' ) ),
    RESOURCETYPE_INDEXBUFFER    = ( ( 'I' << 24 ) | ( 'B' << 16 ) | ( 'U' << 8 ) | ( 'F' ) ),
    RESOURCETYPE_EOF            = 0xffffffff
};


class PackedResource
{
protected:
    u8* m_pSysMemData;
    DWORD m_SysMemDataSize;

    u8* m_pVidMemData;
    DWORD m_VidMemDataSize;

    RESOURCE* m_pResourceTags;
    DWORD m_NumResourceTags;
    bbool m_Initialized;

public:
    HRESULT Create( const Path& filename );

    VOID    Destroy();

    BOOL    Initialized() const;

    VOID    GetResourceTags( DWORD* _pNumResourceTags, RESOURCE** _ppResourceTags ) const;

    D3DResource* RegisterResource( D3DResource* pResource ) const
    {
        return pResource;
    }

    VOID* GetData( DWORD dwOffset ) const
    {
        return &m_pSysMemData[dwOffset];
    }

    D3DResource* GetResource( DWORD dwOffset ) const
    {
        return RegisterResource( ( D3DResource* )GetData( dwOffset ) );
    }

    D3DTexture* GetTexture( DWORD dwOffset ) const
    {
        return ( D3DTexture* )GetResource( dwOffset );
    }

    D3DArrayTexture* GetArrayTexture( DWORD dwOffset ) const
    {
        return ( D3DArrayTexture* )GetResource( dwOffset );
    }

    D3DCubeTexture* GetCubemap( DWORD dwOffset ) const
    {
        return ( D3DCubeTexture* )GetResource( dwOffset );
    }

    D3DVolumeTexture* GetVolumeTexture( DWORD dwOffset ) const
    {
        return ( D3DVolumeTexture* )GetResource( dwOffset );
    }

    D3DVertexBuffer* GetVertexBuffer( DWORD dwOffset ) const
    {
        return ( D3DVertexBuffer* )GetResource( dwOffset );
    }

    VOID* GetData( const CHAR* strName ) const;

    D3DResource* GetResource( const CHAR* strName ) const
    {
        return RegisterResource( ( D3DResource* )GetData( strName ) );
    }

    D3DTexture* GetTexture( const CHAR* strName ) const
    {
        return ( D3DTexture* )GetResource( strName );
    }

    D3DArrayTexture* GetArrayTexture( const CHAR* strName ) const
    {
        return ( D3DArrayTexture* )GetResource( strName );
    }

    D3DCubeTexture* GetCubemap( const CHAR* strName ) const
    {
        return ( D3DCubeTexture* )GetResource( strName );
    }

    D3DVolumeTexture* GetVolumeTexture( const CHAR* strName ) const
    {
        return ( D3DVolumeTexture* )GetResource( strName );
    }

    D3DVertexBuffer* GetVertexBuffer( const CHAR* strName ) const
    {
        return ( D3DVertexBuffer* )GetResource( strName );
    }

            PackedResource();
            ~PackedResource();
};

} // namespace ITF

#endif // RESOURCE_X360_H
