#pragma once
#include "stdafx.h"
#include "MovableText.h"
class CIsland
{
public:
	CIsland(Ogre::String name,Ogre::Vector3& pos,char type);
	~CIsland(void);
    static CIsland* GetMappedIsland(Ogre::Entity*);//����Entity��ָ���Ҷ�Ӧ��CIsland�����û�ҵ������ؿ�
    static void Reset(void);//��m_MapEntity���
	void SetOccupant(CGas *occupant);//called by object of CGas when he successfully occupied an island( in CGas::Attack)

    CGas* GetDomGas(void) const; //���ռ����������
    char GetType(void) const;//�������
    Ogre::Vector3 GetPosition(void) const; //���λ��
    double GetGrowBonus(void) const;//������ֵ�ٶ�
	void SetEntity(Ogre::Entity*);
	void Update(void); //add by wolfwind 2011.3.14
	Ogre::String GetIslandName();//add by wolfwind 2011.3.16

	//add by wolfwind 2011.3.19
	void DrawLineTo(CIsland* island,bool isSameRace);

	//2011.3.27-----------------------
	//����Ȧ
	void MarkEnable(bool isSameRace);
	void MarkDisable();
	//��ʵȦ
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

	int markColor;//����Ȧ��ɫ��0���ޣ�1���̣�2����
	bool showNum;

};
