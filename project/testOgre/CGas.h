#pragma once
#include "stdafx.h"

enum QueryFlags
{
	ISLAND_MASK = 1<<0,
	PLANET_MASK = 1<<1,
	GAS_MASK = 1<<2,
	CITY_MASK = 1<<3,
	MARK_MASK = 1<<4
};
class CGas
{
public:
	CGas(char race, int num,CForceController* force);//���������������forceΪNULL
	~CGas(void);
	void SetTargetIsland(CIsland *targetIsland);
	void Update(double timeSinceLast);
    void SetBelongedIsland(CIsland*);
	char GetRace(void);//���壬'0','1','2'����'3'��'1','2','3'�����������壬 '0'������������
    int GetStrength(void);
    int GetMoveSpeed(void);
    int GetBreedSpeed(void);
    double GetCurrNum(void);//��ǰ����
    Ogre::Vector3 GetCurrPos(void);//��ǰλ��
    CIsland* GetTargetIsland(void);//Ŀ�굺��
    CIsland* GetBelongedIsland(void);//�������죬��TargetIsland��Ϊ��ʱ����������������
	void SetEntity(Ogre::Entity*);
	Ogre::Entity* GetEntity();

	
	Ogre::Real GetCurScale();//add by wolfwind 2011.3.19
	void SetCurScale(Ogre::Real);
private:
	void SendDeathMsg(void);//called in Attack when failed and in Merge When arrived
	void Move(double timeSinceLast);
    void Breed(double timeSinceLast);
	void DealWithConfict(CIsland *confictIsland);//arrived the island, is it the target?
	CIsland* ArrivedSomeIsland(void);
	void Attack(CGas *defender);//attacker will receive this msg, called by CIsland
	void Merge(CGas *preislandgas);
	void MoveAnim();

	double m_Num;//current number of gas
    char m_Race;
	CIsland *m_TargetIsland;
	CIsland *m_BelongedIsland;
	CForceController *m_BelongedForce;
    Ogre::Entity *m_Entity;
    Ogre::Vector3 m_e1;
    Ogre::Vector3 m_e2;
    double m_angle;

	//add by wolfwind 2011.3.18
	Ogre::SceneNode* psNode;
	Ogre::Real curScale;
	Ogre::AnimationState* mGasAnimState;
};
