#ifndef _ITF_UIMENUSCROLL_H_
#define _ITF_UIMENUSCROLL_H_

#ifndef _ITF_UIMENUBASIC_H_
#include "gameplay/components/UI/UIMenuBasic.h"
#endif //_ITF_UIMENUBASIC_H_

#ifndef _ITF_UIUTILS_H_
#include "gameplay/components/UI/UIUtils.h"
#endif //_ITF_UIUTILS_H_

#define UIMenuScroll_CRC ITF_GET_STRINGID_CRC(UIMenuScroll, 188991749)
#define UIMenuScroll_Template_CRC ITF_GET_STRINGID_CRC(UIMenuScroll_Template, 951732277)

namespace ITF
{
    

    class ActorPool
    {
    public:
        ActorPool();
        ~ActorPool();

        void setup(const Path& _actorPath, Actor* _owner, u32 _numPreAlloc = 1);
        void update();
        void clear();
        bbool isValid() {return !m_actorPath.isEmpty();}

        u32 requestActor(u32 _count);
        Actor* popActor(Scene* _scene, Actor* _parent = NULL);
        Actor* releaseActor(Actor* _actor);
        void clearActors();

        u32 getRequestedActorsCount() {return m_requestedActorsCount;}
        u32 getReadyActorsCount() {return m_spawnedActors.size();}
        
    private:

        void updatePoolSize();
        ObjectRef m_ownerRef;
        Path m_actorPath;
        u32 m_requestedActorsCount;
        ITF_VECTOR<Actor*> m_spawningActors;
        ITF_VECTOR<Actor*> m_spawnedActors;
    };

    class UIMenuScroll : public UIMenuBasic
    {
    DECLARE_OBJECT_CHILD_RTTI(UIMenuScroll, UIMenuBasic, UIMenuScroll_CRC);


    public:
        DECLARE_SERIALIZE()  

        UIMenuScroll  ();
        virtual ~UIMenuScroll ();

        virtual void onFinalizeLoad();
        virtual void onStartDestroy(bbool _hotReload);
        virtual void Update( f32 _deltaTime );
        virtual void onInput(const i32 _controllerID, const f32 _axis, const StringID _input);
		virtual void onTouchInput(const TouchDataArray & _touchArray, u32 _i = 0);
        virtual void clearItems();
        virtual UIItem* computeNextItem(const Vec2d& _direction, i32 _neededState = UI_STATE_None, i32 _forbiddenState = UI_STATE_None) const;

        void gotoFirstItemIndex(i32 _index, bbool _isInstant = bfalse);
        void gotoCenterItemIndex(i32 _index, bbool _isInstant = bfalse);


        u32 requestItems(u32 _itemsCount, i32 _modelIndex = 0);
        i32 getItemIndex(UIComponent* _item) const;
        UIComponent* getItemAt(i32 _index);
        UIComponent* getSeparatorAt(i32 _index);
        UIComponent* addItem(i32 _modelIndex = 0, bbool _extendUp = bfalse);
        UIComponent* setItem(i32 _index, i32 _modelIndex = 0);
        UIComponent* addItemBefore(UIComponent* beforeItem, i32 _modelIndex = 0, bbool _extendUp = bfalse);
        UIComponent* removeItem(i32 _index, bbool _colapseDown = bfalse);
        u32 getItemsCount() const {return m_currentItems.size();}
        u32 getModelsCount() const {return m_modelItemPools.size();}
		void setAreItemsDisplayed(bbool _val);
        void setDefaultItem(StringID _friendlyID) {m_defaultItemFriendly = _friendlyID;}

#ifdef ITF_SUPPORT_EDITOR
        void onPostPropertyChange();
#endif //ITF_SUPPORT_EDITOR

    protected:

        class  ItemObject
        {
        public :
            ItemObject() : m_uiComponent(NULL), m_modelIndex(-1), m_offset(Vec2d::Zero) {}
            ItemObject(UIComponent* _component, i32 _modelIndex) : m_uiComponent(_component), m_modelIndex(_modelIndex), m_offset(Vec2d::Zero) 
            {
                m_uiActor = _component ? _component->GetActor()->getRef() : ObjectRef::InvalidRef;
            }


            UIComponent* getUIComponent() const 
            {
#ifdef ITF_SUPPORT_EDITOR
                if(Actor* actor = m_uiActor.getActor())
                {
                    return actor->GetComponent<UIComponent>();
                }
                return NULL;
#else
                return m_uiComponent;
#endif
            }

            void setUIComponent(UIComponent* _component, i32 _modelIndex) 
            {
                m_uiActor = _component ? _component->GetActor()->getRef() : ObjectRef::InvalidRef;
                m_uiComponent = _component; 
                m_modelIndex = _modelIndex;
            }

            const Vec2d& getOffset() const {return m_offset;}
            i32 getModelIndex() const {return m_modelIndex;}
            void setOffset(const Vec2d& _offset) {m_offset = _offset;}

            Vec2d& computeNewOffset(Vec2d& _newOffset, const Vec2d& _collapseSpeed, const Vec2d& _extendSpeed, f32 _dt)
            {
                _newOffset = Vec2d::Zero;

                if(m_offset.x()<0.0f && _extendSpeed.x()!=F32_INFINITY)
                {
                    _newOffset.x() = f32_Min(m_offset.x() + _extendSpeed.x() * _dt, 0.0f);
                }
                else if(m_offset.x()>0.0f && _collapseSpeed.x()!=F32_INFINITY)
                {
                    _newOffset.x() = f32_Max(m_offset.x() - _collapseSpeed.x() * _dt, 0.0f);
                }

                if(m_offset.y()<0.0f && _extendSpeed.y()!=F32_INFINITY)
                {
                    _newOffset.y() = f32_Min(m_offset.y() + _extendSpeed.y() * _dt, 0.0f);
                }
                else if(m_offset.y()>0.0f && _collapseSpeed.y()!=F32_INFINITY)
                {
                    _newOffset.y() = f32_Max(m_offset.y() - _collapseSpeed.y() * _dt, 0.0f);
                }

                return _newOffset;
            }

        private :
            UIComponent* m_uiComponent;
            ActorRef m_uiActor;
            i32 m_modelIndex;
            Vec2d m_offset;
        };

        typedef ITF_VECTOR<ItemObject> ItemList;

        const class UIMenuScroll_Template* getTemplate() const;

        virtual void onBecomeActive();
        virtual void onBecomeInactive();

        Vec3d getItemAbsoluteStartPos() const;
        Vec3d getSeparatorAbsoluteStartPos() const;
        void insertUiComponent(UIComponent* _component, i32 _modelIndex, ItemList& _componentsList, i32 _componentIndex, const Vec3d& _startPos) const;
        bbool setUiComponent(UIComponent* _component, i32 _modelIndex, ItemList& _componentsList, ItemList& _erasedComponentsList, i32 _componentIndex, const Vec3d& _startPos) const;
        void eraseUiComponent(i32 _index, ItemList& _componentsList, ItemList& _erasedComponentsList) const;
        void displayUiComponent(i32 _startIndex, i32 _count, ItemList& _componentsList) const;
        void releaseItem(UIComponent* _component, i32 _modelIndex);
        void releaseUpdate();
        void offsetUpdate(f32 _dt);
        
        void setFirstItemCursor(f32 _cursor);
        void setCurrentOffset(const Vec2d& offset);
        ItemList m_currentItems;
        ItemList m_currentSeparators;
        ItemList m_erasedItems;
        i32 m_firstItemIndex;
        f32 m_firstItemCursor;
		bbool m_areItemDisplayed;
        Vec2d m_currentOffset;


        StringID m_separatorModel;
        ITF_VECTOR<ActorPool> m_modelItemPools;
        Vec2d m_itemOffset;
        i32 m_itemVisiblesCount;
        bbool m_displayOutItems;
        Vec3d m_itemStartPos;
        Vec3d m_separatorStartPos;

        DragScrolling m_dragScrolling;
        f32 m_movingDuration;
        bbool m_isDragScrolling;
        bbool m_hasTouchBegin;
    };


    class UIMenuScroll_Template : public UIMenuBasic_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIMenuScroll_Template, UIMenuBasic_Template, UIMenuScroll_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIMenuScroll);

    public:

        UIMenuScroll_Template();
        ~UIMenuScroll_Template();

        ITF_VECTOR<Path> m_modelItemPaths;
        i32 m_modelSperatorIndex;

        f32 m_movingSelectionDelay;
        f32 m_movingSelectionNormMax;
        f32 m_movingBounceTime;
        f32 m_movingBounceNorm;
        bbool m_movingSnapOnItem;
        f32 m_movingMomentumFriction;
        f32 m_movingMomentumDeceleration;

        Vec2d m_extendSpeed;
        Vec2d m_colapseSpeed;

        
    };

    ITF_INLINE const UIMenuScroll_Template* UIMenuScroll::getTemplate() const
    {
        return static_cast<const UIMenuScroll_Template*>(m_template);
    }
}
#endif // _ITF_UIMENUSCROLL_H_