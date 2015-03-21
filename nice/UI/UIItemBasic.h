#ifndef _ITF_UIITEMBASIC_H_
#define _ITF_UIITEMBASIC_H_

#ifndef _ITF_UIITEM_H_
#include "gameplay/components/UI/UIItem.h"
#endif //_ITF_UIITEM_H_

#ifndef _ITF_UIUTILS_H_
#include "gameplay/components/UI/UIUtils.h"
#endif //_ITF_UIUTILS_H_
#include "../../AI/Utils/EventSender.h"

namespace ITF
{
    

    class UIItemBasic : public UIItem
    {
    DECLARE_OBJECT_CHILD_RTTI(UIItemBasic, UIItem, 3965308782);

    public:
        DECLARE_SERIALIZE()  

        UIItemBasic  ();
        virtual ~UIItemBasic ();

        virtual void onActorLoaded(Pickable::HotReloadType _type);
        virtual void onFinalizeLoad();
        virtual void onStartDestroy(bbool _hotReload);
        virtual void Update(f32 _dt);
		virtual bbool onSelected(const bbool _isSelected);
        virtual bbool onLocked(const bbool _isLocked);
        virtual bbool onDown(const bbool _isDown);
        virtual bbool onActivating(const bbool _isActivate, const i32 _controllerID = -1);
        virtual bbool onIsActivating(f32 _dt);
        virtual bbool onValidate(const bbool _isValidate, const i32 _controllerID = -1) override;

        void setUseLockedItemTextAlpha(bbool _used);

		virtual StringID getAnimSelected();
		virtual StringID getAnimUnselected();
		virtual StringID getAnimLocked();
		virtual void stopBlink();
        ITF_INLINE const Path& getMapPath() const { return m_levelPath; }
		void setAnim(StringID _anim);

    protected:
        const class UIItemBasic_Template* getTemplate() const;

        virtual const Color& getSelectColor() const;
        virtual const Color& getUnselectColor() const;
        virtual const Color& getLockColor() const;

        class AnimLightComponent* m_animComponent;
        class AnimMeshVertexComponent* m_animMeshComponent;
        class RenderBoxComponent* m_boxComponent;
        class UITextBox* m_textComponent;

        PulseGenerator m_blinkPulse;
        PulseGenerator m_highlightPulse;
        ShakeGenerator m_activateShake;
        Vec3d m_shadowOffset;
        bbool m_needRefreshAnimMeshVertex;
		EventSender m_soundOnValidate;
		EventSender m_soundMoreOnValidate;
        bbool m_useLockedItemTextAlpha;

		PathRef m_levelPath;
        u32 m_selectTextStyle;
        ITF_VECTOR<StringID> m_selectAnimMeshVertex;
    };


    class UIItemBasic_Template : public UIItem_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIItemBasic_Template, UIItem_Template, 2055344672);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIItemBasic);

    public:

        UIItemBasic_Template();
        ~UIItemBasic_Template();
        
        f32 m_selectBlinkScaleMin;
        f32 m_selectBlinkScale;
        f32 m_selectBlinkPeriod;
        f32 m_selectBlinkMinBlendSpeed;
        f32 m_selectHightlightAlphaMin;
        f32 m_selectHightlightAlpha;
        f32 m_selectHightlightPeriod;
        f32 m_selectHightlightMinBlendSpeed;
        f32 m_activatingScale;
        f32 m_activatingDuration;
        u32 m_activatingRebound;
        StringID m_animUnselected;
        StringID m_animSelected;
        StringID m_animLocked;
		bool	m_needSyncBlink;
        StringID m_fontEffectUnselected;
        StringID m_fontEffectSelected;
        Color m_colorFactorSelected;
        Color m_colorFactorUnselected;
        Color m_colorFactorLocked;
        f32   m_textLockedAlpha;
        f32   m_animLockedAlpha;
        f32   m_colorBlendTime;
		f32	  m_scaleUnselected;
        Angle m_uvRotationSpeedSelected;
        Angle m_uvRotationSpeedUnselected;
        StringID m_iconNameSelected;
		StringID m_iconNameUnselected;
		StringID m_soundOnSelected;
		StringID m_soundOnChangeSelection;
		StringID m_soundOnSelectedLocked;
        bbool m_inverseShadowState;
    };

    ITF_INLINE const UIItemBasic_Template* UIItemBasic::getTemplate() const
    {
        return static_cast<const UIItemBasic_Template*>(m_template);
    }
}
#endif // _ITF_UIITEM_H_
