#ifndef _ITF_SETCHILDREN2DNOSCREENRATIOCOMPONENT_H_
#define _ITF_SETCHILDREN2DNOSCREENRATIOCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
	class SetChildren2DNoScreenRatioComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(SetChildren2DNoScreenRatioComponent, ActorComponent, 4095412656);
	public:
		DECLARE_SERIALIZE()

		SetChildren2DNoScreenRatioComponent();
		~SetChildren2DNoScreenRatioComponent() {}
		virtual bbool needsUpdate() const { return btrue; }
		virtual bbool needsDraw() const { return btrue; }
		virtual bbool needsDraw2D() const { return btrue; }
		virtual	bbool needsDraw2DNoScreenRatio() const { return bfalse; }	// not for me, juste for my children ! add Is2DNoScreenRatioComponent if you want it for me too

		virtual void onFinalizeLoad();

	private:
		ITF_INLINE const class SetChildren2DNoScreenRatioComponent_Template*  getTemplate() const;
	};


	//////////////////////////////////////////////////////////////////////////////////////////

	class SetChildren2DNoScreenRatioComponent_Template : public ActorComponent_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(SetChildren2DNoScreenRatioComponent_Template, ActorComponent_Template, 656487794);
		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(SetChildren2DNoScreenRatioComponent);

	public:

		SetChildren2DNoScreenRatioComponent_Template();
		~SetChildren2DNoScreenRatioComponent_Template() {}
	};

	const SetChildren2DNoScreenRatioComponent_Template*  SetChildren2DNoScreenRatioComponent::getTemplate() const {return static_cast<const SetChildren2DNoScreenRatioComponent_Template*>(m_template);} 

} // namespace ITF

#endif//_ITF_SETCHILDREN2DNOSCREENRATIOCOMPONENT_H_
