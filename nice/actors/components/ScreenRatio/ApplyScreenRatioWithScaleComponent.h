#ifndef _ITF_APPLYSCREENRATIOWITHSCALECOMPONENT_H_
#define _ITF_APPLYSCREENRATIOWITHSCALECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

	class ApplyScreenRatioWithScaleComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(ApplyScreenRatioWithScaleComponent, ActorComponent,3877723710);

	public:
		DECLARE_SERIALIZE()

		ApplyScreenRatioWithScaleComponent();
		~ApplyScreenRatioWithScaleComponent() {}
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
		i32		m_fullScreen;

	private:
		ITF_INLINE const class ApplyScreenRatioWithScaleComponent_Template*  getTemplate() const;
	};

	class ApplyScreenRatioBackgroundScreenComponent : public ApplyScreenRatioWithScaleComponent 
	{
		DECLARE_OBJECT_CHILD_RTTI(ApplyScreenRatioBackgroundScreenComponent, ActorComponent,3888592547);

	public:
		DECLARE_SERIALIZE()

		ApplyScreenRatioBackgroundScreenComponent();

	private:
		ITF_INLINE const class ApplyScreenRatioBackgroundScreenComponent_Template*  getTemplate() const;
	};


	//////////////////////////////////////////////////////////////////////////////////////////

	class ApplyScreenRatioWithScaleComponent_Template : public ActorComponent_Template 
	{
		DECLARE_OBJECT_CHILD_RTTI(ApplyScreenRatioWithScaleComponent_Template, ActorComponent_Template,3178089050 );

		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(ApplyScreenRatioWithScaleComponent);

	public:

		ApplyScreenRatioWithScaleComponent_Template();
		~ApplyScreenRatioWithScaleComponent_Template() {}
	};

	//////////////////////////////////////////////////////////////////////////////////////////

	class ApplyScreenRatioBackgroundScreenComponent_Template : public ActorComponent_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(ApplyScreenRatioBackgroundScreenComponent_Template, ActorComponent_Template,1392689164 );

		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(ApplyScreenRatioBackgroundScreenComponent);

	public:

		ApplyScreenRatioBackgroundScreenComponent_Template();
		~ApplyScreenRatioBackgroundScreenComponent_Template() {}
	};




	const ApplyScreenRatioWithScaleComponent_Template*  ApplyScreenRatioWithScaleComponent::getTemplate() const {return static_cast<const ApplyScreenRatioWithScaleComponent_Template*>(m_template);} 
	const ApplyScreenRatioBackgroundScreenComponent_Template*  ApplyScreenRatioBackgroundScreenComponent::getTemplate() const {return static_cast<const ApplyScreenRatioBackgroundScreenComponent_Template*>(m_template);} 

} // namespace ITF

#endif//_ITF_APPLYSCREENRATIOWITHSCALECOMPONENT_H_
