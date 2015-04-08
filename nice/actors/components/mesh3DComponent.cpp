#include "precompiled_engine.h"

#ifndef _ITF_MESH3DCOMPONENT_H_
#include "engine/actors/components/mesh3Dcomponent.h"
#endif //_ITF_MESH3DCOMPONENT_H_

#ifndef _ITF_MESH3D_H_
#include "engine/display/Mesh3D.h"
#endif // _ITF_MESH3D_H_

#ifndef _ITF_SKELETON3D_H_
#include    "engine/animation3D/Skeleton3D.h"
#endif // _ITF_SKELETON3D_H_

#ifndef _ITF_ANIMATION3D_H_
#include    "engine/animation3D/Animation3D.h"
#endif // _ITF_ANIMATION3D_H_

#ifndef _MTH_SIMD_QUATERNION_H_
#include "core/math/SIMD/SIMD_Quaternion.h"
#endif //_MTH_SIMD_QUATERNION_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_MESH3DRESOURCE_H_
#include "engine/display/Mesh3DResource.h"
#endif //_ITF_MESH3DRESOURCE_H_
#ifdef ITF_ANDROID
#ifdef _SIMD_SOFTWARE_
using namespace SIMDSoftware;
#endif
#endif

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Mesh3DComponent class
    // 
    ///////////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(Mesh3DComponent)
    
    //-----------------------------------------------------
    BEGIN_SERIALIZATION_CHILD(Mesh3DComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("ScaleZ",m_ScaleZ);
            SERIALIZE_CONTAINER_OBJECT("materialList", m_materialList);
            SERIALIZE_MEMBER("mesh3D",m_mesh3DFile);
            SERIALIZE_CONTAINER("mesh3DList", m_mesh3DFileList );
            SERIALIZE_MEMBER("skeleton3D",m_skeleton3DFile);
            SERIALIZE_MEMBER("animation3D",m_animation3DFile);
            SERIALIZE_CONTAINER("animation3DList", m_animation3DFileList );
            SERIALIZE_OBJECT("animation3DSet", m_animation3DSet );
            SERIALIZE_MEMBER("animationNode", m_animation3DNode);
            SERIALIZE_ENUM_BEGIN("Animation_player_mode",m_anim.m_Mode);
            if ( (serializer->getProperties()&ESerializerProperty_Tool) )
            {
                serializer->SerializeEnumVar( Animation3DPlayer::modeCine, "Cine" );
                serializer->SerializeEnumVar( Animation3DPlayer::modeContracted, "Contracted" );
                serializer->SerializeEnumVar( Animation3DPlayer::modeExpanded, "Expanded" );
                serializer->SerializeEnumVar( Animation3DPlayer::modeExpandedAffectPos, "ExpandedAffectPos" );
            }
            SERIALIZE_ENUM_END();
            SERIALIZE_MEMBER("orientation", m_initialOrientation);
        END_CONDITION_BLOCK()

#ifdef ITF_SUPPORT_EDITOR
        BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
            SERIALIZE_MEMBER("Draw_mesh",m_drawMesh);
            SERIALIZE_MEMBER("Draw_normals",m_drawNormals);
            SERIALIZE_MEMBER("Draw_bounding_volume",m_drawBV);
            SERIALIZE_MEMBER("Draw_skeleton",m_drawSkeleton);
            SERIALIZE_MEMBER("Freeze_skinning",m_freezeSkinning);
            SERIALIZE_MEMBER("Hardware_skinning",m_hardwareSkinning);
            SERIALIZE_MEMBER("Animation_time_ratio",m_animationTimeRatio);
            SERIALIZE_MEMBER("input0",m_input0);
            SERIALIZE_MEMBER("input1",m_input1);
        END_CONDITION_BLOCK()
#endif /*ITF_SUPPORT_EDITOR*/
    END_SERIALIZATION()

    //=============================================================================================
    Mesh3DComponent::Mesh3DComponent() : Super()
    , m_useIK(bfalse)
    , m_allowAnimMod(bfalse)
	, m_currentColor(Color::white())
	, m_colorSrc(Color::white())
	, m_colorDst(Color::white())
	, m_colorBlendTime(0.f)
	, m_colorBlendTotal(0.f)
    , m_playRate(1.f)
	, m_updateVisibility(bfalse)
    {
		m_Inactive  = false;
		m_Invisible  = false;
		
		m_deltaTime = 0.01666666f;

        m_ScaleZ = 0;
        m_FinalMatrix.setIdentity();
        m_ActorMatrix.setIdentity();
        m_GizmoOk = bfalse;
        m_animation3DTree = NULL;
        
		
		m_initialOrientation.setIdentity();
        setOrientation(Matrix44::identity());

        
//        m_blendedMatricesInitialized = bfalse;

#ifdef ITF_SUPPORT_EDITOR
        m_animInputUpdate = NULL;
        m_previousMaterialListSize = 0;
        m_drawMesh = btrue;
        m_drawNormals = bfalse;
        m_drawBV = bfalse;
        m_drawSkeleton = bfalse;
        m_freezeSkinning = bfalse;
        m_animationTimeRatio = 1.0f;
        m_hardwareSkinning = btrue;
        m_input0 = m_input1 = 0.0f;
        m_ignoreModifyInitialPosOnOrientation = bfalse;
#endif 
    }

    //=============================================================================================
    Mesh3DComponent::~Mesh3DComponent()
    {
        if ( m_animation3DTree )
        {
            m_animation3DTree->~Animation3DTree();
            char* memBuffer = reinterpret_cast<char*>(m_animation3DTree);
            ITF::Memory::free(memBuffer);
            m_animation3DTree = NULL;
        }
    }


    //=============================================================================================
    const Mesh3DComponent_Template*  Mesh3DComponent::getTemplate() const 
    {
        return static_cast<const Mesh3DComponent_Template*>(m_template);
    }

    //=============================================================================================
    const Color& Mesh3DComponent::getDefaultColor() const 
    { 
        return getTemplate()->getDefaultColor(); 
    }

    //=============================================================================================
    bbool Mesh3DComponent::isResourceInList(const ITF_VECTOR<ResourceID> & _resources, ResourceID _res)
    {
        for (u32 i = 0; i < _resources.size(); i++)
        {
            if( _resources[i] == _res )
                return true;
        }
        return false;
    }
    
    //=============================================================================================
    // Things to do after actor is loaded
    //
    void Mesh3DComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
#ifdef ITF_SUPPORT_EDITOR
        if (m_actor->isDestructionRequested())
            return;
#endif

        Super::onActorLoaded(_hotReload);

        ///// Resources ---------------------------------------------------------------------------
        LoadMaterialList();	        // Texture list
        LoadMesh3DID();			    // Mesh 
        LoadMesh3DIDList();         // Mesh list
        LoadSkeleton3DID();		    // Skeleton
        LoadAnimation3DID();	    // Animation
        LoadAnimation3DIDList();	// Animation List

        //// input --------------------------------------------------------------------------------
        m_inputList.resize(getTemplate()->getInputList().size());

        for ( u32 i = 0; i < m_inputList.size(); i++ )
        {
            Input* input = &m_inputList[i];
            const InputDesc& inputDesc = getTemplate()->getInputList()[i];

            input->setId(inputDesc.getId());
            input->setType(inputDesc.getType());

            if ( inputDesc.getType() == InputType_F32 )
                input->setValue(0.f);
            else
                input->setValue(static_cast<u32>(0));
        }

        ///// Animation Set ----------------------------------------------------------------------
        m_animation3DSet.setTemplate( getTemplate()->getAnimation3DSetPtr());
        m_animation3DSet.processLocalAnim();

        ///// Animation tree ----------------------------------------------------------------------
        BlendTreeInitData<Animation3DTreeResult> initData;
        initData.m_loadInPlace = btrue;
        initData.m_inputs = &m_inputList;
        m_animation3DTree = getTemplate()->createAnimation3DTreeInstance();
        m_animation3DTree->init(&initData,&getTemplate()->getAnimTreeTemplate());


		AABB forcedaabb;
		if (getForcedAABB(forcedaabb))
			m_actor->growAABB(forcedaabb);
        

        ///// Register event ------------------------------------------------------------------------
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventTransformQuery_CRC,this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventQueryPosition_CRC,this);

        setOrientation(m_initialOrientation);
    }

	bbool Mesh3DComponent::getForcedAABB(AABB & _aabb) const
	{
		///// Compute AABB ------------------------------------------------------------------------
		AABB forcedaabb = getTemplate()->getForcedAABB();
		if (forcedaabb.isZero())
			return bfalse;

		forcedaabb.Scale(m_actor->getScale());
		forcedaabb.setMin(forcedaabb.getMin() + m_actor->get2DPos());
		forcedaabb.setMax(forcedaabb.getMax() + m_actor->get2DPos());
		_aabb = forcedaabb;

		return btrue;
	}

    //=============================================================================================
    // Things to do before actor is destroy
    //
    void Mesh3DComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();

        ///// Resources ---------------------------------------------------------------------------
        UnloadMaterialList();		// Texture list
        UnloadMesh3DID();			// Mesh
        UnloadMesh3DIDList();       // Mesh list
        UnloadSkeleton3DID();		// Skeleton
        UnloadAnimation3DID();		// Animation
        UnloadAnimation3DIDList();	// Animation List
        
        ///// Internal data -----------------------------------------------------------------------
        Mesh3D::MeshDestroyContent(m_3Dmesh.m_mesh);
    }

    //=============================================================================================
    // Things to do after resources are loaded
    //
    void Mesh3DComponent::onResourceLoaded_Mesh3D(Mesh3DResource* _mesh3D, ITF_Mesh &_mesh )
    {
        if (_mesh3D && _mesh3D->getMesh3D())
        {
            _mesh3D->getMesh3D()->MeshGenerate( _mesh3D,_mesh );                                        // generate graphic data
            _mesh3D->SetMaterial( _mesh, m_materialList, getTemplate()->getMaterialList() );    // assign material
        }
    }
    /**/
    void Mesh3DComponent::onResourceLoaded()
    {
        ///// Build mesh --------------------------------------------------------------------------
        Mesh3DResource* mesh3D = static_cast<Mesh3DResource*>(m_mesh3DID.getResource());
        onResourceLoaded_Mesh3D( mesh3D, m_3Dmesh.m_mesh );

        m_3DmeshList.resize( m_mesh3DIDList.size() );
        for (u32 i = 0; i < m_mesh3DIDList.size(); i++)
        {
            Mesh3DResource* mesh3D = static_cast<Mesh3DResource*>(m_mesh3DIDList[i].getResource());
            onResourceLoaded_Mesh3D( mesh3D, m_3DmeshList[i].m_mesh );
        }

        ///// animation3Dset
        if (!m_animation3DSet.isResolved())
            m_animation3DSet.resolve();
        

        ///// animation3D tree
        Animation3DTreeLoadResource resData;
        resData.m_animation3DSet = &m_animation3DSet;
        m_animation3DTree->onLoadResource(&resData);

        ///// link skeleton & animation -----------------------------------------------------------
        Skeleton3DResource *skeleton3D = static_cast<Skeleton3DResource*>(m_skeleton3DID.getResource());
        setSkeleton( skeleton3D );
        
        Animation3DResource *animation3D = static_cast<Animation3DResource*>(m_animation3DID.getResource());
        setAnimation( animation3D );

        ///// anim player -------------------------------------------------------------------------
        m_anim.setAnimation3DTree( m_animation3DTree );
        m_anim.setAnimation3DList( &m_animation3DList );
        m_anim.setAnimation3DSet( &m_animation3DSet );
        m_anim.setInputList( &m_inputList );

        // load a default animation
        if (m_animation3DNode.isValid())
            m_anim.setAction(m_animation3DNode);
        else if (getTemplate()->getAnimationNode().isValid())
            m_anim.setAction(getTemplate()->getAnimationNode());
        else if (m_animation3DSet.getAnimation3DInfoCount() >  0)
            m_anim.setAnimation(m_animation3DSet.getAnimation3DInfo(0)->getAnimation3D());

        if ( m_anim.readyToPlay())
        {
            if (m_Gizmos.size() != m_anim.getSkeleton()->m_ListBones.size())
                m_Gizmos.resize( m_anim.getSkeleton()->m_ListBones.size() );
            m_anim.updateEdge( 0.f, m_Gizmos);
        }

    }

    //=============================================================================================
    // Things to do when resources are unloaded
    //
    void Mesh3DComponent::onUnloadResources()
    {
        Mesh3D::MeshDestroyContent(m_3Dmesh.m_mesh);
        for (u32 i = 0; i < m_3DmeshList.size(); i++)
            Mesh3D::MeshDestroyContent(m_3DmeshList[i].m_mesh);
        m_3DmeshList.clear();
        setAnimation(NULL);
        setSkeleton(NULL);
        setAction("");
    }

    //*********************************************************************************************
    // Event 
    //*********************************************************************************************

    //=============================================================================================
    void Mesh3DComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if(EventTransformQuery *transformQuery = DYNAMIC_CAST(_event,EventTransformQuery))
        {
            Matrix44 M;
            if (transformQuery->getRequestTransform())
            {
                ComputeMatrix(M);
                transformQuery->setTransform( M );
            }
            else if (transformQuery->isScaleSet())
            {
                if (m_ScaleZ == 0.0) m_ScaleZ = 1.0f;
                m_ScaleZ *= transformQuery->getScale().z();
            }
            else if (transformQuery->isSet())
            {
                M = transformQuery->getTransform();
                M.setTranslationToZero();

                Matrix44 normalizedMatrix;
                normalizedMatrix.normalize33(M);
                
                setOrientation(normalizedMatrix);
            }
        }
        else if (EventQueryPosition* onQueryPos = DYNAMIC_CAST(_event,EventQueryPosition))
        {
            const StringID &    boneId  = onQueryPos->getBoneId();
            Skeleton3D *        skl3d   = getSkeleton3D();
            if (boneId.isValid() && skl3d)
            {
                u32         boneIndex = (u32)skl3d->getBoneIndexByName(boneId);
                Matrix44    matrix;
                if (boneIndex != U32_INVALID && getBoneGlobalMatrix(boneIndex, matrix))
                {
                    Vec2d       matrixDir;

                    onQueryPos->setPos(matrix.T());
                    Vec3d(matrix.K()).truncateTo2D(matrixDir);
                    matrixDir.normalize();
                    onQueryPos->setAngle(matrixDir.getAngle());
                }
            }
        }
		else if (EventShow* eventShow = DYNAMIC_CAST(_event,EventShow))
		{
			if ( eventShow->getIsOverrideColor() && m_currentColor != eventShow->getOverrideColor() )
			{
				m_colorSrc = m_currentColor;
				m_colorDst = eventShow->getOverrideColor();
				m_colorBlendTotal = eventShow->getTransitionTime();
				m_colorBlendTime = m_colorBlendTotal;

				if ( m_colorBlendTotal <= 0.f )
				{
					m_colorSrc = m_currentColor = m_colorDst;
				}
			}
		}
    }

	void Mesh3DComponent::processColor( f32 _dt )
	{
		if ( m_colorSrc == m_colorDst || !m_colorBlendTotal)
		{
			return;
		}

		m_colorBlendTime = Max(m_colorBlendTime-_dt,0.f);

		f32 t = 1.f - ( m_colorBlendTime / m_colorBlendTotal );

		if ( t == 1 )
		{
			m_currentColor = m_colorSrc = m_colorDst;
		}
		else
		{
			m_currentColor = Color::Interpolate(m_colorSrc,m_colorDst,t);
		}
	}

    //*********************************************************************************************
    // Texture List
    //*********************************************************************************************

    //=============================================================================================
    void Mesh3DComponent::UnloadMaterialList_Detach()
    {
        Mesh3DResource* mesh3D;
        
        mesh3D = static_cast<Mesh3DResource*>(m_mesh3DID.getResource());
        if (mesh3D)
            mesh3D->DetachMaterial( getMesh().m_mesh );


        u32 minNumber = Min(m_3DmeshList.size(), m_mesh3DIDList.size() );
        for (u32 i = 0; i < minNumber; i++)
        {
            mesh3D = static_cast<Mesh3DResource*>(m_mesh3DIDList[i].getResource());
            if (mesh3D) mesh3D->DetachMaterial( m_3DmeshList[i].m_mesh );
        }
    }

    //=============================================================================================
    void Mesh3DComponent::UnloadMaterialList()
    {
        ///// remove mat from mesh ----------------------------------------------------------------
        UnloadMaterialList_Detach();


        for (u32 i = 0; i < m_materialList.size(); i++)
        {
            m_materialList[i].onUnLoaded(m_actor->getResourceContainer());
        }
    }

    //=============================================================================================
    void Mesh3DComponent::LoadMaterialList()
    {
        for (u32 i = 0; i < m_materialList.size(); i++)
        {
            m_materialList[i].onLoaded(m_actor->getResourceContainer());
        }
#ifdef ITF_SUPPORT_EDITOR
        m_previousMaterialListSize = m_materialList.size();
#endif
    }

    //*********************************************************************************************
    // Mesh
    //*********************************************************************************************

    //=============================================================================================
    void Mesh3DComponent::UnloadMesh3DID()
    {
        if (m_mesh3DID.isValid() && !m_mesh3DFile.isEmpty())
            m_actor->removeResource(m_mesh3DID);
        m_mesh3DID.invalidate();
        Mesh3D::MeshDestroyContent(m_3Dmesh.m_mesh);
    }

    //=============================================================================================
    void Mesh3DComponent::LoadMesh3DID()
    {
        // Mesh 
        if ( !m_mesh3DFile.isEmpty() )
            m_mesh3DID = m_actor->addResource(Resource::ResourceType_Mesh3D, m_mesh3DFile);
        else if ( getTemplate()->getMesh3DID().isValid() )
            m_mesh3DID = getTemplate()->getMesh3DID();
    }

    //=============================================================================================
    void Mesh3DComponent::UnloadMesh3DIDList()
    {
        for (u32 i = 0; i < m_mesh3DIDList.size(); i++)
        {
            if ( m_mesh3DIDList[i] ==  getTemplate()->getMesh3DIDFromList(i)) continue;	    // mesh from template, do nothing
            if ( !m_mesh3DIDList[i].isValid() ) continue;							        // not valid mesh

            m_actor->removeResource(m_mesh3DIDList[i]);
            m_mesh3DIDList[i].invalidate();   
        }
        m_mesh3DIDList.clear();
    }

    //=============================================================================================
    void Mesh3DComponent::LoadMesh3DIDList()
    {
        // Nico: Vu avec Yous => Double boucle pour prendre en compte ce qu'il y a dans le template     
        m_mesh3DIDList.reserve(m_mesh3DFileList.size() + getTemplate()->getMesh3DFileListCount());
        for (u32 i = 0; i < m_mesh3DFileList.size(); i++)
        {
            if ( !m_mesh3DFileList[i].isEmpty() )
                m_mesh3DIDList.push_back(m_actor->addResource(Resource::ResourceType_Mesh3D, m_mesh3DFileList[i]));
        }
        for (u32 i = 0; i < getTemplate()->getMesh3DFileListCount(); i++)
        {
            if ( getTemplate()->getMesh3DIDFromList(i).isValid() 
                &&   !isResourceInList(m_mesh3DIDList, getTemplate()->getMesh3DIDFromList(i)) )
                m_mesh3DIDList.push_back(getTemplate()->getMesh3DIDFromList(i));
        }
    }

    //*********************************************************************************************
    // Skeleton
    //*********************************************************************************************

    //=============================================================================================
    void Mesh3DComponent::UnloadSkeleton3DID()
    {
        if (m_skeleton3DID.isValid() && !m_skeleton3DFile.isEmpty())
            m_actor->removeResource(m_skeleton3DID);
        m_skeleton3DID.invalidate();
        setSkeleton(NULL);
    }

    //=============================================================================================
    void Mesh3DComponent::LoadSkeleton3DID()
    {
        // Mesh 
        if ( !m_skeleton3DFile.isEmpty() )
            m_skeleton3DID = m_actor->addResource(Resource::ResourceType_Skeleton3D, m_skeleton3DFile);
        else if ( getTemplate()->getSkeleton3DID().isValid() )
            m_skeleton3DID = getTemplate()->getSkeleton3DID();
    }

    //*********************************************************************************************
    // Animation
    //*********************************************************************************************

    //=============================================================================================
    void Mesh3DComponent::UnloadAnimation3DID()
    {
        if (m_animation3DID.isValid() && !m_animation3DFile.isEmpty())
            m_actor->removeResource(m_animation3DID);
        m_animation3DID.invalidate();
        setAnimation(NULL);
    }

    //=============================================================================================
    void Mesh3DComponent::LoadAnimation3DID()
    {
        if ( !m_animation3DFile.isEmpty() )
            m_animation3DID = m_actor->addResource(Resource::ResourceType_Animation3D, m_animation3DFile);
        else if ( getTemplate()->getAnimation3DID().isValid() )
            m_animation3DID = getTemplate()->getAnimation3DID();
    }

    //=============================================================================================
    void Mesh3DComponent::UnloadAnimation3DIDList()
    {
        for (u32 i = 0; i < m_animation3DList.getAnims().size(); i++)
        {
            if ( m_animation3DList.getAnims()[i] ==  getTemplate()->getAnimation3DIDFromList(i)) continue;	// animation from template, do nothing
            if ( !m_animation3DList.getAnims()[i].isValid() ) continue;							            // not valid animation

            m_actor->removeResource(m_animation3DList.getAnims()[i]);
            m_animation3DList.getAnims()[i].invalidate();   
        }
        m_animation3DList.getAnims().clear();
    }

    //=============================================================================================
    void Mesh3DComponent::LoadAnimation3DIDList()
    {
        // Nico: Vu avec Yous => Double boucle pour prendre en compte ce qu'il y a dans le template     
        m_animation3DList.reserve(m_animation3DFileList.size() + getTemplate()->getAnimation3DFileListCount());
        for (u32 i = 0; i < m_animation3DFileList.size(); i++)
        {
            if ( !m_animation3DFileList[i].isEmpty() )
                m_animation3DList.push_back(m_actor->addResource(Resource::ResourceType_Animation3D, m_animation3DFileList[i]));
        }
        for (u32 i = 0; i < getTemplate()->getAnimation3DFileListCount(); i++)
        {
            const ResourceID &animResource = getTemplate()->getAnimation3DIDFromList(i);
            if ( getTemplate()->getAnimation3DIDFromList(i).isValid() && !m_animation3DList.isInList( animResource ) )
                m_animation3DList.push_back(animResource);
        }
    }

    //*********************************************************************************************
    // Render
    //*********************************************************************************************

    //=============================================================================================
    // Display
    //
    void Mesh3DComponent::batchPrimitivesMesh3D( const ITF_VECTOR <class View*>& _views, ResourceID &_meshID, Mesh3DPrim &_3Dmesh )
    {

#ifdef ITF_SUPPORT_EDITOR
        if (!m_drawMesh) return;
#endif
        Mesh3DResource* mesh3Dres = static_cast<Mesh3DResource*>(_meshID.getResource());
		if (!mesh3Dres) return;
		Mesh3D* mesh3D = mesh3Dres->getMesh3D();

        if (mesh3D && _3Dmesh.m_mesh.isValid())
        {
            GFXPrimitiveParam primitiveParam = getGfxPrimitiveParam();
			primitiveParam.m_colorFactor = primitiveParam.m_colorFactor*m_currentColor;
			primitiveParam.m_colorFactor.setAlpha(m_alpha);

            _3Dmesh.m_mesh.setMatrix(m_FinalMatrix);
            primitiveParam.m_BV = getVisualAABB();
            _3Dmesh.setCommonParam(primitiveParam);
            _3Dmesh.m_mesh.setCommonParam(primitiveParam);

            
#ifdef ITF_SUPPORT_EDITOR
            if (mesh3D->needSkinning())
            {
				if (_3Dmesh.m_mesh.getCurrentVB()->m_vertexFormat != VertexFormat_PNCTBIBW)
				{
					mesh3D->MeshGenerate(mesh3Dres,_3Dmesh.m_mesh );
					mesh3Dres->SetMaterial( _3Dmesh.m_mesh, m_materialList, getTemplate()->getMaterialList() ); 
				}
			}
			else
			{
				if (_3Dmesh.m_mesh.getCurrentVB()->m_vertexFormat != VertexFormat_PNCT)
				{
					mesh3D->MeshGenerate(mesh3Dres,_3Dmesh.m_mesh );
					mesh3Dres->SetMaterial( _3Dmesh.m_mesh, m_materialList, getTemplate()->getMaterialList() ); 
				}
			}
#endif

            _3Dmesh.m_gizmos.resize( mesh3D->m_SkinToGizmo.size() );
            if (m_GizmoOk)
            {
                for (u32 i = 0; i < mesh3D->m_SkinToGizmo.size(); i++)
                {
                    u32 index = mesh3D->m_SkinToGizmo[i];
                    if (index >= m_Gizmos.size())
                        _3Dmesh.m_gizmos[i] = mesh3D->m_SkinElements[i].m_MatrixFlashInvert;
                    else
                        _3Dmesh.m_gizmos[i].mul44(mesh3D->m_SkinElements[i].m_MatrixFlashInvert, m_Gizmos[index]);
                }
            }
            else
            {
                for (u32 i = 0; i < _3Dmesh.m_gizmos.size(); i++)
                    _3Dmesh.m_gizmos[i].setIdentity();
            }
            

            if(getTemplate()->getForce2DRendering())
            {
                GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &_3Dmesh, GetActor()->getDepth() + getDepthOffset(), GetActor()->getRef());
            }
            else
            {
                GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_3D>(_views, &_3Dmesh, GetActor()->getDepth() + getDepthOffset(), GetActor()->getRef()); 
            }
        }
    }
    /**/
    void Mesh3DComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
		// Exit on Component Inactive
		if (getInvisible())
			return;

		Super::batchPrimitives(_views);

        batchPrimitivesMesh3D( _views, m_mesh3DID, getMesh() );

        u32 count = Min(m_mesh3DIDList.size(), m_3DmeshList.size());

        for (u32 i = 0; i < count; i++)
        {
            if (i < m_mesh3DVisibility.size())
            {
				if (!m_mesh3DVisibility[i])
                    continue;
            }
            batchPrimitivesMesh3D( _views, m_mesh3DIDList[i], m_3DmeshList[i] );
        }

#ifdef ITF_SUPPORT_EDITOR
        DrawEditor();
#endif
    }

    //*********************************************************************************************
    // Update
    //*********************************************************************************************


    //=============================================================================================
    // Compute matrix
    //
    void Mesh3DComponent::ComputeMatrix( Matrix44 &_matrix ) const
    {
        Vec3d scale;
        scale.x() = GetActor()->getScale().x();
        scale.y() = GetActor()->getScale().y();
        scale.z() = m_ScaleZ ? m_ScaleZ : scale.x();

        _matrix.setRotationZ(GetActor()->getAngle());
        _matrix.mul(_matrix, getOrientation());
        _matrix.mulScale( scale );
        _matrix.setTranslation(GetActor()->getPos());
    }

    //=============================================================================================
    // Compute initial matrix
    //
    void Mesh3DComponent::ComputeInitMatrix( Matrix44 &_matrix ) const
    {
        Vec3d scale;
        scale.x() = GetActor()->getWorldInitialScale().x();
        scale.y() = GetActor()->getWorldInitialScale().y();
        scale.z() = m_ScaleZ ? m_ScaleZ : scale.x();

        _matrix.setRotationZ(GetActor()->getWorldInitialRot());
        _matrix.mulScale( scale );
        _matrix.setTranslation(GetActor()->getWorldInitialPos());
    }

    //=============================================================================================
    // Add a request for Inverse Kinematics
    //
    void Mesh3DComponent::requestIK(StringID _nameIK, StringID _boneName, IKType _IKType, const Matrix44& _targetMatrix, u32 _boneNb, f32 _maxRotationByBone, f32 _approximationCoeff, f32 _blendCoeff, i32 _maxIter, f32 _maxDist) 
    { 
        m_requestsIK.push_back(RequestIK(_nameIK, _boneName, _IKType, _targetMatrix, _boneNb, _maxRotationByBone, _approximationCoeff, _blendCoeff, _maxIter, _maxDist)); 
        m_requestsIK.back().m_refBoneIdx = getSkeleton3D()->getBoneIndexByName(_boneName); 
    }

    //=============================================================================================
    // Get the number of bone until the root parent
    //
    u32 Mesh3DComponent::getBoneNbToUppestParentIK(StringID _boneName, i32& _boneIdx)
    {
        Skeleton3D* skeleton = getSkeleton3D();
        _boneIdx = skeleton->getBoneIndexByName(_boneName);
        if( _boneIdx == -1 )
            return 0;

        i32 parentIdx = skeleton->m_ListBones[_boneIdx].m_Father;
        u32 boneNb = 0; // the first
        while( parentIdx != -1 )
        {
            boneNb++;
            parentIdx = skeleton->m_ListBones[parentIdx].m_Father;
        }
        return boneNb;
    }

    //=============================================================================================
    // Get the matrices needed
    //
    void Mesh3DComponent::registerMatricesFromRefBonesIK(i32 _refBoneIdx, u32 _boneMaxNb)
    {
        m_matricesIK.clear();
        m_sumAnglesIK.clear();

        m_matricesIK.reserve(_boneMaxNb);
        m_sumAnglesIK.reserve(_boneMaxNb);

        i32 parentIdx = _refBoneIdx;
        u32 boneNb = 0;
        while( boneNb < _boneMaxNb && parentIdx != -1 )
        {
            Matrix44 result;
            getBoneGlobalMatrixFromFather(parentIdx, result);

            // Matrices
            if( m_matricesIK.size() )
                m_matricesIK.insertAt(0, result);
            else
                m_matricesIK.push_back(result);

            // Angles
            m_sumAnglesIK.push_back(0.0f);

            boneNb++;
            parentIdx = getSkeleton3D()->m_ListBones[parentIdx].m_Father;

//             DebugDraw::line3D(m_matricesIK[0].T(), m_matricesIK[0].T() + m_matricesIK[0].I(), Color::red());
//             DebugDraw::line3D(m_matricesIK[0].T(), m_matricesIK[0].T() + m_matricesIK[0].J(), Color::green());
//             DebugDraw::line3D(m_matricesIK[0].T(), m_matricesIK[0].T() + m_matricesIK[0].K(), Color::blue());
        }

//         if( !m_blendedMatricesInitialized )
//         {
//             m_blendedMatricesInitialized = btrue;
//             m_blendedMatricesIK.reserve(m_matricesIK.size());
// 
//             for( u32 i = 0; i < m_matricesIK.size(); i++ )
//                 m_blendedMatricesIK.push_back(m_matricesIK[i]);
//         }

//         DebugDraw::line3D(m_FinalMatrix.T(), m_FinalMatrix.T() + m_FinalMatrix.I(), Color(0.0f, 0.3f, 0.0f, 0.0f), 5.0f);
//         DebugDraw::line3D(m_FinalMatrix.T(), m_FinalMatrix.T() + m_FinalMatrix.J(), Color(0.0f, 0.0f, 0.3f, 0.0f), 5.0f);
//         DebugDraw::line3D(m_FinalMatrix.T(), m_FinalMatrix.T() + m_FinalMatrix.K(), Color(0.0f, 0.0f, 0.0f, 0.3f), 5.0f);
    }

    //=============================================================================================
    // Apply IK
    //
    void Mesh3DComponent::applyIK(i32 _refBoneIdx)
    {
//         if( CURRENTFRAME%2 )
//         {
//             for( u32 i = 0; i < m_matricesIK.size(); i++ )
//             {
//                 DebugDraw::line3D(m_matricesIK[i].T(), m_matricesIK[i].T() + m_matricesIK[i].I(), Color::red(), 3.0f);
//                 DebugDraw::line3D(m_matricesIK[i].T(), m_matricesIK[i].T() + m_matricesIK[i].J(), Color::green(), 3.0f);
//                 DebugDraw::line3D(m_matricesIK[i].T(), m_matricesIK[i].T() + m_matricesIK[i].K(), Color::blue(), 3.0f);
// 
//                 if( i > 0 )
//                     DebugDraw::line3D(m_matricesIK[i].T(), m_matricesIK[i-1].T(), Color(0.0f, 0.5f, 0.0f, 0.5f));
//                 else
//                 {
//                     Vec3d boneChestPos;
//                     getBoneGlobalPos(1, boneChestPos);
//                     DebugDraw::line3D(m_matricesIK[i].T(), boneChestPos, Color(0.0f, 0.5f, 0.0f, 0.5f));
//                 }
//             }
//         }

// 
//         for( u32 i = 0; i < m_matricesIK.size(); i++ )
//         {
//             static const f32 coeff = 0.5f;
//             Matrix44 temp;
//             temp.lerp(m_blendedMatricesIK[i],m_matricesIK[i], coeff);
//             m_blendedMatricesIK[i] = temp;
//         }

        i32 boneIdx = _refBoneIdx;
        i32 i = m_matricesIK.size()-1;
        i32 fatherIdx;
        while( i >= 0 && boneIdx >= 0 )
        {
            fatherIdx = getSkeleton3D()->m_ListBones[boneIdx].m_Father;
            Matrix44 father = Matrix44::identity();
            if( i > 0 )
            {
                father = m_matricesIK[i-1];
                father.inverse(father);
                Matrix44 local;
                local.mul(m_matricesIK[i],father);
                m_Gizmos[boneIdx] = local;
            }
            else if( fatherIdx != -1 )
            {
                getBoneGlobalMatrixFromFather(fatherIdx, father);
                father.inverse(father);
                Matrix44 local;
                local.mul(m_matricesIK[i],father);
                m_Gizmos[boneIdx] = local;
            }
            boneIdx = fatherIdx;
            i--;
        }
    }

    //=============================================================================================
    // Compute IK
    //

    // attach a bone to a specific location
    void Mesh3DComponent::matrix44TargetIK( const   Matrix44&   _target,
                                                    f32         _maxRotationByBone,  
                                                    i32         _maxIterations, 
                                                    f32         _maxDist )
    {
        u32 matrixNb = m_matricesIK.size();
        if( matrixNb < 2 )
            return;

        Matrix44    IKRotation;
        Float4      oldPos;
        Vec3d       IKTarget = _target.T();

        f32 maxSqrDist = _maxDist*_maxDist;

        while( _maxIterations > 0 )
        {
            for( u32 i = 0; i < matrixNb; i++ )
            {
                Vec3d IKDir = m_matricesIK[matrixNb-1].T() - m_matricesIK[i].T();
                Vec3d TargetDir = IKTarget - m_matricesIK[i].T();

                IKDir       *= f32_ApproxInvSqrt(IKDir.sqrnorm() + MTH_EPSILON);
                TargetDir   *= f32_ApproxInvSqrt(TargetDir.sqrnorm() + MTH_EPSILON);

                Vec3d IKCross;
                IKDir.crossProduct(TargetDir, IKCross);

                f32 IKAngle = f32_ACos(IKDir.dot(TargetDir));

                if( m_sumAnglesIK[i] + IKAngle > _maxRotationByBone )
                    IKAngle = _maxRotationByBone - m_sumAnglesIK[i];

                if( IKAngle > MTH_EPSILON )
                {
                    IKCross *= f32_ApproxInvSqrt(IKCross.sqrnorm() + MTH_EPSILON);

                    // Build rotation Matrix
                    Quaternion q;
                    q.setFromAxisAngle(IKCross, IKAngle);
                    IKRotation.setIdentity();
                    q.matrix33(IKRotation);
                    
                    // Apply Rotation
                    oldPos = m_matricesIK[i].T();
                    m_matricesIK[i].setT(splatZero());
                    m_matricesIK[i].mul(m_matricesIK[i], IKRotation);
                    m_matricesIK[i].setT(oldPos);

                    for( u32 j = i+1; j < matrixNb; j++ )
                    {
                        Float4 offset;
                        offset = m_matricesIK[j].T() - m_matricesIK[i].T();
                        m_matricesIK[j].setT(offset);
                        m_matricesIK[j].mul(m_matricesIK[j], IKRotation);
                        offset = m_matricesIK[j].T() + m_matricesIK[i].T();
                        m_matricesIK[j].setT(offset);
                    }

                    m_sumAnglesIK[i] += IKAngle;
                }

                if( (IKTarget - m_matricesIK[matrixNb-1].T()).sqrnorm() < maxSqrDist )
                {
                    _maxIterations = 0;
                    break;
                }
            }
            _maxIterations--;
        }
    }
    // same as above, reversed
    void Mesh3DComponent::matrix44TargetIKReverse( const Matrix44&   _target,
                                                         f32         _maxRotationByBone,
                                                         i32         _maxIterations,
                                                         f32         _maxDist )
    {
        u32 matrixNb = m_matricesIK.size();
        if( matrixNb < 2 )
            return;

        Matrix44    IKRotation;
        Float4      oldPos;
        Vec3d       IKTarget = _target.T();

        f32 maxSqrDist = _maxDist*_maxDist;

        while( _maxIterations > 0 )
        {
            for( i32 i = matrixNb-2; i >= 0; i-- )
            {
                Vec3d IKDir = m_matricesIK[matrixNb-1].T() - m_matricesIK[i].T();
                Vec3d TargetDir = IKTarget - m_matricesIK[i].T();

                IKDir       *= f32_ApproxInvSqrt(IKDir.sqrnorm() + MTH_EPSILON);
                TargetDir   *= f32_ApproxInvSqrt(TargetDir.sqrnorm() + MTH_EPSILON);

                Vec3d IKCross;
                IKDir.crossProduct(TargetDir, IKCross);

                f32 IKAngle = f32_ACos(IKDir.dot(TargetDir));

                if( m_sumAnglesIK[i] + IKAngle > _maxRotationByBone )
                    IKAngle = _maxRotationByBone - m_sumAnglesIK[i];

                if( IKAngle > MTH_EPSILON )
                {
                    IKCross *= f32_ApproxInvSqrt(IKCross.sqrnorm() + MTH_EPSILON);

                    // Build rotation Matrix
                    Quaternion q;
                    q.setFromAxisAngle(IKCross, IKAngle);
                    IKRotation.setIdentity();
                    q.matrix33(IKRotation);

                    // Set next Matrices as local (before applying the new rotation to the current matrix)
                    for( i32 j = matrixNb-1; j > i; j-- )
                    {
                        Matrix44 invFather = m_matricesIK[j-1];
                        invFather.inverse(invFather);
                        m_matricesIK[j].mul(m_matricesIK[j], invFather);
                    }

                    // Apply Rotation
                    oldPos = m_matricesIK[i].T();
                    m_matricesIK[i].setT(splatZero());
                    m_matricesIK[i].mul(m_matricesIK[i], IKRotation);
                    m_matricesIK[i].setT(oldPos);

                    // replace to world
                    for( u32 j = i+1; j < matrixNb; j++ )
                        m_matricesIK[j].mul(m_matricesIK[j], m_matricesIK[j-1]);

                    m_sumAnglesIK[i] += IKAngle;
                }

                if( (IKTarget - m_matricesIK[matrixNb-1].T()).sqrnorm() < maxSqrDist )
                {
                    _maxIterations = 0;
                    break;
                }
            }
            _maxIterations--;
        }
    }

    // look At (using an angle)
    void Mesh3DComponent::matrix44TargetIKLookAt( const Matrix44& _target, f32 _maxRotationByBone, i32 _maxIterations, f32 _maxAngle )    
    {
        u32 matrixNb = m_matricesIK.size();
        if( matrixNb < 2 )
            return;

        Matrix44    IKRotation;
        Float4      oldPos;
        Vec3d       IKTarget = _target.T();

        while( _maxIterations > 0 )
        {
            for( i32 i = matrixNb-2; i >= 0; i-- )
            {
                Vec3d IKDir = m_matricesIK[matrixNb-1].T() - m_matricesIK[matrixNb-2].T();
                Vec3d TargetDir = IKTarget - m_matricesIK[matrixNb-2].T();

                IKDir       *= f32_ApproxInvSqrt(IKDir.sqrnorm() + MTH_EPSILON);
                TargetDir   *= f32_ApproxInvSqrt(TargetDir.sqrnorm() + MTH_EPSILON);

                Vec3d IKCross;
                IKDir.crossProduct(TargetDir, IKCross);

                f32 IKAngle = f32_ACos(IKDir.dot(TargetDir));

                if( m_sumAnglesIK[i] + IKAngle > _maxRotationByBone )
                    IKAngle = _maxRotationByBone - m_sumAnglesIK[i];

                if( IKAngle > MTH_EPSILON )
                {
                    IKCross *= f32_ApproxInvSqrt(IKCross.sqrnorm() + MTH_EPSILON);

                    // Build rotation Matrix
                    Quaternion q;
                    q.setFromAxisAngle(IKCross, IKAngle);
                    IKRotation.setIdentity();
                    q.matrix33(IKRotation);

                    // Set next Matrices as local (before applying the new rotation to the current matrix)
                    for( i32 j = matrixNb-1; j > i; j-- )
                    {
                        Matrix44 invFather = m_matricesIK[j-1];
                        invFather.inverse(invFather);
                        m_matricesIK[j].mul(m_matricesIK[j], invFather);
                    }

                    // Apply Rotation
                    oldPos = m_matricesIK[i].T();
                    m_matricesIK[i].setT(splatZero());
                    m_matricesIK[i].mul(m_matricesIK[i], IKRotation);
                    m_matricesIK[i].setT(oldPos);

                    // replace to world
                    for( u32 j = i+1; j < matrixNb; j++ )
                        m_matricesIK[j].mul(m_matricesIK[j], m_matricesIK[j-1]);

                    m_sumAnglesIK[i] += IKAngle;
                }
                if( IKAngle < _maxAngle )   // looking at the target
                {
                    _maxIterations = 0;
                    break;  // stop directly, no need for more bones
                }
            }
            _maxIterations--;
        }
    }

    // bone chain smooth
    void Mesh3DComponent::matrix44TargetIKSmoothChain(      StringID    _nameIK, 
                                                      const Matrix44&   _target,
                                                            f32         _maxRotationByBone,
                                                            f32         _approximationCoeff, 
                                                            f32         _blendCoeff )
    {
        u32 matrixNb = m_matricesIK.size();
        if( matrixNb < 2 )
            return;

        // Target pos to look at
        const Vec3d IKTarget = _target.T();

        // current and optimal sight
        Vec3d currentLookAtSight = m_matricesIK[matrixNb-1].T() - m_matricesIK[0].T();
        Vec3d optimalLookAtSight = IKTarget - m_matricesIK[0].T();

        currentLookAtSight /= currentLookAtSight.norm() + MTH_EPSILON;
        optimalLookAtSight /= optimalLookAtSight.norm() + MTH_EPSILON;

        // total angle and angle bone
        static const f32 maxAngularSpeed = MTH_2PI/60.0f;
        f32 totalAngle = f32_ACos(currentLookAtSight.dot(optimalLookAtSight))*_approximationCoeff;
        f32 angleByBone = totalAngle/matrixNb;
        angleByBone = f32_Min(angleByBone, _maxRotationByBone);

        // Direction to deal with
        Vec3d curDir = m_matricesIK[matrixNb-1].T() - m_matricesIK[0].T();
        Vec3d TargetDir = IKTarget - m_matricesIK[0].T();

        Vec3d IKCross;
        curDir.crossProduct(TargetDir, IKCross);
        f32 norm = IKCross.norm();
        if ( norm< MTH_EPSILON )
            return;
        IKCross /= norm;

        // Build rotation Matrix
        Matrix44 IKRotation;
        Quaternion q;
        q.setFromAxisAngle(IKCross, angleByBone);
        IKRotation.setIdentity();
        q.matrix33(IKRotation);

        if ( m_smoothChainMatrixBackup.find(_nameIK) != m_smoothChainMatrixBackup.end() )
            IKRotation.lerp(m_smoothChainMatrixBackup[_nameIK], IKRotation, _blendCoeff);

        for( u32 i = 0; i < matrixNb-1; ++i )
        {
            // Apply Rotation
            Float4 oldPos = m_matricesIK[i].T();
            m_matricesIK[i].setT(splatZero());
            m_matricesIK[i].mul(m_matricesIK[i], IKRotation);
            m_matricesIK[i].setT(oldPos);

            for( u32 j = i+1; j < matrixNb; ++j )
            {
                Float4 offset;
                offset = m_matricesIK[j].T() - m_matricesIK[i].T();
                m_matricesIK[j].setT(offset);
                m_matricesIK[j].mul(m_matricesIK[j], IKRotation);
                offset = m_matricesIK[j].T() + m_matricesIK[i].T();
                m_matricesIK[j].setT(offset);
            }
        }

        // backup
        m_smoothChainMatrixBackup[_nameIK] = IKRotation;
    }
    //=============================================================================================
    // IK computation
    //
    void Mesh3DComponent::IKManagement()
    {
        // Inverse Kinematics base on local matrices
        if( m_useIK )
        {
            m_anim.copyLocalMatrices(m_Gizmos);
            for( u32 i = 0; i < m_requestsIK.size(); i++ )
            {
                RequestIK& request = m_requestsIK[i];
                registerMatricesFromRefBonesIK(request.m_refBoneIdx, request.m_boneNb);

                switch( request.m_IKType )
                {
                    case IKType_Normal:
                        matrix44TargetIK( request.m_targetMatrix, request.m_maxRotationByBone, request.m_maxIter, request.m_maxValue );
                        break;
                    case IKType_Reverse:
                        matrix44TargetIKReverse( request.m_targetMatrix, request.m_maxRotationByBone, request.m_maxIter, request.m_maxValue );
                        break;
                    case IKType_SmoothChain:
                        matrix44TargetIKSmoothChain( request.m_nameIK, request.m_targetMatrix, request.m_maxRotationByBone, request.m_approxCoeff, request.m_blendCoeff );
                        break;
                    case IKType_LookAt:
                        matrix44TargetIKLookAt( request.m_targetMatrix, request.m_maxRotationByBone, request.m_maxIter, request.m_maxValue );
                        break;
                }

                applyIK(request.m_refBoneIdx);
            }
            m_anim.pasteLocalMatrices(m_Gizmos);
            m_requestsIK.clear();
        }   
    }
    //=============================================================================================
    // Animation modification
    //
    void Mesh3DComponent::externalAnimModManagement()
    {
        // External Anim Modification
        if ( m_allowAnimMod )
        {
            EventAnimMod evtAnimMod;
            m_actor->onEvent(&evtAnimMod);
        }
    }

    //=============================================================================================
    // Update
    //
#ifdef ITF_SUPPORT_EDITOR
    #define M_DeltaTimeSave() \
        f32 saveDeltaTime = _deltaTime;\
        _deltaTime *= m_animationTimeRatio;
    #define M_DeltaTimeRestore() \
        _deltaTime = saveDeltaTime;
#else
    #define M_DeltaTimeSave()
    #define M_DeltaTimeRestore()
#endif
    /**/
    void Mesh3DComponent::Update( f32 _deltaTime )
    {

		// Exit on Component Inactive
		if (getInactive())
			return;

		// UPDATE des AnimInput
#ifdef ITF_SUPPORT_EDITOR
		if (m_animInputUpdate)
			m_animInputUpdate->updateAnimInput();
#endif
        
		Super::Update( _deltaTime );

        m_deltaTime = _deltaTime;

        //testtest
        //if (m_inputList.size()) m_inputList[0].setValue(m_input0);
        //if (m_inputList.size() > 1) m_inputList[1].setValue(m_input1);
        //testtest

        m_GizmoOk = bfalse;

        // process animation tree
//	was tested ans was working but since it's not needed I don't bring it back to life (Matt Dutheil)
//         if ((m_animation3DNode.isValid() && !m_anim.getAnimation()) && (!m_anim.m_action.isValid() || m_anim.m_action != m_animation3DNode))
// 		{
//             setAction(m_animation3DNode);
// 		}
        // end process animation tree

        if ( m_anim.getSkeleton())
        {
            if (m_Gizmos.size() != m_anim.getSkeleton()->m_ListBones.size())
                m_Gizmos.resize( m_anim.getSkeleton()->m_ListBones.size() );

            if ( m_anim.readyToPlay())
            {
                M_DeltaTimeSave()

                m_GizmoOk = m_anim.updateEdge( _deltaTime * m_playRate, m_Gizmos);
                //if (m_updateVisibility)
				{
					if (m_anim.getAnimation())
						setVisibility(m_anim.getAnimation());
					else if (m_anim.m_animation3DTree)
						setVisibilityTree(m_anim.m_animation3DTree->getResult());

					m_updateVisibility = bfalse;
				}

                M_DeltaTimeRestore()

                externalAnimModManagement();
                IKManagement();
                
                m_anim.computeGlobal(m_Gizmos); // Update Gizmos with anim player data
            }

            if ( !m_GizmoOk )
            {
                m_anim.getSkeleton()->ComputeGlobal( m_Gizmos );
                m_GizmoOk = btrue;
            }
            else
            {
                m_anim.sendProperties(GetActor());
            }
        }
  
		processColor(_deltaTime);

#ifdef ITF_SUPPORT_EDITOR
        if (m_freezeSkinning) m_GizmoOk = bfalse;
#endif

        if (m_GizmoOk && m_anim.m_Mode != Animation3DPlayer::modeContracted)
        {
            Matrix44 temp;

            switch (m_anim.m_Mode)
            {
            case Animation3DPlayer::modeCine:
                ComputeMatrix( m_FinalMatrix );
                if(m_Gizmos.size())
                {
                    temp = m_FinalMatrix;
                    m_FinalMatrix.mul44(m_Gizmos[0], temp );
                }
                break;

            case Animation3DPlayer::modeExpanded:
            case Animation3DPlayer::modeExpandedAffectPos:
                if (m_anim.m_MagicBoxPrevValid)
                {
                    temp = m_ActorMatrix;
                    m_ActorMatrix.mul44( m_anim.m_MagicBoxPrev, temp );
                    m_anim.m_MagicBoxPrevValid = bfalse;
                }

                if (m_anim.m_Mode == Animation3DPlayer::modeExpandedAffectPos)
                {
                    GetActor()->setPos(GetActor()->getPos() + m_ActorMatrix.T());
                    m_ActorMatrix.setTranslationToZero();
                }

                ComputeMatrix( m_FinalMatrix );
                temp.mul44(m_anim.m_MagicBox, m_FinalMatrix );
                m_FinalMatrix.mul44(m_ActorMatrix, temp );
                break;
            }
        }
        else
            ComputeMatrix( m_FinalMatrix );

        updateAABB();
    }

    //=============================================================================================
    // Update AABB
    //
    void Mesh3DComponent::updateAABB(Mesh3DResource* _mesh3Dres, Mesh3DPrim &_mesh3D)
    {
        if (!_mesh3Dres || !_mesh3Dres->getMesh3D() ) return;

        // old code using 2D aabb, 3D to 2D transform was done by BVtoAABB mesh function
        //AABB aabb;
        //_mesh3Dres->getMesh3D()->BVtoAABB( aabb, m_FinalMatrix );
        //GetActor()->growAABB(aabb);
        AABB3d aabb3d(Vec3d::Zero,Vec3d::Zero);
        _mesh3Dres->getMesh3D()->BVtoAABB3d( aabb3d, m_FinalMatrix );
        GetActor()->growAABB3d(aabb3d);

		AABB forcedaabb;
		if (getForcedAABB(forcedaabb))
            GetActor()->growAABB(forcedaabb);

        _mesh3D.m_bvMin = _mesh3Dres->getMesh3D()->getBVMin() + GetActor()->getPos();
        _mesh3D.m_bvMax = _mesh3Dres->getMesh3D()->getBVMax() + GetActor()->getPos();

        if (m_GizmoOk && m_Gizmos.size() > 0)
        {
            _mesh3D.m_bvMin += m_Gizmos[0].T();
            _mesh3D.m_bvMax += m_Gizmos[0].T();
        }
    }
    /**/
    void Mesh3DComponent::updateAABB()
    {
        Mesh3DResource* mesh3D = static_cast<Mesh3DResource*>(m_mesh3DID.getResource());
        updateAABB( mesh3D, getMesh() );

        for (u32 i = 0; i < m_mesh3DIDList.size(); i++)
        {
            Mesh3DResource* mesh3DInside = static_cast<Mesh3DResource*>(m_mesh3DIDList[i].getResource());
            updateAABB( mesh3DInside, m_3DmeshList[i]);
            //updateAABB( mesh3DInside, getMesh());
        }
    }

	void Mesh3DComponent::changeMaterial(const u32 elementIndex, const u32 materialID)
	{
		if(elementIndex < m_mesh3DIDList.size())
		{
			Mesh3DResource* mesh3D = static_cast<Mesh3DResource*>(m_mesh3DIDList[elementIndex].getResource());
			mesh3D->ChangeMaterial(m_3DmeshList[elementIndex].m_mesh, elementIndex, materialID, m_materialList, getTemplate()->getMaterialList());
		}
	}

    //=============================================================================================
    // Set animation, have to update anim player, and link meshes to animated visibility properties
    //
    void Mesh3DComponent::setSkeleton( Skeleton3DResource *_skeleton3D )
    {
        Skeleton3D *skeleton3D = _skeleton3D ? _skeleton3D->getSkeleton3D() : NULL;
        m_anim.setSkeleton( skeleton3D );
    }


	void Mesh3DComponent::setVisibilityTree(Animation3DTreeResult *_animResult)
	{
		if(_animResult)
		{
			// get visibility properties
			if (m_mesh3DVisibility.size() != m_mesh3DIDList.size())
			{
				m_mesh3DVisibility.resize( m_mesh3DIDList.size());
			}

			for (unsigned int i = 0; i < m_mesh3DIDList.size(); i++)
			{
				if ( !m_mesh3DIDList[i].isValid() ) continue;
				String8 str = m_mesh3DIDList[i].getResource()->getPath().getBasenameWithoutExtension();
				StringID nameID( str );
				int index;

				m_mesh3DVisibility[i] = btrue;

				for (u32 anmResIdx=0; anmResIdx<_animResult->m_anims.size(); anmResIdx++)
				{
                    if(Animation3DInfo* info = _animResult->m_anims[anmResIdx].m_animation3DInfo)
                    {
                        Animation3D * anm3d = info->getAnimation3D();
                        if (anm3d && anm3d->getUserPropertyIndexByName( nameID, index ))
                        {
                            i32 frame = (int)timeToFrame(_animResult->m_anims[anmResIdx].m_timeOffset, 1.0f / anm3d->getFrequency());
                            m_mesh3DVisibility[i] = getUserPropertyAtFrame(anm3d->m_ListUserProperty[index], frame);
                            break;
                        }
                    }
				}
			}
		}
	}

    bbool Mesh3DComponent::getUserPropertyAtFrame(const Animation3D::UserProperty& _userProperty, i32 _frame)
    {
        if (_userProperty.m_keys.size() == 0)
            return btrue;

        i32 startIndex = 0;
        i32 endIndex = _userProperty.m_keys.size()-1;
        for (i32 i=endIndex; i>=startIndex; i--)
        {
            if (_frame >= _userProperty.m_keys[i].m_frame)
                return _userProperty.m_keys[i].m_bool;
        }
        return _userProperty.m_keys[startIndex].m_bool;
    }
	
	void Mesh3DComponent::setVisibility(Animation3D *_anim3D)
	{
		if(_anim3D)
		{
			// get visibility properties
			if (m_mesh3DVisibility.size() != m_mesh3DIDList.size())
			{
				m_mesh3DVisibility.resize( m_mesh3DIDList.size());
			}

			for (unsigned int i = 0; i < m_mesh3DIDList.size(); i++)
			{
				if ( !m_mesh3DIDList[i].isValid() ) continue;
				String8 str = m_mesh3DIDList[i].getResource()->getPath().getBasenameWithoutExtension();
				StringID nameID( str );
				int index;

				m_mesh3DVisibility[i] = btrue;

				if (_anim3D->getUserPropertyIndexByName( nameID, index ))
				{
					m_mesh3DVisibility[i] = getUserPropertyAtFrame(_anim3D->m_ListUserProperty[index], m_anim.getFrame());
				}
			}
		}
	}

    //=============================================================================================
    // Set animation, have to update anim player, and link meshes to animated visibility properties
    //
    void Mesh3DComponent::setAnimation( Animation3DResource *_anim3D, f32 _ratio  )
    {
        if (!_anim3D || !_anim3D->getAnimation3D())
        {
            m_anim.setAnimation(NULL);
            m_mesh3DVisibility.clear();
            return;
        }

        Vec3d posInit = m_actor->getWorldInitialPos();
        m_anim.setAnimation( _anim3D->getAnimation3D(), _ratio, &posInit );

		m_updateVisibility = btrue;
		//setVisibility(_anim3D->getAnimation3D());
    }

    //=============================================================================================
    // Set animation action
    //
    void Mesh3DComponent::setAction( StringID _action )
    {
        m_anim.setAction( _action );
		m_updateVisibility = btrue;
    }

    //*********************************************************************************************
    // Editor function
    //*********************************************************************************************


#ifdef ITF_SUPPORT_EDITOR

    //=============================================================================================
    // Display Editor
    //
    void Mesh3DComponent::DrawEditorMesh3D(ResourceID &_mesh3DID)
    {
        Mesh3DResource* mesh3Dres = static_cast<Mesh3DResource*>(_mesh3DID.getResource());
        if (mesh3Dres && mesh3Dres->getMesh3D())
        {
            /* draw normals */
            if (m_drawNormals) mesh3Dres->getMesh3D()->DrawNormals( m_FinalMatrix );
            /* draw BV */
            if ( m_drawBV) mesh3Dres->getMesh3D()->DrawBV( m_FinalMatrix );
        }
    }

    //=============================================================================================
    // Display Editor
    //
    void Mesh3DComponent::DrawEditor()
    {
        if (m_drawNormals || m_drawBV)
        {
            DrawEditorMesh3D(m_mesh3DID);
            for (u32 i = 0; i < m_mesh3DIDList.size(); i++)
                DrawEditorMesh3D(m_mesh3DIDList[i]);
        }

        if (m_drawSkeleton)  
        {
            Skeleton3DResource *skeleton3D = static_cast<Skeleton3DResource*>(m_skeleton3DID.getResource());
            if (skeleton3D && skeleton3D->getSkeleton3D())
            {
                skeleton3D->getSkeleton3D()->Draw( m_FinalMatrix );
                skeleton3D->getSkeleton3D()->Draw( m_FinalMatrix, m_Gizmos );
            }
        }
    }

    //=============================================================================================
    // Edition static vars
    //
    Path                Mesh3DComponent::m_edit_mesh3DFile;
    ITF_VECTOR<Path>    Mesh3DComponent::m_edit_mesh3DFileList;
    Path                Mesh3DComponent::m_edit_skeleton3DFile;
    Path                Mesh3DComponent::m_edit_animation3DFile;
    ITF_VECTOR<Path>    Mesh3DComponent::m_edit_animation3DFileList;
    Matrix44            Mesh3DComponent::m_edit_orientation;

    //=============================================================================================
    // Things to do when object is moved
    //
    void Mesh3DComponent::onEditorMove(bbool _modifyInitialPos)
    {
        bbool b_ReloadTextureList = bfalse;
        bbool b_ReloadAnimationList = bfalse;
        bbool b_ReloadMeshList = bfalse;

        Super::onEditorMove(_modifyInitialPos);
        
        // Check for texture update
        if (m_materialList.size() != m_previousMaterialListSize)		// texture list increase or decrease
        {
            b_ReloadTextureList = true;
        }
        else
        {
            for (u32 i = 0; i < m_materialList.size(); i++)
            {
                if (!m_materialList[i].hasPathCoherency())
                    b_ReloadTextureList = true;
            }
        }

        // texture change => for now destroy all resources before reloading them (can be optimized : destroy/reload only modified texture 
        if (b_ReloadTextureList)
        {
            UnloadMaterialList();
            LoadMaterialList();
        }

        // check for mesh3D changes
        if ( !m_mesh3DID.isValid() || m_mesh3DFile != m_mesh3DID.getResource()->getPath() )
        {
            UnloadMesh3DID();
            LoadMesh3DID();
        }

        // Check for mesh3D list changes
        if (m_mesh3DFileList.size() != m_mesh3DIDList.size())		// mesh list increase or decrease
        {
            b_ReloadMeshList = true;
        }
        else
        {
            for (u32 i = 0; i < m_mesh3DFileList.size(); i++)
            {
                if (!m_mesh3DIDList[i].isValid())
                {
                    if (!m_mesh3DFileList[i].isEmpty())
                        b_ReloadMeshList = true;
                    continue;
                }
                if ( m_mesh3DIDList[i] ==  getTemplate()->getMesh3DIDFromList(i)) 
                    continue;
                if ( m_mesh3DIDList[i].getResource()->getPath() != m_mesh3DFileList[i])
                    b_ReloadMeshList = true;
            }
        }
        // mesh list changes => for now destroy all resources before reloading them (can be optimized : destroy/reload only modified mesh 
        if (b_ReloadMeshList)
        {
            UnloadMesh3DIDList();
            LoadMesh3DIDList();
        }

        // check for skeleton3D changes
        if ( !m_skeleton3DID.isValid() || m_skeleton3DFile != m_skeleton3DID.getResource()->getPath() )
        {
            UnloadSkeleton3DID();
            LoadSkeleton3DID();
        }

        // check for animation3D changes
        if ( !m_animation3DID.isValid() || m_animation3DFile != m_animation3DID.getResource()->getPath() )
        {
            UnloadAnimation3DID();
            LoadAnimation3DID();
            onResourceLoaded(); // direct call in case of animation already loaded
       }

        // Check for animation3D list changes
        if (m_animation3DFileList.size() != m_animation3DList.size())		// texture list increase or decrease
        {
            b_ReloadAnimationList = true;
        }
        else
        {
            for (u32 i = 0; i < m_animation3DFileList.size(); i++)
            {
                if (!m_animation3DList[i].isValid())
                {
                    if (!m_animation3DFileList[i].isEmpty())
                        b_ReloadAnimationList = true;
                    continue;
                }
                if ( m_animation3DList[i] ==  getTemplate()->getAnimation3DIDFromList(i)) 
                    continue;
                if ( m_animation3DList[i].getResource()->getPath() != m_animation3DFileList[i])
                    b_ReloadAnimationList = true;
            }
        }
        // animation list changes => for now destroy all resources before reloading them (can be optimized : destroy/reload only modified animation 
        if (b_ReloadAnimationList)
        {
            UnloadAnimation3DIDList();
            LoadAnimation3DIDList();
        }

        if(_modifyInitialPos && !m_ignoreModifyInitialPosOnOrientation)
        {
            m_initialOrientation = getOrientation();
        }
        m_ignoreModifyInitialPosOnOrientation = bfalse;
        if (GetActor()->isPhysicalReady())
            Update(0.0f);
    }

    //=============================================================================================
    // things to do at the beginning of loading from editor
    //
    void Mesh3DComponent::onPrePropertyChange()
    {
        m_edit_mesh3DFile = m_mesh3DFile;
        m_edit_mesh3DFileList.resize( m_mesh3DFileList.size());
        for (u32 i = 0; i < m_mesh3DFileList.size(); i++)
            m_edit_mesh3DFileList[i] = m_mesh3DFileList[i];
        m_edit_skeleton3DFile = m_skeleton3DFile;
        m_edit_animation3DFile = m_animation3DFile;
        m_edit_animation3DFileList.resize( m_animation3DFileList.size());
        for (u32 i = 0; i < m_animation3DFileList.size(); i++)
            m_edit_animation3DFileList[i] = m_animation3DFileList[i];
        m_edit_orientation = m_orientation;
    }

    //=============================================================================================
    // things to do at the end of loading from editor
    //
    void postPropertyEditLoad_CheckMesh3D(Path &_mesh3DFile, Path &_editFile )
    {
        if ( (_mesh3DFile != _editFile) && !Mesh3DResource::isValidFilename(_mesh3DFile))
        {
            String8 path;

            _mesh3DFile.toString8(path);
            SYSTEM_ADAPTER->messageBox( "Data error", String8("File not supported for a 3D mesh (must be .m3d)\n  ") + path.cStr(), ITF_MSG_ICONERROR | ITF_MSG_OK, ITF_IDOK );
            _mesh3DFile = _editFile;
        }
    }
    /**/
    void Mesh3DComponent::onPostPropertyChange()
    {
        String8 path;
        
        // check mesh 3D filename
        postPropertyEditLoad_CheckMesh3D( m_mesh3DFile, m_edit_mesh3DFile);
        m_edit_mesh3DFile.clear();

        // Check for mesh3D list filenames
        if (m_mesh3DFileList.size() == m_edit_mesh3DFileList.size())
        {
            for (u32 i = 0; i < m_mesh3DFileList.size(); i++)
                postPropertyEditLoad_CheckMesh3D( m_mesh3DFileList[i], m_edit_mesh3DFileList[i]);
        }
        m_edit_mesh3DFileList.clear();

        // check skeleton3D filename
        if ( (m_skeleton3DFile != m_edit_skeleton3DFile) && !Skeleton3DResource::isValidFilename(m_skeleton3DFile))
        {
            m_skeleton3DFile.toString8(path);
            SYSTEM_ADAPTER->messageBox( "Data error", String8("File not supported for a 3D skeleton (must be .s3d)\n  ") + path.cStr(), ITF_MSG_ICONERROR | ITF_MSG_OK, ITF_IDOK );
            m_skeleton3DFile = m_edit_skeleton3DFile;
        }
        m_edit_skeleton3DFile.clear();

        // check animation3D filename
        if ( (m_animation3DFile != m_edit_animation3DFile) && !Animation3DResource::isValidFilename(m_animation3DFile))
        {
            m_animation3DFile.toString8(path);
            SYSTEM_ADAPTER->messageBox( "Data error", String8("File not supported for a 3D animation (must be .a3d)\n  ") + path.cStr(), ITF_MSG_ICONERROR | ITF_MSG_OK, ITF_IDOK );
            m_animation3DFile = m_edit_animation3DFile;
        }
        m_edit_animation3DFile.clear();

        // Check for animation3D list filenames
        if (m_animation3DFileList.size() == m_edit_animation3DFileList.size())
        {
            for (u32 i = 0; i < m_animation3DFileList.size(); i++)
            {
                if (m_animation3DFileList[i] == m_edit_animation3DFileList[i]) continue;
                if (Animation3DResource::isValidFilename(m_animation3DFileList[i])) continue;

                m_animation3DFileList[i].toString8(path);
                SYSTEM_ADAPTER->messageBox( "Data error", String8("File not supported for a 3D skeleton (must be .s3d)\n  ") + path.cStr(), ITF_MSG_ICONERROR | ITF_MSG_OK, ITF_IDOK );
                m_animation3DFileList[i] = m_edit_animation3DFileList[i];
            }
        }
        m_edit_animation3DFileList.clear();

        if (m_edit_orientation.nearEqual(m_orientation, simd::splatEpsilon6()))
            m_ignoreModifyInitialPosOnOrientation = btrue;
    }

#endif // ITF_SUPPORT_EDITOR

    //*********************************************************************************************
    // Skeleton functions
    //*********************************************************************************************

    //=============================================================================================
    u32  Mesh3DComponent::getBoneCount() const
    {
        return m_Gizmos.size();
    }

    //=============================================================================================
    bbool Mesh3DComponent::hasBones() const
    {
        return (getBoneCount() && m_GizmoOk);
    }

    //=============================================================================================
    bbool Mesh3DComponent::isBoneIndexValid( int _boneIndex ) const
    {
        return (_boneIndex >= 0 && _boneIndex < (int) getBoneCount());
    }
    
    //=============================================================================================
    bbool Mesh3DComponent::getBonePos(u32 _boneIndex, Vec3d& _dest ) const
    {
        if ( !hasBones() || !isBoneIndexValid(_boneIndex)) return bfalse;
        _dest = m_Gizmos[_boneIndex].T();
        return btrue;
    }

    //=============================================================================================
    bbool Mesh3DComponent::getBoneMatrix(u32 _boneIndex, Matrix44& _matrix) const
    {
        if ( !hasBones() || !isBoneIndexValid(_boneIndex)) return bfalse;
        _matrix = m_Gizmos[_boneIndex];
        return btrue;
    }

    //=============================================================================================
    bbool Mesh3DComponent::getBoneGlobalPos(u32 _boneIndex, Vec3d& _dest ) const
    {
        Matrix44 globalMatrix;

        if (!getBoneGlobalMatrix(_boneIndex, globalMatrix))
            return bfalse;
        _dest = globalMatrix.T();
        return btrue;
    }

    //=============================================================================================
    bbool Mesh3DComponent::getBoneGlobalMatrix(u32 _boneIndex, Matrix44& _matrix) const
    {
        Matrix44 boneMatrix;

        if (!getBoneMatrix(_boneIndex, boneMatrix))
            return bfalse;

        if (_boneIndex == 0)
        {
            Matrix44 initMatrix;
            ComputeInitMatrix(initMatrix);
            _matrix.mul( boneMatrix, initMatrix );
        }
        else
            _matrix.mul( boneMatrix, m_FinalMatrix );
        return btrue;
    }

    //=============================================================================================
    bbool Mesh3DComponent::getBoneGlobalMatrixFromFather(u32 _boneIndex, Matrix44& _matrix)
    {
        Matrix44 boneMatrix;

        if (!getBoneMatrix(_boneIndex, boneMatrix))
            return bfalse;

        if (_boneIndex == 0 )
        {
            Matrix44 initMatrix;
            ComputeInitMatrix(initMatrix);
            _matrix.mul( boneMatrix, initMatrix );
        }
        else
        {
            i32 fatherIdx = getSkeleton3D()->m_ListBones[_boneIndex].m_Father;
            Matrix44 father;
            if( fatherIdx != -1 )
            {
                getBoneGlobalMatrixFromFather(fatherIdx, father);
                _matrix.mul( boneMatrix, father );
            }
            else
            {
                ComputeMatrix(father);
                _matrix.mul( boneMatrix, father );
            }
        }
        return btrue;
    }

    //=============================================================================================
    bbool Mesh3DComponent::getPolylineMatrices( const String8 &_name, ITF_VECTOR<Matrix44> &matrixList, bbool _global )
    {
        Skeleton3D *skeleton = getSkeleton3D();
        if (!skeleton) return bfalse;
        return getPolylineMatrices( skeleton->getPolylineIndexByName( _name ), matrixList, _global );
    }

    //=============================================================================================
    bbool Mesh3DComponent::getPolylineMatrices( int _index, ITF_VECTOR<Matrix44> &matrixList, bbool _global )
    {
        Skeleton3D *skeleton = getSkeleton3D();
        if (!skeleton) return bfalse;

        u32 boneIndex, boneCount;
        boneCount = skeleton->getPolylineBoneNumber(_index);
        if (!boneCount) return bfalse;
        
        matrixList.resize(boneCount);
        if (_global)
        {
            for ( boneIndex = 0; boneIndex < boneCount; boneIndex++)
            {
                if (!getBoneGlobalMatrix( skeleton->getPolylineBoneIndex(_index, boneIndex), matrixList[boneIndex]))
                    return bfalse;
            }
        }
        else
        {
            for ( boneIndex = 0; boneIndex < boneCount; boneIndex++)
            {
                if (!getBoneMatrix( skeleton->getPolylineBoneIndex(_index, boneIndex), matrixList[boneIndex]))
                    return bfalse;
            }
        }
        return btrue;
    }

    //=============================================================================================
    bbool Mesh3DComponent::getPolylinePosition( const String8 &_name, ITF_VECTOR<Vec3d> &positionList, bbool _global )
    {
        Skeleton3D *skeleton = getSkeleton3D();
        if (!skeleton) return bfalse;
        return getPolylinePosition( skeleton->getPolylineIndexByName( _name ), positionList, _global );
    }


    //=============================================================================================
    bbool Mesh3DComponent::getPolylinePosition( int _index, ITF_VECTOR<Vec3d> &positionList, bbool _global )
    {
        Skeleton3D *skeleton = getSkeleton3D();
        if (!skeleton) return bfalse;

        u32 boneIndex, boneCount;
        boneCount = skeleton->getPolylineBoneNumber(_index);
        if (!boneCount) return bfalse;

        positionList.resize(boneCount);
        if (_global)
        {
            for ( boneIndex = 0; boneIndex < boneCount; boneIndex++)
            {
                if (!getBoneGlobalPos( skeleton->getPolylineBoneIndex(_index, boneIndex), positionList[boneIndex]))
                    return bfalse;
            }
        }
        else
        {
            for ( boneIndex = 0; boneIndex < boneCount; boneIndex++)
            {
                if (!getBonePos( skeleton->getPolylineBoneIndex(_index, boneIndex), positionList[boneIndex]))
                    return bfalse;
            }
        }
        return btrue;
    }

    // IAnimCommonInterface
    void Mesh3DComponent::IAnimsetAnim( const StringID& _id )
    {
        setAction(_id);
    }

    bbool Mesh3DComponent::IAnimisAnimFinished() const
    {
        return m_animation3DTree && m_animation3DTree->isAnimFinished();
    }

    bbool Mesh3DComponent::IAnimisAnimLooped() const
    {
        return m_animation3DTree && m_animation3DTree->isAnimLooped();
    }

    u32 Mesh3DComponent::IAnimgetNumAnimsPlaying() const
    {
        return m_animation3DTree ? m_animation3DTree->getNumNodes() : 0;
    }

    void Mesh3DComponent::IAnimdbgGetCurAnim( u32 _index, String8& _out ) const
    {
        // TODO
    }

    /////////////////////////////////////////////////////////////////////////////////////////////// 
    // 
    // Mesh3DComponent_template class
    // 
    ///////////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(Mesh3DComponent_Template)
    //-------------------------------------------------
    BEGIN_SERIALIZATION_CHILD(Mesh3DComponent_Template)
        SERIALIZE_OBJECT("forcedAABB", m_forcedAABB);
        SERIALIZE_CONTAINER_OBJECT("materialList", m_materialList)
        SERIALIZE_MEMBER("mesh3D",m_mesh3DFile);
        SERIALIZE_CONTAINER("mesh3Dlist", m_mesh3DFileList )
        SERIALIZE_MEMBER("skeleton3D",m_skeleton3DFile);
        SERIALIZE_MEMBER("animation3D", m_animation3DFile);
        SERIALIZE_MEMBER("defaultColor",m_defaultColor); 
        SERIALIZE_CONTAINER("animation3Dlist", m_animation3DFileList );
        SERIALIZE_OBJECT("animationTree",m_animation3DTree);
        SERIALIZE_OBJECT("animationList", m_animation3DSet );
        SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
        SERIALIZE_MEMBER("force2DRender",m_force2DRendering); 
        SERIALIZE_MEMBER("animationNode", m_animation3DNode);
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////////
    Mesh3DComponent_Template::Mesh3DComponent_Template():
        m_defaultColor(Color::white()),
        m_force2DRendering(btrue)
    {
    } 

    ///////////////////////////////////////////////////////////////////////////////////////////////
    bbool Mesh3DComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);

        // Texture list
        for (u32 i = 0; i < m_materialList.size(); i++)
        {
            m_materialList[i].onLoaded(m_actorTemplate->getResourceContainer());
        }

        // mesh 3D
        if ( !getMesh3DFile().isEmpty() )
            m_mesh3DID = m_actorTemplate->addResource(Resource::ResourceType_Mesh3D, getMesh3DFile());
        // mesh 3D list
        m_mesh3DIDList.resize(m_mesh3DFileList.size());
        for (u32 i = 0; i < m_mesh3DFileList.size(); i++)
        {
            if ( !getMesh3DFileFromList(i).isEmpty() )
                m_mesh3DIDList[i] = m_actorTemplate->addResource(Resource::ResourceType_Mesh3D, getMesh3DFileFromList(i));
        }
        // skeleton 3D
        if ( !getSkeleton3DFile().isEmpty() )
            m_skeleton3DID = m_actorTemplate->addResource(Resource::ResourceType_Skeleton3D, getSkeleton3DFile());
        // animation 3D
        if ( !getAnimation3DFile().isEmpty() )
            m_animation3DID = m_actorTemplate->addResource(Resource::ResourceType_Animation3D, getAnimation3DFile());
        // animation 3D list
        m_animation3DIDList.resize(m_animation3DFileList.size());
        for (u32 i = 0; i < m_animation3DFileList.size(); i++)
        {
            if ( !getAnimation3DFileFromList(i).isEmpty() )
                m_animation3DIDList[i] = m_actorTemplate->addResource(Resource::ResourceType_Animation3D, getAnimation3DFileFromList(i));
        }

        bOk &= m_animation3DTree.onTemplateLoaded(m_actorTemplate->getFile());

        // animation 3D Set
        m_animation3DSet.postSerialize(m_actorTemplate->getResourceContainer());

        // create instance for load in place
        Animation3DTree instanceTree;
        instanceTree.createFromTemplate(&m_animation3DTree);
        CSerializerLoadInPlace serializer;
        serializer.setFactory(GAMEINTERFACE->getAnimation3DTreeNodeInstanceFactory());
        serializer.Init(&m_instanceData);
        instanceTree.Serialize(&serializer,ESerialize_Data_Save);
        serializer.close();

        return bOk;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void Mesh3DComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        Super::onTemplateDelete(_hotReload);

        m_animation3DTree.onTemplateDelete();

        for (u32 i = 0; i < m_materialList.size(); i++)
        {
            m_materialList[i].onUnLoaded(m_actorTemplate->getResourceContainer());
        }

        if ( m_mesh3DID.isValid())
        {
            m_actorTemplate->removeResource(m_mesh3DID);
            m_mesh3DID.invalidate();
        }

        if ( m_skeleton3DID.isValid())
        {
            m_actorTemplate->removeResource(m_skeleton3DID);
            m_skeleton3DID.invalidate();
        }

        if ( m_animation3DID.isValid())
        {
            m_actorTemplate->removeResource(m_animation3DID);
            m_animation3DID.invalidate();
        }

        for (u32 i = 0; i < m_animation3DIDList.size(); i++)
        {
            if ( m_animation3DIDList[i].isValid())
            {
                m_actorTemplate->removeResource(m_animation3DIDList[i]);
                m_animation3DIDList[i].invalidate();
            }
        }

        m_instanceData.clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    Animation3DTree* Mesh3DComponent_Template::createAnimation3DTreeInstance() const
    {
        ArchiveMemory archLoad(m_instanceData.getData(),m_instanceData.getSize());
        CSerializerLoadInPlace serializer;

        serializer.setFactory(GAMEINTERFACE->getAnimation3DTreeNodeInstanceFactory());
        serializer.Init(&archLoad);

        Animation3DTree * ret = serializer.loadInPlace<Animation3DTree>();
        ret->Serialize(&serializer,ESerialize_Data_Load);
        return ret;
    }

	///////////////////////////////////////////////////////////////////////////////////////////////
	u32 Mesh3DComponent_Template::findInputIndex( const StringID& _id ) const
	{
		u32 numInputs = m_inputList.size();

		for ( u32 i = 0; i < numInputs; i++ )
		{
			const InputDesc& input = m_inputList[i];

			if ( input.getId() == _id )
			{
				return i;
			}
		}

		return U32_INVALID;
	}

} // end namespace ITF

