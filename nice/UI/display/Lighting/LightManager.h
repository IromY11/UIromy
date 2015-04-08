#ifndef _ITF_LIGHTMANAGER_H_
#define _ITF_LIGHTMANAGER_H_

#if (GFX_USE_LIGHTINGMODEL == 0)
#include "engine/display/Lighting/LightManager_Model0.h"
#elif (GFX_USE_LIGHTINGMODEL == 1)
#include "engine/display/Lighting/LightManager_Model1.h"
#elif (GFX_USE_LIGHTINGMODEL == 2)
#include "engine/display/Lighting/LightManager_Model2.h"
#endif

namespace ITF
{
	// Light manager class
	class GFX_LightManager : public GFX_LightManager_Implementation
	{
	public:

		///// constructor/destructor --------------------------------------------------------------
		GFX_LightManager(GFXAdapter *_adapter);
		~GFX_LightManager();
	};

} // namespace ITF

#endif // _ITF_NORMALMAPLIGHTMANAGER_H_
