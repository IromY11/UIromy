#include "precompiled_gameplay.h"

#ifndef _ITF_RENDERBOXCOMPONENT_H_
#include "gameplay/Components/UI/RenderBoxComponent.h"
#endif //_ITF_RENDERBOXCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_ANIMMESHVERTEXCOMPONENT_H_
#include "engine/actors/components/AnimMeshVertexComponent.h"
#endif //_ITF_ANIMMESHVERTEXCOMPONENT_H_


namespace ITF
{

#define GRID_X  3
#define GRID_Y  3
    IMPLEMENT_OBJECT_RTTI(RenderSingleAnimData)
    BEGIN_SERIALIZATION_CHILD(RenderSingleAnimData)
        SERIALIZE_MEMBER("xMin", m_xMin);
        SERIALIZE_MEMBER("yMin", m_yMin);
        SERIALIZE_MEMBER("state", m_state);
    END_SERIALIZATION()


    IMPLEMENT_OBJECT_RTTI(RenderBoxComponent_Template)

    BEGIN_SERIALIZATION_CHILD(RenderBoxComponent_Template)
        SERIALIZE_MEMBER("is2D", m_is2D);
        SERIALIZE_MEMBER("addBorders",m_addBorders);
        SERIALIZE_MEMBER("useAnimMeshVertex", m_useAnimMeshVertex);
        SERIALIZE_CONTAINER_OBJECT("anims", m_amvList)

        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
        SERIALIZE_MEMBER("texture", m_material.getTexturePathSet().getTexturePath());
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT("material", m_material);
		SERIALIZE_CONTAINER_OBJECT("AdditionalMaterials",m_additionalMaterials);

        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
        SERIALIZE_MEMBER("frontTexture", m_frontMaterial.getTexturePathSet().getTexturePath());
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT("frontMaterial", m_frontMaterial);

        SERIALIZE_MEMBER("leftWidth",m_leftWidth);
        SERIALIZE_MEMBER("rightWidth", m_rightWidth);
        SERIALIZE_MEMBER("topHeight", m_topHeight);
        SERIALIZE_MEMBER("bottomHeight", m_bottomHeight);

        SERIALIZE_MEMBER("backTopLeftCornerAtlasIndex", m_backTopLeftCornerAtlasIndex);
        SERIALIZE_MEMBER("backTopMiddleCornerAtlasIndex", m_backTopMiddleCornerAtlasIndex);
        SERIALIZE_MEMBER("backTopRightCornerAtlasIndex", m_backTopRightCornerAtlasIndex);
        SERIALIZE_MEMBER("backMiddleLeftCornerAtlasIndex", m_backMiddleLeftCornerAtlasIndex);
        SERIALIZE_MEMBER("backMiddleMiddleCornerAtlasIndex", m_backMiddleMiddleCornerAtlasIndex);
        SERIALIZE_MEMBER("backMiddleRightCornerAtlasIndex", m_backMiddleRightCornerAtlasIndex);
        SERIALIZE_MEMBER("backBottomLeftCornerAtlasIndex", m_backBottomLeftCornerAtlasIndex);
        SERIALIZE_MEMBER("backBottomMiddleCornerAtlasIndex", m_backBottomMiddleCornerAtlasIndex);
        SERIALIZE_MEMBER("backBottomRightCornerAtlasIndex", m_backBottomRightCornerAtlasIndex);

        SERIALIZE_MEMBER("backTopLeftCornerAtlasRotate", m_backTopLeftCornerAtlasRotate);
        SERIALIZE_MEMBER("backTopMiddleCornerAtlasRotate", m_backTopMiddleCornerAtlasRotate);
        SERIALIZE_MEMBER("backTopRightCornerAtlasRotate", m_backTopRightCornerAtlasRotate);
        SERIALIZE_MEMBER("backMiddleLeftCornerAtlasRotate", m_backMiddleLeftCornerAtlasRotate);
        SERIALIZE_MEMBER("backMiddleMiddleCornerAtlasRotate", m_backMiddleMiddleCornerAtlasRotate);
        SERIALIZE_MEMBER("backMiddleRightCornerAtlasRotate", m_backMiddleRightCornerAtlasRotate);
        SERIALIZE_MEMBER("backBottomLeftCornerAtlasRotate", m_backBottomLeftCornerAtlasRotate);
        SERIALIZE_MEMBER("backBottomMiddleCornerAtlasRotate", m_backBottomMiddleCornerAtlasRotate);
        SERIALIZE_MEMBER("backBottomRightCornerAtlasRotate", m_backBottomRightCornerAtlasRotate);

        SERIALIZE_MEMBER("backTopMiddleCornerAtlasTile", m_backTopMiddleCornerAtlasTile);
        SERIALIZE_MEMBER("backMiddleLeftCornerAtlasTile", m_backMiddleLeftCornerAtlasTile);
        SERIALIZE_MEMBER("backMiddleRightCornerAtlasTile", m_backMiddleRightCornerAtlasTile);
        SERIALIZE_MEMBER("backBottomMiddleCornerAtlasTile", m_backBottomMiddleCornerAtlasTile);

        SERIALIZE_MEMBER("xUVToDistance", m_xUVToDistance);
        SERIALIZE_MEMBER("yUVToDistance", m_yUVToDistance);
        SERIALIZE_MEMBER("zOffset", m_zOffset);
        SERIALIZE_MEMBER("dynamicAMVMinimunBorderSize", m_dynamicAMVMinimunBorderSize);

        SERIALIZE_MEMBER("shadowOffset", m_shadowOffset);
        SERIALIZE_MEMBER("shadowAlpha", m_shadowAlpha);
        SERIALIZE_MEMBER("shadowUseFrontTexture", m_shadowUseFrontTexture);
        SERIALIZE_MEMBER("posShadowRelative", m_posShadowRelative);

        SERIALIZE_MEMBER("highlightOffset", m_highlightOffset);
        SERIALIZE_MEMBER("highlightAlpha", m_highlightAlpha);
        SERIALIZE_MEMBER("highlightScale", m_highlightScale);
        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
        SERIALIZE_MEMBER("highlightTexture", m_highlightMaterial.getTexturePathSet().getTexturePath());
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT("highlightMaterial", m_highlightMaterial)

    END_SERIALIZATION()

    RenderBoxComponent_Template::RenderBoxComponent_Template()
        : Super()
        , m_is2D(btrue)
        , m_addBorders(btrue)
        , m_useAnimMeshVertex(btrue)

        , m_leftWidth(0.25f)
        , m_rightWidth(0.25f)
        , m_topHeight(0.25f)
        , m_bottomHeight(0.25f)
        
        , m_backTopLeftCornerAtlasIndex(U32_INVALID)
        , m_backTopMiddleCornerAtlasIndex(U32_INVALID)
        , m_backTopRightCornerAtlasIndex(U32_INVALID)
        , m_backMiddleLeftCornerAtlasIndex(U32_INVALID)
        , m_backMiddleMiddleCornerAtlasIndex(U32_INVALID)
        , m_backMiddleRightCornerAtlasIndex(U32_INVALID)
        , m_backBottomLeftCornerAtlasIndex(U32_INVALID)
        , m_backBottomMiddleCornerAtlasIndex(U32_INVALID)
        , m_backBottomRightCornerAtlasIndex(U32_INVALID)
        
        , m_backTopLeftCornerAtlasRotate(0)
        , m_backTopMiddleCornerAtlasRotate(0)
        , m_backTopRightCornerAtlasRotate(0)
        , m_backMiddleLeftCornerAtlasRotate(0)
        , m_backMiddleMiddleCornerAtlasRotate(0)
        , m_backMiddleRightCornerAtlasRotate(0)
        , m_backBottomLeftCornerAtlasRotate(0)
        , m_backBottomMiddleCornerAtlasRotate(0)
        , m_backBottomRightCornerAtlasRotate(0)

        , m_backTopMiddleCornerAtlasTile(0)
        , m_backMiddleLeftCornerAtlasTile(0)
        , m_backMiddleRightCornerAtlasTile(0)
        , m_backBottomMiddleCornerAtlasTile(0)

        , m_xUVToDistance(1.f)
        , m_yUVToDistance(1.f)
        , m_zOffset(0.f)

        , m_shadowOffset(Vec3d::Zero)
        , m_shadowAlpha(0.f)
        , m_shadowUseFrontTexture(btrue)
        , m_posShadowRelative(bfalse)
        , m_dynamicAMVMinimunBorderSize(0.f)

        , m_highlightOffset(Vec3d::Zero)
        , m_highlightAlpha(0.f)
        , m_highlightScale(Vec2d::One)
    {
    }

    RenderBoxComponent_Template::~RenderBoxComponent_Template()
    {
    }

    bbool RenderBoxComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        if (!Super::onTemplateLoaded(_hotReload))
            return bfalse;

        ResourceContainer * resContainer = m_actorTemplate->getResourceContainer();
        m_material.onLoaded(resContainer);
        m_frontMaterial.onLoaded(resContainer);
        m_highlightMaterial.onLoaded(resContainer);

		for(ITF_VECTOR<GFXMaterialSerializable>::iterator it = m_additionalMaterials.begin(); it != m_additionalMaterials.end(); it++)
		{
			(*it).onLoaded(resContainer);
		}

        return btrue;
    }

    void RenderBoxComponent_Template::onTemplateDelete(bbool _hotReload)
    {
        ResourceContainer * resContainer = m_actorTemplate->getResourceContainer();
        m_material.onUnLoaded(resContainer);
        m_frontMaterial.onUnLoaded(resContainer);
        m_highlightMaterial.onUnLoaded(resContainer);

		for(ITF_VECTOR<GFXMaterialSerializable>::iterator it = m_additionalMaterials.begin(); it != m_additionalMaterials.end(); it++)
		{
			(*it).onUnLoaded(resContainer);
		}

        Super::onTemplateDelete(_hotReload);
    }


    i32 RenderBoxComponent_Template::getRotate(i32 _rotate) const
    {
        while (_rotate < 0)
            _rotate += 3600;

        _rotate = _rotate % 360;
        return _rotate;
    }

	const GFX_MATERIAL & RenderBoxComponent_Template::getMaterial( u32 idx ) const
	{
		if(idx == 0)
			return m_material;
		else
			return m_additionalMaterials[idx-1];
	}


    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(RenderBoxComponent)

		BEGIN_SERIALIZATION_CHILD(RenderBoxComponent)
		SERIALIZE_MEMBER("imageResolution", m_originalTextureSize)
		SERIALIZE_MEMBER("autoSize",m_autoSize);
	if(!m_autoSize)
	{
        SERIALIZE_MEMBER("size",m_size);
	}
	else
	{
		SERIALIZE_MEMBER("scale",m_autoSizeScale);
	}
        SERIALIZE_MEMBER("offset",m_offset);
        SERIALIZE_MEMBER("uvPreTranslation", m_uvPreTranslation);
	if(!m_autoSize)
	{
        SERIALIZE_MEMBER("uvRatio", m_uvRatio);
	}
        SERIALIZE_MEMBER("uvTranslation", m_uvTranslation);
        SERIALIZE_MEMBER("uvRotation", m_uvRotation);
        SERIALIZE_MEMBER("uvTranslationSpeed", m_uvTranslationSpeed);
        SERIALIZE_MEMBER("uvRotationSpeed", m_uvRotationSpeed);
		SERIALIZE_MEMBER("uvPivot", m_uvPivot);
		SERIALIZE_OBJECT("overwritematerial", m_overwrittenmaterial_serialized)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(RenderBoxComponent)
    END_VALIDATE_COMPONENT()

    RenderBoxComponent::RenderBoxComponent()
        : Super()
		, m_originalTextureSize(Vec2d::One*100.0f)
		, m_autoSize(bfalse)
		, m_autoSizeScale(Vec2d::One)
        , m_size(Vec2d::One)
        , m_offset(Vec2d::Zero)
        , m_uvPreTranslation(Vec2d::Zero)
        , m_uvTranslation(Vec2d::Zero)
        , m_uvTranslationSpeed(Vec2d::Zero)
        , m_uvRatio(Vec2d::One)
        , m_uvRotation(Angle::Zero)
        , m_uvRotationSpeed(Angle::Zero)
        , m_uvPivot(Vec2d::Zero)
        , m_indexBuffer(NULL)
        , m_indexBuffer2(NULL)
        , m_drawEnabled(btrue)
        , m_useFrontTexture(bfalse)
        , m_amvComponent(NULL)
        , m_needRefreshUvMatrix(bfalse)
        , m_shadowOffset(Vec3d::Zero)
        , m_shadowAlpha(0.f)
        , m_useShadow(bfalse)
        , m_highlightOffset(Vec3d::Zero)
        , m_highlightAlpha(0.f)
        , m_highlightScale(Vec2d::One)
        , m_useHighlight(bfalse)
        , m_state(U32_INVALID)
        , m_colorSrc(Color::white())
        , m_colorDst(Color::white())
        , m_colorTimeLeft(0.f)
        , m_colorTimeTotal(0.f)
		, m_currentFrontIndex(0)
    {
    }

    RenderBoxComponent::~RenderBoxComponent()
    {
        m_boxMesh.removeVertexBuffer();
        if (m_indexBuffer)
            GFX_ADAPTER->removeIndexBuffer(m_indexBuffer);
        if (m_indexBuffer2)
            GFX_ADAPTER->removeIndexBuffer(m_indexBuffer2);
    }

	void RenderBoxComponent::onStartDestroy( bbool _hotReload )
{
		
		if(!m_overwrittenmaterial_serialized.getTexturePathSet().getTexturePath().isEmpty())
			m_overwrittenmaterial_serialized.onUnLoaded(GetActor()->getResourceContainer());
	}

    
    void RenderBoxComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

		if(!m_overwrittenmaterial_serialized.getTexturePathSet().getTexturePath().isEmpty())
		{
			m_overwrittenmaterial_serialized.onLoaded(GetActor()->getResourceContainer());
			m_overwrittenmaterial = getSerializedOverrideMaterial();
		}

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventShow_CRC,this);

        if(getTemplate()->getUseAnimMeshVertex())
        {
            m_amvComponent      = m_actor->GetComponent<AnimMeshVertexComponent>();
        }
        
        m_useFrontTexture   = getTemplate()->getFrontMaterial().getTexture() != NULL;

        m_shadowOffset  = getTemplate()->getShadowOffset();
        m_shadowAlpha   = getTemplate()->getShadowAlpha();
        m_useShadow     = m_shadowOffset != Vec3d::Zero &&  m_shadowAlpha != 0.f;

        if (m_useShadow)
        {
            ITF_WARNING(m_actor, -m_shadowOffset.z() > f32_Abs(getTemplate()->getZOffset()), "Bad shadow placement !!");
        }

        m_highlightOffset   = getTemplate()->getHighlightOffset();
        m_highlightAlpha    = getTemplate()->getHighlightAlpha();
        m_highlightScale    = getTemplate()->getHighlightScale();
        m_useHighlight      = m_highlightAlpha != 0.f;

        if(m_amvComponent)
        {
            m_amvComponent->setUseActorTransform(bfalse);
        }

        u32 nbLayers        = m_useFrontTexture ? 2 : 1;
        u32 layerVertexCount= GRID_X*2*GRID_Y*2;
        u32 layerIndexCount = GRID_X*GRID_Y*2*3;

        m_boxMesh.createVertexBuffer(layerVertexCount*nbLayers, VertexFormat_PC2T, sizeof(VertexPC2T), vbLockType_static);

		if(!m_overwrittenmaterial_serialized.getTexturePathSet().getTexturePath().isEmpty())
		{
			m_boxMesh.addElementAndMaterial(m_overwrittenmaterial);
		}
		else
		{
			m_boxMesh.addElementAndMaterial(getTemplate()->getMaterial());
			for(u32 i = 0; i < getTemplate()->getAdditionalMaterialSize(); i++)
			{
				m_boxMesh.addElementAndMaterial(getTemplate()->getAdditionalMaterial(i));
			}
			m_currentFrontIndex = getTemplate()->getAdditionalMaterialSize()+1;
		}

        m_indexBuffer = GFX_ADAPTER->createIndexBuffer(layerIndexCount, bfalse);
        ITF_ASSERT(m_indexBuffer);

        if (m_useFrontTexture)
        {
            m_boxMesh.addElementAndMaterial(getTemplate()->getFrontMaterial());
            m_indexBuffer2 = GFX_ADAPTER->createIndexBuffer(layerIndexCount, bfalse);
            ITF_ASSERT(m_indexBuffer2);
        }

        ITF_IndexBuffer * indexBuffer[2] = { m_indexBuffer, m_indexBuffer2 };

        u16* index;
        u32 gridXLen = 2*GRID_X;
        u32 gridYLen = 2*GRID_Y;

        // back index grid 3x3
        for (u32 i=0; i<nbLayers; i++)
        {
            indexBuffer[i]->Lock((void**)&index);
            u32 delta = i*layerVertexCount;

            for (u32 y=0; y<gridYLen; y+=2)
            {
                for (u32 x=0; x<gridXLen; x+=2)
                {
                    (*index++) = u16(x      + y     *gridXLen + delta);
                    (*index++) = u16((x+1)  + y     *gridXLen + delta);
                    (*index++) = u16(x      + (y+1) *gridXLen + delta);
                    (*index++) = u16((x+1)  + y     *gridXLen + delta);
                    (*index++) = u16(x      + (y+1) *gridXLen + delta);
                    (*index++) = u16((x+1)  + (y+1) *gridXLen + delta);
                }
            }
            indexBuffer[i]->Unlock();
        }

        m_boxMesh.getMeshElement(0).m_indexBuffer = m_indexBuffer;
        m_boxMesh.getMeshElement(0).m_count = layerIndexCount;

        if (m_useFrontTexture)
        {
            m_boxMesh.getMeshElement(m_currentFrontIndex).m_indexBuffer   = m_indexBuffer2;
            m_boxMesh.getMeshElement(m_currentFrontIndex).m_count         = layerIndexCount;
        }

        computeBox();
    }

#ifdef ITF_SUPPORT_EDITOR
    void RenderBoxComponent::onPostPropertyChange()
    {
        computeBox();
        computeAMV();
    }
#endif //ITF_SUPPORT_EDITOR

    void RenderBoxComponent::onResourceReady()
    {
		Super::onResourceReady();
		computeBox();
        computeAMV();
    }

	void RenderBoxComponent::setMaterialIndex( u32 idx )
	{
		if(idx > getTemplate()->getAdditionalMaterialSize()+1 || !m_overwrittenmaterial_serialized.getTexturePathSet().getTexturePath().isEmpty() || m_boxMesh.getNbMeshElement() <= idx)
			return;

		m_boxMesh.setMaterial(0,m_boxMesh.getMaterial(idx));

		computeBox();
		computeAMV();
	}

    void RenderBoxComponent::computeAMV()
    {
        if (!m_amvComponent)
            return;
        

        AnimMeshVertex * amv = m_amvComponent->getAnimMeshVertex();
        if (!amv)
            return;

        const RenderSingleAnimDataList & amvList = getTemplate()->getAMVList();
        u32 amvListSize = amvList.size();

        if (amvListSize == 0)
        {
            m_amvComponent->resizeAMVList(0);
            return;
        }

        m_usedAmvList.clear();
        for (u32 i=0; i<amvListSize; i++)
        {
            const RenderSingleAnimData & anim = amvList[i];
            if (anim.m_xMin <= m_trueSize.x() && anim.m_yMin <= m_trueSize.y() &&
                (anim.m_state == U32_INVALID || anim.m_state == m_state))
            {
                m_usedAmvList.emplace_back();
                m_usedAmvList.back().m_index = i;
            }
        }

        amvListSize = m_usedAmvList.size();
        if (amvListSize == 0)
        {
            m_amvComponent->resizeAMVList(0);
            return;
        }

        m_amvComponent->resizeAMVList(amvListSize);
        for (u32 i=0; i<amvListSize; i++)
        {
            const RenderSingleAnimData & anim = amvList[m_usedAmvList[i].m_index];
            u32 animIdx = amv->getAnimIndexByFriendly(anim.m_animName);
            if (animIdx == U32_INVALID)
                animIdx = 0;
            if (m_amvComponent->getAMVAnim(i) != animIdx) // do not reset anim if already set
            {
                m_amvComponent->setAMVAnim(i, animIdx);
                m_amvComponent->setAMVFrame(i, 0);
            }
            m_amvComponent->setAMVColor(i, anim.m_color);
        }

        computeAMVPositions();
    }


    Vec3d RenderBoxComponent::getGlobalPosFromAMVLocalPos(const Vec3d & _pos)
    {
        Vec3d globalPos;
        u32 placement = u32(_pos.z() + 0.5f);
        switch (placement % 3) // x _pos
        {
        case 0:
            globalPos.x() = m_trueSize.x() * -0.5f + _pos.x() * getTemplate()->getLeftWidth();
            break;
        case 2:
            globalPos.x() = m_trueSize.x() * 0.5f - ( 1.f - _pos.x()) * getTemplate()->getRightWidth();
            break;
        default:
            globalPos.x() = (m_trueSize.x() - getTemplate()->getLeftWidth() - getTemplate()->getRightWidth()) * (_pos.x() - 0.5f);
            break;
        }
        switch (placement / 3) // y _pos
        {
        case 0:
            globalPos.y() = m_trueSize.y() * -0.5f + _pos.y() * getTemplate()->getTopHeight();
            break;
        case 2:
            globalPos.y() = m_trueSize.y() * 0.5f - ( 1.f - _pos.y()) * getTemplate()->getBottomHeight();
            break;
        default:
            globalPos.y() = (m_trueSize.y() - getTemplate()->getTopHeight() - getTemplate()->getBottomHeight()) * (_pos.y() - 0.5f);
            break;
        }

        globalPos.z() = 0;
        if (getTemplate()->is2D())
        {
            // convert to 2D coords
            globalPos          = (GetActor()->get2DPos()  + (m_offset  + globalPos.truncateTo2D() * g_pixelBy2DUnit) * GetActor()->getScale()).to3d();
        } else
        {
            globalPos.y()     *= -1.f;
            globalPos         = (GetActor()->get2DPos()  + (m_offset  + globalPos.truncateTo2D()) * GetActor()->getScale()).to3d(GetActor()->getDepth());
        }

        return globalPos;
    }

    void RenderBoxComponent::computeAMVPositions()
    {
        if (!m_amvComponent)
            return;
        
        const RenderSingleAnimDataList & amvList = getTemplate()->getAMVList();
        u32 amvListSize = m_usedAmvList.size();
        for (u32 i=0; i<amvListSize; i++)
        {
            // compute local pos:
            const UsedDataAmv &  usedAmv = m_usedAmvList[i];
            const RenderSingleAnimData & anim = amvList[usedAmv.m_index];
            Vec3d pos = getGlobalPosFromAMVLocalPos(usedAmv.m_override ? usedAmv.m_pos : anim.m_pos);

            Vec2d scale = anim.m_scale;
            if (getTemplate()->is2D())
            {
                scale       *= GetActor()->getScale()*g_pixelBy2DUnit;
                scale.y()   *= -1;
            } else
            {
                scale       *= GetActor()->getScale();
            }

            m_amvComponent->setAMVTransformFrom(i, pos, anim.m_angle.ToRadians(), scale, usedAmv.m_override ? usedAmv.m_flip : anim.m_flip);
        }
    }

    bbool RenderBoxComponent::getUnicRenderSingleAnimDataWithState(u32 _state, RenderSingleAnimData & _renderSingleAnimData)
    {
        const RenderSingleAnimDataList & amvList = getTemplate()->getAMVList();
        u32 amvListSize = amvList.size();
        u32 nbFound = 0;
        for (u32 i=0; i<amvListSize; i++)
        {
            const RenderSingleAnimData & anim = amvList[i];
            if (_state == anim.m_state)
            {
                nbFound++;
                if (nbFound == 1)
                    _renderSingleAnimData = anim;
                else
                    return bfalse;
            }
        }
        return nbFound == 1;
    }

    UsedDataAmv * RenderBoxComponent::getUnicUsedDataWithState(u32 _state)
    {
        const RenderSingleAnimDataList & amvList = getTemplate()->getAMVList();
        UsedDataAmv * unicUsedDataWithState = NULL;
        u32 amvListSize = m_usedAmvList.size();
        for (u32 i=0; i<amvListSize; i++)
        {
            const RenderSingleAnimData & anim = amvList[m_usedAmvList[i].m_index];
            if (anim.m_state == _state)
            {
                if (unicUsedDataWithState == NULL)
                    unicUsedDataWithState = &m_usedAmvList[i];
                else
                    return NULL;
            }
        }
        return unicUsedDataWithState;
    }


    void RenderBoxComponent::setState(u32 _state)
    {
        if (m_state == _state)
            return;

        m_state = _state;
        computeAMV();
    }



    void RenderBoxComponent::processRotate(Vec2d * pt, i32 _rotate)
    {
        Vec2d swapPt;
        switch (_rotate)
        {
        case 90:
            swapPt  = pt[3];
            pt[3]   = pt[2];
            pt[2]   = pt[1];
            pt[1]   = pt[0];
            pt[0]   = swapPt;
            break;
        case 180:
            swapPt  = pt[0];
            pt[0]   = pt[2];
            pt[2]   = swapPt;
            swapPt  = pt[1];
            pt[1]   = pt[3];
            pt[3]   = swapPt;
            break;
        case 270:
            swapPt  = pt[0];
            pt[0]   = pt[1];
            pt[1]   = pt[2];
            pt[2]   = pt[3];
            pt[3]   = swapPt;
            break;
        default:
            break;
        }

    }

    void RenderBoxComponent::processTile(Vec2d * uvTab, u32 * idxTab, u32 _x, u32 _y, f32 midWidth, f32 midHeight, bbool _onWidth, bbool _isRotate)
    {
        const u32 gridXLen = 2*GRID_X;

        u32 idx11InTab    = idxTab[_x       + _y    *gridXLen];
        u32 idx12InTab    = idxTab[_x       + (_y+1)*gridXLen];
        u32 idx21InTab    = idxTab[(_x+1)   + _y    *gridXLen];
        u32 idx22InTab    = idxTab[(_x+1)   + (_y+1)*gridXLen];

        if (_isRotate)
        {
            u32 swap = idx12InTab;
            idx12InTab  = idx21InTab;
            idx21InTab  = swap;

            // width and height are also inverted
            f32 swapHW  = midWidth;
            midWidth    = midHeight;
            midHeight   = swapHW;
        }

        if (_onWidth)
        {
            f32 uvWidth       = uvTab[idx21InTab].x() - uvTab[idx11InTab].x();
            f32 nbTile        = f32_Floor(f32_Abs(midWidth/(uvWidth*getTemplate()->getXUVToDistance())) + 0.5f);
            if (nbTile < 1.f)  nbTile++;

            uvTab[idx21InTab].x() = uvTab[idx11InTab].x() + nbTile * uvWidth;
            uvTab[idx22InTab].x() = uvTab[idx12InTab].x() + nbTile * uvWidth;
        } else // onHeight
        {
            f32 uvHeight      = uvTab[idx12InTab].y() - uvTab[idx11InTab].y();
            f32 nbTile        = f32_Floor(f32_Abs(midHeight/(uvHeight*getTemplate()->getYUVToDistance())) + 0.5f);
            if (nbTile < 1.f)  nbTile++;

            uvTab[idx12InTab].y() = uvTab[idx11InTab].y() + nbTile * uvHeight;
            uvTab[idx22InTab].y() = uvTab[idx21InTab].y() + nbTile * uvHeight;
        }
    }


    void RenderBoxComponent::computeBox()
    {
        Texture * textBack  = getTemplate()->getMaterial(0).getTexture(TEXSET_ID_SEPARATE_ALPHA);
        Texture * textFront = getTemplate()->getMaterial(0).getTexture(TEXSET_ID_DIFFUSE);

		if(!m_overwrittenmaterial_serialized.getTexturePathSet().getTexturePath().isEmpty())
		{
			textBack = m_overwrittenmaterial.getTexture(TEXSET_ID_SEPARATE_ALPHA);
			textFront = m_overwrittenmaterial.getTexture(TEXSET_ID_DIFFUSE);
		}

        if (!textFront)
            return;

        if (!textBack)
            textBack = textFront;

		if(m_autoSize && textFront->isPhysicallyLoaded())
		{
			f32 xsize = (m_originalTextureSize.x()*0.01f) * m_autoSizeScale.x();
			f32 ysize = (m_originalTextureSize.y()*0.01f) * m_autoSizeScale.y();
			m_size = Vec2d(xsize,ysize);
			m_uvRatio = Vec2d(1.0f/m_size.x(),1.0f/m_size.y());
		}

        const RenderBoxComponent_Template * rTemplate = getTemplate();

        f32 leftWidth       = rTemplate->getLeftWidth();
        f32 topHeight       = rTemplate->getTopHeight();
        f32 rightWidth      = rTemplate->getRightWidth();
        f32 bottomHeight    = rTemplate->getBottomHeight();

        f32 midWidth        = m_size.x() - leftWidth - rightWidth;
        f32 midHeight       = m_size.y() - topHeight - bottomHeight;
        
        if (midWidth < 0.f)
            midWidth = 0.f;
        
        if (midHeight < 0.f)
            midHeight = 0.f;

        m_trueSize = Vec2d(leftWidth + midWidth + rightWidth, topHeight + midHeight + bottomHeight);

        if(m_trueSize.x() == 0)
            m_trueSize.x() = 1.0f;

         if(m_trueSize.y() == 0)
            m_trueSize.y() = 1.0f;

        u32 backTopLeftCornerAtlasIndex         = rTemplate->getBackTopLeftCornerAtlasIndex();
        u32 backTopMiddleCornerAtlasIndex       = rTemplate->getBackTopMiddleCornerAtlasIndex();
        u32 backTopRightCornerAtlasIndex        = rTemplate->getBackTopRightCornerAtlasIndex();
        u32 backMiddleLeftCornerAtlasIndex      = rTemplate->getBackMiddleLeftCornerAtlasIndex();
        u32 backMiddleMiddleCornerAtlasIndex    = rTemplate->getBackMiddleMiddleCornerAtlasIndex();
        u32 backMiddleRightCornerAtlasIndex     = rTemplate->getBackMiddleRightCornerAtlasIndex();
        u32 backBottomLeftCornerAtlasIndex      = rTemplate->getBackBottomLeftCornerAtlasIndex();
        u32 backBottomMiddleCornerAtlasIndex    = rTemplate->getBackBottomMiddleCornerAtlasIndex();
        u32 backBottomRightCornerAtlasIndex     = rTemplate->getBackBottomRightCornerAtlasIndex();
 
        
        const UVAtlas * uvAtlas = textBack->getUVAtlas();
        u32   nbUV = uvAtlas ? uvAtlas->getNumberUV() : 0;

        bbool useAtlas = !( backTopLeftCornerAtlasIndex         >= nbUV ||
                            backTopMiddleCornerAtlasIndex       >= nbUV || 
                            backTopRightCornerAtlasIndex        >= nbUV || 
                            backMiddleLeftCornerAtlasIndex      >= nbUV || 
                            backMiddleMiddleCornerAtlasIndex    >= nbUV || 
                            backMiddleRightCornerAtlasIndex     >= nbUV || 
                            backBottomLeftCornerAtlasIndex      >= nbUV || 
                            backBottomMiddleCornerAtlasIndex    >= nbUV || 
                            backBottomRightCornerAtlasIndex     >= nbUV);

        Vec2d       uvBack[36];
        Vec2d       uvWithAtplas[36];
        Vec2d *     uv1;
        Vec2d *     uv2;

        if (useAtlas)
        {
            Vec2d *     uvWithAtplasPtr    = uvWithAtplas;
            const UVAtlas*    uvAtlas      = textBack->getUVAtlas();

            uvAtlas->get4UVAt(backTopLeftCornerAtlasIndex     , uvWithAtplasPtr); processRotate(uvWithAtplasPtr, getTemplate()->getBackTopLeftCornerAtlasRotate()      ); uvWithAtplasPtr+=4;
            uvAtlas->get4UVAt(backTopMiddleCornerAtlasIndex   , uvWithAtplasPtr); processRotate(uvWithAtplasPtr, getTemplate()->getBackTopMiddleCornerAtlasRotate()    ); uvWithAtplasPtr+=4;
            uvAtlas->get4UVAt(backTopRightCornerAtlasIndex    , uvWithAtplasPtr); processRotate(uvWithAtplasPtr, getTemplate()->getBackTopRightCornerAtlasRotate()     ); uvWithAtplasPtr+=4;
            uvAtlas->get4UVAt(backMiddleLeftCornerAtlasIndex  , uvWithAtplasPtr); processRotate(uvWithAtplasPtr, getTemplate()->getBackMiddleLeftCornerAtlasRotate()   ); uvWithAtplasPtr+=4;
            uvAtlas->get4UVAt(backMiddleMiddleCornerAtlasIndex, uvWithAtplasPtr); processRotate(uvWithAtplasPtr, getTemplate()->getBackMiddleMiddleCornerAtlasRotate() ); uvWithAtplasPtr+=4;
            uvAtlas->get4UVAt(backMiddleRightCornerAtlasIndex , uvWithAtplasPtr); processRotate(uvWithAtplasPtr, getTemplate()->getBackMiddleRightCornerAtlasRotate()  ); uvWithAtplasPtr+=4;
            uvAtlas->get4UVAt(backBottomLeftCornerAtlasIndex  , uvWithAtplasPtr); processRotate(uvWithAtplasPtr, getTemplate()->getBackBottomLeftCornerAtlasRotate()   ); uvWithAtplasPtr+=4;
            uvAtlas->get4UVAt(backBottomMiddleCornerAtlasIndex, uvWithAtplasPtr); processRotate(uvWithAtplasPtr, getTemplate()->getBackBottomMiddleCornerAtlasRotate() ); uvWithAtplasPtr+=4;
            uvAtlas->get4UVAt(backBottomRightCornerAtlasIndex , uvWithAtplasPtr); processRotate(uvWithAtplasPtr, getTemplate()->getBackBottomRightCornerAtlasRotate()  ); uvWithAtplasPtr+=4;
        }


        u32 gridXLen = 2*GRID_X;
        u32 gridYLen = 2*GRID_Y;
        u32 idxTab[]   = {  0,  3,      4,  7,      8,  11,
                            1,  2,      5,  6,      9,  10,

                            12, 15,     16, 19,     20, 23,
                            13, 14,     17, 18,     21, 22,

                            24, 27,     28, 31,     32, 35,
                            25, 26,     29, 30,     33, 34 };
        u32 idxInTab;

        {
            // between 0 - 1
            f32 invTotalWidth  = 1.f/m_trueSize.x();
            f32 invTotalHeight = 1.f/m_trueSize.y();
            f32 w1 = leftWidth               * invTotalWidth;
            f32 w2 = (leftWidth + midWidth)  * invTotalWidth;
            f32 h1 = topHeight               * invTotalHeight;
            f32 h2 = (topHeight + midHeight) * invTotalHeight;

            f32 uvW[2*GRID_X] = { 0.f, w1,w1,  w2,w2, 1.f };
            f32 uvH[2*GRID_Y] = { 0.f, h1,h1,  h2,h2, 1.f };

            for (u32 y=0; y<gridYLen; y++)
            {
                for (u32 x=0; x<gridXLen; x++)
                {
                    idxInTab = idxTab[x + y*gridXLen];
                    uvBack[idxInTab] = Vec2d(uvW[x], uvH[y]);
                }
            }
        }

        if (useAtlas)
        {
            if (getTemplate()->getBackTopMiddleCornerAtlasTile())
            {
                bbool isRotate = (getTemplate()->getBackTopMiddleCornerAtlasRotate() % 180) != 0;
                processTile(uvWithAtplas, idxTab, 2, 0, midWidth, midHeight, !isRotate, isRotate);
            }

            if (getTemplate()->getBackBottomMiddleCornerAtlasTile())
            {
                bbool isRotate = (getTemplate()->getBackBottomMiddleCornerAtlasRotate() % 180) != 0;
                processTile(uvWithAtplas, idxTab, 2, 4, midWidth, midHeight, !isRotate, isRotate);
            }

            if (getTemplate()->getBackMiddleLeftCornerAtlasTile())
            {
                bbool isRotate = (getTemplate()->getBackMiddleLeftCornerAtlasRotate() % 180) != 0;
                processTile(uvWithAtplas, idxTab, 0, 2, midWidth, midHeight, isRotate, isRotate);
            }

            if (getTemplate()->getBackMiddleRightCornerAtlasTile())
            {
                bbool isRotate = (getTemplate()->getBackMiddleRightCornerAtlasRotate() % 180) != 0;
                processTile(uvWithAtplas, idxTab, 4, 2, midWidth, midHeight, isRotate, isRotate );
            }
        }

        uv1 = uvBack;
        uv2 = useAtlas ? uvWithAtplas : uvBack;

        VertexPC2T * write;
        m_boxMesh.LockVertexBuffer((void **)&write);

        u32 defaultColor = Color::white().getAsU32();
      
        Vec2d center = Vec2d(0.5f, 0.5f);
        for (u32 index = 0; index < ITF_ARRAY_SIZE(idxTab); index++, write++)
        {
            idxInTab = idxTab[index];
            write->setData(((uvBack[idxInTab]-center)*m_trueSize).to3d(), uv1[idxInTab], uv2[idxInTab], defaultColor); 
        }

        if (m_useFrontTexture)
        {
            for (u32 index = 0; index < ITF_ARRAY_SIZE(idxTab); index++, write++)
            {
                idxInTab = idxTab[index];
                write->setData(((uvBack[idxInTab]-center)*m_trueSize).to3d(), uv2[idxInTab], uv2[idxInTab], defaultColor);
            }
        }

        m_boxMesh.UnlockVertexBuffer();
        m_boxMesh.setCommonParam(m_primitiveParam);
        if (m_primitiveParam.m_colorFactor.getAlpha() <= 1.f - MTH_EPSILON)
            m_boxMesh.adjustZPassFilterFlag(GFX_OCCLUDE_INFO_SMALL_OR_TRANSPARENT);

        updateAnimMaterial(btrue);
        computeShadowMesh();
        computeHighlightMesh();
    }

    void RenderBoxComponent::updateAnimMaterial(bbool _forceFull)
    {
        // anim material
        GFX_UVANIM & uvAnim = m_boxMesh.getMaterial(0).getUVAnim(0);
        uvAnim.m_uvFlagAnim     = 0;
        uvAnim.m_pivot          = m_uvPivot;
        uvAnim.m_scale          = Vec2d(1.f, 1.f);
        uvAnim.m_preScale       = m_uvRatio * m_trueSize;
        uvAnim.m_preTranslate   = m_uvPreTranslation;

        uvAnim.m_lastAnimatedFrame = CURRENTFRAME;
        if (m_uvTranslationSpeed != Vec2d::Zero)
        {
            uvAnim.m_uvFlagAnim |= GFX_UVANIM::UVANIM_F_TRANS;
            uvAnim.m_speedTrans = m_uvTranslationSpeed;
        } else if (_forceFull)
        {
            uvAnim.m_pos            = m_uvTranslation;
        }

        if (m_uvRotationSpeed.ToRadians() != 0.f)
        {
            uvAnim.m_uvFlagAnim |= GFX_UVANIM::UVANIM_F_ROTATE;
            uvAnim.m_speedRotate = -m_uvRotationSpeed.ToRadians();
        } else if (_forceFull)
        {
            uvAnim.m_currentangle   = -m_uvRotation.ToRadians();
        }

        if (uvAnim.m_uvFlagAnim == 0)
        {
            uvAnim.m_uvFlagAnim = GFX_UVANIM::UVANIM_F_MANUAL;
        }

        m_needRefreshUvMatrix = bfalse;
    }

    void RenderBoxComponent::computeShadowMesh()
    {
        if (!m_useShadow)
            return;

        m_shadowMesh.clearMeshElementList();
        m_shadowMesh.clearMaterialList();
        m_shadowMesh.copyVBFromMesh(m_boxMesh);

		if(!m_overwrittenmaterial_serialized.getTexturePathSet().getTexturePath().isEmpty())
			m_shadowMesh.addElementAndMaterial(m_overwrittenmaterial);
		else
		{
			m_shadowMesh.addElementAndMaterial(getTemplate()->getMaterial());
			for(u32 i = 0; i < getTemplate()->getAdditionalMaterialSize(); i++)
			{
				m_shadowMesh.addElementAndMaterial(getTemplate()->getAdditionalMaterial(i));
			}
		}


        if (m_useFrontTexture && getTemplate()->shadowUseFrontTexture())
        {
            m_shadowMesh.addElementAndMaterial(getTemplate()->getFrontMaterial());
        }

        m_shadowMesh.getMeshElement(0).m_indexBuffer = m_boxMesh.getMeshElement(0).m_indexBuffer;
        m_shadowMesh.getMeshElement(0).m_count = m_boxMesh.getMeshElement(0).m_count;
        if (m_useFrontTexture && getTemplate()->shadowUseFrontTexture())
        {
            m_shadowMesh.getMeshElement(m_currentFrontIndex).m_indexBuffer   = m_boxMesh.getMeshElement(m_currentFrontIndex).m_indexBuffer;
            m_shadowMesh.getMeshElement(m_currentFrontIndex).m_count         = m_boxMesh.getMeshElement(m_currentFrontIndex).m_count;
        }

        GFXPrimitiveParam primitiveParam = m_primitiveParam;
        primitiveParam.m_colorFactor = Color::black();
        primitiveParam.m_colorFactor.setAlpha(m_shadowAlpha * getAlpha());

        m_shadowMesh.setCommonParam(primitiveParam);
        m_shadowMesh.adjustZPassFilterFlag(GFX_OCCLUDE_INFO_SMALL_OR_TRANSPARENT);
    }


    void RenderBoxComponent::computeHighlightMesh()
    {
        if (!m_useHighlight || m_highlightAlpha != 0.f)
		{
#ifdef ITF_PROJECT_POP
			setAlpha(m_alphaInit);
#endif
            return;
		}

        m_highlightMesh.clearMeshElementList();
        m_highlightMesh.clearMaterialList();

        m_highlightMesh.copyVBFromMesh(m_boxMesh);
        m_highlightMesh.addElementAndMaterial(getTemplate()->getHighlightMaterial());

        m_highlightMesh.getMeshElement(0).m_indexBuffer = m_boxMesh.getMeshElement(0).m_indexBuffer;
        m_highlightMesh.getMeshElement(0).m_count = m_boxMesh.getMeshElement(0).m_count;

        GFXPrimitiveParam primitiveParam = m_primitiveParam;
        primitiveParam.m_colorFactor.setAlpha(m_highlightAlpha * getAlpha());

        m_highlightMesh.setCommonParam(primitiveParam);
        m_highlightMesh.adjustZPassFilterFlag(GFX_OCCLUDE_INFO_SMALL_OR_TRANSPARENT);
    }

    void RenderBoxComponent::updateAABB()
    {
        AABB aabb(Vec2d::Zero, m_trueSize);
        aabb.moveCenter(Vec2d::Zero);
        if(GetActor()->getIs2D())        
            aabb.Scale(GetActor()->getScale() * g_pixelBy2DUnit);
        else
            aabb.Scale(GetActor()->getScale());
        aabb.Rotate(GetActor()->getAngle(), bfalse);
        aabb.Translate(GetActor()->get2DPos() + (m_offset  + getFinalInvShadowOffset().truncateTo2D()) * GetActor()->getScale());

        GetActor()->growAABB(aabb);
    }

    void RenderBoxComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

        if (m_colorTimeLeft > 0.f && m_colorTimeTotal > 0.f)
        {
            m_colorTimeLeft -= _dt;
            if (m_colorTimeLeft < 0.f)
                m_colorTimeLeft = 0.f;

            m_primitiveParam.m_colorFactor = (m_colorSrc * m_colorTimeLeft + m_colorDst * (m_colorTimeTotal - m_colorTimeLeft))*f32_Inv(m_colorTimeTotal);
        }


        updateAABB();
    }


    void RenderBoxComponent::setColor(const Color & _color, f32 _time)
    {
        m_colorTimeTotal    = _time;
        m_colorTimeLeft     = _time;
        m_colorSrc          = m_primitiveParam.m_colorFactor;
        m_colorDst          = _color;
    }

    Vec3d RenderBoxComponent::getFinalShadowOffset()
    {
        if (getTemplate()->isPosShadowRelative())
        {
            return Vec3d::Zero;
        } else
        {
            return m_shadowOffset;
        }
    }

    Vec3d RenderBoxComponent::getFinalInvShadowOffset()
    {
        if (getTemplate()->isPosShadowRelative())
        {
            return -m_shadowOffset;
        } else
        {
            return Vec3d::Zero;
        }
    }


    void RenderBoxComponent::updateMeshInfo(bbool _is2D)
    {
        // update Position matrix
        if (_is2D)
        {
             m_boxMesh.set2DPosAndScale(GetActor()->get2DPos() + (m_offset  + getFinalInvShadowOffset().truncateTo2D()) * GetActor()->getScale(), GetActor()->getScale() * g_pixelBy2DUnit);
             m_boxMesh.set2DRotation(GetActor()->getAngle());
             if (m_useShadow)
             {
                 m_shadowMesh.set2DPosAndScale(GetActor()->get2DPos() + (m_offset + getFinalShadowOffset().truncateTo2D())*GetActor()->getScale(), GetActor()->getScale() * g_pixelBy2DUnit);
                 m_shadowMesh.set2DRotation(GetActor()->getAngle());
             }
             if (m_useHighlight)
             {
                 m_highlightMesh.set2DPosAndScale(GetActor()->get2DPos() + (m_offset  + getFinalInvShadowOffset().truncateTo2D() + m_highlightOffset.truncateTo2D())*GetActor()->getScale(), m_highlightScale * GetActor()->getScale() * g_pixelBy2DUnit);
                 m_highlightMesh.set2DRotation(GetActor()->getAngle());
             }

        } else
        {
            Matrix44 mat;

            mat.setRotationZ(GetActor()->getAngle());
            mat.setTranslation(GetActor()->getPos() + (m_offset * GetActor()->getScale()).to3d() + getFinalInvShadowOffset());
            // In mesh y is inverted, so do inversion to have top on top
            Vec2d scale = GetActor()->getScale();
            scale.y() *= -1.f;

            mat.mulScale(scale.to3d(1.f));
            m_boxMesh.setMatrix(mat);

            if (m_useShadow)
            {
                mat.setTranslation(GetActor()->getPos() + (m_offset * GetActor()->getScale()).to3d() + getFinalShadowOffset());
                m_shadowMesh.setMatrix(mat);
            }
            if (m_useHighlight)
            {
                mat.setTranslation(GetActor()->getPos() + (m_offset * GetActor()->getScale()).to3d() + m_highlightOffset + getFinalInvShadowOffset());
                mat.mulScale(m_highlightScale.to3d(1.f));
                m_highlightMesh.setMatrix(mat);
            }
        }
        
        computeAMVPositions();

        m_boxMesh.getCommonParam()->m_colorFactor = m_primitiveParam.m_colorFactor;
        m_boxMesh.getCommonParam()->m_colorFactor.m_a = m_primitiveParam.m_colorFactor.m_a * m_alpha;
        if (m_useShadow)       
            m_shadowMesh.getCommonParam()->m_colorFactor.m_a = m_shadowAlpha * m_alpha;

        if (m_useHighlight)       
            m_highlightMesh.getCommonParam()->m_colorFactor.m_a = m_highlightAlpha * m_alpha;

        if (m_needRefreshUvMatrix)
            updateAnimMaterial();
    }

    void RenderBoxComponent::batchPrimitives(const ITF_VECTOR <class View*>& _views)
    {
        Super::batchPrimitives(_views);

        if (!m_drawEnabled || m_alpha < MTH_EPSILON)
            return;

        updateMeshInfo(bfalse);
        if (m_useShadow && m_shadowAlpha > 0.f)
        {
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_shadowMesh, m_actor->getDepth() + getDepthOffset() + getTemplate()->getZOffset() + m_shadowOffset.z(), m_actor->getRef());
        }
        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_boxMesh, m_actor->getDepth() + getDepthOffset() + getTemplate()->getZOffset(), m_actor->getRef());
        if (m_useHighlight && m_highlightAlpha > 0.f)
        {
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_highlightMesh, m_actor->getDepth() + getDepthOffset() + getTemplate()->getZOffset() + m_highlightOffset.z(), m_actor->getRef());
        }
    }

    void RenderBoxComponent::batchPrimitives2D(const ITF_VECTOR <class View*>& _views)
    {
        Super::batchPrimitives2D(_views);

        if (!m_drawEnabled || m_alpha < MTH_EPSILON)
            return;

        updateMeshInfo(btrue);
        if (m_useShadow && m_shadowAlpha > 0.f)
        {
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_2D>(_views, &m_shadowMesh, m_actor->getDepth() + getDepthOffset() + getTemplate()->getZOffset() + m_shadowOffset.z(), m_actor->getRef());
        }
        GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_2D>(_views, &m_boxMesh, m_actor->getDepth() + getDepthOffset() + getTemplate()->getZOffset(), m_actor->getRef());
        if (m_useHighlight && m_highlightAlpha > 0.f)
        {
            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_2D>(_views, &m_highlightMesh, m_actor->getDepth() + getDepthOffset() + getTemplate()->getZOffset() + m_highlightOffset.z(), m_actor->getRef());
        }
    }

    void RenderBoxComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if ( EventShow* eventShow = DYNAMIC_CAST(_event,EventShow) )
        {   
            setAlpha(eventShow->getAlpha());
            if (getAlpha() < 1.f - MTH_EPSILON)
                m_boxMesh.adjustZPassFilterFlag(GFX_OCCLUDE_INFO_SMALL_OR_TRANSPARENT);
            else
            {
                m_boxMesh.adjustZPassFilterFlag(m_primitiveParam.m_gfxOccludeInfo);
            }
		}
		else if( EventViewportVisibility* eventViewportVisibility = DYNAMIC_CAST(_event,EventViewportVisibility) )
		{
			m_primitiveParam.m_viewportVisibility = eventViewportVisibility->getViewportVisibility();
		}
    }

    bbool RenderBoxComponent::needsDraw() const
    {
        return !getTemplate()->is2D();
    }

    bbool RenderBoxComponent::needsDraw2D() const 
    {
        return getTemplate()->is2D(); 
    }

    void RenderBoxComponent::setSize( Vec2d _size )
    {
        // Size must be set in local
        if (GetActor()->getIs2D())
        {
            _size *= GetActor()->currentResolutionToReferenceFactor()/g_pixelBy2DUnit;
        }
        if (getTemplate()->addBordersOnSetSize())
        {
            _size.x() += getTemplate()->getRightWidth() + getTemplate()->getLeftWidth();
            _size.y() += getTemplate()->getTopHeight() + getTemplate()->getBottomHeight();
        }
        if (!m_size.IsEqual(_size, MTH_EPSILON))
        { 
            m_size = _size;
            
            computeBox();
            computeAMV();
        }
    }

    Vec2d RenderBoxComponent::getSize() const
    {
        Vec2d size = m_size;

        if (getTemplate()->addBordersOnSetSize())
        {
            size.x() -= getTemplate()->getRightWidth() + getTemplate()->getLeftWidth();
            size.y() -= getTemplate()->getTopHeight() + getTemplate()->getBottomHeight();
        }
        if(GetActor()->getIs2D())
        {
            size *= g_pixelBy2DUnit;
            size /= GetActor()->currentResolutionToReferenceFactor();
        }

        return size;
    }

    void RenderBoxComponent::setOffset(const Vec2d & _offset)
    {
        if (GetActor()->getIs2D())
            m_offset = GetActor()->currentResolution2dPosToReference(_offset);
        else
            m_offset = _offset;
    }

    Vec2d RenderBoxComponent::getOffset() const
    {
        Vec2d offset = m_offset;

        if (GetActor()->getIs2D())
        {
            offset *= GetActor()->referenceToCurrentResolutionFactor();
        }

        return offset;
    }

    void RenderBoxComponent::setShadowOffset( const Vec3d & val )
    {
        if (val == m_shadowOffset)
            return;

        if (!m_useShadow)
        {
            ITF_WARNING(m_actor, 0, "Cannot set shadow offset on box with no default shadow !");
            return;
        }
        m_shadowOffset = val;
    }

    void RenderBoxComponent::setShadowAlpha( f32 val )
    {
        if (val == m_shadowAlpha)
            return;

        if (!m_useShadow)
        {
            ITF_WARNING(m_actor, 0, "Cannot set shadow alpha on box with no default shadow !");
            return;
        }
        m_shadowAlpha = val;
    }


    void RenderBoxComponent::setHighlightOffset( const Vec3d & val )
    {
        if (val == m_highlightOffset)
            return;

        if (!m_useHighlight)
        {
            ITF_WARNING(m_actor, 0, "Cannot set highlight offset on box with no default highlight !");
            return;
        }
        m_highlightOffset = val;
    }

    void RenderBoxComponent::setHighlightAlpha( f32 val )
    {
        if (val == m_highlightAlpha)
            return;

        if (!m_useHighlight)
        {
            ITF_WARNING(m_actor, 0, "Cannot set highlight alpha on box with no default highlight !");
            return;
        }
        m_highlightAlpha = val;
    }

    void RenderBoxComponent::setHighlightScale( const Vec2d & val )
    {
        if (val == m_highlightScale)
            return;

        if (!m_useHighlight)
        {
            ITF_WARNING(m_actor, 0, "Cannot set highlight scale on box with no default highlight !");
            return;
        }
        m_highlightScale = val;
    }

}
