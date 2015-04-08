#ifndef _ITF_UVDATA_H_
#define _ITF_UVDATA_H_


namespace ITF
{

    struct UVdata
    {
    private:
        ITF_INLINE void init(const Vec2d& _UV0, const Vec2d& _UV1)
        {
            m_UV.resize(2);
            m_UV[0] = _UV0;
            m_UV[1] = _UV1;
        }

    public:
        UVdata()                                                { init( Vec2d(0.f, 0.f), Vec2d(1.f, 1.f)); }
        UVdata(u32 /*_id*/)                                     { init( Vec2d(0.f, 0.f), Vec2d(1.f, 1.f)); }
        UVdata(u32 /*_id*/, Vec2d _UV0, Vec2d _UV1)             { init( _UV0, _UV1); }

        ITF_INLINE void setUV0(const Vec2d& _UV0)               { m_UV[0] = _UV0;};
        ITF_INLINE void setUV1(const Vec2d& _UV1)               { m_UV[1] = _UV1;};
        void setUV( u32 _index, const Vec2d& _UV )
        {
            while ( _index >= m_UV.size()) m_UV.push_back( Vec2d(0,0));
            m_UV[_index] = _UV;
        }

        ITF_INLINE const ITF_VECTOR<Vec2d>& getUVs() const      {return m_UV;};
        ITF_INLINE ITF_VECTOR<Vec2d>& getUVs()                  {return m_UV;};
        ITF_INLINE const Vec2d& getUV0() const                  {return m_UV[0];};
        ITF_INLINE const Vec2d& getUV1() const                  {return m_UV[1];};

        void serialize(ArchiveMemory& _pArchive)                { m_UV.serialize(_pArchive); }
        void setUVdata(const Vec2d& _UV0, const Vec2d& _UV1)    { init( _UV0, _UV1); }

    private:
        ITF_VECTOR<Vec2d> m_UV;
    };

    struct UVparameters
    {
    public:
        UVparameters() : m_Orientation(0.f), m_flag(String8::emptyString)
        {
        }

        struct Parameters
        {
            Parameters() {m_Weight = 1; m_depth = 0; }
            Parameters(f32 _weight, f32 _depth) {m_Weight = _weight; m_depth = _depth; }
            f32 m_Weight;
            f32 m_depth;

            void serialize(ArchiveMemory& _archive)
            {
                _archive.serialize(m_Weight);
                _archive.serialize(m_depth);
            }
        };

        void setWeight( u32 _index, f32 _weight )
        {
            while ( _index >= m_Params.size()) m_Params.push_back( Parameters());
            m_Params[_index].m_Weight = _weight;
        }

        void setDepth( u32 _index, f32 _depth )
        {
            while ( _index >= m_Params.size()) m_Params.push_back( Parameters());
            m_Params[_index].m_depth = _depth;
        }

        ITF_INLINE const ITF_VECTOR<Parameters>& getParams() const   {return m_Params;};
        ITF_INLINE const Parameters& getParamAt( u32 _index ) const {return m_Params[_index]; }

        struct Triangle
        {
            Triangle() { m_index[0] = m_index[1] = m_index[2] = 0; }
            int m_index[3];
            void serialize(ArchiveMemory& _archive)
            {
                _archive.serialize(m_index[0]);
                _archive.serialize(m_index[1]);
                _archive.serialize(m_index[2]);
            }
        };

        void setTriangle( u32 _index, int _index0, int _index1, int _index2 )
        {
            while ( _index >= m_triangles.size()) m_triangles.push_back( Triangle());
            m_triangles[_index].m_index[0] = _index0;
            m_triangles[_index].m_index[1] = _index1;
            m_triangles[_index].m_index[2] = _index2;
        }

        void setTriangle( u32 _index, const Vec3d &_V)
        {
            setTriangle( _index, (int) _V.getX(), (int) _V.getY(), (int) _V.getZ() );
        }

        ITF_INLINE const ITF_VECTOR<Triangle>& getTriangles() const   {return m_triangles;};
        ITF_INLINE const Triangle& getTriangleAt( u32 _index ) const {return m_triangles[_index]; }

        ITF_INLINE f32 getOrientation() const { return m_Orientation; }
        void setOrientation( f32 _Orientation ) { m_Orientation = _Orientation; }

        ITF_INLINE const String8 & getFlag() const { return m_flag; }
        void setFlag( const String8 & _flag ) { m_flag = _flag; }

        void serialize(ArchiveMemory& _pArchive)                
        { 
            _pArchive.serialize( m_Orientation );
            m_flag.serialize( _pArchive );
            m_Params.serialize(_pArchive); 
            m_triangles.serialize(_pArchive); 
        }

    private:
        ITF_VECTOR<Parameters>  m_Params;
        ITF_VECTOR<Triangle>    m_triangles;
        
        float                   m_Orientation;
        String8                 m_flag;
    };


}

#endif //_ITF_UVDATA_H_