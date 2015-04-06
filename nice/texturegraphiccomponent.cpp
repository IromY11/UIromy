#include "precompiled_engine.h"

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT_H_

#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(TextureGraphicComponent)
    BEGIN_SERIALIZATION_CHILD(TextureGraphicComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
                SERIALIZE_MEMBER("texture", m_materialSerializable.getTexturePathSet().getTexturePath());
            END_CONDITION_BLOCK();
            SERIALIZE_ENUM_BEGIN("anchor",m_anchor);
            SERIALIZE_ENUM_VAR(TEXTURE_ANCHOR_TOP_LEFT);
            SERIALIZE_ENUM_VAR(TEXTURE_ANCHOR_MIDDLE_CENTER);
            SERIALIZE_ENUM_VAR(TEXTURE_ANCHOR_MIDDLE_LEFT);
            SERIALIZE_ENUM_VAR(TEXTURE_ANCHOR_MIDDLE_RIGHT);
            SERIALIZE_ENUM_VAR(TEXTURE_ANCHOR_TOP_CENTER);
            SERIALIZE_ENUM_VAR(TEXTURE_ANCHOR_TOP_RIGHT);
            SERIALIZE_ENUM_VAR(TEXTURE_ANCHOR_BOTTOM_CENTER);
            SERIALIZE_ENUM_VAR(TEXTURE_ANCHOR_BOTTOM_LEFT);
            SERIALIZE_ENUM_VAR(TEXTURE_ANCHOR_BOTTOM_RIGHT);
            SERIALIZE_ENUM_END(); 
            SERIALIZE_OBJECT("material", m_materialSerializable);
            SERIALIZE_MEMBER("spriteIndex",m_spriteIndex); 
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    TextureGraphicComponent::TextureGraphicComponent()
        : Super()
        , m_spriteIndex(U32_INVALID)
        , m_newSpriteIndex(U32_INVALID)
        , m_curAngle(Vec3d::Zero)
        , m_anchor(TEXTURE_ANCHOR_MIDDLE_CENTER)
        , m_isDrawEnabled(btrue)
        , m_offset(Vec2d::Zero)
        , m_scale(Vec2d::One)
    {
    }

    TextureGraphicComponent::~TextureGraphicComponent()
    {
    }

    void TextureGraphicComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        GFX_BLENDMODE blendMode = getTemplate()->getBlendMode();
        if (m_materialSerializable.getTexturePathSet().getTexturePath().isEmpty())
        {
            setMaterial(getTemplate()->getMaterial());
        }
        else
        {
            m_materialSerializable.onLoaded(m_actor->getResourceContainer());
            setMaterial(m_materialSerializable);
            blendMode = m_materialSerializable.getBlendMode();
        }

        m_curAngle.x() = getTemplate()->getAngleX().ToRadians();
        m_curAngle.y() = getTemplate()->getAngleY().ToRadians();
        m_curAngle.z() = getTemplate()->getAngleZ().ToRadians();

        m_quad.m_blendMode      = blendMode;
        m_quad.m_size           = Vec2d::Zero;

        if(Texture* texture = m_material.getTexture())
        {
            m_quad.m_size       = getTemplate()->getSize();
        }

        m_quad.m_pos           = -getTemplate()->getPosOffset().to3d(0) - m_offset.to3d(0);
        m_quad.m_pos           += computeMiddleCenterOffset(m_quad.m_size).to3d(0);
        m_actor->transformLocalPosToWorld(m_quad.m_pos);
        m_quad.m_pos.z()        = 0.0f;
        m_quad.m_size          *= m_actor->getScale() * m_scale;

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,EventShow_CRC,this);

        m_newSpriteIndex = m_spriteIndex;

        updateAABB();
    }


    void TextureGraphicComponent::onActorClearComponents()
    {
        if (!m_materialSerializable.getTexturePathSet().getTexturePath().isEmpty())
        {
            m_materialSerializable.onUnLoaded(m_actor->getResourceContainer());
        }

        Super::onActorClearComponents();
    }

	void TextureGraphicComponent::setOverideMaterialTexture(Texture* _texture)
	{
		m_material.setTexture(TEXSET_ID_DIFFUSE, _texture);
	}

    void TextureGraphicComponent::setTextureFile(const Path& _textureFile)
    {
        if(m_materialSerializable.getTexturePathSet().getTexturePath() != _textureFile)
        {
            if (m_actor && !m_materialSerializable.getTexturePathSet().getTexturePath().isEmpty())
            {
                m_materialSerializable.onUnLoaded(m_actor->getResourceContainer());
            }

            m_materialSerializable.getTexturePathSet().setTexturePath(_textureFile);

            if (m_actor && !m_materialSerializable.getTexturePathSet().getTexturePath().isEmpty())
            {
                m_materialSerializable.onLoaded(m_actor->getResourceContainer());
            }

            setMaterial(m_materialSerializable);
        }
    }

    void TextureGraphicComponent::setSwapMaterial(const GFX_MATERIAL* _material)
    {
        if(_material)
        {
            setMaterial(*_material);
        }
        else
        {
            if (m_materialSerializable.getTexturePathSet().getTexturePath().isEmpty())
            {
                setMaterial(getTemplate()->getMaterial());
            }
            else
            {
                setMaterial(m_materialSerializable);
            }
        }
    }

    void TextureGraphicComponent::onResourceLoaded()
    {
        Super::onResourceLoaded();

        setNewSpriteIndex(m_newSpriteIndex);
    }

    bbool TextureGraphicComponent::setNewSpriteIndex(u32 _idx)
    {
        bbool isValid = bfalse;

        if ( const Texture* texture = m_material.getTexture() )
        {
            if ( const UVAtlas* atlas = texture->getUVAtlas() )
            {
                if ( atlas->isIndexValid(_idx) )
                {
                    m_quad.m_uvData = atlas->getUVDataAt(_idx);
                    isValid = btrue;
                }
            }
        }

        m_newSpriteIndex = _idx;

        return isValid;
    }

    void TextureGraphicComponent::resetQuad()
    {
        m_quad.m_uvData.setUVdata(Vec2d::Zero, Vec2d::One);
    }

    void TextureGraphicComponent::setMaterial(const GFX_MATERIAL& _material)
    {
        m_material = _material;
        m_quad.setMaterial(&m_material);
    }

    void TextureGraphicComponent::batchPrimitives( const ITF_VECTOR <class View*>& _views )
    {
        Super::batchPrimitives(_views);

        if(m_isDrawEnabled)
        {
			Texture *tex = m_material.getTexture(TEXSET_ID_DIFFUSE);
			if(!tex || !tex->isPhysicallyLoaded())
				return;

            if (m_renderInTarget)
            {
                // Render Target support -> to do
                //GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_RTARGET>(_views, &m_quad, GetActor()->getDepth() + getTemplate()->getZOffset(), ObjectRef::InvalidRef);
                GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_quad, GetActor()->getDepth() + getDepthOffset() + getTemplate()->getZOffset(), GetActor()->getRef());
            }
            else
                GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_MAIN>(_views, &m_quad, GetActor()->getDepth() + getDepthOffset() + getTemplate()->getZOffset(), GetActor()->getRef());
        }
    }

    void TextureGraphicComponent::batchPrimitives2D( const ITF_VECTOR <class View*>& _views )
    {
        Super::batchPrimitives2D(_views);

        if(m_isDrawEnabled)
        {
			Texture *tex = m_material.getTexture(TEXSET_ID_DIFFUSE);
			if(!tex || !tex->isPhysicallyLoaded())
				return;

            Quad2DInfo & quad2D = m_quad2DPrim.getQuadInfo();
            quad2D.m_color = m_quad.m_color;
            quad2D.m_height = m_quad.m_size.y();
            quad2D.m_width = m_quad.m_size.x();
            Vec2d basePos = m_quad.m_pos.truncateTo2D();
            if (GetActor()->getAngle() != 0.f)
            {
                // In 2d pos do not need to be rotated
                basePos = basePos.RotateAround(GetActor()->get2DPos(), -GetActor()->getAngle());
            }
            quad2D.m_pos2D = (basePos - m_quad.m_size * 0.5f).to3d();
            quad2D.m_angle = m_quad.m_rotation.z();
            quad2D.m_pivot = GetActor()->get2DPos();
			quad2D.m_texture = quad2D.m_texture = m_material.getTexture();
			quad2D.m_uvStart = m_actor->getIsFlipped() ? Vec2d(m_quad.m_uvData.getUV1().x(), m_quad.m_uvData.getUV0().y()): m_quad.m_uvData.getUV0();
			quad2D.m_uvEnd = m_actor->getIsFlipped() ? Vec2d(m_quad.m_uvData.getUV0().x(), m_quad.m_uvData.getUV1().y()): m_quad.m_uvData.getUV1();
            m_quad2DPrim.setCommonParam(*m_quad.getCommonParam());

            GFX_ADAPTER->getZListManager().AddPrimitiveInZList<GFX_ZLIST_2D>(_views, &m_quad2DPrim, m_actor->getDepth() + getDepthOffset() + getTemplate()->getZOffset(), GetActor()->getRef());
        }
    }

    void TextureGraphicComponent::Update( f32 _deltaTime )
    {
        PRF_M_SCOPE(updateTextureGraphic)

        Super::Update( _deltaTime );

        // setup quad information.
        GFXPrimitiveParam primitiveParam = getGfxPrimitiveParam();
        primitiveParam.m_BV = m_actor->getAABB();
        m_quad.setCommonParam(primitiveParam);

        Color col = getTemplate()->getDefaultColor() * primitiveParam.m_colorFactor;
        col.m_a *= m_alpha;
        m_quad.m_color          = col.getAsU32();
        m_quad.m_size           = Vec2d::Zero;

        if(Texture* texture = m_material.getTexture())
        {
            m_quad.m_size       = getTemplate()->getSize();
        }
#ifdef ITF_SUPPORT_EDITOR
        else if(m_actor->isSelected())
        {
            m_quad.m_size       = getTemplate()->getSize();
        }
#endif
        m_quad.m_pos            = -getTemplate()->getPosOffset().to3d(0) - m_offset.to3d(0);
        m_quad.m_pos           += computeMiddleCenterOffset(m_quad.m_size).to3d(0);
        m_actor->transformLocalPosToWorld(m_quad.m_pos);
        m_quad.m_size          *= m_actor->getScale() * m_scale;
        m_quad.m_rotation.x()   = m_curAngle.x() + (m_actor->getIsFlipped() && !m_actor->getIs2D() ? MTH_PI : 0.f );
        m_quad.m_rotation.y()   = m_curAngle.y();
        m_quad.m_rotation.z()   = m_actor->getAngle() + m_curAngle.z() + (m_actor->getIsFlipped() && !m_actor->getIs2D() ? MTH_PI : 0.f );

		Vec2d UV0 = m_quad.m_uvData.getUV0();
		Vec2d UV1 = m_quad.m_uvData.getUV1();
		UV0 += m_quad.getMaterial()->getUVAnim(0).m_speedTrans * _deltaTime;
		UV1 += m_quad.getMaterial()->getUVAnim(1).m_speedTrans * _deltaTime;

        // Clamp UVs
        Vec2d m = 0.5f * (UV0 + UV1);
        if( f32_Abs(m.x()) > 8 )
        {
            UV0.x() -= f32_Sign(m.x()) * 8;
            UV1.x() -= f32_Sign(m.x()) * 8;
        }
        if( f32_Abs(m.y()) > 8 )
        {
            UV0.y() -= f32_Sign(m.y()) * 8;
            UV1.y() -= f32_Sign(m.y()) * 8;
        }

        m_quad.m_uvData.setUV0(UV0);
        m_quad.m_uvData.setUV1(UV1);

        // update angle.
        m_curAngle += _deltaTime * Vec3d(   getTemplate()->getSpeedRotX(),
                                            getTemplate()->getSpeedRotY(),
                                            getTemplate()->getSpeedRotZ());

        // update AABB
        updateAABB();
        m_quad.m_BV = m_actor->getAABB();
    }

    Vec2d TextureGraphicComponent::getSize() const
    {
        return getTemplate()->getSize();
    }

    Vec2d TextureGraphicComponent::computeMiddleCenterOffset(const Vec2d& _size) const
    {
        Vec2d halfSize = _size * 0.5f;
        Vec2d middleCenterOffset = Vec2d::Zero;

        switch(m_anchor)
        {
        case TEXTURE_ANCHOR_TOP_LEFT :
            middleCenterOffset = halfSize;
            break;
        case TEXTURE_ANCHOR_MIDDLE_CENTER :
            middleCenterOffset = Vec2d::Zero;
            break;
        case TEXTURE_ANCHOR_MIDDLE_LEFT :
            middleCenterOffset.x() = halfSize.x();
            break;
        case TEXTURE_ANCHOR_MIDDLE_RIGHT :
            middleCenterOffset.x() = -halfSize.x();
            break;
        case TEXTURE_ANCHOR_TOP_CENTER :
            middleCenterOffset.y() = halfSize.y();
            break;
        case TEXTURE_ANCHOR_TOP_RIGHT :
            middleCenterOffset.x() = -halfSize.x();
            middleCenterOffset.y() = halfSize.y();
            break;
        case TEXTURE_ANCHOR_BOTTOM_CENTER :
            middleCenterOffset.y() = -halfSize.y();
            break;
        case TEXTURE_ANCHOR_BOTTOM_LEFT :
            middleCenterOffset.x() = halfSize.x();
            middleCenterOffset.y() = -halfSize.y();
            break;
        case TEXTURE_ANCHOR_BOTTOM_RIGHT :
            middleCenterOffset.x() = -halfSize.x();
            middleCenterOffset.y() = -halfSize.y();
            break;
        }

        if(!needsDraw2D()) middleCenterOffset.y() *= -1;

        return middleCenterOffset;
    }

    void TextureGraphicComponent::updateAABB()
    {
        const Vec2d halfSize = m_quad.m_size * 0.5f;
        AABB aabb(halfSize);
        aabb.grow(-halfSize);
        aabb.Rotate(m_actor->getAngle(), btrue);

        Vec2d pos = m_quad.m_pos.truncateTo2D();
        aabb.Translate(pos);
        m_actor->growAABB(aabb);
    }


	bbool TextureGraphicComponent::needsDraw2DNoScreenRatio() const 
	{ 
		return m_actor->getIs2DNoScreenRatio() || (getTemplate() && getTemplate()->getDraw2DNoScreenRatio());
	}

    bbool TextureGraphicComponent::needsDraw2D() const
    {
        return m_actor->getIs2D() || (getTemplate() && getTemplate()->getDraw2D());
    }

    bbool TextureGraphicComponent::needsDraw() const
    {
        return !needsDraw2D();
    }

#ifdef ITF_SUPPORT_EDITOR
    void TextureGraphicComponent::onEditorCreated( class Actor* _original )
    {
        Super::onEditorCreated(_original);

        if(needsDraw2D())
        {
            Vec3d pos = m_actor->getPos();
            GFX_ADAPTER->compute3DTo2D(m_actor->getPos(), pos);
            pos.z() = 0;

            m_actor->setWorldInitialPos(pos, btrue);
            m_actor->set2DPos(pos.truncateTo2D());
        }
    }

    void TextureGraphicComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        Update(0.0f);
    }

#endif // ITF_SUPPORT_EDITOR

	void TextureGraphicComponent::getTextureFile(ITF::Path& _path) 
	{
		_path = m_materialSerializable.getTexturePathSet().getTexturePath();
	}

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(TextureGraphicComponent_Template)
    BEGIN_SERIALIZATION_CHILD(TextureGraphicComponent_Template)
        BEGIN_CONDITION_BLOCK(ESerialize_Deprecate);
            SERIALIZE_MEMBER("texture", m_material.getTexturePathSet().getTexturePath());
        END_CONDITION_BLOCK();
        SERIALIZE_OBJECT("material", m_material);
        SERIALIZE_MEMBER("defaultColor",m_defaultColor); 
        SERIALIZE_MEMBER("angleX",m_angleX); 
        SERIALIZE_MEMBER("angleY",m_angleY); 
        SERIALIZE_MEMBER("angleZ",m_angleZ); 
        SERIALIZE_MEMBER("speedRotX",m_speedRotX); 
        SERIALIZE_MEMBER("speedRotY",m_speedRotY); 
        SERIALIZE_MEMBER("speedRotZ",m_speedRotZ); 
        SERIALIZE_MEMBER("size",m_size); 
        SERIALIZE_MEMBER("zOffset",m_zOffset);
        SERIALIZE_MEMBER("draw2D", m_draw2D);
		SERIALIZE_MEMBER("draw2DNoScreenRatio", m_draw2DNoScreenRatio);
    END_SERIALIZATION()

    TextureGraphicComponent_Template::TextureGraphicComponent_Template()
        : m_defaultColor(Color::white())
        , m_angleX(btrue, 0.f)
        , m_angleY(btrue, 0.f)
        , m_angleZ(btrue, 0.f)
        , m_speedRotX(0.f)
        , m_speedRotY(0.f)
        , m_speedRotZ(0.f)
        , m_size(Vec2d::One)
        , m_zOffset(0.f)
        , m_draw2D(bfalse)
		, m_draw2DNoScreenRatio(bfalse)
    {
    }

    bbool TextureGraphicComponent_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);

        if (!m_material.getTexturePathSet().getTexturePath().isEmpty())
        {
            m_material.onLoaded(m_actorTemplate->getResourceContainer());
        }

        return bOk;
    }

    void TextureGraphicComponent_Template::onTemplateDelete( bbool _hotReload )
    {
        if (!m_material.getTexturePathSet().getTexturePath().isEmpty())
        {
            m_material.onUnLoaded(m_actorTemplate->getResourceContainer());
        }

        Super::onTemplateDelete(_hotReload);
    }

}



