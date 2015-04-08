#ifndef _ITF_WIKISCREENRATIOCOMPONENT_H_
#define _ITF_WIKISCREENRATIOCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

	class WikiScreenRatioComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(WikiScreenRatioComponent, ActorComponent,498124360);

	public:
		DECLARE_SERIALIZE()

		WikiScreenRatioComponent();
		~WikiScreenRatioComponent() {}
		virtual bbool needsUpdate() const { return btrue; }
		virtual bbool needsDraw() const { return btrue; }
		virtual bbool needsDraw2D() const { return btrue; }
		virtual	bbool needsDraw2DNoScreenRatio() const { return bfalse; }

		virtual void onActorLoaded(Pickable::HotReloadType _hotReload);

		virtual void preUpdatePos( Vec3d &_pos );

	protected:
		void	updateUIPositionForDeviceAspectRatio();
		Vec2d	fixUIPositionForAspectRatio( Vec2d& _inPos );
		f32		m_lastViewportRatio;


	private:
		ITF_INLINE const class WikiScreenRatioComponent_Template*  getTemplate() const;
	};

	


	//////////////////////////////////////////////////////////////////////////////////////////

	class WikiScreenRatioComponent_Template : public ActorComponent_Template 
	{
		DECLARE_OBJECT_CHILD_RTTI(WikiScreenRatioComponent_Template, ActorComponent_Template,2654571364 );

		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(WikiScreenRatioComponent);

	public:

		WikiScreenRatioComponent_Template();
		~WikiScreenRatioComponent_Template() {}
	};

	const WikiScreenRatioComponent_Template*  WikiScreenRatioComponent::getTemplate() const {return static_cast<const WikiScreenRatioComponent_Template*>(m_template);} 
} // namespace ITF

#endif//_ITF_WIKISCREENRATIOCOMPONENT_H_
