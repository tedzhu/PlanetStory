#pragma once
#include "stdafx.h"
#include "LevelManager.h"

class CWorldMap
{
public:

    static double GetEarthRadius(void);
    static double GetIslandRadius(char islandtype);//type is a, b or c
    static double GetGrowBonus(char islandtype);
    static int GetRaceStrength(char racetype);//type is 1, 2 or 3
    static int GetRaceMoveSpeed(char racetype);
    static int GetRaceBreedSpeed(char racetype);
    static unsigned int GetMaxSelectLimit(void);
	static int GetMaxGasAmount(char islandtype);
    static Ogre::SceneManager* GetSceneMgr(void);

	CWorldMap(void);
	void InitWorld(Ogre::SceneManager *scmgr,LevelManager* lvmgr);//add by wolfwind 2011.3.16
    void CreateWorld(char RaceType,int hardlevel);//modified by wolfwind 2011.3.16
	void OnMousePressed(Ogre::Entity*);
	void OnMouseReleased(Ogre::Entity*);
	void OnMouseMoved(bool LButtonDown,Ogre::Entity* ent); //add by wolfwind 2011.3.19
	void ClearScene(void);//add by wolfwind 2011.3.14
	CIsland* GetIslandByName(Ogre::String name);//add by wolfwind 2011.3.16

    int Update(double timeSinceLastFrame,bool LButtonDown);//当返回0时继续，返回1时玩家赢，返回-1时电脑赢
	int GetHardLevel(void) const;
	std::vector<CIsland*>& GetIslands(void);
	std::vector<CForceController*>& GetForces(void);
	~CWorldMap(void);
    typedef enum {STRENGTH,MOVESPEED,BREEDSPEED} RaceAttribute;

	void WinAnim();//胜利动画
	void LoseAnim();//失败动画
	//void CongAnim();//通关动画
	void DisableAnim();//停止动画
	LevelManager* s_LvMgr;	

private:
	void StartAnim();//开场固定轨迹动画
	void SetMarkIsland(bool LbuttonDown);//岛屿画虚圈画线
	void SetCircleIsland();//岛屿画实圈
	void ClearAllLines();//清除所有画线
	void DestroyAllAttachedMovableObjects( Ogre::SceneNode* i_pSceneNode );
	static double s_eRadius;//the earth's radius
	static int s_iMaxAmount[3];//每个岛屿最大容量
    static double s_iRadius[3];//three radiuses of islands
    static double s_iGrowBonus[3];//three bonuses of different island
    static int s_RaceAttributes[4][3];//different parameters of different races
    static unsigned int s_MaxSelect;//玩家最多可以选择的岛屿数
	static int s_ShowNumLevel;//一定关数后显示数值为"？"
    static Ogre::SceneManager* s_SceneMgr;
    std::vector<CIsland*> m_vecIslands;
    std::vector<CForceController*> m_vecForces;
    CForceController* m_Player;
	CRITICAL_SECTION m_cs;

	
	TiXmlElement *XMLRoot;
	TiXmlElement *XMLlevel;
	Ogre::SceneNode* pEarthNode;
	Ogre::AnimationState* mAnimState;
	bool bCamReset;
	char mPlayerRace;
	int m_HardLevel;
	Ogre::RaySceneQuery* rsq;//鼠标射线查询
	CIsland* lastIsland; //上一个被虚圈的岛屿
};
