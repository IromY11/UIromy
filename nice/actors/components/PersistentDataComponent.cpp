#include "precompiled_engine.h"

#ifndef _ITF_PERSISTENTDATACOMPONENT_H_
#include "engine/actors/components/PersistentDataComponent.h"
#endif //_ITF_PERSISTENTDATACOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PersistentDataComponent_Template)

    BEGIN_SERIALIZATION_CHILD(PersistentDataComponent_Template)
    END_SERIALIZATION()

    PersistentDataComponent_Template::PersistentDataComponent_Template()
        : Super()
    {
    }

    PersistentDataComponent_Template::~PersistentDataComponent_Template()
    {
    }


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(PersistentDataComponent)

    BEGIN_SERIALIZATION_CHILD(PersistentDataComponent)
        PersistentDataComponent::SerializePersistent(serializer, flags);
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(PersistentDataComponent)
    END_VALIDATE_COMPONENT()

    PersistentDataComponent::PersistentDataComponent()
        : Super()
    {
    }

    PersistentDataComponent::~PersistentDataComponent()
    {
    }

    void PersistentDataComponent::SerializePersistent( CSerializerObject* serializer, u32 flags )
    {
        if(flags & ESerialize_Persistent)
            Super::SerializePersistent(serializer, flags);
        SERIALIZE_CONTAINER("i32Map",     m_i32Map);
        SERIALIZE_CONTAINER("f32Map",     m_f32Map);
        SERIALIZE_CONTAINER("string8Map", m_string8Map);
    }

    u32 PersistentDataComponent::getValueCount() const
    {
        return m_i32Map.size()
            +  m_f32Map.size()
            +  m_string8Map.size();
    }

    PersistentDataComponent::eValueType PersistentDataComponent::getValueType(StringID _id) const
    {
        if(m_i32Map.find(_id) != m_i32Map.end())
            return type_i32;
        if(m_f32Map.find(_id) != m_f32Map.end())
            return type_f32;
        if(m_string8Map.find(_id) != m_string8Map.end())
            return type_f32;
        return type_invalid;
    }

    bool PersistentDataComponent::getValue(i32& _res, StringID _id) const
    {
        ITF_MAP<StringID,i32>::const_iterator it = m_i32Map.find(_id);
        if(it != m_i32Map.end())
        {
            _res = it->second;
            return true;
        }
        return false;
    }

    bool PersistentDataComponent::getValue(f32& _res, StringID _id) const
    {
        ITF_MAP<StringID,f32>::const_iterator it = m_f32Map.find(_id);
        if(it != m_f32Map.end())
        {
            _res = it->second;
            return true;
        }
        return false;
    }

    bool PersistentDataComponent::getValue(String8& _res, StringID _id) const
    {
        ITF_MAP<StringID,String8>::const_iterator it = m_string8Map.find(_id);
        if(it != m_string8Map.end())
        {
            _res = it->second;
            return true;
        }
        return false;
    }
}
