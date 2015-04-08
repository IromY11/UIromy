#ifndef _ITF_BASICIKCOMPONENT_H_
#define _ITF_BASICIKCOMPONENT_H_

#define BasicIKComponent_CRC ITF_GET_STRINGID_CRC(BasicIKComponent,921937996)
#define BasicIKComponent_Template_CRC ITF_GET_STRINGID_CRC(BasicIKComponent_Template,3207211238)


namespace ITF
{

    struct TargetIK {
        TargetIK()
            : m_boneIndex(-1)
            , m_position(Vec2d::Zero)
            , m_angleSign(0)
            , m_process(bfalse)
        {
        }
        
        i32         m_boneIndex;
        Vec2d       m_position;
        u32         m_angleSign;
        bbool       m_process;
    };

    struct BoneAngle
    {
        BoneAngle()
            : m_boneIndex(-1)
            , m_angle(0.f)
        {
        }

        BoneAngle(const StringID & _name, i32 _index, f32 _angle)
            : m_name(_name)
            , m_boneIndex(_index)
            , m_angle(_angle)
        {
        }

        StringID    m_name;
        i32         m_boneIndex;
        f32         m_angle;
    };

    class BasicIKComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(BasicIKComponent, ActorComponent, BasicIKComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        BasicIKComponent();
        virtual ~BasicIKComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event);
        virtual void        onResourceReady();

        i32                 getBoneIndex(const StringID & _boneName);
        void                setTarget(i32 index, const Vec2d & _pos) { m_targets[index].m_position = _pos; m_targets[index].m_process = btrue; }
        void                setProcessTarget(i32 index, bbool _porcess) { m_targets[index].m_process = _porcess; }
        void                setTargetSign(i32 index, u32 _angleSign) { m_targets[index].m_angleSign = _angleSign; }

        void                clearBoneAngles() { m_boneAngles.clear(); }
        void                setBoneAngle(const StringID & _bone, f32 _angle);

    private:
        ITF_INLINE const class BasicIKComponent_Template* getTemplate() const;
        void                        computeIK(TargetIK & _target);
        void                        computeBoneAngle(BoneAngle & _boneAngle);

        class AnimLightComponent        * m_animComponent;
        class RenderSimpleAnimComponent * m_renderSimpleAnimComponent;
        ITF_VECTOR<TargetIK>         m_targets;
        ITF_VECTOR<BoneAngle>        m_boneAngles;

        String8                      m_error;
    };


    //---------------------------------------------------------------------------------------------------

    class BasicIKComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(BasicIKComponent_Template, ActorComponent_Template, BasicIKComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(BasicIKComponent)
        DECLARE_SERIALIZE()

    public:

        BasicIKComponent_Template();
        virtual ~BasicIKComponent_Template();


        const ITF_VECTOR<StringID>  & getBones() const { return m_bones; }
        bbool getDefaultProcess() const { return m_defaultProcess; }

    private:
        ITF_VECTOR<StringID>            m_bones;
        bbool                           m_defaultProcess;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const BasicIKComponent_Template* BasicIKComponent::getTemplate() const
    {
        return static_cast<const BasicIKComponent_Template*>(m_template);
    }
}

#endif // _ITF_BASICIKCOMPONENT_H_
