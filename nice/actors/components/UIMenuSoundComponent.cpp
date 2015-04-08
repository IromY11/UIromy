#include "precompiled_engine.h"

#ifndef _ITF_UIMENUSOUNDCOMPONENT_H_
#include "engine/actors/components/UIMenuSoundComponent.h"
#endif //_ITF_UIMENUSOUNDCOMPONENT_H_

#include "adapters/AudioSerializedData/SoundComponent_common.h"

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(UIMenuSoundComponent)

    BEGIN_VALIDATE_COMPONENT(UIMenuSoundComponent)
        VALIDATE_COMPONENT_PARAM("soundComponent", m_soundComponent, "Component mandatory" );
    END_VALIDATE_COMPONENT()

    BEGIN_SERIALIZATION_CHILD(UIMenuSoundComponent)
    END_SERIALIZATION()

#ifdef ITF_SUPPORT_EDITOR
    const u32 UIMenuSoundComponent::MAX_DEBUG_DRAW_ELEMENTS = 16;
#endif //ITF_SUPPORT_EDITOR

    //------------------------------------------------------------------
    UIMenuSoundComponent::UIMenuSoundComponent()
    : m_soundComponent(NULL)
    , m_registeredToUIManager(bfalse)
    {
    }

    //------------------------------------------------------------------
    void UIMenuSoundComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //get sound component
        m_soundComponent = GetActor()->GetComponent<SoundComponent>();
    }

    void UIMenuSoundComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        //register to UI Menu Manager
        ITF_ASSERT(UI_MENUMANAGER);
        //UI_MENUMANAGER->registerUIMenuSoundPlayer(this);
    }

    //------------------------------------------------------------------
    void UIMenuSoundComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();

        //UI_MENUMANAGER->unregisterUIMenuSoundPlayer(this);
    }

    //------------------------------------------------------------------
    void UIMenuSoundComponent::Update(f32 _dt)
    {
        Super::Update(_dt);

#ifdef ITF_SUPPORT_EDITOR
        updateDebug(_dt);
#endif //ITF_SUPPORT_EDITOR
    }

#ifdef ITF_SUPPORT_EDITOR
    void UIMenuSoundComponent::updateDebug(const f32 _dt)
    {
        if(getTemplate()->debugMode())
        {
            const f32 horizontalPosition = 100.0f;
            f32 verticalPosition = 100.0f;
            for(u32 index = 0; index < m_debugActionList.size();)
            {
                MenuActionDebug & debugEntry = m_debugActionList[index];
                
                DebugDraw::text(horizontalPosition, verticalPosition, Color::grey(), 
                    "Menu : %s Action : %s Selection : %s", 
                    debugEntry.m_menu.getDebugString(), 
                    debugEntry.m_action.getDebugString(), 
                    debugEntry.m_selection.getDebugString());
                ++index;
                verticalPosition += 20.0f;
            }
        }
    }
#endif //ITF_SUPPORT_EDITOR

    //------------------------------------------------------------------
    void UIMenuSoundComponent::onAction(const StringID & _menu, const StringID & _action, const StringID & _selection)
    {
#ifdef ITF_SUPPORT_EDITOR
        if( getTemplate()->debugMode() )
        {
            m_debugActionList.push_back(MenuActionDebug(_menu, _action, _selection));
            if( m_debugActionList.size() > MAX_DEBUG_DRAW_ELEMENTS )
            {
                m_debugActionList.eraseKeepOrder(0);
            }
        }
#endif //ITF_SUPPORT_EDITOR

        StringID sound = StringID::Invalid;
        if(getTemplate()->getDataFromMenuActionSelection(_menu, _action, _selection, sound))
        {
            m_soundComponent->playSound(sound);
        }
    }

    //------------------------------------------------------------------
    void UIMenuSoundComponent::stopAllSounds()
    {
        m_soundComponent->stopAll();
    }

    BEGIN_SERIALIZATION(UIMenuActionSound)
        SERIALIZE_MEMBER("action", m_action);
        SERIALIZE_MEMBER("selection", m_selection);
        SERIALIZE_MEMBER("sound", m_sound);
    END_SERIALIZATION()

    //------------------------------------------------------------------
    UIMenuActionSound::UIMenuActionSound()
    : m_action(StringID::Invalid)
    , m_selection(StringID::Invalid)
    , m_sound(StringID::Invalid)
    {        
    }

    BEGIN_SERIALIZATION(UIMenuPageSoundConfig)
        SERIALIZE_MEMBER("pageName", m_menuPageId);
        SERIALIZE_CONTAINER_OBJECT("actionSoundList", m_actionSoundList);
    END_SERIALIZATION()

    //------------------------------------------------------------------
    UIMenuPageSoundConfig::UIMenuPageSoundConfig()
    : m_menuPageId(StringID::Invalid)
    {
    }

    //------------------------------------------------------------------
    bbool UIMenuPageSoundConfig::getDataFromActionSelection(const StringID & _action, const StringID & _selection, StringID & _sound) const
    {
        StringID defaultSound = StringID::Invalid;

        for(u32 index = 0; index < m_actionSoundList.size(); ++index)
        {
            //look for matching action
            if( m_actionSoundList[index].getActionName() == _action )
            {
                //default config for this action
                if( !m_actionSoundList[index].getSelection().isValid())
                {
                    defaultSound = m_actionSoundList[index].getSoundName();
                }
                //specific config
                else if( _selection == m_actionSoundList[index].getSelection() )
                {
                    //if a specific config has been found don't look further and return this sound data
                    _sound = m_actionSoundList[index].getSoundName();
                    return btrue;
                }
            }
        }

        //if no specific sound has been found but a default one
        if( defaultSound.isValid() )
        {
            //return default sound data
            _sound = defaultSound;
            return btrue;
        }
        else
        {
            //nothing has been found
            return bfalse;
        }
    }


    IMPLEMENT_OBJECT_RTTI(UIMenuSoundComponent_Template)

    BEGIN_SERIALIZATION_CHILD(UIMenuSoundComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("pageConfigList", m_pageConfigList);
    
BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
#ifdef ITF_SUPPORT_EDITOR
        SERIALIZE_MEMBER("debugMode", m_debugMode);
#endif //ITF_SUPPORT_EDITOR
END_CONDITION_BLOCK()

    END_SERIALIZATION()

    //------------------------------------------------------------------
    UIMenuSoundComponent_Template::UIMenuSoundComponent_Template()
#ifdef ITF_SUPPORT_EDITOR
    : m_debugMode(bfalse)
#endif //ITF_SUPPORT_EDITOR
    {
    }

    //------------------------------------------------------------------
    bbool UIMenuSoundComponent_Template::getDataFromMenuActionSelection(const StringID & _menu, const StringID & _action, const StringID & _selection, StringID & _sound) const
    {
        for(u32 index = 0; index < m_pageConfigList.size(); ++index)
        {
            if( m_pageConfigList[index].getMenuPageId() == _menu )
            {
                return m_pageConfigList[index].getDataFromActionSelection(_action, _selection, _sound);
            }
        }
        return bfalse;
    }
}

