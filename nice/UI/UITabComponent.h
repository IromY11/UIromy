#ifndef _ITF_UITABCOMPONENT_H_
#define _ITF_UITABCOMPONENT_H_

#ifndef _ITF_UIITEMBASIC_H_
#include "gameplay/components/UI/UIItemBasic.h"
#endif //_ITF_UIITEMBASIC_H_

namespace ITF
{

#define UITabComponent_CRC ITF_GET_STRINGID_CRC(UITabComponent, 814301295)
#define UITabComponent_Template_CRC ITF_GET_STRINGID_CRC(UITabComponent_Template, 403898405)

    class UITabComponent : public UIItemBasic
    {
        DECLARE_OBJECT_CHILD_RTTI(UITabComponent, UIItemBasic, UITabComponent_CRC);

    public:
        DECLARE_SERIALIZE()  

        UITabComponent();
        virtual ~UITabComponent();

        void enableCustomRenderBoxColor(Color _color);
        void disableCustomRenderBoxColor();

    protected:
        const class UITabComponent_Template* getTemplate() const;

        virtual const Color& getSelectColor() const;
        virtual const Color& getUnselectColor() const;
        virtual const Color& getLockColor() const;

    private:
        bbool m_useCustomRenderBoxColor;
        Color m_customRenderBoxColor;
    };

    class UITabComponent_Template : public UIItemBasic_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UITabComponent_Template, UIItemBasic_Template, UITabComponent_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UITabComponent);

    public:

        UITabComponent_Template();
        ~UITabComponent_Template();
    };

    ITF_INLINE const UITabComponent_Template* UITabComponent::getTemplate() const
    {
        return static_cast<const UITabComponent_Template*>(m_template);
    }
}
#endif // _ITF_UITABCOMPONENT_H_