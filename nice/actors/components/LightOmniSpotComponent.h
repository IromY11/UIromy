#ifndef _ITF_LIGHTOMNISPOTCOMPONENT_H_
#define _ITF_LIGHTOMNISPOTCOMPONENT_H_

#define LightOmniSpotComponent_CRC ITF_GET_STRINGID_CRC(LightOmniSpotComponent,3576856458)
#define LightOmniSpotComponent_Template_CRC ITF_GET_STRINGID_CRC(LightOmniSpotComponent_Template,1980189102)

#ifndef _ITF_LIGHTMANAGER_H_
#include "engine/display/lighting/LightManager.h"
#endif // _ITF_LIGHTMANAGER_H_

#if (GFX_USE_LIGHTINGMODEL == 2)

namespace ITF
{
    class LightOmniSpotComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(LightOmniSpotComponent, ActorComponent, LightOmniSpotComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        LightOmniSpotComponent();
        virtual ~LightOmniSpotComponent();

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
        ITF_INLINE const class LightOmniSpotComponent_Template* getTemplate() const;

		GFX_LightOmniSpot m_lightParam;
    };


    //---------------------------------------------------------------------------------------------------

    class LightOmniSpotComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(LightOmniSpotComponent_Template, ActorComponent_Template, LightOmniSpotComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(LightOmniSpotComponent)
        DECLARE_SERIALIZE()

    public:

        LightOmniSpotComponent_Template();
        virtual ~LightOmniSpotComponent_Template();


    private:

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const LightOmniSpotComponent_Template* LightOmniSpotComponent::getTemplate() const
    {
        return static_cast<const LightOmniSpotComponent_Template*>(m_template);
    }
}

#endif // (GFX_USE_LIGHTINGMODEL == 2)

#endif // _ITF_LIGHTOMNISPOTCOMPONENT_H_
