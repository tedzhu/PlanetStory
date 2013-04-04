#pragma once

#include "stdafx.h"
#include "TinyXMLResourceManager.h"

class LevelManager
{
public:
	LevelManager(void);
	~LevelManager(void);
	bool LoadLevelFile();//����ؿ��ļ�
	void LevelUp(); 
	int GetGameState(void);
	void SetGameState(int gs);
	int GetCurLevel(void);
	void SetCurLevel(int lv);
	bool IsFinished();
	TiXmlElement* GetXmlElement();
	
	

protected:
	int mLevel;//��ǰlevel
	int mGameState;//��ǰ��Ϸ���� 0.δ���� -1.����ʤ 1.���ʤ 2.���Ŷ���
	int mLvNum;//level����
	bool finishAll; //�Ƿ�ͨ��
	TiXmlNode* mLevelData;
};
