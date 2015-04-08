#ifndef _ITF_PERSISTENTDATACOMPONENT_H_
#define _ITF_PERSISTENTDATACOMPONENT_H_

#define PersistentDataComponent_CRC ITF_GET_STRINGID_CRC(PersistentDataComponent,3743025505)
#define PersistentDataComponent_Template_CRC ITF_GET_STRINGID_CRC(PersistentDataComponent_Template,2632225646)

namespace ITF
{
    class PersistentDataComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PersistentDataComponent, ActorComponent, PersistentDataComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:
        PersistentDataComponent();
        virtual ~PersistentDataComponent();
        virtual void        SerializePersistent( CSerializerObject* serializer, u32 flags );

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
	    virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        u32 getValueCount() const;
        enum eValueType
        {
            type_invalid = 0
           ,type_i32
           ,type_f32
           ,type_string8
        };

        eValueType getValueType(StringID _id) const;

        bool getValue(i32& _res, StringID _id) const;
        bool getValue(f32& _res, StringID _id) const;
        bool getValue(String8& _res, StringID _id) const;

        void setValue(StringID _id, i32 _val)            { m_i32Map[_id] = _val; }
        void setValue(StringID _id, f32 _val)            { m_f32Map[_id] = _val; }
        void setValue(StringID _id, const String8& _val) { m_string8Map[_id] = _val; }

    protected:
        ITF_MAP<StringID,i32>     m_i32Map;
        ITF_MAP<StringID,f32>     m_f32Map;
        ITF_MAP<StringID,String8> m_string8Map;

        ITF_INLINE const class PersistentDataComponent_Template* getTemplate() const;
    };


    //---------------------------------------------------------------------------------------------------

    class PersistentDataComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(PersistentDataComponent_Template, ActorComponent_Template, PersistentDataComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(PersistentDataComponent)
        DECLARE_SERIALIZE()

    public:

        PersistentDataComponent_Template();
        virtual ~PersistentDataComponent_Template();


    private:

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const PersistentDataComponent_Template* PersistentDataComponent::getTemplate() const
    {
        return static_cast<const PersistentDataComponent_Template*>(m_template);
    }
}

#endif // _ITF_PERSISTENTDATACOMPONENT_H_
