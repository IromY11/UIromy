#include "precompiled_engine.h"

#ifndef _ITF_IS2DNOSCREENRATIOCOMPONENT_H_
#include "engine/actors/components/ScreenRatio/Is2DNoScreenRatioComponent.h"
#endif //_ITF_IS2DNOSCREENRATIOCOMPONENT_H_

namespace ITF
{

	IMPLEMENT_OBJECT_RTTI(Is2DNoScreenRatioComponent)
	BEGIN_SERIALIZATION_CHILD(Is2DNoScreenRatioComponent)
	END_SERIALIZATION();

	Is2DNoScreenRatioComponent::Is2DNoScreenRatioComponent()
	{
	}

	void Is2DNoScreenRatioComponent::onActorLoaded(Pickable::HotReloadType _hotReload )
	{
		//m_actor->setIs2D(btrue);		
		// is2D flag already set to btrue in void Actor::onLoadPreProcessComponents( HotReloadType _hotReload ) 
		// if (pComponent->is2D())
		//		setIs2D(btrue);

		m_actor->setIs2DNoScreenRatio(btrue);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////

	IMPLEMENT_OBJECT_RTTI(Is2DNoScreenRatioComponent_Template)
	BEGIN_SERIALIZATION_CHILD(Is2DNoScreenRatioComponent_Template)
	END_SERIALIZATION();

	Is2DNoScreenRatioComponent_Template::Is2DNoScreenRatioComponent_Template()
	{
	}

} // namespace ITF


