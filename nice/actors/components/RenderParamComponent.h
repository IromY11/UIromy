#ifndef ITF_RENDERPARAMCOMPONENT_H_
#define ITF_RENDERPARAMCOMPONENT_H_

#define RenderParamComponent_CRC ITF_GET_STRINGID_CRC(RenderParamComponent,1642606963)
#define RenderParamComponent_Template_CRC ITF_GET_STRINGID_CRC(RenderParamComponent_Template,4075286399)

#ifndef _ITF_RENDERPARAMMANAGER_H_
#include    "engine/display/RenderParamManager.h"
#endif // _ITF_RENDERPARAMMANAGER_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{
    class RenderParamComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(RenderParamComponent, ActorComponent, RenderParamComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        RenderParamComponent();
        virtual ~RenderParamComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return false; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return m_actor->getIs2DNoScreenRatio(); }

        virtual void        Update(f32 _deltaTime);
        virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views );
        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );

        virtual void		onEvent(Event * _event);

        inline SubRenderParam_ClearColor& getClearColorParam()
		{
			return m_clearColor;
		}

        inline SubRenderParam_Lighting& getLightingParam()
		{
			return m_lighting;
		}

        inline SubRenderParam_Misc& getMiscParam()
		{
			return m_misc;
		}

        inline SubRenderParam_Mask& getMaskParam()
		{
			return m_mask;
		}

#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
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
    private:
        ITF_INLINE const class RenderParamComponent_Template* getTemplate() const;
        void computeUnitScaledAABB();

        SubRenderParam_ClearColor m_clearColor;
        SubRenderParam_Lighting m_lighting;
        SubRenderParam_Misc m_misc;
        SubRenderParam_Mask m_mask;
		GFX_RenderParam	m_renderParam;
		bbool			m_alwaysActive;

	    // Input
        Input           m_inputColor;
        Input           m_inputFactor;
	};


    // Inline functions
    template <typename T>
    ITF_INLINE void RenderParamComponent::setInput(StringID _inputName, T _value )
    {
        if (m_inputColor.getId() == _inputName)
        {
            m_inputColor.setValue(_value);
        }

        if (m_inputFactor.getId() == _inputName)
        {
            m_inputFactor.setValue(_value);
        }
    }

	//---------------------------------------------------------------------------------------------------

    class RenderParamComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(RenderParamComponent_Template, ActorComponent_Template, RenderParamComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(RenderParamComponent)
        DECLARE_SERIALIZE()

    public:

        RenderParamComponent_Template();
        virtual ~RenderParamComponent_Template();

        ITF_INLINE const InputDesc& getInput() const { return m_inputColor; }
        
        InputDesc      m_inputColor;
        InputDesc      m_inputFactor;
    private:
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const RenderParamComponent_Template* RenderParamComponent::getTemplate() const
    {
        return static_cast<const RenderParamComponent_Template*>(m_template);
    }
}

#endif // ITF_RENDERPARAMCOMPONENT_H_
