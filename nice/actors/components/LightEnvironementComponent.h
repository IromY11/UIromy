#ifndef _ITF_LIGHTENVIRONEMENTCOMPONENT_H_
#define _ITF_LIGHTENVIRONEMENTCOMPONENT_H_

#if (GFX_USE_LIGHTINGMODEL == 2)

#define LightEnvironementComponent_CRC ITF_GET_STRINGID_CRC(LightEnvironementComponent,848476097)
#define LightEnvironementComponent_Template_CRC ITF_GET_STRINGID_CRC(LightEnvironementComponent_Template,1858064492)

#ifndef _ITF_LIGHTMANAGER_H_
#include "engine/display/lighting/LightManager.h"
#endif // _ITF_LIGHTMANAGER_H_

namespace ITF
{
    class LightEnvironementComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(LightEnvironementComponent, ActorComponent, LightEnvironementComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        LightEnvironementComponent();
        virtual ~LightEnvironementComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        Update( f32 _dt );
        virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void        onEvent( Event* _event);
		void				updateAABB();

#ifdef ITF_SUPPORT_EDITOR
        virtual void  drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class LightEnvironementComponent_Template* getTemplate() const;

		GFX_LightEnvironement m_lightParam;
    };


    //---------------------------------------------------------------------------------------------------

    class LightEnvironementComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(LightEnvironementComponent_Template, ActorComponent_Template, LightEnvironementComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(LightEnvironementComponent)
        DECLARE_SERIALIZE()

    public:

        LightEnvironementComponent_Template();
        virtual ~LightEnvironementComponent_Template();


    private:

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const LightEnvironementComponent_Template* LightEnvironementComponent::getTemplate() const
    {
        return static_cast<const LightEnvironementComponent_Template*>(m_template);
    }
}

#endif // (GFX_USE_LIGHTINGMODEL == 2)

#endif // _ITF_LIGHTENVIRONEMENTCOMPONENT_H_
