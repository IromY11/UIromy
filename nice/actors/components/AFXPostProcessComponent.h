#ifndef _ITF_AFXPOSTPROCESSCOMPONENT_H_
#define _ITF_AFXPOSTPROCESSCOMPONENT_H_

#ifndef _ITF_AFTERFX_H_
#include "engine/display/AfterFx.h"
#endif //_ITF_AFTERFX_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{
	class AFXPostProcessComponent : public ActorComponent
	{
        DECLARE_OBJECT_CHILD_RTTI(AFXPostProcessComponent, ActorComponent,724868713)
	public:
        DECLARE_SERIALIZE()

        AFXPostProcessComponent();
        ~AFXPostProcessComponent();
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return !m_actor->getIs2D(); }
        virtual bbool needsDraw2D() const { return m_actor->getIs2D(); }
		virtual	bbool needsDraw2DNoScreenRatio() const { return bfalse; }

		virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void  onEvent(Event * _event);
        virtual void  batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void  batchPrimitives2D( const ITF_VECTOR <class View*>& _views );
        virtual void  Update( f32 _deltaTime );

        void updateAABB();
        void pause(bbool _pause) {m_isPause = _pause;}
        void checkAndUpdateActived();
        void updatePrimitive();

#ifdef ITF_SUPPORT_EDITOR
        /////////////////////////////////
        //  Editors
        /////////////////////////////////
        virtual void  drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        /////////////////////////////////
        //  Inputs
        /////////////////////////////////
        template <typename T> void  setInput(StringID _inputName, T _value );
        template <typename T> void  getInputColor( T& _val ) const
        {
            m_inputColor.getValue(_val);
        }
        template <typename T> void  getInputFactor( T& _val ) const
        {
            m_inputFactor.getValue(_val);
        }
        template <typename T> void  getInput( T& _val ) const
        {
            T val;
            m_inputColor.getValue(_val);
            m_inputFactor.getValue(val);

            _val *= val;
        }

        /////////////////////////////////
        //  AFX Parameter
        /////////////////////////////////
        bbool             getUseBlur() const { return m_blur.m_use; }
        void              setUseBlur( bbool _use) { m_blur.m_use = _use; }
        f32               getPixelBlurSize() const { return m_blur.m_pixelSize; }
        u32               getBlurQuality() const { return m_blur.m_quality; }

        bbool             getUseGlow() const { return m_glow.m_use; }
        f32               getGlowFactor() const { return m_glow.m_factor; }
        bbool             getUseTonemap() const { return m_glow.m_useTonemap; }

		bbool             getUseColorSetting() const { return m_colorSetting.m_use; }
		void              setUseColorSetting( bbool _use) { m_colorSetting.m_use = _use; }
		f32               getColorSettingSaturation() const { return m_colorSetting.m_saturation; }
        f32               getColorSettingContrast() const { return m_colorSetting.m_contrast; }
        f32               getColorSettingContrastScale() const { return m_colorSetting.m_contrastScale; }
        f32               getColorSettingBright() const { return m_colorSetting.m_bright; }
        Color             getColorSettingColorCorrection() const { return m_colorSetting.m_colorCorrection; }

        bbool             getUseTile() const { return m_tile.m_use; }
        f32               getTileFactor() const { return m_tile.m_fade; }
        f32               getTileRepetition() const { return m_tile.m_repetition; }

        bbool             getUseMosaic() const { return m_mosaic.m_use; }
        f32               getMosaicFactor() const { return m_mosaic.m_fade; }
        f32               getMosaicPixelSize() const { return m_mosaic.m_pixelSize; }

        bbool             getUseNegatif() const { return m_negatif.m_use; }
        f32               getNegatifFactor() const { return m_negatif.m_fade; }

        bbool             getUseKalei() const { return m_kalei.m_use; }
        f32               getKaleiFactor() const { return m_kalei.m_fade; }
        f32               getKaleiSize() const { return m_kalei.m_size; }
        Vec2d             getKaleiUV1() const { return m_kalei.m_uv1; }
        Vec2d             getKaleiUV2() const { return m_kalei.m_uv2; }
        Vec2d             getKaleiUV3() const { return m_kalei.m_uv3; }

		bbool             getUseEyeFish() const { return m_eyeFish.m_use; }
		void              setUseEyeFish( bbool _active) { m_eyeFish.m_use = _active; }
		f32               getEyeFishFactor() const { return m_eyeFish.m_fade; }
        f32               getEyeFishHeight() const { return m_eyeFish.m_height; }
        f32               getEyeFishScale() const { return m_eyeFish.m_scale; }

        bbool             getUseMirror() const { return m_mirror.m_use; }
        f32               getMirrorFactor() const { return m_mirror.m_fade; }
        f32               getMirrorOffsetX() const { return m_mirror.m_offsetX; }
        f32               getMirrorOffsetY() const { return m_mirror.m_offsetY; }

        bbool             getUseOldTV() const     { return m_oldTV.m_use; }
        f32               getOldTVFactor() const  { return m_oldTV.m_fade; }
        f32               getOldTVLineFade() const { return m_oldTV.m_lineFade; }
        bbool             getOldTVUseScanLine() const { return m_oldTV.m_useScanLine; }
        f32               getOldTVScanLineFade() const { return m_oldTV.m_scanLineFade; }
        f32               getOldTVScanLineSpeed() const { return m_oldTV.m_scanLineSpeed; }
        f32               getOldTVScanLineSize() const { return m_oldTV.m_scanLineSize; }

        bbool             getUseNoise() const { return m_noise.m_use; }
        f32               getNoiseFactor() const { return m_noise.m_fade; }
        f32               getNoiseBlend() const { return m_noise.m_blend; }

        bbool             getUseRadial() const { return m_radial.m_use; }
        f32               getRadialFactor() const { return m_radial.m_fade; }
        Vec2d             getRadialCenterOffset() const { return m_radial.m_centerOffset; }
        f32               getRadialSize() const { return m_radial.m_size; }
        f32               getRadialStrength() const { return m_radial.m_strength; }

		inline u32		  getViewportVisibility() const { return m_viewportVisibility; }
		inline void       setViewportVisibility(ux _flags) { m_viewportVisibility = _flags; }

	private:

        ITF_INLINE const class AFXPostProcessComponent_Template*  getTemplate() const;

        AFXPostProcess* m_afterFx;
        bbool           m_isPause;
        bbool           m_activated;
		u32				m_viewportVisibility;

        // Input
        Input           m_inputColor;
        Input           m_inputFactor;

        //Parameter
        AFX_BlurParam           m_blur;
        AFX_GlowParam           m_glow;
        AFX_RefractionParam     m_refraction;
        AFX_ColorSettingParam   m_colorSetting;

        AFX_TileParam           m_tile;
        AFX_MosaicParam         m_mosaic;
        AFX_NegatifParam        m_negatif;
        AFX_KaleiParam          m_kalei;
        AFX_EyeFishParam        m_eyeFish;
        AFX_MirrorParam         m_mirror;
        AFX_OldTVParam          m_oldTV;
        AFX_NoiseParam          m_noise;
        AFX_RadialParam         m_radial;
   };

    // Inline functions
    template <typename T>
    ITF_INLINE void AFXPostProcessComponent::setInput(StringID _inputName, T _value )
    {
        if (m_inputColor.getId() == _inputName)
        {
            m_inputColor.setValue(_value);
        }

        if (m_inputFactor.getId() == _inputName)
        {
            m_inputFactor.setValue(_value);
        }
        if (ITF_GET_STRINGID_CRC(blurPixelSize, 4284714727) == _inputName)
        {
            m_blur.m_pixelSize = _value;
        }
        if (ITF_GET_STRINGID_CRC(glowFactor, 2758153733) == _inputName)
        {
            m_glow.m_factor = _value;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    class AFXPostProcessComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AFXPostProcessComponent_Template, ActorComponent_Template,3687055764)
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(AFXPostProcessComponent);

    public:

        AFXPostProcessComponent_Template();
        ~AFXPostProcessComponent_Template() {}

        virtual bbool onTemplateLoaded( bbool _hotReload );
        virtual void onTemplateDelete( bbool _hotReload );

        ITF_INLINE const InputDesc& getInput() const { return m_inputColor; }
        
        const Path&    getOldTVTextureFile() const { return m_customTexOldTV; }

        InputDesc      m_inputColor;
        InputDesc      m_inputFactor;

        Path           m_customTexOldTV;
        ResourceID     m_textureOldTVID;
    };

//INLINED

    const AFXPostProcessComponent_Template*  AFXPostProcessComponent::getTemplate() const {return static_cast<const AFXPostProcessComponent_Template*>(m_template);} 
} // namespace ITF

#endif // _ITF_AFXPOSTPROCESSCOMPONENT_H_
