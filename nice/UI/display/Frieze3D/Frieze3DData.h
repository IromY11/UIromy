#ifndef _ITF_FRISE3DDATA_H_
#define _ITF_FRISE3DDATA_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_MESH3DRESOURCE_H_
#include "engine/display/Mesh3DResource.h"
#endif // _ITF_MESH3DRESOURCE_H_

//#define UAF_FRIEZE3D_PROFILING

namespace ITF
{
	// TODO Use the existing UAF bezier curves (BezierCurve)
	struct BezierCurveFrieze3D
	{
		static const u32 NB_DIST_SAMPLES = 20;

		Vec2d p0;
		Vec2d p1;
		Vec2d p2;
		Vec2d p3;
		f32 length;
		f32 distSamples[NB_DIST_SAMPLES];

		Vec2d getPointAtT(f32 t) const;
		Vec2d getTangentAtT(f32 t) const;
		Vec2d getNormalAtT(f32 t) const;
		void computeCurveLength();
		AABB getBoundingBox(f32 t0 = 0.0f,f32 t1 = 1.0f) const;

		f32 getTforDist(f32 dist) const;
	};

	ITF_INLINE Vec2d BezierCurveFrieze3D::getNormalAtT(f32 t) const
	{
		return getTangentAtT(t).getPerpendicular();
	}

	class Frieze3DBuildData
	{
	public :

		Frieze3DBuildData() { reset(); }

		void reset();
		void generateBezier(const PolyPointList& _pointList);
		void generateCollision(PolyPointList& _polyPoints, bbool _flip = bfalse) const;

		const ITF_VECTOR<BezierCurveFrieze3D>& getBezierCurves() const	{ return m_bezierCurves; }
		f32 getCurveLength() const								{ return m_curveLength; }
		bbool isLooping() const									{ return m_loop; }

	private :
		
		static const u32 NB_COLLISIONS_SAMPLES_PER_METER;
		static const f32 COLLISION_ERROR;

		f32 m_curveLength;
		bbool m_loop;
		ITF_VECTOR<BezierCurveFrieze3D> m_bezierCurves;
	};

	class Frieze3DRuntimeData
	{
	public :

		static bbool drawBB;

		Frieze3DRuntimeData() { reset(); }

		void reset();
		void generate(const Frieze3DBuildData& frieze3DData, const Frieze3DConfig& config3D, const ITF_VECTOR<FriseTextureConfig>& textureConfig, bbool _orient);
		void render(const Frieze3DBuildData& _data, const ITF_VECTOR<View*>& _views, f32 depth, const ObjectRef& _ref,  const GFXPrimitiveParam& params);
		
		Matrix44 m_matrix;
		void updateMeshesMatrices();

		f32 getScaleMesh() const { return m_scaleMesh; }
		f32 getLocalMinZ() const { return m_localMinZ; } // TODO Change that
		f32 getLocalMaxZ() const { return m_localMaxZ; }

		AABB m_aabbWorld;
        AABB m_aabbLocal;

		bbool isGlobalBBRecomputationNeeded() const	{ return m_globalBBRecomputeFlag; }
		void askForGlobalBBRecomputation()			{ m_globalBBRecomputeFlag = btrue; }

		bbool useNormalOrientation() const			{ return m_orient; }

	private :

#ifdef UAF_FRIEZE3D_PROFILING
		struct Profiler
		{
			u32 nbMeshRendered;
			u32 nbMeshGenerated;
			u32 nbMeshDisposed;
			u32 nbBBRefined;
		};
		Profiler profiler;
#endif

		struct RefMesh
		{
			RefMesh() { reset(); }

			void reset();
			void generate(Mesh3D* mesh3D);

			f32 getLength() const		{ return m_aabb.getMax().x() - m_aabb.getMin().x(); }
			f32 getScaledLength() const	{ return m_scaledLength; }
			f32 getHeight() const		{ return m_aabb.getMax().y() - m_aabb.getMin().y(); }
			f32 getMinZ() const			{ return m_aabb.getMin().z(); }
			f32 getMaxZ() const			{ return m_aabb.getMax().z(); }	

			void setScale(f32 _scale)	{ m_scaledLength = getLength() * _scale; }

			ITF_VECTOR<VertexPNCT> m_vertices;
			AABB3d m_aabb;
			f32 m_scaledLength;

			bbool m_valid;
		};

		struct Frieze3DMesh
		{
			Frieze3DMesh(
				const RefMesh * _refMeshPtr
#ifdef UAF_FRIEZE3D_PROFILING
		Profiler& _profiler
#endif
				) 
				: m_refMeshPtr(_refMeshPtr)
				, roughAABB(btrue)
				, generated(bfalse)
				, startingBezierIdx(0)
				, startingBezierOffset(0.0f)
#ifdef UAF_FRIEZE3D_PROFILING
				, profiler(_profiler)
#endif 
			{}

			~Frieze3DMesh()
			{
				if (generated)
					disposeMesh();	
			}

			void initIndexBuffer(Mesh3DResource* meshRes, const ITF_VECTOR<FriseTextureConfig>& textureConfig);
			bbool update(const ITF_VECTOR<Camera*>& _cameras, const Frieze3DBuildData& bData, Frieze3DRuntimeData& rtData);
			void render(const ITF_VECTOR<View*>& _views, f32 depth, const ObjectRef& _ref, const GFXPrimitiveParam& params);
			
			void computeWorldBB();

			const RefMesh * m_refMeshPtr; 

			Mesh3DPrim m_mesh;
			AABB3d m_aabbLocal;
			AABB3d m_aabbWorld;

			bbool roughAABB;
			bbool generated;

			u32 startingBezierIdx;
			f32 startingBezierOffset;

#ifdef UAF_FRIEZE3D_PROFILING
			Profiler& profiler;
#endif

		private :

			void refineAABB(const Frieze3DBuildData& bData, Frieze3DRuntimeData& rtData);
			void generateMesh(const Frieze3DBuildData& bData, Frieze3DRuntimeData& rtData);
			void disposeMesh();		
		};

		void computeZBounds();
		
		f32 m_localMinZ;
		f32 m_localMaxZ;

		RefMesh m_refMeshFill;	// fill mesh
		RefMesh m_refMeshLeft;	// left extremity
		RefMesh m_refMeshRight; // right extremity
				
		ITF_VECTOR<Frieze3DMesh> m_meshes;

		f32 m_scaleMesh; // TODO Remove that
		bbool m_orient;

		bbool m_globalBBRecomputeFlag;
	};
}

#endif // _ITF_FRISE3DDATA_H_ 
