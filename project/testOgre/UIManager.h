#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include "stdafx.h"
#include "LevelManager.h"

class UIManager
{
public:
	UIManager(void);
	~UIManager(void);
	bool SetupUI(Ogre::SceneManager* scmgr,LevelManager *lvmgr,CWorldMap* world);		//��ʼ��CEGUI	
	//bool ShowTeamLogo(); //��ʾTeam logo
	bool createMainMenuUI(); //���˵�
	bool createSelectRaceUI();//ѡ������
	bool createHardLevelUI(); //ѡ���Ѷȵȼ�
	bool isStart();		//�Ƿ�ʼ��Ϸ
	bool isQuit();
	bool isCancel();
	bool createPlayUI();	//��Ϸ����ʱ��UI
	void Update();
protected:
	void RegisterMainMenuHandlers();
	void RegisterSelectRaceHandlers();
	void RegisterPlayUIHandlers();
	void RegisterHardLevelHandlers();
	//bool Handle_ClickTeamLogo(const CEGUI::EventArgs &e);
	bool Handle_StartGame(const CEGUI::EventArgs &e);
	bool Handle_Credits(const CEGUI::EventArgs &e);
	bool Handle_ClickCredits(const CEGUI::EventArgs &e);
	bool Handle_Quit(const CEGUI::EventArgs &e);
	bool Handle_SelectRace1(const CEGUI::EventArgs &e);
	bool Handle_SelectRace2(const CEGUI::EventArgs &e);
	bool Handle_SelectRace3(const CEGUI::EventArgs &e);
	bool Handle_HardLevel0(const CEGUI::EventArgs &e);
	bool Handle_HardLevel1(const CEGUI::EventArgs &e);
	bool Handle_HardLevel2(const CEGUI::EventArgs &e);
	bool Handle_Cancel(const CEGUI::EventArgs &e);
	bool Handle_Resume(const CEGUI::EventArgs &e);
	bool Handle_Return(const CEGUI::EventArgs &e);
	bool Handle_Next(const CEGUI::EventArgs &e);
	bool Handle_Retry(const CEGUI::EventArgs &e);
	bool Handle_Menu(const CEGUI::EventArgs &e);
	bool Handle_Cong(const CEGUI::EventArgs &e);

	//void Handle_Cancel();
	CEGUI::Window* mRootWindow;
	CEGUI::Window* mMainMenuUI;
	CEGUI::Window* mSelectRaceUI;
	CEGUI::Window* mHardLevelUI;
	CEGUI::Window* mPlayUI;

	Ogre::SceneManager* mSceneMgr;
	LevelManager *mLvMgr; 
	CWorldMap* mWorld;
	bool mGameStart;
	bool mQuitGame;
	bool mCancelGame;
	char mRaceType;
	int mHardLevel;
};
#endif