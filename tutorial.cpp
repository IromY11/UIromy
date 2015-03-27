



#include <SDL.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GLRenderer.h>

#include <iostream>



void initialiseResourceGroupDirectories();
void initialiseDefaultResourceGroups();
void main_loop();

using namespace CEGUI;
using namespace std;

class GameConsoleWindow
{
    public:
       GameConsoleWindow(string s);                   // Constructor
       void setVisible(bool visible);         // Hide or show the console
       bool isVisible();                      // return true if console is visible, false if is hidden
	CEGUI::Window *m_ConsoleWindow;
    private:
       void CreateCEGUIWindow();                                  // The function which will load in the CEGUI Window and register event handlers
       void RegisterHandlers();                                   // Register our handler functions
       bool Handle_TextSubmitted(const CEGUI::EventArgs &e);      // Handle when we press Enter after typing
       bool Handle_SendButtonPressed(const CEGUI::EventArgs &e);  // Handle when we press the Send button         
       void ParseText(CEGUI::String inMsg);                       // Parse the text the user submitted.
       void OutputText(CEGUI::String inMsg,                       // Post the message to the ChatHistory listbox.
              CEGUI::Colour colour = CEGUI::Colour( 0xFFFFFFFF)); //   with a white color default
 
                                   // This will be a pointer to the ConsoleRoot window.
       CEGUI::String sNamePrefix;                                  // This will be the prefix name we give the layout
       static int iInstanceNumber;                                 // This will be the instance number for this class.
       bool m_bConsole;
};

 
GameConsoleWindow::GameConsoleWindow(string s)
{
   m_ConsoleWindow = NULL;   
   sNamePrefix = "";
   CreateCEGUIWindow();
   m_ConsoleWindow->setName(s);
   setVisible(false);
   m_bConsole = false;
}

void GameConsoleWindow::CreateCEGUIWindow()
{
        //sNamePrefix = ++iInstanceNumber + "_";
        m_ConsoleWindow = WindowManager::getSingleton().loadLayoutFromFile("ChatWindow.layout","layouts");
        if (m_ConsoleWindow)
        {
            CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->addChild(m_ConsoleWindow);
            (this)->RegisterHandlers();
        }
        else CEGUI::Logger::getSingleton().logEvent("Error: Unable to load the ConsoleWindow from .layout");
}

void GameConsoleWindow::RegisterHandlers()
{
    m_ConsoleWindow->getChild(sNamePrefix + "Submit")->subscribeEvent(
                        PushButton::EventClicked,   
                        Event::Subscriber(&GameConsoleWindow::Handle_SendButtonPressed,this));    
    m_ConsoleWindow->getChild(sNamePrefix + "EditBox")->subscribeEvent(Editbox::EventTextAccepted,
                        Event::Subscriber(&GameConsoleWindow::Handle_TextSubmitted,this));
}
bool GameConsoleWindow::Handle_TextSubmitted(const CEGUI::EventArgs &e)
{
    const CEGUI::WindowEventArgs* args = static_cast<const CEGUI::WindowEventArgs*>(&e);
    CEGUI::String Msg = m_ConsoleWindow->getChild(sNamePrefix + "EditBox")->getText();
    (this)->ParseText(Msg);
    m_ConsoleWindow->getChild(sNamePrefix + "EditBox")->setText("");
    return true;
}
bool GameConsoleWindow::Handle_SendButtonPressed(const CEGUI::EventArgs &e)
{
    CEGUI::String Msg = m_ConsoleWindow->getChild(sNamePrefix + "EditBox")->getText();
    (this)->ParseText(Msg);
    m_ConsoleWindow->getChild(sNamePrefix + "EditBox")->setText("");
    return true;
}
void GameConsoleWindow::ParseText(CEGUI::String inMsg)
{
 
       std::string inString = inMsg.c_str();
 
	if (inString.length() >= 1) 
	{
		if (inString.at(0) == '/') 
		{
			std::string::size_type commandEnd = inString.find(" ", 1);
			std::string command = inString.substr(1, commandEnd - 1);
			std::string commandArgs = inString.substr(commandEnd + 1, inString.length() - (commandEnd + 1));
			//convert command to lower case
			for(std::string::size_type i=0; i < command.length(); i++)
			{
				command[i] = tolower(command[i]);
			}
 
			// Begin processing
 
			if (command == "say")
			{
				std::string outString = "You:" + inString; // Append our 'name' to the message we'll display in the list
                         	OutputText(outString);
			}
			else if (command == "quit")
			{
				// do a /quit 
			}
			else if (command == "help")
			{
				// do a /help
			}
			else
			{
				std::string outString = "<" + inString + "> is an invalid command.";
				(this)->OutputText(outString,CEGUI::Colour(1.0f,0.0f,0.0f)); // With red ANGRY colors!
			}
		} // End if /
		else
		{
	 (this)->OutputText(inString); // no commands, just output what they wrote
		}
	} 
}
void GameConsoleWindow::OutputText(CEGUI::String inMsg, CEGUI::Colour colour)
{
 
	// Get a pointer to the ChatBox so we don't have to use this ugly getChild function everytime.
	CEGUI::Listbox *outputWindow = static_cast<CEGUI::Listbox*>(m_ConsoleWindow->getChild(sNamePrefix + "History"));
 
	CEGUI::ListboxTextItem* newItem=0; // This will hold the actual text and will be the listbox segment / item
 
	newItem = new CEGUI::ListboxTextItem(inMsg); // instance new item
        newItem->setTextColours(colour); // Set the text color
	outputWindow->addItem(newItem); // Add the new ListBoxTextItem to the ListBox
}
void GameConsoleWindow::setVisible(bool visible)
{
    m_ConsoleWindow->setVisible(visible);
    m_bConsole = visible;
 
	CEGUI::Editbox* editBox =static_cast<Editbox*> ( m_ConsoleWindow->getChild(sNamePrefix + "EditBox"));
    if(visible)
       editBox->activate();
    else
       editBox->deactivate();
}
 
bool GameConsoleWindow::isVisible()
{
    return m_ConsoleWindow->isVisible();
}
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

	/*Affector* af2 = anim->createAffector("Alpha","float");
	af2->createKeyFrame(0,"0.0");
	af2->createKeyFrame(0.8,"0.1",KeyFrame::P_Linear);*/
	
	
	Window *bg = myRoot->getChild("Image")->getChild("newgame");
	Window *bg2 = myRoot->getChild("Image")->getChild("newgame2");
	Window *bg3 = myRoot->getChild("Image")->getChild("newgame3");
	Window *bg4 = myRoot->getChild("Image")->getChild("newgame4");

	AnimationInstance* inst = CEGUI::AnimationManager::getSingleton().instantiateAnimation(anim);
	AnimationInstance* inst2 = CEGUI::AnimationManager::getSingleton().instantiateAnimation(anim2);
	AnimationInstance* inst3 = CEGUI::AnimationManager::getSingleton().instantiateAnimation(anim);
	AnimationInstance* inst4 = CEGUI::AnimationManager::getSingleton().instantiateAnimation(anim2);
	inst->setTargetWindow(bg); inst->start();
	inst2->setTargetWindow(bg2); inst2->start();
	inst3->setTargetWindow(bg3); inst3->start();
	inst4->setTargetWindow(bg4); inst4->start();
	
	//bg->setAlpha(0.4f);

	
 
	main_loop();
}