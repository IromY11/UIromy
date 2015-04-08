#include "precompiled_engine.h"

#ifndef _ITF_LANGUAGEFILTERCOMPONENT_H_
#include "engine/actors/Components/LanguageFilterComponent.h"
#endif //_ITF_LanguageFilterComponent_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(LanguageFilterComponent_Template)

    BEGIN_SERIALIZATION_CHILD(LanguageFilterComponent_Template)
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(LanguageFilterComponent)

    BEGIN_SERIALIZATION_CHILD(LanguageFilterComponent)
        SERIALIZE_ENUM_BEGIN("operator",m_operator);
        SERIALIZE_ENUM_VAR(ITF_OPERATOR_IS);
        SERIALIZE_ENUM_VAR(ITF_OPERATOR_ISNOT);
        SERIALIZE_ENUM_END(); 
        SERIALIZE_MEMBER("languages",m_languages);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(LanguageFilterComponent)
    END_VALIDATE_COMPONENT()

    void LanguageFilterComponent::onFinalizeLoad()
    {
        bbool matchFilter = bfalse;

        ITF_LANGUAGE currentLanguage = LOCALISATIONMANAGER->getCurrentLanguage();
        if((m_languages.find(currentLanguage) != -1 && m_operator == ITF_OPERATOR_IS)
        || (m_languages.find(currentLanguage) == -1 && m_operator == ITF_OPERATOR_ISNOT))
        {
            matchFilter = btrue;
        }

        if(matchFilter)
        {
            if (!m_actor->isStartPaused())
                m_actor->enable();
        }
        else
        {
            m_actor->disable();

#ifndef ITF_SUPPORT_EDITOR
            // Release all resources to save up some memory (as we can't dynamically change language)
            m_actor->getResourceContainer()->clear();
#endif
        }
    }
};