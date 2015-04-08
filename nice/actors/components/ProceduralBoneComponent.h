#ifndef _ITF_PROCEDURALBONECOMPONENT_H_
#define _ITF_PROCEDURALBONECOMPONENT_H_

#define ProceduralBoneComponent_CRC ITF_GET_STRINGID_CRC(ProceduralBoneComponent,2441725815)
#define ProceduralBoneComponent_Template_CRC ITF_GET_STRINGID_CRC(ProceduralBoneComponent_Template,1429079675)

namespace ITF
{
    class ProceduralBone;
    typedef ITF_VECTOR<const ProceduralBone*> ProceduralBoneList;

    class ProceduralBoneComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ProceduralBoneComponent, ActorComponent, ProceduralBoneComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        ProceduralBoneComponent();
        virtual ~ProceduralBoneComponent();

        virtual bbool       needsUpdate() const                 { return bfalse; }
        virtual bbool       needsDraw() const                   { return bfalse; }
        virtual bbool       needsDraw2D() const                 { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const	{ return bfalse; }

        ITF_INLINE void registerBone( const ProceduralBone* _bone )   { m_bones.push_back(_bone); }
        ITF_INLINE const ProceduralBoneList& getBones() const   { return m_bones; }
        const ProceduralBone* getBone( const StringID& _id ) const;

#ifdef ITF_SUPPORT_EDITOR
    void drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class ProceduralBoneComponent_Template* getTemplate() const;

        ProceduralBoneList m_bones;
    };


    //---------------------------------------------------------------------------------------------------

    class ProceduralBoneComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(ProceduralBoneComponent_Template, ActorComponent_Template, ProceduralBoneComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(ProceduralBoneComponent)
        DECLARE_SERIALIZE()

    public:

        ProceduralBoneComponent_Template()
            : Super()
        {
        }
        virtual ~ProceduralBoneComponent_Template() {}


    private:

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const ProceduralBoneComponent_Template* ProceduralBoneComponent::getTemplate() const
    {
        return static_cast<const ProceduralBoneComponent_Template*>(m_template);
    }
}

#endif // _ITF_PROCEDURALBONECOMPONENT_H_
