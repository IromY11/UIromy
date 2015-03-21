#ifndef _ITF_UIITEMCHECK_H_
#define _ITF_UIITEMCHECK_H_

#ifndef _ITF_UIITEMBASIC_H_
#include "gameplay/components/UI/UIItemBasic.h"
#endif //_ITF_UIITEMBASIC_H_

namespace ITF
{

#define UIItemCheck_CRC ITF_GET_STRINGID_CRC(UIItemCheck,2506628763)

    class UIItemCheck : public UIItemBasic
    {
    DECLARE_OBJECT_CHILD_RTTI(UIItemCheck, UIItemBasic, 2506628763);

    public:
        DECLARE_SERIALIZE()  

        UIItemCheck  ();
        virtual ~UIItemCheck ();

        virtual void onActorLoaded(Pickable::HotReloadType _type);
        virtual void onFinalizeLoad();
        virtual void Update(f32 _dt);
        virtual bbool onValidate(const bbool _isValidate, const i32 _controllerID = -1);
        virtual bbool onChecked(const u32 _checked);
        virtual bbool onSelected(const bbool _isSelected);

        u32   getChecked() const {return m_currentChecked;}

    protected:

        const class UIItemCheck_Template* getTemplate() const;

        void applyChecked(u32 _checked);

        u32 m_initChecked;
        u32 m_currentChecked;
    };


    class UIItemCheck_Template : public UIItemBasic_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIItemCheck_Template, UIItemBasic_Template, 957199126);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIItemCheck);

    public:

        UIItemCheck_Template();
        ~UIItemCheck_Template();

        f32 m_iconScaleFactor;
        SmartLocId m_tagChecked;
        SmartLocId m_tagUnchecked;
        ITF_VECTOR<SmartLocId> m_tagText;
    };

    ITF_INLINE const UIItemCheck_Template* UIItemCheck::getTemplate() const
    {
        return static_cast<const UIItemCheck_Template*>(m_template);
    }
}
#endif // _ITF_UIITEMCHECK_H_