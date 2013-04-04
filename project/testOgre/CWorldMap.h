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

    int Update(double timeSinceLastFrame,bool LButtonDown);//������0ʱ����������1ʱ���Ӯ������-1ʱ����Ӯ
	int GetHardLevel(void) const;
	std::vector<CIsland*>& GetIslands(void);
	std::vector<CForceController*>& GetForces(void);
	~CWorldMap(void);
    typedef enum {STRENGTH,MOVESPEED,BREEDSPEED} RaceAttribute;

	void WinAnim();//ʤ������
	void LoseAnim();//ʧ�ܶ���
	//void CongAnim();//ͨ�ض���
	void DisableAnim();//ֹͣ����
	LevelManager* s_LvMgr;	

private:
	void StartAnim();//�����̶��켣����
	void SetMarkIsland(bool LbuttonDown);//���컭��Ȧ����
	void SetCircleIsland();//���컭ʵȦ
	void ClearAllLines();//������л���
	void DestroyAllAttachedMovableObjects( Ogre::SceneNode* i_pSceneNode );
	static double s_eRadius;//the earth's radius
	static int s_iMaxAmount[3];//ÿ�������������
    static double s_iRadius[3];//three radiuses of islands
    static double s_iGrowBonus[3];//three bonuses of different island
    static int s_RaceAttributes[4][3];//different parameters of different races
    static unsigned int s_MaxSelect;//���������ѡ��ĵ�����
	static int s_ShowNumLevel;//һ����������ʾ��ֵΪ"��"
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
	Ogre::RaySceneQuery* rsq;//������߲�ѯ
	CIsland* lastIsland; //��һ������Ȧ�ĵ���
};
