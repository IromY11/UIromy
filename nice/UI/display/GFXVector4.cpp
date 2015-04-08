#include "precompiled_engine.h"

namespace ITF
{
    BEGIN_SERIALIZATION(GFX_Vector4)
        SERIALIZE_MEMBER("x", m_x);
        SERIALIZE_MEMBER("y", m_y);
        SERIALIZE_MEMBER("z", m_z);
        SERIALIZE_MEMBER("w", m_w);
    END_SERIALIZATION()

} // namespace ITF