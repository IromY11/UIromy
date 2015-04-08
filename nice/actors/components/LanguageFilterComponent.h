#ifndef _ITF_LANGUAGEFILTERCOMPONENT_H_
#define _ITF_LANGUAGEFILTERCOMPONENT_H_

namespace ITF
{
    /*
        This component can help you enable / disable actors depending on the current language
    */
    class LanguageFilterComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(LanguageFilterComponent,ActorComponent,2264048742);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        LanguageFilterComponent() : m_operator(ITF_OPERATOR_IS) {}

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }

        void                onFinalizeLoad() override;

    private:
        ITF_INLINE const class LanguageFilterComponent_Template* getTemplate() const;

        enum
        {
            ITF_OPERATOR_IS,
            ITF_OPERATOR_ISNOT
        }							m_operator;
        ITF_VECTOR<u32>				m_languages;
    };


    //---------------------------------------------------------------------------------------------------

    class LanguageFilterComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(LanguageFilterComponent_Template,ActorComponent_Template,3846937408);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(LanguageFilterComponent);

    public:

        LanguageFilterComponent_Template() {}
        virtual ~LanguageFilterComponent_Template() {}

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const LanguageFilterComponent_Template* LanguageFilterComponent::getTemplate() const
    {
        return static_cast<const LanguageFilterComponent_Template*>(m_template);
    }
};

#endif //_ITF_LANGUAGEFILTERCOMPONENT_H_