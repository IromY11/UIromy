#ifndef ITF_CLEARCOLORCOMPONENT_H_
#define ITF_CLEARCOLORCOMPONENT_H_

#ifndef _ITF_RENDERPARAMMANAGER_H_
#include "engine/display/RenderParamManager.h"
#endif //_ITF_RENDERPARAMMANAGER_H_

#define ClearColorComponent_CRC ITF_GET_STRINGID_CRC(ClearColorComponent,2931499403)
#define ClearColorComponent_Template_CRC ITF_GET_STRINGID_CRC(ClearColorComponent_Template,2858114477)


namespace ITF
{
    class ClearColorComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ClearColorComponent, ActorComponent, ClearColorComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        ClearColorComponent();
        virtual ~ClearColorComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return m_actor->getIs2DNoScreenRatio(); }

        virtual void        Update(f32 _deltaTime);
        virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );

        const Color&        getColor() const {return m_clearColor.m_clearColor;}
        const Color&        getColorFrontLight() const {return m_clearColor.m_clearFrontLightColor;}
        const Color&        getColorBackLight() const {return m_clearColor.m_clearBackLightColor;}

        ITF_INLINE void     setColor( const Color& _value ) { m_clearColor.m_clearColor = _value; }
        ITF_INLINE void     setColorFrontLight( const Color& _value ) { m_clearColor.m_clearFrontLightColor = _value; }
        ITF_INLINE void     setColorBackLight( const Color& _value ) {  m_clearColor.m_clearBackLightColor = _value; }

#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class ClearColorComponent_Template* getTemplate() const;
        void computeUnitScaledAABB();

        SubRenderParam_ClearColor m_clearColor;
		GFX_RenderParam	m_renderParam;
    };


    //---------------------------------------------------------------------------------------------------

    class ClearColorComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(ClearColorComponent_Template, ActorComponent_Template, ClearColorComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(ClearColorComponent)
        DECLARE_SERIALIZE()

    public:

        ClearColorComponent_Template();
        virtual ~ClearColorComponent_Template();

    private:
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const ClearColorComponent_Template* ClearColorComponent::getTemplate() const
    {
        return static_cast<const ClearColorComponent_Template*>(m_template);
    }
}

#endif // ITF_CLEARCOLORCOMPONENT_H_
