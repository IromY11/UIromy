#include "precompiled_gameplay.h"

#ifndef _ITF_SIMPLETEXTCOMPONENT_H_
#include "gameplay/components/UI/SimpleTextComponent.h"
#endif //_ITF_SIMPLETEXTCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(SimpleTextComponent)

BEGIN_SERIALIZATION_CHILD(SimpleTextComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("TextLabel", m_text);
        SERIALIZE_MEMBER("lineId", m_lineId);
        SERIALIZE_MEMBER("drawUsingEngine", m_drawUsingEngine);
        SERIALIZE_MEMBER("drawBoxWidth", m_drawBoxWidth);
        SERIALIZE_MEMBER("drawBoxHeight", m_drawBoxHeight);
        SERIALIZE_MEMBER("textSize", m_textSize);
        SERIALIZE_MEMBER("textColor", m_textColor);
        SERIALIZE_MEMBER("backgroundColor", m_backgroundColor);
        SERIALIZE_MEMBER("centerText", m_center);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

SimpleTextComponent::SimpleTextComponent()
: Super()
, m_drawUsingEngine(btrue)
, m_drawBoxWidth(200.f)
, m_drawBoxHeight(100.f)
, m_textBox(NULL)
, m_textSize(25.0f)
, m_center(btrue)
, m_display(btrue)
, m_needUpdate(btrue)
{
    m_backgroundColor = Color(0.0f,0.0f,0.0f,0.0f);
    m_textColor = COLOR_BLACK;
}


SimpleTextComponent::~SimpleTextComponent()
{
}

void SimpleTextComponent::onBecomeActive()
{
    m_textBox =  m_actor->GetComponent<UITextBox>();
    m_needUpdate = btrue;
}

void SimpleTextComponent::Update(f32 _dt)
{
    if (m_textBox && m_needUpdate)
    {
        updateText();
        m_needUpdate = bfalse;
    }
}

void SimpleTextComponent::updateText()
{
    if (m_textBox)
    {
        TEXT_AREA_ANCHOR anchor = AREA_ANCHOR_TOP_LEFT;
        if(m_center)
            anchor = AREA_ANCHOR_MIDDLE_CENTER;

        m_textBox->setFont(m_textColor, m_textSize);
        m_textBox->setBox(Vec2d(m_drawBoxWidth * m_actor->getScale().x(), m_drawBoxHeight* m_actor->getScale().y()), anchor);

        if(m_lineId!=LocalisationId::Invalid)
        {
            m_textBox->setLoc(m_lineId);
        }
        else
        {
            m_textBox->setText(m_text);
        }
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(SimpleTextComponent_Template)
BEGIN_SERIALIZATION_CHILD(SimpleTextComponent_Template)
END_SERIALIZATION()

}

