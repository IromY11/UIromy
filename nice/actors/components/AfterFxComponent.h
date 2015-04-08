#ifndef _ITF_AFTERFXCOMPONENT_H_
#define _ITF_AFTERFXCOMPONENT_H_

#ifndef _ITF_AFTERFX_H_
#include "engine/display/AfterFx.h"
#endif //_ITF_AFTERFX_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{
    //class Texture;

	class AfterFxComponent : public ActorComponent
	{
        DECLARE_OBJECT_CHILD_RTTI(AfterFxComponent,ActorComponent,1914573461);
	public:
        DECLARE_SERIALIZE()

        AfterFxComponent();
        ~AfterFxComponent();
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return btrue; }
        virtual bbool needsDraw2D() const { return bfalse; }
		virtual	bbool needsDraw2DNoScreenRatio() const { return bfalse; }

		virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void  batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void  onEvent(Event * _event);
        virtual void  Update( f32 _deltaTime );

        void updateAABB();
        void updateAlpha(f32 _deltaTime);

        void start();
        void stop();
        void pause(bbool _pause) {m_isPause = _pause;}

        void addAfterFx( AFTERFX& _afx ) const;

        ITF_INLINE f32 getParamfAt( u32 _n)const;
        ITF_INLINE i32 getParamiAt( u32 _n)const;
        ITF_INLINE Vec3d getParamvAt( u32 _n)const;
        ITF_INLINE Color getParamcAt( u32 _n)const;

        template <typename T>
        void                        setInput(StringID _inputName, T _value );
        const InputContainer &      getInputs() const { return m_inputList; }
        void                        setUpdateAfterFxInput( IUpdateAfterFxInputCriteria* _update ) { m_afterFxInputUpdate = _update; }
        ITF_INLINE Color getColorRTarget() const { return m_colorRTarget; }
        ITF_INLINE void setColorRTarget( Color _value ) { m_colorRTarget = _value; }

        ITF_INLINE bbool getActivated() const { return m_activated; }
        ITF_INLINE void setActivated( const bbool _value ) { m_activated = _value; }

    private:


        ITF_INLINE const class AfterFxComponent_Template*  getTemplate() const;

        AFTERFX*                m_afterFx;
        void preloadInit();

        bbool                   m_isPause;


        Color                   m_colorRTarget;

        u32                     m_renderintarget;

        //INPUTS
        IUpdateAfterFxInputCriteria*       m_afterFxInputUpdate;
        InputContainer          m_inputList;

        ResourceID              m_textureID;

        class LinkComponent *   m_linkComponent;

        f32                     m_timer;
        bbool                   m_isStarted;
        bbool                   m_activated;
    };

    template <typename T>
    ITF_INLINE void AfterFxComponent::setInput(StringID _inputName, T _value )
    {
        u32 numInputs = m_inputList.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            Input& input = m_inputList[i];

            if (input.getId() == _inputName)
            {
                input.setValue(_value);
                break;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    class AfterFxComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AfterFxComponent_Template,ActorComponent_Template,2558007817);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(AfterFxComponent);

    public:

        AfterFxComponent_Template();
        ~AfterFxComponent_Template() {}

        virtual bbool onTemplateLoaded( bbool _hotReload );
        virtual void onTemplateDelete( bbool _hotReload );

        AFTERFX_Type                            getType() const { return m_type; }
        GFX_BLENDMODE                           getFinalBlendMode() const { return m_finalblend; }
        f32                                     getStartRank() const { return m_startRank; }
        f32                                     getRenderRank() const { return m_renderRank; }
        u32                                     getRenderInTarget() const { return m_renderintarget; }
        const Color&                            getColorRTarget() const { return m_colorRTarget; }
        const ITF_VECTOR <f32>&                 getParamF() const { return m_Paramf; }
        const ITF_VECTOR <i32>&                 getParamI() const { return m_Parami; }
        const ITF_VECTOR <Vec3d>&               getParamV() const { return m_Paramv; }
        const ITF_VECTOR <Color>&               getParamC() const { return m_Paramc; }
        ITF_INLINE const f32                    getParamFAt( u32 _n) const { return m_Paramf[_n]; }
        ITF_INLINE const i32                    getParamIAt( u32 _n) const { return m_Parami[_n]; }
        ITF_INLINE const Vec3d                  getParamVAt( u32 _n) const { return m_Paramv[_n]; }
        ITF_INLINE const Color                  getParamCAt( u32 _n) const { return m_Paramc[_n]; }


        enum AlphaMode
        {
            NONE,
            UNIFORM,
            RADIAL,
        };


        const ITF_VECTOR<InputDesc>&            getInputs() const { return m_inputList; }
        const ITF_VECTOR<ProceduralInputData>&  getParamFProceduralInput() const { return m_ParamfProceduralInput; }
        const Color&                            getColorRTargetMin() const { return m_colorRTargetMin; }
        const Color&                            getColorRTargetMax() const { return m_colorRTargetMax; }
        const ProceduralInputData&              getColorRTargetInputData() const { return m_colorRTargetInputData; }
        const Path&                             getTextureFile() const { return m_customTex0; }

        AlphaMode                               getAlphaMode() const { return m_alphaMode;}
        f32                                     getAlphaRadialProgressionSpeed() const { return m_alphaRadialProgressionSpeed; }
        f32                                     getAlphaFadeTime() const { return m_alphaFadeTime; }
        bbool                                   getAlphaFadeIn() const { return m_alphaFadeIn; }
        const AABB&                             getForcedAABB() const { return m_forcedAABB; }
        f32                                     getAlphaFadeTotalDuration() const { return m_alphaFadeTotalDuration; }
        bbool                                   getDeactivateAfterFade() const { return m_deactivateAfterFade; }
        const ResourceID                        getTextureID() const { return m_textureID; }

    private:

        ResourceID                          m_textureID;
        AFTERFX_Type                        m_type;
        GFX_BLENDMODE                       m_finalblend;
        f32                                 m_startRank;
        f32                                 m_renderRank;
        u32                                 m_renderintarget;
        Color                               m_colorRTarget;
        ITF_VECTOR <f32>                    m_Paramf;
        ITF_VECTOR <i32>                    m_Parami;
        ITF_VECTOR <Vec3d>                  m_Paramv;
        ITF_VECTOR <Color>                  m_Paramc;
        ITF_VECTOR <InputDesc>              m_inputList;
        ITF_VECTOR <ProceduralInputData>    m_ParamfProceduralInput;
        Color                               m_colorRTargetMin;
        Color                               m_colorRTargetMax;
        ProceduralInputData                 m_colorRTargetInputData;
        Path                                m_customTex0;
        AlphaMode                           m_alphaMode;
        f32                                 m_alphaRadialProgressionSpeed;
        f32                                 m_alphaFadeTime;
        bbool                               m_alphaFadeIn;
        f32                                 m_alphaFadeTotalDuration;
        AABB                                m_forcedAABB;
        bbool                               m_deactivateAfterFade;
    };

//INLINED

    const AfterFxComponent_Template*  AfterFxComponent::getTemplate() const {return static_cast<const AfterFxComponent_Template*>(m_template);} 

    f32 AfterFxComponent::getParamfAt( u32 _n) const   { return getTemplate()->getParamFAt(_n);};
    i32 AfterFxComponent::getParamiAt( u32 _n) const   { return getTemplate()->getParamIAt(_n);};
    Vec3d AfterFxComponent::getParamvAt( u32 _n) const { return getTemplate()->getParamVAt(_n);};
    Color AfterFxComponent::getParamcAt( u32 _n) const { return getTemplate()->getParamCAt(_n);};

} // namespace ITF

#endif // _ITF_AFTERFXCOMPONENT_H_
