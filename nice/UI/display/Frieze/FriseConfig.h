#ifndef _ITF_FRISECONFIG_H_
#define _ITF_FRISECONFIG_H_

#ifndef ITF_GFX_MATERIAL_TEXTURE_SET_H_
#include "engine/display/material/GFXMaterialTextureSet.h"
#endif //ITF_GFX_MATERIAL_TEXTURE_SET_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef ITF_GFX_MATERIAL_H_
#include "engine/display/material/GFXMaterial.h"
#endif //ITF_GFX_MATERIAL_H_

#ifndef ITF_GFX_PRIMITIVES_H_
#include "engine\display\Primitives\GFXPrimitives.h"
#endif

#define MAX_FLUID_LAYER_COUNT 3


namespace ITF
{

class AnimTrackExt;
class AnimDraw;
class ArchiveMemory;
class AnimMeshScene;

struct FillConfig 
{
    DECLARE_SERIALIZE()

    FillConfig()
        : m_isSmooth(bfalse) 
        , m_smoothFactor(F32_INVALID)        
        , m_offSet(0.5f)
        , m_scale(Vec2d(2.f,2.f))
        , m_idTex(-1)
        , m_zExtrude(0.f)
    {        
    };

    bbool       m_isSmooth;
    f32         m_smoothFactor;
    f32         m_offSet;
    Angle       m_fillAngle;
    Vec2d       m_scale;
    StringID    m_friendly;
    i32         m_idTex;
    f32         m_zExtrude;
};

class FluidFriseLayer
{
    DECLARE_SERIALIZE()

public:
    static const i32 InvalidCollisionHeight = -1000;
    enum {NormalMaterial=0, BacksideMaterial, MaterialCount}; //Indices 0/1 are reserved for normal/backside materials

    FluidFriseLayer() 
        : m_color(1.f,1.f,1.f,1.f)
        , m_gameMaterialTemplate(NULL)
        , m_backsideGameMaterialTemplate(NULL)
        , m_threshold(FLT_MAX)
        , m_collisionHeight((f32)InvalidCollisionHeight)
        , m_erosion(0)
        , m_hasBacksideInversion(bfalse)
        , m_collisionHeightMultiplier(1)
        , m_hasForcedHeightWhenNotColored(bfalse)
        , m_forcedHeightWhenNotColored(-FLT_MAX)
    {}

    void serialize(ArchiveMemory& _archive);

    f32 m_threshold; //Layer info is only applied if value is >= this threshold
    Color m_color;
    Path m_gameMaterialPath, m_backsideGameMaterialPath;
    const class GameMaterial_Template* m_gameMaterialTemplate;
    const class GameMaterial_Template* m_backsideGameMaterialTemplate;
    f32 m_collisionHeight;
    f32 m_collisionHeightMultiplier;
    u32 m_erosion;
    bbool m_hasBacksideInversion;
    bbool m_hasForcedHeightWhenNotColored;
    f32 m_forcedHeightWhenNotColored;

    StringID m_gameMaterialIds[MaterialCount];
};

struct FluidConfig 
{
    DECLARE_SERIALIZE()

    FluidConfig()
        : m_elasticity(0.98f)
        , m_velocity(0.04f)
        , m_viscosity(0.05f)
        , m_absorptionAtEdgeStart(0)
        , m_absorptionAtEdgeEnd(0)
        , m_absorptionAtEdge_Length(0)
        , m_targetHeight(0.5f)
        , m_amplification(0.01f)
        , m_targetMaxHeight(1.f)
        , m_targetMinHeight(0.f)
        , m_targetAddHeight(-0.005f)
        , m_unityWidth(0.1f)
        , m_polylineUnityMult(1)
        , m_enterMult(10.f)
        , m_leaveMult(10.f)
        , m_maxDstInfluence(1.f)
        , m_levelsFront(0)
        , m_levelsBack(0)
        , m_perpendicularBack(bfalse)
        , m_perpendicularBackZ(0.f)
        , m_perpendicularBackScale(1.f)
        , m_perpendicularBackPos(1.f)
        , m_perpendicularBackPosZ(0.f)
        , m_perpendicularFront(bfalse)
        , m_perpendicularFrontZ(0.f)
        , m_perpendicularFrontScale(1.f)
        , m_perpendicularFrontPos(1.f)
        , m_perpendicularFrontPosZ(0.f)
        , m_influenceLimit(FLT_MAX)
        , m_weightMultiplier(0.03f)
        , m_dstInfluenceMultiplier(1.f)
        , m_layerCount(1)
        , m_levelDelta(Vec3d::Zero)
        , m_UVDelta(Vec2d::Zero)
        , m_sideCount(1)
        , m_blendFactor(0.f)
        , m_polylineReaction(btrue)
    {        
    };

    f32     m_elasticity;
    f32     m_velocity;
    f32     m_viscosity;
    f32     m_absorptionAtEdgeStart;
    f32     m_absorptionAtEdgeEnd;
    f32     m_absorptionAtEdge_Length;
    f32     m_targetHeight; 
    f32     m_amplification;
    f32     m_targetMaxHeight;
    f32     m_targetMinHeight;
    f32     m_targetAddHeight;
    f32     m_unityWidth;
    u32     m_polylineUnityMult;
    f32     m_enterMult;
    f32     m_leaveMult;
    f32     m_maxDstInfluence;
    u32     m_levelsFront;
    u32     m_levelsBack;
    Vec3d   m_levelDelta;
    Vec2d   m_UVDelta;
    u32     m_perpendicularBack;
    f32     m_perpendicularBackZ;
    f32     m_perpendicularBackScale;
    f32     m_perpendicularBackPos;
    f32     m_perpendicularBackPosZ;
    u32     m_perpendicularFront;
    f32     m_perpendicularFrontZ;
    f32     m_perpendicularFrontScale;
    f32     m_perpendicularFrontPos;
    f32     m_perpendicularFrontPosZ;
    f32     m_weightMultiplier; 
    f32     m_dstInfluenceMultiplier;
    u32     m_layerCount;
    f32     m_influenceLimit;
    u32     m_sideCount;
    f32     m_blendFactor;
    bbool   m_polylineReaction;
    Path    m_fxActor;
    ITF_VECTOR<FluidFriseLayer> m_layers;
};


struct Mesh3dDataElement
{
    DECLARE_SERIALIZE()

    Mesh3dDataElement()
        : m_textureIndex(0)
        , m_familyIndex(0)
    {
    }

    Path        m_meshPath;
    Path        m_dummyPath;
    u32         m_textureIndex;
    StringID    m_family;
    i32         m_familyIndex;
};

struct Mesh3dData
{
    DECLARE_SERIALIZE()

    Vec2d   m_uvMin;
    Vec2d   m_uvMax;

    i32     getMeshIndex(const StringID & _family, i32 _familyIndex) const;
    u32     getNbElmentsInFamily(const StringID & _family) const;

    ITF_VECTOR<Mesh3dDataElement>   m_meshList;
};


struct Mesh3dConfig
{
    DECLARE_SERIALIZE()

    Mesh3dConfig()
        : m_texureTileSize(1.f)
        , m_random(btrue)
    {
    }

    f32     m_texureTileSize;
    bbool   m_random;
    ITF_VECTOR<Mesh3dData>          m_mesh3dList;
};


#ifdef USE_BOX2D
struct Phys2dConfig
{
    DECLARE_SERIALIZE()

    Phys2dConfig()
        : m_density(1.f)
        , m_friction(0.3f)
        , m_restitution(0.f)
        , m_linearDamping(0.f)
        , m_angularDamping(0.f)
        , m_gravityMupliplier(1.f)
        , m_fixedRotation(bfalse)
        , m_isSensor(bfalse)
    {
    }


    f32                 m_density;
    f32                 m_friction;
    f32                 m_restitution;
    f32                 m_linearDamping;
    f32                 m_angularDamping;
    f32                 m_gravityMupliplier;
    bbool               m_fixedRotation;
    bbool               m_isSensor;
};
#endif // USE_BOX2D

struct CollisionFrieze 
{
    DECLARE_SERIALIZE()

    CollisionFrieze()
        : m_offset(0.5f)
        , m_build(bfalse)
        , m_extremity(Vec2d::Infinity)
        , m_extremity2(Vec2d::Infinity)
        , m_isFlip(bfalse)
        , m_methode(0)
        , m_distMaxToSkipParallelEdge(0.075f)
        , m_isSmooth(bfalse)
        , m_smoothFactor(F32_INVALID)         
    {
    };

    f32     m_offset;
    bbool   m_build;   
    Vec2d   m_extremity;
    Vec2d   m_extremity2;
    bbool   m_isFlip;
    u32     m_methode;
    f32     m_distMaxToSkipParallelEdge;
    bbool   m_isSmooth;
    f32     m_smoothFactor;
};

struct CollisionTexture 
{
    DECLARE_SERIALIZE()

    CollisionTexture()
        : m_offset(F32_INVALID)
        , m_build(U32_INVALID)
        , m_cornerInCur(bfalse)
        , m_cornerOutCur(bfalse)
        , m_cornerIn(Vec2d(F32_INVALID, F32_INVALID))
        , m_cornerOut(Vec2d(F32_INVALID, F32_INVALID))
    {
    };

    f32     m_offset;
    u32     m_build;
    bbool   m_cornerInCur;
    bbool   m_cornerOutCur;    
    Vec2d   m_cornerIn;
    Vec2d   m_cornerOut;
};

struct VertexAnim
{
    DECLARE_SERIALIZE()

    VertexAnim()
        : m_globalSpeed(0.f)
        , m_speedX(1.f)
        , m_speedY(1.f)
        , m_syncX(0.f)
        , m_syncY(0.f)
        , m_amplitudeX(1.f)
        , m_amplitudeY(1.f)
        , m_sync(0.f)
        , m_angleUsed(bfalse)
    {
    }

    f32     m_globalSpeed;
    Angle   m_globalRotSpeed;
    f32     m_speedX;
    f32     m_speedY;
    f32     m_syncX;
    f32     m_syncY;
    f32     m_amplitudeX;
    f32     m_amplitudeY;
    f32     m_sync;
    bbool   m_angleUsed;
    Angle   m_angle;
};

class FriseTextureConfig
{
    DECLARE_SERIALIZE()

public:

    FriseTextureConfig();
    ~FriseTextureConfig();

    bbool   onLoaded(class ResourceContainer * _parentContainer);
    void    onDeleted(class ResourceContainer * _parentContainer);

    void    setGameMaterial( const Path& _gameMaterial );
    const class GameMaterial_Template* getGameMaterial() const { return m_gameMaterialTemplate; }

    // accessors
    bbool isDrawnable() const { return hasDiffuseTexture(); }
    bbool hasDiffuseTexture() const { return ! getTexturePath().isEmpty(); }
    const GFX_MATERIAL & getGFXMaterial() const { return m_gfxMaterial; }     
    const Path & getTexturePath() const { return m_gfxMaterial.getTexturePathSet().getTexturePath(); }
    const GFXMaterialSerializable & getGFXSerializableMat() const { return m_gfxMaterial; }

    StringID        m_friendly;
    Path            m_gameMaterial;
    ColorInteger    m_color;
    CollisionTexture m_collisionTex;

    Vec2d   m_scrollingSpeedTrans;  
    f32     m_scrollingSpeedRotate;
    bbool   m_scrollingUseUV2;
    Vec2d   m_scrollingUV2Scale;
    Vec2d   m_scrollingSpeedTransUV2;  
    f32     m_scrollingSpeedRotateUV2;
    f32     m_fillOffset;
    u8      m_alphaBorder;
    u8      m_alphaUp;    

private:
    friend class XMLAll;
    const ResourceContainer * getResourceContainer() const;
    
    GFXMaterialSerializable m_gfxMaterial;

    const class GameMaterial_Template* m_gameMaterialTemplate;
};

struct Frieze3DConfig
{
	DECLARE_SERIALIZE()

	Frieze3DConfig() : m_orient(btrue) {}

	bbool		m_orient; // Orient the mesh along the normals of the curve
	
	ResourceID	m_meshRefFillID;
	ResourceID	m_meshRefLeftID;
	ResourceID	m_meshRefRightID;

	Path		m_meshRefFillFile;
	Path		m_meshRefLeftFile;
	Path		m_meshRefRightFile;
};

///////////////////////////////////////////////////////////////////////////////////////////
class FriseConfig : public TemplatePickable
{
    DECLARE_OBJECT_CHILD_RTTI(FriseConfig, TemplatePickable,4277131661)
    DECLARE_SERIALIZE()

public:
    
                                        FriseConfig() { init(); }
                                        ~FriseConfig();

    void                                init();

    virtual bbool                       onTemplateLoaded( bbool _hotReload );
    virtual void                        onTemplateDelete( bbool _hotReload );
    void                                finalizeData();
    
#ifndef ITF_STRIPPED_DATA
    virtual void                        getTags(ITF_VECTOR<String8>& _tagList)const;
#endif // ITF_STRIPPED_DATA

    ITF_VECTOR<i32>                     m_FriseTextureConfigIndexBySlope;
    ITF_VECTOR<StringID>                m_FriseTextureConfigFriendlyBySlope;
    ITF_VECTOR<i32>                     m_FriseTextureConfigIndexByZone;
    ITF_VECTOR<FriseTextureConfig>      m_textureConfigs;
    StringID                            m_regionId;
    Path                                m_gameMaterial; 
    Path                                m_gameMaterialStartExtremity; 
    Path                                m_gameMaterialStopExtremity; 
    const class GameMaterial_Template*  m_gameMaterialTemplate;
    const class GameMaterial_Template*  m_gameMaterialStartExtremityTemplate;
    const class GameMaterial_Template*  m_gameMaterialStopExtremityTemplate;
    mutable u32                         m_lastUpdateFrame;
        
    bbool                               m_useFriezeFlipToFlipUV;
    f32                                 m_visualOffset;
    f32                                 m_zVtxExtrudeUp;
    f32                                 m_zVtxExtrudeDown;
    f32                                 m_zVtxExtrudeStart;
    f32                                 m_zVtxExtrudeStop;
    f32                                 m_zVtxExtrudeExtremityStart;
    f32                                 m_zVtxExtrudeExtremityStop;
    f32                                 m_patchCoeffOffset;
    f32                                 m_patchScale;
    bbool                               m_patchOriented;
    f32                                 m_flexibility;
    Angle                               m_wallAngle;
    bbool                               m_isUvFlipX;
    bbool                               m_isUvFlipY;    
    bbool                               m_isRatioFixed;
    Angle                               m_patchAngleMin;
    Vec2d                               m_extremityScale;
    f32                                 m_patchCornerFactorCollision;

    f32     m_cornerFactor;
    f32     m_height;
    f32     m_width;
    u32     m_methode;   
    f32     m_snapCoeff;
    f32     m_snapCoeffUv;
    f32     m_skewAngle;
    bbool   m_isDigShape;
    bbool   m_isLockedDigShape;
    bbool   m_switchExtremityAuto;
    f32     m_offsetExtremity;
    bbool   m_invertMeshOrder;

    // smooth
    bbool       m_isSmoothVisual;
    f32         m_smoothFactorVisual;
        
    CollisionFrieze     m_collisionFrieze; 
    FillConfig          m_fill;
    VertexAnim          m_vtxAnim;
    ColorInteger        m_selfIllumColor;
    FluidConfig         m_fluid;
    Mesh3dConfig        m_mesh3d;

	// Frieze 3D
	Frieze3DConfig		m_frieze3DConfig;

#ifdef USE_BOX2D
    Phys2dConfig        m_phys2d;
#endif //USE_BOX2D
    // tex Switch
    i32     m_idTexSwitch;

    // overlays
    f32         m_scale;    
    f32         m_density;

    bbool       m_cooked;

    GFXPrimitiveParam       m_primitiveParam;

#ifndef ITF_STRIPPED_DATA
    GFXPrimitiveParamForced m_primitiveParamForced;
    void                    updateForcedValues();
#endif //!ITF_STRIPPED_DATA

protected:

    void                    validateData();
    void                    processData();
    void                    setCollisionCorner( CollisionTexture& _collisionTex );
};


} // namespace ITF

#endif  // _ITF_FRISECONFIG_H_
