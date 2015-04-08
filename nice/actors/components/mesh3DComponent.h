
#ifndef _ITF_MESH3DCOMPONENT_H_
#define _ITF_MESH3DCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_ANIMATION3DLIST_H_
#include    "engine/animation3D/Animation3DList.h"
#endif // _ITF_ANIMATION3DLIST_H_

#ifndef _ITF_ANIMATION3DSET_H_
#include    "engine/animation3D/Animation3DSet.h"
#endif // _ITF_ANIMATION3DSET_H_

#ifndef _ITF_ANIMATION3DPLAYER_H_
#include    "engine/animation3D/Animation3Dplayer.h"
#endif // _ITF_ANIMATION3DPLAYER_H_

#ifndef _ITF_ANIMATION3DTREE_H_
#include "engine/blendTree/Animation3DTree/Animation3DTree.h"
#endif //_ITF_ANIMATION3DTREE_H_

#ifndef _ITF_ANIMCOMMONINTERFACE_H_
#include "engine/actors/components/AnimCommonInterface.h"
#endif // _ITF_ANIMCOMMONINTERFACE_H_

#ifndef ITF_ENGINE_MESH3DPRIM_H_
#include "engine/display/Primitives/Mesh3DPrim.h"
#endif //ITF_ENGINE_MESH3DPRIM_H_


namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    class Mesh3DComponent_Template;
    class Mesh3D;
    class Mesh3DResource;
    class Skeleton3DResource;
    class Animation3DResource;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Mesh3DComponent class
    // 
    ///////////////////////////////////////////////////////////////////////////////////////////////
    #define Mesh3DComponent_CRC ITF_GET_STRINGID_CRC(Mesh3DComponent,444504474)
    class Mesh3DComponent : public GraphicComponent, public IAnimCommonInterface
    {
        DECLARE_OBJECT_CHILD_RTTI(Mesh3DComponent,GraphicComponent,444504474)

    public:
        DECLARE_SERIALIZE()

        Mesh3DComponent();
        ~Mesh3DComponent();

        enum IKType
        {
            IKType_Normal = 0,      // up to down
            IKType_Reverse,         // down to up
            IKType_SmoothChain,           // angle sight
            IKType_LookAt,          // sight
        }; 

		Mesh3DPrim &        getMesh() { return m_3Dmesh; };
		const Mesh3DPrim &  getMesh() const { return m_3Dmesh; };

    private:
        virtual bbool       needsUpdate() const { return btrue; }
        virtual void        Update( f32 _deltaTime );
        virtual bbool       needsDraw() const { return btrue; }
        virtual void        batchPrimitives( const ITF_VECTOR <class View*>& _views );
        void                batchPrimitivesMesh3D( const ITF_VECTOR <class View*>& _views, ResourceID &_meshID, Mesh3DPrim &_mesh );

        void                ComputeMatrix( Matrix44 &_matrix ) const;
        void                setSkeleton( Skeleton3DResource *_skeleton3D );
        void                setAnimation( Animation3DResource *_anim3D, float _ratio = 0.0f);
        
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActorClearComponents();
        virtual void        onResourceLoaded();
        void                onResourceLoaded_Mesh3D(Mesh3DResource* _mesh3D, ITF_Mesh &_mesh );
        virtual void        onUnloadResources();

        void                onEvent(Event * _event);

        void                UnloadMaterialList();
        void                UnloadMaterialList_Detach();
        void                LoadMaterialList();

        void                UnloadMesh3DID();
        void                LoadMesh3DID();

        void                UnloadMesh3DIDList();
        void                LoadMesh3DIDList();
        u32                 getMesh3DNumber() {return m_mesh3DFileList.size(); };
        
        void                UnloadSkeleton3DID();
        void                LoadSkeleton3DID();

        void                UnloadAnimation3DID();
        void                LoadAnimation3DID();
        
        void                UnloadAnimation3DIDList();
        void                LoadAnimation3DIDList();

        bbool               isResourceInList(const ITF_VECTOR<ResourceID> & _Resources, ResourceID _res);

        void                updateAABB();
        void                updateAABB(Mesh3DResource* _mesh3Dres, Mesh3DPrim &_mesh3D);

        ITF_INLINE const Color&                     getDefaultColor() const;
		ITF_INLINE const Mesh3DComponent_Template*  getTemplate() const;

	public:

        const Animation3DList & getAnimation3DList()    { return m_animation3DList; }
        const Animation3DSet &  getAnimation3DSet()     { return m_animation3DSet; }

		void				changeMaterial(const u32 elementIndex, const u32 materialID);

        void                ComputeInitMatrix( Matrix44 &_matrix ) const;

		bbool				getInactive()						{ return m_Inactive;	}
		void				setInactive(const bbool _b)			{ m_Inactive = _b;	}
		bbool				getInvisible()						{ return m_Invisible;	}
		void				setInvisible(const bbool _b)		{ m_Invisible = _b;	}

												 
        ITF_INLINE Skeleton3D * getSkeleton3D() {return m_anim.getSkeleton();};
        u32                 getBoneCount() const;
        bbool               hasBones() const;
        bbool               isBoneIndexValid( int _boneIndex ) const;
        bbool               getBonePos(u32 _boneIndex, Vec3d& _dest ) const;
        bbool               getBoneMatrix(u32 _boneIndex, Matrix44& _matrix) const;
        bbool               getBoneGlobalPos(u32 _boneIndex, Vec3d& _dest ) const;
        bbool               getBoneGlobalMatrix(u32 _boneIndex, Matrix44& _matrix) const;
        bbool               getBoneGlobalMatrixFromFather(u32 _boneIndex, Matrix44& _matrix);
        
        bbool               getPolylineMatrices( const String8 &_name, ITF_VECTOR<Matrix44> &matrixList, bbool _global );
        bbool               getPolylinePosition( const String8 &_name, ITF_VECTOR<Vec3d> &positionList, bbool _global );
        bbool               getPolylineMatrices( int _index, ITF_VECTOR<Matrix44> &matrixList, bbool _global );
        bbool               getPolylinePosition( int _index, ITF_VECTOR<Vec3d> &positionList, bbool _global );

        Animation3DPlayer * getAnimPlayer() { return &m_anim; };
		void                setAction( StringID _action );

        void                updateFinalMatrixScale() { ComputeMatrix(m_FinalMatrix); }      // must be call if you change the scale of the actor at runtime

        // IAnimCommonInterface
        virtual void       IAnimsetAnim( const StringID& _id );
        virtual bbool      IAnimisAnimFinished() const;
        virtual bbool      IAnimisAnimLooped() const;
        virtual u32        IAnimgetNumAnimsPlaying() const;
        virtual void       IAnimdbgGetCurAnim( u32 _index, String8& _out ) const;

        ITF_INLINE  void   setOrientation(const Matrix44& _m44) { m_orientation = _m44; }
        ITF_INLINE  const Matrix44&     getOrientation()   const { return m_orientation; }

        ITF_INLINE  const Matrix44&     getFinalMatrix()    const { return m_FinalMatrix; }

        // Anim Modification
        void                externalAnimModManagement();
        void                allowAnimMod(bbool _allowAnimMod) { m_allowAnimMod = _allowAnimMod; }

        // IK
        void                IKManagement();

        ITF_INLINE  void    useIK(bbool _value) { m_useIK = _value; }

        void                requestIK( StringID _nameIK, StringID _boneName, IKType _IKType, const Matrix44& _targetMatrix, u32 _boneNb, f32 _maxRotationByBone, f32 _approximationCoeff = 1.8f, f32 _blendCoeff = 1.0f, i32 _maxIter = 30, f32 _maxValue = 0.01f );
        u32                 getBoneNbToUppestParentIK( StringID _boneName, i32& _boneIdx );
        void                registerMatricesFromRefBonesIK( i32 _refBoneIdx, u32 _boneMaxNb );

        void                matrix44TargetIK( const Matrix44& _target, f32 _maxRotationByBone, i32 _maxIterations, f32 _maxDist );               // Arms & Legs (to prevent broken wrist)
        void                matrix44TargetIKReverse( const Matrix44& _target, f32 _maxRotationByBone, i32 _maxIterations, f32 _maxDist );        // Neck & other stuff
        void                matrix44TargetIKLookAt( const Matrix44& _target, f32 _maxRotationByBone, i32 _maxIterations, f32 _maxAngle );        
        void                matrix44TargetIKSmoothChain( StringID _nameIK, const Matrix44& _target, f32 _maxRotationByBone, f32 _approximationCoeff, f32 _blendCoeff);

        void                applyIK(i32 _refBoneIdx);
        void                setPlayRate(f32 _playRate) { m_playRate =_playRate; }
        f32                 getPlayRate() const { return m_playRate; }

		void				setVisibility(Animation3D *_anim3D);
		void				setVisibilityTree(Animation3DTreeResult *_anim3DTreeResult);
                                                        
        ITF_VECTOR<Matrix44>& getGizmos() { return m_Gizmos; }
              
		// Color stuff
		Color m_currentColor;
		Color m_colorSrc;
		Color m_colorDst;
		f32 m_colorBlendTotal;
		f32 m_colorBlendTime;

		void processColor( f32 _dt );

#ifdef ITF_SUPPORT_EDITOR                                                   
    public:
        virtual void        onPrePropertyChange();
        virtual void        onPostPropertyChange();
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);

        void				setUpdateAnimInput( IUpdateAnimInputCriteria* _update ) { m_animInputUpdate = _update; }

    private:
        virtual void    DrawEditor();
        void            DrawEditorMesh3D(ResourceID &_mesh3DID);


        /**/
        ux                          m_previousMaterialListSize;
        static  Path                m_edit_mesh3DFile;
        static  ITF_VECTOR<Path>    m_edit_mesh3DFileList;
        static  Path                m_edit_skeleton3DFile;
        static  Path                m_edit_animation3DFile;
        static  ITF_VECTOR<Path>    m_edit_animation3DFileList;
        static Matrix44             m_edit_orientation;

        bbool                       m_ignoreModifyInitialPosOnOrientation;
#endif // ITF_SUPPORT_EDITOR

    private:
        bbool                   getUserPropertyAtFrame(const Animation3D::UserProperty& _property, i32 _frame);
        bbool					getForcedAABB(AABB & _aabb) const;
        ITF_VECTOR<GFXMaterialSerializable> m_materialList;
        
        ResourceID              m_mesh3DID;
        Path                    m_mesh3DFile;
        ITF_VECTOR<ResourceID>  m_mesh3DIDList;
        ITF_VECTOR<Path>        m_mesh3DFileList;

        ResourceID              m_skeleton3DID;
        Path                    m_skeleton3DFile;

        ResourceID              m_animation3DID;
        Path                    m_animation3DFile;
        ITF_VECTOR<Path>        m_animation3DFileList;
        Animation3DList         m_animation3DList;
        Animation3DSet          m_animation3DSet;

        StringID                m_animation3DNode;
        Animation3DTree	*			m_animation3DTree;
        InputContainer          m_inputList;
		IUpdateAnimInputCriteria *	m_animInputUpdate;

        // matrix / special 3D component
        f32                     m_ScaleZ;
        Matrix44                m_FinalMatrix;
        Matrix44                m_ActorMatrix;
        Matrix44                m_initialOrientation;
        Matrix44                m_orientation;

        // instance data
        Mesh3DPrim              m_3Dmesh;
        ITF_VECTOR<Mesh3DPrim>  m_3DmeshList;
        ITF_VECTOR<bbool>       m_mesh3DVisibility;
        Animation3DPlayer       m_anim;

        // Gizmos
        bbool                   m_GizmoOk;
        ITF_VECTOR<Matrix44>    m_Gizmos;
        bbool                   m_allowAnimMod;
		bbool					m_updateVisibility;

        f32                     m_deltaTime;
        f32                     m_playRate;

        // IK
        bbool                   m_useIK;
        ITF_VECTOR<Matrix44>    m_matricesIK;               // order => From rootbone to bone
        ITF_VECTOR<f32>         m_sumAnglesIK;

		// Activation
		bbool m_Inactive;
		bbool m_Invisible;

        struct RequestIK
        {
            RequestIK(){}
            RequestIK(StringID _nameIK, StringID _boneName, IKType _IKType, const Matrix44& _targetMatrix, u32 _boneNb, f32 _maxRotationByBone, f32 _approxCoeff, f32 _blendCoeff, i32 _maxIter, f32 _maxValue)
                : m_nameIK(_nameIK)
                , m_boneName(_boneName)
                , m_IKType(_IKType)
                , m_refBoneIdx(-1)
                , m_targetMatrix(_targetMatrix)
                , m_boneNb(_boneNb)
                , m_maxRotationByBone(_maxRotationByBone)
                , m_approxCoeff(_approxCoeff)
                , m_blendCoeff(_blendCoeff)
                , m_maxIter(_maxIter) 
                , m_maxValue(_maxValue) {}

            StringID    m_nameIK;
            StringID    m_boneName;
            IKType      m_IKType;
            i32         m_refBoneIdx;
            u32         m_boneNb;
            Matrix44    m_targetMatrix;
            f32         m_maxRotationByBone;
            f32         m_approxCoeff;
            f32         m_blendCoeff;
            i32         m_maxIter;
            f32         m_maxValue;
        };
        ITF_VECTOR<RequestIK>   m_requestsIK;   // to handle several parts
        ITF_MAP<StringID, Matrix44> m_smoothChainMatrixBackup;

#ifdef ITF_SUPPORT_EDITOR
        bbool                   m_drawMesh;
        bbool                   m_drawNormals;
        bbool                   m_drawBV;
        bbool                   m_drawSkeleton;
        bbool                   m_freezeSkinning;
        bbool                   m_hardwareSkinning;
        f32                     m_animationTimeRatio;
        f32                     m_input0;
        f32                     m_input1;
#endif

    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Mesh3DComponent_Template class
    // 
    ///////////////////////////////////////////////////////////////////////////////////////////////
    #define Mesh3DComponent_Template_CRC ITF_GET_STRINGID_CRC(Mesh3DComponent_Template,1760375194) 
    /**/
    class Mesh3DComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Mesh3DComponent_Template,GraphicComponent_Template,1760375194)
        DECLARE_SERIALIZE()
        DECLARE_ACTORCOMPONENT_TEMPLATE(Mesh3DComponent);

    public:

        Mesh3DComponent_Template();
        ~Mesh3DComponent_Template() {}

        virtual bbool           onTemplateLoaded( bbool _hotReload );
        virtual void            onTemplateDelete( bbool _hotReload );

        const ResourceID &                          getMesh3DID()       const { return m_mesh3DID; }
        const ResourceID &		                    getMesh3DIDFromList(u32 _index)	const { return (_index >= (int) m_mesh3DIDList.size()) ? ResourceID::Invalid : m_mesh3DIDList[_index]; }
        const ResourceID &                          getSkeleton3DID()   const { return m_skeleton3DID; }
        const ITF_VECTOR<GFXMaterialSerializable> & getMaterialList()   const { return m_materialList; }
        const ResourceID &                          getAnimation3DID()  const { return m_animation3DID; };
        const ResourceID &		                    getAnimation3DIDFromList(u32 _index)	const { return (_index >= (int) m_animation3DIDList.size()) ? ResourceID::Invalid : m_animation3DIDList[_index]; }

        const Animation3DSet_Template &             getAnimation3DSet()             const { return m_animation3DSet; } 
        const Animation3DSet_Template*              getAnimation3DSetPtr()          const { return &m_animation3DSet; }
        const ITF_VECTOR<Path> &                    getAnimation3DFileList()        const { return m_animation3DFileList; }
        u32                                         getAnimation3DFileListCount()   const { return m_animation3DFileList.size(); }
        u32                                         getMesh3DFileListCount()        const { return m_mesh3DFileList.size(); }

        ITF_INLINE const Path&                      getMesh3DFile()         const {return m_mesh3DFile;}
        ITF_INLINE const Path&                      getMesh3DFileFromList(int _index)	const {return m_mesh3DFileList[_index];}
        ITF_INLINE const Path&                      getSkeleton3DFile()     const {return m_skeleton3DFile;}
        ITF_INLINE const Path&                      getAnimation3DFile()    const {return m_animation3DFile;}
        ITF_INLINE const Path&                      getAnimation3DFileFromList(int _index)	const {return m_animation3DFileList[_index];}
        ITF_INLINE const Color&                     getDefaultColor()       const {return m_defaultColor;}
        ITF_INLINE const bbool                      getForce2DRendering()       const {return m_force2DRendering;}

        Animation3DTree                           * createAnimation3DTreeInstance() const;
        const Animation3DTree_Template &            getAnimTreeTemplate() const { return m_animation3DTree; }
        const ITF_VECTOR <InputDesc>&               getInputList() const { return m_inputList; }

        u32											getNumInputs() const { return m_inputList.size(); }

        u32											findInputIndex( const StringID& _id ) const;

        const AABB&                                 getForcedAABB() const { return m_forcedAABB; }
        
        const StringID &                            getAnimationNode() const { return m_animation3DNode; }
        
    private:

		ITF_VECTOR<GFXMaterialSerializable> m_materialList;

        friend class ActorCreate_Mesh3DComponent;


        ResourceID                  m_mesh3DID;
        Path                        m_mesh3DFile;
        ITF_VECTOR<ResourceID>      m_mesh3DIDList;
        ITF_VECTOR<Path>            m_mesh3DFileList;

        ResourceID                  m_skeleton3DID;
        Path                        m_skeleton3DFile;

        ResourceID                  m_animation3DID;
        Path                        m_animation3DFile;
        ITF_VECTOR<ResourceID>      m_animation3DIDList;
        ITF_VECTOR<Path>            m_animation3DFileList;
        //Animation3DList         m_animation3DList;

        Animation3DSet_Template     m_animation3DSet;

        Animation3DTree_Template    m_animation3DTree;
        ITF_VECTOR <InputDesc>      m_inputList;

        ArchiveMemory               m_instanceData;
        Color                       m_defaultColor;

        AABB                       m_forcedAABB;

        bbool                       m_force2DRendering;
        StringID                    m_animation3DNode;
    };
}

#endif // _ITF_MESH3DCOMPONENT_H_
