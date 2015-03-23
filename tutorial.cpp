



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
	
	//Window *newWindow = WindowManager::getSingleton().loadLayoutFromFile("./datafiles/layouts/ChatWindow.layout","second_");
	Window *nw = WindowManager::getSingleton().loadLayoutFromFile("vhdemo.layout","layouts");
	//Window *nz = WindowManager::getSingleton().loadLayoutFromFile("ChatWindow.layout","layouts");
	
	myRoot->addChild(nw);
	//myRoot->addChild(nz);
	
	/*GameConsoleWindow gc("stoon"),dd("non");
	gc.setVisible(true);
	dd.setVisible(true);

	//CEGUI::ButtonBase b(,"jj");b.setSize(
	
	FrameWindow* fWnd = static_cast<FrameWindow*>(
						      wmgr.createWindow( "TaharezLook/FrameWindow", "testWindow" ));
	myRoot->addChild( fWnd );
	
	// position a quarter of the way in from the top-left of parent.
	fWnd->setPosition( UVector2( UDim( 0.25f, 0.0f ), UDim( 0.25f, 0.0f ) ) );
	fWnd->setVisible(true);
	Quaternion qs = Quaternion::eulerAnglesDegrees(0,0,15);
	//fWnd->setRotation(qs);
	// set size to be half the size of the parent
	fWnd->setSize( USize( UDim( 0.5f, 0.0f ), UDim( 0.5f, 0.0f ) ) );
	fWnd->setText( "Hello Iromy!!" );*/

	
 
	main_loop();
}