#include "precompiled_engine.h"

#ifndef ITF_FOGBOXCOMPONENT_H_
#include "engine/actors/components/fogBoxComponent.h"
#endif //ITF_FOGBOXCOMPONENT_H_

#ifndef _ITF_FOG_H_
#include "engine\display\Fog.h"
#endif //_ITF_FOG_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(FogBoxComponent)
    BEGIN_SERIALIZATION_CHILD(FogBoxComponent)
      BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("attenuationDist",m_attenuationDist);
        SERIALIZE_MEMBER("near",m_nearDistance);
        SERIALIZE_MEMBER("far",m_farDistance);
        SERIALIZE_MEMBER("nearColor",m_nearColor); 
        SERIALIZE_MEMBER("farColor",m_farColor);
		SERIALIZE_MEMBER("useNearOffset",m_useNearOffset);
        SERIALIZE_MEMBER("ZWorldCliping",m_ZWorldCliping);
      END_CONDITION_BLOCK()
      BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
#ifdef ITF_SUPPORT_DEBUGFEATURE
		SERIALIZE_BOOL("DEBUG_TRANSITION_TEST",m_dbgTranstest);
		SERIALIZE_OBJECT("DEBUG_TRANSITION_SEND",m_toSend);
#endif
      END_CONDITION_BLOCK()
    END_SERIALIZATION();
    
    FogBoxComponent::FogBoxComponent()
    {
		m_attenuationDist = Vec2d(2.0f, 2.0f);
		m_currentNearColor = Color::grey();
		m_currentFarColor = Color::grey();
		m_nearColor = Color::grey();
		m_farColor = Color::grey();
        m_nearDistance = 1.0f;
        m_farDistance = 1.0f;
        m_useNearOffset = bfalse;
        m_ZWorldCliping = 0.0f;

		m_colorSrc = Color::grey();
		m_colorDst = Color::grey();
		m_colorBlendTime = 0.0f;
		m_colorBlendTimeTotal = 0.0f;

		m_alphaOriginalFar = -1.0f;
		m_alphaOriginalNear = 0.0f;
		m_alphaSrc = 1.0f;
		m_alphaDst = 0.0f;
		m_fadingTime = 0.0f;
		m_fadingTimeTotal = 0.0f;

		m_pauseOnFade = bfalse;
		m_destroyOnFade = bfalse;
#ifdef ITF_SUPPORT_DEBUGFEATURE
		m_dbgTranstest = bfalse;
		m_dbgTranstestPrev = bfalse;
#endif
    }

    FogBoxComponent::~FogBoxComponent()
    {
    }
    
#ifdef ITF_SUPPORT_EDITOR
    void FogBoxComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_actor->isSelected())
        {
            f32 actorDepth = m_actor->getDepth();
            AABB attAABB = m_actor->getAABB();

            Vec2d vecScale;
            const Vec2d vecSize(attAABB.getWidth(), attAABB.getHeight());
            if(vecSize.x() != 0 && vecSize.y()!=0)
            {
                const Vec2d vecDist(m_attenuationDist.x(), m_attenuationDist.y());
                vecScale = (vecSize - vecDist * 2) / vecSize;
                Vec2d::Min(&vecScale, &vecScale, &Vec2d::One);
                Vec2d::Max(&vecScale, &vecScale, &Vec2d::Zero);
            }
            else
            {
                vecScale = Vec2d::One;
            }
            attAABB.ScaleFromCenter(vecScale);
            GFX_ADAPTER->drawDBGAABB( attAABB, 1.f, 1.f, 1.f, 0.0f, 1.f, actorDepth );
        }
    }

    void FogBoxComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
        Update(0.0f);
    }

#endif // ITF_SUPPORT_EDITOR

    void FogBoxComponent::Update(f32 _deltaTime)
    {
        //update actor
        AABB aabb(Vec2d(-0.5, -0.5), Vec2d(0.5, 0.5));
        aabb.Scale(GetActor()->getScale());
        aabb.Translate(GetActor()->get2DPos());
        GetActor()->growAABB(aabb);

		updateColorTransitions(_deltaTime);
    }

    void FogBoxComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
        Super::batchPrimitives(_views);

        //update fog manager
        FogParam param = FogParam(m_actor->getAABB(),
            m_attenuationDist,
            m_nearDistance,
            getNearColor(),
            m_farDistance,
            getFarColor(),
            m_useNearOffset,
            m_ZWorldCliping
            );

        GFX_ADAPTER->getFogManager().addFogBox( param );
    }

    void FogBoxComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

		m_currentNearColor = m_nearColor;
		m_currentFarColor = m_farColor;
        //update actor
        AABB aabb(Vec2d(-0.5, -0.5), Vec2d(0.5, 0.5));
        aabb.Scale(GetActor()->getScale());
        aabb.Translate(GetActor()->get2DPos());
        GetActor()->growAABB(aabb);

		m_actor->registerEvent(EventShow_CRC,this);

		m_alphaOriginalFar = m_currentFarColor.getAlpha();
		m_alphaOriginalNear = m_currentNearColor.getAlpha();
    }

	void FogBoxComponent::onPostPropertyChange()
	{
		m_currentNearColor = m_nearColor;
		m_currentFarColor = m_farColor;

		m_alphaOriginalFar = m_currentFarColor.getAlpha();
		m_alphaOriginalNear = m_currentNearColor.getAlpha();
	}

	void FogBoxComponent::onEvent( Event * _event )
	{
		if(EventShow* _eventShow = DYNAMIC_CAST(_event,EventShow))
		{
			if ( _eventShow->getIsOverrideColor() && m_currentNearColor != _eventShow->getOverrideColor() )
			{
				m_colorSrc = m_currentFarColor;
				m_colorDst = _eventShow->getOverrideColor();
				m_colorBlendTimeTotal = _eventShow->getTransitionTime();
				m_colorBlendTime = m_colorBlendTimeTotal;

				if ( m_colorBlendTimeTotal <= 0.f )
				{
					m_colorSrc = m_currentNearColor = m_currentFarColor = m_colorDst;
				}
			}
			if ( _eventShow->getIsOverrideAlpha() )
			{
				m_alphaDst = _eventShow->getAlpha();
				if(_eventShow->getIsOverrideColor())
					m_alphaOriginalFar = m_colorDst.getAlpha();
				else if(m_alphaOriginalFar == -1.0f)
					m_alphaOriginalFar = m_currentFarColor.getAlpha();

				if ( _eventShow->getTransitionTime() <= 0.f || m_alphaDst == m_alphaSrc )
				{
					m_currentFarColor.setAlpha(m_alphaOriginalFar*m_alphaDst);
					m_currentNearColor.setAlpha(m_alphaOriginalNear*m_alphaDst);
					m_alphaSrc			= m_alphaDst;
					m_fadingTimeTotal   = 0.f;
					m_fadingTime		= 0.f;
				}
				else
				{
					m_fadingTimeTotal	= _eventShow->getTransitionTime();
					m_fadingTime		= m_fadingTimeTotal;
				}
			}

			m_pauseOnFade	= _eventShow->getPauseOnEnd();
			m_destroyOnFade = _eventShow->getDestroyOnEnd();
		}
	}

	void FogBoxComponent::updateColorTransitions(f32 _dt)
	{
#ifdef ITF_SUPPORT_DEBUGFEATURE
		if(m_dbgTranstest != m_dbgTranstestPrev)
		{
			onEvent(&m_toSend);
			m_dbgTranstestPrev = m_dbgTranstest;
		}
#endif

		bbool colorFinished = bfalse;
		bbool alphaFinished = bfalse;
		// Process Color
		if( m_colorSrc != m_colorDst && m_colorBlendTimeTotal > 0.f )
		{
			m_colorBlendTime = Max(m_colorBlendTime-_dt,0.f);

			f32 t = 1.f - ( m_colorBlendTime / m_colorBlendTimeTotal );

			if ( t >= 1.f )
			{
				m_currentFarColor = m_currentNearColor = m_colorSrc = m_colorDst;
				colorFinished = btrue;
			}
			else
			{
				m_currentFarColor = m_currentNearColor = Color::Interpolate(m_colorSrc,m_colorDst,t);
				colorFinished = bfalse;
			}
		}
		else
			colorFinished = btrue;

		// Process Alpha
		if ( m_fadingTimeTotal > 0.f && m_fadingTime > 0.f && m_alphaDst != m_alphaSrc ) 
		{
			m_fadingTime = Max(m_fadingTime-_dt,0.f);

			f32 t = 1.f - ( m_fadingTime / m_fadingTimeTotal );

			f32 alphaToSet = 0.0f;
			if ( t >= 1.f )
			{
				alphaToSet = m_alphaSrc = m_alphaDst;
				m_currentFarColor.setAlpha(m_alphaOriginalFar*alphaToSet);
				m_currentNearColor.setAlpha(m_alphaOriginalNear*alphaToSet);
				alphaFinished = btrue;
			}
			else
			{
				alphaToSet = Interpolate(m_alphaSrc,m_alphaDst,t);
				m_currentFarColor.setAlpha(m_alphaOriginalFar*alphaToSet);
				m_currentNearColor.setAlpha(m_alphaOriginalNear*alphaToSet);
				alphaFinished = bfalse;
			}
		}
		else
			alphaFinished = btrue;

		// If both are finished, check if we need to pause or destroy.
		if(alphaFinished && colorFinished)
		{
			if (m_pauseOnFade)
				m_actor->disable();
			else if (m_destroyOnFade)
				m_actor->requestDestruction();
		}
	}

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(FogBoxComponent_Template)
    BEGIN_SERIALIZATION_CHILD(FogBoxComponent_Template)
    END_SERIALIZATION();

    FogBoxComponent_Template::FogBoxComponent_Template()
    {
    }

    bbool FogBoxComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);
        return bOk;
    }

    void FogBoxComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        Super::onTemplateDelete(_hotReload);
    }

} // namespace ITF


