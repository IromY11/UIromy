
#include "precompiled_gameplay.h"

#ifndef _ITF_UIITEMBASIC_H_
#include "gameplay/components/UI/UIItemBasic.h"
#endif //_ITF_UIITEMBASIC_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_RENDERBOXCOMPONENT_H_
#include "gameplay/Components/UI/RenderBoxComponent.h"
#endif //_ITF_RENDERBOXCOMPONENT_H_

#ifndef _ITF_ANIMMESHVERTEXCOMPONENT_H_
#include "engine/actors/components/AnimMeshVertexComponent.h"
#endif //_ITF_ANIMMESHVERTEXCOMPONENT_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#include "engine/actors/managers/UIMenuManager.h"

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(UIItemBasic)
    BEGIN_SERIALIZATION_CHILD(UIItemBasic)
        SERIALIZE_MEMBER("selectTextStyle", m_selectTextStyle);
        SERIALIZE_MEMBER("selectAnimMeshVertex", m_selectAnimMeshVertex);
		SERIALIZE_MEMBER("PathForMap", m_levelPath);
		SERIALIZE_OBJECT("WwiseOnValidate",m_soundOnValidate);
		SERIALIZE_OBJECT("WwiseMoreOnValidate",m_soundMoreOnValidate);
    END_SERIALIZATION()

    UIItemBasic::UIItemBasic() 
    : m_selectTextStyle(1)
    , m_animComponent(NULL)
    , m_boxComponent(NULL)
    , m_textComponent(NULL)
    , m_animMeshComponent(NULL)
    , m_shadowOffset(Vec3d::Zero)
    , m_needRefreshAnimMeshVertex(bfalse)
    , m_useLockedItemTextAlpha(btrue)
	{
    }

    
    UIItemBasic::~UIItemBasic()
    {    
    }

	StringID UIItemBasic::getAnimSelected()
	{
		return getTemplate()->m_animSelected;
	}
	StringID UIItemBasic::getAnimUnselected()
	{
		return getTemplate()->m_animUnselected;
	}
	StringID UIItemBasic::getAnimLocked()
	{
		return getTemplate()->m_animLocked;
	}

    void UIItemBasic::onActorLoaded(Pickable::HotReloadType _type)
    {
        Super::onActorLoaded(_type);

		m_soundOnValidate.init(m_actor, bfalse);
		m_soundMoreOnValidate.init(m_actor, bfalse);
        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_boxComponent = m_actor->GetComponent<RenderBoxComponent>();
        m_textComponent = m_actor->GetComponent<UITextBox>();
        m_animMeshComponent = m_actor->GetComponent<AnimMeshVertexComponent>();

        m_blinkPulse.setup(1.0f, getTemplate()->m_selectBlinkScaleMin, getTemplate()->m_selectBlinkScale, getTemplate()->m_selectBlinkPeriod, getTemplate()->m_selectBlinkMinBlendSpeed);
        m_blinkPulse.reset();

        m_highlightPulse.setup(0.0f, getTemplate()->m_selectHightlightAlphaMin, getTemplate()->m_selectHightlightAlpha, getTemplate()->m_selectHightlightPeriod, getTemplate()->m_selectHightlightMinBlendSpeed);
        m_highlightPulse.reset();

		if (  getTemplate()->m_activatingDuration)
		{
			m_activateShake.setup(1.0f, getTemplate()->m_activatingScale, getTemplate()->m_activatingDuration, getTemplate()->m_activatingRebound);
			m_activateShake.reset();
		}
    }

    void UIItemBasic::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        if(m_boxComponent)
        {
            m_shadowOffset = m_boxComponent->getShadowOffset();
        }

        if(m_textComponent && !UI_MENUMANAGER->useRemoteUI())
        {
            m_textComponent->useActorIcon(getTemplate()->m_iconNameUnselected, btrue);
            m_textComponent->useActorIcon(getTemplate()->m_iconNameSelected, btrue);
            m_textComponent->setActorIcon(getTemplate()->m_iconNameUnselected);
        }

        setUIState(UI_STATE_IsSelected, !hasUIState(UI_STATE_IsSelected));
        onSelected(!hasUIState(UI_STATE_IsSelected));

        setUIState(UI_STATE_IsDown, !hasUIState(UI_STATE_IsDown));
        onDown(!hasUIState(UI_STATE_IsDown));

        setUIState(UI_STATE_IsLocked, !hasUIState(UI_STATE_IsLocked));
        onLocked(!hasUIState(UI_STATE_IsLocked));
    }

    void UIItemBasic::onStartDestroy( bbool _hotReload )
{
        Super::onStartDestroy(_hotReload);

    }

    bbool UIItemBasic::onSelected(const bbool _isSelected)
    {
        if(Super::onSelected(_isSelected))
        {
            if(_isSelected)
            {
                if(m_animComponent)
                {
                    setAnim(getAnimSelected());
                }

                if(RenderBoxComponent* boxComponent = m_actor->GetComponent<RenderBoxComponent>())
                {
                    boxComponent->seUVtRotationSpeed(getTemplate()->m_uvRotationSpeedSelected);
                }

                if(m_textComponent)
                {
                    m_textComponent->setEffect(getTemplate()->m_fontEffectSelected, btrue);
                    m_textComponent->setActorIcon(getTemplate()->m_iconNameSelected);
                }

				if(getTemplate()->m_soundOnChangeSelection)
				{
					UI_MENUMANAGER->playSound(getTemplate()->m_soundOnChangeSelection,getTemplate()->m_soundOnChangeSelection,m_actor);
				}

                m_needRefreshAnimMeshVertex = btrue;
            }
            else
            {
                if(m_animComponent)
                {
                    setAnim(getAnimUnselected());
                }

                if(RenderBoxComponent* boxComponent = m_actor->GetComponent<RenderBoxComponent>())
                {
                    boxComponent->seUVtRotationSpeed(getTemplate()->m_uvRotationSpeedUnselected);
                }

                if(m_textComponent)
                {
                    m_textComponent->setEffect(getTemplate()->m_fontEffectUnselected, btrue);
                    m_textComponent->setActorIcon(getTemplate()->m_iconNameUnselected);
                }

                m_needRefreshAnimMeshVertex = btrue;
            }
            
            return btrue;
        }

        return bfalse;
    }

    bbool UIItemBasic::onLocked(const bbool _isLocked)
    {
		if(m_animComponent)
        {
			if(_isLocked)
				setAnim(getAnimLocked());
			else
				setAnim(getAnimUnselected());

        }
        return Super::onLocked(_isLocked);        
    }

    void UIItemBasic::setUseLockedItemTextAlpha(bbool _used)
    {
        if(m_useLockedItemTextAlpha!=_used)
        {
            m_useLockedItemTextAlpha = _used;
        }
    }

    bbool UIItemBasic::onDown(const bbool _isDown)
    {
        if(Super::onDown(_isDown))
        {
            if(_isDown)
            {
                if(m_textComponent)
                {
                    m_textComponent->setStyle(m_selectTextStyle);
                }

                if(m_boxComponent && m_boxComponent->getUseShadow())
                {
                    if(getTemplate()->m_inverseShadowState)
                    {
                        Vec2d newPos = getAbsolutePosition();
                        newPos -= m_shadowOffset.truncateTo2D() * m_actor->getScale();
                        setAbsolutePosition(newPos, bfalse);
                        m_boxComponent->setShadowOffset(m_shadowOffset);
                    }
                    else
                    {
                        Vec2d newPos = getAbsolutePosition();
                        newPos += m_shadowOffset.truncateTo2D() * m_actor->getScale();
                        setAbsolutePosition(newPos, bfalse);
                        m_boxComponent->setShadowOffset(Vec3d::Zero);
                    }

                }
                
            }
            else
            {
                if(m_textComponent)
                {
                    m_textComponent->setStyle(U32_INVALID);
                }

                if(m_boxComponent && m_boxComponent->getUseShadow())
                {
                    if(getTemplate()->m_inverseShadowState)
                    {
                        Vec2d newPos = getAbsolutePosition();
                        newPos += m_shadowOffset.truncateTo2D() * m_actor->getScale();
                        setAbsolutePosition(newPos, bfalse);
                        m_boxComponent->setShadowOffset(Vec3d::Zero);
                    }
                    else
                    {
                        Vec2d newPos = getAbsolutePosition();
                        newPos -= m_shadowOffset.truncateTo2D() * m_actor->getScale();
                        setAbsolutePosition(newPos, bfalse);
                        m_boxComponent->setShadowOffset(m_shadowOffset);
                    }
                }
            }

            return btrue;
        }

        return bfalse;
    }

    bbool UIItemBasic::onActivating(const bbool _isActivate, const i32 _controllerID)
    {
        if(Super::onActivating(_isActivate, _controllerID))
        {
			if (  getTemplate()->m_activatingDuration)
				m_activateShake.reset();

            return btrue;
        }

        return bfalse;
    }


	void UIItemBasic::stopBlink()
	{
		m_blinkPulse.reset();
	}

    bbool UIItemBasic::onValidate(const bbool _isValidate, const i32 _controllerID)
    {
        if(_isValidate)
        {
            if ( hasUIState(UI_STATE_IsLocked))
                UI_MENUMANAGER->playSound(getTemplate()->m_soundOnSelectedLocked,getTemplate()->m_soundOnSelectedLocked,m_actor);
            else
            {
                UI_MENUMANAGER->playSound(getTemplate()->m_soundOnSelected,getTemplate()->m_soundOnSelected,m_actor);
                m_soundOnValidate.sendEvent();	
                m_soundMoreOnValidate.sendEvent();	
            }
        }
        
        return  UIItem::onValidate(_isValidate, _controllerID);  
    }

    bbool UIItemBasic::onIsActivating(f32 _dt)
    {
        bbool isActivating = Super::onIsActivating(_dt);

		if (  getTemplate()->m_activatingDuration)
			isActivating |= m_activateShake.update(_dt);		

		if (  getTemplate()->m_activatingDuration)
		{
			if ( getTemplate()->m_needSyncBlink)
			{
				Vec2d scale = getBaseScale() * m_activateShake.getValue();
				m_actor->setScale(scale);
			}
			else
			{
				Vec2d scale = m_actor->getScale();
				scale *=  m_activateShake.getValue();
				m_actor->setScale(scale);
			}
		}


        return isActivating;
    }


    void UIItemBasic::setAnim(StringID _anim)
    {
        if(m_animComponent && _anim.isValid())
        {
            m_animComponent->setAnim(_anim);
        }
    }

    void UIItemBasic::Update(f32 _dt)
    {
		Super::Update(_dt);

        // Blink pulse
        bbool needSyncBlink = bfalse;
        if(hasUIState(UI_STATE_IsSelected))
        {
            if(!hasUIState(UI_STATE_IsActivating))
            {
                needSyncBlink = m_blinkPulse.update(bfalse, _dt);
            }
        }
        else
        {
            needSyncBlink = m_blinkPulse.update(btrue, _dt);
        }

        if(needSyncBlink && getTemplate()->m_needSyncBlink)
        {
            Vec2d scale = getBaseScale();
			if( ! hasUIState(UI_STATE_IsSelected))
				scale *= Vec2d( getTemplate()->m_scaleUnselected, getTemplate()->m_scaleUnselected);
            scale *= m_blinkPulse.getValue();
            m_actor->setScale(scale);
        }


        // Highlight pulse
        if(m_boxComponent && m_boxComponent->getUseHighlight())
        {
            bbool needSyncHighlight = bfalse;
            if(hasUIState(UI_STATE_IsSelected))
            {
                if(!hasUIState(UI_STATE_IsActivating))
                {
                    needSyncHighlight = m_highlightPulse.update(bfalse, _dt);
                }
            }
            else
            {
                needSyncHighlight = m_highlightPulse.update(btrue, _dt);
            }

            if(needSyncHighlight)
            {
                f32 alpha = m_highlightPulse.getValue();
                m_boxComponent->setHighlightAlpha(alpha);
            }
        }
        
        Color color = getUnselectColor();
        // Locked anim
        if(hasUIState(UI_STATE_IsSelected))
            color = getSelectColor();
        else if(hasUIState(UI_STATE_IsLocked))
        {
            if(AnimMeshVertexComponent* boxComponent = m_actor->GetComponent<AnimMeshVertexComponent>())
            {
                for(u32 n=0; n<boxComponent->getAMVList().size(); ++n)
                {
                    boxComponent->setAMVFrame(n, 1);
                }
            }

            color = getLockColor();			
        }

        if(m_animComponent)
        {
            m_animComponent->getGfxPrimitiveParam().m_colorFactor = color;            
        }

        if(RenderBoxComponent* boxComponent = m_actor->GetComponent<RenderBoxComponent>())
        {
            if (getTemplate()->m_colorBlendTime > 0.f)
                boxComponent->setColor(color, getTemplate()->m_colorBlendTime);
        }

        // Update alpha if locked
		if(m_useLockedItemTextAlpha)
        {
            bbool useAlpha = hasUIState(UI_STATE_IsLocked) && !hasUIState(UI_STATE_LockedButSelectable) && m_useLockedItemTextAlpha;
            f32 textAlpha = useAlpha ? getTemplate()->m_textLockedAlpha : 1.0f;
            f32 animAlpha = useAlpha ? getTemplate()->m_animLockedAlpha : 1.0f;

            if(m_textComponent)
                m_textComponent->setAlpha(textAlpha);

            if(m_animComponent)
                m_animComponent->setAlpha(animAlpha);

            if(RenderBoxComponent* boxComponent = m_actor->GetComponent<RenderBoxComponent>())
                boxComponent->setAlpha(animAlpha);

            if(AnimMeshVertexComponent* amvComponent = m_actor->GetComponent<AnimMeshVertexComponent>())
                amvComponent->setAlpha(animAlpha);

            if(TextureGraphicComponent* textureComponent = m_actor->GetComponent<TextureGraphicComponent>())
                textureComponent->setAlpha(animAlpha);
        }

        // Anim mesh state
        if(m_needRefreshAnimMeshVertex && m_animMeshComponent && m_actor->isPhysicalReady())
        {
            u32 animListSize = m_animMeshComponent->getAnimListSize();

            if(hasUIState(UI_STATE_IsSelected))
            {
                for(u32 n=0; n<m_selectAnimMeshVertex.size() && n<animListSize; ++n)
                {
                    if(m_selectAnimMeshVertex[n].isValid())
                    {
                        m_animMeshComponent->setAMVAnimByName(n, m_selectAnimMeshVertex[n]);
                    }
                }
            }
            else
            {
                for(u32 n=0; n<m_selectAnimMeshVertex.size() && n<animListSize; ++n)
                {
                    if(m_selectAnimMeshVertex[n].isValid())
                    {
                        m_animMeshComponent->setAMVAnimByName(n, StringID::InvalidId);
                    }
                }
            }

            m_needRefreshAnimMeshVertex = bfalse;
        }
    }

    const Color& UIItemBasic::getSelectColor() const
    {
        return getTemplate()->m_colorFactorSelected;
    }

    const Color& UIItemBasic::getUnselectColor() const
    {
        return getTemplate()->m_colorFactorUnselected;
    }

    const Color& UIItemBasic::getLockColor() const
    {
        return getTemplate()->m_colorFactorLocked;
    }   

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(UIItemBasic_Template)
    BEGIN_SERIALIZATION_CHILD(UIItemBasic_Template)
        SERIALIZE_MEMBER("blinkScaleMin", m_selectBlinkScaleMin);
        SERIALIZE_MEMBER("blinkScale", m_selectBlinkScale);
        SERIALIZE_MEMBER("blinkPeriod", m_selectBlinkPeriod);
        SERIALIZE_MEMBER("blinkMinBlendSpeed", m_selectBlinkMinBlendSpeed);
        SERIALIZE_MEMBER("hightlightAlphaMin", m_selectHightlightAlphaMin);
        SERIALIZE_MEMBER("hightlightAlpha", m_selectHightlightAlpha);
        SERIALIZE_MEMBER("hightlightPeriod", m_selectHightlightPeriod);
        SERIALIZE_MEMBER("hightlightMinBlendSpeed", m_selectHightlightMinBlendSpeed);
        SERIALIZE_MEMBER("activatingScale", m_activatingScale);
        SERIALIZE_MEMBER("activatingDuration", m_activatingDuration);
        SERIALIZE_MEMBER("activatingRebound", m_activatingRebound);
        SERIALIZE_MEMBER("animUnselected", m_animUnselected);
		SERIALIZE_MEMBER("animSelected", m_animSelected);
		SERIALIZE_MEMBER("animLocked", m_animLocked);
		SERIALIZE_MEMBER("needSyncBlink", m_needSyncBlink);
        SERIALIZE_MEMBER("fontEffectUnselected", m_fontEffectUnselected);
        SERIALIZE_MEMBER("fontEffectSelected", m_fontEffectSelected);
        SERIALIZE_MEMBER("colorFactorSelected", m_colorFactorSelected);
        SERIALIZE_MEMBER("colorFactorLocked", m_colorFactorLocked);
        SERIALIZE_MEMBER("colorFactorUnselected", m_colorFactorUnselected);
        SERIALIZE_MEMBER("textLockedAlpha", m_textLockedAlpha);
        SERIALIZE_MEMBER("animLockedAlpha", m_animLockedAlpha);
        SERIALIZE_MEMBER("colorBlendTime", m_colorBlendTime);
		SERIALIZE_MEMBER("ScaleUnselected", m_scaleUnselected);
        SERIALIZE_MEMBER("uvRotationSpeedSelected", m_uvRotationSpeedSelected);
        SERIALIZE_MEMBER("uvRotationSpeedUnselected", m_uvRotationSpeedUnselected);
        SERIALIZE_MEMBER("actorIconSelected", m_iconNameSelected);
        SERIALIZE_MEMBER("actorIconUnselected", m_iconNameUnselected);
        SERIALIZE_MEMBER("inverseShadowState", m_inverseShadowState);
		SERIALIZE_MEMBER("WwisOnSelectedSound",m_soundOnSelected);
		SERIALIZE_MEMBER("WwisOnChangeSelection", m_soundOnChangeSelection)
		SERIALIZE_MEMBER("WwisOnSelectedLocked", m_soundOnSelectedLocked)
			
    END_SERIALIZATION()


    UIItemBasic_Template::UIItemBasic_Template()
    : m_selectBlinkScale(1.2f)
    , m_selectBlinkScaleMin(1.0f)
    , m_selectBlinkPeriod(0.6f)
    , m_selectBlinkMinBlendSpeed(F32_INFINITY)
    , m_selectHightlightAlphaMin(0.0f)
    , m_selectHightlightAlpha(0.3f)
    , m_selectHightlightPeriod(0.6f)
    , m_selectHightlightMinBlendSpeed(F32_INFINITY)
    , m_colorFactorSelected(Color::white())
    , m_colorFactorUnselected(Color::white())
	, m_colorFactorLocked(Color::white())
    , m_textLockedAlpha(0.4f)
    , m_animLockedAlpha(0.4f)
	, m_scaleUnselected(1.0f)
	, m_activatingScale(1.1f)
#ifdef ITF_W1W_MOBILE
    , m_activatingDuration(0.15f)
#else
    , m_activatingDuration(0.5f)
#endif //ITF_W1W_MOBILE
    , m_activatingRebound(4)
    , m_uvRotationSpeedSelected()
    , m_uvRotationSpeedUnselected()
    , m_colorBlendTime(0.f)
    , m_inverseShadowState(bfalse)
	, m_needSyncBlink(true)
	, m_soundOnSelected(ITF_GET_STRINGID_CRC( Play_Menu_Butt_Validate_Gen, 1191456473))
	, m_soundOnChangeSelection(ITF_GET_STRINGID_CRC( Play_Menu_Butt_Validate_Gen, 1191456473))
	, m_soundOnSelectedLocked(ITF_GET_STRINGID_CRC( Play_Menu_Butt_Validate_Locked, 3196175437))
    {  
    }

    
    UIItemBasic_Template::~UIItemBasic_Template()
    {    
    }

}
