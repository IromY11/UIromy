#ifndef _ITF_ACTORUTILS_H_
#define _ITF_ACTORUTILS_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

namespace ITF
{
    // Transform actor utils
    ITF_INLINE Transform2d getWorldTransform2d( const Pickable* _pickable, const bbool _useFlip = btrue, const bbool _useScale = bfalse )
    {
        return Transform2d(_pickable->get2DPos(), _pickable->getAngle(), _useScale ? _pickable->getScale() : Vec2d::One, _useFlip && _pickable->getIsFlipped());
    }

    ITF_INLINE Transform2d getWorldInitTransform2d( const Actor* _actor, const bbool _useFlip = btrue )
    {
        return Transform2d(_actor->getBoundWorldInitialPos(btrue).truncateTo2D(), _actor->getBoundWorldInitialAngle(btrue), _useFlip && _actor->getWorldInitialFlip());
    }

    ITF_INLINE Transform2d getLocalTransform2d( const Actor* _actor, const bbool _useFlip = btrue)
    {
        return Transform2d(_actor->getBoundLocal2DPos(), _actor->getBoundLocalAngle(), _useFlip && _actor->getLocalFlip());
    }

    ITF_INLINE Transform2d getLocalInitTransform2d( const Actor* _actor, const bbool _useFlip = btrue)
    {
        return Transform2d(_actor->getBoundLocalInitial2DPos(), _actor->getBoundLocalInitialAngle(), _useFlip && _actor->getLocalInitialFlip());
    }

    ITF_INLINE Transform3d getWorldTransform( const Pickable* _pickable, const bbool _useFlip = btrue, const bbool _useScale = bfalse )
    {
        return Transform3d(_pickable->getPos(), _pickable->getAngle(), _useScale ? _pickable->getScale() : Vec2d::One, _useFlip && _pickable->getIsFlipped());
    }

    ITF_INLINE Transform3d getWorld2DTransform( const Pickable* _pickable, const bbool _useFlip = btrue, const bbool _useScale = bfalse )
    {
        Vec2d scale = _useScale ? _pickable->getScale() : Vec2d::One;
        scale.y() *= -1.0f;
        return Transform3d(_pickable->getPos(), _pickable->getAngle(), scale, _useFlip && _pickable->getIsFlipped());
    }

    ITF_INLINE Transform3d getWorldInitTransform( const Actor* _actor, const bbool _useFlip = btrue, bbool _useScale = bfalse )
    {
        return Transform3d(_actor->getBoundWorldInitialPos(btrue), _actor->getBoundWorldInitialAngle(btrue), _useScale ? _actor->getWorldInitialScale() : Vec2d::One, _useFlip && _actor->getWorldInitialFlip());
    }

    ITF_INLINE Transform3d getLocalTransform( const Actor* _actor, const bbool _useFlip = btrue, const bbool _useScale = bfalse )
    {
        return Transform3d(_actor->getBoundLocalPos(), _actor->getBoundLocalAngle(), _useScale ? _actor->getScale() : Vec2d::One, _useFlip && _actor->getLocalFlip());
    }

    ITF_INLINE Transform3d getLocalInitTransform( const Actor* _actor, const bbool _useFlip = btrue, const bbool _useScale = bfalse )
    {
        return Transform3d(_actor->getBoundLocalInitialPos(), _actor->getBoundLocalInitialAngle(), _useScale ? _actor->getLocalInitialScale() : Vec2d::One, _useFlip && _actor->getLocalInitialFlip());
    }

	void    getPickableMatrix(const Pickable* _pickable, Matrix44 &_matrix);

    void    setColorAndAlpha(const Actor* _actor, bbool _setColor, const Color &_color, bbool _setAlpha, f32 _alpha);

    void    groupResetTransformationToInitial(Pickable* _pickable);
};

#endif // _ITF_ACTORUTILS_H_
