#ifndef _ITF_JOBUPDATEPARTICLE_H_
#define _ITF_JOBUPDATEPARTICLE_H_


namespace ITF
{
class ITF_ParticleGenerator;

class JobUpdateParticleContext
{
public:
    JobUpdateParticleContext(ITF_ParticleGenerator* _pParticleGenerator,f32 _deltaTime,bbool _bDisableCreation):
     m_deltaTime(_deltaTime),m_disableCreation(_bDisableCreation),m_pParticleGenerator(_pParticleGenerator)
     {

     }

     JobUpdateParticleContext():m_pParticleGenerator(NULL) {};

     f32                       m_deltaTime;//read
     bbool                     m_disableCreation;//read
     ITF_ParticleGenerator*    m_pParticleGenerator;//read-write
};


class JobUpdateParticle :public Job
{
public:
    JobUpdateParticle() { setAsSynchronizable(btrue); };
    JobUpdateParticle(JobUpdateParticleContext& _context):m_context(_context)
    {

    }

    void setContext(const JobUpdateParticleContext& _context) {m_context = _context;}
    static u32 _jobTag_UpdateParticle;
    virtual void synchronize();
    virtual u32 getTag() const {return _jobTag_UpdateParticle;}
    void execute(bbool _immediate);

protected:
    JobUpdateParticleContext m_context;
};



extern void mainJobUpdateParticle(JobUpdateParticleContext* _context,bbool _immediate);
}


#endif //_ITF_JOBUPDATEPARTICLE_H_