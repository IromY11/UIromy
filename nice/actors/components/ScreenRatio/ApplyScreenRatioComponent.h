#ifndef _ITF_APPLYSCREENRATIOCOMPONENT_H_
#define _ITF_APPLYSCREENRATIOCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
	class ApplyScreenRatioComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(ApplyScreenRatioComponent, ActorComponent,2312537425);
	public:
		DECLARE_SERIALIZE()

		ApplyScreenRatioComponent();
		~ApplyScreenRatioComponent() {}
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
		ITF_INLINE const class ApplyScreenRatioComponent_Template*  getTemplate() const;
	};


	//////////////////////////////////////////////////////////////////////////////////////////

	class ApplyScreenRatioComponent_Template : public ActorComponent_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(ApplyScreenRatioComponent_Template, ActorComponent_Template, 3143750254);
		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(ApplyScreenRatioComponent);

	public:

		ApplyScreenRatioComponent_Template();
		~ApplyScreenRatioComponent_Template() {}
	};

	const ApplyScreenRatioComponent_Template*  ApplyScreenRatioComponent::getTemplate() const {return static_cast<const ApplyScreenRatioComponent_Template*>(m_template);} 

} // namespace ITF

#endif//_ITF_APPLYSCREENRATIOCOMPONENT_H_
