#ifndef _ITF_TEXTBOXATTACHEDTOBONECOMPONENT_H_
#define _ITF_TEXTBOXATTACHEDTOBONECOMPONENT_H_

namespace ITF
{
#define TextBoxAttachedToBoneComponent_CRC ITF_GET_STRINGID_CRC(TextBoxAttachedToBoneComponent,3426587754)
#define TextBoxAttachedToBoneComponent_Template_CRC ITF_GET_STRINGID_CRC(TextBoxAttachedToBoneComponent_Template,1355517470)

    class TextBoxAttachedToBoneComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TextBoxAttachedToBoneComponent,ActorComponent,TextBoxAttachedToBoneComponent_CRC);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        TextBoxAttachedToBoneComponent();
        virtual ~TextBoxAttachedToBoneComponent();

        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const             { return bfalse; }
        virtual     bbool       needsDraw2D         (       ) const             { return bfalse; }
		virtual		bbool		needsDraw2DNoScreenRatio(	) const				{ return bfalse; }

        virtual void    onActorLoaded( Pickable::HotReloadType /*_hotReload*/ );
        virtual void    onResourceReady();

        virtual void    Update( f32 _dt );
        
   protected:
        ITF_INLINE const class TextBoxAttachedToBoneComponent_Template*  getTemplate() const;

    private:

        class AnimLightComponent*       m_animComponent;
        class TextBoxComponent*         m_textComponent;
        class UITextBox*                m_uiTextComponent;
        u32                             m_boneIndex;
        bbool                           m_boneValid;
    };

    //---------------------------------------------------------------------------------------------------

    class TextBoxAttachedToBoneComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TextBoxAttachedToBoneComponent_Template,ActorComponent_Template,TextBoxAttachedToBoneComponent_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(TextBoxAttachedToBoneComponent);

    public:

        TextBoxAttachedToBoneComponent_Template();
        ~TextBoxAttachedToBoneComponent_Template() {}

        ITF_INLINE const StringID&          getBoneName()       const { return m_boneName; }
        ITF_INLINE bbool                    getUseBoneScale()   const { return m_useBoneScale; }
        ITF_INLINE bbool                    getUseBoneAngle()   const { return m_useBoneAngle; }
        ITF_INLINE bbool                    getUseBoneAlpha()   const { return m_useBoneAlpha; }


    private:

        StringID        m_boneName;
        bbool           m_useBoneScale;
        bbool           m_useBoneAngle;
        bbool           m_useBoneAlpha;

    };


    ITF_INLINE const TextBoxAttachedToBoneComponent_Template*  TextBoxAttachedToBoneComponent::getTemplate() const {return static_cast<const TextBoxAttachedToBoneComponent_Template*>(m_template);}
}
#endif // _ITF_TEXTBOXATTACHEDTOBONECOMPONENT_H_
