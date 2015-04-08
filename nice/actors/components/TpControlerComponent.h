#ifndef _ITF_TPCONTROLERCOMPONENT_H_
#define _ITF_TPCONTROLERCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

namespace ITF
{
    class TpControlerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TpControlerComponent,ActorComponent,1679496276);

    public:
        DECLARE_SERIALIZE()

        TpControlerComponent();
        //~TpControlerComponent();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return bfalse; }
		virtual	bbool needsDraw2DNoScreenRatio() const { return bfalse; }

		virtual void  Update(f32 _deltaTime);
        virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        ITF_INLINE const class TpControlerComponent_Template*  getTemplate() const;

    private:

        Vec2d               m_position2D;

        // Percentage of the screen
        f32                 m_relativePosX; 
        f32                 m_relativePosY;
        Path                m_instanceFile;
    };

    //---------------------------------------------------------------------------------------------------

    class TpControlerComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TpControlerComponent_Template, ActorComponent_Template,639472293);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(TpControlerComponent);

    public:

        TpControlerComponent_Template() {}
        virtual ~TpControlerComponent_Template() {}

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const TpControlerComponent_Template* TpControlerComponent::getTemplate() const
    {
        return static_cast<const TpControlerComponent_Template*>(m_template);
    }
}
#endif // _ITF_TpControlerComponent_H_

