#ifndef _ITF_JOBUPDATEVISUAL_H_
#define _ITF_JOBUPDATEVISUAL_H_


namespace ITF
{

class AnimLightComponent;

class JobUpdateVisualContext
{
public:
    JobUpdateVisualContext():m_pAnimLightComponent(NULL) {};
    JobUpdateVisualContext(AnimLightComponent*  _pAnimLightComponent, bbool _updateBones, bbool _updatePatches)
        : m_pAnimLightComponent(_pAnimLightComponent)
        , m_updateBones(_updateBones)
        , m_updatePatches(_updatePatches)
    {

    }

    AnimLightComponent*  m_pAnimLightComponent;//read-write
    bbool                m_updateBones;
    bbool                m_updatePatches;
};


class JobUpdateVisual :public Job
{
public:
    JobUpdateVisual() {};

    JobUpdateVisual(JobUpdateVisualContext& _context):m_context(_context)
    {

    }


    void setContext(const JobUpdateVisualContext& _context) {m_context = _context;}

    static u32 _jobTag_UpdateVisual;
    virtual u32 getTag() const {return _jobTag_UpdateVisual;}
    void execute(bbool _immediate);

protected:
    JobUpdateVisualContext m_context;

};



extern void mainJobUpdateVisuals(JobUpdateVisualContext* _context);
extern void mainJobUpdateVisuals(AnimLightComponent*  _pAnimLightComponent, bbool _updateBones, bbool _updatePatches);
}


#endif //_ITF_JOBUPDATEVISUAL_H_