#include "stdafx.h"
double CWorldMap::s_eRadius = 150;
double CWorldMap::s_iRadius[3] = {15,10,5};
int CWorldMap::s_iMaxAmount[3] = {150,100,50};//每个岛屿最大气体数
int CWorldMap::s_ShowNumLevel=5;
double CWorldMap::s_iGrowBonus[3] = {1,1.1,1.2};
int CWorldMap::s_RaceAttributes[4][3] = {
	{10,0,0}, // 中立
	{10,10,10},//清洁
	{12,8,12},  //二氧化碳
	{15,5,5} //有毒
};
unsigned int CWorldMap::s_MaxSelect = 5;
Ogre::SceneManager* CWorldMap::s_SceneMgr = NULL;

double CWorldMap::GetEarthRadius(void)
{
	return s_eRadius;
}

double CWorldMap::GetIslandRadius(char islandtype)
{
	assert(islandtype>='a' && islandtype <='c');
	return s_iRadius[islandtype-'a'];
}

double CWorldMap::GetGrowBonus(char islandtype)
{
	assert(islandtype>='a' && islandtype <='c');
	return s_iGrowBonus[islandtype-'a'];
}

std::vector<CIsland*>& CWorldMap::GetIslands(void)
{
	return this->m_vecIslands;
} 

std::vector<CForceController*>& CWorldMap::GetForces(void)
{
	return this->m_vecForces;
}


CWorldMap::CWorldMap(void)
:m_Player(NULL),
bCamReset(true)
{
	InitializeCriticalSection(&m_cs);
}

//初始化世界
void CWorldMap::InitWorld(Ogre::SceneManager *scmgr,LevelManager* lvmgr)
{
	using namespace Ogre;
	if (lvmgr->GetCurLevel()==0)
	{
		s_SceneMgr = scmgr;
		s_LvMgr=lvmgr;
		s_LvMgr->SetCurLevel(1);
		//get xml data
		XMLRoot=s_LvMgr->GetXmlElement();
	}
	
}

//根据己方玩家的种族创建世界
void CWorldMap::CreateWorld(char playerType,int hardlevel)
{
	using namespace Ogre;
	CIsland::Reset();
	s_LvMgr->SetGameState(0);
	mPlayerRace=playerType;
	m_HardLevel=hardlevel;
	rsq=s_SceneMgr->createRayQuery(Ogre::Ray());
	lastIsland=NULL;

	s_SceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	//camera node
	SceneNode* CamNode=s_SceneMgr->getRootSceneNode()->createChildSceneNode("RootCamNode");

	// Set ambient light
	s_SceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

	//add light
	Light *pLight = s_SceneMgr->createLight("MainLight1");
	pLight->setPosition(300,0,0);
	pLight=s_SceneMgr->createLight("MainLight2");
	pLight->setPosition(-300,0,0);
	pLight=s_SceneMgr->createLight("MainLight3");
	pLight->setPosition(0,300,0);
	pLight=s_SceneMgr->createLight("MainLight4");
	pLight->setPosition(0,-300,0);
	pLight=s_SceneMgr->createLight("MainLight5");
	pLight->setPosition(0,0,300);
	pLight=s_SceneMgr->createLight("MainLight6");
	pLight->setPosition(0,0,-300);

	//add earth
	Entity *pEarthEnt = s_SceneMgr->createEntity("Earth","waterball.mesh");
	pEarthEnt->setQueryFlags(PLANET_MASK);
	pEarthNode = s_SceneMgr->getRootSceneNode()->createChildSceneNode("Earth",Vector3(0,0,0));
	pEarthNode->attachObject(pEarthEnt);

	//标注关卡
	OverlayManager& overlayManager = OverlayManager::getSingleton();
	Ogre::OverlayContainer* coutPanel = static_cast<OverlayContainer*>(
		overlayManager.createOverlayElement("Panel", "PanelName"));
	TextAreaOverlayElement* textArea = static_cast<TextAreaOverlayElement*>(
		overlayManager.createOverlayElement("TextArea", "TextAreaName"));
	textArea->setMetricsMode(Ogre::GMM_PIXELS);
	textArea->setPosition(20, 10);
	textArea->setDimensions(300, 100);
	textArea->setCaption("Level : "+Ogre::StringConverter::toString(s_LvMgr->GetCurLevel()));
	textArea->setCharHeight(30);
	textArea->setFontName("SdkTrays/Caption");
	textArea->setColour(ColourValue(9.0f, 9.0f, 9.0f));
	
	Overlay* overlay = overlayManager.create("OverlayName");
	overlay->add2D(coutPanel);
	// Add the text area to the panel
	coutPanel->addChild(textArea);
	// Show the overlay
	overlay->show();
	//画线节点
	pEarthNode->createChildSceneNode("LineNode");

	TiXmlElement *XMLisland,*XMLpos,*XMLgas;
	//找到当前level对应的XML数据
	int curXmlLevel;
	XMLlevel=XMLRoot->FirstChildElement("level");
	while (XMLlevel)
	{
		curXmlLevel=Ogre::StringConverter::parseInt(XMLlevel->Attribute("value"));
		if (curXmlLevel!=s_LvMgr->GetCurLevel())
		{
			XMLlevel=XMLlevel->NextSiblingElement("level");
		}
		else
			break;
	}

	//根据当前level的Xml数据创建island
	XMLisland=XMLlevel->FirstChildElement("islands");
	XMLisland=XMLisland->FirstChildElement("island");
	while (XMLisland)
	{
		String islandname=XMLisland->Attribute("name");
		const char* islandType=XMLisland->Attribute("type");
		XMLpos=XMLisland->FirstChildElement("position");
		Vector3 tmpPos;
		tmpPos.x=StringConverter::parseReal(XMLpos->Attribute("x"));
		tmpPos.y=StringConverter::parseReal(XMLpos->Attribute("y"));
		tmpPos.z=StringConverter::parseReal(XMLpos->Attribute("z"));

		CIsland *island=new CIsland(islandname,tmpPos,islandType[0]);
		this->m_vecIslands.push_back(island);

		//设置是否显示数值
		if (s_LvMgr->GetCurLevel()>s_ShowNumLevel)
		{
			island->ShowNumEnable(false);
		}
		else
			island->ShowNumEnable(true);

		XMLisland=XMLisland->NextSiblingElement("island");
	}

	//创建ForceController
	CPlayerController *player = new CPlayerController(playerType);
	this->m_Player = player;

	//create enemy type,add by wolfwind 2011.3.14
	char enemy1='0',enemy2='0';
	int i;
	for(i=1;i<=3;i++)
	{
		if (playerType!='0'+i)
		{
			if (enemy1=='0')
			{
				enemy1='0'+i;
			}
			else
				enemy2='0'+i;
		}
	}

	CEnemyController * Enemy1 = new CEnemyController(this,enemy1);
	this->m_vecForces.push_back(Enemy1);

	CEnemyController * Enemy2 = new CEnemyController(this,enemy2);
	this->m_vecForces.push_back(Enemy2);

	CEnemyController * MiddleRace=new CEnemyController(this,'0');
	this->m_vecForces.push_back(MiddleRace);

	//根据当前level的xml数据创建gas
	XMLgas=XMLlevel->FirstChildElement("gases");
	XMLgas=XMLgas->FirstChildElement("gas");
	while(XMLgas)
	{
		String gasName=XMLgas->Attribute("name");
		String gasType=XMLgas->Attribute("type");
		int gasNum=StringConverter::parseInt(XMLgas->Attribute("num"));
		String belong=XMLgas->Attribute("belong");
		CGas* gas;
		if (gasType=="player")
		{
			gas=new CGas(playerType,gasNum,player);
		}
		else if (gasType=="enemy1")
		{
			gas=new CGas(enemy1,gasNum,Enemy1);
		}
		else if (gasType=="enemy2")
		{
			gas=new CGas(enemy2,gasNum,Enemy2);
		}
		else if (gasType=="middle")
		{
			gas=new CGas('0',gasNum,MiddleRace);
		}
		gas->SetBelongedIsland(this->GetIslandByName(belong));
		XMLgas=XMLgas->NextSiblingElement("gas");
	}
	StartAnim();
}

CWorldMap::~CWorldMap(void)
{
	for (std::vector<CIsland*>::iterator iter = this->m_vecIslands.begin();
		iter != this->m_vecIslands.end();
		++iter){
			delete (*iter);
	} //for
	for (std::vector<CForceController*>::iterator iter = this->m_vecForces.begin();
		iter != this->m_vecForces.end();
		++iter){
			delete (*iter);
	} //for
	this->m_vecForces.clear();
	this->m_vecIslands.clear();
	DeleteCriticalSection(&m_cs);
}
int CWorldMap::GetRaceStrength(char racetype)
{
	assert(racetype>='0' && racetype<='3');
	return CWorldMap::s_RaceAttributes[racetype-'0'][STRENGTH];
}

int CWorldMap::GetRaceMoveSpeed(char racetype)
{
	assert(racetype>='0' && racetype<='3');
	return CWorldMap::s_RaceAttributes[racetype-'0'][MOVESPEED];
}

int CWorldMap::GetRaceBreedSpeed(char racetype)
{
	assert(racetype>='0' && racetype<='3');
	return CWorldMap::s_RaceAttributes[racetype-'0'][BREEDSPEED];
} 

unsigned int CWorldMap::GetMaxSelectLimit(void)
{
	return CWorldMap::s_MaxSelect;
}

Ogre::SceneManager* CWorldMap::GetSceneMgr(void)
{
	return CWorldMap::s_SceneMgr;
}

int CWorldMap::Update(double timeSinceLastFrame,bool LButtonDown)
{
	EnterCriticalSection(&m_cs);



	//遍历所有岛屿，更新所有岛屿，取消画实圈
	for (std::vector<CIsland*>::iterator iter=this->m_vecIslands.begin();//add by wolfwind 2011.3.27
		iter !=this->m_vecIslands.end();++iter)
	{
		(*iter)->Update();
		(*iter)->CircleDisable();
	}

	

	if (this->m_Player!=NULL){

		//更新动画
		if (mAnimState->hasEnded())
		{
			mAnimState->setEnabled(false);
			if (bCamReset==false)
			{
				Ogre::Camera* cam=s_SceneMgr->getCamera("PlayerCam");
				cam->setAutoTracking(false);
				Ogre::SceneNode* camNode=s_SceneMgr->getSceneNode("CamAnimNode");
				camNode->detachObject(cam);
				bCamReset=true;
			}
		}
		else if(mAnimState->getEnabled()==true)
		{
			mAnimState->addTime(timeSinceLastFrame);
			return 0;
		}

		//更新mPlayer
		this->m_Player->Update(timeSinceLastFrame);

		//更新EnemyController
		for (std::vector<CForceController*>::iterator iter = this->m_vecForces.begin();
			iter != this->m_vecForces.end();
			){
				(*iter)->Update(timeSinceLastFrame);
				if ( (*iter)->GetCurrGasList().empty()){
					delete (*iter);
					iter = this->m_vecForces.erase(iter);
				} else {
					++iter;
				} //if-else
		} //for

		//取消所有画线
		ClearAllLines();

		SetCircleIsland();//选中岛屿画圈

		SetMarkIsland(LButtonDown);//岛屿画虚圈

	} //if
	
	LeaveCriticalSection(&m_cs);
	if (this->m_Player!=NULL && this->m_Player->GetCurrGasList().empty()){
	    s_LvMgr->SetGameState(-1);//电脑胜
	} //if
	if (this->m_Player!=NULL && this->m_vecForces.size()<2){
		if(this->m_vecForces.empty()){
			PlaySound("sound\\start.wav", NULL, SND_FILENAME | SND_ASYNC);
			s_LvMgr->SetGameState(1);//玩家胜
		}else if (this->m_vecForces[0]->GetRaceType()=='0'){
			PlaySound("sound\\start.wav", NULL, SND_FILENAME | SND_ASYNC);
			s_LvMgr->SetGameState(1);//玩家胜
		}
	} //if
	return 0;
}

void CWorldMap::OnMousePressed(Ogre::Entity* entity)
{
	if (this->m_Player==NULL){
		return;
	} //if
	EnterCriticalSection(&m_cs);
	if (entity == NULL) { 
		dynamic_cast<CPlayerController*>(this->m_Player)->OnMousePressed(NULL);
	} else {
		dynamic_cast<CPlayerController*>(this->m_Player)->OnMousePressed(CIsland::GetMappedIsland(entity));
	} //if-else
	LeaveCriticalSection(&m_cs);
	return;
}

void CWorldMap::OnMouseReleased(Ogre::Entity* entity)
{
	if (this->m_Player==NULL){
		return;
	} //if	
	EnterCriticalSection(&m_cs);
	if (entity == NULL) { 
		dynamic_cast<CPlayerController*>(this->m_Player)->OnMouseReleased(NULL);
	} else {
		dynamic_cast<CPlayerController*>(this->m_Player)->OnMouseReleased(CIsland::GetMappedIsland(entity));
	} //if-else
	LeaveCriticalSection(&m_cs);
	return;
}

void CWorldMap::OnMouseMoved(bool LButtonDown,Ogre::Entity* entity)
{
	if (this->m_Player==NULL)
	{
		return;
	}
	EnterCriticalSection(&m_cs);

	if (entity == NULL) { 
		dynamic_cast<CPlayerController*>(this->m_Player)->OnMouseMoved(LButtonDown,NULL);
	} else {
		dynamic_cast<CPlayerController*>(this->m_Player)->OnMouseMoved(LButtonDown,CIsland::GetMappedIsland(entity));
	} //if-else

	LeaveCriticalSection(&m_cs);
	return;
}

int CWorldMap::GetMaxGasAmount(char islandtype)
{
	return s_iMaxAmount[islandtype-'a'];
}

void CWorldMap::ClearScene(void)
{
	for (std::vector<CIsland*>::iterator iter = this->m_vecIslands.begin();
		iter != this->m_vecIslands.end();
		++iter){
			delete (*iter);
	} //for
	for (std::vector<CForceController*>::iterator iter = this->m_vecForces.begin();
		iter != this->m_vecForces.end();
		++iter){
			delete (*iter);
	} //for
	this->m_vecForces.clear();
	this->m_vecIslands.clear();
	delete m_Player;//add by wolfwind 
	m_Player=NULL;
	//pEarthNode->removeAndDestroyAllChildren();
	//pEarthNode->setVisible(false);
	s_SceneMgr->clearScene();
	s_SceneMgr->destroyAllParticleSystems();

	Ogre::OverlayManager& overlayManager =Ogre:: OverlayManager::getSingleton();
	overlayManager.destroyOverlayElement("PanelName");
	overlayManager.destroyOverlayElement("TextAreaName");
	overlayManager.destroyAllOverlayElements(true);
	overlayManager.destroyAll();


}

//add by wolfwind 2011.3.16
CIsland* CWorldMap::GetIslandByName(Ogre::String name)
{
	for (std::vector<CIsland*>::iterator it=this->m_vecIslands.begin();
		it!=this->m_vecIslands.end();it++)
	{
		if ((*it)->GetIslandName()==name)
		{
			return (*it);
		}
	}
	return NULL;
}

void CWorldMap::StartAnim()
{
	using namespace Ogre;
	Camera* cam=s_SceneMgr->getCamera("PlayerCam");
	//cam->getParentSceneNode()->detachObject(cam);
	SceneNode* camNode=s_SceneMgr->getRootSceneNode()->createChildSceneNode("CamAnimNode",Vector3(0,0,0));
	camNode->attachObject(cam);
	Animation* CamAnim=s_SceneMgr->createAnimation("CameraTrack",10);
	CamAnim->setInterpolationMode(Animation::IM_SPLINE);
	NodeAnimationTrack* track=CamAnim->createNodeTrack(0,camNode);

	track->createNodeKeyFrame(0)->setTranslate(Vector3(200, 0, 0));
	track->createNodeKeyFrame(2.5)->setTranslate(Vector3(0, -50, 100));
	track->createNodeKeyFrame(5)->setTranslate(Vector3(-300, 100, 0));
	track->createNodeKeyFrame(7.5)->setTranslate(Vector3(0, 200, -300));
	track->createNodeKeyFrame(10)->setTranslate(Vector3(200, 0, 0));

	mAnimState=s_SceneMgr->createAnimationState("CameraTrack");
	cam->setAutoTracking(true,s_SceneMgr->getSceneNode("Earth"));
	mAnimState->setEnabled(true);
	mAnimState->setLoop(false);
	bCamReset=false;
}

void CWorldMap::LoseAnim()
{
	using namespace Ogre;
	Camera* cam=s_SceneMgr->getCamera("PlayerCam");
	SceneNode* camNode=s_SceneMgr->getSceneNode("CamAnimNode");
	camNode->attachObject(cam);
	cam->setAutoTracking(true,s_SceneMgr->getSceneNode("Earth"));
	mAnimState->setEnabled(true);
	mAnimState->setLoop(true);
	bCamReset=false;
}

int CWorldMap::GetHardLevel(void) const
{
	return this->m_HardLevel;
}

void CWorldMap::WinAnim()
{
	Ogre::ParticleSystem* ps;
	if (mPlayerRace == '1'){
		ps = s_SceneMgr->createParticleSystem("WinPs","CleanAir");
	} //if
	else if (mPlayerRace=='2'){
		ps = s_SceneMgr->createParticleSystem("WinPs","CO2Air");
	} //if
	else if (mPlayerRace=='3')
	{
		ps = s_SceneMgr->createParticleSystem("WinPs","DustAir");
	}
	ps->setKeepParticlesInLocalSpace(true);
	Ogre::SceneNode* psNode=s_SceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0,0,0));
	psNode->createChildSceneNode()->attachObject(ps);
	psNode->scale(15,15,15);
	psNode->setPosition(0,-300,0);
	LoseAnim();
}

//void CWorldMap::CongAnim()
//{
//	using namespace Ogre;
//	LoseAnim();
//	mAnimState->setLoop(false);
//	ParticleSystem* ps;
//	SceneNode* mFountainPivot;
//
//	// create some nice fireworks and place it at the origin
//	ps = s_SceneMgr->createParticleSystem("Fireworks", "Examples/Fireworks");
//	s_SceneMgr->getRootSceneNode()->attachObject(ps);
//
//	// create a green nimbus around the ogre head
//	ps = s_SceneMgr->createParticleSystem("Nimbus", "Examples/GreenyNimbus");
//	s_SceneMgr->getRootSceneNode()->attachObject(ps);
//
//	// create shared pivot node for spinning the fountains
//	mFountainPivot = s_SceneMgr->getRootSceneNode()->createChildSceneNode();
//
//	ps = s_SceneMgr->createParticleSystem("Fountain1", "Examples/PurpleFountain");  // create fountain 1
//	// attach the fountain to a child node of the pivot at a distance and angle
//	mFountainPivot->createChildSceneNode(Vector3(200, -100, 0), Quaternion(Degree(20), Vector3::UNIT_Z))->attachObject(ps);
//
//	ps = s_SceneMgr->createParticleSystem("Fountain2", "Examples/PurpleFountain");  // create fountain 2
//	// attach the fountain to a child node of the pivot at a distance and angle
//	mFountainPivot->createChildSceneNode(Vector3(-200, -100, 0), Quaternion(Degree(-20), Vector3::UNIT_Z))->attachObject(ps);
//}

void CWorldMap::DisableAnim()
{
	if (this->m_Player!=NULL )// 
	{
		mAnimState->setEnabled(false);
		Ogre::Camera* cam=s_SceneMgr->getCamera("PlayerCam");
		cam->setAutoTracking(false);
		Ogre::SceneNode* camNode=s_SceneMgr->getSceneNode("CamAnimNode");
		camNode->detachObject(cam);
		//s_SceneMgr->destroySceneNode(camNode);
		//s_SceneMgr->getSceneNode("RootCamNode")->attachObject(cam);
		bCamReset=true;
		cam->setPosition(450,0,0);
	}
}

//岛屿画虚圈
void CWorldMap::SetMarkIsland(bool LButtonDown)
{
	CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
	Ogre::Viewport* vp=s_SceneMgr->getCurrentViewport();
	Ogre::Ray mouseRay = s_SceneMgr->getCamera("PlayerCam")->getCameraToViewportRay(mousePos.d_x/float(vp->getActualWidth()), mousePos.d_y/float(vp->getActualHeight()));


	rsq->setRay(mouseRay);
	rsq->setSortByDistance(true);
	rsq->setQueryMask(ISLAND_MASK);
	// Execute query
	Ogre::RaySceneQueryResult &result = rsq->execute();
	Ogre::RaySceneQueryResult::iterator iter= result.begin();

	// Get results, create a node/entity on the position
	bool selectflag=false;
	//for ( iter; iter != result.end(); iter++ )
	//{
		if (iter!=result.end() && iter->movable && iter->movable->getName().substr(0,2)=="is"
			&& iter->movable->getParentSceneNode()->getPosition().distance(s_SceneMgr->getCamera("PlayerCam")->getPosition())<480)
		{
			Ogre::Entity* islandent=dynamic_cast<Ogre::Entity*>(iter->movable);
			CIsland* island=CIsland::GetMappedIsland(islandent);
			if (island->GetDomGas()->GetRace()==this->mPlayerRace)//如果同类
			{
				if (LButtonDown==false)//左键没按下，画虚圈
				{
					//if (island->GetMarkColor()==2)//原先画了红圈
					//{
					//	island->MarkDisable();
					//}
					//if (island->GetMarkColor()==0)//原先未画圈
					//{
					//	island->MarkEnable(true);
					//}
					if (lastIsland!=NULL)
					{
						lastIsland->MarkDisable();
					}
					
					island->MarkEnable(true);
					lastIsland=island;
					selectflag=true;
				}
				else
				{
					island->CircleEnable(true);//左键按下，画实圈
				}

				//画绿线
				std::set<CGas*> gasSet=dynamic_cast<CPlayerController*>(m_Player)->GetSelectedGas();
				if (gasSet.size()>1)
				{
					for (std::set<CGas*>::iterator iter = gasSet.begin();
						iter != gasSet.end();
						++iter){
							(*iter)->GetBelongedIsland()->DrawLineTo(island,true);
					} 

				}
				/*else
				{
					ClearAllLines();
				}*/

				
			}
			else//如果异类
			{
				if(LButtonDown==false)//左键没按下，画虚圈
				{
					//if (island->GetMarkColor()==1)//原先画了绿圈
					//{
					//	island->MarkDisable();
					//}
					//if (island->GetMarkColor()==0)//原先未画圈
					//{
					//	island->MarkEnable(false);
					//}
					if (lastIsland!=NULL)
					{
						lastIsland->MarkDisable();
					}
					island->MarkEnable(false);
					lastIsland=island;
					selectflag=true;
				}
				else
				{
					island->CircleEnable(false);//左键按下，画实圈
				}

				//画红线
				std::set<CGas*> gasSet=dynamic_cast<CPlayerController*>(m_Player)->GetSelectedGas();
				if (!gasSet.empty())
				{
					for (std::set<CGas*>::iterator iter = gasSet.begin();
						iter != gasSet.end();
						++iter){
							(*iter)->GetBelongedIsland()->DrawLineTo(island,false);
					} 

				}
				/*else
				{
					ClearAllLines();
				}*/
				
			}
			
			//break;
		} // if
	//} // for

	if(selectflag==false && lastIsland!=NULL)
	{
		lastIsland->MarkDisable();
		lastIsland=NULL;
		//ClearAllLines();
	}
}

//选中列表中的岛屿画实圈
void CWorldMap::SetCircleIsland()
{
	std::set<CGas*> gasSet=dynamic_cast<CPlayerController*>(m_Player)->GetSelectedGas();
	if (gasSet.empty())
	{
		return;
	}
	CIsland* island;
	for (std::set<CGas*>::iterator iter = gasSet.begin();
		iter != gasSet.end();
		++iter){
			island=(*iter)->GetBelongedIsland();
			if ((*iter)->GetCurrNum()>0 && (*iter)->GetRace()==mPlayerRace)
			{
				island->CircleEnable(true);
			}
			else
			{
				island->CircleEnable(false);
			}
	} 
}

//清除所有画线
void CWorldMap::ClearAllLines()
{
	Ogre::SceneNode* linenode=s_SceneMgr->getSceneNode("LineNode");
	if (linenode->numChildren()!=0)
	{
		DestroyAllAttachedMovableObjects(linenode);
		linenode->removeAndDestroyAllChildren();
	}
}

//清空某节点的所有内容
void CWorldMap::DestroyAllAttachedMovableObjects( Ogre::SceneNode* i_pSceneNode )
{   
	using namespace Ogre;
	if ( !i_pSceneNode )   
	{     
		return;   
	}   // Destroy all the attached objects  
	SceneNode::ObjectIterator itObject = i_pSceneNode->getAttachedObjectIterator();  
	while ( itObject.hasMoreElements() )   
	{    
		MovableObject* pObject = static_cast<MovableObject*>(itObject.getNext()); 
		i_pSceneNode->getCreator()->destroyMovableObject( pObject );  
	}   // Recurse to child SceneNodes 
	SceneNode::ChildNodeIterator itChild = i_pSceneNode->getChildIterator();  
	while ( itChild.hasMoreElements() ) 
	{      
		SceneNode* pChildNode = static_cast<SceneNode*>(itChild.getNext());  
		DestroyAllAttachedMovableObjects( pChildNode );  
	}
}
