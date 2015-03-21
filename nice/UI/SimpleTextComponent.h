#ifndef _ITF_SIMPLETEXTCOMPONENT_H_
#define _ITF_SIMPLETEXTCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif //_LOCALISATION_ID_H_

namespace ITF
{
    class UITextBox;

    // TO BE DEPRECATED! Yousri, please delete this after you refactored PlayText_evt
    class SimpleTextComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SimpleTextComponent,ActorComponent,2170710125);

    public:
        DECLARE_SERIALIZE()

        SimpleTextComponent();
        ~SimpleTextComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }
        virtual void        Update(f32 _dt);

        void                updateText();

        virtual void        onBecomeActive();

        const String8 &     getTextLabel() {return m_text;}
        void                setText(const String8 & _text) {m_text = _text; m_needUpdate = btrue;}
        void				setTextColor(const Color& _color) { m_textColor = _color; m_needUpdate = btrue; };

        void                setTextSize( const f32 _size ) { m_textSize = _size; m_needUpdate = btrue;}
        const f32           getTextSize() const { return m_textSize; }
        void                setBackgroundColor(const Color& _color) { m_backgroundColor = _color; m_needUpdate = btrue; }

        void                display( bbool _display ) { m_display = _display; m_needUpdate = btrue; }
        LocalisationId      getLineId() const { return m_lineId; }

        void                clearInstanceData(Actor *_actor) { m_actor = _actor; m_textBox = NULL;  }

    private:
        bbool           m_needUpdate;
        String8         m_text;
        LocalisationId  m_lineId;
        bbool           m_drawUsingEngine;

        f32             m_drawBoxWidth, m_drawBoxHeight;
        UITextBox*      m_textBox;
        f32             m_textSize;

        Color           m_textColor;
        Color           m_backgroundColor;

        bbool           m_center;
        bbool           m_display;
    };

    //---------------------------------------------------------------------------------------------------

    class SimpleTextComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(SimpleTextComponent_Template,ActorComponent_Template,1086261024);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(SimpleTextComponent);

    public:

        SimpleTextComponent_Template() {}
        ~SimpleTextComponent_Template() {}

    private:

    };
}

#endif // _ITF_SIMPLETEXTCOMPONENT_H_

