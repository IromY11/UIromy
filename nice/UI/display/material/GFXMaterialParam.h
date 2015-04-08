#ifndef ITF_GFX_MATERIAL_PARAM_H_
#define ITF_GFX_MATERIAL_PARAM_H_

#ifndef ITF_TEMPLATEDATABASE_TYPES_H
#include "engine/TemplateManager/TemplateDatabase_types.h"
#endif // ITF_TEMPLATEDATABASE_TYPES_H

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

namespace ITF
{
    template <unsigned int F, unsigned int I, unsigned int V>
    class GFX_MaterialParamsT
    {
    public:

        GFX_MaterialParamsT()
        {
            fill(m_fparams, m_fparams + F, 0.f);
            fill(m_iparams, m_iparams + I, 0);
            fill(m_vparams, m_vparams + V, GFX_Vector4(0.f, 0.f, 0.f, 0.f));
        };

        bool operator == (const GFX_MaterialParamsT & _rhMatParams) const
        {
            return ITF::equal(m_fparams, m_fparams + F, _rhMatParams.m_fparams)
                && ITF::equal(m_iparams, m_iparams + I, _rhMatParams.m_iparams)
                && ITF::equal(m_vparams, m_vparams + V, _rhMatParams.m_vparams);
        }

        bool operator != (const GFX_MaterialParamsT & _rhMatParams) const { return ! operator == (_rhMatParams); }

        ITF_INLINE void setfParamsAt(u32 _at, f32 _val) {ITF_ASSERT(_at < F); m_fparams[_at] = _val;}
        ITF_INLINE f32 getfParamsAt(u32 _at) const {ITF_ASSERT(_at < F); return m_fparams[_at];}
        ITF_INLINE void setiParamsAt(u32 _at, i32 _val) {ITF_ASSERT(_at < I); m_iparams[_at] = _val;}
        ITF_INLINE i32 getiParamsAt(u32 _at) const {ITF_ASSERT(_at < I); return m_iparams[_at];}

        ITF_INLINE void setvParamsAt(u32 _at, const GFX_Vector4& _vec) {ITF_ASSERT(_at < V); m_vparams[_at] = _vec;}
        ITF_INLINE GFX_Vector4 getvParamsAt(u32 _at) const {ITF_ASSERT(_at < V); return m_vparams[_at];}
        ITF_INLINE void getvParamsAt(u32 _at, f32* _out) const {ITF_ASSERT(_at < V); _out[0] = m_vparams[_at].x();_out[1] = m_vparams[_at].y();_out[2] = m_vparams[_at].m_z;_out[3] = m_vparams[_at].m_w;}
        ITF_INLINE f32* getfParams() {return m_fparams;}
        ITF_INLINE i32* getiParams() {return m_iparams;}
    protected:
        f32         m_fparams[ F ];
        i32         m_iparams[ I ];
        GFX_Vector4 m_vparams[ V ];
    };

} // namespace ITF

#endif // ITF_GFX_MATERIAL_PARAM_H_
