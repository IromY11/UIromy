#include "precompiled_engine.h"

#ifndef _ITF_APPLYSCREENRATIOWITHSCALECOMPONENT_H_
#include "engine/actors/components/ScreenRatio/ApplyScreenRatioWithScaleComponent.h"
#endif //_ITF_APPLYSCREENRATIOWITHSCALECOMPONENT_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT_H_

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
	IMPLEMENT_OBJECT_RTTI(ApplyScreenRatioBackgroundScreenComponent)
	BEGIN_SERIALIZATION_CHILD(ApplyScreenRatioBackgroundScreenComponent)
	END_SERIALIZATION();

	ApplyScreenRatioBackgroundScreenComponent::ApplyScreenRatioBackgroundScreenComponent()
	{ 
		m_fullScreen = 1; 
	}


	IMPLEMENT_OBJECT_RTTI(ApplyScreenRatioWithScaleComponent)
	BEGIN_SERIALIZATION_CHILD(ApplyScreenRatioWithScaleComponent)
	END_SERIALIZATION();

	ApplyScreenRatioWithScaleComponent::ApplyScreenRatioWithScaleComponent()
		:	m_lastViewportRatio(1.0f),
			m_fullScreen(0)
	{
	}

	void ApplyScreenRatioWithScaleComponent::onActorLoaded(Pickable::HotReloadType _hotReload )
	{
#if defined(_RATIO_ENABLED_)
		Vec2d scale = m_actor->getScale();

		#if defined(ITF_SUPPORT_EDITOR)
			// early return if not exact 4/3 aspect ratio
			if( !CONFIG->m_editorOptions.m_UITestRatio )
				return;
		#endif

        float ratio = GFX_ADAPTER->getScreenWidth() * 9.0f / 16.0f;
		ratio = GFX_ADAPTER->getScreenHeight() / ratio;
        
		
		return;
		/*

		// Apply the ratio to the scale as well
		scale.x() = scale.x() * ratio; 
		scale.y() = scale.y() * ratio; 

        m_actor->setScale(scale);

		if( m_fullScreen )
		{
			Vec2d pos = m_actor->get2DPos();
			Vec2d oldPos = pos;
			fixUIPositionForAspectRatio(pos);

			LOG( "ApplyScreenRatioWithScale FULLSCREEN : onActorLoaded %s from %0.2f %0.2f to %0.2f %0.2f", this->m_actor->getUserFriendly().cStr(), oldPos.x(), oldPos.y(), pos.x(), pos.y());

			m_actor->set2DPos(pos);
		}
		else
		{
			Vec2d pos = m_actor->get2DPos();
			LOG( "ApplyScreenRatioWithScale NOT FULLSCREEN : onActorLoaded %s - CurPos %0.2f %0.2f", this->m_actor->getUserFriendly().cStr(), pos.x(), pos.y());
	
		}
		*/
#endif
	}


	void ApplyScreenRatioWithScaleComponent::preUpdatePos( Vec3d &_pos )
	{
#if defined(_RATIO_ENABLED_)
		#if defined(ITF_SUPPORT_EDITOR)
			// early return if not exact 4/3 aspect ratio
			if( !CONFIG->m_editorOptions.m_UITestRatio )
				return;
		#endif


		return;

		/*

		// Aspect Ratio repositionning
        View::getCurrentView()->compute3DTo2D(_pos, updatedPos);
        translatedPos = updatedPos;

		// restore previous, then apply again
		if( m_fullScreen )
			translatedPos.y() = translatedPos.y() / m_lastViewportRatio; // default is 1.0f


		m_lastViewportRatio = GFX_ADAPTER->getScreenWidth() * 9.0f / 16.0f;
		m_lastViewportRatio = GFX_ADAPTER->getScreenHeight() / m_lastViewportRatio;

		
		//if(m_actor->getUserFriendlyID().GetValue() == 0x9df4da30 )
		{
			// retour en coordonnées de départ
			Vec2d orgPos = m_actor->referencePos2dToCurrentResolution(translatedPos.truncateTo2D() );

			updatedPos = orgPos.to3d(updatedPos.z() );

			// updatedPos.x() = translatedPos.x() / ( 798.0f / 1024.0f );

		}
		


		// updatedPos.x() = translatedPos.x() * m_lastViewportRatio ;
		// updatedPos.y() = translatedPos.y() * m_lastViewportRatio ;

        View::getCurrentView()->compute2DTo3D(updatedPos, _pos);
		*/

		// _pos : Pos2d courante (recalé dans le viewport actuel)
		// si on scale les positions, on va décaler

		//Vec2d tmpPos = m_actor->get2DPos();

		//_pos = m_actor->getPos(); // sans le ratio


		//return; //
	
		/*Vec2d orgPos = m_actor->referencePos2dToCurrentResolution(translatedPos.truncateTo2D() );

		updatedPos = orgPos.to3d(updatedPos.z() );
		_pos = updatedPos;
		*/

#endif
	}

	Vec2d ApplyScreenRatioWithScaleComponent::fixUIPositionForAspectRatio( Vec2d& _inPos )
	{
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
	}


	void ApplyScreenRatioWithScaleComponent::updateUIPositionForDeviceAspectRatio()
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

	IMPLEMENT_OBJECT_RTTI(ApplyScreenRatioWithScaleComponent_Template)
	BEGIN_SERIALIZATION_CHILD(ApplyScreenRatioWithScaleComponent_Template)
	END_SERIALIZATION();

	ApplyScreenRatioWithScaleComponent_Template::ApplyScreenRatioWithScaleComponent_Template()
	{
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////

	IMPLEMENT_OBJECT_RTTI(ApplyScreenRatioBackgroundScreenComponent_Template)
		BEGIN_SERIALIZATION_CHILD(ApplyScreenRatioBackgroundScreenComponent_Template)
		END_SERIALIZATION();

	ApplyScreenRatioBackgroundScreenComponent_Template::ApplyScreenRatioBackgroundScreenComponent_Template()
	{
	}

} // namespace ITF


