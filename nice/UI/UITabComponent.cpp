#include "precompiled_gameplay.h"

#ifndef _ITF_UITABCOMPONENT_H_
#include "gameplay/components/UI/UITabComponent.h"
#endif //_ITF_UITABCOMPONENT_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_RENDERBOXCOMPONENT_H_
#include "gameplay/Components/UI/RenderBoxComponent.h"
#endif //_ITF_RENDERBOXCOMPONENT_H_

#ifndef _ITF_ANIMMESHVERTEXCOMPONENT_H_
#include "engine/actors/components/AnimMeshVertexComponent.h"
#endif //_ITF_ANIMMESHVERTEXCOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(UITabComponent)
    BEGIN_SERIALIZATION_CHILD(UITabComponent)
    END_SERIALIZATION()

    UITabComponent::UITabComponent() 
        : m_useCustomRenderBoxColor(bfalse)
        , m_customRenderBoxColor(Color::white())
    {
    }

    UITabComponent::~UITabComponent()
    {

    }

    void UITabComponent::enableCustomRenderBoxColor(Color _color)
    {
        m_useCustomRenderBoxColor = btrue;
        m_customRenderBoxColor = _color;

        if(m_animComponent)
            m_animComponent->getGfxPrimitiveParam().m_colorFactor = m_customRenderBoxColor;

        if(RenderBoxComponent* boxComponent = m_actor->GetComponent<RenderBoxComponent>())
            boxComponent->setColor(m_customRenderBoxColor, getTemplate()->m_colorBlendTime);
    }

    void UITabComponent::disableCustomRenderBoxColor()
    {
        m_useCustomRenderBoxColor = bfalse;

        if(hasUIState(UI_STATE_IsSelected))
        {
            if(m_animComponent)
            {
                if(hasUIState(UI_STATE_IsLocked))
                    m_animComponent->getGfxPrimitiveParam().m_colorFactor = getLockColor();
                else
                    m_animComponent->getGfxPrimitiveParam().m_colorFactor = getSelectColor();
            }

            if(RenderBoxComponent* boxComponent = m_actor->GetComponent<RenderBoxComponent>())
                boxComponent->setColor(getSelectColor(), getTemplate()->m_colorBlendTime);
        }
        else
        {
            if(m_animComponent)
            {
                if(hasUIState(UI_STATE_IsLocked))
                    m_animComponent->getGfxPrimitiveParam().m_colorFactor = getLockColor();
                else
                    m_animComponent->getGfxPrimitiveParam().m_colorFactor = getUnselectColor();
            }

            if(RenderBoxComponent* boxComponent = m_actor->GetComponent<RenderBoxComponent>())
                boxComponent->setColor(getUnselectColor(), getTemplate()->m_colorBlendTime);
        }
    }

    const Color& UITabComponent::getSelectColor() const
    {
        return m_useCustomRenderBoxColor ? m_customRenderBoxColor : getTemplate()->m_colorFactorSelected;
    }

    const Color& UITabComponent::getUnselectColor() const
    {
        return m_useCustomRenderBoxColor ? m_customRenderBoxColor : getTemplate()->m_colorFactorUnselected;
    }

    const Color& UITabComponent::getLockColor() const
    {
        return m_useCustomRenderBoxColor ? m_customRenderBoxColor : getTemplate()->m_colorFactorLocked;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(UITabComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UITabComponent_Template)
    END_SERIALIZATION()


    UITabComponent_Template::UITabComponent_Template()
    { 
    }

    UITabComponent_Template::~UITabComponent_Template()
    {    
    }

}
