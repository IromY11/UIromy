
#ifndef _ITF_COLORCOMPUTERCOMPONENT_H_
#define _ITF_COLORCOMPUTERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

class colorComputerComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(colorComputerComponent,ActorComponent,3495094582);

public:
    DECLARE_SERIALIZE()

    colorComputerComponent();
    ~colorComputerComponent();

    virtual bbool           needsUpdate() const { return bfalse; }
    virtual bbool           needsDraw() const { return bfalse; }
    virtual bbool           needsDraw2D() const { return bfalse; }
	virtual	bbool			needsDraw2DNoScreenRatio() const { return bfalse; }

#ifdef ITF_SUPPORT_EDITOR
    virtual void            drawEdit            ( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    const Color& getNearColor() {return m_farColor;}
    const Color& getFarColor() {return m_nearColor;}
    
    f32                     getFactorFromZ(f32 _z);

    void                    setId(u32 _id) { m_tagId = _id;}
    const u32               getTagId() const { return m_tagId;}

protected:
        
    f32                     m_near;
    f32                     m_far;
    Color                   m_nearColor;
    Color                   m_farColor;

    f32                     m_width;
    f32                     m_height;

    u32                     m_tagId;

private:

    ITF_INLINE const class colorComputerComponent_Template*  getTemplate() const;
};

class colorComputerComponent_Template : public ActorComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(colorComputerComponent_Template,ActorComponent_Template,1246862451);
    DECLARE_SERIALIZE()
    DECLARE_ACTORCOMPONENT_TEMPLATE(colorComputerComponent);

public:

    colorComputerComponent_Template();
    ~colorComputerComponent_Template();

private:

};


const colorComputerComponent_Template*  colorComputerComponent::getTemplate() const {return static_cast<const colorComputerComponent_Template*>(m_template);}
}

#endif // _ITF_COLORCOMPUTERCOMPONENT_H_

