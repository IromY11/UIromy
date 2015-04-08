#ifndef _ITF_FLUIDSHAPER_H_
#define _ITF_FLUIDSHAPER_H_


namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Base class for fluid shapers used in strings
    class FluidShaper
    {
    public:
        FluidShaper() 
            : m_linearOffset(1.5f)
            , m_linearRadius(2.f)
            , m_yScale(1)
            , m_linearSpeed(10)
            , m_refCount(0)
            , m_isColored(btrue)
            , m_hasGameMaterial(btrue)
            , m_minAnnouncer(-FLT_MAX)
            , m_maxAnnouncer(FLT_MAX)
            , m_transitionLen(0.f) 
        {}
        virtual f32 getHeight(f32 _linearX, bbool &_found, f32 &_waveWeight, bbool &_hasColor, bbool &_hasGameMaterial) const = 0;
        f32 getHeight_2D(const Vec2d &_pos2d, bbool &_found, f32 &_waveWeight, bbool &_hasColor, bbool &_hasGameMaterial) const {return getHeight(project2DPos(_pos2d),_found,_waveWeight, _hasColor, _hasGameMaterial);}

        void setHasGameMaterial(bbool _hasGMT) {m_hasGameMaterial = _hasGMT;}

        bbool hasGameMaterial() const {return m_hasGameMaterial;}

        void setAnnouncerRange(f32 _minX, f32 _maxX) {m_minAnnouncer = _minX; m_maxAnnouncer = _maxX;}

        void setTransitionLen(f32 _linearLength) {m_transitionLen = _linearLength;}

        f32 getTransitionLen() const {return m_transitionLen;}

        f32 project2DPos(const Vec2d &_pos) const {return _pos.dot(m_projectionDir);}

        virtual void addTimeSlice(f32 _dt) 
        {
            m_linearOffset += _dt*m_linearSpeed;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void setProjectionDir(const Vec2d &_dir) {m_projectionDir = _dir;}
        const Vec2d &getProjectionDir() const {return m_projectionDir;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        virtual void setPosition(const Vec2d &_pos) {m_linearOffset = _pos.dot(m_projectionDir);}
        virtual void setYScale(f32 _yScale) {m_yScale = _yScale;}
        virtual void setRadius(f32 _radius) {m_linearRadius = _radius;}
        void setIsColored(bbool _isColored)  {m_isColored = _isColored;}
        bbool getIsColored() const  {return m_isColored;}
        f32 getRadius() const {return m_linearRadius;}
        f32 getYScale() const {return m_yScale;}
        void setLinearOffset(f32 _linearOffset) {m_linearOffset = _linearOffset;}
        f32 getLinearOffset() const {return m_linearOffset;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        void incrementRefCount() {m_refCount++;}
        void decrementRefCount() 
        {
            ITF_ASSERT_CRASH(m_refCount>0, "incorrect ref count");
            m_refCount--; 
            if (m_refCount==0) 
            {
                delete this;
            }
        }
    protected:
        virtual ~FluidShaper() {}

        f32 m_linearRadius;
        f32 m_yScale;
        f32 m_linearOffset;
        f32 m_linearSpeed;
        i32 m_refCount;
        bbool m_isColored;
        Vec2d m_projectionDir;
        bbool m_hasGameMaterial;
        f32 m_minAnnouncer, m_maxAnnouncer;
        f32 m_transitionLen;


    };
}

#endif
