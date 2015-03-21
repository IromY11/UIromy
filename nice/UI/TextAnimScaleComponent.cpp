#include "precompiled_gameplay.h"

#ifndef _ITF_TEXTANIMSCALECOMPONENT_H_
#include "gameplay/components/UI/TextAnimScaleComponent.h"
#endif //_ITF_TEXTANIMSCALECOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_TEXTBOXCOMPONENT_H_
#include "gameplay/components/UI/TextBoxComponent.h"
#endif //_ITF_TEXTBOXCOMPONENT_H_

#ifndef _ITF_MULTITEXTBOXCOMPONENT_H_
#include "gameplay/Components/UI/MultiTextBoxComponent.h"
#endif // _ITF_MULTITEXTBOXCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_
#include "core/serializer/ZSerializerObject_defines.h"

#ifndef _ITF_INTERSECTION_H_
#include "engine/physics/Collision/Intersection.h"
#endif //_ITF_INTERSECTION_H_

#ifndef _ITF_RENDERBOXCOMPONENT_H_
#include "gameplay/Components/UI/RenderBoxComponent.h"
#endif //_ITF_RENDERBOXCOMPONENT_H_

#ifndef _ITF_ANIMMESHVERTEXCOMPONENT_H_
#include "engine/actors/components/AnimMeshVertexComponent.h"
#endif //_ITF_ANIMMESHVERTEXCOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(TextAnimScaleComponent)
    BEGIN_SERIALIZATION_CHILD(TextAnimScaleComponent)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
    SERIALIZE_MEMBER("isActive", m_isActive);
    SERIALIZE_MEMBER("margingTop", m_margingTop);
    SERIALIZE_MEMBER("margingLeft", m_margingLeft);
    SERIALIZE_MEMBER("margingRight", m_margingRight);
    SERIALIZE_MEMBER("margingBottom", m_margingBottom);
    SERIALIZE_ENUM_SCALETYPE("scaleType", m_scaleType);
    SERIALIZE_BOOL("autoFillContained", m_autoFillContained);
    SERIALIZE_CONTAINER("ContainedChildren", m_containedObjectPaths);
    SERIALIZE_BOOL("autoFillReposition", m_autoFillRepositioned);
    SERIALIZE_CONTAINER("repositionObjects", m_repositionedObjectPaths);
    SERIALIZE_CONTAINER("relRepositionObjects", m_relRepositionedObjectPaths);
    SERIALIZE_MEMBER("minimumSize", m_minimumSize);
    BEGIN_CONDITION_BLOCK_NOT(ESerialize_Instance)     
    SERIALIZE_OBJECT("aabb", m_aabb);
    END_CONDITION_BLOCK_NOT()
    END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(TextAnimScaleComponent)
        bbool goodOrder = btrue;
        u32 scaleOrder = m_actor->GetComponentOrder<TextAnimScaleComponent>();
        u32 textOrder = m_actor->GetComponentOrder<UITextBox>();
        if(textOrder != U32_INVALID && scaleOrder < textOrder)
            goodOrder = bfalse;
        textOrder = m_actor->GetComponentOrder<TextBoxComponent>();
        if(textOrder != U32_INVALID && scaleOrder < textOrder)
            goodOrder = bfalse;
        textOrder = m_actor->GetComponentOrder<MultiTextBoxComponent>();
        if(textOrder != U32_INVALID && scaleOrder < textOrder)
            goodOrder = bfalse;      
        VALIDATE_COMPONENT_PARAM_CATEGORY(Graph, "", goodOrder, "TextAnimScaleComponent must be after all texts Component !");
    END_VALIDATE_COMPONENT()

        ///////////////////////////////////////////////////////////////////////////////////////////
        TextAnimScaleComponent::TextAnimScaleComponent()
        : m_animLightComponent(NULL)
        , m_animatedComponent(NULL)
        , m_renderBoxComponent(NULL)
        , m_scaleType(ScaleType_All)
        , m_isActive(btrue)
        , m_margingTop(0.0f)
        , m_margingLeft(0.0f)
        , m_margingRight(0.0f)
        , m_margingBottom(0.0f)
        , m_wantedCursor(Vec2d::Zero)
        , m_updateDate(0.0f)
        , m_isLocked(bfalse)
        , m_wantedSize(Vec2d::One)
        , m_wantedOffset(Vec2d::Zero)
        , m_autoFillContained(bfalse)
        , m_autoFillRepositioned(bfalse)
        , m_minimumSize(Vec2d(0.0f, 0.0f))
        , m_needUpdate(bfalse)
        , m_forceUpdate(bfalse)
        , m_isReady(btrue)
    {
        m_aabb.invalidate();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    TextAnimScaleComponent::~TextAnimScaleComponent()
    {

    }

    void TextAnimScaleComponent::onStartDestroy( bbool _hotReload )
{
        Super::onStartDestroy(_hotReload);


    }

    void TextAnimScaleComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if(!m_containedActors.empty())
        {
            setIsReady(bfalse);
        }
    }

    void TextAnimScaleComponent::onBecomeInactive()
    {
        Super::onBecomeActive();

        if(!m_containedActors.empty())
        {
            setIsReady(bfalse);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void TextAnimScaleComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_renderBoxComponent = m_actor->GetComponent<RenderBoxComponent>();

        if(m_animatedComponent && !m_renderBoxComponent)
        {
            m_animatedComponent->setUpdateAnimInput(this);
        }

        Vec2d baseSize = getTemplate()->m_baseSize;
        Vec2d baseAnchor = getTemplate()->m_baseAnchor;

        if(m_actor->getIs2D())
        {
            baseSize   *= g_pixelBy2DUnit;
            baseAnchor *= g_pixelBy2DUnit;
        }

        m_wantedSize = baseSize;
        m_wantedOffset = Vec2d::Zero;

        if(!m_aabb.isValid())
        {
            m_aabb = AABB(Vec2d::Zero, baseSize);
            m_aabb.Translate(-baseAnchor);
        }

        
        applyWantedAABB(m_aabb);
        
    }

    void TextAnimScaleComponent::onFinalizeLoad()
    {
        Super::onFinalizeLoad();

        m_containedActors.clear();

        if(m_actor->GetComponent<UITextBox>() || m_actor->GetComponent<TextBoxComponent>() || m_actor->GetComponent<MultiTextBoxComponent>())
        {
            m_containedActors.push_back(m_actor->getRef());
        }

        for(u32 i = 0; i < m_containedObjectPaths.size(); ++i)
        {
            if(Actor* child = (Actor*) SceneObjectPathUtils::getObjectFromRelativePath(m_actor, m_containedObjectPaths[i]))
            {
                if(child->GetComponent<UITextBox>() || child->GetComponent<TextBoxComponent>() || child->GetComponent<MultiTextBoxComponent>())
                {
                    m_containedActors.push_back(child->getRef());
                }
            }
        }

        m_repositionActors.clear();
        for(u32 i = 0; i < m_repositionedObjectPaths.size(); ++i)
        {
            if(Pickable* child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, m_repositionedObjectPaths[i]))
            {
                m_repositionActors.push_back(child->getRef());
            }
        }

        m_relRepositionActors.clear();
        for(u32 i = 0; i < m_relRepositionedObjectPaths.size(); ++i)
        {
            if(Pickable *child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, m_relRepositionedObjectPaths[i]))
            {
                m_relRepositionActors.push_back(child->getRef());
            }
        }

        if(!m_containedActors.empty())
        {
            setIsReady(bfalse);
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void TextAnimScaleComponent::onPostPropertyChange()
    {
        fillContainersDefault();

        m_forceUpdate = btrue;
    }
#endif

    void TextAnimScaleComponent::setIsReady(bbool _isReady)
    {
        if(m_isReady !=_isReady)
        {
            if (m_animLightComponent)
            {
                m_animLightComponent->enableDraw(_isReady);
            }

            if (m_renderBoxComponent)
            {
                m_renderBoxComponent->enableDraw(_isReady);
            }

            if(AnimMeshVertexComponent* anim = m_actor->GetComponent<AnimMeshVertexComponent>())
            {
                anim->enableDraw(_isReady);
            }

            if(UITextBox* textBox = m_actor->GetComponent<UITextBox>())
            {
                textBox->setIsVisible(_isReady);
            }

            if(TextBoxComponent* textBox = m_actor->GetComponent<TextBoxComponent>())
            {
                textBox->setIsVisible(_isReady);
            }

            if(MultiTextBoxComponent* textBox = m_actor->GetComponent<MultiTextBoxComponent>())
            {
                const u32 count = textBox->getTextBoxCount();
                for(u32 i = 0; i < count; ++i)
                    textBox->setIsVisible(i, _isReady);
            }

            m_isReady = _isReady;
        }
    }

    void TextAnimScaleComponent::repositionActor(Actor *_actor, const Vec2d &_pivotOffset, const Vec2d &_sizeOffset) const
    {
        Bind* bind = _actor->getParentBind();
        Vec2d actorPos = _actor->get2DPos();

        if(bind)
        {
            if(Actor* parent = (Actor*)bind->m_runtimeParent.getObject())
            {
                f32 worldAngle;
                Vec3d worldPos;
                if(parent->getChildrenBindHandler().computeWorldCoordinates(bind, bind->getPosOffset(), bind->getAngleOffset(), worldPos, worldAngle))
                    actorPos = worldPos.truncateTo2D();
            }            
        }
        m_actor->transformWorldPosToLocal(actorPos);

        if((m_scaleType & ScaleType_Verticale)!=0)
        {
            if(actorPos.y() < m_wantedOffset.y())
            {
                actorPos.y() -= _sizeOffset.y();
            }
            else if(actorPos.y() > m_wantedOffset.y())
            {
                actorPos.y() += _sizeOffset.y();
            }
        }

        if((m_scaleType & ScaleType_Horizontale)!=0)
        {
            if(actorPos.x() < m_wantedOffset.x())
            {
                actorPos.x() -= _sizeOffset.x();
            }
            else if(actorPos.x() > m_wantedOffset.x())
            {
                actorPos.x() += _sizeOffset.x();
            }
        }

        actorPos += _pivotOffset;

        // Set the new position
        m_actor->transformLocalPosToWorld(actorPos);
        const Vec3d finalPos = actorPos.to3d(_actor->getDepth());
        _actor->setPos(finalPos);
        _actor->setWorldInitialPos(finalPos, btrue);

        // Set local position
        if(bind)
        {
            if(Actor* parent = (Actor*) bind->m_runtimeParent.getObject())
            {
                Vec3d localPos;
                f32 localAngle;
                if(parent->getChildrenBindHandler().computeLocalCoordinates(bind, finalPos, _actor->getAngle(), localPos, localAngle))
                {
                    bind->setPosOffset(localPos);
                    bind->setInitialPositionOffset(localPos);
                }
            }
        }
    }

    AABB TextAnimScaleComponent::computeWantedAABB()
    {
        AABB wantedAABB;
        wantedAABB.invalidate();

        for(u32 n=0; n<m_containedActors.size(); ++n)
        {
            if(Actor* actor = (Actor*) m_containedActors[n].getObject())
            {
                if(!actor->isEnabled())
                    continue;

                if(UITextBox* textBox = actor->GetComponent<UITextBox>())
                {
                    wantedAABB.grow(textBox->getTextArea().getTextAreaAABBWithoutRotation());
                }
                else if(TextBoxComponent* textBox = actor->GetComponent<TextBoxComponent>())
                {
                    wantedAABB.grow(textBox->getTextArea().getTextAreaAABBWithoutRotation());
                }
                else if(MultiTextBoxComponent* textBox = actor->GetComponent<MultiTextBoxComponent>())
                {
                    const u32 count = textBox->getTextBoxCount();
                    for(u32 i = 0; i < count; ++i)
                        wantedAABB.grow(textBox->getTextArea(i).getTextAreaAABBWithoutRotation());
                }

				//if(RenderBoxComponent* textBox = actor->GetComponent<RenderBoxComponent>())
				//{
				//	wantedAABB.grow(textBox->getVisualAABB());
				//}
            }
        }

        if(wantedAABB.isValid())
        {
            wantedAABB.transformGlobalToLocal(m_actor->get2DPos(), m_actor->getScale(), 0.0f);
        }
        else
        {
            wantedAABB.setMin(Vec2d::Zero);
            wantedAABB.setMax(Vec2d::Zero);
        }

        return wantedAABB;
    }

    bbool TextAnimScaleComponent::getNeedUpdate(bbool& _needUpdate, bbool& _isTextUpdating)
    {
        for(u32 n=0; n<m_containedActors.size(); ++n)
        {
            if(Actor* actor = (Actor*) m_containedActors[n].getObject())
            {
                if(UITextBox* textBox = actor->GetComponent<UITextBox>())
                {
                    if(textBox->getTextArea().getUpdateDate()>=m_updateDate)
                    {
                        _needUpdate = btrue;
                    }

                    _isTextUpdating |= (actor->isEnabled() && !textBox->getTextArea().getIsUpdated());
                }
                else if(TextBoxComponent* textBox = actor->GetComponent<TextBoxComponent>())
                {
                    if(textBox->getTextArea().getUpdateDate()>=m_updateDate)
                    {
                        _needUpdate = btrue;
                    }

                    _isTextUpdating |= (actor->isEnabled() && !textBox->getTextArea().getIsUpdated());
                }
                else if(MultiTextBoxComponent* textBox = actor->GetComponent<MultiTextBoxComponent>())
                {
                    const u32 count = textBox->getTextBoxCount();
                    for(u32 i = 0; i < count; ++i)
                    {
                        if(textBox->getTextArea(i).getUpdateDate() >= m_updateDate)
                        {
                            _needUpdate = btrue;
                        }

                        _isTextUpdating |= (actor->isEnabled() && !textBox->getTextArea(i).getIsUpdated());
                    }
                }
            }
        }

        return _needUpdate && !_isTextUpdating;
    }


    void TextAnimScaleComponent::Update( f32 _deltaTime )
    {
        PRF_M_SCOPE(updateTextAnimScaleComponent)

        Super::Update(_deltaTime);

        bbool needUpdate = bfalse;
        bbool isTextComputing = bfalse;


        if(getNeedUpdate(needUpdate, isTextComputing) || m_forceUpdate) // one frame delay
        {
            m_updateDate = ELAPSEDTIME;
            m_needUpdate = btrue;
        }


        if(m_isActive && m_needUpdate && m_actor->getScale()!=Vec2d::Zero)
        {
            if(!m_isLocked)
            {
                EventQueryBackgroundBoxResizeAllowed eventQuery;
                m_actor->onEvent(&eventQuery);

                if(eventQuery.getIsResizeAllowed())
                {
                    AABB newAABB = computeWantedAABB();

                    applyWantedAABB(newAABB);

                    m_forceUpdate = bfalse;
                    m_needUpdate = bfalse;
                }
            }
        }



        if(!m_isReady && !isTextComputing && !m_needUpdate)
        {
            setIsReady(btrue);
        }

        if(m_animLightComponent && !m_renderBoxComponent)
        {
            Vec2d baseSize = getTemplate()->m_baseSize;
            Vec2d baseAnchor = getTemplate()->m_baseAnchor;

            if(m_actor->getIs2D())
            {
                baseSize *= g_pixelBy2DUnit;
                baseAnchor *= g_pixelBy2DUnit;
            }

            Vec2d offset = (baseSize * 0.5f - baseAnchor + m_wantedOffset) * m_actor->getScale();
            m_animLightComponent->setInstancePositionOffset(offset);
        }


#ifdef ITF_SUPPORT_EDITOR
        if(UIComponent::isEditionMode(m_actor))
        {
            // Computed AABB
            AABB globalAABB = m_aabb;
            globalAABB.transformLocalToGlobal(m_actor->get2DPos(), m_actor->getScale(), m_actor->getAngle());
            UIComponent::drawBox2D(globalAABB.getMin(), globalAABB.getMax(), 2.0f, Color::yellow());

            Vec2d minimumSize = m_minimumSize;
            Vec2d wantedSize = m_wantedSize;
            Vec2d wantedOffset = m_wantedOffset;
            
            if (m_actor->getIs2D())
            {
                minimumSize = m_actor->referencePos2dToCurrentResolution(minimumSize);
                wantedSize = m_actor->referencePos2dToCurrentResolution(wantedSize);
                wantedOffset = m_actor->referencePos2dToCurrentResolution(wantedOffset);
            }


            AABB minSize(-minimumSize*0.5f, minimumSize*0.5f);
            minSize.transformLocalToGlobal(globalAABB.getCenter(), m_actor->getScale(), m_actor->getAngle());
            UIComponent::drawBox2D(minSize.getMin(), minSize.getMax(), 2.0f, Color::green());

            AABB wantedAABB(-wantedSize*0.5f, wantedSize*0.5f);
            wantedAABB.Translate(wantedOffset);
            wantedAABB.transformLocalToGlobal(m_actor->get2DPos(), m_actor->getScale(), m_actor->getAngle());
            UIComponent::drawBox2D(wantedAABB.getMin(), wantedAABB.getMax(), 2.0f, Color::white());
        }
#endif
    }

    void TextAnimScaleComponent::applyWantedAABB(const AABB& _wantedAABB)
    {
        AABB lastAABB = m_aabb;
        AABB newAABB = _wantedAABB;
        Vec2d newAabbSize = newAABB.getSize();
        Vec2d lastAabbSize = lastAABB.getSize();
        Vec2d newAAbbHalfSize = newAabbSize * 0.5f;
        Vec2d newWantedSize = m_wantedSize;
        Vec2d newWantedOffset = m_wantedOffset;
        Vec2d factor( 1.0f, 1.0f );


        Vec2d baseSize = getTemplate()->m_baseSize;
        Vec2d baseAnchor = getTemplate()->m_baseAnchor;
        
        f32   margingTop = getTemplate()->m_margeSize.y();
        f32   margingLeft = getTemplate()->m_margeSize.x();
        f32   margingRight = getTemplate()->m_margeSize.x();
        f32   margingBottom = getTemplate()->m_margeSize.y();
        Vec2d minimumSize = m_minimumSize;


        if(m_actor->getIs2D())
        {
            margingTop *= g_pixelBy2DUnit;
            margingLeft *= g_pixelBy2DUnit;
            margingRight *= g_pixelBy2DUnit;
            margingBottom *= g_pixelBy2DUnit;


            margingTop += m_margingTop;
            margingLeft += m_margingLeft;
            margingRight += m_margingRight;
            margingBottom += m_margingBottom;
            
            baseSize *= g_pixelBy2DUnit;
            baseAnchor *= g_pixelBy2DUnit;
            factor = m_actor->referenceToCurrentResolutionFactor();
        }
        else
        {
            margingTop += m_margingTop;
            margingLeft += m_margingLeft;
            margingRight += m_margingRight;
            margingBottom += m_margingBottom;
            f32 margingTmp = margingTop;
            margingTop = margingBottom;
            margingBottom = margingTmp;
        }

        if((m_scaleType & ScaleType_Horizontale)!=0)
        {
            if(newAabbSize.x() < minimumSize.x() && minimumSize.x()>0.0f)
            {
                newAABB.ScaleFromCenter(Vec2d(newAabbSize.x() / minimumSize.x(), 1.0f));
                newAabbSize.x() = minimumSize.x();
            }

            if(lastAabbSize.x() < minimumSize.x() && minimumSize.x()>0.0f)
            {
                lastAABB.ScaleFromCenter(Vec2d(lastAabbSize.x() / minimumSize.x(), 1.0f));
                lastAabbSize.x() = minimumSize.x();
            }

            newWantedSize.x() = newAabbSize.x() + margingLeft + margingRight;
            newWantedOffset.x() = newAABB.getCenter().x() + (margingRight - margingLeft) * 0.5f;

            if(newWantedSize.x() <0.0f)
            {
                newWantedSize.x() = 0.0f;
            }
        }

        if((m_scaleType & ScaleType_Verticale)!=0)
        {
            if(newAabbSize.y() < minimumSize.y() && minimumSize.y()>0.0f)
            {
                newAABB.ScaleFromCenter(Vec2d(1.0f, newAabbSize.y() / minimumSize.y()));
                newAabbSize.y() = minimumSize.y();
            }

            if(lastAabbSize.y() < minimumSize.y() && minimumSize.y()>0.0f)
            {
                lastAABB.ScaleFromCenter(Vec2d(1.0f, lastAabbSize.y() / minimumSize.y()));
                lastAabbSize.y() = minimumSize.y();
            }

            newWantedSize.y() = newAabbSize.y() + margingTop + margingBottom;
            newWantedOffset.y() = newAABB.getCenter().y() + (margingBottom - margingTop) * 0.5f;

            if(newWantedSize.y() <0.0f)
            {
                newWantedSize.y() = 0.0f;
            }
        }


        // Actor repositioning
        if(!m_repositionActors.empty())
        {
            const Vec2d pivotOffset = newWantedOffset - m_wantedOffset;
            const Vec2d sizeOffset = (newWantedSize - m_wantedSize) * 0.5f;
            for(u32 i = 0; i < m_repositionActors.size(); ++i)
            {
                if(Actor* actor = (Actor*)m_repositionActors[i].getObject())
                {
                    repositionActor(actor, pivotOffset, sizeOffset);
                }
            }
        }

        if(!m_relRepositionActors.empty())
        {
            const Vec2d pivotOffset = newAABB.getCenter() - lastAABB.getCenter();
            const Vec2d sizeOffset = (newAabbSize - lastAabbSize) * 0.5f;
            for(u32 i = 0; i < m_relRepositionActors.size(); ++i)
            {
                if(Actor* actor = (Actor*)m_relRepositionActors[i].getObject())
                {
                    repositionActor(actor, pivotOffset, sizeOffset);
                }
            }
        }

        // Apply on render box component
        if (m_renderBoxComponent)
        {
            m_renderBoxComponent->setSize(newWantedSize * factor);
            m_renderBoxComponent->setOffset((baseSize * 0.5f - baseAnchor + newWantedOffset) * factor);
        }
        // Apply on anim component
        else if(m_animLightComponent)
        {
            Vec2d scale = newWantedSize / baseSize;
            Vec2d offset = (baseSize * 0.5f - baseAnchor + newWantedOffset) * m_actor->getScale();

            m_animLightComponent->setInstancePositionOffset(offset);

            if(getTemplate()->m_animInputX == StringID::InvalidId && getTemplate()->m_animInputY == StringID::InvalidId)
            {
                m_animLightComponent->setScaleMultiplier(scale);
            }
            else
            {
                f32 baseSizeX = baseSize.x();
                if((m_scaleType & ScaleType_Horizontale)!=0 && baseSizeX!=0.0f)
                {
                    m_wantedCursor.x() = (newAabbSize.x() - baseSize.x() + (margingLeft + margingRight)) / baseSizeX;
                }

                f32 baseSizeY = baseSize.y();
                if((m_scaleType & ScaleType_Verticale)!=0 && baseSizeY!=0.0f)
                {
                    m_wantedCursor.y() = (newAabbSize.y() - baseSize.y() + (margingTop + margingBottom)) / baseSizeY;
                }
            }
        }



        // Apply
        m_aabb = _wantedAABB;

        m_wantedSize = newWantedSize;
        m_wantedOffset = newWantedOffset;

#ifdef ITF_SUPPORT_EDITOR
        m_wantedAABB = AABB (-m_wantedSize*0.5f, m_wantedSize*0.5f);
        m_wantedAABB.Translate(m_wantedOffset);
#endif
    }

#ifdef ITF_SUPPORT_EDITOR
    void TextAnimScaleComponent::setWantedAABB( const AABB &_aabb )
    {
        Vec2d wantedOffset;
        Vec2d wantedSize;

        // get margin values
        f32     templateMargingTop = getTemplate()->m_margeSize.y();
        f32     templateMargingLeft = getTemplate()->m_margeSize.x();
        f32     templateMargingRight = getTemplate()->m_margeSize.x();
        f32     templateMargingBottom = getTemplate()->m_margeSize.y();
        f32     margingTop, margingBottom, margingLeft, margingRight;

        if(m_actor->getIs2D())
        {
            Vec2d scaleInRes = g_pixelBy2DUnit * m_actor->referenceToCurrentResolutionFactor();
            templateMargingTop *= scaleInRes.y();
            templateMargingLeft *= scaleInRes.x();
            templateMargingRight *= scaleInRes.x();
            templateMargingBottom *= scaleInRes.y();
        }
        
        margingTop = templateMargingTop + m_margingTop;
        margingLeft = templateMargingLeft + m_margingLeft;
        margingRight = templateMargingRight + m_margingRight;
        margingBottom = templateMargingBottom + m_margingBottom;
        
        if(!m_actor->getIs2D())
            Swap( margingBottom, margingTop);

        // get oldaabb data from current wanted aabb
        wantedOffset = m_wantedAABB.getCenter();
        wantedSize = m_wantedAABB.getSize();

        Vec2d aabbSize;
        aabbSize.x() = wantedSize.x() - margingLeft - margingRight;
        aabbSize.y() = wantedSize.y() - margingTop - margingBottom;
        Vec2d aabbCenter;
        aabbCenter.x() = wantedOffset.x() - (margingRight - margingLeft) * 0.5f;
        aabbCenter.y() = wantedOffset.y() - (margingBottom - margingTop) * 0.5f;

        wantedOffset = _aabb.getCenter();
        wantedSize = _aabb.getSize();
        f32 rightPlusLeft = wantedSize.x() - aabbSize.x();
        f32 bottomPlusTop = wantedSize.y() - aabbSize.y();
        f32 rightMinusLeft = (wantedOffset.x() - aabbCenter.x()) * 2;
        f32 bottomMinusTop = (wantedOffset.y() - aabbCenter.y()) * 2;
        f32 right = (rightPlusLeft + rightMinusLeft) * 0.5f;
        f32 left = right - rightMinusLeft;
        f32 bottom = (bottomPlusTop + bottomMinusTop) * 0.5f;
        f32 top = bottom - bottomMinusTop;

        if(!m_actor->getIs2D())
            Swap( top, bottom);

        m_margingTop = top - templateMargingTop;
        m_margingLeft = left - templateMargingLeft;
        m_margingRight = right- templateMargingRight;
        m_margingBottom = bottom - templateMargingBottom;

        AABB newAABB = computeWantedAABB();
        applyWantedAABB(newAABB);
    }
#endif 


    void TextAnimScaleComponent::updateAnimInput()
    {
        if(m_animatedComponent)
        {
            if(getTemplate()->m_animInputX != StringID::InvalidId)
            {
                m_animatedComponent->setInput(getTemplate()->m_animInputX, m_wantedCursor.x());
            }

            if(getTemplate()->m_animInputY != StringID::InvalidId)
            {
                m_animatedComponent->setInput(getTemplate()->m_animInputY, m_wantedCursor.y());
            }
        }
    }

    void TextAnimScaleComponent::setScaleType(ScaleType _scaleType)
    {
        m_scaleType = _scaleType;
    }

    void TextAnimScaleComponent::fillContainersDefault()
    {
        Actor* parent = m_actor->getParent().getActor();
        if(!parent)
            return;

        AABB containerAABB = m_actor->getAABB();

        if(m_autoFillContained)
        {
            m_containedObjectPaths.clear();
        }

        if(m_autoFillRepositioned)
        {
            m_repositionedObjectPaths.clear();
            m_relRepositionedObjectPaths.clear();
        }

        const ObjectRefList& children = parent->getChildrenBindHandler().getChildren();
        for(u32 n=0; n<children.size(); ++n)
        {
            if(Actor* childActor = (Actor*) children[n].getObject())
            {
                // check if not self:
                if(childActor->getRef() == GetActor()->getRef())
                {
                    continue;
                }

                ObjectPath path;
                if(!SceneObjectPathUtils::getRelativePathFromObject(m_actor, childActor, path)) 
                {
                    SceneObjectPathUtils::getAbsolutePathFromObject(childActor, path); 
                }

                if(m_autoFillRepositioned)
                {
                    // check if unique (not already in list)
                    bbool alreadyFound = bfalse;
                    for(u32 i = 0; i < m_repositionedObjectPaths.size(); ++i)
                    {
                        if(path == m_repositionedObjectPaths[i])
                        {
                            alreadyFound = btrue;
                            break;
                        }
                    }

                    if(alreadyFound)
                    {
                        continue;
                    }

                    m_repositionedObjectPaths.push_back(path);
                }

                if(m_autoFillContained)
                {
                    if(UITextBox* textBox = childActor->GetComponent<UITextBox>())
                    {
                        //if(containerAABB.contains(textBox->getTextArea().getAABB()))
                        {
                            m_containedObjectPaths.push_back(path);
                        }
                    }
                }
            }
        }


        m_autoFillContained = bfalse;
        m_autoFillRepositioned = bfalse;
    }

    ITF::Vec2d TextAnimScaleComponent::getIntersection( const Vec2d& _lineStart, const Vec2d& _lineStop, const Vec2d& _topLeft, const Vec2d& _bottomRight )
    {
        const Vec2d topRight = Vec2d(_bottomRight.x(), _topLeft.y());
        const Vec2d bottomLeft = Vec2d(_topLeft.x(), _bottomRight.y());

        Vec2d collisionPoint = Vec2d::Zero;
        f32 epsilon = 0.0f;

        // bottom (more likely to early out)
        if(Segment_Segment(_lineStart, _lineStop, bottomLeft, _bottomRight, collisionPoint, &epsilon))
        {
            return collisionPoint;
        }
        // top
        else if(Segment_Segment(_lineStart, _lineStop, _topLeft, topRight, collisionPoint, &epsilon))
        {
            return collisionPoint;
        }
        //right
        else if(Segment_Segment(_lineStart, _lineStop, topRight, _bottomRight, collisionPoint, &epsilon))
        {
            return collisionPoint;
        }
        // left
        else if(Segment_Segment(_lineStart, _lineStop, _topLeft, bottomLeft, collisionPoint, &epsilon))
        {
            return collisionPoint;
        }

        return collisionPoint;
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(TextAnimScaleComponent_Template)
        BEGIN_SERIALIZATION_CHILD(TextAnimScaleComponent_Template)
        SERIALIZE_MEMBER("baseSize", m_baseSize);
    SERIALIZE_MEMBER("baseSizeMax", m_baseSizeMax);
    SERIALIZE_MEMBER("margeSize", m_margeSize);
    SERIALIZE_MEMBER("animInputX", m_animInputX);
    SERIALIZE_MEMBER("animInputY", m_animInputY);
    END_SERIALIZATION()

        TextAnimScaleComponent_Template::TextAnimScaleComponent_Template()
        : m_baseSize(Vec2d::Zero)
        , m_baseSizeMax(Vec2d::Zero)
        , m_margeSize(Vec2d::Zero)
        , m_animInputX(StringID::InvalidId)
        , m_animInputY(StringID::InvalidId)
    {
    }
}
