#include "precompiled_engine.h"

#ifndef _ITF_ANIMMESHVERTEXPETCOMPONENT_H_
#include "engine/actors/components/AnimMeshVertexPetComponent.h"
#endif //_ITF_ANIMMESHVERTEXPETCOMPONENT_H_

#ifndef _ITF_ANIMMESHVERTEXCOMPONENT_H_
#include "engine/actors/components/AnimMeshVertexComponent.h"
#endif //_ITF_ANIMMESHVERTEXCOMPONENT_H_


#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RESOURCE_CONTAINER_H_
#include "engine/resources/ResourceContainer.h"
#endif //_ITF_RESOURCE_CONTAINER_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

namespace ITF
{
    BEGIN_SERIALIZATION(AnimMeshVertexPetPart)
        SERIALIZE_MEMBER("type", m_type);
        SERIALIZE_MEMBER("variant", m_variant)
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(AnimMeshVertexPetData)
        SERIALIZE_CONTAINER_OBJECT("parts", m_parts)
        SERIALIZE_MEMBER("position", m_position)
        SERIALIZE_MEMBER("angle", m_angle)
        SERIALIZE_MEMBER("scale", m_scale)
    END_SERIALIZATION()


#ifdef USE_BODY_OFFSET
    BEGIN_SERIALIZATION(PartOffsetData)
        SERIALIZE_MEMBER("name", m_name)
        SERIALIZE_MEMBER("position", m_position)
        SERIALIZE_MEMBER("angle", m_angle)
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(TypeOffsetData)
        SERIALIZE_MEMBER("type", m_type)
        SERIALIZE_CONTAINER_OBJECT("partOffset", m_partOffset)
    END_SERIALIZATION()
#endif

    IMPLEMENT_OBJECT_RTTI(AnimMeshVertexPetComponent_Template)

    BEGIN_SERIALIZATION_CHILD(AnimMeshVertexPetComponent_Template)
        SERIALIZE_MEMBER("allowUpdate", m_allowUpdate)
        SERIALIZE_CONTAINER("baseParts", m_baseParts)
        SERIALIZE_CONTAINER("animList", m_animList)
        SERIALIZE_CONTAINER_OBJECT("pets", m_petList)    
        SERIALIZE_CONTAINER("randomPets", m_randomPetsPosList)    
        SERIALIZE_CONTAINER_OBJECT("allPets", m_allPetList)

#ifdef USE_BODY_OFFSET
        SERIALIZE_CONTAINER("bodyIndex", m_bodyIndex)
        SERIALIZE_CONTAINER_OBJECT("bodyOffsetList", m_bodyOffsetList)
#endif
    END_SERIALIZATION()

    AnimMeshVertexPetComponent_Template::AnimMeshVertexPetComponent_Template()
        : Super()
        , m_allowUpdate(btrue)
#ifdef USE_BODY_OFFSET        
        , m_bodyIndex(U32_INVALID)
#endif
    {
    }

    AnimMeshVertexPetComponent_Template::~AnimMeshVertexPetComponent_Template()
    {
    }

	bbool AnimMeshVertexPetComponent_Template::onTemplateLoaded( bbool _hotReload)
    {
        bbool bOk = Super::onTemplateLoaded(_hotReload);    

#ifdef USE_BODY_OFFSET
        // fill m_partOffsetListByType from m_bodyOffsetList
        const ITF_VECTOR<String8> & basePart        = getBaseParts();
        ITF_VECTOR<StringID>        basePartId;
        u32                         basePartSize    = basePart.size();
        basePartId.resize(basePartSize);

        ITF_VECTOR<StringID>::iterator              baseIterId  = basePartId.begin();
        for (ITF_VECTOR<String8>::const_iterator    baseIter    = basePart.begin();
            baseIter != basePart.end(); ++baseIter, ++baseIterId)
        {
            *baseIterId = *baseIter;
        }

        for (ITF_VECTOR<TypeOffsetData>::const_iterator typeIter = m_bodyOffsetList.begin();
             typeIter != m_bodyOffsetList.end(); ++typeIter)
        {
            const TypeOffsetData &  typeOffset  = *typeIter;
            u32                     index       = typeOffset.m_type.cStr()[0] - 'A';

            if (index >= m_partOffsetListByType.size())
                m_partOffsetListByType.resize(index+1);

            ITF_VECTOR<Transform3d> & offsetList = m_partOffsetListByType[index];
            offsetList.resize(basePartSize);
            for (u32 i=0; i<basePartSize; i++)
                offsetList[i].setIdentity();

            ITF_VECTOR<PartOffsetData>::const_iterator partIter = typeOffset.m_partOffset.begin();
            for (; partIter != typeOffset.m_partOffset.end(); ++partIter)
            {
                const StringID & name = partIter->m_name;
                for (u32 i=0; i<basePartSize; i++)
                {
                    if (basePartId[i] == name)
                    {
                        offsetList[i].setFrom(partIter->m_position, partIter->m_angle * MTH_DEGTORAD);
                        break;
                    }
                }
            }
        }
#endif
        return bOk;
    }

    //---------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(AnimMeshVertexPetComponent)

    BEGIN_SERIALIZATION_CHILD(AnimMeshVertexPetComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(AnimMeshVertexPetComponent)
        VALIDATE_PARAM("", m_amvComponent, "Pets needs AnimMeshVertexComponent");
    END_VALIDATE_COMPONENT()

    AnimMeshVertexPetComponent::AnimMeshVertexPetComponent()
        : Super()
        , m_amvComponent(NULL)
    {
    }

    AnimMeshVertexPetComponent::~AnimMeshVertexPetComponent()
    {
    }


    void AnimMeshVertexPetComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
    }

    void AnimMeshVertexPetComponent::Update( const f32 _dt )
    {
        Super::Update(_dt);

        u32 nbPets = m_animListData.size();
        for (u32 i=0; i<nbPets; i++)
        {
            u32 frame = CURRENTFRAME;
            setPetFrame(i, frame);
        }
    }

    bbool AnimMeshVertexPetComponent::setPetAnim(u32 _petIdx, const StringID & _petAnim)
    {
        if (!m_amvComponent || _petIdx >= m_animListData.size() )
            return bfalse;

        AnimMeshVertexPetAnim & animData = m_animListData[_petIdx];

        ITF_MAP<StringID, ITF_VECTOR<u32> >::const_iterator animIter = animData.m_animNameToSubAnimIndexes.find(_petAnim);
        if (animIter == animData.m_animNameToSubAnimIndexes.end())
            return bfalse;

        const ITF_VECTOR<u32> & lst = animIter->second;

        u32 inRuntimeIdx = animData.m_startIdx;
        ITF_VECTOR<u32>::const_iterator lstIter = lst.begin();
        for (u32 offsetIdx=0; lstIter != lst.end(); ++lstIter, ++offsetIdx)
        {
            if (*lstIter == U32_INVALID)
                continue;

            m_amvComponent->setAMVAnim(inRuntimeIdx, *lstIter);
            inRuntimeIdx++;
        }


#ifdef USE_BODY_OFFSET        
        const ITF_VECTOR<Transform3d> * offsetListBase = NULL;
        if (animData.m_animType)
            offsetListBase  = &getTemplate()->getPartOffsetList(animData.m_animType);
        lstIter             = lst.begin();
        inRuntimeIdx        = 0;
        for (u32 offsetIdx  = 0; lstIter != lst.end(); ++lstIter, ++offsetIdx)
        {
            if (*lstIter == U32_INVALID)
                continue;

            animData.m_partOffset[inRuntimeIdx] = offsetListBase ? (*offsetListBase)[offsetIdx] : Transform3d::Identity();
            inRuntimeIdx++;
        }
#endif

        return btrue;

    }

    u32 AnimMeshVertexPetComponent::getPetNbFrames(u32 _petIdx)
    {
        if (!m_amvComponent || _petIdx >= m_animListData.size() )
            return U32_INVALID;

        AnimMeshVertexPetAnim       & animData = m_animListData[_petIdx];
        AnimMeshVertex * amnMeshVertex = m_amvComponent->getAnimMeshVertex();
         
         return amnMeshVertex->getNbFrameForAnim( m_amvComponent->getAMVAnim(animData.m_startIdx));
    }

    bbool AnimMeshVertexPetComponent::setPetFrame(u32 _petIdx, u32& _frame)
    {
        if (!m_amvComponent || _petIdx >= m_animListData.size() )
            return bfalse;

        u32 nbFrames = getPetNbFrames(_petIdx);
        _frame = _frame % nbFrames;

        AnimMeshVertexPetAnim       & animData = m_animListData[_petIdx];
        for (u8 i=0; i<animData.m_nbIdx; i++)
            m_amvComponent->setAMVFrame(animData.m_startIdx+i, _frame);

        return btrue;
    }

    bbool AnimMeshVertexPetComponent::setPetTransform(u32 _petIdx, const Transform3d & _transform)
    {
        if (!m_amvComponent || _petIdx >= m_animListData.size() )
            return bfalse;

        AnimMeshVertexPetAnim       & animData = m_animListData[_petIdx];

#ifdef USE_BODY_OFFSET
        ITF_VECTOR<Transform3d>     & animOffset = m_animListData[_petIdx].m_partOffset;
        ITF_VECTOR<Transform3d>::iterator offsetIter = animOffset.begin();

        for (u8 i=0; i<animData.m_nbIdx; i++, ++offsetIter)
            m_amvComponent->setAMVTransform(animData.m_startIdx+i, _transform * (*offsetIter));
#else
        for (u8 i=0; i<animData.m_nbIdx; i++)
            m_amvComponent->setAMVTransform(animData.m_startIdx+i, _transform);
#endif

        return btrue;
    }

    bbool AnimMeshVertexPetComponent::setPetColor(u32 _petIdx, const Color & _color)
    {
        if (!m_amvComponent || _petIdx >= m_animListData.size() )
            return bfalse;

        AnimMeshVertexPetAnim       & animData = m_animListData[_petIdx];
        for (u8 i=0; i<animData.m_nbIdx; i++)
            m_amvComponent->setAMVColor(animData.m_startIdx+i, _color);

        return btrue;
    }


    void AnimMeshVertexPetComponent::updateAllUvRedirect()
    {
        for (ITF_VECTOR<AnimMeshVertexPetAnim>::iterator animDataIter = m_animListData.begin();
            animDataIter != m_animListData.end(); ++animDataIter)
        {
            AnimMeshVertexPetAnim       & animData = *animDataIter;
            for (u8 i=0; i<animData.m_nbIdx; i++)
                m_amvComponent->setAMVUVRedirectPtr(animData.m_startIdx+i, &animData.m_uvRedirect);
        }
    }


    bbool AnimMeshVertexPetComponent::fillPetListAdnPetAnim(const AnimMeshVertexPetData & _petData,
                                                            SingleAnimDataRuntimeList & _animList, 
                                                            AnimMeshVertexPetAnim     &  _petAnim)
    {
        if (!m_amvComponent)
            return bfalse;

        AnimMeshVertex * amnMeshVertex = m_amvComponent->getAnimMeshVertex();

        const ITF_VECTOR<String8> & baseParts = getTemplate()->getBaseParts();
        const ITF_VECTOR<String8> & baseAnims = getTemplate()->getAnimList();

        u32 basePartsSize = baseParts.size();

        if (baseAnims.size() == 0 || baseParts.size() == 0)
            return bfalse;

        if (_petData.m_parts.size() != basePartsSize)
        {
            ITF_WARNING(m_actor, 0, "Pet data do not match with default structure");
            return bfalse;
        }

        String8 subAnimName;
        ITF_VECTOR<u32> defaultAnims;

        for (ITF_VECTOR<String8>::const_iterator animIter = baseAnims.begin();
            animIter != baseAnims.end(); ++animIter)
        {
            ITF_VECTOR<u32> anims;
            anims.resize(basePartsSize);
            for (u32 partIdx=0; partIdx < basePartsSize; ++partIdx)
            {
                u32 animIdx = U32_INVALID;
                const AnimMeshVertexPetPart & petPart = _petData.m_parts[partIdx];
                if (petPart.m_variant >= 0)
                {
                    subAnimName.setTextFormat("%s_%s_%c", animIter->cStr(), baseParts[partIdx].cStr(), petPart.m_type);
                    animIdx = amnMeshVertex->getAnimIndexByFriendly(subAnimName);
                }
                anims[partIdx] = animIdx;
            }
            _petAnim.m_animNameToSubAnimIndexes[*animIter] = anims;
            if (defaultAnims.size() == 0)
                defaultAnims = anims;
        }

        u32 uvSize = amnMeshVertex->m_uvIndexToUVData.size()*ANIM_MESHVERTEX_POINTS;
        _petAnim.m_uvRedirect.resize(uvSize);
        for (u32 uvIdx=0; uvIdx<uvSize; uvIdx++)
            _petAnim.m_uvRedirect[uvIdx] = uvIdx;

        // change uv variant
        basePartsSize = defaultAnims.size();
        for (u32 partIdx=0; partIdx < basePartsSize; ++partIdx)
        {
            const AnimMeshVertexPetPart & petPart = _petData.m_parts[partIdx];
            if (petPart.m_variant <= 0)
                continue;

            u32 animIdx = defaultAnims[partIdx];
            if (animIdx >= amnMeshVertex->getNbAnims())
                continue;

            const FrameMeshInfo & firstFrame = amnMeshVertex->m_frameIndexToMeshDataByAnim[animIdx][0];
            for (u16 patchIdx = 0; patchIdx < firstFrame.m_nbPatchs; patchIdx++)
            {
                const PatchData & patch = amnMeshVertex->m_patchList[firstFrame.m_patchIndex + patchIdx];
                if (petPart.m_variant  < (i32)amnMeshVertex->m_uvIndexRedirect[patch.m_uvIndex].size())
                    _petAnim.m_uvRedirect[patch.m_uvIndex] = amnMeshVertex->m_uvIndexRedirect[patch.m_uvIndex][petPart.m_variant];
            }
        }

#ifdef USE_BODY_OFFSET        
        u32 bodyIdx = getTemplate()->getBodyIndex();
        if (bodyIdx == U32_INVALID)
        {
            _petAnim.m_animType = '\0';
        } else
        {
            _petAnim.m_animType = _petData.m_parts[bodyIdx].m_type;
        }
#endif

        u32 useFullElemets = 0;
        ITF_MAP<StringID, ITF_VECTOR<u32> >::const_iterator animIter = _petAnim.m_animNameToSubAnimIndexes.find(baseAnims[0]);
        if (animIter != _petAnim.m_animNameToSubAnimIndexes.end())
        {
            const ITF_VECTOR<u32> & lst = animIter->second;
            {
                for (ITF_VECTOR<u32>::const_iterator lstIter = lst.begin();
                    lstIter != lst.end(); ++lstIter)
                {
                    if (*lstIter != U32_INVALID)
                        useFullElemets++;
                }
                _animList.resize(useFullElemets);
#ifdef USE_BODY_OFFSET      
                _petAnim.m_partOffset.resize(useFullElemets);
#endif
            }
        }

        _petAnim.m_startIdx  = (u16)m_amvComponent->getAMVListSize();
        _petAnim.m_nbIdx     = (u8) useFullElemets;
        return btrue;
    }

    i32 AnimMeshVertexPetComponent::addPet( const AnimMeshVertexPetData & _petData )
    {
        SingleAnimDataRuntimeList   animList;
        AnimMeshVertexPetAnim       petAnim;
        const ITF_VECTOR<String8> & baseAnims = getTemplate()->getAnimList();

        if (!m_amvComponent || !fillPetListAdnPetAnim(_petData, animList, petAnim))
            return -1;

        for (SingleAnimDataRuntimeList::const_iterator anmIter = animList.begin();
            anmIter != animList.end(); ++anmIter)
            m_amvComponent->addToAMVList(*anmIter);
        m_animListData.push_back(petAnim);

        u32 animIdx = m_animListData.size()-1;
        setPetAnim(animIdx, baseAnims[0]);
        
        Transform3d tranform(_petData.m_position, _petData.m_angle, _petData.m_scale, _petData.m_flip);
        setPetTransform(animIdx, tranform);

        return (i32)animIdx;
    }

    bbool AnimMeshVertexPetComponent::removePet( i32 _idx )
    {
        if (!m_amvComponent || _idx >= (i32)m_animListData.size())
            return bfalse;

        AnimMeshVertexPetAnim & animData = m_animListData[_idx];
        u16                     startIdx = animData.m_startIdx;
        u16                     nbIdx    = animData.m_nbIdx;
        

        m_amvComponent->deleteAMVFromList(  startIdx, nbIdx);
        m_animListData.erase(m_animListData.begin() + _idx);

        for (ITF_VECTOR<AnimMeshVertexPetAnim>::iterator anmDataIter = m_animListData.begin();
            anmDataIter != m_animListData.end(); ++anmDataIter)
        {
            if (anmDataIter->m_startIdx > startIdx)
                anmDataIter->m_startIdx -= nbIdx;
        }

        return btrue;
    }

    bbool AnimMeshVertexPetComponent::updatePet( i32 _idx, const AnimMeshVertexPetData & _petData )
    {
        if (!AnimMeshVertexPetComponent::removePet( _idx ))
            return bfalse;

        SingleAnimDataRuntimeList   animList;
        AnimMeshVertexPetAnim       petAnim;
        const ITF_VECTOR<String8> & baseAnims = getTemplate()->getAnimList();

        if (!m_amvComponent || !fillPetListAdnPetAnim(_petData, animList, petAnim))
            return bfalse;

        for (SingleAnimDataRuntimeList::const_iterator anmIter = animList.begin();
            anmIter != animList.end(); ++anmIter)
            m_amvComponent->addToAMVList(*anmIter);
        m_animListData.insert(m_animListData.begin() + _idx, petAnim);

        setPetAnim(_idx, baseAnims[0]);

        Transform3d tranform(_petData.m_position, _petData.m_angle, _petData.m_scale, _petData.m_flip);
        setPetTransform(_idx, tranform);

        return btrue;

    }

    void AnimMeshVertexPetComponent::sortPets()
    {
        // Using bubble sort because the list is 
        SafeArray<pair < u16, u16> >  sortTab;
        u32             sortTabSize         = m_animListData.size();
        u32             sortTabSizeMinusOne = sortTabSize-1;

        if (!m_amvComponent || sortTabSize == 0)
            return;

        sortTab.resize(sortTabSize);
        ITF_VECTOR<AnimMeshVertexPetAnim>::const_iterator animIter = m_animListData.begin();
        for (u16 i=0; animIter != m_animListData.end(); ++animIter, ++i)
        {
            sortTab[i] = pair< u16, u16>(i, animIter->m_startIdx) ;
        }

        u32 nbSteps = 0;
        bbool swapDone = btrue;
        while (swapDone)
        {
            swapDone = bfalse;
            for (u32 i=0; i<sortTabSizeMinusOne; i++)
            {
                if (m_amvComponent->getAMVTransform(sortTab[i].second).m_z > m_amvComponent->getAMVTransform(sortTab[i+1].second).m_z)
                {
                    pair< u16, u16> swap= sortTab[i];
                    sortTab[i]          = sortTab[i+1];
                    sortTab[i+1]        = swap;

                    nbSteps++;
                    swapDone = btrue;
                }
            }
        }
        if (nbSteps == 0)
            return;

        SingleAnimDataRuntimeList newAnimListRuntimeList;
        newAnimListRuntimeList.reserve(m_amvComponent->getAMVListSize());

        u16 startIdx = 0;
        for (u32 i=0; i<sortTabSize; i++)
        {
            AnimMeshVertexPetAnim & petData     = m_animListData[sortTab[i].first];
            u32 sourceIdx                       = petData.m_startIdx;
            petData.m_startIdx                  = startIdx;
            for (u8 part=0; part<petData.m_nbIdx; part++, sourceIdx++)
            {
                newAnimListRuntimeList.push_back(m_amvComponent->getAMV(sourceIdx));
            }
            startIdx += petData.m_nbIdx;
        }
        m_amvComponent->swapAMVList(newAnimListRuntimeList);
    }

    void AnimMeshVertexPetComponent::fillRandomPetData(AnimMeshVertexPetData& _petData, bbool _allowEmpty)
    {
        static Seeder random(u32(uPtr(this)));

        AnimMeshVertex * amnMeshVertex = m_amvComponent->getAnimMeshVertex();

        const ITF_VECTOR<String8> & baseParts = getTemplate()->getBaseParts();
        const ITF_VECTOR<String8> & baseAnims = getTemplate()->getAnimList();

        u32 basePartsSize = baseParts.size();
        _petData.m_parts.resize(baseParts.size());

        // search number of anims by type/variant
        String8 subAnimName;
        for (u32 partIdx=0; partIdx < basePartsSize; ++partIdx)
        {
            AnimMeshVertexPetPart & petPart = _petData.m_parts[partIdx];
            SafeArray<u32>          nbPetVariantByTypes;

            u32     animIdx     = U32_INVALID;
            char    currentType = 'A';

            do
            {
                subAnimName.setTextFormat("%s_%s_%c", baseAnims[0].cStr(), baseParts[partIdx].cStr(), currentType);
                animIdx = amnMeshVertex->getAnimIndexByFriendly(subAnimName);

                if (animIdx != U32_INVALID)
                {
                    const FrameMeshInfo & firstFrame = amnMeshVertex->m_frameIndexToMeshDataByAnim[animIdx][0];
                    const PatchData & patch = amnMeshVertex->m_patchList[firstFrame.m_patchIndex];
                    if (amnMeshVertex->m_uvIndexRedirect[patch.m_uvIndex].size() > 0 )
                        nbPetVariantByTypes.push_back( amnMeshVertex->m_uvIndexRedirect[patch.m_uvIndex].size() );
                    else
                        nbPetVariantByTypes.push_back( 1 );
                    currentType++;
                }
            } while (animIdx != U32_INVALID);

            if (nbPetVariantByTypes.size())
            {
                petPart.m_type          = (char)(random.GetFloat(0.f, (f32)nbPetVariantByTypes.size()              - 1.f)  + 0.5f) + 'A';
                if (_allowEmpty)
                    petPart.m_variant   = (int)(random.GetFloat(0.f, (f32)nbPetVariantByTypes[petPart.m_type-'A'] + 1.f)) - 1;
                else
                    petPart.m_variant   = (int)(random.GetFloat(0.f, (f32)nbPetVariantByTypes[petPart.m_type-'A']      ));
            } else
            {
                petPart.m_type          = 'A';
                petPart.m_variant       = -1;
            }
        }
    }

    i32 AnimMeshVertexPetComponent::addRandomPet(const Vec3d & _pos)
    {
        AnimMeshVertexPetData petData;
        petData.m_position = _pos;

        fillRandomPetData(petData, bfalse);

        return addPet(petData);
    }

    const ITF_VECTOR<AnimMeshVertexPetData>& AnimMeshVertexPetComponent::getAvailablePetList() const
    {
        return getTemplate()->getAllPetList();
    }

    void AnimMeshVertexPetComponent::onResourceLoaded()
    {
        Super::onResourceLoaded();

        m_amvComponent = GetActor()->GetComponent<AnimMeshVertexComponent>();
        if (!m_amvComponent)
            return;

        const ITF_VECTOR<AnimMeshVertexPetData> & petList = getTemplate()->getPetList();
        u32 petListSize = petList.size();
        for (u32 i=0; i<petListSize; ++i)
        {
            addPet(petList[i]);
        }


        const ITF_VECTOR<String8> & animList = getTemplate()->getAnimList();
        const ITF_VECTOR<Vec3d> & randList = getTemplate()->getRandomPetPosList();
        u32 randListSize = randList.size();
        for (u32 i=0; i<randListSize; ++i)
        {
            i32 idx = addRandomPet(randList[i]);
            setPetAnim(idx, animList[i % animList.size()]);
        }

        sortPets();
        updateAllUvRedirect();
    }

    bbool AnimMeshVertexPetComponent::needsUpdate() const
    {
        return getTemplate()->getAllowUpdate();
    }


}
