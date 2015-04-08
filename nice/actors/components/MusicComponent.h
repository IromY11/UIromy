#ifndef _ITF_MUSICCOMPONENT_H_
#define _ITF_MUSICCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_


#define MusicComponent_CRC ITF_GET_STRINGID_CRC(MusicComponent, 2665941472) 
#define MusicComponent_Template_CRC 3999487448


#ifdef ITF_SUPPORT_RAKI

#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_

#ifndef _ITF_MUSICPARTSET_H_
#include "engine/sound/Music/MusicPartSet.h"
#endif // _ITF_MUSICPARTSET_H_

#ifndef _ITF_MUSICTREE_H_
#include "engine/blendTree/MusicTree/MusicTree.h"
#endif //_ITF_MUSICTREE_H_

#ifndef _ITF_MUSICDESCRIPTOR_H_
#include "engine/sound/music/MusicDescriptor.h"
#endif // _ITF_MUSICDESCRIPTOR_H_


namespace ITF
{
    class MusicComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(MusicComponent,ActorComponent,MusicComponent_CRC);
        DECLARE_SERIALIZE()

    public:

        MusicComponent();
        virtual ~MusicComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual bbool       needsDraw2DNoScreenRatio() const { return bfalse; }

        virtual void        Update( f32 _deltaTime );

        virtual void        onSceneActive();
        virtual void        onSceneInactive();
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        onFinalizeLoad();
        virtual void onStartDestroy(bbool _hotReload);
        virtual void        onResourceLoaded();
        virtual void        onActorLoaded(Pickable::HotReloadType _hotReload );
        virtual void        onActorClearComponents();

        virtual void        validate(bbool &_isComponentValidated);

        ITF_INLINE const class MusicComponent_Template * getTemplate() const;

    private:
        bbool               m_initializeMusicDescriptor;
        MusicDescriptor*    m_loadedMusicDescriptor;

        static const MusicComponent* ms_registeredComponent;
    };


    class MusicComponent_Template : public ActorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(MusicComponent_Template,ActorComponent_Template,MusicComponent_Template_CRC);
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(MusicComponent);

    public:

        MusicComponent_Template();
        virtual ~MusicComponent_Template();

        ITF_INLINE const MusicTree_Template& getMusicTreeTemplate() const { return m_musicTree; }
        ITF_INLINE const MusicPartSet_Template& getMusicPartSetTemplate() const { return m_musicPartSet; }
        ITF_INLINE const ITF_VECTOR<InputDesc>& getInputList() const { return m_inputList; }

        MusicTree* createTreeInstance() const;
        bbool onTemplateLoaded( bbool _hotReload );

        void onTemplateDelete( bbool _hotReload );

    private:
        ITF_VECTOR <InputDesc>  m_inputList;
        MusicPartSet_Template   m_musicPartSet;
        MusicTree_Template      m_musicTree;
        ArchiveMemory           m_instanceData;
    };

    ITF_INLINE const MusicComponent_Template* MusicComponent::getTemplate() const
    {
        return static_cast<const MusicComponent_Template*>(m_template);
    }
}
#else


namespace ITF
{
	class MusicComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(MusicComponent,ActorComponent,MusicComponent_CRC);
		DECLARE_SERIALIZE()

		virtual bbool       needsUpdate() const { return bfalse; }
		virtual bbool       needsDraw() const { return bfalse; }
		virtual bbool       needsDraw2D() const { return bfalse; }
		virtual	bbool		needsDraw2DNoScreenRatio() const { return bfalse; }


	public:
		ITF_INLINE const class MusicComponent_Template * getTemplate() const;

		static const MusicComponent* ms_registeredComponent;
	};


	class MusicComponent_Template : public ActorComponent_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(MusicComponent_Template,ActorComponent_Template,MusicComponent_Template_CRC);
		DECLARE_SERIALIZE()
		DECLARE_ACTORCOMPONENT_TEMPLATE(MusicComponent);

	public:

		MusicComponent_Template();
		virtual ~MusicComponent_Template();
	};

	ITF_INLINE const MusicComponent_Template* MusicComponent::getTemplate() const
	{
		return static_cast<const MusicComponent_Template*>(m_template);
	}
}




#endif // _ITF_MUSICCOMPONENT_H_
#endif //#ifdef ITF_SUPPORT_RAKI