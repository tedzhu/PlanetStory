#include "stdafx.h"

UIManager::UIManager(void)
:mRootWindow(NULL)
,mMainMenuUI(NULL)
,mSelectRaceUI(NULL)
,mPlayUI(NULL)
,mGameStart(false)
,mSceneMgr(NULL)
,mWorld(NULL)
,mQuitGame(false)
,mCancelGame(false)
,mRaceType(0)
{
}

UIManager::~UIManager(void)
{

}

//Set up
bool UIManager::SetupUI(Ogre::SceneManager* scmgr,LevelManager* lvmgr,CWorldMap* world)
{
	mSceneMgr=scmgr;
	mWorld=world;
	mLvMgr=lvmgr;
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	CEGUI::SchemeManager::getSingleton().create("Planet.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("MainMenu", "Mouse");

	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	mRootWindow = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");
	CEGUI::System::getSingleton().setGUISheet(mRootWindow);
	return true;
}

void UIManager::Update()
{
	using namespace CEGUI;
	if (mLvMgr->GetGameState()==0 || mLvMgr->GetGameState()==2)//如果未结束或者播放每关结束动画
	{
		return;
	}
	else 
	{


		if (mLvMgr->GetGameState()>0 )//如果赢
		{
			if (mLvMgr->IsFinished()==false)//未通关
			{
				PlaySound("sound\\pass.wav", NULL, SND_FILENAME | SND_ASYNC);
				//隐藏Menu、Cancel按钮
				mPlayUI->setVisible(false);
				//添加Next按钮
				CEGUI::Window *NextButton =CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"Next.layout");
				NextButton->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_Next,this));
				mRootWindow->addChildWindow(NextButton);

				mWorld->WinAnim();
				mLvMgr->SetGameState(2);//正在播放动画
			}
			else//通关
			{
				PlaySound("sound\\win.wav", NULL, SND_FILENAME | SND_ASYNC);
				//删除Menu、Cancel按钮
				mPlayUI->destroy();
				//congratulations
				//CEGUI::Window *ConButton =CEGUI::WindowManager::getSingleton().createWindow("Vanilla/Button", "ConButton");
				//ConButton->setText("Congratulations!");
				//ConButton->setSize(CEGUI::UVector2(CEGUI::UDim(0.15f, 0), CEGUI::UDim(0.05f, 0)));
				//ConButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4f,0),CEGUI::UDim(0.4f,0)));
				//ConButton->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_Cong,this));
				//mRootWindow->addChildWindow(ConButton);

				//mWorld->CongAnim();
				CEGUI::Window *EndWindow=CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"End.layout");
				mRootWindow->addChildWindow(EndWindow);
				if (mRaceType=='1')
				{
					EndWindow=EndWindow->getChild("End/CleanEnd");					
				}
				else if (mRaceType=='2')
				{
					EndWindow=EndWindow->getChild("End/GreenhouseEnd");
				}
				else if (mRaceType=='3')
				{
					EndWindow=EndWindow->getChild("End/DustEnd");
				}
				EndWindow->setVisible(true);
				EndWindow->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_Cong,this));
				mLvMgr->SetGameState(0);
				mWorld->ClearScene();
			}
		}
		else if (mLvMgr->GetGameState()<0)//如果输
		{
			PlaySound("sound\\lose.wav", NULL, SND_FILENAME | SND_ASYNC);
			//隐藏Menu、Cancel按钮
			mPlayUI->setVisible(false);

			CEGUI::Window *MenuRetryWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"MenuRetry.layout");
			mRootWindow->addChildWindow(MenuRetryWindow);

			CEGUI::Window *childwindow=MenuRetryWindow->getChild("MenuRetry/RetryButton");
			childwindow->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_Retry,this));
			

			childwindow=MenuRetryWindow->getChild("MenuRetry/MenuButton");
			childwindow->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_Menu,this));

			//mWorld->ClearScene();
			mWorld->LoseAnim();
			mLvMgr->SetGameState(2);//正在播放动画
		}

	}
}
//Register Handlers
void UIManager::RegisterMainMenuHandlers()
{
	using namespace CEGUI;
	mMainMenuUI->getChild("MainMenu/StartButton")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_StartGame,this));
	mMainMenuUI->getChild("MainMenu/CreditButton")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_Credits,this));
	mMainMenuUI->getChild("MainMenu/QuitButton")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_Quit,this));

}
void UIManager::RegisterSelectRaceHandlers()
{
	using namespace CEGUI;
	mSelectRaceUI->getChild("ChooseRace/CleanAir")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_SelectRace1,this));
	mSelectRaceUI->getChild("ChooseRace/CO2Air")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_SelectRace2,this));
	mSelectRaceUI->getChild("ChooseRace/DustAir")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_SelectRace3,this));
}

void UIManager::RegisterHardLevelHandlers()
{
	using namespace CEGUI;
	mHardLevelUI->getChild("HardLevel/EasyButton")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_HardLevel0,this));
	mHardLevelUI->getChild("HardLevel/NormalButton")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_HardLevel1,this));
	mHardLevelUI->getChild("HardLevel/HardButton")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_HardLevel2,this));
}

void UIManager::RegisterPlayUIHandlers()
{
	using namespace CEGUI;
	mPlayUI->getChild("PlayUI/PauseButton")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_Cancel,this));
	mPlayUI->getChild("PlayUI/ResumeButton")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_Resume,this));
	mPlayUI->getChild("PlayUI/SystemButton")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&UIManager::Handle_Return,this));
}

//Handles
//bool UIManager::Handle_ClickTeamLogo(const CEGUI::EventArgs &e)
//{
//	CEGUI::Window* TeamLogoWindow=mRootWindow->getChild("TeamLogoWindow");
//	mRootWindow->removeChildWindow(TeamLogoWindow);
//	TeamLogoWindow->destroy();
//	this->createMainMenuUI();
//	return true;
//}

bool UIManager::Handle_StartGame(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu2.wav", NULL, SND_FILENAME | SND_ASYNC);
	mRootWindow->removeChildWindow(mMainMenuUI);
	this->createSelectRaceUI();
	return true;
}
bool UIManager::Handle_Credits(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu2.wav", NULL, SND_FILENAME | SND_ASYNC);
	using namespace CEGUI;

	CEGUI::Window* CreditsWindow=CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"Credit.layout");
	mRootWindow->addChildWindow(CreditsWindow);

	CreditsWindow->subscribeEvent(Window::EventMouseClick,Event::Subscriber(&UIManager::Handle_ClickCredits,this));
	return true;
}
bool UIManager::Handle_ClickCredits(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu2.wav", NULL, SND_FILENAME | SND_ASYNC);
	CEGUI::Window* CreditsWindow=mRootWindow->getChild("Credit");
	mRootWindow->removeChildWindow(CreditsWindow);
	CreditsWindow->destroy();
	return true;
}
bool UIManager::Handle_Quit(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu2.wav", NULL, SND_FILENAME | SND_ASYNC);
	mQuitGame=true;
	return true;
}
bool UIManager::Handle_SelectRace1(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu2.wav", NULL, SND_FILENAME | SND_ASYNC);
	//destroy buttons
	mSelectRaceUI->destroy();


	mRaceType='1';
	createHardLevelUI();

	return true;
}
bool UIManager::Handle_SelectRace2(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu2.wav", NULL, SND_FILENAME | SND_ASYNC);
	//destroy buttons
	mSelectRaceUI->destroy();

	mRaceType='2';
	createHardLevelUI();

	return true;
}
bool UIManager::Handle_SelectRace3(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu2.wav", NULL, SND_FILENAME | SND_ASYNC);
	//destroy buttons
	mSelectRaceUI->destroy();

	mRaceType='3';
	createHardLevelUI();

	return true;
}

bool UIManager::Handle_HardLevel0(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\start.wav", NULL, SND_FILENAME | SND_ASYNC);
	mGameStart=true;

	//destroy buttons
	mHardLevelUI->destroy();


	mHardLevel=0;
	mWorld->InitWorld(mSceneMgr,mLvMgr);
	mWorld->CreateWorld(mRaceType,mHardLevel);

	this->createPlayUI();
	return true;
}

bool UIManager::Handle_HardLevel1(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\start.wav", NULL, SND_FILENAME | SND_ASYNC);
	mGameStart=true;

	//destroy buttons
	mHardLevelUI->destroy();


	mHardLevel=1;
	mWorld->InitWorld(mSceneMgr,mLvMgr);
	mWorld->CreateWorld(mRaceType,mHardLevel);

	this->createPlayUI();
	return true;
}

bool UIManager::Handle_HardLevel2(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\start.wav", NULL, SND_FILENAME | SND_ASYNC);
	mGameStart=true;

	//destroy buttons
	mHardLevelUI->destroy();

	mHardLevel=2;
	mWorld->InitWorld(mSceneMgr,mLvMgr);
	mWorld->CreateWorld(mRaceType,mHardLevel);

	this->createPlayUI();
	return true;
}


bool UIManager::Handle_Cancel(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu.wav", NULL, SND_FILENAME | SND_ASYNC);
		mCancelGame=true;
		mPlayUI->getChild("PlayUI/PauseButton")->setVisible(false);
		mPlayUI->getChild("PlayUI/ResumeButton")->setVisible(true);


	return true;
}
bool UIManager::Handle_Resume(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu.wav", NULL, SND_FILENAME | SND_ASYNC);
		mCancelGame=false;
		mPlayUI->getChild("PlayUI/PauseButton")->setVisible(true);
		mPlayUI->getChild("PlayUI/ResumeButton")->setVisible(false);

	return true;
}
bool UIManager::Handle_Return(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu.wav", NULL, SND_FILENAME | SND_ASYNC);
	mPlayUI->destroy();
	mRootWindow->addChildWindow(mMainMenuUI);

	//Reset Game
	mWorld->DisableAnim();
	mWorld->ClearScene();
	mRaceType=0;
	mLvMgr->SetCurLevel(0);

	return true;
}

bool UIManager::Handle_Menu(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu.wav", NULL, SND_FILENAME | SND_ASYNC);
	mPlayUI->destroy();
	mRootWindow->addChildWindow(mMainMenuUI);
	CEGUI::Window* window;
	window=mRootWindow->getChild("MenuRetry");
	window->destroy();

	//Reset Game
	mWorld->DisableAnim();
	mWorld->ClearScene();
	mRaceType=0;
	mLvMgr->SetCurLevel(0);

	return true;
}

bool UIManager::Handle_Cong(const CEGUI::EventArgs &e)
{
	mRootWindow->addChildWindow(mMainMenuUI);//回到主菜单
	mRootWindow->getChild("End")->destroy();//销毁结局窗口

	//Reset Game
	//mWorld->DisableAnim();
	//mWorld->ClearScene();
	mRaceType=0;
	mLvMgr->SetCurLevel(0);

	return true;
}
bool UIManager::Handle_Next(const CEGUI::EventArgs &e)
{
	PlaySound("sound\\menu.wav", NULL, SND_FILENAME | SND_ASYNC);
	//显示Menu、Cancel按钮
	mPlayUI->setVisible(true);
	CEGUI::Window* window=mRootWindow->getChild("NextButton");
	mRootWindow->removeChildWindow(window);
	window->destroy();

	mLvMgr->LevelUp();
	mWorld->ClearScene();
	mWorld->CreateWorld(mRaceType,mHardLevel);
	return true;
}

bool UIManager::Handle_Retry(const CEGUI::EventArgs &e)
{
	//显示Menu、Cancel按钮
	mPlayUI->setVisible(true);
	CEGUI::Window* window;
	
	window=mRootWindow->getChild("MenuRetry");
	mRootWindow->removeChildWindow(window);
	window->destroy();

	mWorld->ClearScene();
	mWorld->CreateWorld(mRaceType,mHardLevel);
	return true;
}
bool UIManager::isStart()
{
	if (mGameStart==true)
	{
		return true;
	}
	else
		return false;
}
bool UIManager::isQuit()
{
	if (mQuitGame==true)
	{
		return true;
	}
	else
		return false;
}
bool UIManager::isCancel()
{
	if (mCancelGame==true)
	{
		return true;
	}
	else
		return false;
}
//bool UIManager::ShowTeamLogo()
//{
//	using namespace CEGUI;
//	CEGUI::Window* TeamLogoWindow;
//	TeamLogoWindow = CEGUI::WindowManager::getSingleton().createWindow("Vanilla/StaticImage","TeamLogoWindow" );
//	TeamLogoWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5,0),CEGUI::UDim(0.5,0)));
//	TeamLogoWindow->setSize(UVector2(UDim(0,150),UDim(0,100)));
//	TeamLogoWindow->setProperty("Image","set:Vanilla image:full_image");
//	mRootWindow->addChildWindow(TeamLogoWindow);
//
//	TeamLogoWindow->subscribeEvent(Window::EventMouseClick,Event::Subscriber(&UIManager::Handle_ClickTeamLogo,this));
//	return true;
//}



bool UIManager::createMainMenuUI()
{
	mMainMenuUI=CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"MainMenu.layout");
	mRootWindow->addChildWindow(mMainMenuUI);
	this->RegisterMainMenuHandlers();
	return true;
}


bool UIManager::createSelectRaceUI()
{
	mSelectRaceUI=CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"ChooseRace.layout");
	mRootWindow->addChildWindow(mSelectRaceUI);
	this->RegisterSelectRaceHandlers();
	return true;
}

bool UIManager::createHardLevelUI()
{
	mHardLevelUI=CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"HardLevel.layout");
	mRootWindow->addChildWindow(mHardLevelUI);
	this->RegisterHardLevelHandlers();
	return true;
}

bool UIManager::createPlayUI()
{
	mPlayUI=CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"PlayUI.layout");
	mRootWindow->addChildWindow(mPlayUI);
	this->RegisterPlayUIHandlers();
	return true;
}