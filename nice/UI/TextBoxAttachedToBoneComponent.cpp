#include "precompiled_gameplay.h"

#ifndef _ITF_TEXTBOXATTACHEDTOBONECOMPONENT_H_
#include "gameplay/components/UI/TextBoxAttachedToBoneComponent.h"
#endif //_ITF_TEXTBOXATTACHEDTOBONECOMPONENT_H_

#ifndef _ITF_TEXTBOXCOMPONENT_H_
#include "gameplay/components/UI/TextBoxComponent.h"
#endif //_ITF_TEXTBOXCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TextBoxAttachedToBoneComponent)
    BEGIN_SERIALIZATION_CHILD(TextBoxAttachedToBoneComponent)
    END_SERIALIZATION()


    ///////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_VALIDATE_COMPONENT(TextBoxAttachedToBoneComponent)
        VALIDATE_COMPONENT_PARAM("animComponent", m_animComponent, "an anim component is mandatory");
        VALIDATE_COMPONENT_PARAM("", getTemplate()->getBoneName().isValid(), "bone name not set correctly in template");
    END_VALIDATE_COMPONENT()

    ///////////////////////////////////////////////////////////////////////////////////////////

    TextBoxAttachedToBoneComponent::TextBoxAttachedToBoneComponent()
    : m_boneIndex(U32_INVALID)
    , m_animComponent(NULL)
    , m_boneValid(bfalse)
    , m_textComponent(NULL)
    , m_uiTextComponent(NULL)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    TextBoxAttachedToBoneComponent::~TextBoxAttachedToBoneComponent()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxAttachedToBoneComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_textComponent = m_actor->GetComponent<TextBoxComponent>();
        m_uiTextComponent = m_actor->GetComponent<UITextBox>();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxAttachedToBoneComponent::onResourceReady()
    {
        Super::onResourceReady();

        m_boneIndex = m_animComponent->getBoneIndex(getTemplate()->getBoneName());
        m_boneValid = bfalse;

        ITF_WARNING_CATEGORY(GPP,m_actor,m_boneIndex != U32_INVALID,"Unknown bone : %s",getTemplate()->getBoneName().getDebugString());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxAttachedToBoneComponent::Update( f32 _dt )
    {
        if ( m_boneIndex != U32_INVALID ) 
        {
            if (m_boneValid)
            {
                Vec2d pos;
                if ( m_animComponent->getBonePos(m_boneIndex, pos) )
                {
                    if(m_textComponent)
                    {
                        Vec2d offset = pos - m_actor->get2DPos();
                        // offset is actor relative
                        m_actor->transformWorldVectorToLocal(offset);

                        m_textComponent->setMainOffset(offset);

                        Vec2d scale;
                        if( getTemplate()->getUseBoneScale() && m_animComponent->getBoneScale(m_boneIndex, scale))
                            m_textComponent->setMainScale(scale);

                        f32 angle;
                        if ( getTemplate()->getUseBoneAngle()  && m_animComponent->getBoneAngle(m_boneIndex, angle))
                            m_textComponent->setMainAngle(angle);

                        f32 alpha;
                        if ( getTemplate()->getUseBoneAlpha() &&  m_animComponent->getBoneAlpha(m_boneIndex, alpha))
                            m_textComponent->setAlpha(alpha);
                    }
                    else if(m_uiTextComponent)
                    {
                        Vec2d offset = pos * Vec2d(g_pixelBy2DUnit, -g_pixelBy2DUnit) * m_animComponent->getScale();
                        m_actor->transformWorldVectorToLocal(offset);

                        m_uiTextComponent->setMainOffset(offset);
                        
                        Vec2d scale;
                        if( getTemplate()->getUseBoneScale() && m_animComponent->getBoneScale(m_boneIndex, scale))
                            m_uiTextComponent->setMainScale(scale);

                        //f32 angle;
                        //if ( getTemplate()->getUseBoneAngle()  && m_animComponent->getBoneAngle(m_boneIndex, angle))
                        //    m_uiTextComponent->setMainAngle(angle);

                        f32 alpha;
                        if ( getTemplate()->getUseBoneAlpha() &&  m_animComponent->getBoneAlpha(m_boneIndex, alpha))
                            m_uiTextComponent->setAlpha(alpha);
                    }
                }
            }
            m_boneValid = m_animComponent->isBoneDataOk(m_boneIndex);
        }

        Super::Update(_dt);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TextBoxAttachedToBoneComponent_Template)
    BEGIN_SERIALIZATION_CHILD(TextBoxAttachedToBoneComponent_Template)
        SERIALIZE_MEMBER("boneName"         , m_boneName);
        SERIALIZE_MEMBER("useBoneScale"     , m_useBoneScale);
        SERIALIZE_MEMBER("useBoneAngle"     , m_useBoneAngle);
        SERIALIZE_MEMBER("useBoneAlpha"     , m_useBoneAlpha);
    END_SERIALIZATION()

    TextBoxAttachedToBoneComponent_Template::TextBoxAttachedToBoneComponent_Template()
    : m_useBoneScale(bfalse)
    , m_useBoneAngle(bfalse)
    , m_useBoneAlpha(bfalse)
    {}
}
