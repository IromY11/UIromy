#include "precompiled_engine.h"

#include "gameplay/Components/UI/UIMenu.h"

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    #define UITEXTCONFIG_PATH GETPATH_ALIAS("uitextconfig")
    

    ///////////////////////////////////////////////////////////////////////////////////////////
    UITextManager::UITextManager ()
    : m_template(NULL), m_fontEffects(NULL)
    {    
        ITF_ASSERT(EVENTMANAGER);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(EventSingletonConfigChanged_CRC,this);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UITextManager::~UITextManager ()
    {       
        if (EVENTMANAGER)
        {
            EVENTMANAGER->unregisterEvent(EventSingletonConfigChanged_CRC, this);
        }
        
        if (TEMPLATEDATABASE && !m_configPath.isEmpty())
        {
            TEMPLATEDATABASE->releaseTemplate(m_configPath);
        }
        
        if (TEMPLATEDATABASE && !m_fontEffectsPath.isEmpty())
        {
            TEMPLATEDATABASE->releaseTemplate(m_fontEffectsPath);
        }
    }

    void UITextManager::init()
    {
        // load config file
        m_configPath = UITEXTCONFIG_PATH;

        if ( !m_configPath.isEmpty() )
        {
            const UITextManager_Template* config = TEMPLATEDATABASE->requestTemplate<UITextManager_Template>(m_configPath);
            if (!config)
            {
                ITF_FATAL_ERROR("Couldn't load UI text config: %s", m_configPath.toString8().cStr());
            }
            else
            {
                // store template
                m_template = const_cast<UITextManager_Template*>(config);
                if (SYSTEM_ADAPTER->isBackAndSelectButtonsInverted())
                {
                    UITextManager_Template::ActorIcon tempActorIcon;
                    const UITextManager_Template::ActorIcon *tempActorIconPtr = m_template->getActorIcon(input_actionID_Valid);
                    const UITextManager_Template::ActorIcon *tempActorIconPtr2 = m_template->getActorIcon(input_actionID_Back);
                    if (tempActorIconPtr != NULL && tempActorIconPtr2 != NULL)
                    {
                        tempActorIcon = *tempActorIconPtr;
                        m_template->setActorIcon(input_actionID_Valid, *tempActorIconPtr2);
                        m_template->setActorIcon(input_actionID_Back, tempActorIcon);
                    }

                    StringID menuNext = ITF_GET_STRINGID_CRC(MENU_NEXT, 266243684);
                    StringID menuPrev = ITF_GET_STRINGID_CRC(MENU_PREV, 362520035);
                    tempActorIconPtr = m_template->getActorIcon(menuNext);
                    tempActorIconPtr2 = m_template->getActorIcon(menuPrev);
                    if (tempActorIconPtr != NULL && tempActorIconPtr2 != NULL)
                    {
                        tempActorIcon = *tempActorIconPtr;
                        m_template->setActorIcon(menuNext, *tempActorIconPtr2);
                        m_template->setActorIcon(menuPrev, tempActorIcon);
                    }
                }
            }
        }

        // load config file with font effect presets
        m_fontEffectsPath = GETPATH_ALIAS("fonteffectpreset");
        if ( !m_fontEffectsPath.isEmpty() )
        {
            const FontEffectList_Template *fontEffectList = TEMPLATEDATABASE->requestTemplate<FontEffectList_Template>(m_fontEffectsPath);
            if (!fontEffectList)
            {
                ITF_FATAL_ERROR("Couldn't load font effect preset list : %s", m_configPath.toString8().cStr());
            }
            else
            {
                // store template
                m_fontEffects = const_cast<FontEffectList_Template*>(fontEffectList);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::onEvent(Event* _event)
    {
#ifdef ITF_SUPPORT_HOTRELOAD_TEMPLATE
        if (EventSingletonConfigChanged* configChanged = DYNAMIC_CAST(_event,EventSingletonConfigChanged))
        {
            // reload camera shake config file
            const Path configFile(UITEXTCONFIG_PATH);
            if (configChanged->getPath() == configFile)
            {
                const UITextManager_Template* config = TEMPLATEDATABASE->reloadTemplate<UITextManager_Template>(configFile);
                if (!config)
                {
                    ITF_FATAL_ERROR("Couldn't reload load UI text config: %s", configFile.toString8().cStr());
                }
                m_template = const_cast<UITextManager_Template*>(config); // store template
            }

            if (configChanged->getPath() == m_fontEffectsPath)
            {
                const FontEffectList_Template *fontEffectList = TEMPLATEDATABASE->reloadTemplate<FontEffectList_Template>(m_fontEffectsPath);
                if (!fontEffectList)
                {
                    ITF_FATAL_ERROR("Couldn't reload font effect preset list : %s", m_configPath.toString8().cStr());
                }
                else
                {
                    m_fontEffects = const_cast<FontEffectList_Template*>(fontEffectList);
                }
            }
        }
#endif //ITF_SUPPORT_HOTRELOAD_TEMPLATE
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::addVariable (const StringID &_varFriendly, const String8 &_varValue)
    {
        m_variables[_varFriendly] = _varValue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::removeVariable (const StringID &_varFriendly)
    {
        TextVariable::iterator it = m_variables.find(_varFriendly);
        if (it != m_variables.end())
           m_variables.erase(it);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextManager::updateVariable (const StringID &_varFriendly, const String8 &_newValue)
    {
        TextVariable::iterator it = m_variables.find(_varFriendly);
        if (it != m_variables.end())
            (*it).second = _newValue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    const String8& UITextManager::getVariableValue (const StringID &_varFriendly) const
    {
        TextVariable::const_iterator it = m_variables.find(_varFriendly);
        if (it != m_variables.end())
            return (*it).second;
        else return String8::emptyString;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    const UITextManager_Template::ActorIcon* UITextManager::getActorIcon(StringID _iconName) const
    {
        return m_template->getActorIcon(_iconName);
    }


    //////////////////////////////////////////////////////////////////////////
    const FontEffect_Template *UITextManager::getFontEffect( u32 _index ) const 
    { 
        if(const FontEffect_Template * effect = m_fontEffects->getByIndex(_index))
        {
            if(effect->hasFlags(FontEffect_Template::LanguageJapaneseExcept) && LOCALISATIONMANAGER->getCurrentLanguage()==ITF_LANGUAGE_JAPANESE)
            {
                return NULL;
            }
            return effect;
        }
        return NULL; 
    }

    const FontEffect_Template *UITextManager::getFontEffect( StringID _name ) const 
    { 
        if(const FontEffect_Template * effect = m_fontEffects->get(_name))
        {
            if(effect->hasFlags(FontEffect_Template::LanguageJapaneseExcept) && LOCALISATIONMANAGER->getCurrentLanguage()==ITF_LANGUAGE_JAPANESE)
            {
                return NULL;
            }
            return effect;
        }
        return NULL;
    }

    //////////////////////////////////////////////////////////////////////////

    BEGIN_SERIALIZATION_SUBCLASS(UITextManager_Template,ActorIcon)
        SERIALIZE_MEMBER("iconName",m_iconName);
        SERIALIZE_MEMBER("iconPath",m_iconPath);
        SERIALIZE_MEMBER("fontSize",m_fontSize);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(UITextManager_Template)
    BEGIN_SERIALIZATION_CHILD(UITextManager_Template)
        SERIALIZE_CONTAINER_OBJECT("actorIcons",m_actorIcons); 
    END_SERIALIZATION()

    const UITextManager_Template::ActorIcon* UITextManager_Template::getActorIcon(StringID _iconName) const
    {
        if(_iconName.isValid())
        {
            for(ITF_VECTOR<ActorIcon>::const_iterator it =m_actorIcons.begin(); it!=m_actorIcons.end(); ++it)
            {
                const ActorIcon& actorIcon = *it;
                if(actorIcon.m_iconName == _iconName)
                {
                    return &actorIcon;
                }
            }
        }

        return NULL;
    }

    void UITextManager_Template::setActorIcon(StringID _iconName, const UITextManager_Template::ActorIcon &_actorIcon)
    {
        if(_iconName.isValid())
        {
            for(ITF_VECTOR<ActorIcon>::iterator it = m_actorIcons.begin(); it != m_actorIcons.end(); ++it)
            {
                ActorIcon& actorIcon = *it;
                if(actorIcon.m_iconName == _iconName)
                {
                    actorIcon = _actorIcon;
                    actorIcon.m_iconName = _iconName;
                }
            }
        }
    }

    //=============================================================================================
    //
    //  Font Effect template
    //      data for some effect on text characters...
    //
    //=============================================================================================

    //---------------------------------------------------------------------------------------------
    // Serialization
    //
    BEGIN_SERIALIZATION(FontEffect_Template)
        //----- name ------------------------------------------------------------------------------
        SERIALIZE_MEMBER("name", m_name );

        //----- type ------------------------------------------------------------------------------
        SERIALIZE_ENUM_BEGIN("flags", m_flags);
        SERIALIZE_ENUM_VAR(none);
        SERIALIZE_ENUM_VAR(LanguageJapaneseExcept);
        SERIALIZE_ENUM_END();

        //----- type ------------------------------------------------------------------------------
        SERIALIZE_ENUM_BEGIN("type", m_type);
            SERIALIZE_ENUM_VAR(shiftY);
            SERIALIZE_ENUM_VAR(scale);
            SERIALIZE_ENUM_VAR(rotate);
            SERIALIZE_ENUM_VAR(zoomAlpha);
            SERIALIZE_ENUM_VAR(clampY);
            SERIALIZE_ENUM_VAR(shiftYandRotate);
        SERIALIZE_ENUM_END();

        //----- time ------------------------------------------------------------------------------
        SERIALIZE_MEMBER("fadeinStart", m_timeFadeInStart);
        SERIALIZE_MEMBER("fadeinEnd", m_timeFadeInEnd);
        SERIALIZE_MEMBER("fadeoutStart", m_timeFadeOutStart);
        SERIALIZE_MEMBER("fadeoutEnd", m_timeFadeOutEnd);
        //----- speed -----------------------------------------------------------------------------
        SERIALIZE_MEMBER("speedMin", m_speedMin);
        SERIALIZE_MEMBER("speedMax", m_speedMax);
        //----- static ----------------------------------------------------------------------------
        SERIALIZE_MEMBER("static", m_static);
        SERIALIZE_MEMBER("staticSeed", m_staticSeed);
        //----- params ----------------------------------------------------------------------------
        SERIALIZE_MEMBER("min", m_min);
        SERIALIZE_MEMBER("max", m_max);
        SERIALIZE_MEMBER("limit", m_limit);
        SERIALIZE_MEMBER("value", m_value);
        SERIALIZE_MEMBER("rotateCycle", m_rotateCycle);
        SERIALIZE_MEMBER("rotateSeedFactor", m_rotateSeedFactor);
    END_SERIALIZATION()

    //=================================================================================================
    //
    //  Font Effect List
    //      data for some effect on text characters...
    //
    //=================================================================================================

    //---------------------------------------------------------------------------------------------
    // list of font effect template
    //
    IMPLEMENT_OBJECT_RTTI(FontEffectList_Template)
    BEGIN_SERIALIZATION_CHILD(FontEffectList_Template)
        SERIALIZE_CONTAINER_OBJECT("effects",m_effects);
    END_SERIALIZATION()

    //---------------------------------------------------------------------------------------------
    // return font effect at given index
    //
    const FontEffect_Template *FontEffectList_Template::getByIndex( u32 _index ) const
    {
        if (_index >= m_effects.size()) return NULL;
        return &m_effects[_index];
    }

    //---------------------------------------------------------------------------------------------
    // return font effect with given _name
    //
    const FontEffect_Template *FontEffectList_Template::get( StringID _name ) const
    {
        for (u32 i = 0; i < m_effects.size(); i++)
        {
            if (m_effects[i].isNamed(_name))
                return &m_effects[i];
        }
        return NULL;
    }



}
