#pragma once
#include "stdafx.h"
#include "MovableText.h"
class CIsland
{
public:
	CIsland(Ogre::String name,Ogre::Vector3& pos,char type);
	~CIsland(void);
    static CIsland* GetMappedIsland(Ogre::Entity*);//根据Entity的指针找对应的CIsland，如果没找到，返回空
    static void Reset(void);//将m_MapEntity清空
	void SetOccupant(CGas *occupant);//called by object of CGas when he successfully occupied an island( in CGas::Attack)

    CGas* GetDomGas(void) const; //获得占领它的气体
    char GetType(void) const;//获得类型
    Ogre::Vector3 GetPosition(void) const; //获得位置
    double GetGrowBonus(void) const;//气体增值速度
	void SetEntity(Ogre::Entity*);
	void Update(void); //add by wolfwind 2011.3.14
	Ogre::String GetIslandName();//add by wolfwind 2011.3.16

	//add by wolfwind 2011.3.19
	void DrawLineTo(CIsland* island,bool isSameRace);

	//2011.3.27-----------------------
	//画虚圈
	void MarkEnable(bool isSameRace);
	void MarkDisable();
	//画实圈
	void CircleEnable(bool isSameRace);
	void CircleDisable();

	int GetMarkColor();
	void ShowNumEnable(bool b);
private:
    void ChangeAspect(void);
    static std::map<Ogre::Entity*,CIsland*> s_MapEntity;
    
    char m_type;
    CGas *m_DomGas;
    Ogre::Entity *m_Entity;
	int m_GasNum; //add by wolfwind 2011.3.14
	Ogre::MovableText *m_NumText;    //add by wolfwind 2011.3.14 
	Ogre::String m_Name;//add by wolfwind 2011.3.16
	Ogre::SceneNode *pIslandNode;

	int markColor;//画虚圈颜色，0：无，1：绿，2：红
	bool showNum;

};
