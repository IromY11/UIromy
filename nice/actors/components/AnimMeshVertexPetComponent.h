#ifndef _ITF_ANIMMESHVERTEXPETCOMPONENT_H_
#define _ITF_ANIMMESHVERTEXPETCOMPONENT_H_

#define AnimMeshVertexPetComponent_CRC ITF_GET_STRINGID_CRC(AnimMeshVertexPetComponent,194664988)
#define AnimMeshVertexPetComponent_Template_CRC ITF_GET_STRINGID_CRC(AnimMeshVertexPetComponent_Template,3214879515)

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_ANIMATIONMESHVERTEX_H_
#include "engine/animation/AnimationMeshVertex.h"
#endif // _ITF_ANIMATIONMESHVERTEX_H_

namespace ITF
{
//#define USE_BODY_OFFSET

    class AnimMeshVertexPetPart
    {
        DECLARE_SERIALIZE()

    public:
        AnimMeshVertexPetPart() : m_type('A'), m_variant(-1) {}
        AnimMeshVertexPetPart(char _type, i32 _variant) : m_type(_type), m_variant(_variant) {}

    public:
        char        m_type;
        i32         m_variant;
    };

    class AnimMeshVertexPetData
    {
        DECLARE_SERIALIZE()

    public:
        AnimMeshVertexPetData() : m_position(Vec3d::Zero), m_angle(0.f), m_scale(Vec2d::One), m_flip(bfalse)
        {
        }

        ITF_VECTOR<AnimMeshVertexPetPart>   m_parts;
        Vec3d                               m_position;
        f32                                 m_angle;
        Vec2d                               m_scale;
        bbool                               m_flip;
    };

    class AnimMeshVertexPetAnim
    {
    public:
        u8                                  m_nbIdx;
        u16                                 m_startIdx;

        ITF_MAP<StringID, ITF_VECTOR<u32> > m_animNameToSubAnimIndexes;
        SafeArray<u32>                      m_uvRedirect;

#ifdef USE_BODY_OFFSET        
        char                                m_animType;
        ITF_VECTOR<Transform3d>             m_partOffset;
#endif
    };

    class AnimMeshVertexPetComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimMeshVertexPetComponent, ActorComponent, AnimMeshVertexPetComponent_CRC)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        AnimMeshVertexPetComponent();
        virtual ~AnimMeshVertexPetComponent();

        virtual bbool       needsUpdate() const;
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        Update( f32 _dt );
        virtual void        onResourceLoaded();
        
        i32                            addPet(const AnimMeshVertexPetData & _petData);

        bbool                          setPetAnim(u32 _petIdx, const StringID & _petAnim);
        bbool                          setPetFrame(u32 _petIdx, u32& _frame);
        bbool                          setPetTransform(u32 _petIdx, const Transform3d & _transform);
        bbool                          setPetColor(u32 _petIdx, const Color& _color);

        u32                            getPetNbFrames(u32 _petIdx);
        void                           sortPets();

        void                           updateAllUvRedirect();
        void                           fillRandomPetData(AnimMeshVertexPetData& _petData, bbool _allowEmpty = btrue);
        i32                            addRandomPet(const Vec3d & _pos);
        bbool                          removePet( i32 _idx );
        bbool                          updatePet( i32 _idx, const AnimMeshVertexPetData & _petData );

        const ITF_VECTOR<AnimMeshVertexPetData>& getAvailablePetList() const;

    private:
        bbool fillPetListAdnPetAnim(const AnimMeshVertexPetData & _petData, SingleAnimDataRuntimeList & _animList,  AnimMeshVertexPetAnim &  _petAnim);
        ITF_INLINE const class AnimMeshVertexPetComponent_Template* getTemplate() const;

        ITF_VECTOR<AnimMeshVertexPetAnim>       m_animListData;
        class AnimMeshVertexComponent *         m_amvComponent;
    };


    //---------------------------------------------------------------------------------------------------
#ifdef USE_BODY_OFFSET
    class PartOffsetData
    {
        DECLARE_SERIALIZE()
    
    public:
        StringID    m_name;
        Vec3d       m_position;
        f32         m_angle;
    };

    class TypeOffsetData
    {
        DECLARE_SERIALIZE()

    public:
        String8                     m_type;
        ITF_VECTOR<PartOffsetData>  m_partOffset;
    };
#endif

    class AnimMeshVertexPetComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimMeshVertexPetComponent_Template, ActorComponent_Template, AnimMeshVertexPetComponent_Template_CRC)
        DECLARE_ACTORCOMPONENT_TEMPLATE(AnimMeshVertexPetComponent)
        DECLARE_SERIALIZE()

    public:

        AnimMeshVertexPetComponent_Template();
        virtual ~AnimMeshVertexPetComponent_Template();


        virtual bbool       onTemplateLoaded( bbool _hotReload );
        
        const ITF_VECTOR<String8> &                 getBaseParts() const { return m_baseParts; }
        const ITF_VECTOR<String8> &                 getAnimList() const { return m_animList; }
        const ITF_VECTOR<AnimMeshVertexPetData> &   getPetList() const { return m_petList; }
        const ITF_VECTOR<Vec3d> &                   getRandomPetPosList() const { return m_randomPetsPosList; }
        
#ifdef USE_BODY_OFFSET
        const ITF_VECTOR<Transform3d> &             getPartOffsetList(char _type) const { return m_partOffsetListByType[_type - 'A']; }
        u32                                         getBodyIndex() const { return m_bodyIndex; }
#endif
        const ITF_VECTOR<AnimMeshVertexPetData>&    getAllPetList()     const { return m_allPetList; }
        ITF_INLINE  bbool                           getAllowUpdate()    const { return m_allowUpdate; }

    private:
        bbool                               	m_allowUpdate;
        ITF_VECTOR<String8>                     m_baseParts;
        ITF_VECTOR<String8>                     m_animList;

#ifdef USE_BODY_OFFSET
        u32                                     m_bodyIndex;
        ITF_VECTOR<TypeOffsetData>              m_bodyOffsetList;
        ITF_VECTOR<ITF_VECTOR<Transform3d> >    m_partOffsetListByType;
#endif

        ITF_VECTOR<AnimMeshVertexPetData>       m_allPetList;

        // TEST ONLY
        ITF_VECTOR<AnimMeshVertexPetData>       m_petList;
        ITF_VECTOR<Vec3d>                       m_randomPetsPosList;

    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const AnimMeshVertexPetComponent_Template* AnimMeshVertexPetComponent::getTemplate() const
    {
        return static_cast<const AnimMeshVertexPetComponent_Template*>(m_template);
    }
}

#endif // _ITF_ANIMMESHVERTEXPETCOMPONENT_H_
