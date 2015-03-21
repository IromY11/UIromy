#include "precompiled_GFXAdapter_Directx9.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#include "Resource_x360.h"
#include <xgraphics.h>

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

namespace ITF
{

/// Magic values to identify XPR files.
struct XPR_HEADER
{
    DWORD m_Magic;
    DWORD m_HeaderSize;
    DWORD m_DataSize;
};

const DWORD XPR2_MAGIC_VALUE = 0x58505232;

const DWORD eXALLOCAllocatorId_AtgResource = eXALLOCAllocatorId_GameMax;

//----------------------------------------------------------------------------//

PackedResource::PackedResource()
{
    m_pSysMemData = NULL;
    m_SysMemDataSize = 0L;
    m_pVidMemData = NULL;
    m_VidMemDataSize = 0L;
    m_pResourceTags = NULL;
    m_NumResourceTags = 0L;
    m_Initialized = bfalse;
}

//----------------------------------------------------------------------------//

PackedResource::~PackedResource()
{
    Destroy();
}

//----------------------------------------------------------------------------//

void* PackedResource::GetData( const CHAR* strName ) const
{
    if( NULL == m_pResourceTags || NULL == strName )
        return NULL;

    for( DWORD i = 0; i < m_NumResourceTags; i++ )
    {
        if( !_stricmp( strName, m_pResourceTags[i].strName ) )
        {
            return &m_pSysMemData[m_pResourceTags[i].dwOffset];
        }
    }

    return NULL;
}
//----------------------------------------------------------------------------//


static __forceinline void* AllocateContiguousMemory( DWORD Size, DWORD Alignment,
                                                     DWORD Protection = XALLOC_MEMPROTECT_WRITECOMBINE )
{
    return XMemAlloc( Size, MAKE_XALLOC_ATTRIBUTES( 0, 0, 0, 0, eXALLOCAllocatorId_AtgResource,
                                                    Alignment, Protection, 0,
                                                    XALLOC_MEMTYPE_PHYSICAL ) );
}
//----------------------------------------------------------------------------//


static __forceinline void FreeContiguousMemory( void* pData )
{
    return XMemFree( pData, MAKE_XALLOC_ATTRIBUTES( 0, 0, 0, 0, eXALLOCAllocatorId_AtgResource,
                                                    0, 0, 0, XALLOC_MEMTYPE_PHYSICAL ) );
}

//----------------------------------------------------------------------------//

HRESULT PackedResource::Create( const Path& filename )
{
    File*  binaryFile = FILEMANAGER->openFile(filename ,ITF_FILE_ATTR_READ);

    ITF_ASSERT(binaryFile);
    if( binaryFile == NULL )
    {
        return E_FAIL;
    }

    XPR_HEADER xprh;
    
    if ( !binaryFile->read(&xprh, sizeof( XPR_HEADER )))
    {
        FILEMANAGER->closeFile(binaryFile);
        return E_FAIL;
    }

    if( xprh.m_Magic != XPR2_MAGIC_VALUE )
    {
        FILEMANAGER->closeFile(binaryFile);
        return E_FAIL;
    }

    m_SysMemDataSize = xprh.m_HeaderSize;
    m_VidMemDataSize = xprh.m_DataSize;

    m_pSysMemData = newAlloc(mId_Resource, u8[m_SysMemDataSize]);
    if( m_pSysMemData == NULL )
    {
        m_SysMemDataSize = 0;
        return E_FAIL;
    }
    m_pVidMemData = ( u8* )AllocateContiguousMemory( m_VidMemDataSize, XALLOC_PHYSICAL_ALIGNMENT_4K );
    if( m_pVidMemData == NULL )
    {
        m_SysMemDataSize = 0;
        m_VidMemDataSize = 0;
        delete[] m_pSysMemData;
        m_pSysMemData = NULL;
        return E_FAIL;
    }

    u8* pBuffer = newAlloc(mId_Temporary, u8[m_VidMemDataSize]);

    binaryFile->read(  m_pSysMemData, m_SysMemDataSize );
    binaryFile->read(  pBuffer, m_VidMemDataSize );

    ITF_MemcpyWriteCombined((DWORD*)m_pVidMemData,pBuffer,m_VidMemDataSize);
    SF_DEL_ARRAY(pBuffer);

    FILEMANAGER->closeFile(binaryFile);

    m_NumResourceTags = *( DWORD* )( m_pSysMemData + 0 );
    m_pResourceTags = ( RESOURCE* )( m_pSysMemData + 4 );

    for( DWORD i = 0; i < m_NumResourceTags; i++ )
    {
        m_pResourceTags[i].strName = ( CHAR* )( m_pSysMemData + ( DWORD )m_pResourceTags[i].strName );

        // Fixup the texture memory
        if( ( m_pResourceTags[i].dwType & 0xffff0000 ) == ( RESOURCETYPE_TEXTURE & 0xffff0000 ) )
        {
            D3DTexture* pTexture = ( D3DTexture* )&m_pSysMemData[m_pResourceTags[i].dwOffset];

            XGOffsetBaseTextureAddress( pTexture, m_pVidMemData, m_pVidMemData );

            //PIXSetTextureName(pTexture, m_pResourceTags[i].strName);
        }
    }

    m_Initialized = btrue;

    return S_OK;
}

//----------------------------------------------------------------------------//

void PackedResource::GetResourceTags( DWORD* _pNumResourceTags,
                                      RESOURCE** _ppResourceTags ) const
{
    if( _pNumResourceTags )
        ( *_pNumResourceTags ) = m_NumResourceTags;

    if( _ppResourceTags )
        ( *_ppResourceTags ) = m_pResourceTags;
}

//----------------------------------------------------------------------------//

void PackedResource::Destroy()
{
    delete[] m_pSysMemData;
    m_pSysMemData = NULL;
    m_SysMemDataSize = 0L;

    if( m_pVidMemData != NULL )
        FreeContiguousMemory( m_pVidMemData );
    m_pVidMemData = NULL;
    m_VidMemDataSize = 0L;

    m_pResourceTags = NULL;
    m_NumResourceTags = 0L;

    m_Initialized = bfalse;
}

//----------------------------------------------------------------------------//

BOOL PackedResource::Initialized() const
{
    return m_Initialized;
}

} // namespace ITF
