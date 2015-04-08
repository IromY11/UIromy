#ifndef ITF_MASKRESOLVERCOMPONENT_H_
#define ITF_MASKRESOLVERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef ITF_MASKRESOLVEPRIMITIVE_H_
#include "engine/display/MaskResolvePrimitive.h"
#endif //ITF_MASKRESOLVEPRIMITIVE_H_

namespace ITF
{

    class MaskResolverComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(MaskResolverComponent, ActorComponent, 2480302702);
    public:
        DECLARE_SERIALIZE()

        MaskResolverComponent();
        ~MaskResolverComponent();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return btrue; }
        virtual bbool needsDraw2D() const { return bfalse; }
		virtual	bbool needsDraw2DNoScreenRatio() const { return bfalse; }

		virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void  Update(f32 _deltaTime);
        virtual void  batchPrimitives( const ITF_VECTOR <class View*>& _views );

        inline bbool getFrontLightBuffer() const;
        inline bbool getFrontLightBufferInverted() const;
        inline bbool getBackLightBuffer() const;
        inline bbool getBlurFrontLightBuffer() const { return m_blurFrontLightBuffer; }
        inline bbool getBlurBackLightBuffer() const { return m_blurBackLightBuffer; }
        inline u32   getBlurQuality() const { return m_blurQuality; }
        inline u32   getBlurSize() const { return m_blurSize; }
        inline bbool getClearFrontLight() const { return m_clearFrontLight; }
        inline Color getClearFrontLightColor() const { return m_clearFrontLightColor; }
        inline bbool getClearBackLight() const { return m_clearBackLight; }
        inline Color getClearBackLightColor() const { return m_clearBackLightColor; }

#ifdef ITF_SUPPORT_EDITOR
        virtual void onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    private:
        inline const class MaskResolverComponent_Template *  getTemplate() const;

        void computeUnitScaledAABB();

        MaskResolvePrimitive m_maskResolvePrimitive;

        bbool m_clearFrontLight;
        Color m_clearFrontLightColor;
        bbool m_clearBackLight;
        Color m_clearBackLightColor;
        bbool m_blurFrontLightBuffer;
        bbool m_blurBackLightBuffer;
        u32   m_blurQuality;
        u32   m_blurSize;
    };

    //////////////////////////////////////////////////////////////////////////////////////////

    class MaskResolverComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(MaskResolverComponent_Template, ActorComponent_Template, 2336959986);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(MaskResolverComponent);

    public:

        MaskResolverComponent_Template();
        ~MaskResolverComponent_Template() {}

        bbool getFrontLightBuffer() const { return m_resolveFrontLightBuffer; }
        bbool getFrontLightBufferInverted() const { return m_resolveFrontLightBufferInverted; }
        bbool getBackLightBuffer() const { return m_resolveBackLightBuffer; }

    private:
        virtual bbool onTemplateLoaded( bbool _hotReload );
        virtual void onTemplateDelete( bbool _hotReload );

        bbool m_resolveFrontLightBuffer;
        bbool m_resolveBackLightBuffer;
        bbool m_resolveFrontLightBufferInverted;
    };

    //INLINED
    inline const MaskResolverComponent_Template *  MaskResolverComponent::getTemplate() const {return static_cast<const MaskResolverComponent_Template*> (m_template); }
    inline bbool MaskResolverComponent::getFrontLightBuffer() const { return getTemplate()->getFrontLightBuffer(); }
    inline bbool MaskResolverComponent::getFrontLightBufferInverted() const { return getTemplate()->getFrontLightBufferInverted(); }
    inline bbool MaskResolverComponent::getBackLightBuffer() const { return getTemplate()->getBackLightBuffer(); }

} // namespace ITF

#endif // ITF_MASKRESOLVERCOMPONENT_H_
