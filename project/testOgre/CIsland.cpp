#include "stdafx.h"
#include "MovableText.h"

std::map<Ogre::Entity*,CIsland*> CIsland::s_MapEntity;

CIsland::CIsland(Ogre::String name,Ogre::Vector3& pos,char type)
:m_type(type)
,m_DomGas(NULL)
,m_Name(name)
,m_GasNum(0)
,markColor(0)
{
    //TODO: 根据不同的类型载入不同的岛屿的模型
	static char* IslandMesh[3]={
		"island_large.mesh",
		"island_middle.mesh",
		"island_small.mesh"
	};

	Ogre::SceneManager* sm=CWorldMap::GetSceneMgr();
	m_Entity = sm->createEntity(name,IslandMesh[type-'a']);
	m_Entity->setQueryFlags(ISLAND_MASK);
	Ogre::SceneNode *pEarthNode =  sm->getSceneNode("Earth");
	pIslandNode = pEarthNode->createChildSceneNode(name+"Node",pos);
	pIslandNode->attachObject(m_Entity);
	pIslandNode->scale(Ogre::Vector3(2,2,2));
	Ogre::Vector3 src = pIslandNode->getOrientation()* Ogre::Vector3::UNIT_X; //当前模型朝向
	Ogre::Quaternion quat = src.getRotationTo(pos); //pos为方向向量
	pIslandNode->rotate(quat); //转动到目标方向
	pIslandNode-> yaw(Ogre::Degree(-90));
	s_MapEntity.insert(std::make_pair<Ogre::Entity*,CIsland*>(this->m_Entity,this));

	//add city
	Ogre::Entity* cityEnt=sm->createEntity(name+"city","city.mesh");
	cityEnt->setQueryFlags(CITY_MASK);
	Ogre::SceneNode* cityNode=pEarthNode->createChildSceneNode(name+"cityNode");
	cityNode->attachObject(cityEnt);

	//add circle
	Ogre::Entity* circleEnt=sm->createEntity(m_Name+"GreenCircleEnt","MarkGreenCircle.mesh");
	circleEnt->setQueryFlags(MARK_MASK);
	Ogre::SceneNode* greenCircleNode=pIslandNode->createChildSceneNode(m_Name+"GreenCircleNode");
	greenCircleNode->attachObject(circleEnt);
	circleEnt=sm->createEntity(m_Name+"RedCircleEnt","MarkRedCircle.mesh");
	circleEnt->setQueryFlags(MARK_MASK);
	Ogre::SceneNode* redCircleNode=pIslandNode->createChildSceneNode(m_Name+"RedCircleNode");
	redCircleNode->attachObject(circleEnt);


	if (type=='a')
	{
		cityNode->scale(3,3,3);
		cityNode->setPosition(pos*0.982f);
		greenCircleNode->scale(0.8f,0.8f,0.8f);
		redCircleNode->scale(0.8f,0.8f,0.8f);
	}
	else if (type=='b')
	{
		cityNode->scale(2.5f,2.5f,2.5f);
		cityNode->setPosition(pos*0.99f);
		greenCircleNode->scale(0.6f,0.6f,0.6f);
		redCircleNode->scale(0.6f,0.6f,0.6f);
	}
	else
	{
		cityNode->scale(2,2,2);
		cityNode->setPosition(pos);
		greenCircleNode->scale(0.5f,0.5f,0.5f);
		redCircleNode->scale(0.5f,0.5f,0.5f);
	}
	cityNode->rotate(quat);
	cityNode->yaw(Ogre::Degree(-90));
	greenCircleNode->pitch(Ogre::Degree(-90));
	greenCircleNode->setVisible(false);
	redCircleNode->pitch(Ogre::Degree(-90));
	redCircleNode->setVisible(false);

	//set Num Text,add by Wolfwind 2011.3.13
	m_NumText=new Ogre::MovableText(m_Entity->getName()+"Text","?");
	m_NumText->setTextAlignment(Ogre::MovableText::H_CENTER, Ogre::MovableText::V_ABOVE); // Center horizontally and display above the node
	m_NumText->setCharacterHeight( 20.0f ); //msg->setAdditionalHeight( ei.getRadius() )
	m_NumText->setColor(Ogre::ColourValue::White);
	m_NumText->setQueryFlags(MARK_MASK);
	Ogre::SceneNode* pTextNode=pIslandNode->getParentSceneNode()->createChildSceneNode();
	pTextNode->attachObject(m_NumText);
	pTextNode->setPosition(pIslandNode->getPosition()*1.2f);
    return;
}

CIsland::~CIsland(void)
{
    //TODO:删除该Entity和相应Node
    if (this->m_Entity!=NULL){
        s_MapEntity.erase(this->m_Entity);
        Ogre::SceneNode *pSceneNode = this->m_Entity->getParentSceneNode();
        pSceneNode->detachAllObjects();
        CWorldMap::GetSceneMgr()->destroyEntity(this->m_Entity);
        CWorldMap::GetSceneMgr()->destroySceneNode(pSceneNode);
        this->m_Entity = NULL;
    } //if
    return;
}


void CIsland::SetOccupant(CGas *occupant)
{
    this->m_DomGas = occupant;
    this->ChangeAspect();
}

CGas* CIsland::GetDomGas(void) const
{
    return this->m_DomGas;
}

char CIsland::GetType(void) const
{
    return this->m_type;
}

Ogre::Vector3 CIsland::GetPosition(void) const
{
    return this->m_Entity->getParentSceneNode()->getPosition();
}

double CIsland::GetGrowBonus(void) const
{
    return CWorldMap::GetGrowBonus(this->m_type);
}

void CIsland::ChangeAspect(void)
{
    //TODO: Change the Aspect depends on the type of race which one occupied it
}
void CIsland::SetEntity(Ogre::Entity* entity)
{
	this->m_Entity = entity;
}

CIsland* CIsland::GetMappedIsland(Ogre::Entity* entity)
{
    std::map<Ogre::Entity*,CIsland*>::iterator iter = s_MapEntity.find(entity);
    if (iter == s_MapEntity.end()){
        return NULL;
    }
    return iter->second;
}

void CIsland::Reset(void)
{
    s_MapEntity.clear();
}

void CIsland::Update(void)
{
	
	int newGasNum=int(this->m_DomGas->GetCurrNum())+1;
	if (newGasNum!=m_GasNum)
	{
		if (newGasNum>m_GasNum)
		{
			m_GasNum++;
		}
		if (newGasNum<m_GasNum)
		{
			m_GasNum--;
		}
		if (showNum==false)
		{
			m_NumText->setCaption("?");
		}
		else
		{
			m_NumText->setCaption(Ogre::StringConverter::toString(m_GasNum));
		}
		
		Ogre::SceneNode* gasNode=this->GetDomGas()->GetEntity()->getParentSceneNode();
		Ogre::Real gascurscale=this->GetDomGas()->GetCurScale();
		Ogre::Real tmpReal;
		if (gascurscale>0 && this->GetDomGas()->GetCurrNum()!=0)
		{
			tmpReal=(this->GetDomGas()->GetCurrNum()/150)*3.0+0.5;//scale 0-150->0.5-3.5
			this->GetDomGas()->SetCurScale(tmpReal);
			tmpReal=tmpReal/gascurscale;
			gasNode->scale(tmpReal,tmpReal,tmpReal);			
		}
		else
		{
			gasNode->setVisible(false);
		}
	}
}

Ogre::String CIsland::GetIslandName()
{
	return m_Name;
}

//画线
void CIsland::DrawLineTo(CIsland* target,bool isSameRace)
{
	//如果已经有了该线，不需重画
	if (CWorldMap::GetSceneMgr()->hasSceneNode(m_Name+"lineNode"))
	{
		return;
	}
	//如果是自身到自身，不需画
	if (this->m_Name==target->GetIslandName())
	{
		return;
	}
	float angle =0;

	Ogre::Vector3 p1 = this->GetPosition();
	Ogre::Vector3 p2 = target->GetPosition();
	Ogre::Vector3 e1 = p1/CWorldMap::GetEarthRadius();
	Ogre::Vector3 e2= (p1.crossProduct(p2)).crossProduct(p1);
	if (e2==Ogre::Vector3(0,0,0))
	{
		Ogre::Vector3 tmpVec;
		srand((unsigned)time(0));
		do 
		{
			tmpVec.x=rand()%10;
			tmpVec.y=rand()%10;
			tmpVec.z=rand()%10;
			e2=p2.crossProduct(tmpVec);
		} while (e2==Ogre::Vector3(0,0,0));
		e2.normalise();
	}
	e2 = e2/sqrt(e2.x*e2.x+e2.y*e2.y+e2.z*e2.z);
	if (e2.x*p2.x+e2.y*p2.y+e2.z*p2.z<0){
		e2 *=-1;
	} //if

	Ogre::SceneNode* lineRootNode=CWorldMap::GetSceneMgr()->getSceneNode("LineNode")->createChildSceneNode(m_Name+"lineNode");
	while(1)
	{
		angle+=0.08f;
		Ogre::Vector3 vec = (e1*cos(angle)+e2*sin(angle))*CWorldMap::GetEarthRadius();

		Ogre::Entity* lineEnt;
		if (isSameRace==false)
		{
			lineEnt=CWorldMap::GetSceneMgr()->createEntity("RedBall.mesh");
		}
		if (isSameRace==true)
		{
			lineEnt=CWorldMap::GetSceneMgr()->createEntity("GreenBall.mesh");
		}
		Ogre::SceneNode* lineNode=lineRootNode->createChildSceneNode();
		lineEnt->setQueryFlags(MARK_MASK);
		lineNode->attachObject(lineEnt);
		lineNode->setPosition(vec);
		lineNode->scale(0.5f,0.5f,0.5f);
		Ogre::Vector3 src = lineNode->getOrientation()*Ogre::Vector3::UNIT_X;
		Ogre::Quaternion quat = src.getRotationTo(vec);
		lineNode->rotate(quat,Ogre::Node::TS_WORLD);
		//lineNode->roll(Ogre::Degree(-90));

		if (lineNode->getPosition().distance(target->GetPosition())<10)
		{
			break;
		}
	}
	return;

}


//画虚圈
void CIsland::MarkEnable(bool isSameRace)
{
	using namespace Ogre;
	Entity* markEnt;
	SceneManager* sm=CWorldMap::GetSceneMgr();

	if (sm->hasEntity(m_Name+"MarkEnt"))
	{
		return;
	}
	if (isSameRace==true)
	{
		markEnt=sm->createEntity(m_Name+"MarkEnt","MarkGreen.mesh");
		markColor=1;
	}
	else
	{
		markEnt=sm->createEntity(m_Name+"MarkEnt","MarkRed.mesh");
		markColor=2;
	}

	markEnt->setQueryFlags(MARK_MASK);
	SceneNode*	markNode=pIslandNode->createChildSceneNode(m_Name+"MarkNode");
	markNode->attachObject(markEnt);
	markNode->pitch(Degree(-90));
	if (m_type=='a')
	{
		markNode->scale(0.8f,0.8f,0.8f);
	}
	else if (m_type=='b')
	{
		markNode->scale(0.6f,0.6f,0.6f);
	}
	else if (m_type=='c')
	{
		markNode->scale(0.5f,0.5f,0.5f);
	}
}
//删虚圈
void CIsland::MarkDisable()
{
	using namespace Ogre;
	SceneManager* sm=CWorldMap::GetSceneMgr();
	if (sm->hasEntity(m_Name+"MarkEnt"))
	{
		sm->destroySceneNode(m_Name+"MarkNode");
		sm->destroyEntity(m_Name+"MarkEnt");
	}
	markColor=0;
}

//画实圈
void CIsland::CircleEnable(bool isSameRace)
{
	using namespace Ogre;
	SceneManager* sm=CWorldMap::GetSceneMgr();
	SceneNode* circlenode;
	if (isSameRace==true)
	{
		circlenode=sm->getSceneNode(m_Name+"GreenCircleNode");
	}
	else
	{
		circlenode=sm->getSceneNode(m_Name+"RedCircleNode");
	}
	circlenode->setVisible(true);
}
//删实圈
void CIsland::CircleDisable()
{
	using namespace Ogre;
	SceneManager* sm=CWorldMap::GetSceneMgr();
	SceneNode* circlenode;
	circlenode=sm->getSceneNode(m_Name+"GreenCircleNode");
	circlenode->setVisible(false);
	circlenode=sm->getSceneNode(m_Name+"RedCircleNode");
	circlenode->setVisible(false);
}

int CIsland::GetMarkColor()
{
	return markColor; 
}

void CIsland::ShowNumEnable(bool b)
{
	showNum=b;
}