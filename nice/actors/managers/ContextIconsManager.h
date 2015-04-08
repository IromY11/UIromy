#ifndef _ITF_CONTEXTICONSMANAGER_H_
#define _ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

namespace ITF {

class ContextIconsManager_Template;
class UIComponent;

//------------------------------------------------------------------------------
class ContextIconsManager
{
public:
    ContextIconsManager();
    ~ContextIconsManager();

    void init();
    void update();
    bbool isVisible() const;
    void show(
        EContextIcon _left,
        EContextIcon _right,
        EContextIcon _topLeft = ContextIcon_Invalid,
        EContextIcon _topRight = ContextIcon_Invalid
        );
    void hide();

    void changeLeftIcon(EContextIcon _icon);
    void changeRightIcon(EContextIcon _icon);
    void changeTopLeftIcon(EContextIcon _icon);
    void changeTopRightIcon(EContextIcon _icon);

    EContextIconType getType(EContextIcon _icon) const;
    
    const String8& getIconStr(u32 _padType, EContextIconType _context);

    ITF_INLINE void setRuntimeDepthRank(u32 _val) { m_useRuntimeDepthRank = btrue; m_runtimeDepthRank = _val; }
    ITF_INLINE void resetRuntimeDepthRank() { m_useRuntimeDepthRank = bfalse; m_runtimeDepthRank = 0; }

private:
    void setupMenu();
    void setupIcon(EContextIcon _icon, UIComponent* _iconUI, UIComponent* _textUI);

    static const EContextIconType s_iconsTypes[ContextIcon_Count];

    ContextIconsManager_Template* m_template;
    Path m_configPath;

    EContextIcon m_left;
    EContextIcon m_right;
    EContextIcon m_topLeft;
    EContextIcon m_topRight;

    bbool m_useRuntimeDepthRank;
    i32 m_runtimeDepthRank;
};

//------------------------------------------------------------------------------
class ContextIconsManager_Template : public TemplateObj
{
    DECLARE_OBJECT_CHILD_RTTI(ContextIconsManager_Template, TemplateObj,543381722);
    DECLARE_SERIALIZE()

public:

    struct ButtonName
    {
        DECLARE_SERIALIZE()

        ITF_VECTOR <String8>        m_names;
    };

    ContextIconsManager_Template();
    virtual ~ContextIconsManager_Template();

    ITF_INLINE const StringID& getMenuId() const { return m_menuId; }
    ITF_INLINE const ITF_VECTOR<LocalisationId>& getLineIds() const { return m_lineIds; }
    ITF_INLINE const ITF_VECTOR<ButtonName>& getButtonNames() const { return m_buttonNames; }

private:
    StringID m_menuId;
    ITF_VECTOR<LocalisationId> m_lineIds;
    ITF_VECTOR< ButtonName > m_buttonNames;
};

} // namespace ITF

#endif //_ITF_CONTEXTICONSMANAGER_H_
