#ifndef _ITF_MESH3DRESOURCE_H_
#define _ITF_MESH3DRESOURCE_H_

#ifndef _ITF_MESH3D_H_
#include "engine/display/Mesh3D.h"
#endif

namespace ITF
{

    class Mesh3D;
///////////////////////////////////////////////////////////////////////////////////////////////
// 
// Mesh3DResource class
// 
///////////////////////////////////////////////////////////////////////////////////////////////
class Mesh3DResource :  public Resource
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Mesh3DResource, Resource,2801724095)

public:

    ResourceType	getType()       const                       { return ResourceType_Mesh3D;}
    Mesh3D*			getMesh3D()									{ return m_mesh3D;}
    void            setMesh3D(Mesh3D* _p_Mesh)					{ m_mesh3D = _p_Mesh;}

    void            setCanLoadBin(bbool _canLoadBin)            { m_canLoadBin = _canLoadBin;}
    bbool           getCanLoadBin(void) const                   { return m_canLoadBin;} 

    bbool			tryLoadFileByPlatform(const Platform& _platform);
    void            prepareIndexAndVertexBuffer();

    bbool			loadFile(const Path& _path, const Platform& _szPlatform, bbool _tryLoadingBin, bbool raiseError);
    void			postProcess(const Path& _path, const Platform& _szPlatform, bbool _tryLoadingBin);

    virtual bbool	needsUpdate() const { return btrue; }

    void			SetMaterial(ITF_Mesh &_mesh, const ITF_VECTOR<GFXMaterialSerializable> & _materialInstanceList, const ITF_VECTOR<GFXMaterialSerializable> & _materialTplList);
	void			ChangeMaterial(ITF_Mesh &_mesh, u32 elementIndex, u32 materialID, const ITF_VECTOR<GFXMaterialSerializable> & _materialInstanceList, const ITF_VECTOR<GFXMaterialSerializable> & _materialTplList);
    void			DetachMaterial(ITF_Mesh &_mesh);

    ITF_IndexBuffer*    getIndexBuffer(u32 _index)              {return m_indexBuffer[_index];}
    ITF_VertexBuffer*   getVertexBuffer()                       {return m_vertexBuffer;}
    u32                 getIndexBufferCount() const             {return m_indexBuffer.size();} 

private:
    friend class ResourceLoader; // Don't add more friends here. You MUST use RESOURCE_MANAGER to instancate a resource

    ArchiveLinker*          m_archiveLinker;


    ITF_VECTOR<ITF_IndexBuffer*>    m_indexBuffer;
    ITF_VertexBuffer*               m_vertexBuffer;

    Mesh3DResource(const Path &path);
    virtual ~Mesh3DResource();
    void    flushPhysicalData();
    bbool   tryLoadFile();

    Mesh3D			*m_mesh3D;
    bbool           m_canLoadBin;


#ifdef ITF_SUPPORT_COOKING
public:
    virtual bbool mustBeCooked()     const   {return btrue;}
    typedef bbool (*cookFileCallback) (const Platform&/*_szPlatform*/,const Path& /*_filename*/);
    static cookFileCallback	m_cookCallback;
#endif //ITF_SUPPORT_COOKING

#ifdef ITF_SUPPORT_EDITOR
public:
    static bbool isValidFilename( const Path &_filename );
#endif // ITF_SUPPORT_EDITOR
};

}

#endif //_ITF_MESH3DRESOURCE_H_
