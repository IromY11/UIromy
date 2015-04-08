#ifndef _ITF_DUMMYCOMPONENT_H_
#define _ITF_DUMMYCOMPONENT_H_

namespace ITF
{
    /*
        This component does nothing, to hack around the fact that actors
        with no components are considered invalid.
        It might also help us to identify those "intentionally left blank" actors
        in case we ever need to refactor them.
    */
    class DummyComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(DummyComponent,ActorComponent,2957589102);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        DummyComponent();
        virtual ~DummyComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }

    private:
        ITF_INLINE const class DummyComponent_Template* getTemplate() const;

    };


    //---------------------------------------------------------------------------------------------------

    class DummyComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(DummyComponent_Template,ActorComponent_Template,2112894595);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(DummyComponent);

    public:

        DummyComponent_Template() {}
        virtual ~DummyComponent_Template() {}

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const DummyComponent_Template* DummyComponent::getTemplate() const
    {
        return static_cast<const DummyComponent_Template*>(m_template);
    }
};

#endif //_ITF_DUMMYCOMPONENT_H_