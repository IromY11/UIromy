#include "precompiled_engine.h"

#ifndef _ITF_ANIMATION3D_H_
#include    "engine/animation3D/Animation3D.h"
#endif // _ITF_ANIMATION3D_H_

#ifndef _ITF_ANIMATION3DINFO_H_
#include    "engine/animation3D/Animation3DInfo.h"
#endif // _ITF_ANIMATION3DINFO_H_

#ifndef _ITF_ANIMATION3DSET_H_
#include    "engine/animation3D/Animation3DSet.h"
#endif // _ITF_ANIMATION3DSET_H_

#ifndef _ITF_SKELETON3D_H_
#include    "engine/animation3D/Skeleton3D.h"
#endif // _ITF_SKELETON3D_H_

#ifndef _ITF_ANIMATION3DPLAYER_H_
#include    "engine/animation3D/Animation3Dplayer.h"
#endif // _ITF_ANIMATION3DPLAYER_H_

#ifndef _ITF_ANIMATION3DTREERESULT_H_
#include "engine/blendTree/Animation3DTree/Animation3DTreeResult.h"
#endif //_ITF_ANIMATION3DTREERESULT_H_

#ifndef __EDGE_ANIM_PPU_H__
#include "edge/anim/edgeanim_ppu.h"
#endif // __EDGE_ANIM_PPU_H__

#ifndef _MTH_SIMD_QUATERNION_H_
#include "core/math/SIMD/SIMD_Quaternion.h"
#endif // _MTH_SIMD_QUATERNION_H_

#include "Jobs/EdgeAnim/Animation3Dplayeredge_job.h"

#ifdef ITF_PS3
    #define USE_EDGE_ANIM_JOB
    #include "adapters/SystemAdapter_PS3/SystemAdapter_PS3.h"
    #include "core/system/PS3/Job/AdvJobDynChain_PS3.h"
    #include "core/system/PS3/Job/AdvJobDynChain_Impl_PS3.h"

    PURS_DECLARE_JOB_CODE_ENTRY(Job_EdgeAnim);
#endif


namespace ITF
{
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // some EDGE defines 
    ///////////////////////////////////////////////////////////////////////////////////////////////
    #define SPURS_SPU_NUM                           2
    #define EDGE_ANIM_EXTERNAL_POSE_CACHE_MASK	    -1			// all SPUs need external pose cache space in main memory
    #define EDGE_ANIM_EXTERNAL_POSE_CACHE           (128*1024)	// arbitrarily allocate 128KB for each SPUs pose stack extension in main memory
    #define EDGE_ANIM_SCRATCH_BUFFER                (96*1024)   // 128k allows 40 pose stack for 50 bones

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // EDG class :interface with edge
    ///////////////////////////////////////////////////////////////////////////////////////////////
    class EDG
    {
    public:

        static EDG *getSingleton()
        {
#ifdef USE_EDGEANIM
            if (NULL == mpo_Singleton ) mpo_Singleton = newAlloc( mId_Edge, EDG());
            return mpo_Singleton;
#else
            return NULL;
#endif // USE_EDGEANIM
        }

        static void DestroyEdgeEngine()
        {
#ifdef USE_EDGEANIM
            if (mpo_Singleton)
            {
                ITF::Memory::alignedFree(mpo_Singleton->mp_EdgeAnimPpuContext);
                mpo_Singleton->mp_EdgeAnimPpuContext = NULL;
                ITF::Memory::alignedFree(mpo_Singleton->mp_ExternalSpuStorage);
                mpo_Singleton->mp_ExternalSpuStorage = NULL;
                ITF::Memory::alignedFree(mpo_Singleton->mp_ScratchBufferPC);
                mpo_Singleton->mp_ScratchBufferPC = NULL;
                delete(mpo_Singleton);
                mpo_Singleton = NULL;
            }
#endif // USE_EDGEANIM
        }

        //void PlayOneAnim(EdgeAnimSkeleton *_skeleton, EdgeAnimAnimation * _animation, f32 _fTime, ITF_VECTOR<Matrix44> & _resultLocalMat);
        void PlayAnimTree(EdgeAnimSkeleton *_skeleton, ITF_VECTOR<ITF_EdgeAnimBlendBranch> &_branches, ITF_VECTOR<ITF_EdgeAnimBlendLeaf> &_leaves, ITF_VECTOR<Matrix44> & _resultLocalMat);
        void Release();

    public:
        EdgeAnimPpuContext *    mp_EdgeAnimPpuContext;
        void *                  mp_ExternalSpuStorage;

        // PC Only
        uint8_t *               mp_ScratchBufferPC;

#ifdef USE_EDGE_ANIM_JOB
        AdvJobDynChain<Purs::Job256> m_jobChain;
#endif

    private:
        EDG();
        ~EDG() { }

        //JOB_Descriptor_S * runJob(SKL & _skl, ITF_EdgeAnimBlendBranch * _branches, u32 _numBranches, ITF_EdgeAnimBlendLeaf * _leaves, u32 _numLeaves);
        void runJob(EdgeAnimSkeleton &_skeleton, ITF_EdgeAnimBlendBranch * _branches, u32 _numBranches, ITF_EdgeAnimBlendLeaf * _leaves, u32 _numLeaves, ITF_VECTOR<Matrix44> & _resultLocalMat);

    private:
        static EDG* mpo_Singleton;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // some EDGE data
    ///////////////////////////////////////////////////////////////////////////////////////////////
    EDG* EDG::mpo_Singleton = NULL;
    

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // constructor 
    ///////////////////////////////////////////////////////////////////////////////////////////////
    EDG::EDG()
#if defined(USE_EDGE_ANIM_JOB)
        : m_jobChain(
                        Purs::JobChainDesc(static_cast<SystemAdapter_PS3 *>(SystemAdapter::getptr())->getMainSpursInstance(), "EdgeAnim"),
                        10, 4 // enqueue up to 10*(4-1) jobs until stall, sync every 5 jobs
                    )
#endif
    {
#if !(defined ITF_IOS || defined ITF_ANDROID)
        // intialize edge animation
        mp_EdgeAnimPpuContext = (EdgeAnimPpuContext*)  ITF::Memory::alignedMallocCategory(sizeof(EdgeAnimPpuContext), 16,MemoryId::mId_Edge);
        uSize externalCacheSize = uSize(edgeAnimComputeExternalStorageSize( SPURS_SPU_NUM,EDGE_ANIM_EXTERNAL_POSE_CACHE_MASK,EDGE_ANIM_EXTERNAL_POSE_CACHE ));

        mp_ExternalSpuStorage = ITF::Memory::alignedMallocCategory( externalCacheSize, 16 ,MemoryId::mId_Edge);
        edgeAnimPpuInitialize( mp_EdgeAnimPpuContext,SPURS_SPU_NUM,EDGE_ANIM_EXTERNAL_POSE_CACHE_MASK,EDGE_ANIM_EXTERNAL_POSE_CACHE,mp_ExternalSpuStorage );
#endif

        // animation scratch buffer when running on PC
        mp_ScratchBufferPC = (uint8_t*) ITF::Memory::alignedMallocCategory( EDGE_ANIM_SCRATCH_BUFFER, 128 ,MemoryId::mId_Edge);
    }

#ifdef USE_EDGE_ANIM_JOB
    PURS_IMPLEMENT_JOB_CODE(Job_EdgeAnim);

    static void waitEndOfEdgeJob(volatile u64 * _setAtEnd)
    {
        while (!*_setAtEnd)
        {
            sys_timer_usleep(50);
        }
    }
#endif

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // do edge evaluation
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void EDG::runJob(EdgeAnimSkeleton &_skeleton, ITF_EdgeAnimBlendBranch *_branches, u32 _numBranches, ITF_EdgeAnimBlendLeaf *_leaves, u32 _numLeaves, ITF_VECTOR<Matrix44> & _resultLocalMat)
    {
#ifdef USE_EDGEANIM
        Matrix44 *sklLocal = &_resultLocalMat.front();
#if defined(USE_EDGE_ANIM_JOB)
        u32 nbResultMat = _resultLocalMat.size();

        Purs::Job256 * job = m_jobChain.GetNewCommandData();
        job->Reset();
        job->SetCode(Job_EdgeAnim);
        job->SetInputData(0, Purs::JobInput(_branches, _numBranches * sizeof(ITF_EdgeAnimBlendBranch)));
        job->SetInputData(1, Purs::JobInput(_leaves, _numLeaves * sizeof(ITF_EdgeAnimBlendLeaf)));
        job->SetReadOnlyInputData(2, Purs::JobReadOnlyInput(&_skeleton, _skeleton.sizeTotal));
        job->SetReadOnlyInputData(3, Purs::JobReadOnlyInput(mp_EdgeAnimPpuContext, sizeof(EdgeAnimPpuContext)));
        job->SetOutputData(4, Purs::JobOutput(sklLocal, sizeof(Matrix44) * nbResultMat));
        job->SetUserData(5, _numBranches);
        job->SetUserData(6, _numLeaves);
        job->SetUserData(8, 0); // slot 8 is set to 1 at job end (fenced) so job completion can be checked this way
        job->AddScratchSize(EDGE_ANIM_SCRATCH_BUFFER);
        m_jobChain.PushCommand(*job);
        
        waitEndOfEdgeJob(&job->workArea.userData[8]); // immediate synch
#else   // No job for edge anim
        runEdgeAnimJob(&_skeleton, mp_EdgeAnimPpuContext, _branches, _numBranches, _leaves, _numLeaves, sklLocal, mp_ScratchBufferPC, EDGE_ANIM_SCRATCH_BUFFER);
#endif // !USE_EDGE_ANIM_JOB
#endif // USE_EDGEANIM
    }


    //======================================================================================================================
    //
    //======================================================================================================================
    void EDG::PlayAnimTree(EdgeAnimSkeleton *_skeleton, ITF_VECTOR<ITF_EdgeAnimBlendBranch> &_branches, ITF_VECTOR<ITF_EdgeAnimBlendLeaf> &_leaves, ITF_VECTOR<Matrix44> & _resultLocalMat)
    {
        if (_branches.size())
            runJob(*_skeleton, &_branches[0], _branches.size(), &_leaves[0], _leaves.size(), _resultLocalMat);
        else
            runJob(*_skeleton, NULL, 0, &_leaves[0], _leaves.size(), _resultLocalMat);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Update with edge player
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bbool Animation3DPlayer::updateEdge( f32 _deltaTime, ITF_VECTOR<Matrix44> &_globalM)
    {
        if (!m_Skeleton || !m_Skeleton->m_edgeSkeleton) return bfalse;

        updateTimer( _deltaTime );

        if (m_action.isValid())
        {
            m_MagicBoxPrev = m_MagicBox;
            updateTree( _deltaTime );
        }
        else
        {
            if (!m_Animation || !m_Animation->m_edgeAnimation) 
                return bfalse;
            updateProperties();
            buildTree( m_Animation, m_TimerSnapToFrame );
        }

        if (m_BlendLeaves.size() == 0)
            return bfalse;

        m_Skeleton->InitLocal(m_ListMatrix);
        m_skeletonIsUpdate = btrue;

        EDG::getSingleton()->PlayAnimTree( m_Skeleton->m_edgeSkeleton, m_BlendBranches, m_BlendLeaves, m_ListMatrix );

//        m_Skeleton->ComputeGlobal(m_ListMatrix, _globalM);
        m_MagicBox = m_ListMatrix[0];
        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Copy Matrices
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void Animation3DPlayer::copyLocalMatrices(ITF_VECTOR<Matrix44> &_globalM)
    {
        for( u32 i = 0; i < m_ListMatrix.size(); i++ )
            _globalM[i] = m_ListMatrix[i];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Paste Matrices
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void Animation3DPlayer::pasteLocalMatrices(ITF_VECTOR<Matrix44> &_globalM)
    {
        for( u32 i = 0; i < m_ListMatrix.size(); i++ )
            m_ListMatrix[i] = _globalM[i];
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Compute Global Matrices
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void Animation3DPlayer::computeGlobal(ITF_VECTOR<Matrix44> &_globalM)
    {
        m_Skeleton->ComputeGlobal(m_ListMatrix, _globalM);
        m_skeletonIsUpdate = btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // init/destroy edge
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void Animation3DPlayer::edgeInit()
    {
        EDG::getSingleton();
    }
    /**/
    void Animation3DPlayer::edgeDestroy()
    {
        EDG::DestroyEdgeEngine();
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update tree edge from animation3D tree result
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void Animation3DPlayer::buildEdgeTreeClean()
    {
        m_BlendBranches.clear();
        m_BlendLeaves.clear();
    }
    /**/
    void Animation3DPlayer::buildEdgeTreeReserve( u32 _size )
    {
        _size = _size < 8 ? 8 : _size;
        m_BlendBranches.reserve( _size );
        m_BlendLeaves.reserve( _size );
    }
    /**/
    f32 Animation3DPlayer::buildEdgeTreeLeaf(const Animation3DTreeResultLeaf &leaf)
    {
        if (leaf.m_animation3DInfo == NULL)
            return F32_INVALID;

        EdgeAnimAnimation *edgeAnim = leaf.m_animation3DInfo->getTemplate()->getAnimation3D()->m_edgeAnimation;
        ITF_EdgeAnimBlendLeaf edgeLeaf = ITF_EdgeAnimBlendLeaf( edgeAnim, edgeAnim->sizeHeader, leaf.m_timeOffset );
        m_BlendLeaves.push_back(edgeLeaf);
        return leaf.m_weight;
    }
    /**/
    f32 Animation3DPlayer::buildEdgeTreeLeaf(Animation3D *_animation, f32 _time)
    {
        EdgeAnimAnimation *edgeAnim = _animation->m_edgeAnimation;
        ITF_EdgeAnimBlendLeaf edgeLeaf = ITF_EdgeAnimBlendLeaf( edgeAnim, edgeAnim->sizeHeader, _time );
        m_BlendLeaves.push_back(edgeLeaf);
        return 1.0f;
    }
    /**/
    void Animation3DPlayer::buildEdgeTreeBranch( bbool _doubleLeaf, uint16_t _left, f32 _leftWeight, uint16_t _right, f32 _rightWeight )
    {
        int16_t left = (_doubleLeaf ? EDGE_ANIM_BLEND_TREE_INDEX_LEAF : EDGE_ANIM_BLEND_TREE_INDEX_BRANCH) | _left;
        int16_t right = EDGE_ANIM_BLEND_TREE_INDEX_LEAF | _right;
        f32     weight = _rightWeight / (_leftWeight + _rightWeight);
        
        ITF_EdgeAnimBlendBranch g_branch = ITF_EdgeAnimBlendBranch(  EDGE_ANIM_BLENDOP_BLEND_LINEAR, left, right, weight);
        m_BlendBranches.push_back( g_branch );
    }

    void Animation3DPlayer::buildEdgeTreeBranchAdd( bbool _doubleLeaf, uint16_t _left, f32 _leftWeight, uint16_t _right, f32 _rightWeight )
    {
        int16_t left = (_doubleLeaf ? EDGE_ANIM_BLEND_TREE_INDEX_LEAF : EDGE_ANIM_BLEND_TREE_INDEX_BRANCH) | _left;
        int16_t right = EDGE_ANIM_BLEND_TREE_INDEX_LEAF | _right;

        ITF_EdgeAnimBlendBranch g_branch = ITF_EdgeAnimBlendBranch( EDGE_ANIM_BLENDOP_BLEND_ADD_DELTA_RIGHT, left, right, _rightWeight);
        m_BlendBranches.push_back( g_branch );
    }
    /**/
    void Animation3DPlayer::buildEdgeTree( Animation3DTreeResult *_treeResult )
    {
        buildEdgeTreeClean();

        // check size
        if (!_treeResult->m_anims.size())
            return;

        // reserve enough memory
        buildEdgeTreeReserve(_treeResult->m_anims.size());

        // only one anim => only one leaf
        if (_treeResult->m_anims.size() == 1)
        {
            const Animation3DTreeResultLeaf &leaf = _treeResult->m_anims[0];
            if (leaf.m_reachEnd)
                m_MagicBoxPrevValid = btrue;
            buildEdgeTreeLeaf(leaf);
            return;
        }

        // get total weight for weight normalization
        u32 leafCount = _treeResult->m_anims.size();
        f32 totalWeight = 1.f;

        // build first branch with two first leaves
        f32 weight0, weight1;
        uint16_t leafCur = 0, branchLast = 0;

        weight0 = buildEdgeTreeLeaf(_treeResult->m_anims[0]) * totalWeight;
        weight1 = buildEdgeTreeLeaf(_treeResult->m_anims[1]) * totalWeight;
        if (weight0 == F32_INVALID || weight1 == F32_INVALID)
        {
            buildEdgeTreeClean();
            return;
        }

        // create branch
        if (f32_Abs(weight1 + weight0 - 1.f) < MTH_EPSILON)
            buildEdgeTreeBranch( btrue, leafCur, weight0, leafCur + 1, weight1 );
        else
            buildEdgeTreeBranchAdd( btrue, leafCur, weight0, leafCur + 1, weight1 );
        
        branchLast = 0;
        leafCur = 2;
        weight0 += weight1;

        // create other branches = last branch + next leaf
        while (leafCur < leafCount)
        {
            weight1 = buildEdgeTreeLeaf(_treeResult->m_anims[leafCur]) * totalWeight;
            if (weight1 == F32_INVALID)
            {
                buildEdgeTreeClean();
                return;
            }

            // create branch
            buildEdgeTreeBranch( bfalse, branchLast, weight0, leafCur, weight1 );
            
            branchLast++;
            leafCur++;
            weight0 += weight1;
        }
    }

    /**/
    void Animation3DPlayer::buildEdgeTree(Animation3D *_animation, f32 _time )
    {
        buildEdgeTreeClean();
        buildEdgeTreeReserve(1);
        buildEdgeTreeLeaf(_animation, _time);
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // save edge tree for blend purpose
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void Animation3DPlayer::storeEdgeClean()
    {
        m_storeBlendBranches.clear();
        m_storeBlendLeaves.clear();
        m_blendFactor = -1.0f;
    }
    /**/
    void Animation3DPlayer::storeEdgeTree()
    {
        m_storeBlendBranches.clear();
        m_storeBlendBranches.reserve( m_BlendBranches.size() );
        m_storeBlendLeaves.clear();
        m_storeBlendLeaves.reserve( m_BlendLeaves.size() );

        u32 i;
        for (i = 0; i < m_BlendLeaves.size(); i++)
        {
            ITF_EdgeAnimBlendLeaf &blendLeaf = m_BlendLeaves[i];
            ITF_EdgeAnimBlendLeaf edgeLeaf = ITF_EdgeAnimBlendLeaf
                ( 
                (EdgeAnimAnimation*) blendLeaf.animationHeaderEa, 
                blendLeaf.animationHeaderSize,
                blendLeaf.evalTime, 
                blendLeaf.flags, 
                blendLeaf.userVal
                );
            m_storeBlendLeaves.push_back(edgeLeaf);
        }
        
        for ( i = 0; i < m_BlendBranches.size(); i++)
        {
            ITF_EdgeAnimBlendBranch &blendBranch = m_BlendBranches[i];
            ITF_EdgeAnimBlendBranch edgeBranch = ITF_EdgeAnimBlendBranch
                ( 
                (EdgeAnimBlendOp) blendBranch.operation, 
                blendBranch.left,
                blendBranch.right, 
                blendBranch.alpha, 
                blendBranch.flags,
                blendBranch.userVal
                );
            m_storeBlendBranches.push_back(edgeBranch);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // merge stored edge tree with current played
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void Animation3DPlayer::mergeStoredEdgeTree()
    {
        if (m_storeBlendLeaves.size() == 0) return;

        uint16_t leafOffset = (uint16_t) m_BlendLeaves.size();
        uint16_t branchOffset = (uint16_t) m_BlendBranches.size();

        u32 i;
        uint16_t left;
        uint16_t right;

        for (i = 0; i < m_storeBlendLeaves.size(); i++)
        {
            ITF_EdgeAnimBlendLeaf &blendLeaf = m_storeBlendLeaves[i];
            ITF_EdgeAnimBlendLeaf edgeLeaf = ITF_EdgeAnimBlendLeaf
            ( 
                (EdgeAnimAnimation*) blendLeaf.animationHeaderEa, 
                blendLeaf.animationHeaderSize,
                blendLeaf.evalTime, 
                blendLeaf.flags, 
                blendLeaf.userVal
            );
            m_BlendLeaves.push_back(edgeLeaf);
        }

        for ( i = 0; i < m_storeBlendBranches.size(); i++)
        {
            ITF_EdgeAnimBlendBranch &blendBranch = m_storeBlendBranches[i];
            left = blendBranch.left;
            if (left & EDGE_ANIM_BLEND_TREE_INDEX_LEAF) left += leafOffset;
            else left += branchOffset;

            right = blendBranch.right;
            if (right & EDGE_ANIM_BLEND_TREE_INDEX_LEAF) right += leafOffset;
            else right += branchOffset;

            ITF_EdgeAnimBlendBranch edgeBranch = ITF_EdgeAnimBlendBranch
            ( 
                (EdgeAnimBlendOp) blendBranch.operation, 
                left,
                right, 
                blendBranch.alpha, 
                blendBranch.flags,
                blendBranch.userVal
            );
            m_BlendBranches.push_back(edgeBranch);
        }

        // add a branch to blend the two tree
        left = branchOffset ? (EDGE_ANIM_BLEND_TREE_INDEX_BRANCH | (uint16_t)(branchOffset - 1u)) : EDGE_ANIM_BLEND_TREE_INDEX_LEAF;
        right = m_storeBlendBranches.size() ? (EDGE_ANIM_BLEND_TREE_INDEX_BRANCH | (uint16_t)(m_BlendBranches.size() - 1u)) : (EDGE_ANIM_BLEND_TREE_INDEX_LEAF | leafOffset);
        f32     weight = m_blendFactor;
        ITF_EdgeAnimBlendBranch g_branch = ITF_EdgeAnimBlendBranch( EDGE_ANIM_BLENDOP_BLEND_LINEAR, left, right, weight );
        m_BlendBranches.push_back( g_branch );
    }

} // namespace ITF


