#ifndef ITF_SAVEACTORSTATECOMPONENT_H_
#define ITF_SAVEACTORSTATECOMPONENT_H_

#define SaveActorStateComponent_CRC ITF_GET_STRINGID_CRC(SaveActorStateComponent,608469777)
#define SaveActorStateComponent_Template_CRC ITF_GET_STRINGID_CRC(SaveActorStateComponent_Template,1445883070)

namespace ITF
{
    class SaveActorStateComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SaveActorStateComponent, ActorComponent, SaveActorStateComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        SaveActorStateComponent();
        virtual ~SaveActorStateComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
		virtual bbool       needsDraw2D() const { return bfalse; }
		virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        Update(f32 _deltaTime);

		ITF_INLINE const AABB& getSavedAABB() const { return m_savedAbsoluteAABB; }

    private:
        ITF_INLINE const class SaveActorStateComponent_Template* getTemplate() const;

		AABB	m_savedAbsoluteAABB;
    };


    //---------------------------------------------------------------------------------------------------

    class SaveActorStateComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(SaveActorStateComponent_Template, ActorComponent_Template, SaveActorStateComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(SaveActorStateComponent)
        DECLARE_SERIALIZE()

    public:

        SaveActorStateComponent_Template();
        virtual ~SaveActorStateComponent_Template();

    private:
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const SaveActorStateComponent_Template* SaveActorStateComponent::getTemplate() const
    {
        return static_cast<const SaveActorStateComponent_Template*>(m_template);
    }
}

#endif // ITF_SAVEACTORSTATECOMPONENT_H_
