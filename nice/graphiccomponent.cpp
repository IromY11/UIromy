#include "precompiled_engine.h"

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#include "gameplay/GameMaterial.h"

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMATERIALSMANAGER_H_
#include "gameplay/managers/GameMaterialManager.h"
#endif //_ITF_GAMEMATERIALSMANAGER_H_

#ifndef _ITF_SERIALIZED_OBJECT_CONTENT_H_
#include "core/serializer/SerializedObjectContent.h"
#endif //_ITF_SERIALIZED_OBJECT_CONTENT_H_


//#define ENABLE_SHADOW_DEBUG

#ifdef ENABLE_SHADOW_DEBUG
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(GraphicComponent)

#define PRIMITIVE_PARAM_LABEL "PrimitiveParameters"

BEGIN_SERIALIZATION_CHILD(GraphicComponent)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
       
        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
            SERIALIZE_MEMBER("ColorFactor", m_primitiveParam.m_colorFactor);    
            SERIALIZE_MEMBER("ColorFog", m_primitiveParam.m_colorFog);
            SERIALIZE_MEMBER("useStaticFog", m_primitiveParam.m_useStaticFog);
            SERIALIZE_MEMBER("renderInReflection", m_primitiveParam.m_renderInReflection);
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT(PRIMITIVE_PARAM_LABEL, m_primitiveParam);
        // WARNING don't>of serialize in FINAL.
        SERIALIZE_MEMBER("colorComputerTagId", m_colorComputerTagId);
        SERIALIZE_MEMBER("renderInTarget", m_renderInTarget);
        SERIALIZE_MEMBER("disableLight",m_disableLight);
        SERIALIZE_MEMBER("disableShadow",m_disableShadow);
        SERIALIZE_MEMBER("depthOffset",m_depthOffset);
		SERIALIZE_MEMBER("AlphaInit",m_alphaInit);
    END_CONDITION_BLOCK()

#ifndef ITF_STRIPPED_DATA
    BEGIN_CONDITION_BLOCK(ESerialize_ForcedValues)
        SERIALIZE_OBJECT("PRIMITIVEPARAMFORCED", m_primitiveParamForced);
    END_CONDITION_BLOCK()
#endif // !ITF_STRIPPED_DATA
END_SERIALIZATION()

GraphicComponent::GraphicComponent()
: Super()
, m_hDiv(2)
, m_vDiv(2)
, m_shadowFilter(ECOLLISIONFILTER_ENVIRONMENT)
, m_alpha(1.0f)
, m_alphaSrc(1.0f)
, m_alphaDst(1.0f)
, m_alphaInit(1.0f)
, m_faidingTime(1.0f)
, m_faidingValue(0.0f)
, m_pauseOnFade(bfalse)
, m_destroyOnFade(bfalse)
, m_renderInTarget(bfalse)
, m_primitiveParam(GFXPrimitiveParam())
, m_colorComputerTagId(0)
, m_shadowObj(NULL)
, m_shadowUseBase(bfalse)
, m_shadowDisableOffset(bfalse)
, m_useShadow(bfalse)
, m_disableLight(UNINIT_BBOOL)
, m_disableShadow(UNINIT_BBOOL)
, m_depthOffset(0.f)
, m_shadowBoneOffset( Vec3d::Zero)
{
#if defined(ITF_SUPPORT_EDITOR) 
    m_gfxOccludeInfoPrevious = m_primitiveParam.m_gfxOccludeInfo;
#endif
}

GraphicComponent::~GraphicComponent()
{
    SF_DEL(m_shadowObj);
}

void GraphicComponent::onActorClearComponents()
{
    if (m_shadowObj)
    {
        destroyShadowMesh();
    }
}

ITF_IndexBuffer* GraphicComponent::getStaticIndexBuffer(u32 _hdiv, u32 _vdiv)
{ 
    return GFX_ADAPTER->getPatchGridIndexBuffer(_hdiv, _vdiv);
};


void GraphicComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

	setAlpha(m_alphaInit);
    if (m_disableLight == UNINIT_BBOOL || getTemplate()->forceDisableLight())
    {
        m_disableLight = getTemplate()->getDisableLight();
    }

    if ( m_disableShadow == 1 || !getTemplate()->getIsUseShadow() )
    {
        m_useShadow = bfalse;
    }
    else
    {
        m_useShadow = btrue;
    }

	if ( getTemplate()->getUseNoColShadow())
		setShadowUseBase( btrue);

    m_posOffset = getTemplate()->getPosOffset();

#ifdef ITF_CTR
    m_hDiv = 2;
    m_vDiv = 2;
#else
#ifdef ITF_WII
#define DOWNSHIFT 1
#else
#define DOWNSHIFT 0
#endif
    if (getTemplate()->getPatchLevel())
    {
        m_hDiv  = getTemplate()->getPatchLevel()>>DOWNSHIFT;
        m_vDiv  = getTemplate()->getPatchLevel()>>DOWNSHIFT;
    }
    else
    {
        m_hDiv  = getTemplate()->getHDiv()>>DOWNSHIFT;
        m_vDiv  = getTemplate()->getVDiv()>>DOWNSHIFT;
    }
#endif
    /*if (getTemplate()->getRenderInTarget())
        m_matFlags |= MAT_F_RENDERINTARGET;
    else
        m_matFlags &= ~MAT_F_RENDERINTARGET;*/

    if ( m_useShadow )
    {
        m_shadowObj = newAlloc(mId_Vertex, ShadowObj);
        // constructor ??.
        m_shadowObj->m_renderShadow = bfalse;
        m_shadowObj->m_shadowAlpha = 1.f;
        m_shadowObj->m_shadowMul = 1.f;

        if (!m_shadowObj->m_shadowMesh.getCurrentVB())
        {
            createShadowMesh();
        }
    }

	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventViewportVisibility_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventShow_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventReset_CRC,this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventEnableShadow_CRC,this);
}

#ifdef ITF_SUPPORT_COOKING
void GraphicComponent::onCookProcess()
{
    Super::onCookProcess();
}
#endif // ITF_SUPPORT_COOKING


void GraphicComponent::onStartDestroy( bbool _hotReload )
{
    if (m_shadowObj)
    {
        destroyShadowMesh();
        SF_DEL(m_shadowObj);
    }
}

void GraphicComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    reset();
}

void GraphicComponent::reset()
{
    m_alphaDst = m_alphaSrc = 1.f;
    m_alpha = m_alphaInit;
    m_faidingTime = 0.0f;
    m_faidingValue = 0.0f;
}

void GraphicComponent::processAlpha(f32 _deltaTime)
{
    if ( m_faidingValue <= 0.f|| m_faidingTime <= 0.f || m_alphaDst == m_alphaSrc ) 
        return;

    m_faidingValue -= _deltaTime;

    if (m_faidingValue <= 0.f)
    {
        m_alphaSrc = m_alpha = m_alphaDst;
        m_faidingValue = 0.f;

        if (m_pauseOnFade)
            m_actor->disable();
        else if (m_destroyOnFade)
            m_actor->requestDestruction();
    }
    else
    {
        f32 t = 1.f - ( m_faidingValue / m_faidingTime );
        m_alpha = Interpolate(m_alphaSrc,m_alphaDst,t);
    }
}

Vec2d GraphicComponent::getOffset() const
{
    return m_posOffset;
}

void GraphicComponent::setOffset( const Vec2d& _offset )
{
    m_posOffset = _offset;
}

void GraphicComponent::processShadow(f32 _deltaTime, f32 _angle, const Vec2d &_boneOffset /*= Vec2d::Zero*/)
{
    //	shadow construction:
    //	
    //	p0. --- p1. --- contactPos. --- p2. --- p3. 
    //

    if ( m_useShadow )
    {
        // reinit States.
        m_shadowObj->m_renderShadow = bfalse;
        m_shadowObj->m_sS = 1;
        m_shadowObj->m_sE = 3;

        /// Find collisions polylines.
        Vec2d offsetVector = m_shadowDisableOffset ? Vec2d::Zero : ( ( getTemplate()->getShadowOffsetPos().truncateTo2D() + Vec2d( getShadowOffset().x(), 0.f)) * m_actor->getScale() );

        if ( getTemplate()->getUseRootBone() )
        {
            if (m_actor->getIsFlipped())
                _angle = MTH_PI - _angle;

            offsetVector = offsetVector.Rotate(_angle);
		}

		offsetVector += _boneOffset;

        if (m_actor->getIsFlipped() && !getTemplate()->getUseNoColShadow())
            offsetVector.x() *= -1.f;
        
#ifdef ENABLE_SHADOW_DEBUG
        DebugDraw::circle(m_actor->get2DPos() + offsetVector, m_actor->getDepth() + MTH_EPSILON, 0.1f);
#endif
        if ( m_shadowUseBase )
        {
            compuseShadowPosFromBase(offsetVector);
        }
        else
        {
            compuseShadowPosFromContacts(offsetVector);
        }
    }
}

void GraphicComponent::compuseShadowPosFromBase( const Vec2d& _shadowOffset )
{
    Vec2d offset = getOffset() + _shadowOffset;
    Vec2d shadsize = getShadowSize();
    
    m_shadowObj->m_shadowAlpha = m_alpha;
    shadsize.x() *= m_shadowObj->m_shadowMul;
    f32 tsize = shadsize.x() * m_shadowObj->m_shadowAlpha;			
    f32 halfsize = tsize * 0.5f;
    Vec2d contactPos = m_actor->get2DPos() + offset;

    Vec2d tmp = Vec2d::Right * halfsize;
    Vec2d p1 = contactPos - tmp;
    Vec2d p2 = contactPos + tmp;

    m_shadowObj->m_shadowPos[1] = Vec3d( p1.x(), p1.y(), m_actor->getDepth());
    m_shadowObj->m_shadowPos[2] = Vec3d( contactPos.x(), contactPos.y(), m_actor->getDepth());
    m_shadowObj->m_shadowPos[3] = Vec3d( p2.x(), p2.y(), m_actor->getDepth());

    m_shadowObj->m_renderShadow = btrue;

    computeVertexBufferCache();
}

void GraphicComponent::compuseShadowPosFromContacts( const Vec2d& _shadowOffset )
{
    PhysRayCastContactsContainer contacts;
    Vec2d rayCastDir = Vec2d(0, -getTemplate()->getShadowDist());
    const Vec2d raycastP0 = m_actor->getPos().truncateTo2D() + _shadowOffset;
    const Vec2d raycastP1 = raycastP0 + rayCastDir;

    if ( PHYSWORLD->rayCastEnvironment( raycastP0, raycastP1, m_shadowFilter, m_actor->getDepth(), contacts) )
    {
        /// get nearest collision.
        f32 coeff = F32_INFINITY;
        u32 minc = U32_INVALID;

        for ( u32 i = 0; i < contacts.size(); i++ )
        {
            SRayCastContact& c = contacts[i];

            if ( c.m_collidableUserData == m_actor->getRef() )
            {
                continue;
            }

            PolyLine* poly = AIUtils::getPolyLine(c.m_collidableUserData);

            if ( !poly || poly->getOwnerActor() == m_actor )
            {
                continue;
            }

            if ( c.m_edgeIndex0 < poly->getVectorsCount() )
            {
                const PolyLineEdge& edge = poly->getEdgeAt(c.m_edgeIndex0);
                const GameMaterial_Template* mat = edge.getGameMaterialTemplate();

                if ( mat && mat->getNoShadow() )
                {
                    continue;
                }
            }

            if ( c.m_edgeIndex1 < poly->getVectorsCount() && c.m_edgeIndex1 != c.m_edgeIndex0 )
            {
                const PolyLineEdge& edge = poly->getEdgeAt(c.m_edgeIndex1);
                const GameMaterial_Template* mat = edge.getGameMaterialTemplate();

                if ( mat && mat->getNoShadow() )
                {
                    continue;
                }
            }

            if ( contacts[i].m_t0 < coeff )
            {
                minc = i;
                coeff = contacts[i].m_t0;
            }
        }

        if ( minc == U32_INVALID || contacts[minc].m_edgeIndex0 == U32_INVALID ) 
        {
            return;
        }

        m_shadowObj->m_shadowAlpha = getTemplate()->computeSizeFactor(1.f - coeff);

        PolyLine* polyLine;
        const PolyLineEdge* edge;
        AIUtils::getPolyLine(contacts[minc].m_collidableUserData,contacts[minc].m_edgeIndex0, polyLine, edge);

        f32 rejectAngle = fabs(atan2f(edge->m_normalizedVector.y(), edge->m_normalizedVector.x())) * MTH_RADTODEG;
        if (rejectAngle > 80)
            return;

#ifdef showallcontacts
        for ( u32 i = 0; i < contacts.size(); i++ )
        {
            Vec2d contactPos = p0 + ( p1 - p0) *contacts[i].m_t0;
            GFX_ADAPTER->drawDBGCircle(contactPos.x(), contactPos.y(), 0.2f, 1.f,0.f,0.f);
        }
#endif

        Vec2d contactPos = raycastP0 + ( raycastP1 - raycastP0) * contacts[minc].m_t0;

        Vec2d shadsize = getShadowSize();
        //subanimation can control sizex.
        shadsize.x() *= m_shadowObj->m_shadowMul;
        f32 tsize = shadsize.x() * m_shadowObj->m_shadowAlpha;			
        f32 halfsize = tsize * 0.5f;

        Vec2d tmp = edge->m_normalizedVector * halfsize;
        Vec2d p1 = contactPos - tmp;
        Vec2d p2 = contactPos + tmp;

        const f32 mindist = 0.3f;
        const f32 offsetcut = 0.2f;

        PolyLine* nextPoly = NULL;
        u32 nextEdge = U32_INVALID; 
        f32 leng = 0.f;

        Vec2d p1edg = edge->getPos();
        Vec2d p2edg = p1edg + (edge->m_normalizedVector * edge->m_length);

        /// p1 > end of edge ??.
        leng = (p2edg - p1).norm();
        if (leng > edge->m_length)
        {
            /// Find on the left.
            AIUtils::getAdjacentEdge(polyLine, contacts[minc].m_edgeIndex0, bfalse, nextPoly, nextEdge);
            if (nextPoly)
            {
                const PolyLineEdge* edgen = &nextPoly->getEdgeAt(nextEdge);
                if (edgen)
                {
                    if (leng > (edge->m_length + mindist))
                    {
                        // Limit angle.
                        f32 anglelimit = getTemplate()->getAngleLimit();
                        f32 angleSegLimit = f32_ACos(edge->m_normalizedVector.dot(edgen->m_normalizedVector))* MTH_RADTODEG;
                        f32 angleEdge = f32_Abs(atan2f(edgen->m_normalizedVector.y(), edgen->m_normalizedVector.x())) * MTH_RADTODEG;
                        if ((anglelimit > 0.f && angleEdge > anglelimit) || angleSegLimit > 88)
                        {
                            p1 = p1edg - (edge->m_normalizedVector * offsetcut);
                        }
                        else
                        {
#ifdef simpleShadow
                            contactPos = edge->getPos();
                            f32 dst = f32_Abs(halfsize - (contactPos - edge->getPos()).norm());
                            p1 = contactPos - (edgen->m_normalizedVector * dst);
#else
                            p1 = edge->getPos();
                            f32 dst = halfsize - (contactPos - p1).norm();
                            m_shadowObj->m_sS--;
                            Vec2d p0 = p1 - (edgen->m_normalizedVector * dst);
                            m_shadowObj->m_shadowPos[0] = Vec3d( p0.x(), p0.y(), m_actor->getDepth());
#endif
                        }
                    }
                }
            }
            else
            {
                p1 = p1edg - (edge->m_normalizedVector * offsetcut);
            }
        }

        /// p2 > start of edge ??.
        leng = (p2 - p1edg).norm();
        if (leng > edge->m_length)
        {
            nextPoly = NULL;
            nextEdge = U32_INVALID; 

            /// Find on the right.
            AIUtils::getAdjacentEdge(polyLine, contacts[minc].m_edgeIndex0, btrue, nextPoly, nextEdge);
            if (nextPoly)
            {
                const PolyLineEdge* edgen = &nextPoly->getEdgeAt(nextEdge);
                if (edgen)
                {
                    if (leng > (edge->m_length + mindist))
                    {
                        // Limit angle. World/local?
                        f32 anglelimit = getTemplate()->getAngleLimit();
                        f32 angleSegLimit = f32_ACos(edge->m_normalizedVector.dot(edgen->m_normalizedVector))* MTH_RADTODEG;
                        f32 angleEdge = f32_Abs(atan2f(edgen->m_normalizedVector.y(), edgen->m_normalizedVector.x())) * MTH_RADTODEG;
                        if ((anglelimit > 0.f && angleEdge > anglelimit) || angleSegLimit > 88)
                        {
                            Vec2d::ScaleAdd(&p2, &edge->m_normalizedVector, offsetcut, &p2edg);
                        }
                        else
                        {
#ifdef simpleShadow
                            contactPos = edgen->getPos();
                            f32 dst = halfsize - (edgen->getPos() - contactPos).norm();
                            p2 = contactPos + (edgen->m_normalizedVector * dst);
#else
                            p2 = edgen->getPos();
                            f32 dst = halfsize - (p2 - contactPos).norm();
                            m_shadowObj->m_sE++;
                            Vec2d p3;
                            Vec2d::ScaleAdd(&p3, &edgen->m_normalizedVector, dst, &p2);
                            m_shadowObj->m_shadowPos[4] = Vec3d( p3.x(), p3.y(), m_actor->getDepth());
#endif
                        }
                    }
                }
            }
            else
            {
                Vec2d::ScaleAdd(&p2, &edge->m_normalizedVector, offsetcut, &p2edg);
            }
        }

		if ( getShadowOffset().y())
		{
			p1.y()+=getShadowOffset().y();
			p2.y()+=getShadowOffset().y();
			contactPos.y()+=getShadowOffset().y();
		}
        m_shadowObj->m_shadowPos[1] = Vec3d( p1.x(), p1.y(), m_actor->getDepth());
        m_shadowObj->m_shadowPos[2] = Vec3d( contactPos.x(), contactPos.y(), m_actor->getDepth());
        m_shadowObj->m_shadowPos[3] = Vec3d( p2.x(), p2.y(), m_actor->getDepth());

        m_shadowObj->m_renderShadow = btrue;

        computeVertexBufferCache();
    }
}

void GraphicComponent::Update( f32 _deltaTime )
{
    // Compute AABB
    ITF_ASSERT(m_actor);

    processAlpha(_deltaTime);
}

void GraphicComponent::createShadowMesh()
{
    //if a previous mesh is created destroy it
    destroyShadowMesh();

    ITF_Mesh & shadowMesh = m_shadowObj->m_shadowMesh;
        
    shadowMesh.createVertexBuffer(10 , VertexFormat_PCT, sizeof(VertexPCT), vbLockType_dynDiscard);

    //create mesh element
    if( shadowMesh.getNbMeshElement() == 0u )
    {
        shadowMesh.addElementAndMaterial(getTemplate()->getShadowMaterial());
        shadowMesh.removePassFilterFlag(GFX_ZLIST_PASS_ZPREPASS_FLAG); // no need for shadow in ZPrepass
    }

    ITF_MeshElement & meshElt = shadowMesh.getMeshElement();

    //create mesh element index buffer
    meshElt.m_indexBuffer = GFX_ADAPTER->createIndexBuffer(24, bfalse);

    //fill mesh element index buffer
    u16* indexBuffer;
    meshElt.m_indexBuffer->Lock((void**)&indexBuffer);

    u16 inc = 0;
    for (u32 i = 0; i < 4 ; i++ , inc += 2)
    {
        indexBuffer[i * 6 + 0] = inc + 0;
        indexBuffer[i * 6 + 1] = inc + 1;
        indexBuffer[i * 6 + 2] = inc + 3;
        indexBuffer[i * 6 + 3] = inc + 3;
        indexBuffer[i * 6 + 4] = inc + 2;
        indexBuffer[i * 6 + 5] = inc + 0;
    }

    meshElt.m_indexBuffer->Unlock();

    //set up mesh element
    meshElt.m_count = 24;

    //set mesh matrix
    shadowMesh.setMatrix(Matrix44::identity());
}

//------------------------------------------------------------------

void GraphicComponent::destroyShadowMesh()
{
    ITF_Mesh & shadowMesh = m_shadowObj->m_shadowMesh;

    //free vertex buffers
    shadowMesh.removeVertexBuffer();

    //free index buffer
    if(shadowMesh.getNbMeshElement() > 0u )
    {
        if (shadowMesh.getMeshElement().m_indexBuffer != nullptr)
        {
            GFX_ADAPTER->removeIndexBuffer(shadowMesh.getMeshElement().m_indexBuffer);
        }
    }
}

void GraphicComponent::computeVertexBufferCache()
{
    Vec3d dir;
    Vec3d perp;
    Vec3d lastperp;
    
    Vec2d shadsize = getShadowSize();
    shadsize.x() *= m_shadowObj->m_shadowMul;
    u32 nS = m_shadowObj->m_sE - m_shadowObj->m_sS;
    
    f32 hlfsizey = shadsize.y() * m_shadowObj->m_shadowAlpha * 0.5f;
    
    f32 distU = 0.f;
    for (u32 i = 0; i < nS; i++ )
    {
        const Vec3d& p1 = m_shadowObj->m_shadowPos[i + m_shadowObj->m_sS];
        const Vec3d& p2 = m_shadowObj->m_shadowPos[i + m_shadowObj->m_sS + 1];
        f32 length = (p2 - p1).norm();

        dir = (p2 - p1).normalize();
        Vec3d::ZAxis.crossProduct(dir, perp);

        // Blend with last perpendicular.
        if (i>0)
        {
            perp.x() = f32_Lerp(perp.x(), lastperp.x(), 0.5f);
            perp.y() = f32_Lerp(perp.y(), lastperp.y(), 0.5f);
            perp.normalize();
        }

        lastperp = perp;
        perp *= hlfsizey;

        m_shadowObj->m_cacheVB[ i * 2].m_pos = p1 - perp;
        m_shadowObj->m_cacheVB[i * 2+1].m_pos = p1 + perp;

        m_shadowObj->m_cacheVB[i * 2].m_uv = Vec2d(distU, 0.f);
        m_shadowObj->m_cacheVB[i * 2+1].m_uv = Vec2d(distU, 1.f);

        distU += length/shadsize.x();

        if (i == nS - 1)
        {
            perp = dir.getPerpendicular();
            perp *= hlfsizey;

            m_shadowObj->m_cacheVB[i * 2 + 2].m_pos = p2 - perp;
            m_shadowObj->m_cacheVB[i * 2 + 3].m_pos = p2 + perp;
            
            m_shadowObj->m_cacheVB[i * 2 + 2].m_uv = Vec2d(1.f, 0.f);
            m_shadowObj->m_cacheVB[i * 2 + 3].m_uv = Vec2d(1.f, 1.f);
        }
    }

    //color and UVs.
    Color col = Color::white();
    col.m_a = m_shadowObj->m_shadowAlpha * getTemplate()->getShadowAttenuation() * m_alpha;
    u32 colo = col.getAsU32();
    for (u32 i=0; i < 10; i++)
    {
        m_shadowObj->m_cacheVB[i].m_color = colo;
    }
}

void GraphicComponent::drawShadow(const ITF_VECTOR <class View*>& _views)
{
    if ( m_shadowObj && m_shadowObj->m_renderShadow && m_useShadow )
    {
        ITF_Mesh & shadowMesh = m_shadowObj->m_shadowMesh;

        //switch mesh user buffer
        shadowMesh.swapVBForDynamicMesh();
        ITF_VertexBuffer * vertexBuffer = shadowMesh.getCurrentVB();

        u32 nS = m_shadowObj->m_sE - m_shadowObj->m_sS;
        u32 nV = nS*2+2;

        shadowMesh.getMeshElement().m_startVertex = 0;
        shadowMesh.getMeshElement().m_count = nS*6;

        // Shadow will use owner's parameters (static fog and so on)
        shadowMesh.setCommonParam(getGfxPrimitiveParam()); 

        //lock vertex buffer data
        VertexPCT* data = NULL;
        GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vertexBuffer, (void**)&data );
        ITF_ASSERT_MSG(data, "Unable to lock a vertex buffer");
        if(data != NULL)
        {
            //write in vertex buffer data
            ITF_MemcpyWriteCombined(data, &m_shadowObj->m_cacheVB, sizeof(VertexPCT)*nV);
            //unlock vertex buffer
            GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(vertexBuffer);
            //add primitive to be drawn
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &shadowMesh, GetActor()->getDepth() - 0.01f + getTemplate()->getShadowOffsetPos().z(), m_actor->getRef());
        }
    }
}

void GraphicComponent::onEvent( Event* _event)
{
    Super::onEvent(_event);

    if ( EventShow* eventShow = DYNAMIC_CAST(_event,EventShow) )
    {
        if ( eventShow->getIsOverrideAlpha() &&  eventShow->getIsAlphaRatio())
        {
            m_alphaSrc      = m_alpha;
            m_alphaDst      = eventShow->getAlpha();

            if ( eventShow->getTransitionTime() <= 0.f || m_alphaDst == m_alphaSrc )
            {
                m_alpha         = m_alphaDst;
                m_alphaSrc      = m_alphaDst;
                m_faidingValue  = 0.f;
                m_faidingTime   = 0.f;
            }
            else
            {
                m_faidingTime   = eventShow->getTransitionTime();
                m_faidingValue  = m_faidingTime;
            }
        }

        m_pauseOnFade = eventShow->getPauseOnEnd();
        m_destroyOnFade = eventShow->getDestroyOnEnd();
	}
	else if( EventViewportVisibility* eventViewportVisibility = DYNAMIC_CAST(_event,EventViewportVisibility) )
	{
		m_primitiveParam.m_viewportVisibility = eventViewportVisibility->getViewportVisibility();
	}
    else if ( _event->IsClassCRC(EventReset::GetClassCRCStatic()) )
    {
        reset();
    }
    else if ( EventEnableShadow* eventEnableShadow = DYNAMIC_CAST(_event,EventEnableShadow) )
    {
        m_useShadow = eventEnableShadow->isEnabled();
    }
}

AABB GraphicComponent::getVisualAABB() const
{
    if ( !getTemplate() || ( getTemplate()->getVisualAABB().getMin() == Vec2d::Zero && getTemplate()->getVisualAABB().getMax() == Vec2d::Zero ) )
        return GetActor()->getAABB();
    else
    {
        AABB AABBtemp(getTemplate()->getVisualAABB());
        AABBtemp.Scale(GetActor()->getScale());
        AABBtemp.Rotate(GetActor()->getAngle(), bfalse);
        AABBtemp.Translate(GetActor()->getPos().truncateTo2D());
        return AABBtemp;
    }
}

const class Angle & GraphicComponent::getAngleOffset() const
{
    return getTemplate()->getRotOffset();
}

ITF_IndexBuffer* GraphicComponent::getMyStaticIndexBuffer()
{
#if !defined(ITF_DISABLE_WARNING)
    u32 prevHDiv = m_hDiv;
    u32 prevVDiv = m_vDiv;
#endif // !defined(ITF_DISABLE_WARNING)

    ITF_IndexBuffer* idxBuffer = GFX_ADAPTER->getPatchGridIndexBuffer(m_hDiv, m_vDiv);

#ifndef ITF_WII
    ITF_WARNING_CATEGORY(GPP,
        m_actor, prevHDiv == m_hDiv && prevVDiv == m_vDiv, 
        "Actor's component is using an invalid patch level %ix%i (only allowed: %s)", 
        prevHDiv, prevVDiv, GFXAdapter::getAllowedPatchGridBufferTypesStr().cStr() );
#endif

    return idxBuffer;
}

Vec2d GraphicComponent::getShadowSize()
{
    return getTemplate()->getShadowSize() * m_actor->getScale();
}

#if defined(ITF_SUPPORT_EDITOR) 
void GraphicComponent::onPrePropertyChange()
{
    m_gfxOccludeInfoPrevious = m_primitiveParam.m_gfxOccludeInfo;
}

void GraphicComponent::onPostPropertyChange()
{
    if (m_primitiveParam.m_gfxOccludeInfo != m_gfxOccludeInfoPrevious)
    {
        GetActor()->setHotReloadAfterModif();
    }

    bbool prevUseShadow = m_useShadow;
    bbool newUseShadow;

    if ( m_disableShadow == 1 || !getTemplate()->getIsUseShadow() )
    {
        newUseShadow = bfalse;
    }
    else
    {
        newUseShadow = btrue;
    }

    if ( newUseShadow != prevUseShadow )
    {
        m_actor->setHotReloadAfterModif();
    }
}
#endif

#ifndef ITF_STRIPPED_DATA

void GraphicComponent::updateForcedValues( const SerializedObjectContent_Object* _content )
{
    Super::updateForcedValues(_content);

    const SerializedObjectContent_Object* contentPrimitiveParameters = _content ? _content->getObjectWithTag(PRIMITIVE_PARAM_LABEL) : NULL;
    m_primitiveParamForced.update(contentPrimitiveParameters);
}

#endif // !ITF_STRIPPED_DATA

void GraphicComponent::setGFXPrimitiveParam( const GFXPrimitiveParam& _param )
{
#ifndef ITF_STRIPPED_DATA
    m_primitiveParamForced.setPrimitiveParam(m_primitiveParam, _param);
#else
    m_primitiveParam = _param;
#endif
}

f32 GraphicComponent::getDepthOffset() const
{
    return getTemplate()->getDepthOffset() + m_depthOffset;
}


///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(GraphicComponent_Template)
BEGIN_SERIALIZATION_CHILD(GraphicComponent_Template)
    SERIALIZE_MEMBER("patchLevel",m_patchLevel);
    SERIALIZE_MEMBER("patchHLevel",m_hDiv);
    SERIALIZE_MEMBER("patchVLevel",m_vDiv);
    SERIALIZE_OBJECT("visualAABB",m_visualAABB);
    SERIALIZE_MEMBER("renderintarget",m_renderInTarget);
    SERIALIZE_MEMBER("posOffset",m_posOffset);
    SERIALIZE_MEMBER("depthOffset",m_depthOffset);
    SERIALIZE_MEMBER("angleOffset",m_angleOffset);

    SERIALIZE_ENUM_BEGIN("blendmode",m_blendMode);
        SERIALIZE_ENUM_VAR(GFX_BLEND_UNKNOWN);
        SERIALIZE_ENUM_VAR(GFX_BLEND_COPY);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADEST);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADESTPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ADD);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ADDALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_SUBALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_SUB);
        SERIALIZE_ENUM_VAR(GFX_BLEND_MUL);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAMUL);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAMUL);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHA);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADEST);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADESTPREMULT);
        SERIALIZE_ENUM_VAR(GFX_BLEND_MUL2X);
        SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHATOCOLOR);
        SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHATOCOLOR);
        SERIALIZE_ENUM_VAR(GFX_BLEND_SCREEN);
    SERIALIZE_ENUM_END();

    SERIALIZE_ENUM_BEGIN("materialtype",m_matType);
        SERIALIZE_ENUM_VAR(GFX_MAT_DEFAULT);
        SERIALIZE_ENUM_VAR(GFX_MAT_REFRACTION);
        SERIALIZE_ENUM_VAR(GFX_MAT_PATCH);
        SERIALIZE_ENUM_VAR(GFX_MAT_FRIEZEANIM);
        SERIALIZE_ENUM_VAR(GFX_MAT_GLOW);
        SERIALIZE_ENUM_VAR(GFX_MAT_ALPHAFADE);
        SERIALIZE_ENUM_VAR(GFX_MAT_FRIEZEOVERLAY);
        SERIALIZE_ENUM_VAR(GFX_MAT_REFLECTION);
        SERIALIZE_ENUM_VAR(GFX_MAT_DRAW_MASK);
        SERIALIZE_ENUM_VAR(GFX_MAT_MASKED_TEX);
    SERIALIZE_ENUM_END();
    
    SERIALIZE_MEMBER("selfIllumColor",m_selfIllumColor);
    SERIALIZE_MEMBER("disableLight",m_disableLight);
    SERIALIZE_MEMBER("forceDisableLight",m_forceDisableLight);
	SERIALIZE_MEMBER("useShadow", m_useShadow);
	SERIALIZE_MEMBER("useNoColShadow", m_useNoColShadow);
	SERIALIZE_MEMBER("useRootBone", m_useRootBone);
	SERIALIZE_MEMBER("useBoneName", m_useBoneName);
    SERIALIZE_MEMBER("shadowSize",m_shadowSize);
    // insure retro compatibility of material with texture = "" syntax
    BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
        SERIALIZE_MEMBER("shadowTextureFile", m_shadowMaterial.getTexturePathSet().getTexturePath());
    END_CONDITION_BLOCK();
    //
    SERIALIZE_OBJECT("shadowMaterial", m_shadowMaterial);
    SERIALIZE_MEMBER("shadowAttenuation",m_shadowAttenuation);
    SERIALIZE_MEMBER("shadowDist",m_shadowDist);
    SERIALIZE_MEMBER("shadowOffsetPos",m_shadowOffsetPos);
    SERIALIZE_MEMBER("angleLimit",m_angleLimit);
    SERIALIZE_MEMBER("curveSize0",m_curveSize0);
    SERIALIZE_MEMBER("curveSize1",m_curveSize1);
    SERIALIZE_MEMBER("curveSizePower",m_curveSizePower);
    
END_SERIALIZATION()

GraphicComponent_Template::GraphicComponent_Template()
: m_patchLevel(0)
, m_hDiv(2)
, m_vDiv(2)
, m_visualAABB(AABB(Vec2d::Zero))
, m_renderInTarget(0)
, m_posOffset(Vec2d::Zero)
, m_angleOffset(btrue,0.f)
, m_blendMode(GFX_BLEND_ALPHA)
, m_matType(GFX_MAT_DEFAULT)
, m_selfIllumColor(Color::zero())
, m_disableLight(bfalse)
, m_forceDisableLight(bfalse)
, m_useShadow(bfalse)
, m_useNoColShadow(bfalse)
, m_useRootBone(bfalse)
, m_shadowSize(Vec2d(1.8f, 0.30f))
, m_shadowAttenuation(1.f)
, m_shadowDist(4.f)
, m_shadowOffsetPos(Vec3d::Zero)
, m_angleLimit(0.f)
, m_depthOffset(0.f)
, m_curveSize0(0.0f)
, m_curveSize1(1.0f)
, m_curveSizePower(1.0f)
{
}

bbool GraphicComponent_Template::onTemplateLoaded( bbool _hotReload )
{
    bbool bOk = Super::onTemplateLoaded(_hotReload);
    m_shadowMaterial.onLoaded(m_actorTemplate->getResourceContainer());
    return bOk;
}

void GraphicComponent_Template::onTemplateDelete( bbool _hotReload )
{
    Super::onTemplateDelete(_hotReload);

    m_shadowMaterial.onUnLoaded(m_actorTemplate->getResourceContainer());
}

}

