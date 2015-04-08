#include "precompiled_engine.h"

#ifndef _ITF_RANDOMANIMBANKCHANGECOMPONENT_H_
#include "engine/actors/components/RandomAnimBankChangeComponent.h"
#endif //_ITF_RANDOMANIMBANKCHANGECOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{	
	//---------------------------------------------------------------------------------------------------
	BEGIN_SERIALIZATION_SUBCLASS(RandomAnimBankChangeComponent_Template,tplRandomPatchNames)
		SERIALIZE_MEMBER("Name", m_name);
		SERIALIZE_MEMBER("Percent", m_percent);
	END_SERIALIZATION()

	//---------------------------------------------------------------------------------------------------
	IMPLEMENT_OBJECT_RTTI(RandomAnimBankChangeComponent_Template)
		
	BEGIN_SERIALIZATION_CHILD(RandomAnimBankChangeComponent_Template)
		SERIALIZE_CONTAINER_OBJECT("PatchNameCanBeRemoved", m_patchNamesCanBeRemoved);
	END_SERIALIZATION()

	RandomAnimBankChangeComponent_Template::RandomAnimBankChangeComponent_Template() : Super()
	{
	}

	RandomAnimBankChangeComponent_Template::~RandomAnimBankChangeComponent_Template()
	{
	}

	void RandomAnimBankChangeComponent_Template::getRandomPatchNameToRemoved(ITF_VECTOR<strRandomPatchName> &_out) const 
	{ 
		_out.clear();
		const u32 mySize = m_patchNamesCanBeRemoved.size();
		if( mySize > 0 )
		{
			for ( u32 u = 0; u < mySize; ++u )
			{
				const f32 percent = Seeder::getSharedSeeder().GetFloat(0.0f, 100.0f);
				if(percent <= m_patchNamesCanBeRemoved[u].m_percent)
				{
					_out.push_back(strRandomPatchName(m_patchNamesCanBeRemoved[u].m_name, this));
				}
			}
		}
	}


	//---------------------------------------------------------------------------------------------------
	BEGIN_SERIALIZATION(strRandomPatchName)
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
		BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
			if(m_this != NULL)
			{
				u32 stringIDToEnum = U32_INVALID;
				const ITF_VECTOR<RandomAnimBankChangeComponent_Template::tplRandomPatchNames> &nameList = m_this->getPatchNamesCanBeRemoved();
				BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit_Save)
					for ( u32 u = 0; u < nameList.size(); ++u )
					{
						if(nameList[u].m_name == m_name)
						{
							stringIDToEnum = u;
							break;
						}
					}
				END_CONDITION_BLOCK()
				SERIALIZE_ENUM_BEGIN("PatchName", stringIDToEnum)
					for ( u32 u = 0; u < nameList.size(); ++u )
					{
						SERIALIZE_ENUM_NAME_VAR(nameList[u].m_name.getDebugString(), u);
					}
					SERIALIZE_ENUM_NAME_VAR("none", U32_INVALID);
				SERIALIZE_ENUM_END()
				BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit_Load)
					m_name = stringIDToEnum < nameList.size() ? nameList[stringIDToEnum].m_name : StringID::Invalid;
				END_CONDITION_BLOCK()
			}
			else
			{
				SERIALIZE_MEMBER("PatchName", m_name);
			}
		}
		else
		{
			SERIALIZE_MEMBER("PatchName", m_name);
		END_CONDITION_BLOCK()
#else															
		SERIALIZE_MEMBER("PatchName", m_name);
#endif
	END_SERIALIZATION()

	//---------------------------------------------------------------------------------------------------
	BEGIN_SERIALIZATION_SUBCLASS(RandomAnimBankChangeComponent,strRandomAnimBankPart)
		SERIALIZE_MEMBER("SRC", m_src)
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
		BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
			const SubAnimSet* subAnimSet = m_animComponent != NULL ? m_animComponent->getSubAnimSet() : NULL;
			if (subAnimSet != NULL)
			{
				StringID idResult;
				u32 stringIDToEnum = U32_INVALID;
				BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit_Save)
					stringIDToEnum = subAnimSet->getTemplate()->getBankIdChangeIndex(m_name);
				END_CONDITION_BLOCK()
				SERIALIZE_ENUM_BEGIN("BankChange", stringIDToEnum)
					const ITF_VECTOR<BankIdChange> &bankList = subAnimSet->getTemplate()->getBankIdChangeList();
					for ( NamesMap::Iterator it(subAnimSet->getTemplate()->getNameToBankIdChange()); !it.isEnd(); it.increment() )
					{
						const u32 index = u32(it.second());
						const BankIdChange bankIdChange = bankList[index];
						if( bankIdChange.isSameSrc(m_src) )
						{
							const StringID bankName(u32(it.first()));
							SERIALIZE_ENUM_NAME_VAR(bankName.getDebugString(), index);
							if( index == stringIDToEnum )
								idResult = bankName;
						}
					}
					SERIALIZE_ENUM_NAME_VAR("_default", U32_INVALID);
				SERIALIZE_ENUM_END()
				BEGIN_CONDITION_BLOCK(ESerialize_PropertyEdit_Load)
					m_name = idResult;
				END_CONDITION_BLOCK()
			}
			else
			{
				SERIALIZE_MEMBER("BankChange", m_name);
			}
		}
		else
		{
			SERIALIZE_MEMBER("BankChange", m_name);
		END_CONDITION_BLOCK()
#else															
		SERIALIZE_MEMBER("BankChange", m_name);
#endif
	END_SERIALIZATION()


	//---------------------------------------------------------------------------------------------------
	IMPLEMENT_OBJECT_RTTI(RandomAnimBankChangeComponent)

	BEGIN_SERIALIZATION_CHILD(RandomAnimBankChangeComponent)
		BEGIN_CONDITION_BLOCK_NOT(ESerializeGroup_PropertyEdit)
			SERIALIZE_BOOL("isAlreadyCompute",m_alreadyCompute);
			SERIALIZE_MEMBER("NbItem", m_nbItem);
		END_CONDITION_BLOCK()
		SERIALIZE_MEMBER("ForceRecompute", m_forceRecompute);
		SERIALIZE_MEMBER("RecomputeOnceOnDuplicate", m_recomputeOnDuplicate);
		ITF_ASSERT( m_nbItem <= NB_BANK_MAX );
		for (u32 u = 0; u < m_nbItem && u < NB_BANK_MAX; ++u)
		{
			String8 temp;
			temp.setTextFormat("RandomAnimBank_%d", u);
			SERIALIZE_OBJECT(temp.cStr(), m_randomAnimBank[u]);
		}
		SERIALIZE_CONTAINER_OBJECT("PatchNamesToRemove", m_patchNamesToRemove);
		SERIALIZE_FUNCTION(onPropertyChanged, ESerialize_PropertyEdit_Load);
	END_SERIALIZATION()

	BEGIN_VALIDATE_COMPONENT(RandomAnimBankChangeComponent)
		VALIDATE_COMPONENT_PARAM("AnimLightComponent", m_animComponent, "AnimLightComponent mandatory");
	END_VALIDATE_COMPONENT()

	void RandomAnimBankChangeComponent::onPropertyChanged()
	{
		refreshAnimBankChanges();
	}

	RandomAnimBankChangeComponent::RandomAnimBankChangeComponent() : Super()
		, m_animComponent(NULL)
		, m_forceRecompute(bfalse)
		, m_alreadyCompute(bfalse)
		, m_recomputeOnDuplicate(btrue)
		, m_nbItem(0)
	{
	}

	RandomAnimBankChangeComponent::~RandomAnimBankChangeComponent()
	{
	}

	void RandomAnimBankChangeComponent::onFinalizeLoad()
	{
		Super::onFinalizeLoad();

		m_animComponent = GetActor()->GetComponent<AnimLightComponent>();

		for (u32 u = 0; u < NB_BANK_MAX; ++u)
		{
			m_randomAnimBank[u].m_animComponent = m_animComponent;
		}
	}

	void RandomAnimBankChangeComponent::tryToComputeRand()
	{
		if( (m_animComponent != NULL) && (!m_alreadyCompute || m_forceRecompute) )
		{
			m_alreadyCompute = btrue;
			m_nbItem = 0;
			const SubAnimSet* subAnimSet = m_animComponent->getSubAnimSet();
			if(subAnimSet != NULL)
			{
				// 1 - find all different possible bank src
				ITF_MAP<StringID, ITF_VECTOR<BankIdChange> > allBankSrc;
				const ITF_VECTOR<BankIdChange> bankList = subAnimSet->getTemplate()->getBankIdChangeList();
				for ( ITF_VECTOR<BankIdChange>::const_iterator it = bankList.begin(); it != bankList.end(); ++it )
				{
					const BankIdChange bankIdChange = *it;
					const StringID mySrc = bankIdChange.getFirstIdRedirect();
					if(mySrc.isValid())
					{
						if( allBankSrc.find(mySrc) != allBankSrc.end() )
						{
							allBankSrc[mySrc].push_back(bankIdChange);
						}
						else
						{
							ITF_VECTOR<BankIdChange>  bankList;
							bankList.push_back(bankIdChange);
							allBankSrc[mySrc] = bankList;
						}
					}
				}
				// 2 - add random on each
				for ( ITF_MAP<StringID, ITF_VECTOR<BankIdChange> >::const_iterator it = allBankSrc.begin(); it != allBankSrc.end(); ++it )
				{
					u32 size = it->second.size();
					if(size > 0)
					{
						if( m_nbItem < NB_BANK_MAX )
						{
							u32 index = Seeder::getSharedSeeder().GetUnseededU32(0, size + 1);
							if( index >= size )
								m_randomAnimBank[m_nbItem] = strRandomAnimBankPart(it->second[0].getFirstIdRedirect(), StringID::Invalid, m_animComponent);	// special case of default item
							else
								m_randomAnimBank[m_nbItem] = strRandomAnimBankPart(it->second[index].getFirstIdRedirect(), it->second[index].getName(), m_animComponent);
							m_nbItem ++;
						}
						else
						{
							ITF_WARNING(m_actor, bfalse, "YOU SHOULD ADD MORE RANDOM ANIM BANK VARS");
						}
					}
				}
				// 3 - add random on patch
				getTemplate()->getRandomPatchNameToRemoved(m_patchNamesToRemove);
			}
		}
	}

	void RandomAnimBankChangeComponent::onBecomeActive()
	{
		Super::onBecomeActive();

		tryToComputeRand();
		refreshAnimBankChanges();
	}

	void RandomAnimBankChangeComponent::refreshAnimBankChanges()
	{
		if(m_nbItem > 0)
		{
			SubAnimSet* subAnimSet = m_animComponent->getSubAnimSet();
			if(subAnimSet != NULL)
			{
				subAnimSet->clearBankIdChange();
				for ( u32 u = 0; u < m_nbItem && u < NB_BANK_MAX; ++u)
				{
					StringID bankName = m_randomAnimBank[u].m_name;
					if( bankName.isValid() )
						subAnimSet->addBankIdChange(bankName);
				}
			}
		}
		m_animComponent->clearPatchChange();
		ITF_VECTOR<strRandomPatchName>::iterator it = m_patchNamesToRemove.begin();
		while ( it != m_patchNamesToRemove.end() )
		{
			if(it->m_name.isValid() && getTemplate()->getPatchNamesCanBeRemoved().find( RandomAnimBankChangeComponent_Template::tplRandomPatchNames(it->m_name) ) == -1)
			{
				it = m_patchNamesToRemove.erase(it);
			}
			else
			{
				it->m_this = getTemplate();
				m_animComponent->setPatchChange(it->m_name, StringID::Invalid);
				it++;
			}
		}
	}

#ifdef ITF_SUPPORT_EDITOR
	void RandomAnimBankChangeComponent::onEditorCreated( class Actor* _original )
	{
		Super::onEditorCreated(_original);

		if(m_recomputeOnDuplicate)
		{
			m_alreadyCompute = bfalse;
		}
	}
#endif // ITF_SUPPORT_EDITOR

}
