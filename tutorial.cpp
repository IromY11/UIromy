



#include <SDL.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GLRenderer.h>

#include <iostream>



void initialiseResourceGroupDirectories();
void initialiseDefaultResourceGroups();
void main_loop();

using namespace CEGUI;
using namespace std;


int main(int argc, char *argv[])
{
	SDL_Surface * screen;
	atexit (SDL_Quit);
	SDL_Init (SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode (1024, 600, 0, SDL_OPENGL);
	if (screen == NULL) {
		/* Se ainda não der, desiste! */ 
		fprintf (stderr, "Impossível ajustar ao vídeo: %s\n", SDL_GetError ());
		exit (1);
	}
	SDL_WM_SetCaption("Iromy",NULL);
 
    CEGUI::OpenGLRenderer::bootstrapSystem();
	//SDL_ShowCursor (SDL_DISABLE);
	SDL_EnableUNICODE (1);
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);


	initialiseResourceGroupDirectories();
	initialiseDefaultResourceGroups();

	
	//cout<<WindowManager::getDefaultResourceGroup()<<endl;
	//cout<<System::getSingleton().getDefaultGUIContext().getDefaultFont()<<endl;
	
	SchemeManager::getSingleton().createFromFile( "vhdemo.scheme" );
	//FontManager::getSingleton().createFromFile( "vhdemo.font" );
	
 
	System::getSingleton().getDefaultGUIContext().setDefaultFont( "vhdemo" );
	//System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage( "TaharezLook/MouseArrow" );
	//System::getSingleton().getDefaultGUIContext().setDefaultTooltipType( "TaharezLook/Tooltip" );
 
	WindowManager& wmgr = WindowManager::getSingleton();
	Window* myRoot = wmgr.createWindow( "DefaultWindow", "root" );
	System::getSingleton().getDefaultGUIContext().setRootWindow( myRoot );
	
	Window *nw = WindowManager::getSingleton().loadLayoutFromFile("vhdemo.layout","layouts");
	
	
	myRoot->addChild(nw);

	Animation *anim = AnimationManager::getSingleton().createAnimation("walo");
	anim->setDuration(1.8f);
	anim->setReplayMode(Animation::RM_Loop);
	Affector* af1 = anim->createAffector("Position","UVector2");
	af1->setApplicationMethod(af1->AM_Relative);
	af1->createKeyFrame(0,"{{0,200},{0,0}}");
	af1->createKeyFrame(0.9,"{{0,470},{0,0}}",KeyFrame::P_Linear);
	af1->createKeyFrame(1.8,"{{0,200},{0,0}}",KeyFrame::P_Linear);
	
	Animation *anim2 = AnimationManager::getSingleton().createAnimation("walo2");
	anim2->setDuration(1.8f);
	anim2->setReplayMode(Animation::RM_Loop);
	Affector* af2 = anim2->createAffector("Position","UVector2");
	af2->setApplicationMethod(af1->AM_Relative);
	af2->createKeyFrame(0,"{{0,200},{0,0}}");
	af2->createKeyFrame(0.6,"{{0,470},{0,0}}",KeyFrame::P_Linear);
	af2->createKeyFrame(1.8,"{{0,200},{0,0}}",KeyFrame::P_Linear);


	Animation *anim3 = AnimationManager::getSingleton().createAnimation("walo3");
	anim3->setDuration(0.3f);
	anim3->setReplayMode(Animation::RM_Once);
	Affector* af3 = anim3->createAffector("Position","UVector2");
	af3->setApplicationMethod(af1->AM_Absolute);
	af3->createKeyFrame(0,"{{0,15},{0,280}}");
	af3->createKeyFrame(0.3,"{{0,95},{0,280}}",KeyFrame::P_Linear);
	anim3->defineAutoSubscription("MouseEntersArea","Start");

	Animation *anim4 = AnimationManager::getSingleton().createAnimation("walo4");
	anim4->setDuration(0.3f);
	anim4->setReplayMode(Animation::RM_Once);
	Affector* af4 = anim4->createAffector("Position","UVector2");
	af4->setApplicationMethod(af1->AM_Absolute);
	af4->createKeyFrame(0,"{{0,95},{0,280}}");
	af4->createKeyFrame(0.3,"{{0,15},{0,280}}",KeyFrame::P_Linear);
	anim4->defineAutoSubscription("MouseLeavesArea","Start");

	Animation *anim5 = AnimationManager::getSingleton().createAnimation("walo5");
	anim5->setDuration(0.3f);
	anim5->setReplayMode(Animation::RM_Once);
	Affector* af5 = anim5->createAffector("Position","UVector2");
	af5->setApplicationMethod(af1->AM_Absolute);
	af5->createKeyFrame(0,"{{0,15},{0,325}}");
	af5->createKeyFrame(0.3,"{{0,95},{0,325}}",KeyFrame::P_Linear);
	anim5->defineAutoSubscription("MouseEntersArea","Start");

	Animation *anim6 = AnimationManager::getSingleton().createAnimation("walo6");
	anim6->setDuration(0.3f);
	anim6->setReplayMode(Animation::RM_Once);
	Affector* af6 = anim6->createAffector("Position","UVector2");
	af6->setApplicationMethod(af1->AM_Absolute);
	af6->createKeyFrame(0,"{{0,95},{0,325}}");
	af6->createKeyFrame(0.3,"{{0,15},{0,325}}",KeyFrame::P_Linear);
	anim6->defineAutoSubscription("MouseLeavesArea","Start");

	Animation *anim7 = AnimationManager::getSingleton().createAnimation("walo7");
	anim7->setDuration(0.3f);
	anim7->setReplayMode(Animation::RM_Once);
	Affector* af7 = anim7->createAffector("Position","UVector2");
	af7->setApplicationMethod(af1->AM_Absolute);
	af7->createKeyFrame(0,"{{0,15},{0,370}}");
	af7->createKeyFrame(0.3,"{{0,95},{0,370}}",KeyFrame::P_Linear);
	anim7->defineAutoSubscription("MouseEntersArea","Start");

	Animation *anim8 = AnimationManager::getSingleton().createAnimation("walo8");
	anim8->setDuration(0.3f);
	anim8->setReplayMode(Animation::RM_Once);
	Affector* af8 = anim8->createAffector("Position","UVector2");
	af8->setApplicationMethod(af1->AM_Absolute);
	af8->createKeyFrame(0,"{{0,95},{0,370}}");
	af8->createKeyFrame(0.3,"{{0,15},{0,370}}",KeyFrame::P_Linear);
	anim8->defineAutoSubscription("MouseLeavesArea","Start");

	Animation *anim9 = AnimationManager::getSingleton().createAnimation("walo9");
	anim9->setDuration(0.3f);
	anim9->setReplayMode(Animation::RM_Once);
	Affector* af9 = anim9->createAffector("Position","UVector2");
	af9->setApplicationMethod(af1->AM_Absolute);
	af9->createKeyFrame(0,"{{0,15},{0,415}}");
	af9->createKeyFrame(0.3,"{{0,95},{0,415}}",KeyFrame::P_Linear);
	anim9->defineAutoSubscription("MouseEntersArea","Start");

	Animation *anim10 = AnimationManager::getSingleton().createAnimation("walo10");
	anim10->setDuration(0.3f);
	anim10->setReplayMode(Animation::RM_Once);
	Affector* af10 = anim10->createAffector("Position","UVector2");
	af10->setApplicationMethod(af1->AM_Absolute);
	af10->createKeyFrame(0,"{{0,95},{0,415}}");
	af10->createKeyFrame(0.3,"{{0,15},{0,415}}",KeyFrame::P_Linear);
	anim10->defineAutoSubscription("MouseLeavesArea","Start");

	/*Affector* af2 = anim->createAffector("Alpha","float");
	af2->createKeyFrame(0,"0.0");
	af2->createKeyFrame(0.8,"0.1",KeyFrame::P_Linear);*/
	
	
	Window *bg = myRoot->getChild("Image")->getChild("newgame");
	Window *bg2 = myRoot->getChild("Image")->getChild("newgame2");
	Window *bg3 = myRoot->getChild("Image")->getChild("newgame3");
	Window *bg4 = myRoot->getChild("Image")->getChild("newgame4");

	AnimationManager& v = AnimationManager::getSingleton();

	AnimationInstance* inst = v.instantiateAnimation(anim);
	AnimationInstance* inst2 = v.instantiateAnimation(anim2);
	AnimationInstance* inst3 = v.instantiateAnimation(anim);
	AnimationInstance* inst4 = v.instantiateAnimation(anim2);
	AnimationInstance* inst5 = v.instantiateAnimation(anim3);
	AnimationInstance* inst6 = v.instantiateAnimation(anim4);
	AnimationInstance* inst7 = v.instantiateAnimation(anim5);
	AnimationInstance* inst8 = v.instantiateAnimation(anim6);
	AnimationInstance* inst9 = v.instantiateAnimation(anim7);
	AnimationInstance* inst10 = v.instantiateAnimation(anim8);
	AnimationInstance* inst11 = v.instantiateAnimation(anim9);
	AnimationInstance* inst12 = v.instantiateAnimation(anim10);
	inst5->setTargetWindow(bg);
	inst6->setTargetWindow(bg);
	inst7->setTargetWindow(bg2);
	inst8->setTargetWindow(bg2);
	inst9->setTargetWindow(bg3);
	inst10->setTargetWindow(bg3);
	inst11->setTargetWindow(bg4);
	inst12->setTargetWindow(bg4);
	/*inst->setTargetWindow(bg); inst->start();
	inst2->setTargetWindow(bg2); inst2->start();
	inst3->setTargetWindow(bg3); inst3->start();
	inst4->setTargetWindow(bg4); inst4->start();*/
	
	//bg->setAlpha(0.4f);

	
 
	main_loop();
}