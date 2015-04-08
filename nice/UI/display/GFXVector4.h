#ifndef ITF_GFX_VECTOR4_H_
#define ITF_GFX_VECTOR4_H_

namespace ITF
{

    class GFX_Vector4
    {
        DECLARE_SERIALIZE()

    public:
        f32& x() {return m_x;}
        f32& y() {return m_y;}
        f32& z() {return m_z;}
        f32& w() {return m_w;}

        const f32& x() const {return m_x;}
        const f32& y() const {return m_y;}
        const f32& z() const {return m_z;}
        const f32& w() const {return m_w;}

        f32 m_x, m_y, m_z, m_w;

        GFX_Vector4() : m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(0.0f)  {}
        
        GFX_Vector4(const GFX_Vector4& src)
            :   m_x(src.m_x)
            ,   m_y(src.m_y)
            ,   m_z(src.m_z)
            ,   m_w(src.m_w)
        {
        }


        GFX_Vector4(const f32 x, const f32 y, const f32 z, const f32 w)
            :   m_x(x)
            ,   m_y(y)
            ,   m_z(z)
            ,   m_w(w)
        {
        }

        const GFX_Vector4& operator = (const GFX_Vector4& src)
        {
            m_x = src.m_x;
            m_y = src.m_y;
            m_z = src.m_z;
            m_w = src.m_w;
            return *this;
        }

        bool operator == (const GFX_Vector4& rh) const
        {
            return m_x == rh.m_x
                && m_y == rh.m_y
                && m_z == rh.m_z
                && m_w == rh.m_w;
        }

        void set(const f32 x, const f32 y, const f32 z, const f32 w)
        {
            m_x = x;
            m_y = y;
            m_z = z;
            m_w = w;
        }

        void set(const Vec3d &_vec, const f32 w)
        {
            m_x = _vec.x();
            m_y = _vec.y();
            m_z = _vec.z();
            m_w = w;
        }

        void set(const Vec2d &_vec0, const Vec2d &_vec1)
        {
            m_x = _vec0.x();
            m_y = _vec0.y();
            m_z = _vec1.x();
            m_w = _vec1.y();
        }

        void set(const Vec2d &_vec0, const f32 z, const f32 w)
        {
            m_x = _vec0.x();
            m_y = _vec0.y();
            m_z = z;
            m_w = w;
        }

        void set(const Color &_col)
        {
            m_x = _col.getRed();
            m_y = _col.getGreen();
            m_z = _col.getBlue();
            m_w = _col.getAlpha();
        }

        void mul(const Color &_col, f32 _multiplier)
        {
            m_x = _col.getRed() * _multiplier;
            m_y = _col.getGreen() * _multiplier;
            m_z = _col.getBlue() * _multiplier;
            m_w = _col.getAlpha() * _multiplier;
        }

		void operator*= (f32 _multiplier)
		{
            m_x *= _multiplier;
            m_y *= _multiplier;
            m_z *= _multiplier;
            m_w *= _multiplier;
		}

		void normalize3D()
		{
			f32 invNorm = f32_InvSqrt(m_x*m_x+m_y*m_y+m_z*m_z);
            m_x *= invNorm;
            m_y *= invNorm;
            m_z *= invNorm;
		}
		void normalize4D()
		{
			f32 invNorm = f32_InvSqrt(m_x*m_x+m_y*m_y+m_z*m_z+m_w*m_w);
            m_x *= invNorm;
            m_y *= invNorm;
            m_z *= invNorm;
            m_w *= invNorm;
		}
    };

} // namespace ITF

#endif // ITF_GFX_VECTOR4_H_
