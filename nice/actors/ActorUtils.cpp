#include "precompiled_engine.h"

#ifndef _ITF_ACTORUTILS_H_
#include "engine/Actors/ActorUtils.h"
#endif //_ITF_ACTORUTILS_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/Components/UI/UITextBox.h"
#endif // _ITF_UITEXTBOX_H_

#ifndef _ITF_TEXTBOXCOMPONENT_H_
#include "gameplay/components/UI/TextBoxComponent.h"
#endif //_ITF_TEXTBOXCOMPONENT_H_

#ifndef _ITF_W1W_COMICSTEXTBOXCOMPONENT_H_
#include "W1W/HUD/W1W_ComicsTextBoxComponent.h"
#endif //_ITF_W1W_COMICSTEXTBOXCOMPONENT_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

namespace ITF
{
	void getPickableMatrix(const Pickable* _pickable, Matrix44 &_matrix)
	{
		_matrix.setIdentity();
        _matrix.setRotationZ(_pickable->getAngle());
        _matrix.setTranslation(_pickable->getPos());

        Vec3d scale( _pickable->getScale().to3d(1.f));
		if (_pickable->getIsFlipped())
			scale.x() *= -1.f;
        _matrix.mulScale(scale);
	}

    void groupResetTransformationToInitial(Pickable* _pickable)
    {
        _pickable->resetTransformationToInitial();

        if (const SubSceneActor *ssa = DYNAMIC_CAST(_pickable,SubSceneActor) )
        {
            if (Scene* subScene = ssa->getSubScene())
            {
                const ITF_VECTOR<Pickable*>& pickables = subScene->getPickableList();
                for(u32 j = 0; j < pickables.size(); ++j)
                {
                    groupResetTransformationToInitial(pickables[j]);
                }
            }
        }
    }

    void setColorAndAlpha(const Actor* _actor, bbool _setColor, const Color &_color, bbool _setAlpha, f32 _alpha)
    {
        GraphicComponent * graphComp = _actor->GetComponent<GraphicComponent>();            
        if (graphComp)
        {
            if(_setAlpha)
            {        
                graphComp->setAlpha(_alpha);
            }

            if(_setColor)
            {
                GFXPrimitiveParam primitiveParam = graphComp->getGfxPrimitiveParam();
                primitiveParam.m_colorFactor = _color;
                graphComp->setGFXPrimitiveParam(primitiveParam);
            } 
        }

        UITextBox * textBox = _actor->GetComponent<UITextBox>(); 
        if(textBox)
        {
            if(_setAlpha)
                textBox->setAlpha(_alpha);

            if(_setColor)
                textBox->setColor(_color);
        }

        TextBoxComponent *textBox2  = _actor->GetComponent<TextBoxComponent>(); 
        if(textBox2)
        {
            if(_setAlpha)
                textBox2->setAlpha(_alpha);

            if(_setColor)
                textBox2->setColor(_color);
		}

		W1W_ComicsTextBoxComponent *textBox3  = _actor->GetComponent<W1W_ComicsTextBoxComponent>(); 
		if(textBox3)
		{
			if(_setAlpha)
				textBox3->setAlpha(_alpha);

			if(_setColor)
				textBox3->setColor(_color);
		}

        // Set color and alpha inside groups too
        if (const SubSceneActor *ssa = DYNAMIC_CAST(_actor,SubSceneActor) )
        {
            if (Scene* subScene = ssa->getSubScene())
            {
                const ITF_VECTOR <Actor*>& actors = subScene->getActors();
                const u32 actorCount = actors.size();
                for(u32 j = 0; j < actorCount; ++j)
                {                        
                    setColorAndAlpha(actors[j], _setColor, _color, _setAlpha, _alpha);
                }
                const ITF_VECTOR <Frise*>& frises = subScene->getFrises();
                const u32 frisesCount = frises.size();
                for(u32 j = 0; j < frisesCount; ++j)
                {                    
                    Frise* frise = frises[j];

                    if(_setAlpha)
                        frise->setAlpha(_alpha);

                    if(_setColor)
                        frise->setColorFactorRunTime(_color);
                }
            }
        }

    }
}
