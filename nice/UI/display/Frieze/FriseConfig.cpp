#include "precompiled_engine.h"

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_GAMEMATERIALSMANAGER_H_
#include "gameplay/managers/GameMaterialManager.h"
#endif //_ITF_GAMEMATERIALSMANAGER_H_

#ifndef ITF_STRIPPED_DATA
#ifndef SERIALIZEROBJECTLUA_H
#include "engine/serializer/ZSerializerObjectLua.h"
#endif // SERIALIZEROBJECTLUA_H
#endif // ITF_SUPPORT_EDITOR || ITF_SUPPORT_EDITOR


namespace ITF
{
    #define friezeConfigSlopeCount 17
    #define friezeConfigZoneCount 4

    BEGIN_SERIALIZATION(FluidFriseLayer)
        SERIALIZE_MEMBER("erosion", m_erosion);
        SERIALIZE_MEMBER("color", m_color);
        SERIALIZE_MEMBER("threshold", m_threshold);
        SERIALIZE_MEMBER("gameMaterial", m_gameMaterialPath);
        SERIALIZE_MEMBER("backsideGameMaterial", m_backsideGameMaterialPath);
        m_gameMaterialIds[NormalMaterial] = m_gameMaterialPath.getStringID();
        m_gameMaterialIds[BacksideMaterial] = m_backsideGameMaterialPath.getStringID();
        SERIALIZE_MEMBER("collisionHeight", m_collisionHeight);
        SERIALIZE_MEMBER("hasBacksideInversion", m_hasBacksideInversion);
        SERIALIZE_MEMBER("forcedHeightWhenNotColored", m_forcedHeightWhenNotColored);
        SERIALIZE_MEMBER("hasForcedHeightWhenNotColored", m_hasForcedHeightWhenNotColored);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(FluidConfig)
        SERIALIZE_MEMBER("Elasticity", m_elasticity);
        SERIALIZE_MEMBER("Velocity", m_velocity);
        SERIALIZE_MEMBER("Viscosity", m_viscosity);
        SERIALIZE_MEMBER("TargetHeight", m_targetHeight);
        SERIALIZE_MEMBER("Amplification", m_amplification);
        SERIALIZE_MEMBER("TargetMaxHeight", m_targetMaxHeight);
        SERIALIZE_MEMBER("TargetMinHeight", m_targetMinHeight);
        SERIALIZE_MEMBER("TargetAddHeight", m_targetAddHeight);
        SERIALIZE_MEMBER("UnityWidth", m_unityWidth);
        SERIALIZE_MEMBER("PolylineUnityMult", m_polylineUnityMult);
        SERIALIZE_MEMBER("PolylineReaction", m_polylineReaction);
        SERIALIZE_MEMBER("EnterMult", m_enterMult);
        SERIALIZE_MEMBER("LeaveMult", m_leaveMult);
        SERIALIZE_MEMBER("MaxDstInfluence", m_maxDstInfluence);
        SERIALIZE_MEMBER("LevelsFront", m_levelsFront);
        SERIALIZE_MEMBER("LevelsBack", m_levelsBack);
        SERIALIZE_MEMBER("LevelDelta", m_levelDelta);
        SERIALIZE_MEMBER("UVDelta", m_UVDelta);
        SERIALIZE_MEMBER("PerpendicularBack", m_perpendicularBack);
        SERIALIZE_MEMBER("PerpendicularBackZ", m_perpendicularBackZ);
        SERIALIZE_MEMBER("PerpendicularBackScale", m_perpendicularBackScale);
        SERIALIZE_MEMBER("PerpendicularBackPos", m_perpendicularBackPos);
        SERIALIZE_MEMBER("PerpendicularBackPosZ", m_perpendicularBackPosZ);
        SERIALIZE_MEMBER("PerpendicularFront", m_perpendicularFront);
        SERIALIZE_MEMBER("PerpendicularFrontZ", m_perpendicularFrontZ);
        SERIALIZE_MEMBER("PerpendicularFrontScale", m_perpendicularFrontScale);
        SERIALIZE_MEMBER("PerpendicularFrontPos", m_perpendicularFrontPos);
        SERIALIZE_MEMBER("PerpendicularFrontPosZ", m_perpendicularFrontPosZ);
        SERIALIZE_MEMBER("WeightMultiplier", m_weightMultiplier);
        SERIALIZE_MEMBER("dstInfluenceMultiplier", m_dstInfluenceMultiplier);
        SERIALIZE_MEMBER("AbsorptionAtEdgeStart", m_absorptionAtEdgeStart);
        SERIALIZE_MEMBER("AbsorptionAtEdgeEnd", m_absorptionAtEdgeEnd);
        SERIALIZE_MEMBER("AbsorptionAtEdge_Length", m_absorptionAtEdge_Length);
        SERIALIZE_MEMBER("InfluenceLimit", m_influenceLimit);
        SERIALIZE_MEMBER("SideCount", m_sideCount);
        SERIALIZE_MEMBER("LayerCount", m_layerCount);
        SERIALIZE_MEMBER("BlendFactor", m_blendFactor);
        SERIALIZE_MEMBER("FxActor", m_fxActor);      
        SERIALIZE_CONTAINER_OBJECT("Layers", m_layers);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(Mesh3dDataElement)
        SERIALIZE_MEMBER("path", m_meshPath);
        SERIALIZE_MEMBER("dummyPath", m_dummyPath);
        SERIALIZE_MEMBER("textureIndex", m_textureIndex);
        SERIALIZE_MEMBER("family", m_family);
        SERIALIZE_MEMBER("familyIndex", m_familyIndex);
        END_SERIALIZATION()

    BEGIN_SERIALIZATION(Mesh3dData)
        SERIALIZE_MEMBER("uvMin", m_uvMin);
        SERIALIZE_MEMBER("uvMax", m_uvMax);
        SERIALIZE_CONTAINER_OBJECT("meshList", m_meshList);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(Mesh3dConfig)
        SERIALIZE_MEMBER("texureTileSize", m_texureTileSize);
        SERIALIZE_BOOL("random", m_random);
        SERIALIZE_CONTAINER_OBJECT("mesh3dList", m_mesh3dList);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(FillConfig)
        SERIALIZE_MEMBER("smoothFactor",m_smoothFactor);         
        SERIALIZE_MEMBER("offset",m_offSet);
        SERIALIZE_MEMBER("angle",m_fillAngle);
        SERIALIZE_MEMBER("scale",m_scale);
        SERIALIZE_MEMBER("tex",m_friendly);
        SERIALIZE_MEMBER("zExtrude", m_zExtrude);
        BEGIN_CONDITION_BLOCK(ESerialize_DataBin)
            SERIALIZE_MEMBER("isSmooth",m_isSmooth);
            SERIALIZE_MEMBER("texIndex",m_idTex);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

#ifdef USE_BOX2D
    BEGIN_SERIALIZATION(Phys2dConfig)
        SERIALIZE_MEMBER("density", m_density)
        SERIALIZE_MEMBER("friction", m_friction)
        SERIALIZE_MEMBER("restitution", m_restitution)
        SERIALIZE_MEMBER("linearDamping", m_linearDamping)
        SERIALIZE_MEMBER("angularDamping", m_angularDamping)
        SERIALIZE_MEMBER("gravityMupliplier", m_gravityMupliplier)
        SERIALIZE_BOOL("fixedRotation", m_fixedRotation)
        SERIALIZE_BOOL("isSensor", m_isSensor)
    END_SERIALIZATION()
#endif //USE_BOX2D

    BEGIN_SERIALIZATION(CollisionFrieze)
        SERIALIZE_MEMBER("build",m_build);
        SERIALIZE_MEMBER("offset",m_offset);
        SERIALIZE_MEMBER("extremity",m_extremity);
        SERIALIZE_MEMBER("extremity2",m_extremity2);
        SERIALIZE_MEMBER("flip",m_isFlip);
        SERIALIZE_MEMBER("methode",m_methode);
        SERIALIZE_MEMBER("distMaxToSkipParallelEdge",m_distMaxToSkipParallelEdge);
        SERIALIZE_MEMBER("smoothFactor",m_smoothFactor);        
        BEGIN_CONDITION_BLOCK(ESerialize_DataBin)
            SERIALIZE_MEMBER("isSmooth",m_isSmooth);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(CollisionTexture)
        SERIALIZE_MEMBER("build",m_build);
        SERIALIZE_MEMBER("offset",m_offset);
        SERIALIZE_MEMBER("cornerIn",m_cornerIn);
        SERIALIZE_MEMBER("cornerOut",m_cornerOut);
        BEGIN_CONDITION_BLOCK(ESerialize_DataBin)
            SERIALIZE_MEMBER("cornerInCur",m_cornerInCur);
            SERIALIZE_MEMBER("cornerOutCur",m_cornerOutCur);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(VertexAnim)
        SERIALIZE_MEMBER("animGlobalSpeed",m_globalSpeed);
        SERIALIZE_MEMBER("animGlobalRotSpeed",m_globalRotSpeed);
        SERIALIZE_MEMBER("animSpeedX",m_speedX);
        SERIALIZE_MEMBER("animSpeedY",m_speedY);
        SERIALIZE_MEMBER("animSyncX",m_syncX);
        SERIALIZE_MEMBER("animSyncY",m_syncY);
        SERIALIZE_MEMBER("animAmplitudeX",m_amplitudeX);
        SERIALIZE_MEMBER("animAmplitudeY",m_amplitudeY);
        SERIALIZE_MEMBER("animSync",m_sync);
        SERIALIZE_MEMBER("animAngleUsed",m_angleUsed);
        SERIALIZE_MEMBER("animAngle",m_angle);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(FriseTextureConfig)
        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
            SERIALIZE_MEMBER("path", m_gfxMaterial.getTexturePathSet().getTexturePath());
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT("material", m_gfxMaterial);
        SERIALIZE_MEMBER("friendly",m_friendly);
        SERIALIZE_MEMBER("gameMaterial",m_gameMaterial);
        SERIALIZE_MEMBER("color",m_color);
        SERIALIZE_MEMBER("fillingOffset",m_fillOffset);
        SERIALIZE_OBJECT("collision",m_collisionTex);
        SERIALIZE_MEMBER("scrollUV",m_scrollingSpeedTrans);
        SERIALIZE_MEMBER("scrollAngle",m_scrollingSpeedRotate);
        SERIALIZE_MEMBER("useUV2",m_scrollingUseUV2);
        SERIALIZE_MEMBER("scaleUV2",m_scrollingUV2Scale);
        SERIALIZE_MEMBER("scrollUV2",m_scrollingSpeedTransUV2);
        SERIALIZE_MEMBER("scrollAngle2",m_scrollingSpeedRotateUV2);
        SERIALIZE_MEMBER("alphaBorder",m_alphaBorder);
        SERIALIZE_MEMBER("alphaUp",m_alphaUp);
    END_SERIALIZATION()

	BEGIN_SERIALIZATION(Frieze3DConfig)
		SERIALIZE_MEMBER("orient",m_orient);
		SERIALIZE_MEMBER("mesh3DFile",m_meshRefFillFile);
		SERIALIZE_MEMBER("mesh3DFile_Left",m_meshRefLeftFile);
		SERIALIZE_MEMBER("mesh3DFile_Right",m_meshRefRightFile);
	END_SERIALIZATION()

    BEGIN_SERIALIZATION_CHILD(FriseConfig)              
        SERIALIZE_MEMBER("gameMaterial",m_gameMaterial);
        SERIALIZE_MEMBER("gameMaterialExtremityStart",m_gameMaterialStartExtremity);
        SERIALIZE_MEMBER("gameMaterialExtremityStop",m_gameMaterialStopExtremity);
        SERIALIZE_MEMBER("visualOffset",m_visualOffset);
        SERIALIZE_MEMBER("regionId",m_regionId);        
        SERIALIZE_MEMBER("useFriezeFlipToFlipUV",m_useFriezeFlipToFlipUV);
        SERIALIZE_MEMBER("cornerFactor",m_cornerFactor);
        SERIALIZE_MEMBER("height",m_height);
        SERIALIZE_MEMBER("width",m_width);
        SERIALIZE_MEMBER("methode",m_methode);
        SERIALIZE_MEMBER("snapCoeff",m_snapCoeff);
        SERIALIZE_MEMBER("snapCoeffUv",m_snapCoeffUv);
        SERIALIZE_MEMBER("flexibility",m_flexibility);
        SERIALIZE_MEMBER("wallAngle",m_wallAngle);
        SERIALIZE_MEMBER("isUvFlipX",m_isUvFlipX);
        SERIALIZE_MEMBER("isUvFlipY",m_isUvFlipY);
        SERIALIZE_MEMBER("isRatioFixed",m_isRatioFixed);                      
        SERIALIZE_MEMBER("smoothFactorVisual",m_smoothFactorVisual);                
        SERIALIZE_MEMBER("scale",m_scale);
        SERIALIZE_MEMBER("density",m_density);                                
        SERIALIZE_OBJECT("collision",m_collisionFrieze);
        SERIALIZE_OBJECT("fill",m_fill);
#ifdef USE_BOX2D
        SERIALIZE_OBJECT("phys2d",m_phys2d);
#endif //USE_BOX2D
        SERIALIZE_MEMBER("zExtrudeUp",m_zVtxExtrudeUp);
        SERIALIZE_MEMBER("zExtrudeDown",m_zVtxExtrudeDown);
        SERIALIZE_MEMBER("zExtrudeStop",m_zVtxExtrudeStop);
        SERIALIZE_MEMBER("zExtrudeStart",m_zVtxExtrudeStart);
        SERIALIZE_MEMBER("zExtrudeExtremityStart",m_zVtxExtrudeExtremityStart);
        SERIALIZE_MEMBER("zExtrudeExtremityStop",m_zVtxExtrudeExtremityStop);

        SERIALIZE_MEMBER("patchCoeffOffset",m_patchCoeffOffset);
        SERIALIZE_MEMBER("patchScale",m_patchScale);
        SERIALIZE_MEMBER("patchOriented",m_patchOriented);
        SERIALIZE_MEMBER("patchCornerFactorCollision", m_patchCornerFactorCollision);

        SERIALIZE_MEMBER("patchAngleMin", m_patchAngleMin);
        SERIALIZE_MEMBER("ExtremityScale", m_extremityScale);

        SERIALIZE_MEMBER("selfIllumColor",m_selfIllumColor);
        SERIALIZE_CONTAINER_OBJECT("textureConfigs",m_textureConfigs);
        SERIALIZE_OBJECT("VertexAnim",m_vtxAnim);
        SERIALIZE_OBJECT("Fluid",m_fluid);
        SERIALIZE_OBJECT("mesh3D",m_mesh3d);

        SERIALIZE_MEMBER("invertMeshOrder", m_invertMeshOrder);
        SERIALIZE_MEMBER("skewAngle",m_skewAngle);

        SERIALIZE_MEMBER("isDigShape", m_isDigShape);
        SERIALIZE_MEMBER("isLockedDigShape", m_isLockedDigShape);

        SERIALIZE_MEMBER("switchExtremityAuto", m_switchExtremityAuto);
        SERIALIZE_MEMBER("offsetExtremity", m_offsetExtremity);

        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
            SERIALIZE_MEMBER("slope_180",m_FriseTextureConfigFriendlyBySlope[0]);
            SERIALIZE_MEMBER("slope_202",m_FriseTextureConfigFriendlyBySlope[1]);
            SERIALIZE_MEMBER("slope_225",m_FriseTextureConfigFriendlyBySlope[2]);
            SERIALIZE_MEMBER("slope_247",m_FriseTextureConfigFriendlyBySlope[3]);
            SERIALIZE_MEMBER("slope_270",m_FriseTextureConfigFriendlyBySlope[4]);
            SERIALIZE_MEMBER("slope_292",m_FriseTextureConfigFriendlyBySlope[5]);
            SERIALIZE_MEMBER("slope_315",m_FriseTextureConfigFriendlyBySlope[6]);
            SERIALIZE_MEMBER("slope_337",m_FriseTextureConfigFriendlyBySlope[7]);
            SERIALIZE_MEMBER("slope_0",m_FriseTextureConfigFriendlyBySlope[8]);
            SERIALIZE_MEMBER("slope_22",m_FriseTextureConfigFriendlyBySlope[9]);
            SERIALIZE_MEMBER("slope_45",m_FriseTextureConfigFriendlyBySlope[10]);
            SERIALIZE_MEMBER("slope_67",m_FriseTextureConfigFriendlyBySlope[11]);
            SERIALIZE_MEMBER("slope_90",m_FriseTextureConfigFriendlyBySlope[12]);
            SERIALIZE_MEMBER("slope_112",m_FriseTextureConfigFriendlyBySlope[13]);
            SERIALIZE_MEMBER("slope_135",m_FriseTextureConfigFriendlyBySlope[14]);
            SERIALIZE_MEMBER("slope_157",m_FriseTextureConfigFriendlyBySlope[15]);
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerialize_DataBin)
            SERIALIZE_MEMBER("idTexSwitch",m_idTexSwitch);
            SERIALIZE_MEMBER("cooked",m_cooked);
            SERIALIZE_CONTAINER("textureConfigIndexBySlope",m_FriseTextureConfigIndexBySlope);
            SERIALIZE_CONTAINER("textureConfigIndexByZone",m_FriseTextureConfigIndexByZone);            
            SERIALIZE_MEMBER("smoothVisual",m_isSmoothVisual); 
        END_CONDITION_BLOCK()

        SERIALIZE_OBJECT("PrimitiveParameters", m_primitiveParam);
		SERIALIZE_OBJECT("frieze3D", m_frieze3DConfig );
    END_SERIALIZATION()
   
    IMPLEMENT_OBJECT_RTTI(FriseConfig)

    void FriseConfig::init()    
    {             
        m_useFriezeFlipToFlipUV                 = btrue;    
        m_cornerFactor                          = 1.5f;
        m_height                                = 2.f;
        m_width                                 = 2.f;
        m_methode                               = 1;
        m_isDigShape                            = bfalse;
        m_isLockedDigShape                      = bfalse;
        m_switchExtremityAuto                   = bfalse;
        m_offsetExtremity                       = 0.f;
        m_snapCoeff                             = 0.1f;
        m_snapCoeffUv                           = 0.6f;
        m_isSmoothVisual                        = bfalse;               
        m_smoothFactorVisual                    = F32_INVALID;        
        m_visualOffset                          = 0.5f;
        m_idTexSwitch                           = -1;
        m_zVtxExtrudeUp                         = 0.f;
        m_zVtxExtrudeDown                       = 0.f;
        m_zVtxExtrudeStart                      = 0.f;
        m_zVtxExtrudeStop                       = 0.f;
        m_zVtxExtrudeExtremityStart             = 0.f;
        m_zVtxExtrudeExtremityStop              = 0.f;
        m_patchCoeffOffset                      = 1.f;
        m_patchScale                            = 1.f;
        m_patchOriented                         = bfalse;
        m_patchCornerFactorCollision            = 1.f;
        m_flexibility                           = 0.75f;        
        m_selfIllumColor                        = COLOR_ZERO;
        m_isUvFlipX                             = bfalse;
        m_isUvFlipY                             = bfalse;
        m_isRatioFixed                          = btrue;
        m_scale                                 = 1.f;    
        m_density                               = 1.f;        
        m_cooked                                = bfalse;        
        m_lastUpdateFrame                       = 0;
        m_gameMaterialTemplate                  = NULL;
        m_gameMaterialStartExtremityTemplate    = NULL;
        m_gameMaterialStopExtremityTemplate     = NULL;
        m_skewAngle                             = F32_INVALID;
        m_invertMeshOrder                       = bfalse;

        m_gameMaterialTemplate                 = NULL;
        m_gameMaterialStartExtremityTemplate   = NULL;
        m_gameMaterialStopExtremityTemplate    = NULL;

        m_wallAngle.SetRadians( MTH_PIBY4 );
        m_patchAngleMin.SetRadians( 0.f );
        m_extremityScale = Vec2d::One;

        m_FriseTextureConfigIndexBySlope.resize( friezeConfigSlopeCount, -1);
        m_FriseTextureConfigIndexByZone.resize( friezeConfigZoneCount, -1);
        m_FriseTextureConfigFriendlyBySlope.resize( friezeConfigSlopeCount );

        m_primitiveParam = GFXPrimitiveParam::getDefault();
        m_fluid.m_layers.resize( 1 );
    }

    FriseTextureConfig::FriseTextureConfig()
        : m_alphaBorder(U8_INVALID)
        , m_alphaUp(U8_INVALID)
        , m_fillOffset(F32_INVALID)
        , m_color(COLOR_WHITE)
        , m_scrollingSpeedRotate(0.f)
        , m_scrollingSpeedTrans(Vec2d::Zero)
        , m_scrollingUseUV2(bfalse)
        , m_scrollingUV2Scale(Vec2d::One)
        , m_scrollingSpeedRotateUV2(0.f)
        , m_scrollingSpeedTransUV2(Vec2d::Zero)
        , m_gameMaterialTemplate(NULL)
    {
    }

    FriseTextureConfig::~FriseTextureConfig()
    {
    }


    FriseConfig::~FriseConfig()
    {        
    }

    bbool FriseTextureConfig::onLoaded(class ResourceContainer * _parentContainer)
    {
        bbool ret = btrue;

        ITF_ASSERT(_parentContainer);

        m_gfxMaterial.onLoaded(_parentContainer);
       
        if ( !m_gameMaterial.isEmpty() )
        {
            m_gameMaterialTemplate = GAMEMATERIAL_MANAGER->requestGameMaterial(m_gameMaterial);

            if ( !m_gameMaterialTemplate )
            {
                ret = bfalse;
            }
        }

        return ret;
    }

    void FriseTextureConfig::onDeleted(class ResourceContainer * _parentContainer)
    {
        ITF_ASSERT(_parentContainer);

        m_gfxMaterial.onUnLoaded(_parentContainer);
        if ( !m_gameMaterial.isEmpty() && m_gameMaterialTemplate )
        {
            GAMEMATERIAL_MANAGER->releaseGameMaterial(m_gameMaterialTemplate);
        }
    }

    void FriseTextureConfig::setGameMaterial( const Path& _gameMaterial )
    {
        if ( m_gameMaterial != _gameMaterial )
        {
            if ( m_gameMaterialTemplate )
            {
                GAMEMATERIAL_MANAGER->releaseGameMaterial(m_gameMaterialTemplate);
                m_gameMaterialTemplate = NULL;
            }

            m_gameMaterial = _gameMaterial;

            if ( !m_gameMaterial.isEmpty() )
            {
                m_gameMaterialTemplate = GAMEMATERIAL_MANAGER->requestGameMaterial(m_gameMaterial);
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // VL : after template is loaded update primitive forced values 
    // (i.e. value defined in fcg or in light preset included in fcg
    //
#ifndef ITF_STRIPPED_DATA
    void FriseConfig::updateForcedValues()
    {
        CSerializerObjectLua serializer;
        if(!serializer.OpenToRead(getFile()))
            return;
        
        Pickable *pickable = NULL;
        
        serializer.SerializeObject("params", pickable, GAMEINTERFACE->getClassTemplateFactory(), ESerialize_Data_Load | ESerialize_InstanceLoad);
        serializer.Close();

        if(!pickable) return;
        const SerializedObjectContent_Object* content = serializer.getContent()->getObject(0);
        const SerializedObjectContent_Object* contentPrimitiveParameters = content ? content->getObjectWithTag("PrimitiveParameters") : NULL;
        m_primitiveParamForced.update(contentPrimitiveParameters);
        SF_DEL(pickable);
    }
#else
#define updateForcedValues()
#endif

    bbool FriseConfig::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);


        m_FriseTextureConfigFriendlyBySlope[16] = m_FriseTextureConfigFriendlyBySlope[0];


        ResourceContainer* resourceContainer = getResourceContainer();

        updateForcedValues();

        if ( !m_gameMaterial.isEmpty() )
        {
            m_gameMaterialTemplate = GAMEMATERIAL_MANAGER->requestGameMaterial(m_gameMaterial);

            if ( !m_gameMaterialTemplate )
            {
                bOk = bfalse;
            }
        }

        if ( !m_gameMaterialStartExtremity.isEmpty() )
        {
            m_gameMaterialStartExtremityTemplate = GAMEMATERIAL_MANAGER->requestGameMaterial(m_gameMaterialStartExtremity);

            if ( !m_gameMaterialStartExtremityTemplate )
            {
                bOk = bfalse;
            }
        }

        if ( !m_gameMaterialStopExtremity.isEmpty() )
        {
            m_gameMaterialStopExtremityTemplate = GAMEMATERIAL_MANAGER->requestGameMaterial(m_gameMaterialStopExtremity);

            if ( !m_gameMaterialStopExtremityTemplate )
            {
                bOk = bfalse;
            }
        }

        // texture config res dependencies
        const u32 textureConfigCount = m_textureConfigs.size();
        for (u32 i = 0; i < textureConfigCount ; i++)
        {
            FriseTextureConfig& texConfig = m_textureConfigs[i];

            bOk &= texConfig.onLoaded(resourceContainer);

            // gMat
            if ( !m_gameMaterial.isEmpty() && texConfig.m_gameMaterial.isEmpty() )
                texConfig.setGameMaterial(m_gameMaterial);
        }

        const u32 fluidLayersCount = m_fluid.m_layers.size();
        for (u32 i = 0; i < fluidLayersCount; i++)
        {
            FluidFriseLayer &layer = m_fluid.m_layers[i];

            if ( !layer.m_gameMaterialPath.isEmpty() )
            {
                layer.m_gameMaterialTemplate = GAMEMATERIAL_MANAGER->requestGameMaterial(layer.m_gameMaterialPath);

                if ( !layer.m_gameMaterialTemplate )
                {
                    bOk = bfalse;
                }
            }

            if ( !layer.m_backsideGameMaterialPath.isEmpty() )
            {
                layer.m_backsideGameMaterialTemplate = GAMEMATERIAL_MANAGER->requestGameMaterial(layer.m_backsideGameMaterialPath);

                if ( !layer.m_backsideGameMaterialTemplate )
                {
                    bOk = bfalse;
                }
            }
        }

		if (m_methode == Frise::In3D)
		{
			if (!m_frieze3DConfig.m_meshRefFillFile.isEmpty())
				m_frieze3DConfig.m_meshRefFillID = addResource(Resource::ResourceType_Mesh3D,m_frieze3DConfig.m_meshRefFillFile);
			if (!m_frieze3DConfig.m_meshRefLeftFile.isEmpty())
				m_frieze3DConfig.m_meshRefLeftID = addResource(Resource::ResourceType_Mesh3D,m_frieze3DConfig.m_meshRefLeftFile);
			if (!m_frieze3DConfig.m_meshRefRightFile.isEmpty())
				m_frieze3DConfig.m_meshRefRightID = addResource(Resource::ResourceType_Mesh3D,m_frieze3DConfig.m_meshRefRightFile);
		}

        if ( !m_cooked )
        {
            finalizeData();
            m_cooked = btrue;
        }

        return bOk;
    }

    void FriseConfig::finalizeData()
    {
        validateData();
        processData();    
    }

    void FriseConfig::onTemplateDelete( bbool _hotReload )
    {
        ResourceContainer* resourceContainer = getResourceContainer();

        for (u32 i = 0; i < m_fluid.m_layers.size(); i++)
        {
            FluidFriseLayer &layer = m_fluid.m_layers[i];

            if ( layer.m_gameMaterialTemplate )
            {
                GAMEMATERIAL_MANAGER->releaseGameMaterial(layer.m_gameMaterialTemplate);
            }

            if ( layer.m_backsideGameMaterialTemplate )
            {
                GAMEMATERIAL_MANAGER->releaseGameMaterial(layer.m_backsideGameMaterialTemplate);
            }
        }

        for (u32 i = 0; i < m_textureConfigs.size(); i++)
            m_textureConfigs[i].onDeleted(resourceContainer);

        if ( m_gameMaterialStopExtremityTemplate )
        {
            GAMEMATERIAL_MANAGER->releaseGameMaterial(m_gameMaterialStopExtremityTemplate);
        }

        if ( m_gameMaterialStartExtremityTemplate )
        {
            GAMEMATERIAL_MANAGER->releaseGameMaterial(m_gameMaterialStartExtremityTemplate);
        }

        if ( m_gameMaterialTemplate )
        {
            GAMEMATERIAL_MANAGER->releaseGameMaterial(m_gameMaterialTemplate);
        }

        // Let all game materials unregister from the resource container etc then clear the resource container
        Super::onTemplateDelete(_hotReload);
    }

    void FriseConfig::validateData()
    {
        if ( m_methode >= Frise::BuildFriezeCount )
            m_methode = Frise::InGeneric;
        else if ( m_methode == Frise::InPipePatchSimple )
        {
            m_visualOffset = 0.5f; // not yet supported
        }

        m_snapCoeff = Max( 0.f, m_snapCoeff );
        m_snapCoeffUv = Max( 0.f, m_snapCoeffUv );
        m_height = Max( m_height, 0.f );
        m_width = Max( m_width, 0.f );
        m_flexibility = Clamp( m_flexibility, 0.f, 1.f );
        m_patchCoeffOffset = Max( m_patchCoeffOffset, 0.f) * 0.5f;
        m_patchScale = f32_Max( m_patchScale, 0.f );

        m_extremityScale.x() = f32_Max( m_extremityScale.x(), 0.f );
        m_extremityScale.y() = f32_Max( m_extremityScale.y(), 0.f );

        m_wallAngle.SetRadians( Clamp( m_wallAngle.ToRadians(), 0.f, MTH_PIBY2 ) );
        m_patchAngleMin.SetRadians( Clamp( m_patchAngleMin.ToRadians(), 0.f, MTH_PI) );

        if ( m_smoothFactorVisual != F32_INVALID )
        {
            m_smoothFactorVisual = Clamp( m_smoothFactorVisual, 0.f, 10.f);
            m_isSmoothVisual = btrue;
        }

        // fill
        if ( m_fill.m_smoothFactor != F32_INVALID )
        {
            m_fill.m_smoothFactor = Clamp( m_fill.m_smoothFactor, 0.f, 10.f);
            m_fill.m_isSmooth = btrue;
        }

        if ( m_cornerFactor < 1.f )
            m_cornerFactor = 1.f;
        else if ( m_cornerFactor >= 1.4f && m_cornerFactor <= 1.51f ) // square corner
            m_cornerFactor = 1.41421356f;

        m_visualOffset = Clamp( m_visualOffset, 0.f, 1.f );  

        // colllision
        if ( m_collisionFrieze.m_smoothFactor != F32_INVALID )
        {
            m_collisionFrieze.m_smoothFactor = Clamp( m_collisionFrieze.m_smoothFactor, 0.f, 10.f);
            m_collisionFrieze.m_isSmooth = btrue;
        }
        
        m_collisionFrieze.m_distMaxToSkipParallelEdge = Max( m_collisionFrieze.m_distMaxToSkipParallelEdge, 0.01f); // to not have parallel edge collision

        // skew
        if ( m_skewAngle != F32_INVALID )
            m_skewAngle *= MTH_DEGTORAD;

        // fluid
        if ( m_methode == Frise::InFluid || m_methode == Frise::InString )
        {            
            m_fluid.m_polylineUnityMult = m_fluid.m_polylineReaction ? Max( m_fluid.m_polylineUnityMult, (u32)1) : U32_INVALID;
            m_fluid.m_sideCount         = Clamp( (u32)m_fluid.m_sideCount,  (u32)1, (u32)2);
            m_fluid.m_layerCount        = Clamp( (u32)m_fluid.m_layerCount, (u32)1, (u32)MAX_FLUID_LAYER_COUNT);            
            
            for ( u32 i=0; i<m_fluid.m_layerCount; i++ )
            {
                FluidFriseLayer& layer = m_fluid.m_layers[i];

                layer.m_hasForcedHeightWhenNotColored = layer.m_forcedHeightWhenNotColored != -FLT_MAX;
            }
        }
       
        // tex config values
        const u32 textureConfigCount = m_textureConfigs.size();
        for (u32 i = 0; i < textureConfigCount ; i++)
        {
            FriseTextureConfig& texConfig = m_textureConfigs[i];

            // fill
            if ( texConfig.m_fillOffset == F32_INVALID )
                texConfig.m_fillOffset = m_fill.m_offSet;

            // collision
            if ( texConfig.m_collisionTex.m_build == U32_INVALID )
                texConfig.m_collisionTex.m_build = m_collisionFrieze.m_build?1:0;

            if ( texConfig.m_collisionTex.m_offset == F32_INVALID )
                texConfig.m_collisionTex.m_offset = m_collisionFrieze.m_offset;

            if ( texConfig.m_collisionTex.m_cornerIn.x() != F32_INVALID )
            {
                texConfig.m_collisionTex.m_cornerIn.x() = Clamp( texConfig.m_collisionTex.m_cornerIn.x(), 0.f, 1.f);
                texConfig.m_collisionTex.m_cornerIn.y() = Clamp( texConfig.m_collisionTex.m_cornerIn.y(), 0.f, 1.f);
            }

            if ( texConfig.m_collisionTex.m_cornerOut.x() != F32_INVALID )
            {
                texConfig.m_collisionTex.m_cornerOut.x() = Clamp( texConfig.m_collisionTex.m_cornerOut.x(), 0.f, 1.f);
                texConfig.m_collisionTex.m_cornerOut.y() = Clamp( texConfig.m_collisionTex.m_cornerOut.y(), 0.f, 1.f);
            }

            // color
            if ( texConfig.m_alphaBorder == U8_INVALID )
                texConfig.m_alphaBorder = texConfig.m_color.getU8Alpha();

            if ( texConfig.m_alphaUp == U8_INVALID )
                texConfig.m_alphaUp = texConfig.m_color.getU8Alpha();
        }
    }

    void FriseConfig::processData()
    {
        const u32 numTextures = m_textureConfigs.size();
        u32 noFillTextureIndex = U32_INVALID;

        ITF_ASSERT( m_fill.m_idTex == -1);

        if ( m_fill.m_friendly.isValid() )
        {
            for ( u32 i = 0; i < numTextures; i++ )
            {
                if ( m_textureConfigs[i].m_friendly == m_fill.m_friendly )
                {
                    m_fill.m_idTex = i;

                    if ( noFillTextureIndex != U32_INVALID )
                    {
                        break;
                    }
                }
                else if ( m_textureConfigs[i].isDrawnable() )
                {
                    noFillTextureIndex = i;

                    if ( m_fill.m_idTex != -1 )
                    {
                        break;
                    }
                }
            }
        }

        for ( u32 i = 0; i < friezeConfigSlopeCount; i++ )
        {
            m_FriseTextureConfigIndexBySlope[i] = U32_INVALID;

            if ( !m_FriseTextureConfigFriendlyBySlope[i].isValid() )
            {
                m_FriseTextureConfigIndexBySlope[i] = noFillTextureIndex;
            }
            else
            {
                for ( u32 j = 0; j < numTextures; j++ )
                {
                    if ( m_FriseTextureConfigFriendlyBySlope[i] == m_textureConfigs[j].m_friendly )
                    {
                        m_FriseTextureConfigIndexBySlope[i] = j;
                        break;
                    }
                }
            }
        }

        m_idTexSwitch = -1;

        for ( u32 i = 0; i < numTextures; i++ )
        {
            FriseTextureConfig& texConfig = m_textureConfigs[i];

            if ( texConfig.m_friendly == ITF_GET_STRINGID_CRC(texZoneTop,3905696052) )
            {
                m_FriseTextureConfigIndexByZone[0] = i;
                setCollisionCorner( texConfig.m_collisionTex );
            }
            else if ( texConfig.m_friendly == ITF_GET_STRINGID_CRC(texZoneRight,1954320889) )
            {
                m_FriseTextureConfigIndexByZone[1] = i;
                
                Vec2d collisionRotate;
                CollisionTexture& collisionTex = texConfig.m_collisionTex;

                // collision OUT
                collisionRotate = collisionTex.m_cornerOut;
                collisionTex.m_cornerOut.x() = 1.f -collisionRotate.y();
                collisionTex.m_cornerOut.y() = collisionRotate.x();

                // collision IN
                collisionRotate = collisionTex.m_cornerIn;
                collisionTex.m_cornerIn.x() = 1.f -collisionRotate.y();
                collisionTex.m_cornerIn.y() = collisionRotate.x();   

                setCollisionCorner( collisionTex );
            }
            else if ( texConfig.m_friendly == ITF_GET_STRINGID_CRC(texZoneLeft, 3639257683) )
            {
                m_FriseTextureConfigIndexByZone[2] = i;
                
                Vec2d collisionRotate;
                CollisionTexture& collisionTex = texConfig.m_collisionTex;

                // collision OUT
                collisionRotate = collisionTex.m_cornerOut;
                collisionTex.m_cornerOut.x() = collisionRotate.y();
                collisionTex.m_cornerOut.y() = 1.f -collisionRotate.x();

                // collision IN
                collisionRotate = collisionTex.m_cornerIn;
                collisionTex.m_cornerIn.x() = collisionRotate.y();
                collisionTex.m_cornerIn.y() = 1.f -collisionRotate.x();

                setCollisionCorner( collisionTex );
            }
            else if ( texConfig.m_friendly == ITF_GET_STRINGID_CRC(texZoneBottom,74673459) )
            {
                m_FriseTextureConfigIndexByZone[3] = i;
                
                Vec2d collisionRotate;
                CollisionTexture& collisionTex = texConfig.m_collisionTex;

                // collision OUT
                collisionRotate = collisionTex.m_cornerOut;
                collisionTex.m_cornerOut.x() = 1.f -collisionRotate.x();
                collisionTex.m_cornerOut.y() = 1.f -collisionRotate.y();

                // collision IN
                collisionRotate = collisionTex.m_cornerIn;
                collisionTex.m_cornerIn.x() = 1.f -collisionRotate.x();
                collisionTex.m_cornerIn.y() = 1.f -collisionRotate.y();   

                setCollisionCorner( collisionTex );
            }
            else if ( texConfig.m_friendly == ITF_GET_STRINGID_CRC(texSwitch,2442097248) )
            {
                m_idTexSwitch = i;
            }
        }        
    }

    void FriseConfig::setCollisionCorner( CollisionTexture& _collisionTex )
    {
        Vec2d collisionCorner;

        // collision OUT
        collisionCorner = _collisionTex.m_cornerOut;
        collisionCorner.x() = 1.f -collisionCorner.x();

        if ( collisionCorner.y() > collisionCorner.x() )
        {
            _collisionTex.m_cornerOutCur = btrue;
            _collisionTex.m_cornerOut.x() = collisionCorner.y() -collisionCorner.x();
            _collisionTex.m_cornerOut.y() = collisionCorner.x();
        } 
        else
        {
            _collisionTex.m_cornerOutCur = bfalse;
            _collisionTex.m_cornerOut.x() = collisionCorner.x() -collisionCorner.y();
            _collisionTex.m_cornerOut.y() = collisionCorner.y();
        }

        // collision IN
        collisionCorner = _collisionTex.m_cornerIn;

        if ( collisionCorner.x() > collisionCorner.y() )
        {
            _collisionTex.m_cornerInCur = btrue;
            _collisionTex.m_cornerIn.x() = collisionCorner.x() -collisionCorner.y();
            _collisionTex.m_cornerIn.y() = 1.f -collisionCorner.x();
        } 
        else
        {
            _collisionTex.m_cornerInCur = bfalse;
            _collisionTex.m_cornerIn.x() = collisionCorner.y() -collisionCorner.x();
            _collisionTex.m_cornerIn.y() = 1.f -collisionCorner.y();
        }
    }

#ifndef ITF_STRIPPED_DATA
    void FriseConfig::getTags( ITF_VECTOR<String8>& _tagList ) const
    {
        Super::getTags(_tagList);
        
        if(_tagList.size() == 0)
            _tagList.push_back("frieze");
    }
#endif // ITF_STRIPPED_DATA

} // namespace ITF
