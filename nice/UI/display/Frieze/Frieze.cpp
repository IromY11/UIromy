#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRIEZESTATS_H_
#include "engine/display/Frieze/FriezeStats.h"
#endif // _ITF_FRIEZESTATS_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef _ITF_PROFILERMACRO_H_
#include "engine/profiler/profilerMacro.h"
#endif //_ITF_PROFILERMACRO_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif //_ITF_PREFETCH_FCT_H_

#ifndef ITF_GFX_MATERIAL_SHADER_MANAGER_H_
#include "engine/display/material/GFXMaterialShaderManager.h"
#endif //ITF_GFX_MATERIAL_SHADER_MANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYS2D_H_
#include "engine/Phys2d/Phys2d.h"
#endif //_ITF_PHYS2D_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#include "gameplay/GameMaterial.h"

#include "core/Config.h"

#ifndef ITF_STRIPPED_DATA
    #ifndef _ITF_SERIALIZED_OBJECT_CONTENT_H_
    #include "core/serializer/SerializedObjectContent.h"
    #endif //_ITF_SERIALIZED_OBJECT_CONTENT_H_
#endif // ITF_STRIPPED_DATA

#ifdef ITF_SUPPORT_EDITOR
    #ifndef _ITF_METAFRIEZE_H_
    #include "engine/display/MetaFrieze/MetaFrieze.h"
    #endif //_ITF_METAFRIEZE_H_
#endif // ITF_SUPPORT_EDITOR

#include "engine/display/Frieze3D/Frieze3DData.h"

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _PATHFINDING_H_
#include "engine/pathfinding/Pathfinding.h"
#endif // _PATHFINDING_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

namespace ITF
{    
    IMPLEMENT_OBJECT_RTTI(Frise)

    BEGIN_SERIALIZATION_SUBCLASS(Frise, IndexList)
        SERIALIZE_CONTAINER("List", m_list);  
        SERIALIZE_MEMBER("IdTexConfig", m_idTexConfig);
    END_SERIALIZATION()
        
    BEGIN_SERIALIZATION_SUBCLASS(Frise, MeshBuildData)
        SERIALIZE_CONTAINER_OBJECT("AnimIndexList", m_animIndexList);
        SERIALIZE_CONTAINER_OBJECT("AnimVertexList", m_animVertexList);
        SERIALIZE_CONTAINER_OBJECT("StaticIndexList", m_staticIndexList);
        SERIALIZE_CONTAINER_OBJECT("StaticVertexList", m_staticVertexList);
        SERIALIZE_OBJECT("OverlayIndexList", m_overlayIndexList);
        SERIALIZE_CONTAINER_OBJECT("OverlayVertexList", m_overlayVertexList);          
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Frise, CollisionData)
        SERIALIZE_CONTAINER_OBJECT("LocalCollisionList", m_localCollisionList);
        SERIALIZE_CONTAINER_OBJECT("WorldCollisionList", m_worldCollisionList);    
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Frise, MeshStaticData)
        SERIALIZE_OBJECT("LocalAABB", m_aabbWorld);
        SERIALIZE_OBJECT("WorldAABB", m_aabbLocal);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Frise, MeshAnimData)
        SERIALIZE_OBJECT("LocalAABB", m_aabbWorld);
        SERIALIZE_OBJECT("WorldAABB", m_aabbLocal);    
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Frise, MeshOverlayData)
        SERIALIZE_OBJECT("LocalAABB", m_aabbWorld);
        SERIALIZE_OBJECT("WorldAABB", m_aabbLocal);   
        SERIALIZE_MEMBER("CosAngle", m_cosAngle);
        SERIALIZE_MEMBER("SinAngle", m_sinAngle);
    END_SERIALIZATION()
        
    BEGIN_SERIALIZATION_SUBCLASS(Frise, MeshFluidData)
        SERIALIZE_OBJECT("Data", m_data);
        SERIALIZE_OBJECT("LocalAABB", m_aabbWorld);
        SERIALIZE_OBJECT("WorldAABB", m_aabbLocal);    
    END_SERIALIZATION()

    #define PRIMITIVE_PARAM_LABEL "PrimitiveParameters"

    BEGIN_SERIALIZATION_CHILD(Frise)

#ifdef ITF_SUPPORT_EDITOR
        SERIALIZE_FUNCTION(preProperties,ESerialize_PropertyEdit);
#endif // ITF_SUPPORT_EDITOR

        BEGIN_CONDITION_BLOCK(ESerialize_Checkpoint)  
            SERIALIZE_MEMBER("EventShowColorDst",m_eventShowColorDst)
            SERIALIZE_MEMBER("disablePhysic",m_disablePhysic)
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerialize_DataBin)    
            SERIALIZE_OBJECT("CollisionData", m_pCollisionData);
            SERIALIZE_OBJECT("MeshStaticData", m_pMeshStaticData);
            SERIALIZE_OBJECT("MeshAnimData", m_pMeshAnimData);
            SERIALIZE_OBJECT("MeshOverlayData", m_pMeshOverlayData);
            SERIALIZE_OBJECT("VisualPolyline", m_visualPolyline);
            SERIALIZE_OBJECT("VisualPolyPointLocal", m_visualPolyPointLocal);
            SERIALIZE_OBJECT("MeshBuildData", m_meshBuildData);   
            SERIALIZE_OBJECT("MeshFluidData", m_pMeshFluidData);     
            SERIALIZE_MEMBER("AABB_MinZ", m_aabbMinZ);
            SERIALIZE_MEMBER("AABB_MaxZ", m_aabbMaxZ);
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate)
            SERIALIZE_CONTAINER_OBJECT("LOCAL_POINTS", m_pointsList.getMutableEdgeData());
            SERIALIZE_MEMBER("LOOP", m_pointsList.m_loop);
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data|ESerialize_Editor)
            BEGIN_CONDITION_BLOCK_NOT(ESerialize_Instance)     
                SERIALIZE_OBJECT("PointsList", m_pointsList);
            END_CONDITION_BLOCK_NOT()
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)            
            SERIALIZE_MEMBER("ConfigName", m_templatePath);
            SERIALIZE_MEMBER("SwitchTexturePipeExtremity", m_switchTexturePipeExtremity);
            SERIALIZE_MEMBER("SwitchExtremityStart", m_switchExtremityStart);
            SERIALIZE_MEMBER("SwitchExtremityStop", m_switchExtremityStop);
            SERIALIZE_MEMBER("FillUVOrigin", m_fillUVOrigin);
            SERIALIZE_MEMBER("PreComputedForCook", m_preComputedForCook);
            SERIALIZE_OBJECT("XfForCook", m_xfForCook);
            SERIALIZE_MEMBER("DepthForCook", m_depthForCook);
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
                SERIALIZE_MEMBER("ColorFactor", m_primitiveParamSerialized.m_colorFactor);
                SERIALIZE_MEMBER("ColorFog", m_primitiveParamSerialized.m_colorFog);
                SERIALIZE_MEMBER("useStaticFog", m_primitiveParamSerialized.m_useStaticFog);
                SERIALIZE_MEMBER("renderInRelection", m_primitiveParamSerialized.m_renderInReflection);
            END_CONDITION_BLOCK();
        END_CONDITION_BLOCK();

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_OBJECT(PRIMITIVE_PARAM_LABEL, m_primitiveParamSerialized);
			SERIALIZE_MEMBER("DepthOffset", m_depthOffset);
            SERIALIZE_MEMBER("UvX_Offset",m_uvXoffSet);
            SERIALIZE_MEMBER("UvY_Offset",m_uvYoffSet);
            SERIALIZE_MEMBER("UvX_Flip",m_uvX_flip);
            SERIALIZE_MEMBER("UvY_Flip",m_uvY_flip);
            SERIALIZE_MEMBER("uvY_Tile",m_uvY_tile);
            SERIALIZE_MEMBER("Filling_OffSetUv",m_fillOffSetUv);
            SERIALIZE_MEMBER("Anim_SyncGlobal",m_animGlobalSync);
            SERIALIZE_MEMBER("AtlasIndex", m_atlasIndex);
            SERIALIZE_MEMBER("AtlasTessellationIndex", m_atlasTessellationIndex);
            SERIALIZE_MEMBER("Thickness", m_thickness);
            SERIALIZE_MEMBER("UseMovingCollision", m_useMovingCollision);
            SERIALIZE_MEMBER("UseTemplatePrimitiveParams", m_usePrimParamFcg);
            SERIALIZE_GFX_GRID_FILTER("FluidEmitterFilter", m_fluidEmitterFilter);

            SERIALIZE_ENUM_BEGIN("LockTexture",m_lockTexture);
                SERIALIZE_ENUM_VAR(LockTexture_None);
                SERIALIZE_ENUM_VAR(LockTexture_Top);
                SERIALIZE_ENUM_VAR(LockTexture_Right);
                SERIALIZE_ENUM_VAR(LockTexture_Left);
                SERIALIZE_ENUM_VAR(LockTexture_Bottom);
                SERIALIZE_ENUM_VAR(LockTexture_Switch);
            SERIALIZE_ENUM_END();

            SERIALIZE_MEMBER("MatShader", m_materialShaderOverridePath);
        END_CONDITION_BLOCK()

#ifdef ITF_SUPPORT_EDITOR
        BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
            SERIALIZE_MEMBER("ConfigNameEdit",m_editConfigName);
            SERIALIZE_MEMBER("ColorComputerTagId", m_colorComputerTagId);            
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw|ESerialize_Editor)
            SERIALIZE_MEMBER("IsCenterPivot", m_isCenterPivot);
            SERIALIZE_MEMBER("Time", m_time);
        END_CONDITION_BLOCK()
        BEGIN_CONDITION_BLOCK(ESerialize_Editor)
            SERIALIZE_MEMBER("MetaFriezeRef", m_metaFrieze);
        END_CONDITION_BLOCK()
#endif // ITF_SUPPORT_EDITOR

#ifndef ITF_STRIPPED_DATA
        BEGIN_CONDITION_BLOCK(ESerialize_ForcedValues)
            SERIALIZE_OBJECT("PRIMITIVEPARAMFORCED", m_primitiveParamForced);
        END_CONDITION_BLOCK()
#endif // !ITF_STRIPPED_DATA

#ifdef USE_BOX2D
        SERIALIZE_ENUM_BEGIN("bodyType", m_bodyType)
            SERIALIZE_ENUM_VAR(b2_staticBody);
            SERIALIZE_ENUM_VAR(b2_kinematicBody);
            SERIALIZE_ENUM_VAR(b2_dynamicBody);
        SERIALIZE_ENUM_END();
        
        SERIALIZE_MEMBER("categoryBits", m_categoryBits)
        SERIALIZE_MEMBER("maskBits", m_maskBits)
#endif // USE_BOX2D

        SERIALIZE_MEMBER("Mesh3dSeed", m_seed);
        SERIALIZE_CONTAINER("MeshManualySet", m_meshManualySet);

        SERIALIZE_MEMBER("animSpeedFactor", m_animSpeedFactor);
        SERIALIZE_MEMBER("animAmplitudeFactor", m_animAmplitudeFactor);

    END_SERIALIZATION()

    DECLARE_RASTER(frize_Update, RasterGroup_Misc, Color::blue());
    DECLARE_RASTER(frize_Draw, RasterGroup_Misc, Color::red());        

    bool PrefetchFcg( const Path& path, PrefetchFactory::Params& params )
    {
        /*
        ResourceID  id;
        bbool       alreadyLoaded = bfalse;

        params.type = Resource::ResourceType_FriezeConfig;
        params.refType = PrefetchFactory::Params::Logical;
        {
            csAutoLock cs( RESOURCE_MANAGER->getQueryOpsCs() );

            alreadyLoaded = RESOURCE_MANAGER->getResourceIdFromFile( id, path.getStringID(), Resource::ResourceType_FriezeConfig );
            if ( alreadyLoaded )
            {
                params.res = id.getResource();
                RESOURCE_MANAGER->queryAddUserToResourceLogicalData( id );
                return btrue;
            }
        }
        params.res = Frise::loadFriezeConfig( path, bfalse );
        */
        return btrue;
    }

    Frise::Frise()
        : Pickable()                     
        , m_usePrimParamFcg(btrue)
        , m_time(0.f)        
        , m_atlasIndex(0)
        , m_atlasTessellationIndex(0)
        , m_useMovingCollision(bfalse)
        , m_moveCollisionFrame(0)
        , m_aabbMinZ(0.f)
        , m_aabbMaxZ(0.f)

        , m_meshBuildData(nullptr)

		, m_depthOffset(0.0f)

        // uv
        , m_uvX_flip(bfalse)
        , m_uvY_flip(bfalse)
        , m_uvY_tile(1.f)
        , m_uvXoffSet(0.f)
        , m_uvYoffSet(0.f)

        //  Fill
        , m_fillOffSetUv(Vec2d::Zero)
        , m_fillUVOrigin(Vec2d::Zero)

        // vtx
        , m_thickness(1.f)

        // line data
        ,m_querriedRecomputeData(btrue)

        , m_switchTexturePipeExtremity(0)
        , m_lockTexture(LockTexture_None)

        // mesh data
        , m_pCollisionData(NULL)
        , m_pMeshOverlayData(NULL)
        , m_pMeshAnimData(NULL)
        , m_pMeshStaticData(NULL)
        , m_pMeshFluidData(NULL)

		// 3D
		, m_pFrieze3DBuildData(NULL)
		, m_pFrieze3DRuntimeData(NULL)
        
        , m_matShaderOverride(NULL)

        // switch extremity
        , m_switchExtremityStart(bfalse)
        , m_switchExtremityStop(bfalse)

        , m_preComputedForCook(bfalse)        
        , m_regionRegistered(bfalse)
        , m_disablePhysic(bfalse)
        , m_physicRegistered(bfalse)        

        , m_eventShowTime(1.f)
        , m_depthForCook(0.f)
        , m_animGlobalSync(0.f)
#ifdef USE_BOX2D
        , m_b2dBody(NULL)
        , m_b2dWorld(NULL)
        , m_categoryBits(0xFFFF)
        , m_maskBits(0xFFFF)
#endif//USE_BOX2D

#ifdef SKIP_NODRAW_UPDATE
        , m_skippedDt(0.f)
#endif // SKIP_NODRAW_UPDATE

#ifdef DEVELOPER_JAY_COOK
        , m_hasBeenRecomputed(bfalse)
#endif // DEVELOPER_JAY_COOK
#ifdef USE_BOX2D
        , m_bodyType((u32)b2_staticBody)
#endif //USE_BOX2D
		, m_fluidEmitterFilter(0x0000ffff)
        , m_seed(0)
        , m_animSpeedFactor(1.f)
        , m_animAmplitudeFactor(1.f)
        , m_meshPosition(0)
    {
		setObjectType(BaseObject::eFrise);        

        m_templatePath = Path::EmptyPath;

        // animation vertex
        m_animGlobalRotCur.SetRadians(0.f);
        m_visualPolyline.m_ownerFrise = this;

        m_xfForCook.setIdentity();               

#ifdef ITF_SUPPORT_EDITOR
        m_isCenterPivot = btrue;
        m_prevUseMovingCollision = bfalse;
        m_useAtlasPolygoneEditor = bfalse;
        m_colorComputerTagId = 0;
        m_metaFrieze.invalidate();
        m_isFilling = bfalse;       
#endif
    }

    Frise::~Frise()
    {
        clearComputedData();
    }

    Frise::RecomputeData::~RecomputeData()
    {
        for (ITF_MAP<StringID, Mesh3D*>::iterator meshIter = m_meshCookMap.begin();
            meshIter != m_meshCookMap.end(); meshIter++)
        {
            SF_DEL(meshIter->second);
        }
    }

    void Frise::clearComputedData()
    {
        ITF_ASSERT(!m_physicRegistered);

        SF_DEL(m_meshBuildData);

        clearMeshAnimData();
        clearMeshStaticData();      
        clearCollisionData();
        clearVisualData();
        clearMeshFluidData();
        clearMeshOverlayData();

		SF_DEL(m_pFrieze3DRuntimeData);
		SF_DEL(m_pFrieze3DBuildData);
    }

    void Frise::onDestroy( bbool _hotReload )
    {
        clearComputedData();        

        releaseMaterialOverride();

        Super::onDestroy(_hotReload);
    }

    void Frise::onStartDestroy( bbool _hotReload )
    {
        if ( !_hotReload )
        {
            FRIEZESTAT_DEL_FRIEZE(getRef());
        }

        if ( m_pCollisionData )
        {
            const u32 worldCollisionCount = m_pCollisionData->m_worldCollisionList.size();
            for( u32 i = 0; i < worldCollisionCount; i++)
            {
                if ( m_pCollisionData->m_worldCollisionList[i] )
                {
                    m_pCollisionData->m_worldCollisionList[i]->onStartDestroy();
                }
            }
        }

#ifdef USE_BOX2D
        if (m_b2dBody && m_b2dWorld)
        {
            m_b2dWorld->DestroyBody(m_b2dBody);
            m_b2dWorld = NULL;
            m_b2dBody  = NULL;
        }
#endif

        Super::onStartDestroy(_hotReload);
    }
  
    void Frise::requestDestruction()
    {
        if ( !isDestructionRequested() )
        {
            Super::requestDestruction();
            m_querriedRecomputeData = bfalse;
        }
    }

    void Frise::requestTemplateType()
    {
        m_templatePickable = (FriseConfig*)requestTemplateType(m_templatePath);
    }

    const TemplatePickable* Frise::requestTemplateType(const Path& _path) const
    {
        return TEMPLATEDATABASE->requestTemplate<FriseConfig>(_path,getRef());
    }

    void Frise::onLoaded(HotReloadType _hotReload)
    {
        Super::onLoaded(_hotReload);

        m_disablePhysic = bfalse;
        m_eventShowTime = 1.f;
        resetGFXPrimitiveParamRunTime();

#ifndef ITF_FINAL
        validate();
#endif

        acquireMaterialOverride();

        setUpdateGroup(WorldUpdate::UpdateGroup_Environment);

        if (HotReloadType_Checkpoint != _hotReload)
        {
            if ( m_preComputedForCook )
            {
                executeRecomputeDataCooked();
                checkUpdateTransformForDataCooked();
            }
            else
                forceRecomputeData();
        }        

        getWorldUpdateElement()->setUpdateOnlyOnChange(bfalse);
        if( m_pMeshFluidData )
            m_pMeshFluidData->m_data.resetAllLayerSettings();
        else if ( !m_pMeshOverlayData && !m_pMeshAnimData )
        {
            getWorldUpdateElement()->setUpdateOnlyOnChange();
            getWorldUpdateElement()->setHasChangedForUpdate();
        }

		const FriseConfig * config = getConfig();
        if (config && config->m_methode == Frise::In3D)
		{
			if (!config->m_frieze3DConfig.m_meshRefFillFile.isEmpty())
				addResource(Resource::ResourceType_Mesh3D,config->m_frieze3DConfig.m_meshRefFillFile);
			if (!config->m_frieze3DConfig.m_meshRefLeftFile.isEmpty())
				addResource(Resource::ResourceType_Mesh3D,config->m_frieze3DConfig.m_meshRefLeftFile);
			if (!config->m_frieze3DConfig.m_meshRefRightFile.isEmpty())
				addResource(Resource::ResourceType_Mesh3D,config->m_frieze3DConfig.m_meshRefRightFile);
		}
    }
	
	void Frise::onResourceLoaded()
	{
		Super::onResourceLoaded();

        // need recompute AFTER resource load -> need mesh to be loaded
        if ( !m_preComputedForCook && getConfig()->m_mesh3d.m_mesh3dList.size())
        {
            forceRecomputeData();
        }

		if ( m_pFrieze3DRuntimeData )
		{
			m_pFrieze3DRuntimeData->generate(*m_pFrieze3DBuildData, getConfig()->m_frieze3DConfig, getConfig()->m_textureConfigs, getConfig()->m_frieze3DConfig.m_orient);

			const Transform2d xf( get2DPos(), getAngle(), getScale(), getIsFlipped() );

			updateMeshMatrixAndAabb( xf ); 
			updateAABB( xf );
		}
	}

	void Frise::onResourceUnloaded()
	{
		if ( m_pFrieze3DRuntimeData )
			m_pFrieze3DRuntimeData->reset();
		//getConfig()->m_frieze3DConfig.m_meshRefID.invalidate();

		Super::onResourceUnloaded();
	}

    void Frise::checkUpdateTransformForDataCooked()
    {
        if ( f32_Abs( m_depthForCook -getDepth()) > MTH_EPSILON )
        {
            onDepthChanged(m_depthForCook);
            processUpdateTransform();
        }
        else 
        {
            Transform2d xfCurrent;
            xfCurrent.setFrom( get2DPos(), getAngle(), getScale(), getIsFlipped() );

            if ( !xfCurrent.getPos().IsEqual( m_xfForCook.getPos(), MTH_EPSILON ) 
            ||   !xfCurrent.getScale().IsEqual( m_xfForCook.getScale(), MTH_EPSILON) 
            ||   f32_Abs(getShortestAngleDelta(xfCurrent.getAngle(), m_xfForCook.getAngle())) > MTH_EPSILON )
            {
                processUpdateTransform();
            }
        }
    }

    void Frise::syncMovingCollisions()
    {        
        if ( getWorldUpdateElement() )
        {
            if ( m_useMovingCollision )
            {
                if ( CURRENTFRAME != m_moveCollisionFrame )
                {
                    const u32 polylineCount = m_pCollisionData ? m_pCollisionData->m_worldCollisionList.size() : 0;
                    for (u32 i = 0; i < polylineCount; i++)
                    {
                        PolyLine* polInWorldSpace = m_pCollisionData->m_worldCollisionList[i];

                        if ( polInWorldSpace->m_physShape )
                        {
                            PhysShapeMovingPolyline* movingPoly = static_cast<PhysShapeMovingPolyline*>(polInWorldSpace->m_physShape);
                            movingPoly->sync();
                        }
                    }
                }
                else
                {
                    getWorldUpdateElement()->setHasChangedForUpdate();
                }
            }
        }
    }

    void Frise::updateMovingCollisions()
    {
        if ( getWorldUpdateElement() )
        {
            if ( m_useMovingCollision )
            {
                const u32 polylineCount = m_pCollisionData ? m_pCollisionData->m_worldCollisionList.size() : 0;
                for (u32 i = 0; i < polylineCount; i++)
                {
                    PolyLine* polInWorldSpace = m_pCollisionData->m_worldCollisionList[i];

                    if ( polInWorldSpace->m_physShape )
                    {
                        PhysShapeMovingPolyline* movingPoly = static_cast<PhysShapeMovingPolyline*>(polInWorldSpace->m_physShape);
                        movingPoly->setMoved();
                        polInWorldSpace->m_physBody->updateAABB();
                    }
                }
            }

            m_moveCollisionFrame = CURRENTFRAME;
            getWorldUpdateElement()->setHasChangedForUpdate();
        }
    }

    void Frise::processUpdateTransform()
    {
        if ( getWorldUpdateElement() )
        {
            getWorldUpdateElement()->setHasChangedForUpdate();
        }

        syncMovingCollisions();
        updateTransformationFromLocalToWorld();
        updateConnections();
        updateMovingCollisions();
    }

    void Frise::processUpdateTranslate(const Vec3d & _prevPos) // just translated
    {
        if ( getWorldUpdateElement() )
        {
            getWorldUpdateElement()->setHasChangedForUpdate();
        }

        syncMovingCollisions();
        updateTranslated(_prevPos);
        updateConnections();
        updateMovingCollisions();
    }


    void Frise::processUpdateFluid( const FriseConfig* _pConfig, f32 _elapsed )
    {
        if (_pConfig->m_methode != InFluid && _pConfig->m_methode != InString)
            return;

        ITF_ASSERT_MSG( m_pMeshFluidData || getPosCount() <2, "No data fluid");
        if (!m_pMeshFluidData)
            return;

        switch ( _pConfig->m_methode )
        {
        case InFluid :
            m_pMeshFluidData->m_data.ProcessEdgeFluidList_InFluid(_elapsed);
            break;
        case InString:
            m_pMeshFluidData->m_data.ProcessEdgeFluidList_InString(_elapsed);
            break;
        default:
            break;
        }
    }

    void Frise::update(f32 _elapsed)
    {        
        PRF_M_SCOPE(updateFrise)

#ifdef DEVELOPER_JAY_COOK
//         if ( m_hasBeenRecomputed )
//         {            
//             GFX_ADAPTER->drawDBGAABB( getAABB(), Color::red(), 0.f, 5.f, getDepth() );
//         }
// 
//         if ( m_pCollisionData )
//         {             
// 
//             for(u32 colIdx = 0; colIdx < m_pCollisionData->m_worldCollisionList.size(); ++colIdx)
//             {
//                 PolyLine* pol = m_pCollisionData->m_worldCollisionList[colIdx];
//                 const PolyPointList& pointList = pol->getEdgeData();
// 
//                 for ( u32 i=0; i<pointList.getPosCount() -1; i++)
//                 {
//                     GFX_ADAPTER->drawDBGLine( pointList.getPosAt(i), pointList.getPosAt(i +1), Color::green(), 0.f, 2.f, getDepth());                    
//                 }
//                 
//                 if ( pol->getConnection().getPrevious() )
//                 {
//                     GFX_ADAPTER->drawDBGCircle( pointList.getPosAt(0).to3d(getDepth()), 1.f, Color::green() );
//                     GFX_ADAPTER->drawDBGCircle( pointList.getPosAt(0).to3d(getDepth()), 0.5f, Color::green() );
//                 }
//             }
//         }
#endif // DEVELOPER_JAY_COOK

        PROFILER_UPDATE_PICKABLE(this,getRef());
        TIMERASTER_SCOPE(frize_Update);
        STATS_FRIEZE_UPDATEINC

        const FriseConfig* pConfig = getConfig();
        if ( !pConfig ) // TODO move test in #ifndef ITF_FINAL
            return;

#ifndef ITF_FINAL
        if (hasDataError())
            return;
#endif

        m_time += _elapsed;

        if (m_querriedRecomputeData) // TODO remove this variable
        {
            executeRecomputeData();
        }

        processUpdateColor(_elapsed);
        
#ifdef USE_BOX2D
        if (m_b2dBody && m_b2dBody->GetType() != b2_staticBody)
        {
            setPos(Phys2d::b2Vec2ToVec2d(m_b2dBody->GetPosition()).to3d(getDepth()));
            if (!m_b2dBody->IsFixedRotation())
                setAngle(m_b2dBody->GetAngle());
        }
#endif //USE_BOX2D
        
        syncMovingCollisions();
        processUpdateFluid( pConfig, _elapsed );

        if ( m_pMeshAnimData || m_pMeshOverlayData )
        {
            m_animGlobalRotCur += pConfig->m_vtxAnim.m_globalRotSpeed;
            m_animGlobalRotCur.Snap();

            if ( m_pMeshAnimData )
            {                
                m_pMeshAnimData->m_mesh.getMeshElementMaterial().getMatParams().setvParamsAt(0, GFX_Vector4( m_time, getAnimGlobalSync(), pConfig->m_vtxAnim.m_globalSpeed * m_animSpeedFactor, m_animGlobalRotCur.ToRadians()));
                m_pMeshAnimData->m_mesh.getMeshElementMaterial().getMatParams().setvParamsAt(1, GFX_Vector4( m_animAmplitudeFactor, 0.f, 0.f, 0.f));
            }
            else
            {
                setMeshOverlayDataMatrix( &pConfig->m_vtxAnim );    
            }
        }

        if (m_pMeshFluidData)
            m_pMeshFluidData->m_data.bindFxActor();
    }

    void Frise::processUpdateColor(f32 _dt)
    {
        if ( m_eventShowTime < 1.f ) 
        {
            getWorldUpdateElement()->setHasChangedForUpdate();

            m_eventShowTime += _dt * m_eventShowTimeSpeed;

            Color newColorFactor = Color::Interpolate( m_eventShowColorScr, m_eventShowColorDst, m_eventShowTime );
            m_primitiveParamRunTime.m_colorFactor = newColorFactor;

            if ( m_eventShowTime >= 1.f )
            {
                if ( m_eventShowPauseOnEnd )
                {
                    disable();
                }
                else if ( m_eventShowDestroyOnEnd )
                {
                    requestDestruction();
                }
            }
        }
    }

    void Frise::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        for (u32 i = 0; i < m_eventListeners.size(); ++i)
        {
            Actor * actor = ActorRef(m_eventListeners[i]).getActor();
            if(actor)
            {
                actor->onEvent(_event);
            }
        }

        if ( EventDisableCollision* collision = DYNAMIC_CAST(_event,EventDisableCollision) )
        {
            getWorldUpdateElement()->setHasChangedForUpdate();
            if ( collision->getDisable() )
            {
                m_disablePhysic = btrue;
                unregisterPhysic();
            }
            else // restore
            {
                m_disablePhysic = bfalse;

                if ( isActive() )
                {
                    registerPhysic();
                }
            }
            return;
        }
        else if ( EventPause* onPause = DYNAMIC_CAST(_event,EventPause) )
        {
            getWorldUpdateElement()->setHasChangedForUpdate();
            if ( onPause->getPause() )
                disable();
            else
                enable();
        }
		else if( EventViewportVisibility* eventViewportVisibility = DYNAMIC_CAST(_event,EventViewportVisibility) )
		{
			m_primitiveParamRunTime.m_viewportVisibility = eventViewportVisibility->getViewportVisibility();
		}
        else if ( EventShow* eventShow = DYNAMIC_CAST(_event,EventShow) )
        {
            if(getWorldUpdateElement() == NULL) //Frize not charged because tag system
                return;
            
            getWorldUpdateElement()->setHasChangedForUpdate();
            m_eventShowTime         = 0.f;
            m_eventShowColorScr     = m_primitiveParamRunTime.m_colorFactor;
            m_eventShowPauseOnEnd   = eventShow->getPauseOnEnd();
            m_eventShowDestroyOnEnd = eventShow->getDestroyOnEnd(); 

            if ( eventShow->getIsOverrideColor() )
            {                
                m_eventShowColorDst = eventShow->getOverrideColor();
            }
            else
            {
                m_eventShowColorDst = m_eventShowColorScr;
            }

            if(eventShow->getIsOverrideAlpha())
            {
                if(eventShow->getIsAlphaRatio())
                    m_eventShowColorDst.setAlpha( m_primitiveParamSerialized.m_colorFactor.getAlpha() * eventShow->getAlpha() );
                else
                    m_eventShowColorDst.setAlpha( eventShow->getAlpha() );
            }
            else
            {
                m_eventShowColorDst.setAlpha(m_eventShowColorScr.getAlpha());
            }

            if ( eventShow->getTransitionTime() > 0.f )
            {
                m_eventShowTimeSpeed = 1.f / eventShow->getTransitionTime();
            }
            else
            {
                m_eventShowTimeSpeed = F32_INFINITY;
            }
        }
        else if ( EventTeleport* eventTeleport = DYNAMIC_CAST(_event,EventTeleport) )
        {
            if (eventTeleport->getApplyPosAndAngle())
            {
                setPos(eventTeleport->getPos());
                setAngle(eventTeleport->getAngle());
                
                if ( m_useMovingCollision && eventTeleport->getReset() )
                {
                    const u32 polylineCount = m_pCollisionData ? m_pCollisionData->m_worldCollisionList.size() : 0;
                    for (u32 i = 0; i < polylineCount; i++)
                    {
                        PolyLine* polInWorldSpace = m_pCollisionData->m_worldCollisionList[i];

                        if ( polInWorldSpace->m_physShape )
                        {
                            PhysShapeMovingPolyline* movingPoly = static_cast<PhysShapeMovingPolyline*>(polInWorldSpace->m_physShape);
                            movingPoly->setMoved();
                            movingPoly->sync();
                        }
                    }
                }
            }
        }

        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        switch ( config->m_methode )
        {
        case InFluid:
        case InString:

            ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

            if ( !m_pMeshFluidData )
                break;

            if (EventStickOnPolyline * stickOnPolyEvent = DYNAMIC_CAST(_event,EventStickOnPolyline))
            {
                //case : cushion
                ITF_ASSERT(m_pMeshFluidData->m_data.isCushion());
                if (stickOnPolyEvent->isSticked())
                {
                    Actor * act = (Actor *)stickOnPolyEvent->getSender().getObject();
                    if (act)
                        m_pMeshFluidData->m_data.addActor(stickOnPolyEvent->getSender(), 0.f, act->getRadius(), 1.f, stickOnPolyEvent->getWeight(), bfalse, bfalse );
                }
                else
                    m_pMeshFluidData->m_data.removeActor(stickOnPolyEvent->getSender(), 0.f, bfalse);
            } else if (EventSwiming * swiming = DYNAMIC_CAST(_event,EventSwiming))
            {
                if (swiming->isSwiming())
                    m_pMeshFluidData->m_data.addActor(swiming->getActor(), swiming->getSpeed(), swiming->getRadius(), swiming->getWaterMultiplier(), swiming->getWeight(), swiming->getQueryPosition(), swiming->isNoFx() );
                else
                    m_pMeshFluidData->m_data.removeActor(swiming->getActor(), swiming->getSpeed(), swiming->isNoFx());
            } else if (EventSwimingPlouf * plouf = DYNAMIC_CAST(_event,EventSwimingPlouf))
            {
                ITF_ASSERT(!m_pMeshFluidData->m_data.isCushion());
                f32 fakeMass=1;
                m_pMeshFluidData->m_data.sendPerturbation(plouf->getPos(), plouf->getAlteration(), plouf->getRadius(), plouf->isRandom(), fakeMass);
            }
            else if (EventOnLinkedToWaveGenerator *linkedToWaveGenerator=DYNAMIC_CAST(_event,EventOnLinkedToWaveGenerator))
            {
                if (linkedToWaveGenerator->getIsLinked())
                {
                    m_pMeshFluidData->m_data.addStringWaveGenerator(linkedToWaveGenerator->getWaveGenerator());
                }
                else
                {
                    m_pMeshFluidData->m_data.removeStringWaveGenerator(linkedToWaveGenerator->getWaveGenerator());
                }
            }
            break;
        default:
            break;
        }
    }

    void Frise::clearVisualData()
    {
        m_visualPolyline.clear();
        m_visualPolyPointLocal.clear();
    }

    void Frise::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {        
        PROFILER_BATCHPRIMITIVE_PICKABLE(this,getRef());
        TIMERASTER_SCOPE(frize_Draw);

        renderFrieze(_views);
    }

    void Frise::setGFXPrimitiveParamSerialized( const GFXPrimitiveParam& _param)
    { 

#ifndef ITF_STRIPPED_DATA
        m_primitiveParamForced.setPrimitiveParam( m_primitiveParamSerialized, _param );
#else
        m_primitiveParamSerialized = _param;
#endif // ITF_STRIPPED_DATA

        resetGFXPrimitiveParamRunTime();
    }

    void Frise::setColorFactorRunTime(const Color& _color)
    {
        m_primitiveParamRunTime.m_colorFactor = _color;
    }

    void Frise::setColorFogRunTime(const Color& _color)
    {
        m_primitiveParamRunTime.m_colorFog = _color;
    }

    void Frise::setAlpha( f32 _value )
    { 
        m_primitiveParamRunTime.m_colorFactor.m_a = _value;
    }

    void Frise::initEventShowColors()
    {
        m_eventShowColorScr = m_primitiveParamSerialized.m_colorFactor;
        m_eventShowColorDst = m_primitiveParamSerialized.m_colorFactor;
    }

    void Frise::resetGFXPrimitiveParamRunTime( )
    { 
        m_primitiveParamRunTime = m_primitiveParamSerialized;
        m_primitiveParamRunTime.m_BV = getAABB();   

        initEventShowColors();
    }

    void Frise::renderFrieze( const ITF_VECTOR <class View*>& _views )
    {
        STATS_FRIEZE_DRAWINC

#ifndef ITF_FINAL
        if (hasDataError())
            return;
#endif
        if ( isDestructionRequested() )
            return;
        
        //  mesh static
        if( m_pMeshStaticData )
        {
            m_pMeshStaticData->m_mesh.setCommonParam(m_primitiveParamRunTime);
			if (!is3D())
				GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_pMeshStaticData->m_mesh, getDepth() + m_depthOffset, getRef());
			else
				GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_3D>(_views, &m_pMeshStaticData->m_mesh, getDepth() + m_depthOffset, getRef());
		}

        // mesh anim
        if ( m_pMeshAnimData )
        {                
            m_pMeshAnimData->m_mesh.setCommonParam(m_primitiveParamRunTime);
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_pMeshAnimData->m_mesh, getDepth() + 0.0001f + m_depthOffset, getRef());
        }
        
        // mesh overlay
        if ( m_pMeshOverlayData )
        {          
            m_pMeshOverlayData->m_mesh.setCommonParam(m_primitiveParamRunTime);
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_pMeshOverlayData->m_mesh, getDepth() + m_depthOffset, getRef());
        }

		// frieze 3D
		if ( m_pFrieze3DRuntimeData && m_pFrieze3DBuildData )
		{
			m_pFrieze3DRuntimeData->render(*m_pFrieze3DBuildData, _views, getDepth() + m_depthOffset, getRef(), m_primitiveParamRunTime );
			if (m_pFrieze3DRuntimeData->isGlobalBBRecomputationNeeded())
			{
				// What are the consequences of updating this in render ? Is it needed to defer that ?
				const Transform2d xf( get2DPos(), getAngle(), getScale(), getIsFlipped() );
				updateMeshMatrixAndAabb( xf ); 
				updateAABB( xf );
			}
		}

        if ( m_pMeshFluidData )
        {
            // first compute "true" depth, well, all depth
            f32 minDepth = getDepth();
            f32 maxDepth = getDepth();

            const ITF_VECTOR<EdgeFluidLevel> & edgeFluidListLevel = m_pMeshFluidData->m_data.getEdgeFluidLevels();
            const u32 edgeFluidListCount = edgeFluidListLevel.size();

            for ( u32 i = 0; i<edgeFluidListCount ; i++ )
            {
                const EdgeFluidLevel & edge = edgeFluidListLevel[i];
                const f32 depth = edge.getTrueDepth();

                if (minDepth > depth)
                    minDepth = depth;
                if (maxDepth < depth)
                    maxDepth = depth;
            }

            ITF_VECTOR<FullFluidToDraw> & drawDataList = m_pMeshFluidData->m_data.getDrawDataList();
            u32 nbUsedFullFluidToDraw = 0;

            for ( u32 i = 0; i<edgeFluidListCount ; i++ )
            {
                bbool needCompute = bfalse;
                const EdgeFluidLevel & edge = edgeFluidListLevel[i];

                for (ITF_VECTOR <class View*>::const_iterator it = _views.begin();
                    it != _views.end(); it++)
                {
                    const Camera      * pCam  = (*it)->getCamera();
                    if (pCam->isRectVisible( edge.m_edgeFluid->m_worldAABB, minDepth) ||
                        pCam->isRectVisible( edge.m_edgeFluid->m_worldAABB, maxDepth))
                    {
                        needCompute = btrue;
                        break;
                    }
                }

                if (!needCompute)
                    continue;

                const i32 myMode  = edge.m_data.m_way;
                const f32 myDepth = edge.getDepth();

                FullFluidToDraw * fullToDraw = NULL;
                // check for reuse of a drawdatalist
                for (u32 j = 0; j<nbUsedFullFluidToDraw; ++j)
                {
                    FullFluidToDraw & fullFluid = drawDataList[j];
                    if (myMode == fullFluid.m_mode && f32_Abs(myDepth - fullFluid.m_depth) < MTH_EPSILON)
                    {
                        fullToDraw = &fullFluid;
                    }
                }
                if (fullToDraw == NULL)
                {
                    if (nbUsedFullFluidToDraw>=drawDataList.size())
                    {
                        drawDataList.resize(nbUsedFullFluidToDraw+1u);
                    }
                    FullFluidToDraw & fullElmt = drawDataList[nbUsedFullFluidToDraw];
                    nbUsedFullFluidToDraw++;

                    fullElmt.m_fluidList.clear();
                    fullElmt.m_mode      = myMode;
                    fullElmt.m_depth     = myDepth;
                    fullElmt.setCommonParam(m_primitiveParamRunTime);
                    fullElmt.m_worldAABB = edge.m_edgeFluid->m_worldAABB;

                    fullElmt.setPassFilterFlag(edge.m_edgeFluid->m_material);
                    fullElmt.adjustZPassFilterFlag(GFX_OCCLUDE_INFO_SMALL_OR_TRANSPARENT); // No ZPass for a fluid (probably transparent)

                    fullToDraw = &fullElmt;
                }
                else
                {
                    fullToDraw->m_worldAABB.grow(edge.m_edgeFluid->m_worldAABB);
                }

                fullToDraw->m_fluidList.emplace_back();
                FluidToDraw & fluidElmt = fullToDraw->m_fluidList.back();
                edge.m_edgeFluid->FillFluidToDraw(&edge.m_data, edge.m_layerInfos, edge.m_xf, edge.m_scale, fluidElmt);
                fluidElmt.setMaterialPtr(&edge.m_edgeFluid->m_material);                
            }

            for (ITF_VECTOR<FullFluidToDraw>::iterator prevDrawIt = drawDataList.begin();
                prevDrawIt != drawDataList.end(); ++prevDrawIt)
            {
                GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &(*prevDrawIt), prevDrawIt->m_depth + m_depthOffset, getRef());
            }
        }
    }

    void Frise::onDepthChanged( f32 _prevDepth )
    {
        Super::onDepthChanged(_prevDepth);
        
        const ITF_VECTOR<PolyLine*>* pCollisionData = getCollisionData_WorldSpace();
        const FriseConfig* pConfig = getConfig();

        if ( pCollisionData )
        {
            unregisterPhysic();

            const u32 count = pCollisionData->size();
            for( u32 i = 0; i < count; ++i )
            {
                PolyLine* pCol = (*pCollisionData)[i];
                ITF_ASSERT(pCol);

                pCol->setDepth(getDepth());
                pCol->getConnection().reset();
            }

            if ( !m_querriedRecomputeData && isActive() && !m_disablePhysic ) // if recompute is required then registerPhysic will be call during recompute
            {
                registerPhysic();
            }
        }
        
        m_visualPolyline.setDepth(getDepth());
        
        unregisterRegion();
        if( isActive() && pConfig && pConfig->m_regionId.isValid())
        {
            registerRegion( pConfig );
        }

        if ( !m_querriedRecomputeData )
        {
            processUpdateTransform();
        }

        if ( m_pMeshFluidData )
        {
            ITF_VECTOR<EdgeFluid>& edgeFluidList = m_pMeshFluidData->m_data.getEdgeFluid();

            const u32 edgeFluidListCount = edgeFluidList.size();
            for ( u32 i = 0; i < edgeFluidListCount; ++i )
            {
                EdgeFluid& edgeFluid = edgeFluidList[i];

                edgeFluid.m_depth = getDepth();
            }
        }        
    }

    void Frise::onFlipChanged()
    {
        const FriseConfig* config = getConfig();
        if ( config && !config->m_useFriezeFlipToFlipUV )
        {
            forceRecomputeData();
            return;
        }

        processUpdateTransform();
    }

    void Frise::onFinalizeLoad( bbool _hotReload )
    {
        Super::onFinalizeLoad(_hotReload);

        if ( !_hotReload )
        {
            FRIEZESTAT_ADD_FRIEZE(getRef());
        }

        const FriseConfig* friseConfig = getConfig();

        if (!m_usePrimParamFcg)
            //m_usePrimParamFcg = memcmp(&GFXPrimitiveParam::getDefault(), &m_primitiveParamSerialized, sizeof(GFXPrimitiveParam)) == 0;
            m_usePrimParamFcg = m_primitiveParamSerialized == GFXPrimitiveParam::getDefault();

        if ( friseConfig )
        {
            ResourceContainer* resourceContainer = getResourceContainer();

            //if (m_usePrimParamFcg && memcmp(&GFXPrimitiveParam::getDefault(), &friseConfig->m_primitiveParam, sizeof(GFXPrimitiveParam)) != 0 )
            if (m_usePrimParamFcg && !(friseConfig->m_primitiveParam == GFXPrimitiveParam::getDefault()) )
            {
#ifndef ITF_STRIPPED_DATA
                friseConfig->m_primitiveParamForced.setPrimitiveParamInvert(m_primitiveParamSerialized, friseConfig->m_primitiveParam);
                resetGFXPrimitiveParamRunTime();
#endif
            }

            if ( friseConfig->m_gameMaterialTemplate )
            {
                resourceContainer->addChild(friseConfig->m_gameMaterialTemplate->getResourceContainer());
#ifdef USE_BOX2D
                if (m_categoryBits == 0xFFFF)
                {
                    m_categoryBits = (u16)friseConfig->m_gameMaterialTemplate->getFilter();
                }
#endif
            }

            if ( friseConfig->m_gameMaterialStartExtremityTemplate )
            {
                resourceContainer->addChild(friseConfig->m_gameMaterialStartExtremityTemplate->getResourceContainer());
            }

            if ( friseConfig->m_gameMaterialStopExtremityTemplate )
            {
                resourceContainer->addChild(friseConfig->m_gameMaterialStopExtremityTemplate->getResourceContainer());
            }

            const u32 fluidLayersCount = friseConfig->m_fluid.m_layers.size();

            for ( u32 i = 0; i < fluidLayersCount; i++ )
            {
                const FluidFriseLayer &layer = friseConfig->m_fluid.m_layers[i];

                if ( layer.m_gameMaterialTemplate )
                {
                    resourceContainer->addChild(layer.m_gameMaterialTemplate->getResourceContainer());
                }

                if ( layer.m_backsideGameMaterialTemplate )
                {
                    resourceContainer->addChild(layer.m_backsideGameMaterialTemplate->getResourceContainer());
                }
            }

            const u32 textureConfigCount = friseConfig->m_textureConfigs.size();

            for (u32 i = 0; i < textureConfigCount ; i++)
            {
                const FriseTextureConfig& texConfig = friseConfig->m_textureConfigs[i];

                if ( texConfig.getGameMaterial() )
                {
                    resourceContainer->addChild(texConfig.getGameMaterial()->getResourceContainer());
                }
            }
        }
        
#ifdef USE_BOX2D
        if (m_categoryBits == 0xFFFF)
        {
            m_categoryBits = 0x0001;
        }
#endif

        if (m_pMeshFluidData && friseConfig)
            m_pMeshFluidData->m_data.spawnFxActor(friseConfig->m_fluid.m_fxActor);

        m_moveCollisionFrame = CURRENTFRAME;

#if !defined(ITF_DISABLE_WARNING)

        if (CONFIG && CONFIG->m_uselessFriezeWarning && friseConfig)
        {
            const bbool hasVisual = m_pMeshStaticData || m_pMeshAnimData || m_pMeshOverlayData || m_pMeshFluidData;
            const bbool hasCollision = m_pCollisionData != NULL;            
            const bbool hasRegion =  friseConfig->m_regionId.isValid();
            const bbool isDigShape = friseConfig->m_isDigShape;

            ITF_WARNING(this, hasVisual || hasCollision || hasRegion || isDigShape, "Useless frieze detected without visual, collision or region");
        }

#endif // !defined(ITF_DISABLE_WARNING)
    }

    void Frise::saveCheckpointData()
    {
#ifndef ITF_FINAL
        if ( !hasDataError() )
#endif
        {
            ActorsManager::FriseDataContainer* dataContainer = ACTORSMANAGER->getSerializeDataFrise(this);
            if (dataContainer)
            {
                if (dataContainer->m_checkpoint)
                    dataContainer->m_checkpoint->rewindForWriting();
                else
                    dataContainer->m_checkpoint = newAlloc(mId_SavegameManager,ArchiveMemory);

                CSerializerObjectBinary serializeBin;
                serializeBin.Init(dataContainer->m_checkpoint);
                Serialize(&serializeBin,ESerialize_Checkpoint_Save);
            }
        }
    }

    void Frise::setDefaultDataForCheckpoint()
    {
        m_disablePhysic = bfalse;
        //m_eventShowColorDst = Color();
    }

    void Frise::preLoadChekpointData()
    {
        resetTransformationToInitial();
        resetGFXPrimitiveParamRunTime();
        setDefaultDataForCheckpoint();
        initPause();
    }

    void Frise::postLoadCheckpointData()
    {
#ifndef ITF_FINAL
        if ( !hasDataError() )
#endif
        {
            ActorsManager::FriseDataContainer* dataContainer = ACTORSMANAGER->getSerializeDataFrise(this);
            if (dataContainer && dataContainer->m_checkpoint && dataContainer->m_checkpoint->getSize())
            {
                dataContainer->m_checkpoint->rewindForReading();
                CSerializerObjectBinary serializeBin;
                serializeBin.Init(dataContainer->m_checkpoint);
                Serialize(&serializeBin,ESerialize_Checkpoint_Load);

                m_eventShowColorScr = m_primitiveParamRunTime.m_colorFactor = m_eventShowColorDst;
            }
        }
    }
    
#ifdef ITF_SUPPORT_EDITOR
    void Frise::preProperties()
    {
        m_editConfigName = m_templatePath;
        m_prevUseMovingCollision = m_useMovingCollision;
        m_editPrimitiveParam = m_primitiveParamSerialized;
    }

    void Frise::onPostPropertyChange()
    {
        if ( !(m_editPrimitiveParam == m_primitiveParamSerialized))
        {
            if (getUsePrimParamFcg())
            {
                const FriseConfig* friseConfig = getConfig();
                if (friseConfig)
                {
                    GFXPrimitiveParam   m_editPrimitiveParamAfter = m_primitiveParamSerialized;
                    friseConfig->m_primitiveParamForced.setPrimitiveParamInvert( m_editPrimitiveParamAfter, friseConfig->m_primitiveParam );
                    friseConfig->m_primitiveParamForced.setPrimitiveParamInvert( m_editPrimitiveParam, friseConfig->m_primitiveParam );
                    if (m_editPrimitiveParam == m_editPrimitiveParamAfter)
                        setUsePrimParamFcg( bfalse );
                }
                else
                    setUsePrimParamFcg( bfalse );
            }

            resetGFXPrimitiveParamRunTime();
        }

        acquireMaterialOverride();

        if ( m_prevUseMovingCollision != m_useMovingCollision )
        {
            setHotReloadAfterModif();
        }
        else if ( getTemplatePath() != m_editConfigName )
        {
            hotReloadReset(btrue);
            setTemplatePath(m_editConfigName);
            setHotReloadAfterModif();
        }
        else
        {
            forceRecomputeData();
        }

        Super::onPostPropertyChange();
    }
#endif // ITF_SUPPORT_EDITOR

    void Frise::acquireMaterialOverride()
    {
        releaseMaterialOverride();
        if (!m_materialShaderOverridePath.isEmpty())
        {
            m_matShaderOverride = GFXMaterialShaderManager::getInstance()->acquire(m_materialShaderOverridePath);
        }
    }

    void Frise::releaseMaterialOverride()
    {
        if (m_matShaderOverride)
        {
            GFXMaterialShaderManager::getInstance()->release(m_matShaderOverride);
            m_matShaderOverride = NULL;
        }
    }

    void Frise::registerPhysic()
    {
        if ( !m_physicRegistered )
        {
            m_physicRegistered = btrue;

#ifdef USE_BOX2D
            if (m_b2dBody)
                m_b2dBody->SetActive(1);
#endif

#ifdef USE_PHYSWORLD
            if ( m_pCollisionData )
            {
                const u32 collisionCount = m_pCollisionData->m_worldCollisionList.size();
                for (u32 i = 0; i < collisionCount; i++)
                {
                    PolyLine* pPoly = m_pCollisionData->m_worldCollisionList[i];
                    ITF_ASSERT(!pPoly->m_ownerActor.getActor());
                    ITF_ASSERT_CRASH(pPoly->m_physBody,"Physic body was not allocated");
                    PHYSWORLD->insertBody(pPoly->m_physBody, getDepth());
#ifdef USE_PATHFINDING
                    PATHFINDING->updatePolyline(pPoly);
#endif // USE_PATHFINDING
					GFX_ADAPTER->getGridFluidManager().addPolyLine(pPoly);
                }   

                updateConnections();
            }
#endif
        }
    }

    void Frise::unregisterPhysic()
    {
        if ( m_physicRegistered )
        {
            m_physicRegistered = bfalse;

#ifdef USE_BOX2D
            if (m_b2dBody)
                m_b2dBody->SetActive(0);
#endif

#ifdef USE_PHYSWORLD
            if ( m_pCollisionData )
            {
                const u32 collisionCount = m_pCollisionData->m_worldCollisionList.size();
                for (u32 i = 0; i < collisionCount; i++)
                {
                    PolyLine* pPoly = m_pCollisionData->m_worldCollisionList[i];
                    ITF_ASSERT(!pPoly->m_ownerActor.getActor());
                    ITF_ASSERT_CRASH(pPoly->m_physBody,"Physic body was not allocated");
                    PHYSWORLD->removeBody(pPoly->m_physBody);
#ifdef USE_PATHFINDING
                    PATHFINDING->removePolyline(pPoly);
#endif // USE_PATHFINDING
					GFX_ADAPTER->getGridFluidManager().removePolyLine(pPoly);
                }
            }
#endif
        }
    }

    void Frise::onBecomeActive()
    {
        Super::onBecomeActive();
        
        if ( !m_disablePhysic )
        {
            registerPhysic();
        }
        
        const FriseConfig* pConfig = getConfig();
        if( pConfig && pConfig->m_regionId.isValid())
        {
            registerRegion( pConfig );
        }
        
        WORLD_MANAGER->unregisterChildDependencies(this);
    }
    
    void Frise::onBecomeInactive()
    {
        Super::onBecomeInactive();

        unregisterPhysic();
        unregisterRegion();
    }

    void Frise::forceRecomputeData( bbool _isPreComputingForCook /*= bfalse*/ )
    {
        recomputeData();
        executeRecomputeData( _isPreComputingForCook );
    }

    void Frise::erasePosAt(u32 _index)
    {
        m_pointsList.erasePosAt(_index); 

#ifdef  ITF_SUPPORT_EDITOR
        m_pickingInfo2D = NULL;
#endif //ITF_SUPPORT_EDITOR

        recomputeData();
    }

    void Frise::removeAutoBlendCopyMode()
    {
        // if colorFactor alpha is no more opaque, change blend-copy to blend-alpha
        if (m_primitiveParamRunTime.m_colorFactor.m_a != 1.f)
        {
            removeAutoBlendCopyMode(m_pMeshStaticData->m_mesh);
            removeAutoBlendCopyMode(m_pMeshAnimData->m_mesh);
            removeAutoBlendCopyMode(m_pMeshOverlayData->m_mesh);
        }
    }

    void Frise::removeAutoBlendCopyMode(ITF_Mesh & _mesh)
    {
        for(ux i = 0, n = _mesh.getNbMeshElement(); i<n; ++i)
        {
            GFX_MATERIAL & mat = _mesh.getMeshElementMaterial();
            if (mat.getBlendMode() == GFX_BLEND_COPY)
            {
                mat.setBlendMode(GFX_BLEND_ALPHA); 
            }
        }
    }

 
#ifdef ITF_SUPPORT_EDITOR
    Pickable* Frise::clone(Scene* _pDstScene, bbool _callOnLoaded) const
    {
        ITF_ASSERT_MSG(_pDstScene, "A valid scene must be provided");

        Frise* localFrieze = newAlloc(mId_Frieze, Frise());
        BaseObjectClone(const_cast<Frise*>(this), localFrieze);

        localFrieze->onCreate();

        if ( !localFrieze->getInstanceDataFile().isEmpty() )
        {
            localFrieze->loadInstanceData(bfalse);
        }

        // Register into the scene
        if (_pDstScene)
        {
            localFrieze->generateUniqueName(_pDstScene);
            _pDstScene->registerPickable(localFrieze);
        }

        if(_callOnLoaded && !_pDstScene)
        {
            localFrieze->onLoaded(HotReloadType_Default);
            localFrieze->onFinalizeLoad(bfalse);
        }
        else
            localFrieze->resetTransformationToInitial();

        // Link
        const ObjectRefList *parents = LINKMANAGER->getParents(getRef());
        if(parents)
        {
            SafeArray<BaseObject*> resolvedRefs;
            ID_SERVER->getObjectListNoNull(*parents, resolvedRefs);
            for(u32 i = 0; i < resolvedRefs.size(); ++i)
            {
                BaseObject* const obj = resolvedRefs[i];
                Actor* actor = DYNAMIC_CAST(obj, Actor);
                if(actor == NULL)
                    continue;
                LinkComponent* linkComp = actor->GetComponent<LinkComponent>();
                if(linkComp == NULL)
                    continue;

                ObjectPath relPath;
                SceneObjectPathUtils::getRelativePathFromObject(actor,localFrieze,relPath);
                if ( relPath.isValid())
                {
                    //UNDOREDO_MANAGER->preChange(actor);
                    linkComp->editor_addChild(relPath);
                }
            }
        }

        return localFrieze;
    }

    void Frise::onEditorCreated( Pickable* _original )
    {
        if ( _original )
        {
            m_fillUVOrigin = ((Frise*)_original)->m_fillUVOrigin;
        }
        else if ( m_fillUVOrigin == Vec2d::Zero )
        {
            m_fillUVOrigin = get2DPos();
        }
            
        forceRecomputeData();
    }

    void Frise::onEditorCenterPivot()
    {
        u32 posCount = getPosCount();
        if ( isLooping() && posCount >2 )
            posCount --;

        if ( posCount >0 )
        {
            Vec2d center(getPosAt(0));
            for ( u32 i=1; i<posCount; i++)
            {
                center += getPosAt(i);              
            }

            center /= (f32)posCount;
            transformLocalPosToWorld(center);

            onEditorTranslatePivot( center );
        }
    }

    void Frise::onEditorMove( bbool _modifyInitialPos )
    {
        Super::onEditorMove(_modifyInitialPos);

        processUpdateTransform();
    }

    void Frise::onEditorTranslatePivot( const Vec2d& _newPos )
    {
        Vec2d deltaPos( _newPos);
        transformWorldPosToLocal( deltaPos );

        if ( !deltaPos.IsEqual( Vec2d::Zero, 0.01f ))
        {
            const u32 posCount = getPosCount();
            for ( u32 i=0; i<posCount; i++)
            {
                setPosAt( getPosAt(i) -deltaPos, i);
            }

            // fill uvs
            m_fillUVOrigin += deltaPos;

            const Vec3d newPos = _newPos.to3d(getDepth());
            setWorldInitialPos(newPos, btrue);
            setPos(newPos);
        }

        onEditorRecomputeData();
    }

    bbool Frise::hasLoopingMethode() const
    {
        if ( const FriseConfig* config = getConfig() )
        {
            switch ( config->m_methode)
            {
            case Frise::InExtremity:
            case Frise::InExtremitySimple:
            case Frise::InExtremityLong:
            case Frise::InAtlas:            
                return bfalse;
                break;

            default:
                return btrue;
                break;
            }
        }

        return bfalse;
    }

    bbool Frise::hasSwitchExtremityMethode() const
    {
        if ( const FriseConfig* config = getConfig() )
        {
            switch ( config->m_methode)
            {
            case Frise::InExtremity:
                return !config->m_switchExtremityAuto;

            case Frise::InExtremityLong:
            case Frise::InPipePatchSimple:            
                return btrue;

            default:
                return bfalse;
            }
        }

        return bfalse;
    }

    void Frise::updateMetaFriezeDataError()
    {
        if ( hasMetaFriezeOwner() )
        {
            MetaFrieze* pMetaFrieze = DYNAMIC_CAST(getMetaFriezeOwner().getObject(), MetaFrieze);
            if ( pMetaFrieze )
            {
                pMetaFrieze->updateDataError();
            } else
            {
                m_metaFrieze.invalidate();
            }
        }
    }

    void Frise::replaceReinitDataByCurrent()
    {
        if ( !isSerializable() )
        {
            return;
        }

        if(ACTORSMANAGER->isFriseRegistered(this))
        {
            ACTORSMANAGER->unregisterSerializeDataFrise(this);
            ACTORSMANAGER->registerSerializeDataFrise(this);
            //saveCheckpointData();
        }
    }

#endif // ITF_SUPPORT_EDITOR

    void Frise::updateTransformationFromLocalToWorld()
    {
        const Transform2d xf( get2DPos(), getAngle(), getScale(), getIsFlipped() );

        updateMeshMatrixAndAabb( xf ); 
        updatePolylineCollision( xf );
        updateVisualPolyline( xf );
        updateFluidData( xf );
        updateAABB( xf );   
    }

    void Frise::updateTranslated(const Vec3d & _prevPos)
    {
        const Vec3d deltaTranslate = getPos() - _prevPos;
        if ( f32_Abs(deltaTranslate.z()) >= MTH_EPSILON)
        {
            updateTransformationFromLocalToWorld();
        }

        Vec2d deltaTranslate2D;
        deltaTranslate.truncateTo2D(deltaTranslate2D);

        updateMeshMatrixAndAabb( deltaTranslate2D ); 
        updatePolylineCollision( deltaTranslate2D );
        updateVisualPolyline( deltaTranslate2D );
        updateFluidData( deltaTranslate2D );
        m_primitiveParamRunTime.m_BV = getAABB();
    }


    void Frise::updateFluidData( const Transform2d& _xf )
    {
        if ( m_pMeshFluidData )
        {
            ITF_VECTOR<EdgeFluidLevel> & edgeFluidListLevel = m_pMeshFluidData->m_data.getEdgeFluidLevels();
            const u32 edgeFluidListCount = edgeFluidListLevel.size();

            for ( u32 i = 0; i<edgeFluidListCount ; i++ )
            {
                EdgeFluidLevel & edge = edgeFluidListLevel[i];
                edge.m_xf = _xf;
                edge.m_scale = getScale().y();

                EdgeFluid *edgeFluid = edge.m_edgeFluid;
                const Vec2d p0 = _xf.transformPos( edgeFluid->m_pos[0] );
                const Vec2d p1 = _xf.transformPos( edgeFluid->m_pos[1] );
                const Vec2d p2 = _xf.transformPos( edgeFluid->m_pos[2] );
                const Vec2d p3 = _xf.transformPos( edgeFluid->m_pos[3] );
                const Vec2d sight = p2 - p0;

                edgeFluid->m_worldNorm = sight.norm();
                edgeFluid->m_worldAngle = atan2(sight.y(), sight.x());

                if ( edgeFluid->m_worldNorm )
                    edgeFluid->m_worldHeight = (p1 - p0).dot(sight.getPerpendicular()/edgeFluid->m_worldNorm);
                else
                    edgeFluid->m_worldHeight = 0.f;

                transformLocalAabbToWorld( edgeFluid->m_localAABB, edgeFluid->m_worldAABB, _xf );
            }

            transformLocalAabbToWorld( m_pMeshFluidData->m_aabbLocal, m_pMeshFluidData->m_aabbWorld, _xf );
        }
    }

    void Frise::updateFluidData( const Vec2d & _deltaPos)
    {
        if ( m_pMeshFluidData )
        {
            ITF_VECTOR<EdgeFluidLevel> & edgeFluidListLevel = m_pMeshFluidData->m_data.getEdgeFluidLevels();
            const u32 edgeFluidListCount = edgeFluidListLevel.size();

            for ( u32 i = 0; i<edgeFluidListCount ; i++ )
            {
                EdgeFluidLevel & edge = edgeFluidListLevel[i];
                edge.m_xf.m_pos += _deltaPos;
                edge.m_scale = getScale().y();

                EdgeFluid *edgeFluid = edge.m_edgeFluid;
                const Vec2d p0 = edgeFluid->m_pos[0] + _deltaPos;
                const Vec2d p1 = edgeFluid->m_pos[1] + _deltaPos;
                const Vec2d p2 = edgeFluid->m_pos[2] + _deltaPos;
                const Vec2d p3 = edgeFluid->m_pos[3] + _deltaPos;
                const Vec2d sight = p2 - p0;

                edgeFluid->m_worldNorm = sight.norm();
                edgeFluid->m_worldAngle = atan2(sight.y(), sight.x());

                if ( edgeFluid->m_worldNorm )
                    edgeFluid->m_worldHeight = (p1 - p0).dot(sight.getPerpendicular()/edgeFluid->m_worldNorm);
                else
                    edgeFluid->m_worldHeight = 0.f;

                edgeFluid->m_worldAABB.Translate(_deltaPos);
            }

            m_pMeshFluidData->m_aabbWorld.Translate(_deltaPos);
        }
    }

    void Frise::setLocalToWorldMatrix( Matrix44* _pMatrix )
    {
        _pMatrix->setRotationZ(getAngle());
        _pMatrix->setTranslation(getPos());

        Vec3d scale( getScale().x(), getScale().y(), ( getScale().x() +getScale().y() ) *0.5f );
        if ( getIsFlipped() )
            scale.x() *= -1.f;

        _pMatrix->mulScale(scale);
    }

    void Frise::updatePolylineCollision( const Transform2d& _xf )
    {
        if ( !m_pCollisionData )
            return;

        ITF_ASSERT( m_pCollisionData->m_worldCollisionList.size() > 0 );
        ITF_ASSERT( m_pCollisionData->m_worldCollisionList.size() == m_pCollisionData->m_localCollisionList.size() );

#ifdef USE_BOX2D
        initPhys2d();
#endif

        const u32 polylineCount = m_pCollisionData->m_worldCollisionList.size();        
        for (u32 i = 0; i < polylineCount; i++)
        {
            const PolyPointList& polInLocalSpace = m_pCollisionData->m_localCollisionList[i];
            PolyLine* polInWorldSpace = m_pCollisionData->m_worldCollisionList[i];

            // reset connection
            polInWorldSpace->m_connection.reset();
            
            ITF_ASSERT( polInLocalSpace.getPosCount() == polInWorldSpace->getPosCount() );

            const u32 posCount = polInWorldSpace->getPosCount();

            if ( getIsFlipped() )
            {
                for( u32 j=0; j< posCount; j++)
                {
                    polInWorldSpace->setPosAt( _xf.transformPos( polInLocalSpace.getPosAt(posCount -1 -j) ), j);
                }
            }
            else
            {
                for( u32 j=0; j< posCount; j++)
                {
                    polInWorldSpace->setPosAt( _xf.transformPos( polInLocalSpace.getPosAt(j) ), j);
                }
            }

            polInWorldSpace->forceRecomputeData(); // recompute polyline     
            if ( polInWorldSpace->getPhysBody() )
            {
                polInWorldSpace->getPhysBody()->setRecomputeAABB();
                polInWorldSpace->getPhysBody()->updateAABB(); // recompute phys body aabb
            }

#ifdef USE_BOX2D
            addPhys2dCollision(polInLocalSpace);
#endif
        }        
    }

    void Frise::updatePolylineCollision( const Vec2d & _delta )
    {
        if ( !m_pCollisionData )
            return;

        ITF_ASSERT( m_pCollisionData->m_worldCollisionList.size() > 0 );
        ITF_ASSERT( m_pCollisionData->m_worldCollisionList.size() == m_pCollisionData->m_localCollisionList.size() );

        const u32 polylineCount = m_pCollisionData->m_worldCollisionList.size();        
        for (u32 i = 0; i < polylineCount; i++)
        {
            PolyLine* polInWorldSpace = m_pCollisionData->m_worldCollisionList[i];

            // reset connection
            polInWorldSpace->m_connection.reset();

            polInWorldSpace->translate(_delta);
        }        
    }

    void Frise::transformLocalAabbToWorld( const AABB& _local, AABB& _world, const Transform2d& _xf )
    {
        _world.setMinAndMax( _xf.transformPos(_local.getMin()) );
        _world.grow( _xf.transformPos(_local.getMinXMaxY()) );
        _world.grow( _xf.transformPos(_local.getMax()) );
        _world.grow( _xf.transformPos(_local.getMaxXMinY()) );
    }

    void Frise::updateVisualPolyline( const Transform2d& _xf )
    {
        const u32 posCount = m_visualPolyPointLocal.getPosCount();

        if ( posCount )
        {
            ITF_ASSERT( posCount == m_visualPolyline.getPosCount() );

            if ( getIsFlipped() )
            {
                for( u32 j=0; j< posCount; j++)
                {
                    m_visualPolyline.setPosAt( _xf.transformPos( m_visualPolyPointLocal.getPosAt(posCount -1 -j) ), j);
                }
            }
            else
            {
                for( u32 j=0; j< posCount; j++)
                {
                    m_visualPolyline.setPosAt( _xf.transformPos( m_visualPolyPointLocal.getPosAt(j) ), j);
                }
            }

            m_visualPolyline.forceRecomputeData();            
        }
    }

    void Frise::updateVisualPolyline( const Vec2d& _deltaPos)
    {
        const u32 posCount = m_visualPolyPointLocal.getPosCount();

        if ( posCount )
        {
            ITF_ASSERT( posCount == m_visualPolyline.getPosCount() );

            m_visualPolyline.translate(_deltaPos);
        }
    }

    void Frise::initializeMovingPolylinesAfterRecomputeData()
    {
        if ( m_pCollisionData )
        {
            const u32 polylineCount = m_pCollisionData->m_worldCollisionList.size();
            for (u32 i = 0; i < polylineCount; i++)
            {
                PolyLine* polInWorldSpace = m_pCollisionData->m_worldCollisionList[i];

                if ( m_useMovingCollision && polInWorldSpace->m_physShape )
                {
                    PhysShapeMovingPolyline* movingPoly = static_cast<PhysShapeMovingPolyline*>(polInWorldSpace->m_physShape);
                    movingPoly->setMoved();
                    movingPoly->sync();
                }
                if ( polInWorldSpace->m_physBody )
                {
                    polInWorldSpace->m_physBody->updateAABB();
                }
            }
        }
    }

    void Frise::registerRegion( const FriseConfig* _pConfig )
    {
        if ( !m_regionRegistered )
        {
            REGIONS_MANAGER->addRegion(m_visualPolyline.getRef(), _pConfig->m_regionId, getDepth());
            m_regionRegistered = btrue;
        }
    }

    void Frise::unregisterRegion()
    {
        if ( m_regionRegistered )
        {
            REGIONS_MANAGER->removeRegion(m_visualPolyline.getRef());
            m_regionRegistered = bfalse;
        }
    }

    void Frise::disable()
    {
        Super::disable();

        unregisterPhysic();
        unregisterRegion();
    }

    void Frise::enable()
    {
        Super::enable();

        if ( isActive() ) 
        {
            if ( !m_disablePhysic )
            {
                registerPhysic();
            }

            const FriseConfig* pConfig = getConfig();
            if ( pConfig && pConfig->m_regionId.isValid() )
            {
                registerRegion( pConfig );
            }
        }
    }

    void Frise::preComputeForCook()
    {

#ifndef ITF_USE_BINARY_FRIEZE
        m_preComputedForCook = bfalse;

        if ( !m_preComputedForCook )
            return;
#endif

        FriseConfig* friseConfig = NULL;

#ifdef ITF_SUPPORT_LOGICDATABASE
        CSerializerObjectLogicDB serializerLogicDB;

        serializerLogicDB.init(getTemplatePath(),FILESERVER->getPlatform());

        if ( serializerLogicDB.isValid() && !serializerLogicDB.isEmpty() )
        {
            friseConfig = newAlloc(mId_GameplayTemplate, FriseConfig());
            friseConfig->Serialize(&serializerLogicDB,ESerialize_Data_Load);

            m_templatePickable = friseConfig;
        }
#endif

        if ( m_templatePickable )
        {           
            if(friseConfig)
                friseConfig->finalizeData();

            setPos( getWorldInitialPos() );
            setAngle( getWorldInitialRot() );
            setIsFlipped( getWorldInitialFlip() );
            setScale( getWorldInitialScale() );

            m_xfForCook.setFrom( get2DPos(), getAngle(), getScale(), getIsFlipped() );
            m_depthForCook = getDepth();
            m_preComputedForCook = btrue;

            forceRecomputeData( btrue );
        }

        SF_DEL(friseConfig);
        m_templatePickable = NULL;
    }

#ifndef ITF_FINAL
    bbool Frise::validate()
    {
        bbool isValid = Super::validate();

        if ( isValid && !getConfig() )
        {
            String8 str;
            if(m_templatePath != NULL && !m_templatePath.isEmpty())
                str.setTextFormat("Invalid or missing Frieze Config missing file: \"%s\"",m_templatePath.toString8().cStr());
            else
                str = "Invalid or missing Frieze Config";
            setDataError(str);
            isValid = bfalse;
        }

        return isValid;
    }

    void Frise::resetDataError()
    {
        Super::resetDataError();

#ifdef ITF_SUPPORT_EDITOR
        updateMetaFriezeDataError();
#endif // ITF_SUPPORT_EDITOR

    }

    void Frise::setDataError(const String8 & _str)
    {
        Super::setDataError(_str);

#ifdef ITF_SUPPORT_EDITOR
        updateMetaFriezeDataError();
#endif // ITF_SUPPORT_EDITOR

    }
#endif //!ITF_FINAL

#ifndef ITF_STRIPPED_DATA
    void Frise::updateForcedValues( const SerializedObjectContent_Object* _content )
    {
        Super::updateForcedValues( _content );
        
    const SerializedObjectContent_Object* contentPrimitiveParameters = _content ? _content->getObjectWithTag(PRIMITIVE_PARAM_LABEL) : NULL;
        m_primitiveParamForced.update(contentPrimitiveParameters);
    }
#endif // !ITF_STRIPPED_DATA

} // namespace ITF

