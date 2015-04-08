#include "precompiled_engine.h"

#ifndef _ITF_FRISE3DDATA_H_
#include "engine/display/Frieze3D/Frieze3DData.h"
#endif // _ITF_FRISE3DDATA_H_ 

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif // _ITF_CAMERA_H_

namespace ITF
{
	const u32 Frieze3DBuildData::NB_COLLISIONS_SAMPLES_PER_METER = 4;
	const f32 Frieze3DBuildData::COLLISION_ERROR = 0.02f;
	
	Vec2d BezierCurveFrieze3D::getPointAtT(f32 t) const
	{
		f32 t2 = t * t;
		f32 t3 = t2 * t;
		f32 mt = 1.0f - t;
		f32 mt2 = mt * mt;
		f32 mt3 = mt2 * mt;
		return mt3 * p0 + (3.0f * mt2 * t) * p1 + (3.0f * mt * t2) * p2 + t3 * p3;	
	}

	Vec2d BezierCurveFrieze3D::getTangentAtT(f32 t) const
	{
		f32 mt = 1.0f - t;
		f32 C0 = 3.0f * mt * mt;
		f32 C1 = 6.0f * mt * t;
		f32 C2 = 3.0f * t * t;
		return ((-C0) * p0 + (C0 - C1) * p1 + (C1 - C2) * p2 + C2 * p3).normalize();
	}

	void BezierCurveFrieze3D::computeCurveLength()
	{	
		Vec2d lastPt = p0;
		length = 0.0f;
		for (u32 i = 0; i < NB_DIST_SAMPLES; ++i)
		{
			f32 t = (i + 1.0f) / NB_DIST_SAMPLES;
			Vec2d pt = getPointAtT(t);
			f32 dist = (pt - lastPt).norm();
			length += dist;
			distSamples[i] = length;
			lastPt = pt;			
		}	
	}

	f32 BezierCurveFrieze3D::getTforDist(f32 dist) const // TODO to optimize (dicho search ?)
	{
		if (dist >= length)
			return 1.0f;
		if (dist <= 0.0f)
			return 0.0f;
								
		f32 prevDist = 0.0f;
		for (int i = 0; i < NB_DIST_SAMPLES; ++i)
		{
			float tDist = distSamples[i];
			if (tDist >= dist)
			{
				f32 ratio = (dist - prevDist) / (tDist - prevDist);
				f32 nextT = (f32)(i + 1) / NB_DIST_SAMPLES;
				f32 prevT = (f32)i / NB_DIST_SAMPLES;
				return prevT + ratio * (nextT - prevT);
			}
			prevDist = tDist;
		}
			
		return 1.0f; // should never get there
	}

	Vec2d getQuadraticZeros(f32 a,f32 b,f32 c,f32 minT,f32 maxT)
	{
		f32 delta = b * b - 4 * a * c;
		Vec2d res(minT,maxT);
		if (delta > 0)
		{
			f32 sqrtDelta = f32_Sqrt(delta);
			f32 aaInv = 1.0f / (2.0f * a);
			res.set((-b - sqrtDelta) * aaInv,(-b + sqrtDelta) * aaInv);
		} 
		else if (delta == 0)
		{
			f32 resf = -b / (2.0f * a);
			res.set(resf,resf);
		}
		res = Vec2d::Clamp(res,Vec2d(minT,minT),Vec2d(maxT,maxT));
		return res;
	}

	AABB BezierCurveFrieze3D::getBoundingBox(f32 t0,f32 t1) const
	{
		AABB bb;
		const Vec2d A = 3.0f * (p3 + 3.0f * (p1 - p2) - p0);
		const Vec2d B = 3.0f * (p2 - 2.0f * p1 + p0);
		const Vec2d C = 3.0f * (p1 - p0);

		Vec2d xT = getQuadraticZeros(A.x(),B.x(),C.x(),t0,t1);
		Vec2d yT = getQuadraticZeros(A.y(),B.y(),C.y(),t0,t1);

		Vec2d bound0 = getPointAtT(t0);
		Vec2d bound1 = getPointAtT(t1);
		
		Vec2d xBound0 = getPointAtT(xT.x());
		Vec2d xBound1 = getPointAtT(xT.y());

		Vec2d yBound0 = getPointAtT(yT.x());
		Vec2d yBound1 = getPointAtT(yT.y());

		bb.setMinX(f32_Min(f32_Min(f32_Min(bound0.x(),bound1.x()),xBound0.x()),xBound1.x()));
		bb.setMaxX(f32_Max(f32_Max(f32_Max(bound0.x(),bound1.x()),xBound0.x()),xBound1.x()));
		bb.setMinY(f32_Min(f32_Min(f32_Min(bound0.y(),bound1.y()),yBound0.y()),yBound1.y()));
		bb.setMaxY(f32_Max(f32_Max(f32_Max(bound0.y(),bound1.y()),yBound0.y()),yBound1.y()));
		return bb;
	}

	void Frieze3DBuildData::reset()
	{
		m_curveLength = 0.0f;
		m_bezierCurves.clear();
		m_loop = bfalse;
	}

	struct PointData
	{
		Vec2d pos;
		Vec2d tangent;
		Vec2d sight;
		f32 length;
		f32 curvature;
	};

	void Frieze3DBuildData::generateBezier(const PolyPointList& _pointList)
	{
		reset();
		m_loop = _pointList.isLooping();

		u32 nbPoints = _pointList.getPosCount();

		const u32 MIN_NB_POINTS = m_loop ? 4 : 2;
		if (nbPoints < MIN_NB_POINTS)
			return;	

		ITF_VECTOR<PointData> pointData;
		pointData.reserve(nbPoints);

		// First fill in the points with position and curvature and check for merged points
		Vec2d oldPos(-FLT_MAX,-FLT_MAX);
		for (u32 i = 0; i < nbPoints; ++i)
		{
			if (oldPos != _pointList.getPosAt(i))
			{
				PointData point;
				point.pos = _pointList.getPosAt(i);
				point.curvature = _pointList.getPointScaleAt(i);
				oldPos = point.pos;
				pointData.push_back(point);
			}
		}

		nbPoints = pointData.size();
		if (nbPoints < MIN_NB_POINTS)
			return;

		// Then compute the direction and length of the edge and the tangents
		// First point
		PointData& firstPoint = pointData[0];
		firstPoint.sight = pointData[1].pos - firstPoint.pos;
		firstPoint.length = firstPoint.sight.norm();
		firstPoint.sight /= firstPoint.length;
		pointData[0].tangent = pointData[0].sight;

		for (u32 i = 1; i < nbPoints - 1; ++i)
		{
			PointData& point = pointData[i];
			point.sight = pointData[i + 1].pos - point.pos;
			point.length = point.sight.norm();
			point.sight /= point.length;
			point.tangent = (point.sight + pointData[i - 1].sight).normalize();
		}

		// tangent for extermities
		if (m_loop)
		{
			(pointData[0].tangent += pointData[nbPoints - 2].sight).normalize();	
			pointData[nbPoints - 1].tangent = pointData[0].tangent;
			pointData[nbPoints - 1].curvature = pointData[0].curvature;
		}
		else
			pointData[nbPoints - 1].tangent = pointData[nbPoints - 2].sight;

		// Compute bezier curves
		m_bezierCurves.reserve(nbPoints - 1);
		for (u32 i = 0; i < nbPoints - 1; ++i)
		{
			BezierCurveFrieze3D bezier;		
			const PointData& point = pointData[i];
			const PointData& nextPoint = pointData[i + 1];
			bezier.p0 = point.pos;
			bezier.p3 = nextPoint.pos;
			bezier.p1 = bezier.p0 + point.tangent * (point.curvature * point.length * 0.1f);
			bezier.p2 = bezier.p3 - nextPoint.tangent * (nextPoint.curvature * point.length * 0.1f);
			bezier.computeCurveLength();
			m_curveLength += bezier.length;
			m_bezierCurves.push_back(bezier);
		}
	}

	void Frieze3DBuildData::generateCollision(PolyPointList& _polyPoints, bbool _flip) const // TODO Handle different methods + game materials
	{		
		u32 nbCurves = m_bezierCurves.size();
		if (nbCurves == 0)
			return;

		u32 nbCollisionPoints = 1;
		for (u32 i = 0; i < nbCurves; ++i)
			nbCollisionPoints += std::max(1u,static_cast<u32>(NB_COLLISIONS_SAMPLES_PER_METER * m_bezierCurves[i].length));
		ITF_VECTOR<Vec2d> collisionPoints(nbCollisionPoints);
		
		if (!_flip)
		{
			u32 colIdx = 0;
			for (u32 i = 0; i < nbCurves; ++i)
			{
				u32 nbCollisions = std::max(1u,static_cast<u32>(NB_COLLISIONS_SAMPLES_PER_METER * m_bezierCurves[i].length));
				for (u32 j = 0; j < nbCollisions; ++j)
					collisionPoints[colIdx++] = m_bezierCurves[i].getPointAtT((float)j / nbCollisions);
			}
			collisionPoints.back() = m_bezierCurves[nbCurves - 1].getPointAtT(1.0f);
		}
		else
		{
			u32 colIdx = 1;
			for (i32 i = nbCurves - 1; i >= 0; --i)
			{
				u32 nbCollisions = std::max(1u,static_cast<u32>(NB_COLLISIONS_SAMPLES_PER_METER * m_bezierCurves[i].length));
				for (i32 j = nbCollisions - 1; j >= 0; --j)
					collisionPoints[colIdx++] = m_bezierCurves[i].getPointAtT((float)j / nbCollisions);
			}
			collisionPoints.front() = m_bezierCurves[nbCurves - 1].getPointAtT(1.0f);
		}
		
		u32 startIdx = 0;
		u32 endIdx = 2;
		u32 nbPoints = collisionPoints.size();

		_polyPoints.clear();
		_polyPoints.addPoint(collisionPoints[0]); // first point
		while (endIdx < nbPoints) // Bad O(n²) complexity in the worst case (when all points are aligned) !
		{
			for (u32 i = endIdx - 1; i > startIdx; --i)
			{
				if (PointDistToSegment(collisionPoints[i],collisionPoints[startIdx],collisionPoints[endIdx]) >= COLLISION_ERROR)
				{
					_polyPoints.addPoint(collisionPoints[i]);
					startIdx = i;
					endIdx = startIdx + 1;
					break;
				}
			}
			++endIdx;
		}
		_polyPoints.addPoint(collisionPoints[nbPoints - 1]); // last point
	}

	bbool Frieze3DRuntimeData::drawBB = bfalse;

	void Frieze3DRuntimeData::reset()
	{
		m_meshes.clear();
		m_refMeshFill.reset();
		m_refMeshLeft.reset();
		m_refMeshRight.reset();
		m_scaleMesh = 1.0f;
		m_localMinZ = m_localMaxZ = 0.0f;
        m_orient = btrue;
        m_globalBBRecomputeFlag = bfalse;
	}

	Mesh3DResource* getResource(const ResourceID& _id)
	{
		return _id.isValid() ? (static_cast<Mesh3DResource*>(_id.getResource())) : NULL;
	}

	void Frieze3DRuntimeData::generate(const Frieze3DBuildData& data, const Frieze3DConfig& config3D, const ITF_VECTOR<FriseTextureConfig>& textureConfig, bbool _orient)
	{
		reset();
		m_orient = _orient;

		Mesh3DResource* meshResFill = getResource(config3D.m_meshRefFillID);
		Mesh3DResource* meshResLeft = getResource(config3D.m_meshRefLeftID);
		Mesh3DResource* meshResRight = getResource(config3D.m_meshRefRightID);

		if (!meshResFill)
			return;

		m_refMeshFill.generate(meshResFill->getMesh3D());
		if (!m_refMeshFill.m_valid)
			return;

		// extremities
		if (meshResLeft)	m_refMeshLeft.generate(meshResLeft->getMesh3D());
		if (meshResRight)	m_refMeshRight.generate(meshResRight->getMesh3D());

		bbool hasLeftExtremity = m_refMeshLeft.m_valid && !data.isLooping();
		bbool hasRightExtremity = m_refMeshRight.m_valid && !data.isLooping(); 

		const ITF_VECTOR<BezierCurveFrieze3D>& bezierCurves = data.getBezierCurves();
		u32 nbBeziers = bezierCurves.size();
		f32 curveLength = data.getCurveLength();

		if (curveLength <= 0.0f)
			return;
	
		f32 totalMeshLength = 0.0f;
		f32 oldLength = 0.0f;
		u32 nbMesh = 0;

		if (hasLeftExtremity)
		{
			totalMeshLength += m_refMeshLeft.getLength();	
			++nbMesh;
		}

		if (hasRightExtremity && totalMeshLength < curveLength)
		{
			oldLength = totalMeshLength;
			totalMeshLength += m_refMeshRight.getLength();	
			++nbMesh;	
		}

		while (totalMeshLength < curveLength) // A loop is used so that little effort will be necessary when introducing variations 
		{
			oldLength = totalMeshLength;
			totalMeshLength += m_refMeshFill.getLength();
			++nbMesh;
		}

		if (f32_Abs(curveLength - oldLength) <= f32_Abs(curveLength - totalMeshLength) && nbMesh > 0)
		{
			totalMeshLength = oldLength;
			--nbMesh;
		}

		if (nbMesh < ((hasLeftExtremity && hasRightExtremity) ? 2u : 1u))
			return;

		m_scaleMesh = curveLength / totalMeshLength; 
		m_refMeshFill.setScale(m_scaleMesh);
		m_localMinZ = m_refMeshFill.getMinZ();
		m_localMaxZ = m_refMeshFill.getMaxZ();
		if (hasLeftExtremity) 
		{
			m_refMeshLeft.setScale(m_scaleMesh);
			m_localMinZ = f32_Min(m_localMinZ,m_refMeshLeft.getMinZ());
			m_localMaxZ = f32_Max(m_localMaxZ,m_refMeshLeft.getMaxZ());
		}
		if (hasRightExtremity)
		{
			m_refMeshRight.setScale(m_scaleMesh);
			m_localMinZ = f32_Min(m_localMinZ,m_refMeshRight.getMinZ());
			m_localMaxZ = f32_Max(m_localMaxZ,m_refMeshRight.getMaxZ());
		}
		
		Vec3d meshSize = m_refMeshFill.m_aabb.getSize();		
		f32 minZ = m_refMeshFill.m_aabb.getMin().z(); 
		f32 maxZ = m_refMeshFill.m_aabb.getMax().z();

		m_meshes.reserve(nbMesh);

		u32 startFillIdx = hasLeftExtremity ? 1 : 0;
		u32 endFillIdx = hasRightExtremity ? nbMesh - 1 : nbMesh;

		// Shared index buffers
		if (hasLeftExtremity && meshResLeft)
		{
			m_meshes.push_back(Frieze3DMesh(
				&m_refMeshLeft
#ifdef UAF_FRIEZE3D_PROFILING
				,profiler
#endif
			));
			m_meshes.back().initIndexBuffer(meshResLeft,textureConfig);
		}

		for (u32 i = startFillIdx; i < endFillIdx; ++i)
		{	
			m_meshes.push_back(Frieze3DMesh(
				&m_refMeshFill
#ifdef UAF_FRIEZE3D_PROFILING
				,profiler
#endif
			));

			m_meshes.back().initIndexBuffer(meshResFill,textureConfig);
		}

		if (hasRightExtremity && meshResRight)
		{
			m_meshes.push_back(Frieze3DMesh(
				&m_refMeshRight
#ifdef UAF_FRIEZE3D_PROFILING
				,profiler
#endif
			));
			m_meshes.back().initIndexBuffer(meshResRight,textureConfig);
		}

		m_aabbLocal.setMax(Vec3d(-FLT_MAX,-FLT_MAX,-FLT_MAX));
		m_aabbLocal.setMin(Vec3d(FLT_MAX,FLT_MAX,FLT_MAX));

		f32 bezierStartDist = 0.0f;
		u32 refBezierIdx = 0;

		// Init some data (rough BB + bezier curves indexes by mesh)
		for (u32 i = 0; i < nbMesh; ++i)
		{
			Frieze3DMesh& mesh = m_meshes[i];
			mesh.m_aabbLocal.invalidate(); // So that it can grow
			mesh.startingBezierIdx = refBezierIdx;
			mesh.startingBezierOffset = bezierStartDist;	

			f32 remainingLength = mesh.m_refMeshPtr->getScaledLength();
			do 
			{
				const BezierCurveFrieze3D& bezier = bezierCurves[refBezierIdx];

				f32 currentCurveLength = bezierCurves[refBezierIdx].length - bezierStartDist;
				f32 startT = bezier.getTforDist(bezierStartDist);
				f32 endT = 1.0f;
				
				if (remainingLength >= currentCurveLength)
				{
					remainingLength -= currentCurveLength;
					bezierStartDist = 0.0f;
					++refBezierIdx;
				}
				else
				{
					bezierStartDist += remainingLength;
					endT = bezier.getTforDist(bezierStartDist);		
					remainingLength = 0.0f;	
				}

				AABB bezierAABB = bezier.getBoundingBox(startT,endT);

				// This is a very rough estimate ! Can be improved
				f32 bias = m_orient ? mesh.m_refMeshPtr->getHeight() : 0.0f; 
				Vec3d minBB = bezierAABB.getMin().to3d(minZ) - Vec3d(bias,mesh.m_refMeshPtr->getHeight(),0.0f);
				Vec3d maxBB = bezierAABB.getMax().to3d(maxZ) + Vec3d(bias,mesh.m_refMeshPtr->getHeight(),0.0f);

				mesh.m_aabbLocal.grow(AABB3d(minBB,maxBB));
				mesh.roughAABB = btrue;				
			}
			while ( refBezierIdx != nbBeziers && remainingLength > 0.0f );

			// update entire local aabb
			AABB aabb2d(mesh.m_aabbLocal.getMin().truncateTo2D(),mesh.m_aabbLocal.getMax().truncateTo2D());
			m_aabbLocal.grow(aabb2d);
			mesh.computeWorldBB();
		}
	}

	void Frieze3DRuntimeData::render(const Frieze3DBuildData& _data, const ITF_VECTOR<View*>& _views, f32 depth, const ObjectRef& _ref, const GFXPrimitiveParam& params)
	{
		u32 nbMeshes = m_meshes.size();
		AABB3d aabb;


#ifdef UAF_FRIEZE3D_PROFILING
		profiler.nbMeshRendered = 0;
		profiler.nbMeshGenerated = 0;
		profiler.nbMeshDisposed = 0;
		profiler.nbBBRefined = 0;
#endif

		ITF_VECTOR<Camera*> cameras;
		cameras.reserve(_views.size());
		for (u32 i = 0; i < _views.size(); ++i)
			if (_views[i]->getViewType() == View::viewType_Camera)
			{
				Camera* camera = _views[i]->getCamera();
				if (camera)
					cameras.push_back(camera);
			}

		m_globalBBRecomputeFlag = bfalse;

		for (u32 i = 0; i < nbMeshes; ++i)
			if (m_meshes[i].update(cameras,_data,*this))
			{
				m_meshes[i].render(_views,depth,_ref,params);

#ifdef ITF_SUPPORT_DEBUGFEATURE
				if (Frieze3DRuntimeData::drawBB)
				{
					const AABB3d& bb = m_meshes[i].m_aabbWorld;
					Vec3d Pt[8];

					for (u32 j = 0; j < 8; j++ )
					{
						Pt[j].x() = (j & 1) ? bb.getMax().x() : bb.getMin().x();
						Pt[j].y() = (j & 2) ? bb.getMax().y() : bb.getMin().y();
						Pt[j].z() = (j & 4) ? bb.getMax().z() : bb.getMin().z();
					}

					GFX_ADAPTER->drawDBG3DLine( Pt[0], Pt[1], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[1], Pt[3], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[3], Pt[2], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[2], Pt[0], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[0], Pt[4], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[1], Pt[5], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[2], Pt[6], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[3], Pt[7], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[4], Pt[5], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[5], Pt[7], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[7], Pt[6], 0,1,0 );
					GFX_ADAPTER->drawDBG3DLine( Pt[6], Pt[4], 0,1,0 );
				}
#endif //ITF_SUPPORT_DEBUGFEATURE
			}

#ifdef ITF_SUPPORT_DEBUGFEATURE
		// Draw bezier tangents
		if (Frieze3DRuntimeData::drawBB)
		{
			Vec2d offset = ((Frise*)_ref.getObject())->get2DPos(); // A bit dirty
			// TODO Take orientation into account
			
			u32 nbBeziers = _data.getBezierCurves().size();
			for (u32 i = 0; i < nbBeziers; ++i)
			{
				const BezierCurveFrieze3D& bezier = _data.getBezierCurves()[i];
				Vec2d p0 = bezier.p0 + offset;
				Vec2d p1 = bezier.p1 + offset;
				Vec2d p2 = bezier.p2 + offset;
				Vec2d p3 = bezier.p3 + offset;
				GFX_ADAPTER->drawDBGLine(p0,p1,Color::red());
				GFX_ADAPTER->drawDBGLine(p2,p3,Color::red());
			}
		}
#endif //ITF_SUPPORT_DEBUGFEATURE

		if (isGlobalBBRecomputationNeeded())
		{
			m_aabbLocal.setMax(Vec3d(-FLT_MAX,-FLT_MAX,-FLT_MAX));
			m_aabbLocal.setMin(Vec3d(FLT_MAX,FLT_MAX,FLT_MAX));
			for (u32 i = 0; i < nbMeshes; ++i)
			{
				m_aabbLocal.grow(m_meshes[i].m_aabbLocal.getMin());
				m_aabbLocal.grow(m_meshes[i].m_aabbLocal.getMax());
			}
		}

#ifdef UAF_FRIEZE3D_PROFILING
		if (profiler.nbMeshRendered != 0)
			LOG("NB FRIEZE 3D MESH RENDERERED : %d",profiler.nbMeshRendered);
		if (profiler.nbMeshGenerated != 0)
			LOG("NB FRIEZE 3D MESH GENERATED : %d",profiler.nbMeshGenerated);
		if (profiler.nbMeshDisposed != 0)
			LOG("NB FRIEZE 3D MESH DISPOSED : %d",profiler.nbMeshDisposed);
		if (profiler.nbBBRefined != 0)
			LOG("NB FRIEZE 3D BB REFINED : %d",profiler.nbBBRefined);
#endif
	}

	void Frieze3DRuntimeData::updateMeshesMatrices()
	{
		u32 nbMeshes = m_meshes.size();
		for (u32 i = 0; i < nbMeshes; ++i)
		{
			m_meshes[i].m_mesh.m_mesh.setMatrix(m_matrix);
			m_meshes[i].computeWorldBB();
		}
	}

	Vec3d transformPos(const Transform2d& _xf,const Vec3d v)
	{
		return _xf.transformPos(v.truncateTo2D()).to3d(v.z());
	}

	Vec3d mergeXY(const Vec3d& v0,const Vec3d& v1)
	{
		return Vec3d(v0.x(),v1.y(),v0.z());
	}

	void Frieze3DRuntimeData::RefMesh::reset()
	{
		m_valid = bfalse;
		m_vertices.clear();		
		m_aabb.setMin(Vec3d::Zero);
		m_aabb.setMax(Vec3d::Zero);
		m_scaledLength = 1.0f;
	}

	void Frieze3DRuntimeData::RefMesh::generate(Mesh3D* mesh3D)
	{
		reset();

		if (!mesh3D)
			return;

		// Ref vertex buffer
		u32 nbVertices = mesh3D->m_ListUniqueVertex.size();
		if (nbVertices == 0)
			return;

		m_vertices.reserve(nbVertices);
		for (u32 i = 0; i < nbVertices; ++i)
		{
			//const Mesh3D::UniqueVertex &vertex = mesh3D->m_listUniqueVertex[i];
			//VertexPNCT vPNCT;
			//vPNCT.m_color = COLOR_WHITE;
			//vPNCT.m_normal = mesh3D->m_ListNormal[vertex.m_Normal];
			//vPNCT.m_pos = mesh3D->m_ListVertex[vertex.m_Vertex];
			//vPNCT.m_uv = mesh3D->m_ListUV1[vertex.m_UV];

            const Mesh3D::UniqueVertex &vertex = mesh3D->m_ListUniqueVertex[i];
            VertexPNCT vPNCT;
            vPNCT.m_color   = COLOR_WHITE;
            vPNCT.m_normal  = vertex.Normal;
            vPNCT.m_pos     = vertex.Position;
            vPNCT.m_uv      = vertex.UV;
            
            m_vertices.push_back(vPNCT);
		}

		// Bounding box
		m_aabb.setMin(mesh3D->getBVMin());
		m_aabb.setMax(mesh3D->getBVMax());

		m_valid = btrue;
	}

	bbool isInFrustum(const AABB3d& _aabb, const ITF_VECTOR<Camera*>& _cameras)
	{
		u32 nbCameras = _cameras.size();		
		for (u32 i = 0; i < nbCameras; ++i)
			if (_cameras[i]->getViewFrustum().isInFrustum(_aabb)) // culling
				return btrue;
		return bfalse;
	}

	void Frieze3DRuntimeData::Frieze3DMesh::refineAABB(const Frieze3DBuildData& bData, Frieze3DRuntimeData& rtData)
	{
	 	ITF_ASSERT(roughAABB);

		const ITF_VECTOR<BezierCurveFrieze3D>& curves = bData.getBezierCurves();
		u32 nbCurves = curves.size();

		const ITF_VECTOR<VertexPNCT>& refVertices = m_refMeshPtr->m_vertices;
		u32 nbVertices = refVertices.size();

		f32 scale = rtData.getScaleMesh();
		f32 xStart = m_refMeshPtr->m_aabb.getMin().x(); 

		m_aabbLocal.invalidate();

		for (u32 i = 0; i < nbVertices; ++i)
		{
			Vec3d refPos = refVertices[i].m_pos;
			f32 pointX = (refPos.x() - xStart) * scale + startingBezierOffset;
			f32 pointY = refPos.y();
		
			u32 bezierIdx = startingBezierIdx;

			while (pointX >= curves[bezierIdx].length && bezierIdx < nbCurves - 1)
			{
				pointX -= curves[bezierIdx].length;
				++bezierIdx;
			}

			const BezierCurveFrieze3D& bez = curves[bezierIdx];
			f32 t = bez.getTforDist(pointX);
			Vec2d pos2D = bez.getPointAtT(t);
			Vec2d tangent = bez.getTangentAtT(t);
			Vec2d normal = rtData.useNormalOrientation() ? tangent.getPerpendicular() : Vec2d(0.0f,f32_Sign(tangent.x()));
			pos2D = pos2D + pointY * normal;
			m_aabbLocal.grow(pos2D.to3d(refPos.z()));
		}

		computeWorldBB();
		rtData.askForGlobalBBRecomputation();
#ifdef UAF_FRIEZE3D_PROFILING
		++profiler.nbBBRefined;
#endif
		roughAABB = bfalse;
	}

	void Frieze3DRuntimeData::Frieze3DMesh::generateMesh(const Frieze3DBuildData& bData, Frieze3DRuntimeData& rtData)
	{
		ITF_ASSERT(!generated);

		const ITF_VECTOR<BezierCurveFrieze3D>& curves = bData.getBezierCurves();
		u32 nbCurves = curves.size();

		const ITF_VECTOR<VertexPNCT>& refVertices = m_refMeshPtr->m_vertices;
		u32 nbVertices = refVertices.size();

		f32 scale = rtData.getScaleMesh();
		f32 xStart = m_refMeshPtr->m_aabb.getMin().x(); 

		ITF_Mesh& mesh3D = m_mesh.m_mesh;
		mesh3D.createVertexBuffer(nbVertices,VertexFormat_PNCT,sizeof(VertexPNCT),vbLockType_static); // TODO Use a pool of vertex buffer as we they re all the same size ! 
		ITF_VertexBuffer* vb = mesh3D.getCurrentVB();

		Matrix44 rot = Matrix44::identity();

		VertexPNCT* vbData;
		vb->Lock((void**)&vbData);
		for (u32 i = 0; i < nbVertices; ++i)
		{
			Vec3d refPos = refVertices[i].m_pos;
			f32 pointX = (refPos.x() - xStart) * scale + startingBezierOffset;
			f32 pointY = refPos.y();
			u32 bezierIdx = startingBezierIdx;

			while (pointX >= curves[bezierIdx].length && bezierIdx < nbCurves - 1)
			{
				pointX -= curves[bezierIdx].length;
				++bezierIdx;
			}

			const BezierCurveFrieze3D& bez = curves[bezierIdx];
			f32 t = bez.getTforDist(pointX);
			Vec2d pos2D = bez.getPointAtT(t);
			Vec2d tangent = bez.getTangentAtT(t);
			Vec2d normal = rtData.useNormalOrientation() ? tangent.getPerpendicular() : Vec2d(0.0f,f32_Sign(tangent.x()));
			pos2D = pos2D + pointY * normal;

			// To handle shearing we take the perpendiculars
			rot.setI(-normal.getPerpendicular().to3d(0.0f).toFloat4_0());
			rot.setJ(tangent.getPerpendicular().to3d(0.0f).toFloat4_0());

			//Vec3d n;
			//rot.transformVector(n,refVertices[i].m_normal);

			VertexPNCT vertex = refVertices[i];
			vertex.m_pos = pos2D.to3d(refPos.z());
			//vertex.m_normal = n.normalize();
			vbData[i] = vertex;
		}
		vb->Unlock();

#ifdef UAF_FRIEZE3D_PROFILING
		++profiler.nbMeshGenerated;
#endif
		generated = btrue;
	}

	void Frieze3DRuntimeData::Frieze3DMesh::disposeMesh()
	{
		ITF_ASSERT(generated);
		m_mesh.m_mesh.removeVertexBuffer();
#ifdef UAF_FRIEZE3D_PROFILING
		++profiler.nbMeshDisposed;
#endif
		generated = bfalse;
	}

	void Frieze3DRuntimeData::Frieze3DMesh::initIndexBuffer(Mesh3DResource* meshRes, const ITF_VECTOR<FriseTextureConfig>& textureConfig)
	{
		ITF_Mesh& mesh3D = m_mesh.m_mesh;
				
        Mesh3D * meshPtr = meshRes->getMesh3D();
        if (!meshPtr)
            return;

        u32 nbIndexBuffers = meshPtr->m_ListElement.size();
		for (u32 j = 0; j < nbIndexBuffers; ++j)
		{
			mesh3D.addElementAndMaterial(textureConfig[meshPtr->m_ListElement[j].m_MaterialID].getGFXMaterial());
			ITF_MeshElement& element = mesh3D.getMeshElement(j);
			element.m_indexBuffer = meshRes->getIndexBuffer(j); // Shared index buffers
			element.m_count = element.m_indexBuffer->m_nIndices;
		}
	}

	bbool Frieze3DRuntimeData::Frieze3DMesh::update(const ITF_VECTOR<Camera*>& _cameras, const Frieze3DBuildData& bData, Frieze3DRuntimeData& rtData)
	{
		bbool inFrustum = isInFrustum(m_aabbWorld,_cameras);
		if (inFrustum)
		{
			if (generated)
				return true;
			else // needs generation
			{
				if (roughAABB)
				{
					refineAABB(bData,rtData);
					inFrustum = isInFrustum(m_aabbWorld,_cameras); // recheck frustum as the aabb has been refined
				}

				if (inFrustum)
				{
					generateMesh(bData,rtData);
					return true;
				}
				return false;
			}
		}
		else if (generated)
			disposeMesh();

		return false;
	}

	void Frieze3DRuntimeData::Frieze3DMesh::render(const ITF_VECTOR<View*>& _views, f32 depth, const ObjectRef& _ref,  const GFXPrimitiveParam& params)
	{
		ITF_ASSERT(generated);
		m_mesh.setCommonParam(params);
		GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_3D>(_views, &m_mesh, depth, _ref);
#ifdef UAF_FRIEZE3D_PROFILING
		++profiler.nbMeshRendered;
#endif
	}

	void Frieze3DRuntimeData::Frieze3DMesh::computeWorldBB()
	{
		Vec3d v0 = m_aabbLocal.getMin();
		Vec3d v1 = m_aabbLocal.getMax();
		Vec3d v2 = Vec3d(v0.x(),v1.y(),0.0f); // we dont care about the z
		Vec3d v3 = Vec3d(v1.x(),v0.y(),0.0f);
		const Matrix44& t = m_mesh.m_mesh.getMatrix();

		t.transformPoint(v0,v0);
		t.transformPoint(v1,v1);
		t.transformPoint(v2,v2);
		t.transformPoint(v3,v3);

		m_aabbWorld.invalidate();
		m_aabbWorld.grow(v0);
		m_aabbWorld.grow(v1);
		m_aabbWorld.grow(v2);
		m_aabbWorld.grow(v3);

		m_mesh.m_bvMin = m_aabbWorld.getMin();
		m_mesh.m_bvMax = m_aabbWorld.getMax();
	}
}
