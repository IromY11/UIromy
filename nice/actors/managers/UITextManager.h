#ifndef _ITF_UITEXTMANAGER_H_
#define _ITF_UITEXTMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_EVENTLISTENER_H_
#include "engine/events/IEventListener.h"
#endif //_ITF_EVENTLISTENER_H_

#ifndef _ITF_FONTTEXT_H_
# include "engine/display/FontText.h"
#endif //_ITF_FONTTEXT_H_

namespace ITF
{
    class UITextManager_Template;
    class FontEffectList_Template;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class UITextManager_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(UITextManager_Template, TemplateObj,2855701572);
        DECLARE_SERIALIZE()

    public:
        UITextManager_Template() {}

        struct ActorIcon
        {
            DECLARE_SERIALIZE()

            ActorIcon() : m_fontSize(0.0f){}

            StringID m_iconName;
            Path m_iconPath;
            f32 m_fontSize;
        };

        const ActorIcon* getActorIcon(StringID _iconName) const;
        void setActorIcon(StringID _iconName, const UITextManager_Template::ActorIcon &_actorIcon);
        ITF_VECTOR<ActorIcon>          m_actorIcons;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class UITextManager : public IEventListener
    {
    public:

        UITextManager();
        ~UITextManager();

        void                        init                (       );
        void                        onEvent             (Event* _event);

        const UITextManager_Template::ActorIcon* getActorIcon(StringID _iconName) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Add a variable to the table of variables
        /// @param _varFriendly friendly of the variable
        /// @param _varValue value of the variable
        void            addVariable (const StringID &_varFriendly, const String8 &_varValue);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Remove a variable of the table of variables
        /// @param _varFriendly friendly of the variable
        void            removeVariable (const StringID &_varFriendly);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Update the variable in the table of variables
        /// @param _varFriendly friendly of the variable
        /// @param _newValue new value of the variable
        void            updateVariable (const StringID &_varFriendly, const String8 &_newValue);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Get the value of the variable
        /// @param _varFriendly friendly of the variable
        const String8&   getVariableValue (const StringID &_varFriendly) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Get font effect
        const class FontEffect_Template *getFontEffect( u32 _index ) const;
        const class FontEffect_Template *getFontEffect( StringID _name ) const;


    private:
        
        typedef ITF_MAP<StringID,String8>    TextVariable;

        // Template
        UITextManager_Template*     m_template;
        Path                        m_configPath;

        // Font Effects
        FontEffectList_Template *   m_fontEffects;
        Path                        m_fontEffectsPath;

        // Variables
        TextVariable                m_variables;

    };





    //------------------------------------------------------------------------------
    /// Collection of font effect preset
    //
    class FontEffectList_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(FontEffectList_Template, TemplateObj,1382126731);
        DECLARE_SERIALIZE()

    public:
        FontEffectList_Template() : Super() {}
        ITF_INLINE const ITF_VECTOR<FontEffect_Template>& getEffects() const { return m_effects; }

        const FontEffect_Template *getByIndex(u32 _index) const;
        const FontEffect_Template *get(StringID _name) const;

    private:
        ITF_VECTOR<FontEffect_Template> m_effects;
    };

}
#endif // _ITF_UITEXTMANAGER_H_
