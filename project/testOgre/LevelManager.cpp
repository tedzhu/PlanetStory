#include "stdafx.h"


LevelManager::LevelManager(void)
:mLevel(0)
,mGameState(0)
{
}

LevelManager::~LevelManager(void)
{
}
bool LevelManager::LoadLevelFile()
{
	OGRE_NEW TinyXMLResourceManager();
	TinyXMLPtr xmlLevelFile=TinyXMLResourceManager::getSingleton().create("Level.level","Popular");
	xmlLevelFile->load();
	
	if (xmlLevelFile->getError())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("An error occured in file"+xmlLevelFile->getName()+":"+Ogre::String(xmlLevelFile->getErrorDesc()));
		return false;
	}
	else
	{
		mLevelData=xmlLevelFile->getXMLData();
	}
	TiXmlElement   *XMLRoot;
	XMLRoot=mLevelData->GetDocument()->RootElement();
	if (Ogre::String(XMLRoot->Value())!="levels")
	{
		return false;
	}
	mLvNum=Ogre::StringConverter::parseInt(XMLRoot->Attribute("num"));
	return true;
}
TiXmlElement* LevelManager::GetXmlElement()
{
	return mLevelData->GetDocument()->RootElement();
}
int LevelManager::GetGameState()
{
	return mGameState;
}
void LevelManager::SetGameState(int gs)
{
	mGameState=gs;
}
int LevelManager::GetCurLevel()
{
	return mLevel;
}
void LevelManager::SetCurLevel(int lv)
{
	mLevel=lv;
}
void LevelManager::LevelUp()
{
	mLevel++;
}

bool LevelManager::IsFinished()
{
	if (mLevel<mLvNum)
	{
		return false;
	}
	else
		return true;
}