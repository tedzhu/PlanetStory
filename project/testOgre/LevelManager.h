#pragma once

#include "stdafx.h"
#include "TinyXMLResourceManager.h"

class LevelManager
{
public:
	LevelManager(void);
	~LevelManager(void);
	bool LoadLevelFile();//载入关卡文件
	void LevelUp(); 
	int GetGameState(void);
	void SetGameState(int gs);
	int GetCurLevel(void);
	void SetCurLevel(int lv);
	bool IsFinished();
	TiXmlElement* GetXmlElement();
	
	

protected:
	int mLevel;//当前level
	int mGameState;//当前游戏进度 0.未结束 -1.电脑胜 1.玩家胜 2.播放动画
	int mLvNum;//level总数
	bool finishAll; //是否通关
	TiXmlNode* mLevelData;
};
