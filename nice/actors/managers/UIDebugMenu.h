#if defined(ITF_SUPPORT_CHEAT) && !defined(ITF_FINAL)
#ifndef _ITF_UIDEBUGMENU_H_
#define _ITF_UIDEBUGMENU_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif // _ITF_INPUTADAPTER_H_

namespace ITF 
{
    class DebugMenu;
    class DebugMenuEntry;

    //-----------------------------------------------------------------------    
    class DebugMenuRect
    {
    public:
        DebugMenuRect() : m_position(Vec2d::Zero), m_size(Vec2d::Zero) {}
        DebugMenuRect(const Vec2d &_pos, const Vec2d &_size) : m_position(_pos), m_size(_size) {}
        DebugMenuRect(const DebugMenuRect &_rect) : m_position(_rect.m_position), m_size(_rect.m_size) {}

        bbool isPointInside(const Vec2d &_pos) const;
        bbool isRectInside(const DebugMenuRect &_rect) const;

        Vec2d m_position;
        Vec2d m_size;
    };

    //-----------------------------------------------------------------------    
    class UIDebugMenu : public ITouchListener
    {
        friend class Plugin_Cheats;

        struct TouchContact
        {
            TouchContact(u32 _controller, u32 _id, const Vec2d &_pos, f32 _time) : m_controller(_controller), m_touchId(_id), m_initPosition(_pos), m_position(_pos), m_time(_time), m_ended(bfalse) {}
            u32 m_controller;
            u32 m_touchId;
            Vec2d m_initPosition;
            Vec2d m_position;
            f32  m_time;
            bbool m_ended;
        };

    public:
        UIDebugMenu();
        ~UIDebugMenu();

        bbool                       getIsInDBGMenu      (       ) const { return m_inDBGMenu; }
        void                        setIsInDBGMenu      (bbool _isInDBGMenu, u32 _player);
        void                        addDebugMenu( class DebugMenu* _cheat );
	    void						addDebugMenu_System();
        void                        addDebugMenu_MapList();
        void                        update(f32 _dt);
        void                        setDisplayedController(u32 _controllerId) { m_displayedController = _controllerId; }
        u32                         getDisplayedController() const { return m_displayedController; }
        bbool                       getTouchMode() const { return m_touchMode; }
        void                        setTouchMode(bbool _val);
        u32                         getPlayer() const { return m_player; }
        DebugMenu*                  getCurrentMenu() { return m_DBGmenus[m_currentPageDBGMenu]; }
        void                        setDBGMemory(ITF::bbool val) { m_inDBGMemory = val; m_inDBGMemoryDisplay = 30*30;}
        ITF::bbool                  getDBGMemory() const { return m_inDBGMemory; }

    protected:
        void                        updateTouchInput(f32 _dt);
        void                        onContactEnd(const Vec2d &_position);
        void                        updateDBGMenuStatus();
        void                        updateDBGMenuPad();
        void                        displayDebugMenu();
        void                        applyGamePadButtonDBGMenu (u32 _numPad, InputAdapter::PressStatus* _buts, float* _pos);
        void                        setNextDBGMenu();
        void                        setPreviousDBGMenu();
        bbool                       canScrollUp();
        void                        scrollUp();
        bbool                       canScrollDown();
        void                        scrollDown();
        void                        displayControllerState(u32 _controllerId) const;
        void                        displayBaseControllerState(u32 _controllerId, f32 &_x, f32 &_y, bbool _isPS4) const;
        void                        displayTouchScreenControllerState(u32 _controllerId, f32 &_x, f32 &_y) const;
        virtual void                Receive(const TouchDataArray & _touchArray);
        ITF_VECTOR<DebugMenu*>      &getMenus() { return m_DBGmenus; }
        TouchContact                *getTouchContact(u32 _controller, u32 _touchId);
        void                        updateDBGMenuMemory();

    protected:
        bbool                       m_inDBGMenu;
        u32                         m_currentPageDBGMenu;
        u32                         m_player;
        u32                         m_activatingPadIndex;
        ITF_VECTOR<DebugMenu*>      m_DBGmenus;
        f32                         m_timeoutDBGMenu;
        f32                         m_timerDBGMenu;
        f32                         m_timeoutJustPressed;
        bbool                       m_firstPressed;
        bbool                       m_joyReleased;
        u32                         m_displayedController;
        TouchDataArray              m_lastTouchData;
        bbool                       m_touchMode;
        DebugMenuRect               m_touchLeftButton;
        DebugMenuRect               m_touchRightButton;
        DebugMenuRect               m_touchTopButton;
        DebugMenuRect               m_touchBottomButton;
        ITF_VECTOR<TouchContact>    m_currentContacts;
        bbool                       m_countdownRunning;
        u32                         m_contactCount;
        
        bbool                       m_inDBGMemory;
        unsigned int                m_inDBGMemoryDisplay;

    };

    //-----------------------------------------------------------------------
    class DebugMenu
    {
    public:
        DebugMenu(const String8 &_label);
        ~DebugMenu();
        void update();
        void display();
        void up();
        void down() ;
        void setEntryName( const char* _name, u32 _index );
        void select(u32 _player = 0);
        void onDirection(bool _left);
        void addEntry(DebugMenuEntry * _entry, bbool _unique = bfalse, i32 _index = -1);
        void removeEntry(u32 _index);
        u32 getEntriesSize();
        DebugMenuEntry* getEntry(u32 _index){ if(_index < getEntriesSize()) return m_entries[_index]; else return NULL; }
        u32 getSelectionIndex()const { return m_selection; }
        void setSelectionIndex(u32 _index) { m_selection = _index; }
        void clear();
        const String8 &getLabel() const { return m_label; }
        void setParent(UIDebugMenu *_parent) { m_parent = _parent; }
        const UIDebugMenu *getParent() const { return m_parent; }
        u32 getPlayer() const { return m_parent->getPlayer(); }
        const DebugMenuRect &getScreenRect() const { return m_screenRect; }
        void setScreenRect(const DebugMenuRect &val) { m_screenRect = val; }
        const Vec2d &getScreenPos() const { return m_screenRect.m_position; }
        void setScreenPos(const Vec2d &val) { m_screenRect.m_position = val; }
        const Vec2d &getSize() const { return m_screenRect.m_size; }
        void setSize(const Vec2d &val) { m_screenRect.m_size = val; }
        void updateLayout();
        bbool getTouchMode() const { return m_parent && m_parent->getTouchMode(); }
        const DebugMenuRect &getClippingRect() const { return m_clippingRect; }
        DebugMenuRect &getClippingRect() { return m_clippingRect; }
        void setClippingRect(const DebugMenuRect &val) { m_clippingRect = val; }
        void onContactEnd(const Vec2d &_screenPos);
        void setSelectionAt(const Vec2d &_screenPos);

    protected:
        void computeAABB(Vec2d &_min, Vec2d &_max);

    protected:
        UIDebugMenu *m_parent;
        String8 m_label;
        u32 m_selection;
        ITF_VECTOR<DebugMenuEntry* > m_entries;

        DebugMenuRect m_screenRect;
        DebugMenuRect m_clippingRect;
    };

    //-----------------------------------------------------------------------
    class DebugMenuEntry 
    {
    public:
        DebugMenuEntry( const String8 & _name);
        virtual void select(u32 _player) const {}
        virtual void onDirection(bool _left) const {}
        virtual const String8 getName(u32 _player) const { return m_name; } 
        virtual void update();
        void setName( const char* _name ) { m_name = _name; }

        void display();
        bbool isPointInside(Vec2d _screenPos) { return m_screenRect.isPointInside(_screenPos); }
        bbool isInRect(const DebugMenuRect &_rect) { return _rect.isRectInside(m_screenRect); }
        void setParent(DebugMenu *_parent) { m_parent = _parent; }
        void setRelativePos(const Vec2d &_pos) { m_relativeRect.m_position = _pos; }
        const Vec2d &getScreenPos() const { return m_screenRect.m_position; }
        const Vec2d &getSize() const { return m_relativeRect.m_size; }
        Vec2d &getSize() { return m_relativeRect.m_size; }
        u32 getPlayer() const { return m_parent->getParent()->getPlayer(); }
        u32 getTouchMode() const { return m_parent && m_parent->getTouchMode(); }
        void updateSize();
        bbool getSelected() const { return m_selected; }
        void setSelected(bbool val) { m_selected = val; }

    protected:
        void updateScreenPos();

    protected:
        String8 m_name;

        DebugMenu *m_parent;
        DebugMenuRect m_relativeRect;
        DebugMenuRect m_screenRect;
        bbool m_selected;
    };


    //-----------------------------------------------------------------------
    class DebugMenuEntry_ChangeMap: public DebugMenuEntry
    {
    public:
        DebugMenuEntry_ChangeMap(const String8 & _name, const Path & _mapPath): DebugMenuEntry(_name), m_mapPath(_mapPath) {}
        virtual void select(u32 _playerIndex) const;
    private:
        Path m_mapPath;
    };

    //-----------------------------------------------------------------------
    class ChangeCheckpoint_Entry : public DebugMenuEntry
    {
    public:
        ChangeCheckpoint_Entry(const String8 & _name, class Actor* _checkpoint )
            : DebugMenuEntry(_name)
            , m_checkpoint(_checkpoint)
        {}
        virtual void select(u32 _playerIndex) const;

        class Actor* getActor() { return m_checkpoint; }

    private :
        class Actor* m_checkpoint;
    };




    //========================================================================
    // Inlines
    //========================================================================

    //-----------------------------------------------------------------------
    inline bbool DebugMenuRect::isPointInside(const Vec2d &_pos) const
    {
        return _pos.x() >= m_position.x() &&
            _pos.y() >= m_position.y() &&
            _pos.x() <= m_position.x() + m_size.x() &&
            _pos.y() <= m_position.y() + m_size.y();
    }

    //-----------------------------------------------------------------------
    inline bbool DebugMenuRect::isRectInside(const DebugMenuRect &_rect) const
    {
        return  isPointInside(_rect.m_position) && isPointInside(_rect.m_position + _rect.m_size);
    }

} // namespace ITF

#endif //_ITF_UIDEBUGMENU_H_
#endif // ITF_SUPPORT_CHEAT
