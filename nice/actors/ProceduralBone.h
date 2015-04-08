#ifndef _ITF_PROCEDURALBONE_H_
#define _ITF_PROCEDURALBONE_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

namespace ITF
{
    class ProceduralBone
    {
    public:

        ProceduralBone() {}
        virtual ~ProceduralBone() {}

        ITF_INLINE const Transform3d& getTransform() const { return m_transform; }
        ITF_INLINE Transform3d& getTransformMutable() { return m_transform; }
        ITF_INLINE void setTransform( const Transform3d& _value ) { m_transform = _value; }

        ITF_INLINE const StringID& getId() const { return m_id; }
        ITF_INLINE void setId( const StringID& _value ) { m_id = _value; }

        // utils
        static const ProceduralBone* getBone( const Actor* _actor, const StringID _id );
        static void registerBone( const Actor* _actor, const ProceduralBone* _bone );

    private:

        Transform3d m_transform;
        StringID m_id;
    };
}

#endif // _ITF_PROCEDURALBONE_H_
