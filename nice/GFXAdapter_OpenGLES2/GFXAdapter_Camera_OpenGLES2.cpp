#include "precompiled_GFXAdapter_OpenGLES2.h"

#ifndef _ITF_OPENGLES2_DRAWPRIM_H_
#include "adapters/GFXAdapter_OpenGLES2/GFXAdapter_OpenGLES2.h"
#endif //_ITF_OPENGLES2_DRAWPRIM_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#include "core/math/SIMD/SIMD_Quaternion.h"

namespace ITF
{
	// ------------------------------------------------------------------------------

	void GFXAdapter_OpenGLES2::updateCameraFrustumPlanes(Camera *_cam)
	{
		extractFrustumPlanes(m_worldViewProj.getViewProj(), _cam);
	}

} // namespace ITF
