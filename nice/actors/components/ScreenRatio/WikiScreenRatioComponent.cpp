#include "precompiled_engine.h"

#ifndef _ITF_WIKISCREENRATIOCOMPONENT_H_
#include "engine/actors/components/ScreenRatio/WikiScreenRatioComponent.h"
#endif //_ITF_WIKISCREENRATIOCOMPONENT_H_


#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#if defined(ITF_SUPPORT_EDITOR)
#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_
#endif //ITF_SUPPORT_EDITOR

#if defined(ITF_IOS) || defined(ITF_ANDROID) || defined(ITF_SUPPORT_EDITOR)
	#define _RATIO_ENABLED_
#endif


namespace ITF
{
	IMPLEMENT_OBJECT_RTTI(WikiScreenRatioComponent)
	BEGIN_SERIALIZATION_CHILD(WikiScreenRatioComponent)
	END_SERIALIZATION();

	WikiScreenRatioComponent::WikiScreenRatioComponent()
		:	m_lastViewportRatio(1.0f)
	{
	}

	void WikiScreenRatioComponent::onActorLoaded(Pickable::HotReloadType _hotReload )
	{
		m_actor->setIs2DNoScreenRatio(btrue);
		return;
		/*
#if defined(_RATIO_ENABLED_)
		Vec2d scale = m_actor->getScale();

		#if defined(ITF_SUPPORT_EDITOR)
			// early return if not exact 4/3 aspect ratio
			if( !CONFIG->m_editorOptions.m_UITestRatio )
				return;
		#endif

		// only apply the scale here !
        float ratio = GFX_ADAPTER->getScreenWidth() * 9.0f / 16.0f;
		ratio = GFX_ADAPTER->getScreenHeight() / ratio;
        
		return;

#endif*/
        
	}


	void WikiScreenRatioComponent::preUpdatePos( Vec3d &_pos )
	{
#if defined(_RATIO_ENABLED_)
		#if defined(ITF_SUPPORT_EDITOR)
			// early return if not exact 4/3 aspect ratio
			if( !CONFIG->m_editorOptions.m_UITestRatio )
				return;
		#endif

		return; // Done already
		/*
        Vec3d translatedPos;
		Vec3d updatedPos = _pos;

		// Aspect Ratio repositionning
        View::getCurrentView()->compute3DTo2D(_pos, updatedPos);
        translatedPos = updatedPos;


		m_lastViewportRatio = GFX_ADAPTER->getScreenWidth() * 9.0f / 16.0f;
		m_lastViewportRatio = GFX_ADAPTER->getScreenHeight() / m_lastViewportRatio;


		updatedPos.y() = translatedPos.y() * m_lastViewportRatio ;

        View::getCurrentView()->compute2DTo3D(updatedPos, _pos);
		*/
#endif
	}

	Vec2d WikiScreenRatioComponent::fixUIPositionForAspectRatio( Vec2d& _inPos )
	{
		/*
		Vec2d updatedPos;

		updatedPos.x() = _inPos.x();	// Ne change pas en x
		updatedPos.y() = _inPos.y();

#if defined(_RATIO_ENABLED_)
		// Aspect Ratio repositioning
		#if defined(ITF_SUPPORT_EDITOR)
			// early return if not exact 4/3 aspect ratio
			if( !CONFIG->m_editorOptions.m_UITestRatio )
				return _inPos;
		#endif

		// go back to original pos
		Vec2d oldPos = _inPos;

		_inPos.y() = _inPos.y() / m_lastViewportRatio; // default is 1.0f


		m_lastViewportRatio = GFX_ADAPTER->getScreenWidth() * 9.0f / 16.0f;
		m_lastViewportRatio = GFX_ADAPTER->getScreenHeight() / m_lastViewportRatio;

		updatedPos.y() = _inPos.y() * m_lastViewportRatio; // we should take in account the size


		LOG( "ApplyScreenRatioWithScale : fixUIPositionForAspectRatio %s from %0.2f %0.2f to %0.2f %0.2f", this->m_actor->getUserFriendly().cStr(), oldPos.x(), oldPos.y(), updatedPos.x(), updatedPos.y());

#endif

		return updatedPos;
		*/
		return _inPos;
	}


	void WikiScreenRatioComponent::updateUIPositionForDeviceAspectRatio()
	{
		// Fix Aspect Ratio
		bool moveit=true;

		// Do not reposition UIMenu, as the whole menus would move
		if( IsClassCRC(UIMenu_CRC) )
			moveit=false;

		// Reposition the UI, based on the current device
		if(  m_actor->getIs2D() && moveit && !m_actor->getIs2DNoScreenRatio() )
		{
            Vec2d absPos = m_actor->get2DPos();
            Vec2d bound2dPos = fixUIPositionForAspectRatio(absPos);
			LOG( "ApplyScreenRatioWithScale : updateUIPositionForDeviceAspectRatio %s from %0.2f %0.2f to %0.2f %0.2f", this->m_actor->getUserFriendly().cStr(), absPos.x(), absPos.y(), bound2dPos.x(), bound2dPos.y());
            m_actor->set2DPos(bound2dPos);
		}
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////

	IMPLEMENT_OBJECT_RTTI(WikiScreenRatioComponent_Template)
	BEGIN_SERIALIZATION_CHILD(WikiScreenRatioComponent_Template)
	END_SERIALIZATION();

	WikiScreenRatioComponent_Template::WikiScreenRatioComponent_Template()
	{
	}


} // namespace ITF


