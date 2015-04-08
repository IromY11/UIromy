#include "precompiled_engine.h"

#ifndef _ITF_ATLASGRAPHICCOMPONENT_H_
#include "engine/actors/components/AtlasGraphicComponent.h"
#endif //_ITF_ATLASGRAPHICCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_NGON2D_H_
#include "Core/math/Ngon2d.h"
#endif // _ITF_NGON2D_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AtlasGraphicComponent_Template)

    BEGIN_SERIALIZATION_CHILD(AtlasGraphicComponent_Template)
        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
            SERIALIZE_MEMBER("texture", m_material.getTexturePathSet().getTexturePath());
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT("material", m_material);
        SERIALIZE_ENUM_BEGIN("textureLayer",m_textureLayer); 
            SERIALIZE_ENUM_VAR(TEXSET_ID_DIFFUSE);
            SERIALIZE_ENUM_VAR(TEXSET_ID_BACK_LIGHT);
            SERIALIZE_ENUM_VAR(TEXSET_ID_NORMAL);
            SERIALIZE_ENUM_VAR(TEXSET_ID_SEPARATE_ALPHA);
            SERIALIZE_ENUM_VAR(TEXSET_ID_DIFFUSE_2);
            SERIALIZE_ENUM_VAR(TEXSET_ID_BACK_LIGHT_2);
            SERIALIZE_ENUM_VAR(TEXSET_ID_SPECULAR);
            SERIALIZE_ENUM_VAR(TEXSET_ID_COLORMASK);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("texelRatio", m_texelRatio);
    END_SERIALIZATION()

    AtlasGraphicComponent_Template::AtlasGraphicComponent_Template()
        : Super()
        , m_textureLayer(TEXSET_ID_DIFFUSE)
        , m_texelRatio(Vec2d::One)
    {
    }

    AtlasGraphicComponent_Template::~AtlasGraphicComponent_Template()
    {
    }

    bbool AtlasGraphicComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);

        if (!m_material.getTexturePathSet().getTexturePath().isEmpty())
            m_material.onLoaded(m_actorTemplate->getResourceContainer());

        return bOk;
    }

    void AtlasGraphicComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        if (!m_material.getTexturePathSet().getTexturePath().isEmpty())
            m_material.onUnLoaded(m_actorTemplate->getResourceContainer());

        Super::onTemplateDelete(_hotReload);
    }

    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(AtlasGraphicComponent)

    BEGIN_SERIALIZATION_CHILD(AtlasGraphicComponent)
        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
            SERIALIZE_MEMBER("texture", m_materialSerializable.getTexturePathSet().getTexturePath());
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT("material", m_materialSerializable);
        SERIALIZE_ENUM_BEGIN("textureLayer",m_textureLayerSerializable);
            SERIALIZE_ENUM_VAR(TEXSET_ID_DIFFUSE)
            SERIALIZE_ENUM_VAR(TEXSET_ID_BACK_LIGHT)
            SERIALIZE_ENUM_VAR(TEXSET_ID_NORMAL)
            SERIALIZE_ENUM_VAR(TEXSET_ID_SEPARATE_ALPHA)
            SERIALIZE_ENUM_VAR(TEXSET_ID_DIFFUSE_2)
            SERIALIZE_ENUM_VAR(TEXSET_ID_BACK_LIGHT_2)
            SERIALIZE_ENUM_VAR(TEXSET_ID_SPECULAR)
            SERIALIZE_ENUM_VAR(TEXSET_ID_COLORMASK)
        SERIALIZE_ENUM_END()
        SERIALIZE_MEMBER("atlasIndex", m_atlasIndex);
        SERIALIZE_MEMBER("extrudeFactor", m_extrudeFactor);
        SERIALIZE_MEMBER("offset", m_offset);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(AtlasGraphicComponent)
    END_VALIDATE_COMPONENT()

    AtlasGraphicComponent::AtlasGraphicComponent()
        : Super()
        , m_textureLayerSerializable(TEXSET_ID_DIFFUSE)
        , m_textureLayer(TEXSET_ID_DIFFUSE)
        , m_atlasIndex(0)
        , m_extrudeFactor(1.f)
        , m_indexBuffer(NULL)
        , m_offset(Vec3d::Zero)
    {
    }

    AtlasGraphicComponent::~AtlasGraphicComponent()
    {
    }


    void AtlasGraphicComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        if (m_materialSerializable.getTexturePathSet().getTexturePath().isEmpty())
        {
            setMaterial(getTemplate()->getMaterial());
            m_textureLayer = getTemplate()->getTextureLayer();
        }
        else
        {
            m_materialSerializable.onLoaded(m_actor->getResourceContainer());
            setMaterial(m_materialSerializable);
            m_textureLayer = m_textureLayerSerializable;
        }
    }

    void AtlasGraphicComponent::updateAABB()
    {
        AABB computeAABB = m_aabb;
        computeAABB.Scale(m_actor->getScale());
        computeAABB.Rotate(m_actor->getAngle());
        computeAABB.Translate(getFinalPos().truncateTo2D());

        m_actor->growAABB3d(AABB3d(computeAABB.getMin().to3d(m_zMinMax.x() * m_extrudeFactor  + m_actor->getDepth() + m_offset.z()), 
                                   computeAABB.getMax().to3d(m_zMinMax.y() * m_extrudeFactor  + m_actor->getDepth() + m_offset.z())));
    }

    void AtlasGraphicComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

        updateAABB();
    }

    void AtlasGraphicComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);
    }

    // rectangle 4 points, 6 indexes
    void AtlasGraphicComponent::fillRectangle(ITF_VECTOR<VertexPCT> & _ptList, ITF_VECTOR<u16> & _idxList, const UVdata * pUvData, const UVparameters * pUvParam)
    {
        ITF_ASSERT_CRASH(pUvData->getUVs().size() == 2, "bad sub function");
        bbool useParams = pUvParam && (pUvParam->getParams().size() == pUvData->getUVs().size());
        Vec2d   texelRatio = getTemplate()->getTexelRatio();

        Vec2d p0  = pUvData->getUVs()[0];
        Vec2d p1  = pUvData->getUVs()[1];
        Vec2d p01(p0.x(), p1.y());
        Vec2d p10(p1.x(), p0.y());

        f32   z0  = useParams ? pUvParam->getParamAt(0).m_depth : 0.f;
        f32   z1  = useParams ? pUvParam->getParamAt(1).m_depth : 0.f;
        f32   zxx = (z0 + z1)*0.5f;

        u32 color = 0xFFFFFFFF;
        Vec2d center = (p0 + p1)*texelRatio*0.5f;

        _ptList.resize(4);
        _ptList[0].setData((p0*texelRatio - center).to3d(z0), p0, color);
        _ptList[1].setData((p01*texelRatio - center).to3d(zxx), p01, color);
        _ptList[2].setData((p10*texelRatio - center).to3d(zxx), p10, color);
        _ptList[3].setData((p1*texelRatio - center).to3d(z1), p1, color);


        _idxList.resize(6);
        _idxList[0] = 0;
        _idxList[1] = 1;
        _idxList[2] = 2;
        _idxList[3] = 3;
        _idxList[4] = 2;
        _idxList[5] = 1;
    }

    void AtlasGraphicComponent::fillVertex(ITF_VECTOR<VertexPCT> & _ptList, const UVdata * pUvData, const UVparameters * pUvParam)
    {
        bbool useParams = pUvParam && (pUvParam->getParams().size() == pUvData->getUVs().size());
        u32   color     = 0xFFFFFFFF;

        Vec3d                               center(Vec3d::Zero);
        u32                                 idx;
        ITF_VECTOR<Vec2d>::const_iterator   uvIter;
        Vec2d   texelRatio = getTemplate()->getTexelRatio();

        for (idx = 0, uvIter = pUvData->getUVs().begin();
            uvIter != pUvData->getUVs().end(); uvIter++)
        {
            center += (*uvIter * texelRatio).to3d(useParams ? pUvParam->getParamAt(idx++).m_depth : 0.f);
        }
        center *= f32_Inv((f32)pUvData->getUVs().size());


        m_aabb.invalidate();
        m_zMinMax.x()   = F32_INFINITY;
        m_zMinMax.y()   = -F32_INFINITY;

        _ptList.resize(pUvData->getUVs().size());
        ITF_VECTOR<VertexPCT>::iterator ptListIter = _ptList.begin();
        for (idx = 0, uvIter = pUvData->getUVs().begin();
            uvIter != pUvData->getUVs().end(); uvIter++, ptListIter++)
        {
            ptListIter->m_pos   = (*uvIter * texelRatio).to3d(useParams ? pUvParam->getParamAt(idx++).m_depth : 0.f) - center;
            ptListIter->m_uv    = (*uvIter);
            ptListIter->m_color = color;

            m_aabb.grow(ptListIter->m_pos.truncateTo2D());
            if (m_zMinMax.x() > ptListIter->m_pos.z())
                m_zMinMax.x() = ptListIter->m_pos.z();
            if (m_zMinMax.y() < ptListIter->m_pos.z())
                m_zMinMax.y() = ptListIter->m_pos.z();
        }    
    }

    // triangles already set
    void AtlasGraphicComponent::fillTriangles(ITF_VECTOR<VertexPCT> & _ptList, ITF_VECTOR<u16> & _idxList, const UVdata * pUvData, const UVparameters * pUvParam)
    {
        ITF_ASSERT_CRASH(pUvParam && pUvParam->getTriangles().size(), "bad sub function");
        
        fillVertex(_ptList, pUvData, pUvParam);
        
        _idxList.resize(pUvParam->getTriangles().size()*3);
        ITF_VECTOR<u16>::iterator ptIdxIter = _idxList.begin();
        for (ITF_VECTOR<UVparameters::Triangle>::const_iterator triangleIt = pUvParam->getTriangles().begin();
            triangleIt != pUvParam->getTriangles().end(); triangleIt++)
        {
            *(ptIdxIter++) = (u16)triangleIt->m_index[0];
            *(ptIdxIter++) = (u16)triangleIt->m_index[1];
            *(ptIdxIter++) = (u16)triangleIt->m_index[2];
        }
    }


    void AtlasGraphicComponent::fillPointList(ITF_VECTOR<VertexPCT> & _ptList, ITF_VECTOR<u16> & _idxList, const UVdata * pUvData, const UVparameters * pUvParam)
    {
        fillVertex(_ptList, pUvData, pUvParam);

        _idxList.resize(pUvData->getUVs().size() * 4);
        u32 nbIndex = FillNgon(const_cast<Vec2d*>(&pUvData->getUVs()[0]), pUvData->getUVs().size(),&_idxList[0]) * 3;
        _idxList.resize(nbIndex);

    }


    void AtlasGraphicComponent::clearMesh()
    {
        GFX_ADAPTER->removeIndexBuffer( m_indexBuffer );
        m_mesh.removeVertexBuffer(0);

        m_indexBuffer = NULL;
    }

    void AtlasGraphicComponent::createMesh()
    {
        Texture * pTexture = getMaterial().getTextureSet().getTexture(m_textureLayer);
        if (!pTexture)
            return;

        const UVAtlas * pAtlas = pTexture->getUVAtlas();
        if (!pAtlas)
            return;

        const u32 uvDataCount = pAtlas->getNumberUV();
        if (!uvDataCount)
            return;

        m_atlasIndex = Min( m_atlasIndex, uvDataCount -1 );

        const UVdata       * pUvData  = &pAtlas->getUVDatabyIndex( m_atlasIndex );
        const UVparameters * pUvParam = pAtlas->getUvParameterByIndex( m_atlasIndex );

        const u32 uvsListCount = pUvData->getUVs().size();
        if ( uvsListCount == 0 )
            return;

        ITF_VECTOR<VertexPCT> ptList;
        ITF_VECTOR<u16>       idxList;
        if ( uvsListCount == 2 )
        {
            fillRectangle(ptList, idxList, pUvData, pUvParam);
        }
        else if ( pUvParam && pUvParam->getTriangles().size() )
        {
            fillTriangles(ptList, idxList, pUvData, pUvParam);
        }
        else
        {
            fillPointList(ptList, idxList, pUvData, pUvParam);
        }

        m_indexBuffer = GFX_ADAPTER->createIndexBuffer(  idxList.size(), bfalse);
        u16        *idxBuff;
        m_indexBuffer->Lock((void **) &idxBuff);
        ITF_Memcpy(idxBuff, &idxList[0], idxList.size() * sizeof(u16));
        m_indexBuffer->Unlock();

        m_mesh.removeVertexBuffer(0);
        m_mesh.createVertexBuffer(ptList.size(), VertexFormat_PCT, sizeof(VertexPCT), vbLockType_static);
        m_mesh.addElementAndMaterial(getMaterial());
        m_mesh.getMeshElement().m_indexBuffer = m_indexBuffer;
        m_mesh.getMeshElement().m_count       = idxList.size();

        VertexPCT    *pdata;
        m_mesh.LockVertexBuffer((void **) &pdata);
        ITF_Memcpy(pdata, &ptList[0], ptList.size() * sizeof(VertexPCT));
        m_mesh.UnlockVertexBuffer();
    }

    void AtlasGraphicComponent::onResourceLoaded()
    {
        clearMesh();
        createMesh();
    }


    void AtlasGraphicComponent::onUnloadResources()
    {
       clearMesh();
    }

    void AtlasGraphicComponent::onPostPropertyChange()
    {
        clearMesh();
        createMesh();
    }

    void AtlasGraphicComponent::batchPrimitives( const ITF_VECTOR<class View*>& _views )
    {
        if (!m_indexBuffer)
            return;

        Matrix44 mat;
        mat.setIdentity();
        mat.mulScale( m_actor->getScale().to3d(m_extrudeFactor));
        
        Matrix44 rot;
        rot.setRotationZ(m_actor->getAngle());

        mat.mul33_inline(mat, rot);
        mat.setTranslation(getFinalPos());

        m_mesh.setMatrix(mat);
        m_mesh.setCommonParam(getGfxPrimitiveParam());

        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_mesh, m_actor->getDepth() + getDepthOffset(), GetActor()->getRef());
    }


}
