#ifndef ITF_FOGBOXCOMPONENT_H_
#define ITF_FOGBOXCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

namespace ITF
{
    class DetectorComponent;
    #define FogBoxComponent_CRC ITF_GET_STRINGID_CRC(FogBoxComponent,2758206841)
    class FogBoxComponent : public ActorComponent
	{
        DECLARE_OBJECT_CHILD_RTTI(FogBoxComponent, ActorComponent, 2758206841);
	public:
        DECLARE_SERIALIZE()

        FogBoxComponent();
        ~FogBoxComponent();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return btrue; }
        virtual bbool needsDraw2D() const { return bfalse; }
		virtual	bbool needsDraw2DNoScreenRatio() const { return bfalse; }
		
		virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void  Update(f32 _deltaTime);
		virtual void  batchPrimitives( const ITF_VECTOR <class View*>& _views );
		virtual void  onEvent( Event * _event );
		virtual void  onPostPropertyChange();
#ifdef ITF_SUPPORT_EDITOR
        virtual void  drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        const Color& getNearColor() const {return m_currentNearColor;}
        const Color& getFarColor() const {return m_currentFarColor;}
        const Vec2d& getAttenuation() const {return m_attenuationDist;}
        const f32& getNearDistance() const { return m_nearDistance; }
        const f32& getFarDistance() const { return m_farDistance; } 

        ITF_INLINE void         setNearColor( const Color& _value ) { m_nearColor = _value; onPostPropertyChange();}
        ITF_INLINE void         setNearDistance( const f32& _value ) { m_nearDistance = _value; }
        ITF_INLINE void         setFarColor( const Color& _value ) { m_farColor = _value; onPostPropertyChange();}
        ITF_INLINE void         setFarDistance( const f32& _value ) { m_farDistance = _value; }

    private:
		virtual void updateColorTransitions(f32 _dt);

        inline const class FogBoxComponent_Template *  getTemplate() const;

		Vec2d   m_attenuationDist;
		Color   m_currentNearColor;
		Color   m_currentFarColor;
		Color   m_nearColor;
		Color   m_farColor;
        f32     m_nearDistance;
        f32     m_farDistance;
        bbool   m_useNearOffset;
        f32     m_ZWorldCliping;

		// Color Transitioning
		Color m_colorSrc;
		Color m_colorDst;
		f32 m_colorBlendTime;
		f32 m_colorBlendTimeTotal;

		f32 m_alphaOriginalFar;
		f32 m_alphaOriginalNear;
		f32 m_alphaSrc;
		f32 m_alphaDst;
		f32 m_fadingTime;
		f32 m_fadingTimeTotal;

		bbool m_pauseOnFade;
		bbool m_destroyOnFade;
#ifdef ITF_SUPPORT_DEBUGFEATURE
		bbool m_dbgTranstest;
		bbool m_dbgTranstestPrev;
		EventShow m_toSend;
#endif
    };

    //////////////////////////////////////////////////////////////////////////////////////////

    class FogBoxComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(FogBoxComponent_Template, ActorComponent_Template, 914676649);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(FogBoxComponent);

    public:

        FogBoxComponent_Template();
        ~FogBoxComponent_Template() {}

    private:
        virtual bbool onTemplateLoaded( bbool _hotReload );
        virtual void onTemplateDelete( bbool _hotReload );
    };

    //INLINED
    inline const FogBoxComponent_Template *  FogBoxComponent::getTemplate() const {return static_cast<const FogBoxComponent_Template*> (m_template); }

} // namespace ITF

#endif // ITF_FOGBOXCOMPONENT_H_
