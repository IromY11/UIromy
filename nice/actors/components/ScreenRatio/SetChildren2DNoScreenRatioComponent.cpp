#include "precompiled_engine.h"

#ifndef _ITF_SETCHILDREN2DNOSCREENRATIOCOMPONENT_H_
#include "engine/actors/components/ScreenRatio/SetChildren2DNoScreenRatioComponent.h"
#endif //_ITF_SETCHILDREN2DNOSCREENRATIOCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

	IMPLEMENT_OBJECT_RTTI(SetChildren2DNoScreenRatioComponent)
	BEGIN_SERIALIZATION_CHILD(SetChildren2DNoScreenRatioComponent)
	END_SERIALIZATION();

	SetChildren2DNoScreenRatioComponent::SetChildren2DNoScreenRatioComponent()
	{
	}

	void SetChildren2DNoScreenRatioComponent::onFinalizeLoad()
	{
		// update child actors flag
		// onActorLoaded() is too soon
		// Update() is too late
		AIUtils::ActorBindIterator bindIt( m_actor );
		while( Actor * child = bindIt.getNextChild() )
		{
			child->setIs2DNoScreenRatio(btrue);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////

	IMPLEMENT_OBJECT_RTTI(SetChildren2DNoScreenRatioComponent_Template)
	BEGIN_SERIALIZATION_CHILD(SetChildren2DNoScreenRatioComponent_Template)
	END_SERIALIZATION();

	SetChildren2DNoScreenRatioComponent_Template::SetChildren2DNoScreenRatioComponent_Template()
	{
	}

} // namespace ITF


