#ifndef _ITF_IS2DNOSCREENRATIOCOMPONENT_H_
#define _ITF_IS2DNOSCREENRATIOCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
	class Is2DNoScreenRatioComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(Is2DNoScreenRatioComponent, ActorComponent, 2114793209);
	public:
		DECLARE_SERIALIZE()

		Is2DNoScreenRatioComponent();
		~Is2DNoScreenRatioComponent() {}
		virtual bbool needsUpdate() const { return btrue; }
		virtual bbool needsDraw() const { return btrue; }
		virtual bbool needsDraw2D() const { return btrue; }
		virtual	bbool needsDraw2DNoScreenRatio() const { return btrue; }

		//virtual bbool is2D() const { return needsDraw2D(); }

		virtual void onActorLoaded(Pickable::HotReloadType _hotReload);

	private:
		ITF_INLINE const class Is2DNoScreenRatioComponent_Template*  getTemplate() const;
	};


	//////////////////////////////////////////////////////////////////////////////////////////

	class Is2DNoScreenRatioComponent_Template : public ActorComponent_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(Is2DNoScreenRatioComponent_Template, ActorComponent_Template, 1611461414);
		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(Is2DNoScreenRatioComponent);

	public:

		Is2DNoScreenRatioComponent_Template();
		~Is2DNoScreenRatioComponent_Template() {}
	};

	const Is2DNoScreenRatioComponent_Template*  Is2DNoScreenRatioComponent::getTemplate() const {return static_cast<const Is2DNoScreenRatioComponent_Template*>(m_template);} 

} // namespace ITF

#endif//_ITF_IS2DNOSCREENRATIOCOMPONENT_H_
