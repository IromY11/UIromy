#include "precompiled_engine.h"

#ifndef _ITF_STATICMESHVERTEXCOMPONENT_H_
#include "engine/actors/components/StaticMeshVertexComponent.h"
#endif //_ITF_STATICMESHVERTEXCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifdef ITF_SUPPORT_EDITOR
#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_
#endif

//#define ALLOW_FCG_AUTO_UPDATE

namespace ITF
{
    BEGIN_SERIALIZATION(StaticMeshElement)
        SERIALIZE_MEMBER("pos", m_pos);
        SERIALIZE_MEMBER("color", m_color);
        SERIALIZE_MEMBER("animated", m_animated);
        SERIALIZE_MEMBER("frisePath", m_frisePath);
        SERIALIZE_CONTAINER("staticIndexList", m_staticIndexList);
        SERIALIZE_CONTAINER_OBJECT("staticVertexList", m_staticVertexList);
    END_SERIALIZATION()



    IMPLEMENT_OBJECT_RTTI(StaticMeshVertexComponent_Template)

    BEGIN_SERIALIZATION_CHILD(StaticMeshVertexComponent_Template)
    END_SERIALIZATION()

    StaticMeshVertexComponent_Template::StaticMeshVertexComponent_Template()
        : Super()
    {
    }

    StaticMeshVertexComponent_Template::~StaticMeshVertexComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(StaticMeshVertexComponent)

    BEGIN_SERIALIZATION_CHILD(StaticMeshVertexComponent)
        SERIALIZE_CONTAINER_OBJECT("staticMeshElements", m_staticMeshElements);
        SERIALIZE_CONTAINER("sortedMeshElements", m_sortedMeshElements);
        SERIALIZE_OBJECT("material", m_material);
        SERIALIZE_OBJECT("localAABB", m_localAABB);
        SERIALIZE_MEMBER("mergeRange", m_mergeRange);
        SERIALIZE_MEMBER("friseGroupPath", m_friseGroupPath);
        SERIALIZE_MEMBER("forceMatrixUpdate", m_forceMatrixUpdate);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(StaticMeshVertexComponent)
    END_VALIDATE_COMPONENT()

    StaticMeshVertexComponent::StaticMeshVertexComponent()
        : Super()
        , m_meshNeedUpdate(bfalse)
        , m_meshNeedFullUpdate(bfalse)
        , m_forceMatrixUpdate(bfalse)
        , m_mergeRange(0.1f)
    {
    }

    StaticMeshVertexComponent::~StaticMeshVertexComponent()
    {
    }


    void StaticMeshVertexComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_material.onLoaded(GetActor()->getResourceContainer());

#ifdef ITF_SUPPORT_EDITOR
        Pickable * friseGroup = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, m_friseGroupPath);
        if (friseGroup)
            m_friseGroup = friseGroup->getRef();
        actorDataToSubSceneData();
#endif

        fillStaticMeshFromData();
        computeLocalAABB();
        updateAABB();
    }

#ifdef ITF_SUPPORT_EDITOR
    void StaticMeshVertexComponent::onBecomeActive()
    {
#ifdef ALLOW_FCG_AUTO_UPDATE
        computeFriseGroup(btrue);
#endif //ALLOW_FCG_AUTO_UPDATE
    }
#endif

    void StaticMeshVertexComponent::onActorClearComponents()
    {
        m_material.onUnLoaded(GetActor()->getResourceContainer());

        clearStaticMesh();
    }

#ifdef ITF_SUPPORT_EDITOR
    static int forceUpdateCount = 1;
    static int forceUpdate = 0;

    void StaticMeshVertexComponent::onPrePropertyChange()
    {
        u32 smeSize = m_staticMeshElements.size();
        for (u32 i=0; i<smeSize; i++)
        {
            StaticMeshElement & sme = m_staticMeshElements[i];
            sme.m_posBeforeEdit = sme.m_pos;
        }
    }

     
    void StaticMeshVertexComponent::onPostPropertyChange()
    {
        u32 smeSize = m_staticMeshElements.size();
        for (u32 i=0; i<smeSize; i++)
        {
            StaticMeshElement & sme = m_staticMeshElements[i];
            Vec3d move = sme.m_pos - sme.m_posBeforeEdit;
            if (!move.isNullEpsilon())
                moveElement(i, move);
        }

        if (m_meshNeedFullUpdate)
            onEditorMove();
        
        m_meshNeedUpdate = btrue;
        updateMesh();

        forceUpdate = forceUpdateCount;
    }
#endif //ITF_SUPPORT_EDITOR

    void StaticMeshVertexComponent::updateMesh()
    {
        if (!m_meshNeedUpdate && !m_forceMatrixUpdate)
            return;

        Matrix44 mat;
        mat.setRotationZ(GetActor()->getAngle());
        mat.setTranslation(GetActor()->getPos());
        mat.mulScale(GetActor()->getScale().to3d(1.f));

        for (ITF_VECTOR<ITF_Mesh>::iterator meshIter = m_staticMeshList.begin();
            meshIter != m_staticMeshList.end(); meshIter++)
        {
            if (m_meshNeedUpdate)
            {
                if (meshIter->getNbMaterial() == 0)
                {
                    meshIter->addMaterial(m_material);
                } else
                {
                    meshIter->setMaterial(0, m_material);
                }

                m_primitiveParam.m_BV = m_localAABB;
                meshIter->setCommonParam(m_primitiveParam);
            }

            meshIter->setMatrix(mat);
        }

        m_meshNeedUpdate = bfalse;
    }

#ifdef ITF_SUPPORT_EDITOR
    // Force Link to SubScene group
    void StaticMeshVertexComponent::actorDataToSubSceneData(bbool _init)
    {
        // Force Link to SubScene group
        Pickable * friseGroupPick = (Pickable *)m_friseGroup.getObject();
        if (!friseGroupPick)
            return;

        friseGroupPick->setPos(GetActor()->getPos());
        friseGroupPick->setAngle(GetActor()->getAngle());
        friseGroupPick->setScale(GetActor()->getScale());
        friseGroupPick->setIsFlipped(GetActor()->getIsFlipped());

        if (!_init)
            return;

        friseGroupPick->setWorldInitialPos(GetActor()->getPos(), btrue);
        friseGroupPick->setWorldInitialRot(GetActor()->getAngle(), btrue);
        friseGroupPick->setWorldInitialScale(GetActor()->getScale(), btrue);
        friseGroupPick->setWorldInitialFlip(GetActor()->getIsFlipped(), btrue);
    }

#endif

    void StaticMeshVertexComponent::Update( const f32 _dt )
    {
        PRF_M_SCOPE(updateStaticMeshVertex)

        Super::Update(_dt);

#ifdef ITF_SUPPORT_EDITOR
        actorDataToSubSceneData();
#endif
        updateMesh();
        updateAABB();
    }

    void StaticMeshVertexComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
        Super::batchPrimitives( _views );

#ifdef ITF_SUPPORT_EDITOR
        if (forceUpdate > 0)
        {
            forceUpdate--;
            m_meshNeedFullUpdate = btrue;
            onEditorMove();
        }
#endif

        if (m_staticMeshElements.size())
        {
            ITF_VECTOR<MeshZData>::iterator meshZIter = m_staticMeshZData.begin();
            for (ITF_VECTOR<ITF_Mesh>::iterator meshIter = m_staticMeshList.begin();
                meshIter != m_staticMeshList.end(); meshIter++, meshZIter++)
            {      
                meshIter->getCommonParam()->m_colorFactor.setAlpha(m_alpha);
                GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &(*meshIter), m_actor->getDepth() + getDepthOffset() +  meshZIter->m_z, m_actor->getRef());
            }
        }
    }


    void StaticMeshVertexComponent::clearStaticMesh()
    {
        for (ITF_VECTOR<ITF_Mesh>::iterator meshIter = m_staticMeshList.begin();
            meshIter != m_staticMeshList.end(); meshIter++)
        {
            if (meshIter->getNbMeshElement() > 0)
            {
                ITF_MeshElement & elem = meshIter->getMeshElement();
                GFX_ADAPTER->removeIndexBuffer(elem.m_indexBuffer);
            }
            meshIter->removeVertexBuffer();
        }
        m_staticMeshList.clear();
        m_staticMeshZData.clear();
    }


    inline u32 PixelMul32_2 ( const u32 c0, const u32 c1)
    {
        return	(( ( (c0 & 0xFF000000) >> 16 ) * ( (c1 & 0xFF000000) >> 16 ) ) & 0xFF000000 ) |
                (( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
                (( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
                (( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
    }
    
    void StaticMeshVertexComponent::fillStaticMeshFromData()
    {
        clearStaticMesh();

//        u32 vtxCount      = 0;
//        u32 indexCount    = 0;
        u32 sCount        = m_staticMeshElements.size();

        sortElements();
        MeshZData * currentMeshZData = NULL;
        for (u32 i=0; i<sCount; i++)
        {
            StaticMeshElement & sme = m_staticMeshElements[m_sortedMeshElements[i]];
            if (!currentMeshZData || (sme.m_pos.z() - m_staticMeshZData.back().m_z) > m_mergeRange )
            {
                m_staticMeshZData.emplace_back();
                currentMeshZData = &m_staticMeshZData.back();
                
                currentMeshZData->m_begin       = i;
                currentMeshZData->m_size        = 1;
                currentMeshZData->m_indexSize   = sme.m_staticIndexList.size();
                currentMeshZData->m_vertexSize  = sme.m_staticVertexList.size();
                currentMeshZData->m_z           = sme.m_pos.z();
                currentMeshZData->m_animated	= sme.m_animated;
            } else
            {
                currentMeshZData->m_size++;
                currentMeshZData->m_indexSize  += sme.m_staticIndexList.size();
                currentMeshZData->m_vertexSize += sme.m_staticVertexList.size();
                currentMeshZData->m_animated   |= sme.m_animated;
            }
        }


        m_staticMeshList.resize(m_staticMeshZData.size());
        
        ITF_VECTOR<MeshZData>::iterator meshZIter = m_staticMeshZData.begin();
        for (ITF_VECTOR<ITF_Mesh>::iterator meshIter = m_staticMeshList.begin();
            meshIter != m_staticMeshList.end(); meshIter++, meshZIter++)
        {
            if (meshZIter->m_animated)
            {
                meshIter->createVertexBuffer(meshZIter->m_vertexSize,
                    VertexFormat_PNC3T,
                    sizeof(VertexPNC3T),
                    vbLockType_static,
                    VB_T_MESH);


                VertexPNC3T   *pdata = 0;
                meshIter->LockVertexBuffer((void **) &pdata);
                for (u32 i=meshZIter->m_begin, count=0; count<meshZIter->m_size; i++, count++)
                {
                    StaticMeshElement & sme = m_staticMeshElements[m_sortedMeshElements[i]];
//                    u32 vSize = sme.m_staticVertexList.size();
                    u32 colorMult = sme.m_color.getAsU32();
                    for (ITF_VECTOR<VertexPNC3T>::iterator svIter = sme.m_staticVertexList.begin();
                        svIter != sme.m_staticVertexList.end(); svIter++, pdata++)
                    {
                        *pdata         = *svIter;
                        pdata->m_color = PixelMul32_2(pdata->m_color, colorMult);
                    }
                }
                meshIter->UnlockVertexBuffer();                        
            } else
            {
                meshIter->createVertexBuffer(meshZIter->m_vertexSize,
                    VertexFormat_PCT,
                    sizeof(VertexPCT),
                    vbLockType_static,
                    VB_T_MESH);


                VertexPCT   *pdata = 0;
                meshIter->LockVertexBuffer((void **) &pdata);
                for (u32 i=meshZIter->m_begin, count=0; count<meshZIter->m_size; i++, count++)
                {
                    StaticMeshElement & sme = m_staticMeshElements[m_sortedMeshElements[i]];
//                    u32 vSize = sme.m_staticVertexList.size();
                    u32 colorMult = sme.m_color.getAsU32();
                    for (ITF_VECTOR<VertexPNC3T>::iterator svIter = sme.m_staticVertexList.begin();
                        svIter != sme.m_staticVertexList.end(); svIter++, pdata++)
                    {
                        pdata->setData(svIter->m_pos, svIter->m_uv, PixelMul32_2(svIter->m_color, colorMult));
                    }
                }
                meshIter->UnlockVertexBuffer();                        
            }

            meshIter->addElementAndMaterial( m_material );
            ITF_MeshElement & elem = meshIter->getMeshElement();

            elem.m_indexBuffer  = GFX_ADAPTER->createIndexBuffer(meshZIter->m_indexSize, bfalse);
            elem.m_startVertex  = 0;
            elem.m_count        = meshZIter->m_indexSize;

            u16         *pindexelem     = 0;
            u16         curVertexOffset = 0;
            elem.m_indexBuffer->Lock((void**)&pindexelem);
            for (u32 i=meshZIter->m_begin, count=0; count<meshZIter->m_size; i++, count++)
            {
                StaticMeshElement & sme             = m_staticMeshElements[m_sortedMeshElements[i]];
//                u32                 smeIndexSize    = sme.m_staticIndexList.size();
                for (ITF_VECTOR<u16>::iterator siIter = sme.m_staticIndexList.begin();
                    siIter != sme.m_staticIndexList.end(); siIter++, pindexelem++)
                {
                    *pindexelem = *siIter + curVertexOffset;
                }
                curVertexOffset += u16(sme.m_staticVertexList.size());
            }
            elem.m_indexBuffer->Unlock();
        }

        computeLocalAABB();

        m_meshNeedUpdate = btrue;
    }

    void StaticMeshVertexComponent::computeLocalAABB()
    {
        m_localAABB.invalidate();

        u32 sCount        = m_staticMeshElements.size();
        for (u32 sIdx=0; sIdx<sCount; sIdx++)
        {
            StaticMeshElement & sme = m_staticMeshElements[sIdx];
            for (ITF_VECTOR<VertexPNC3T>::iterator svIter = sme.m_staticVertexList.begin();
                svIter != sme.m_staticVertexList.end(); svIter++)
            {
                m_localAABB.grow(svIter->m_pos.truncateTo2D());
            }
        }
    }

    void StaticMeshVertexComponent::updateAABB()
    {
        AABB animAABB(m_localAABB);
        animAABB.Scale(m_actor->getScale());
        if (m_actor->getIsFlipped())
            animAABB.FlipHorizontaly(bfalse);
        animAABB.Rotate(m_actor->getAngle(), bfalse);
        animAABB.Translate(m_actor->get2DPos());

        GetActor()->growAABB(animAABB);
    }


    bbool StaticMeshVertexComponent::areFrisesMergeable(const ITF_VECTOR<Frise *> & _frise, String8 & _error)
    {
        const GFXMaterialSerializable * _usingMat = NULL;
        return areFrisesMergeable(_frise, _error, _usingMat);
    }

    bbool StaticMeshVertexComponent::canAddFrises(const ITF_VECTOR<Frise *> & _frise)
    {
        //const GFXMaterialSerializable * _usingMat = NULL;
        Scene * subScene = getFriseGroupScene();
        if (!subScene)
            return bfalse;

        ITF_VECTOR<Frise*> friseList = subScene->getFrises();
        friseList.insert(friseList.end(), _frise.begin(), _frise.end());
        
        return isFriseListMergeable(friseList);

        //String8 _error;
        //return areFrisesMergeable(_frise, _error, _usingMat);
    }

    bbool StaticMeshVertexComponent::areFrisesMergeable(const ITF_VECTOR<Frise *> & _frise, String8 & _error, const GFXMaterialSerializable * & _usingMat)
    {
        String8 errorStr;
        for (u32 fIdx=0; fIdx<_frise.size(); fIdx++)
        {
            Frise * curFrise = _frise[fIdx];

            if (!curFrise->m_pMeshStaticData && !curFrise->m_pMeshAnimData)
            {
                _error.setTextFormat("Frise %s is not static", curFrise->getUserFriendly().cStr());
                return bfalse;
            }

            if (curFrise->m_pCollisionData)
            {
                _error.setTextFormat("Frise %s has collision data", curFrise->getUserFriendly().cStr());
                return bfalse;
            }

            const FriseConfig * friseConfig = curFrise->getConfig();
            if (!friseConfig)
            {
                _error.setTextFormat("Frise %s has wrong FriseConfig", curFrise->getUserFriendly().cStr());
                return bfalse;
            }

            if (friseConfig->m_textureConfigs.size() != 1)
            {
                _error.setTextFormat("Frise %s has friseConfig with bad number of texture config", curFrise->getUserFriendly().cStr());
                return bfalse;
            }


            const GFXMaterialSerializable & serializableMat = friseConfig->m_textureConfigs[0].getGFXSerializableMat();
            if (!_usingMat)
            {
                _usingMat = &friseConfig->m_textureConfigs[0].getGFXSerializableMat();
            } else
            {
                if (*_usingMat != serializableMat)
                {
                    _error.setTextFormat("Cannot merge frises with different materials");
                    return bfalse;
                }
            }
        }
        return btrue;
    }



    u32 StaticMeshVertexComponent::addFrise( Frise * _frise)
    {
        Transform3d frisePtToGlobal;
        frisePtToGlobal.setFrom( _frise->getPos(), _frise->getAngle(), _frise->getScale(), _frise->getIsFlipped() );

        Transform3d actorPtToGlobal;
        actorPtToGlobal.setFrom( GetActor()->getPos(), GetActor()->getAngle(), GetActor()->getScale(), GetActor()->getIsFlipped() );

        ITF_Mesh & friseMesh = _frise->m_pMeshStaticData ? _frise->m_pMeshStaticData->m_mesh : _frise->m_pMeshAnimData->m_mesh;
        u32 nbVertex    = friseMesh.getCurrentVB()->m_nVertex;
        m_staticMeshElements.emplace_back();
        StaticMeshElement & staticElment = m_staticMeshElements.back();
        staticElment.m_staticVertexList.resize(nbVertex);

        if (_frise->m_pMeshStaticData)
        {
            VertexPCT   *fdata = 0;
            friseMesh.LockVertexBuffer((void **) &fdata);

            Vec3d pos;
            for (u32 i=0; i<nbVertex; i++, fdata++)
            {
                pos = frisePtToGlobal.transformPos(fdata->m_pos);
                pos = actorPtToGlobal.inverseTransformPos(pos);

                staticElment.m_staticVertexList[i].setData(pos, fdata->m_color, fdata->m_uv);
            }
        } else
        {
            staticElment.m_animated = btrue;
            
            VertexPNC3T   *fdata = 0;
            friseMesh.LockVertexBuffer((void **) &fdata);

            Vec3d pos;
            for (u32 i=0; i<nbVertex; i++, fdata++)
            {
                pos = frisePtToGlobal.transformPos(fdata->m_pos);
                pos = actorPtToGlobal.inverseTransformPos(pos);

                staticElment.m_staticVertexList[i] = *fdata;
                staticElment.m_staticVertexList[i].setPos(pos);
            }
        }

        friseMesh.UnlockVertexBuffer();

        ITF_MeshElement & fElement = friseMesh.getMeshElement();
        u32 nbIndex     = fElement.m_count;
        staticElment.m_staticIndexList.resize(nbIndex);

        u16         *pindexelem = 0;
        fElement.m_indexBuffer->Lock((void**)&pindexelem);
        ITF_MemcpyWriteCombined(&staticElment.m_staticIndexList[0], pindexelem, sizeof(u16) * nbIndex );
        fElement.m_indexBuffer->Unlock();

        staticElment.m_pos = frisePtToGlobal.transformPos(Vec3d::Zero);
        staticElment.m_pos = actorPtToGlobal.inverseTransformPos(staticElment.m_pos);
        staticElment.m_color = _frise->getGfxPrimitiveParamSerialized().m_colorFactor;

        SceneObjectPathUtils::getRelativePathFromObject( m_actor, _frise, staticElment.m_frisePath);

        return m_staticMeshElements.size()-1;
    }


    bbool StaticMeshVertexComponent::isFriseListMergeable(const ITF_VECTOR<Frise *> & _frise)
    {
        const GFXMaterialSerializable * pMat = NULL;
        for (u32 texIdx = 0; texIdx < TEXSET_ID_COUNT; texIdx++)
        {
            if (m_material.getTextureSet().getTexture(TextureSetSubID(texIdx)) != NULL)
            {
                pMat = &m_material;
                break;
            }
        }

        String8 errorStr;
        return areFrisesMergeable(_frise, errorStr, pMat);
    }

    bbool StaticMeshVertexComponent::addFriseList(const ITF_VECTOR<Frise *> & _frise)
    {
        const GFXMaterialSerializable * pMat = NULL;
        for (u32 texIdx = 0; texIdx < TEXSET_ID_COUNT; texIdx++)
        {
            if (m_material.getTextureSet().getTexture(TextureSetSubID(texIdx)) != NULL)
            {
                pMat = &m_material;
                break;
            }
        }

        String8 errorStr;
        if (!areFrisesMergeable(_frise, errorStr, pMat))
            return bfalse;

        if (&m_material != pMat)
        {
            m_material = *pMat;
            m_material.onLoaded(GetActor()->getResourceContainer());
        }

        bbool startPaused = _frise.size() > 0;
        if (_frise.size() > 0)
        {
            Frise * pFrise0 = _frise[0];
            m_primitiveParam = pFrise0->getGfxPrimitiveParamSerialized();
            m_primitiveParam.m_colorFactor = Color::white();
        }
        for (u32 fIdx=0; fIdx<_frise.size(); fIdx++)
        {
            addFrise(_frise[fIdx]);
            startPaused &= _frise[fIdx]->isStartPaused();
        }
        m_actor->setStartForcedPaused(startPaused);

        sortElements();
        fillStaticMeshFromData();

        return btrue;
    }

    void StaticMeshVertexComponent::setFriseGroup(ObjectRef _group)
    {
        m_friseGroup  =_group;
        Pickable * friseGroupPick = (Pickable *)m_friseGroup.getObject();
        if (!friseGroupPick)
            return;

        SceneObjectPathUtils::getRelativePathFromObject( m_actor, friseGroupPick, m_friseGroupPath);
    }

    Scene * StaticMeshVertexComponent::getFriseGroupScene() const
    {
        SubSceneActor * friseGroupPick = (SubSceneActor *)m_friseGroup.getObject();
        if (!friseGroupPick)
            return NULL;

        return friseGroupPick->getSubScene();
    }
    
    struct SortStaticMeshElement
    {
        // just to avoid warning
        SortStaticMeshElement& operator=(const SortStaticMeshElement&);

        SortStaticMeshElement(const StaticMeshVertexComponent & _staticMeshVertexComponent)
            : smv(_staticMeshVertexComponent)
        {
        }

        // sort first per Z then per address
        inline bool operator()(u32 a, u32 b)
        {
            return smv.getStaticMeshElments()[a].m_pos.z() < smv.getStaticMeshElments()[b].m_pos.z();
        }

        const StaticMeshVertexComponent & smv;
    };

    void StaticMeshVertexComponent::sortElements()
    {
        u32 nbMeshElements = m_staticMeshElements.size();

        m_sortedMeshElements.resize(nbMeshElements);
        for (u32 i=0; i<nbMeshElements; i++)
        {
            m_sortedMeshElements[i] = i;
        }

        stablesort(m_sortedMeshElements.begin(), m_sortedMeshElements.end(), SortStaticMeshElement(*this));
    }

#if defined(ITF_SUPPORT_EDITOR)
    bbool StaticMeshVertexComponent::computeFriseGroup(bbool _force)
    {
        if (!m_meshNeedFullUpdate && !_force)
            return bfalse;
        m_meshNeedFullUpdate = bfalse;

        Scene * subScene = getFriseGroupScene();
        if (!subScene)
            return bfalse;

        ITF_VECTOR <Frise*> friseList = subScene->getFrises();
        for (ITF_VECTOR <Frise*>::iterator friseIter = friseList.begin();
            friseIter != friseList.end();)
        {
            if ((*friseIter)->isDestructionRequested())
            {
                friseIter            = friseList.erase(friseIter);
                m_meshNeedFullUpdate = btrue;
            }
            else
                friseIter++;
        }

        if (!isFriseListMergeable(friseList))
            return bfalse;

        if (!_force)
        {
            m_staticMeshElements.clear();
            return addFriseList(friseList);
        }

#ifdef ALLOW_FCG_AUTO_UPDATE
        ITF_VECTOR<StaticMeshElement> tmpMeshList = m_staticMeshElements;
        m_staticMeshElements.clear();
        if (!addFriseList(friseList))
        {
            m_staticMeshElements = tmpMeshList;
            return bfalse;
        } else
        {
            bbool meshChanged = tmpMeshList.size() != m_staticMeshElements.size();
            ITF_VECTOR<StaticMeshElement>::const_iterator currentIter = m_staticMeshElements.begin();
            ITF_VECTOR<StaticMeshElement>::const_iterator prevIter = tmpMeshList.begin();
            for (; !meshChanged && prevIter != tmpMeshList.end(); ++currentIter, ++prevIter)
            {
                meshChanged |= !prevIter->m_pos.IsEqual(currentIter->m_pos, MTH_EPSILON);
                meshChanged |= prevIter->m_color.getAsU32() != currentIter->m_color.getAsU32();

                ITF_VECTOR<u16>::const_iterator prevStaticIndexIter = prevIter->m_staticIndexList.begin();
                ITF_VECTOR<u16>::const_iterator currentStaticIndexIter = currentIter->m_staticIndexList.begin();
                meshChanged |= prevIter->m_staticIndexList.size() != currentIter->m_staticIndexList.size();
                for (; !meshChanged && prevStaticIndexIter != prevIter->m_staticIndexList.end(); ++prevStaticIndexIter, ++currentStaticIndexIter)
                {
                    meshChanged |= *prevStaticIndexIter != *currentStaticIndexIter;
                }

                ITF_VECTOR<VertexPNC3T>::const_iterator prevStaticVertexIter = prevIter->m_staticVertexList.begin();
                ITF_VECTOR<VertexPNC3T>::const_iterator currentStaticVertexIter = currentIter->m_staticVertexList.begin();
                meshChanged |= prevIter->m_staticVertexList.size() != currentIter->m_staticVertexList.size();
                for (; !meshChanged && prevStaticVertexIter != prevIter->m_staticVertexList.end(); ++prevStaticVertexIter, ++currentStaticVertexIter)
                {
                    meshChanged |= !prevStaticVertexIter->m_pos.IsEqual(currentStaticVertexIter->m_pos, MTH_EPSILON);
                    meshChanged |= prevStaticVertexIter->m_color != currentStaticVertexIter->m_color;
                    meshChanged |= !prevStaticVertexIter->m_uv.IsEqual(currentStaticVertexIter->m_uv, MTH_EPSILON);
                }
            }

            if (meshChanged)
            {
                PLUGINGATEWAY->onObjectChanged(GetActor());
            }
            return btrue;
        }
#else //ALLOW_FCG_AUTO_UPDATE
        m_staticMeshElements.clear();
        return addFriseList(friseList);
#endif
    }


    void StaticMeshVertexComponent::transformElement(u32 _index, const Transform3d & _transform)
    {
        if (_index >= m_staticMeshElements.size())
            return;

        StaticMeshElement & meshElement = m_staticMeshElements[_index];
        u32 vertexSize = meshElement.m_staticVertexList.size();
        for (u32 i=0; i<vertexSize; i++)
        {
            Vec3d & pos = meshElement.m_staticVertexList[i].m_pos;
            pos -= meshElement.m_pos;
            pos = _transform.transformPos(pos);
            pos += meshElement.m_pos;
        }

        meshElement.m_pos += _transform.m_pos.to3d(_transform.m_z);
    }

    Pickable * StaticMeshVertexComponent::getElement(u32 _index)
    {
        StaticMeshElement & sme = m_staticMeshElements[_index];
        return SceneObjectPathUtils::getObjectFromRelativePath(m_actor, sme.m_frisePath);
    }

    void StaticMeshVertexComponent::moveElement(u32 _index, const Vec3d & _move)
    {
        StaticMeshElement & sme = m_staticMeshElements[_index];
        Pickable * pick = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, sme.m_frisePath);
        if (!pick)
            return;
        pick->setPos(pick->getPos() + _move);
        pick->setWorldInitialPos(pick->getWorldInitialPos() + _move, btrue);
        m_meshNeedFullUpdate = btrue;
    }

    void StaticMeshVertexComponent::rotateElement(u32 _index, f32 _angle)
    {
        StaticMeshElement & sme = m_staticMeshElements[_index];
        Pickable * pick = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, sme.m_frisePath);
        if (!pick)
            return;
        pick->setAngle(pick->getAngle() + _angle);
        pick->setWorldInitialRot(pick->getWorldInitialRot() + _angle, btrue);
        m_meshNeedFullUpdate = btrue;
    }

    void StaticMeshVertexComponent::scaleElement(u32 _index, const Vec2d & _scale)
    {
        StaticMeshElement & sme = m_staticMeshElements[_index];
        Pickable * pick = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, sme.m_frisePath);
        if (!pick)
            return;
        pick->setScale(pick->getScale() * _scale);
        pick->setWorldInitialScale(pick->getWorldInitialScale() * _scale, btrue);
        m_meshNeedFullUpdate = btrue;
    }


    void StaticMeshVertexComponent::updateFriseColors()
    {
        u32 smeSize = m_staticMeshElements.size();
        for (u32 i=0; i<smeSize; i++)
        {
            StaticMeshElement & sme = m_staticMeshElements[i];
            Frise * frise = SAFE_DYNAMIC_CAST(SceneObjectPathUtils::getObjectFromRelativePath(m_actor, sme.m_frisePath), Frise);
            if (!frise)
                continue;

            const GFXPrimitiveParam& primitiveParam = frise->getGfxPrimitiveParamSerialized();
            if (primitiveParam.m_colorFactor != sme.m_color)
            {
                GFXPrimitiveParam newPrimitiveParam = primitiveParam;
                newPrimitiveParam.m_colorFactor = sme.m_color;
                frise->setGFXPrimitiveParamSerialized(newPrimitiveParam);
            }
        }
    }

    void StaticMeshVertexComponent::onEditorMove(bbool _modifyInitialPos /* = btrue */)
    {
        actorDataToSubSceneData(btrue);
        updateFriseColors();

        if (!computeFriseGroup(bfalse))
            fillStaticMeshFromData();
        updateMesh();
        updateAABB();
    }

#endif // ITF_SUPPORT_EDITOR

}
