
#ifndef _ITF_GRAPHICCOMPONENT_H_
#define _ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "../actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef ITF_GFX_PRIMITIVES_H_
#include "engine\display\Primitives\GFXPrimitives.h"
#endif

#ifndef ITF_ENGINE_MESH_H_
# include "engine/display/Primitives/Mesh.h"
#endif //ITF_ENGINE_MESH_H_

namespace ITF
{

struct ShadowObj
{
    //shadow mesh
    ITF_Mesh				m_shadowMesh;
    //TODO: create only if shadow ou alors structure shadow...
    VertexPCT				m_cacheVB[10];

    Vec3d					m_shadowPos[5];
    u32						m_sS;
    u32						m_sE;
    bbool					m_renderShadow;
    f32						m_shadowAlpha;
    f32						m_shadowMul;
};

#define GraphicComponent_CRC ITF_GET_STRINGID_CRC(GraphicComponent,2152159230)
class GraphicComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(GraphicComponent,ActorComponent,2152159230);

public:
    DECLARE_SERIALIZE()

    GraphicComponent();
    ~GraphicComponent();

    virtual bbool           needsUpdate() const { return btrue; }
    virtual bbool           needsDraw() const { return bfalse; }
    virtual bbool           needsDraw2D() const { return bfalse; }
	virtual bbool			needsDraw2DNoScreenRatio() const { return bfalse; }
    virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onStartDestroy(bbool _hotReload);
    virtual void            onActorClearComponents();
    virtual void            onCheckpointLoaded();

    virtual void            Update( f32 _deltaTime );
    virtual void            onEvent( Event * _event);
    virtual AABB            getVisualAABB() const;

    const bbool             useShadow() const {  return m_shadowObj ? btrue : bfalse;  }
    void					setShadowMul(f32 _val) {m_shadowObj->m_shadowMul = _val;}
    void                    setShadowUseBase( bbool _val ) { m_shadowUseBase = _val; }
    void                    setShadowDisableOffset( bbool _val ) { m_shadowDisableOffset = _val; }

    void					createShadowMesh();
    void					destroyShadowMesh();
    void					computeVertexBufferCache();

    Vec2d                   getShadowSize();

    static ITF_IndexBuffer*     getStaticIndexBuffer(u32 _hdiv, u32 _vdiv);
    ITF_IndexBuffer*            getMyStaticIndexBuffer();

    ShadowObj*				getShadowObject() {return m_shadowObj;}
    
    ITF_INLINE f32          getAlpha() const { return m_alpha; }
    ITF_INLINE void         setAlpha( const f32 _alpha ) { m_alpha = _alpha; }
   
    ITF_INLINE const GFXPrimitiveParam& getGfxPrimitiveParam() const                { return m_primitiveParam;}
    ITF_INLINE       GFXPrimitiveParam& getGfxPrimitiveParam()                      { return m_primitiveParam;}
    void                    setGFXPrimitiveParam( const GFXPrimitiveParam& _param);

    ITF_INLINE u32          getHDiv() const { return m_hDiv; }
    ITF_INLINE u32          getVDiv() const { return m_vDiv; }

    void                    setTagId(u32 _tagid) {  m_colorComputerTagId = _tagid;}
    const u32               getTagId() const {  return m_colorComputerTagId;    }
    
    int                     getDisableLight() const { return m_disableLight; }
    void					setDisableLight(	int _v	) {	m_disableLight = _v;	};

    void                    setShadowFilter( u32 _filter ) { m_shadowFilter = _filter; }

    ITF_INLINE const class GraphicComponent_Template*  getTemplate() const;

#ifdef ITF_SUPPORT_COOKING
    virtual void            onCookProcess();
#endif // ITF_SUPPORT_COOKING

#ifndef ITF_STRIPPED_DATA
    GFXPrimitiveParamForced m_primitiveParamForced;
    ITF_INLINE const GFXPrimitiveParamForced& getGfxPrimitiveParamForced() const                { return m_primitiveParamForced;}
    ITF_INLINE GFXPrimitiveParamForced& getGfxPrimitiveParamForced()                      { return m_primitiveParamForced;}
    virtual void            updateForcedValues( const SerializedObjectContent_Object* _content );
#endif // !ITF_STRIPPED_DATA

    const class Angle &     getAngleOffset() const;
    
    f32                     getDepthOffset() const;
    void                    setDepthOffset(f32 _depth) { m_depthOffset = _depth; }
	void					setShadowOffset(Vec3d	_vec) {m_shadowBoneOffset = _vec; }
	Vec3d		            getShadowOffset() {return m_shadowBoneOffset; }

	const Color&			getColorForMask(ux _index) const { return m_primitiveParam.getColorForMask(_index);}
    void					setColorForMask(ux _index, const Color &_color) {m_primitiveParam.setColorForMask(_index, _color);}

    virtual Vec2d           getOffset() const;
    void                    setOffset(const Vec2d& _offset);

protected:

    void                    processAlpha(f32 _deltaTime); // process alpha when override Update
    void                    processShadow(f32 _deltaTime, f32 _angle, const Vec2d &_boneOffset = Vec2d::Zero);
    void                    compuseShadowPosFromBase( const Vec2d& _shadowOffset );
    void                    compuseShadowPosFromContacts( const Vec2d& _shadowOffset );
    void					drawShadow(const ITF_VECTOR <class View*>& _views);

#if defined(ITF_SUPPORT_EDITOR) 
    void                                        onPrePropertyChange();
    void                                        onPostPropertyChange();
    GFXOccludeInfo          m_gfxOccludeInfoPrevious;
#endif

    Vec2d                   m_posOffset;

    u32                     m_hDiv;
    u32                     m_vDiv;
    u32                     m_shadowFilter;
    f32                     m_alpha;
    f32                     m_alphaSrc;
    f32                     m_alphaDst;
    f32                     m_faidingTime;
    f32                     m_faidingValue;
    u32                     m_pauseOnFade : 1,
                            m_destroyOnFade : 1,
                            m_shadowUseBase : 1,
                            m_shadowDisableOffset : 1,
                            m_useShadow : 1;
    int                     m_disableLight;
    int                     m_disableShadow;
    bbool                   m_renderInTarget;
    f32                     m_depthOffset;
	f32						m_alphaInit;

	Vec3d					m_shadowBoneOffset;

    GFXPrimitiveParam       m_primitiveParam;

    // WARNING don't serialize in Final :
    u32                     m_colorComputerTagId;

    // Shadow.
    ShadowObj*				m_shadowObj;

private:
    void                    reset();
};

class GraphicComponent_Template : public ActorComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(GraphicComponent_Template,ActorComponent_Template,1900486610);
    DECLARE_SERIALIZE()
    DECLARE_ACTORCOMPONENT_TEMPLATE(GraphicComponent);

public:

    GraphicComponent_Template();
    ~GraphicComponent_Template() {}

    virtual bbool           onTemplateLoaded( bbool _hotReload );
    virtual void            onTemplateDelete( bbool _hotReload );

    u32                     getPatchLevel() const { return m_patchLevel; }
    u32                     getHDiv() const { return m_hDiv; }
    u32                     getVDiv() const { return m_vDiv; }
    const AABB&             getVisualAABB() const { return m_visualAABB; }
    const Vec2d&            getPosOffset() const { return m_posOffset; }
    const Angle&            getRotOffset() const { return m_angleOffset; }
    GFX_BLENDMODE           getBlendMode() const { return m_blendMode; }
    GFX_MATERIAL_TYPE       getMaterialType() const { return m_matType; }
    Color                   getSelfIllumColor() const { return m_selfIllumColor; }
    ITF_INLINE bbool        getDisableLight() const { return m_disableLight; }
    ITF_INLINE bbool        forceDisableLight() const { return m_forceDisableLight; }

    bbool					getIsUseShadow() const {return m_useShadow;}
	bbool					getUseRootBone() const {return m_useRootBone;}
	bbool					getUseNoColShadow() const {return m_useNoColShadow;}
	const StringID			&getUseBoneName() const { return m_useBoneName; }
	
    const Vec2d&            getShadowSize() const { return m_shadowSize; }
    const f32	            getShadowAttenuation() const { return m_shadowAttenuation; }
    const Vec3d&            getShadowOffsetPos() const {return m_shadowOffsetPos; }
    const f32	            getShadowDist() const { return m_shadowDist; }
    const f32	            getAngleLimit() const { return m_angleLimit; }
    const GFX_MATERIAL &    getShadowMaterial() const { return m_shadowMaterial; }
    const f32	            getDepthOffset() const { return m_depthOffset; }
    const f32               computeSizeFactor(f32 _f) const { return f32_Clamp(powf(_f,m_curveSizePower) * (m_curveSize1 - m_curveSize0) + m_curveSize0, 0.0f, 1.0f); }

private:

    GFXMaterialSerializable m_shadowMaterial;
    u32                     m_patchLevel;
    u32                     m_hDiv;
    u32                     m_vDiv;
    AABB                    m_visualAABB;
    bbool                   m_renderInTarget; //map to m_matflags.
    Vec2d                   m_posOffset;
    Angle                   m_angleOffset;
    GFX_BLENDMODE           m_blendMode;
    GFX_MATERIAL_TYPE       m_matType;
    Color                   m_selfIllumColor;
    bbool                   m_disableLight; // default value, overridable per instance
    bbool                   m_forceDisableLight;    //force disable light value...

    // shadow Params.
    bbool					m_useShadow;
	bbool					m_useNoColShadow;
    bbool					m_useRootBone;
	StringID				m_useBoneName;
    Vec2d					m_shadowSize;
    f32						m_shadowAttenuation;
    f32						m_shadowDist;
    Vec3d					m_shadowOffsetPos;
    f32						m_angleLimit;
    f32                     m_depthOffset;
    f32                     m_curveSize0;
    f32                     m_curveSize1;
    f32                     m_curveSizePower;
};

    //ITF_INLINE const class GraphicComponent_Template* GraphicComponent::getTemplate() const { return static_cast<const GraphicComponent_Template*>(m_template); }
    const GraphicComponent_Template*  GraphicComponent::getTemplate() const {return static_cast<const GraphicComponent_Template*>(m_template);}

}

#endif // _ITF_GRAPHICCOMPONENT_H_

