#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#include "engine/actors/components/texturegraphiccomponent2D.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT2D_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

namespace ITF
{
#define UI2D_WIDTH_REF  1280.0f
#define UI2D_HEIGHT_REF 720.0f

    IMPLEMENT_OBJECT_RTTI(TextureGraphicComponent2D)
        BEGIN_SERIALIZATION_CHILD(TextureGraphicComponent2D)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_ENUM_BEGIN("ALIGN", m_align);
    SERIALIZE_ENUM_VAR(align_free);
    SERIALIZE_ENUM_VAR(align_centerX);
    SERIALIZE_ENUM_VAR(align_centerY);
    SERIALIZE_ENUM_VAR(align_centerXY);
    SERIALIZE_ENUM_END();
    SERIALIZE_MEMBER("SCREEN_POURCENT_X",m_screenPourcentX);
    SERIALIZE_MEMBER("SCREEN_POURCENT_Y",m_screenPourcentY);            
    SERIALIZE_MEMBER("texture",m_instanceFile);
    SERIALIZE_MEMBER("quadSize",m_quadSize);
    END_CONDITION_BLOCK()

        END_SERIALIZATION()

        TextureGraphicComponent2D::TextureGraphicComponent2D()
        : Super()
        , m_depthRank(0)
        , m_align(align_free)
        , m_screenPourcentX(0.0f)
        , m_screenPourcentY(0.0f)
        , m_width(0.0f)
        , m_height(0.0f)
        , m_isWaitingForTex(bfalse)
        , m_quadSize(Vec2d::Zero)

    {}

    TextureGraphicComponent2D::~TextureGraphicComponent2D()
    {
    }

    void TextureGraphicComponent2D::onActorClearComponents()
    {
        Super::onActorClearComponents();

        if ( m_textureID.isValid() && !m_instanceFile.isEmpty() )
        {
            m_actor->removeResource(m_textureID);
            m_textureID.invalidate();
        }
    }

    void TextureGraphicComponent2D::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        if ( !m_instanceFile.isEmpty() )
        {
            m_textureID = m_actor->addResource(Resource::ResourceType_Texture, m_instanceFile);
        }
        else if ( getTemplate()->getTextureID().isValid() )
        {
            m_textureID = getTemplate()->getTextureID();
        }

        m_depthRank = getTemplate()->getDepthRank();
        m_isWaitingForTex = btrue; 
    }

    Texture* TextureGraphicComponent2D::getTexture()
    {
        return static_cast<Texture*>(m_textureID.getResource());        
    }

    void TextureGraphicComponent2D::setScreenPourcentValues()
    {
        Texture * tex = getTexture();
        if (tex && tex->isPhysicallyLoaded())
        {

            const Vec2d scale = m_actor->getScale();

            int Screen_W = 0, Screen_H = 0;
            SYSTEM_ADAPTER->getWindowSize(Screen_W, Screen_H);

#ifdef ITF_WINDOWS
            if(m_quadSize == Vec2d::Zero)
            {
                m_quadSize.x() = (f32)tex->getSizeX();
                m_quadSize.y() = (f32)tex->getSizeY();
            }
#endif //ITF_WINDOWS

            m_screenPourcentX = (100.0f * m_quadSize.x() * scale.x()) / UI2D_WIDTH_REF;
            m_screenPourcentY = (100.0f * m_quadSize.y() * scale.y()) / UI2D_HEIGHT_REF;

            m_width = Screen_W * m_screenPourcentX * 0.01f;
            m_height = Screen_H * m_screenPourcentY * 0.01f;
        }
    }

    void  TextureGraphicComponent2D::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);        

        // Update AABB
        Texture * tex = getTexture();
        if (tex && tex->isPhysicallyLoaded())
        {
            if(m_isWaitingForTex)
            {
                setScreenPourcentValues();
                m_isWaitingForTex = bfalse;
            }

            int Screen_W = 0, Screen_H = 0;
            SYSTEM_ADAPTER->getWindowSize(Screen_W, Screen_H);

            // Compute texture size displayed on screen depending on its screen pourcentage
            m_width = Screen_W * m_screenPourcentX * 0.01f;
            m_height = Screen_H * m_screenPourcentY * 0.01f;

            updateAABB();
        }
    }

    void TextureGraphicComponent2D::updateAABB()
    {
        const f32 halfTexWidth = m_width * 0.5f;
        const f32 halfTexHeight = m_height * 0.5f;

        const Vec2d vMin = Vec2d(m_actor->getPos().x() - halfTexWidth, m_actor->getPos().y() - halfTexHeight); 
        const Vec2d vMax = Vec2d(m_actor->getPos().x() + halfTexWidth, m_actor->getPos().y() + halfTexHeight); 

        const AABB aabb(Vec2d(vMin.x(),vMin.y()),Vec2d(vMax.x(),vMax.y()));
        m_actor->growAABB(aabb);
    }

#ifdef ITF_SUPPORT_EDITOR
    void TextureGraphicComponent2D::onEditorCreated( class Actor* _original )
    {
        Super::onEditorCreated(_original);

        // Calculate the relative position
        Texture * tex = getTexture();
        if (tex && tex->isPhysicallyLoaded())
        {
            setScreenPourcentValues();
            m_isWaitingForTex = bfalse;
        } else        
            m_isWaitingForTex = btrue;        
    }

    void TextureGraphicComponent2D::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        // Calculate the relative position
        Texture * tex = getTexture();
        if (tex && tex->isPhysicallyLoaded())
        {
            setScreenPourcentValues();
        }

        updateAABB();
    }

#endif // ITF_SUPPORT_EDITOR

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(TextureGraphicComponent2D_Template)
        BEGIN_SERIALIZATION_CHILD(TextureGraphicComponent2D_Template)
        SERIALIZE_MEMBER("texture",m_textureFile);
    SERIALIZE_MEMBER("rank",m_depthRank);
    END_SERIALIZATION()

    TextureGraphicComponent2D_Template::TextureGraphicComponent2D_Template():
    m_depthRank(0)
    {
    }

    bbool TextureGraphicComponent2D_Template::onTemplateLoaded( bbool _hotReload )
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);

        if ( !getTextureFile().isEmpty() )
        {
            m_textureID = m_actorTemplate->addResource(Resource::ResourceType_Texture, getTextureFile());
        }

        return bOk;
    }

    void TextureGraphicComponent2D_Template::onTemplateDelete( bbool _hotReload )
    {
        Super::onTemplateDelete(_hotReload);

        if (m_textureID.isValid())
        {
            m_actorTemplate->removeResource(m_textureID);
            m_textureID.invalidate();
        }
    }

}

