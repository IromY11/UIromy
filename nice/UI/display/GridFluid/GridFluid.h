#ifndef _ITF_GRIDFLUID_H_
#define _ITF_GRIDFLUID_H_

#ifdef GFX_USE_GRID_FLUIDS
	#ifndef _ITF_BASEOBJECT_H_
	#include "core/baseObject.h"
	#endif //_ITF_BASEOBJECT_H_

	#ifndef _ITF_RESOURCE_H_
	#include "engine/resources/resource.h"
	#endif //_ITF_RESOURCE_H_

	#ifndef _ITF_KEYARRAY_H_
	#include "core/container/KeyArray.h"
	#endif //_ITF_KEYARRAY_H_

	#ifndef _ITF_GFX_ADAPTER_H_
	#include "engine/AdaptersInterfaces/GFXAdapter.h"
	#endif //_ITF_GFX_ADAPTER_H_

	#define float4 ITF::GFX_Vector4
	#include "../bin/Shaders_dev/Unified/ShaderParameters.fxh" 
	#undef float4

	#ifndef ITF_ENGINE_QUAD3DPRIM_H_
	#include "engine/display/Primitives/Quad3DPrim.h"
	#endif //ITF_ENGINE_QUAD3DPRIM_H_

    #ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
    #include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
    #endif //_ITF_GFXADAPTER_SHADERMANAGER_H_

    #ifndef ITF_GFX_ZLIST_H_
    #include "engine/display/GFXZList.h"
    #endif //ITF_GFX_ZLIST_H_

    #ifndef ITF_GFX_CONTEXT_H_
    #include "engine/display/GFXContext.h"
    #endif //ITF_GFX_CONTEXT_H_

	class GFX_Vector4;
	class GFXAdapter;
	class RenderPassContext;
	class RenderTarget;
#endif


namespace ITF
{
	class GFXAdapter_shaderManager;
	class PolyLine;
	class AABB;
	class GFXPrimitive;

    enum GFX_GridModifierMode
    {
        GFX_GRID_MOD_MODE_FLUID = 0x1,
        GFX_GRID_MOD_MODE_FORCE = 0x2,
        GFX_GRID_MOD_MODE_BLOCKER = 0x4,
        GFX_GRID_MOD_MODE_FLUID_FORCE = GFX_GRID_MOD_MODE_FLUID|GFX_GRID_MOD_MODE_FORCE,
        GFX_GRID_MOD_MODE_FLUID_DEBUG = 0x8,
        GFX_GRID_MOD_MODE_FORCE_DEBUG = 0x10,
        GFX_GRID_MOD_MODE_ATTRACT = 0x20,
    };

#ifdef GFX_USE_GRID_FLUIDS
	enum GridFluid_RenderMode
	{
		GRDFLD_FluidTransp,
		GRDFLD_FluidOpaque,
		GRDFLD_FluidOpaqueEmitter,
		GRDFLD_Velocity,
		GRDFLD_VelocityEmitter,
		GRDFLD_Pressure,
		ENUM_FORCE_SIZE_32(GridFluid_RenderMode)
	};

    #define SERIALIZE_ENUM_GFX_GRID_RENDER_MODE(name, val) \
    SERIALIZE_ENUM_BEGIN(name, val); \
        SERIALIZE_ENUM_VAR(GRDFLD_FluidTransp); \
        SERIALIZE_ENUM_VAR(GRDFLD_FluidOpaque); \
        SERIALIZE_ENUM_VAR(GRDFLD_FluidOpaqueEmitter); \
        SERIALIZE_ENUM_VAR(GRDFLD_Velocity); \
        SERIALIZE_ENUM_VAR(GRDFLD_VelocityEmitter); \
        SERIALIZE_ENUM_VAR(GRDFLD_Pressure); \
    SERIALIZE_ENUM_END()

    #define SERIALIZE_GFX_GRID_FILTER(name, val) SERIALIZE_MEMBER(name, val.m_filter)

	class GFX_GridFluidFilter
	{
	public:
		GFX_GridFluidFilter()
		{
			m_filter = 0x0000ffff;
		}
		GFX_GridFluidFilter(u32 _filter)
		{
			m_filter = _filter;
		}
		inline bool operator==(const GFX_GridFluidFilter &other) const
		{
			return m_filter == other.m_filter;
		}
		inline void operator=(const GFX_GridFluidFilter &other)
		{
			m_filter = other.m_filter;
		}
        inline u32 getValue() const
        {
            return m_filter;
        }
	public:
		u32 m_filter;
	};

	class GF_TEXTURE_RESSOURCE
	{
	public:
		GF_TEXTURE_RESSOURCE(Path& _texturePath, ResourceID& _textureRessource)
		{
			m_texturePath = &_texturePath;
			m_textureRessource = &_textureRessource;
		}
		Path			*m_texturePath;
		ResourceID		*m_textureRessource;
	};

	class GFX_GridFluidNoise : public BaseObject
	{
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(GFX_GridFluidNoise, BaseObject, ITF_GET_STRINGID_CRC(GFX_GridFluidNoise,877909439))
	public:
		GFX_GridFluidNoise();

		void setupNoise(GFXAdapter * _gfxDeviceAdapter, f32 _time, f32 _dt, shaderDefineKey &vsDefine, shaderDefineKey &psDefine,
			            GFX_Vector4 *_psConst, const Vec2d &_worldUVOffset);

		Path			m_texturePath;
        ResourceID		m_textureRessource;
		f32				m_scaleX;
		f32				m_scaleY;
		f32				m_intensity;
		f32				m_freq;
		f32				m_speedX;
		f32				m_speedY;
		f32				m_modulation;
	};

	class GFX_GridFluidFlowTex : public BaseObject
	{
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(GFX_GridFluidFlowTex, BaseObject, ITF_GET_STRINGID_CRC(GFX_GridFluidFlowTex,1825664174))
	public:
		GFX_GridFluidFlowTex();

		void setupFlowTex(GFXAdapter * _gfxDeviceAdapter, f32 _time, shaderDefineKey &vsDefine, shaderDefineKey &psDefine, GFX_Vector4 *_vsConst, GFX_Vector4 *_psConst, const Vec2d &_worldUVOffset);

		Path			m_texturePath;
        ResourceID		m_textureRessource;
		f32				m_deformation;
		f32				m_intensity;
		f32				m_scaleX1;
		f32				m_scaleY1;
		f32				m_scaleX2;
		f32				m_scaleY2;
		f32				m_scaleX3;
		f32				m_scaleY3;
		f32				m_speedX1;
		f32				m_speedY1;
		f32				m_speedX2;
		f32				m_speedY2;
		f32				m_speedX3;
		f32				m_speedY3;
		f32				m_rgbMultiplier;
		f32				m_alphaMultiplier;
		f32				m_densityFactor;
		f32				m_velocityPower;
	};

	class GFX_GridFluidDuDvTex : public BaseObject
	{
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(GFX_GridFluidDuDvTex, BaseObject, ITF_GET_STRINGID_CRC(GFX_GridFluidDuDvTex,1040441632))
	public:
		GFX_GridFluidDuDvTex();

		void setupDuDvTex(GFXAdapter * _gfxDeviceAdapter, f32 _time, shaderDefineKey &vsDefine, shaderDefineKey &psDefine, GFX_Vector4 *_vsConst, GFX_Vector4 *_psConst, const Vec2d &_worldUVOffset);

		Path			m_texturePath;
        ResourceID		m_textureRessource;
		f32				m_intensity;
		f32				m_scaleX1;
		f32				m_scaleY1;
		f32				m_speedX1;
		f32				m_speedY1;
	};

	class GFX_GridFluidAdditionnalRender : public BaseObject
	{
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(GFX_GridFluidAdditionnalRender, BaseObject, ITF_GET_STRINGID_CRC(GFX_GridFluidAdditionnalRender,3505192833))
	public:
		GFX_GridFluidAdditionnalRender();

		Vec3d					m_offset;
        Color					m_fluidCol;
		GFX_GridFluidFlowTex	m_flowTexture;
		GFX_GridFluidDuDvTex	m_dudvTexture;
		GFX_BLENDMODE			m_blendMode;
		Path					m_colorTexPath;
        ResourceID				m_colorTexRessource;
	};

	class GFX_GridFluidEmitterFactors : public BaseObject
	{
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(GFX_GridFluidEmitterFactors, BaseObject, ITF_GET_STRINGID_CRC(GFX_GridFluidEmitterFactors,3374429576))
	public:
		GFX_GridFluidEmitterFactors();

		f32 m_externalForce;
		f32 m_externalFluid;
		f32 m_externalPrimitive;
	};

	enum GFX_GridFluid_RequestStatus
	{
		GRD_FLD_REQUEST_PROCESSED,
		GRD_FLD_REQUEST_NOT_FOUND,
		GRD_FLD_REQUEST_NOT_PROCESSED,
		GRD_FLD_REQUEST_GPU_COMPUTING,
	};

#define GFX_GridFluid_CRC		ITF_GET_STRINGID_CRC(GFX_GridFluid, 1183879228)
	class GFX_GridFluid : public GFXPrimitive, public IRTTIObject
	{
	protected:
		class Request
		{
		public:
			u32							m_id;
			f32							m_result;
			GFX_GridFluid_RequestStatus m_status;
			Vec2d						m_pos;
			Vec2d						m_halfWidth;
			Vec2d						m_halfHeight;
			RenderTarget *				m_target;
		};
    public:
        DECLARE_OBJECT_CHILD_RTTI(GFX_GridFluid, IRTTIObject, GFX_GridFluid_CRC);
        DECLARE_SERIALIZE()

		static const u32 TextureSampler0 = 0;
		static const u32 FluidSampler = 0;
		static const u32 BoundariesSampler = 1;
		static const u32 BlurBoundSampler = 2;
		static const u32 VelocitySampler = 3;
		static const u32 DivFieldSampler = 4;
		static const u32 PressureSampler = 5;
		static const u32 NoiseSampler = 6;
		static const u32 JacobiBSampler = 0;
		static const u32 JacobiXSampler = 1;
		static const u32 DuDvSampler = 5;
		static const u32 EmitterSampler = 6;
		static const u32 MaskSampler = 7;
		static const u32 FlowSampler = 8;
		static const u32 ColorSampler = 9;
		static const u32 AttractSpeedSampler = 1;
		static const u32 AttractSourceSampler = 2;

        ///// constructor ----------------------------------------------------------
        GFX_GridFluid();
        ~GFX_GridFluid();

        ///// members --------------------------------------------------------------
		virtual void prerendering(GFXAdapter * _gfxDeviceAdapter, f32 _dt, u32 _zListViewID);
        virtual void directDraw(const RenderPassContext & _rdrPassCtxt, GFXAdapter * _gfxDeviceAdapter, f32 _Z = 0.0f, f32 _ZWorld = 0.0f, f32 _ZAbs = 0.0f);
		u32 getPassFilterFlag() const { return 0; }

		// Get list of texture ressources adresses.
		void getTextureRessourceList( ITF_VECTOR<GF_TEXTURE_RESSOURCE> &_list );

		// Request a density computation. Return an id allowing to retrieve result next frame.
		u32 sendRequestDensity(const Vec2d &_pos, const Vec2d &_halfWidth, const Vec2d &_halfHeight);

		// Retrieve request result.
		GFX_GridFluid_RequestStatus getRequestDensity( u32 _id, f32 &_result );

		// Delete a request.
		void deleteRequest( u32 _id );

		// Delete all request.
		void deleteAllRequest();

	protected:
		void computeRoundedCoord();
		void releaseTargets();
		void initTargets(GFXAdapter * _gfxDeviceAdapter);
		void clearTargets(GFXAdapter * _gfxDeviceAdapter);
		void clearOneTarget(GFXAdapter * _gfxDeviceAdapter, RenderTarget *_target, const GFX_Vector4 &_color);
		void begin2DRendering(GFXAdapter * _gfxDeviceAdapter);
		void end2DRendering(GFXAdapter * _gfxDeviceAdapter);
		void drawQuad(GFXAdapter * _gfxDeviceAdapter, shaderEntryKey _entryKeyVS, shaderEntryKey _entryKeyPS, RenderTarget *&_target, bbool _bilinear,
			          i32 _targetSampler = -1, shaderDefineKey _vsDefine = 0, shaderDefineKey _psDefine = 0, f32 _shiftU = 0.0f, f32 _shiftV = 0.0f,
					  GFX_TEXADRESSMODE _addressModeU = GFX_TEXADRESSMODE_CLAMP, GFX_TEXADRESSMODE _addressModeV = GFX_TEXADRESSMODE_CLAMP);
		void drawSprite(GFXAdapter * _gfxDeviceAdapter, const Matrix44 &_World, f32 _px, f32 _py, f32 _width, f32 _height );

		void setupTarget(GFXAdapter * _gfxDeviceAdapter, RenderTarget *_target);
		void renderModifierList(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists, ux _maskMode, RenderTarget *_target, shaderEntryKey _shaderPS, bbool _bilinear);
		void renderPolyLines(GFXAdapter * _gfxDeviceAdapter);
		void drawOpenPolyline(GFXAdapter * _gfxDeviceAdapter, class PolyLine *polyLine );
		bbool drawClosePolyline(GFXAdapter * _gfxDeviceAdapter, class PolyLine *polyLine );
		void renderEmitterDebug(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists);
		void renderFluidPrimitives(GFXAdapter * _gfxDeviceAdapter, ux _maskMode);
		void injectFluid(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists);
		void perturbVelocity(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists);
		void updateArbitraryBoundary(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists);
		void renderAttractors(GFXAdapter * _gfxDeviceAdapter, f32 _dt, GFX_Zlist<GFX_GridFluidModifierList> &_modifierLists);
		void updateOffset(GFXAdapter * _gfxDeviceAdapter);
		void moveTextures(GFXAdapter * _gfxDeviceAdapter);
		void updateVelocity(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		void advectVelocity(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		void diffuseVelocity(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		void computeDivergence(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		void updatePressure(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		void subtractPressureGradient(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		void advectFluid(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		void diffuseFluid(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		void blurTarget(GFXAdapter * _gfxDeviceAdapter, RenderTarget *_target, RenderTarget *_source, f32 _intensity, u32 nbIter, f32 threshold);
		void applyAttractSpeed(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		void applyAttractColor(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		void freeTempTarget(GFXAdapter * _gfxDeviceAdapter);
		void processRequests(GFXAdapter * _gfxDeviceAdapter);
		void sendGPURequest(GFXAdapter * _gfxDeviceAdapter, Request &request);
		void retrieveGPURequest(GFXAdapter * _gfxDeviceAdapter, Request &request);

		bbool isEmitterAccepted(const GFX_GridFluidFilter &_emitterFilter) const
		{
			return (_emitterFilter.getValue() & m_rejectFilter.getValue()) == 0 && (_emitterFilter.getValue() & m_requiredFilter.getValue()) == m_requiredFilter.getValue();
		}

    public:
		// Position of the fluid.
		Vec3d					m_boxPos;
		Vec3d					m_roundedPos;
		Vec3d					m_prevRoundedPos;
		Vec2d					m_worldUVOffset;
		// Offset in texel in textures from last move.
		Vec2d					m_texelOffset;
		// Size of the fluid in the world.
		Vec2d					m_boxSize;
		// Particle grid size (final texture size).
		u16						m_particleTexSizeX;
		u16						m_particleTexSizeY;
		// Speed grid size (simulation texture size).
		u16						m_speedTexSizeX;
		u16						m_speedTexSizeY;

		GFX_GridFluidFilter		m_requiredFilter;
		GFX_GridFluidFilter		m_rejectFilter;

		// Weight.
		f32						m_weight;
		f32						m_viscosity;
		f32						m_fluidDiffusion;
		f32						m_FluidLoss;
		f32						m_VelocityLoss;
		f32						m_pressureDiffusion;
		f32						m_pressureIntensity;
		f32						m_attractForce;
		f32						m_attractExpand;
		f32						m_attractColorize;
		GridFluid_RenderMode	m_renderMode;
		bbool					m_reinit;
		GFX_GridFluidNoise		m_fluidNoise;
		GFX_GridFluidNoise		m_velocityNoise;
		GFX_GridFluidNoise		m_attractNoise;
		bbool					m_active;
		bbool					m_pause;
		// Use RGB fluid.
		bbool					m_useRGBFluid;
		Color					m_neutralColor;
		u32                     m_nbIter;

		Path					m_maskPath;
        ResourceID				m_maskRessource;
		GFX_GridFluidAdditionnalRender m_mainRender;
		GFX_GridFluidEmitterFactors	m_emitterFactors;

		ITF_VECTOR<GFX_GridFluidAdditionnalRender> m_addRenderList;
		ITF_VECTOR<Request>     m_requestList;
		u32						m_lastRequestId;

	protected:
		f32						m_time;
		bbool					m_useAttract;
		bbool					m_wasUsingAttract;
		// Fluid textures.
		RenderTarget *			m_velocityTarget;  // xy texture
		RenderTarget *			m_boundariesTarget; // x texture
		RenderTarget *			m_pressureTarget; // x texture
		RenderTarget *			m_divFieldTarget; // x texture
		RenderTarget *			m_fluidTarget; // x texture
		RenderTarget *			m_emitterTarget; // x texture for debug
		RenderTarget *			m_attractSourceTarget; // RGB texture
		RenderTarget *			m_attractSpeedTarget; // XY texture.
		// Current target size.
		static u32				m_curTargetSizeX, m_curTargetSizeY;
		// Previous target size.
		static u32				m_previousTargetSizeX, m_previousTargetSizeY;
        static RenderPassContext m_passCtx;
        static PrimitiveContext m_primitiveCxt;
        static DrawCallContext	m_drawCallCtx;

		// Bounding volume.
		AABB m_fluidAABB;

		// Current frame counter, allow to know if modifier list is up to date for this frame.
		u32 m_frameCount;
	};

    #define SERIALIZE_ENUM_GFX_GRID_MOD_MODE(name, val) \
    SERIALIZE_ENUM_BEGIN(name, val); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_MODE_FLUID); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_MODE_FORCE); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_MODE_BLOCKER); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_MODE_FLUID_FORCE); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_MODE_ATTRACT); \
    SERIALIZE_ENUM_END()

    enum GFX_GridModifierPulse
    {
        GFX_GRID_MOD_PULSE_NONE,
        GFX_GRID_MOD_PULSE_SINUS,
        GFX_GRID_MOD_PULSE_TRIANGLE1,
        GFX_GRID_MOD_PULSE_TRIANGLE2,
        GFX_GRID_MOD_PULSE_TRIANGLE3,
        GFX_GRID_MOD_PULSE_PALIER1,
        GFX_GRID_MOD_PULSE_PALIER2,
        GFX_GRID_MOD_PULSE_PALIER3,
    };

    #define SERIALIZE_ENUM_GFX_GRID_MOD_PULSE(name, val) \
    SERIALIZE_ENUM_BEGIN(name, val); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_PULSE_NONE); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_PULSE_SINUS); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_PULSE_TRIANGLE1); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_PULSE_TRIANGLE2); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_PULSE_TRIANGLE3); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_PULSE_PALIER1); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_PULSE_PALIER2); \
        SERIALIZE_ENUM_VAR(GFX_GRID_MOD_PULSE_PALIER3); \
    SERIALIZE_ENUM_END()

	class GFX_GridFluidModifier : public BaseObject
	{
		friend class GFX_GridFluid;
    public:
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(GFX_GridFluidModifier, BaseObject, ITF_GET_STRINGID_CRC(GFX_GridFluidModifier,3448460494))

        ///// constructor ----------------------------------------------------------
        GFX_GridFluidModifier();
        ~GFX_GridFluidModifier();

		// Is valid.
		bbool isValid(f32 _curTime) const
		{
			return ( m_textureRessource.isValid() && ( (m_lifeTime == 0.0f ) || (_curTime < m_lifeTime + m_startDelay) ) );
		}

		// Update.
		void update(f32 _curTime, const Vec3d &_speed, const Matrix44 &_matrix);

	protected:
		// Bounding volume.
		AABB m_AABB;

    public:
		// Position of the fluid.
		Vec3d			m_boxPos;
		// Size of the fluid in the world.
		Vec2d			m_boxSize;
		// Texture : force (rg), or fluid (r), or blocker (r)
        ResourceID		m_textureRessource;
		// Type of modifier : velocity/fluid/blocker
		GFX_GridModifierMode m_mode;
		// Force added to the texture.
        Vec2d			m_force;
		// Force texture amplitude.
		f32				m_forceTexFactor;
		// Intensity : one another multiplier.
		f32				m_intensity;
		// Color used for RGB grid.
		Color			m_fluidColor;
		// Active ?
		bbool			m_active;
		// Modulate color with parent speed.
		bbool			m_speedToForce;
		// Is an external modifier.
		bbool			m_isExternal;
		// Texture path.
		Path			m_texturePath;
		// Time and co
		f32				m_lifeTime;
		f32				m_startDelay;
		f32				m_fadeInLength;
		f32				m_fadeOutLength;
		// Scale at the initial and end time.
		Vec2d			m_scaleInit;
		Vec2d			m_scaleEnd;
		// Rotation at the initial and end time, in degree.
		f32				m_rotationInit;
		f32				m_rotationEnd;
		// Translation speed.
		Vec2d			m_speed;
		// Current state.
		f32				m_curIntensity;
		Vec2d			m_curScale;
		f32				m_curRotation;
		Vec2d			m_curTranslation;
		// Pulsation.
		GFX_GridModifierPulse m_pulseMode;
		f32				m_pulseFreq;
		f32				m_pulseCur;
		f32				m_pulsePhase;
		f32				m_pulseAmplitude;

		// Filter bitfield.
		GFX_GridFluidFilter	m_emitterFilter;
	};

	class GFX_GridFluidModifierList : public GFXObject, public BaseObject
	{
    public:
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(GFX_GridFluidModifierList, BaseObject, ITF_GET_STRINGID_CRC(GFX_GridFluidModifierList,4044229226))

        ///// constructor ----------------------------------------------------------
        GFX_GridFluidModifierList();
        ~GFX_GridFluidModifierList();

		// Is valid.
		bbool isValid() const;
		// Invalidate.
		ITF_INLINE void invalidate()
		{
			m_name.invalidate();
		}

		// Add empty element.
		ITF_INLINE void addOneModList()
		{
			GFX_GridFluidModifier mod;
			m_modList.push_back(mod);
		}

		ITF_INLINE u32 size()
		{
			return m_modList.size();
		}

		ITF_INLINE f32 getZ()
		{
			return ((f32*)&m_matrix)[14];
		}

		// Copy from other modifier list.
		void operator= (const GFX_GridFluidModifierList &_src);

		GFX_GridFluidModifier* operator [](int i) {return &m_modList[i];}

		// Update.
		void update(GFXAdapter * _gfxDeviceAdapter, f32 _dt);
		// Reinit modifiers : current time = 0.
		void reInit();
    public:
		// Modifier list name/id.
		StringID m_name;
		// Position, orientation, scale of the modifiers.
		Matrix44 m_matrix;
		// Modifiers list.
        ITF_VECTOR<GFX_GridFluidModifier>  m_modList;
		// Previous position.
		Vec3d m_prevPos;
		// Current time.
		f32	m_curTime;
		// Current frame counter, allow to know if modifier list is up to date for this frame.
		u32 m_frameCount;
		// Active ?
		bbool m_active;
	};

	// Grid fluid manager. Share render targets to the fluids.
	class GFX_GridFluidManager
	{
		friend class GFX_GridFluid;
		friend class GFX_GridFluidNoise;
		friend class GFX_GridFluidFlowTex;
		friend class GFX_GridFluidDuDvTex;
	public:

		///// constructor/destructor --------------------------------------------------------------
		GFX_GridFluidManager();
		~GFX_GridFluidManager();

		void releaseAllTargets();

		bbool loadShader(GFXAdapter_shaderManager& _shaderManager);
		void releaseShader(GFXAdapter_shaderManager& _shaderManager);

		// Get a temporary render target.
		RenderTarget *getTempTarget(ux _width, ux _height, Texture::PixFormat _format);

		// Store an unused temporary texture.
		void storeTempTarget(RenderTarget *_target);

		// Register a collision.
		void addPolyLine(PolyLine *_polyLine);
		// Unregister a collision.
		void removePolyLine(PolyLine *_polyLine);
		// Register primitive as fluid emitter.
		void batchFluidPrimitive(GFXPrimitive *_primitive, const AABB &_boundingVolume, f32 _depth, GFX_GridFluidFilter _filters, GFX_GridModifierMode _mode);
		// Clear fluid primitive list.
		void clearFluidPrimitiveList();
		// Set fluid states for a primitive rendering in a fluid.
		void setFluidStates(GFXAdapter * _gfxDeviceAdapter);
	protected:
		bbool			m_areDrawingFluidPrimitive;
		f32				m_primitiveFactor;

		struct PrimBatch
		{
			GFXPrimitive *m_primitive;
			AABB m_boundingVolume;
			f32 m_depth;
			GFX_GridFluidFilter m_filters;
			GFX_GridModifierMode m_mode;
		};
		// Temporary texture textures.
        ITF_VECTOR<RenderTarget *> m_tempTargetTable;
		// Batch a new fluid primitive.
        ITF_VECTOR<PrimBatch> m_fluidPrimitiveTable;
		// Grid Fluids fx
		shaderGroupKey     mc_shader_Fluids;
		shaderEntryKey     mc_entry_GF_PCT1_VS;
		shaderEntryKey     mc_entry_GF_PCT5_VS;
		shaderEntryKey     mc_entry_GF_Noise_VS;
		shaderEntryKey     mc_entry_GF_DrawFluid_VS;
		shaderEntryKey     mc_entry_GF_Blur_VS;
		shaderEntryKey     mc_entry_GF_CalculateOffsets_VS;
		shaderEntryKey     mc_entry_GF_AttractApplyColor_VS;
		shaderEntryKey     mc_entry_GF_Init_PS;
		shaderEntryKey     mc_entry_GF_CalculateOffsets_PS;
		shaderEntryKey     mc_entry_GF_UpdateVelocity_PS;
		shaderEntryKey     mc_entry_GF_AdvectFluid_PS;
		shaderEntryKey     mc_entry_GF_AdvectVelocity_PS;
		shaderEntryKey     mc_entry_GF_F4Jacobi_PS;
		shaderEntryKey     mc_entry_GF_F1Jacobi_PS;
		shaderEntryKey     mc_entry_GF_DivField_PS;
		shaderEntryKey     mc_entry_GF_SubtractPressureGradient_PS;
		shaderEntryKey     mc_entry_GF_DrawQuad_PS;
		shaderEntryKey     mc_entry_GF_DrawVelocity_PS;
		shaderEntryKey     mc_entry_GF_DrawBoundary_PS;
		shaderEntryKey     mc_entry_GF_DrawFluid_PS;
		shaderEntryKey     mc_entry_GF_Blur_PS;
		shaderEntryKey     mc_entry_GF_AttractCopy_PS;
		shaderEntryKey     mc_entry_GF_AttractComputeSpeed_PS;
		shaderEntryKey     mc_entry_GF_AttractApplySpeed_PS;
		shaderEntryKey     mc_entry_GF_AttractApplyColor_PS;
		shaderEntryKey     mc_entry_GF_RequestCopy_PS;

		shaderDefineKey    mc_define_GF_UseNoise;
		shaderDefineKey    mc_define_GF_UseFlow;
		shaderDefineKey    mc_define_GF_UseDuDv;
		shaderDefineKey    mc_define_GF_RenderMode0;
		shaderDefineKey    mc_define_GF_RenderMode1;
		shaderDefineKey    mc_define_GF_RenderMode2;
		shaderDefineKey    mc_define_GF_UseMask;
		shaderDefineKey    mc_define_GF_UseAlphaLerp;
		shaderDefineKey    mc_define_GF_UseColorTex;
		shaderDefineKey    mc_define_GF_UseRGBFluid;

		// List of collisions registered.
		ITF_VECTOR<PolyLine*> m_polyLineList;

		// For triangulation.
		ITF_VECTOR<Vec2d> m_pointList;
		ITF_VECTOR<u16> m_polygone;
	};
#else

	// Empty class
	class GFX_GridFluidFilter
	{
	public:
		GFX_GridFluidFilter()
		{
		}
		GFX_GridFluidFilter(u32 _filter)
		{
		}
		inline bool operator==(const GFX_GridFluidFilter &other) const
		{
			return true;
		}
		inline void operator=(const GFX_GridFluidFilter &other)
		{
		}
		inline u32 getValue() const
		{
			return 0;
		}
	};

	// Empty class
	class GFX_GridFluidManager
	{
	public:

		///// constructor/destructor --------------------------------------------------------------
		GFX_GridFluidManager() {}
		~GFX_GridFluidManager() {}

		void releaseAllTargets() {}

		bbool loadShader(GFXAdapter_shaderManager& _shaderManager) {return true;}
		void releaseShader(GFXAdapter_shaderManager& _shaderManager) {}

		// Register a collision.
		void addPolyLine(PolyLine *_polyLine) {}
		// Unregister a collision.
		void removePolyLine(PolyLine *_polyLine) {}
		// Register primitive as fluid emitter.
		void batchFluidPrimitive(GFXPrimitive *_primitive, const AABB &_boundingVolume, f32 _depth, GFX_GridFluidFilter _filters, GFX_GridModifierMode _mode) {}
		// Clear fluid primitive list.
		void clearFluidPrimitiveList() {}
		// Set fluid states for a primitive rendering in a fluid.
		void setFluidStates(GFXAdapter * _gfxDeviceAdapter) {}
	};

    #define SERIALIZE_GFX_GRID_FILTER(name, val)
#endif // GFX_USE_GRID_FLUIDS

} // namespace ITF

#endif // _ITF_GRIDFLUID_H_
