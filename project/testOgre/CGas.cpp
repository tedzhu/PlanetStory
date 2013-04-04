#include "stdafx.h"


static const double PRECISION=1e-10;

Ogre::Vector3 VectorXVector(const Ogre::Vector3&  u,const Ogre::Vector3& v)
{
    return Ogre::Vector3(u.y*v.z-v.y*u.z,
        u.z*v.x-v.z*u.x,
        u.x*v.y-v.x*u.y);
}
CGas::CGas(char race, int num,CForceController* force)
:m_Race(race)
,m_Num(num)
,m_BelongedForce(force)
,m_BelongedIsland(NULL)
,m_TargetIsland(NULL)
,mGasAnimState(NULL)
{
    //TODO: 根据新的模型名称修改创建Entity和绑定Node节点的相关参数
	Ogre::SceneNode *pEarthNode =  CWorldMap::GetSceneMgr()->getSceneNode("Earth");
	Ogre::SceneNode *pGasNode = pEarthNode->createChildSceneNode( Ogre::Vector3(0,0,0) );
	this->m_Entity = CWorldMap::GetSceneMgr()->createEntity("smallcube.mesh");
	m_Entity->setQueryFlags(GAS_MASK);
	this->m_Entity->setVisible(false);
	pGasNode->attachObject(m_Entity);

	Ogre::ParticleSystem* ps;
	psNode=pGasNode->createChildSceneNode();
	if (this->m_Race == '1'){
		//
		
		ps = CWorldMap::GetSceneMgr()->createParticleSystem(pGasNode->getName()+"ps","CleanAir");
		ps->setKeepParticlesInLocalSpace(true);
	/*	
		
		psNode->scale(0.01,0.01,0.01);
		psNode->translate(0,0.5,0);*/
		
	} //if
	else if (this->m_Race=='2'){
		ps = CWorldMap::GetSceneMgr()->createParticleSystem(pGasNode->getName()+"ps","CO2Air");
		ps->setKeepParticlesInLocalSpace(true);
		//this->m_Entity = CWorldMap::GetSceneMgr()->createEntity("robot.mesh");//edit by wolfwind 2011.3.14
	} //if
	else if (this->m_Race=='3')
	{
		ps = CWorldMap::GetSceneMgr()->createParticleSystem(pGasNode->getName()+"ps","DustAir");
		ps->setKeepParticlesInLocalSpace(true);
		//this->m_Entity = CWorldMap::GetSceneMgr()->createEntity("ninja.mesh");//edit by wolfwind 2011.3.14
	}
	else if (this->m_Race=='0')
	{
		ps = CWorldMap::GetSceneMgr()->createParticleSystem(pGasNode->getName()+"ps");
	}
	Ogre::Real f=(num/150)*3.0+0.5;//scale 0-150->0.5-3.5
	ps->setQueryFlags(GAS_MASK);
	psNode->attachObject(ps);
	psNode->translate(0,5,0);
	psNode->scale(f,f,f);
	curScale=f;

	m_BelongedForce->AddGas(this);

	//pGasNode->showBoundingBox(true);
    return;
}

CGas::~CGas(void)
{
    if (this->m_Entity!=NULL){
        Ogre::SceneNode *pSceneNode = this->m_Entity->getParentSceneNode();
        pSceneNode->detachAllObjects();
        CWorldMap::GetSceneMgr()->destroyEntity(this->m_Entity);
        CWorldMap::GetSceneMgr()->destroySceneNode(pSceneNode);
        this->m_Entity = NULL;
    } //if
    return;
}

void CGas::SetTargetIsland(CIsland *targetisland)
{
    if (this->m_Num<2 || targetisland == this->m_BelongedIsland){
        return;
    } //if

    this->m_Num /=2;
    CGas *gas = new CGas(m_Race,static_cast<int>(m_Num),m_BelongedForce);
    gas->m_Entity->getParentSceneNode()->setPosition(this->GetCurrPos());
    gas->m_BelongedIsland = this->m_BelongedIsland;
    gas->m_TargetIsland = targetisland;

    Ogre::Vector3 p1 = gas->GetCurrPos();
    Ogre::Vector3 p2 = gas->m_TargetIsland->GetPosition();
    gas->m_e1 = p1/CWorldMap::GetEarthRadius();
    gas->m_e2 = VectorXVector( VectorXVector(p1,p2),p1);
	if (gas->m_e2==Ogre::Vector3(0,0,0))
	{
		Ogre::Vector3 tmpVec;
		srand((unsigned)time(0));
		do 
		{
			tmpVec.x=rand()%10;
			tmpVec.y=rand()%10;
			tmpVec.z=rand()%10;
			gas->m_e2=p2.crossProduct(tmpVec);
		} while (gas->m_e2==Ogre::Vector3(0,0,0));
		gas->m_e2.normalise();
	}
    gas->m_e2 = gas->m_e2/sqrt(gas->m_e2.x*gas->m_e2.x+gas->m_e2.y*gas->m_e2.y+gas->m_e2.z*gas->m_e2.z);
    if (gas->m_e2.x*p2.x+gas->m_e2.y*p2.y+gas->m_e2.z*p2.z<0){
        gas->m_e2 *=-1;
    } //if
    gas->m_angle = 0;
	gas->MoveAnim();
}


void CGas::SetBelongedIsland(CIsland* island)
{
    this->m_BelongedIsland = island;
    island->SetOccupant(this);
    Ogre::Vector3 pos = island->GetPosition();
    this->m_Entity->getParentSceneNode()->setPosition(pos);
    Ogre::Vector3 src = this->m_Entity->getParentSceneNode()->getOrientation()*Ogre::Vector3::UNIT_X;
    Ogre::Quaternion quat = src.getRotationTo(pos);
	this->m_Entity->getParentSceneNode()->rotate(quat,Ogre::Node::TS_WORLD);
    this->m_Entity->getParentSceneNode()->roll(Ogre::Degree(-90));
}

char CGas::GetRace(void)
{
    return this->m_Race;
}

int CGas::GetStrength(void)
{
    return CWorldMap::GetRaceStrength(this->m_Race);
}
int CGas::GetMoveSpeed(void)
{
    return CWorldMap::GetRaceMoveSpeed(this->m_Race);
}

int CGas::GetBreedSpeed(void)
{
    return CWorldMap::GetRaceBreedSpeed(this->m_Race);
}

double CGas::GetCurrNum(void)
{
    return this->m_Num;
}

Ogre::Vector3 CGas::GetCurrPos(void)
{
    return this->m_Entity->getParentSceneNode()->getPosition();
}

CIsland* CGas::GetTargetIsland(void)
{
    return this->m_TargetIsland;
}

CIsland* CGas::GetBelongedIsland(void)
{
    return this->m_BelongedIsland;
}

void CGas::SendDeathMsg(void)
{
	this->m_BelongedForce->DelDeadGas(this);
}

void CGas::Move(double timeSinceLastFrame)
{
    m_angle += timeSinceLastFrame*this->GetMoveSpeed()/100;
    Ogre::Vector3 vec = (this->m_e1*cos(m_angle)+this->m_e2*sin(m_angle))*CWorldMap::GetEarthRadius();
    this->m_Entity->getParentSceneNode()->setPosition(vec);
    Ogre::Vector3 src = this->m_Entity->getParentSceneNode()->getOrientation()*Ogre::Vector3::UNIT_X;
    Ogre::Quaternion quat = src.getRotationTo(vec);
	this->m_Entity->getParentSceneNode()->rotate(quat,Ogre::Node::TS_WORLD);
    this->m_Entity->getParentSceneNode()->roll(Ogre::Degree(-90));
}

void CGas::Breed(double timeSinceLast)
{
	if (this->m_Race == '0' || this->m_TargetIsland!=NULL){ //中立种族和移动中的不繁殖
		return;
	} //if
	if (this->m_Num > CWorldMap::GetMaxGasAmount(this->m_BelongedIsland->GetType()) ){
		this->m_Num -= 1;
		return;
	} //if
	this->m_Num += this->m_BelongedIsland->GetGrowBonus()*this->GetBreedSpeed()*timeSinceLast/15;
	return;
}

void CGas::DealWithConfict(CIsland *confictIsland)
{
	assert(confictIsland!=NULL);
	assert(this->m_TargetIsland!=NULL);
	if (this->m_BelongedIsland == confictIsland) {
		return;
	} 
	if (this->m_TargetIsland !=confictIsland){
		//TODO:避开此岛屿去别的地方走
	} else {
		CGas * gas = confictIsland->GetDomGas();
		
		//mGasAnimState->setEnabled(false);
		if (gas==NULL){
			confictIsland->SetOccupant(this);
			this->m_TargetIsland = NULL;
			this->SetBelongedIsland(confictIsland);
		} else if (gas->GetRace() == this->m_Race){
			this->Merge(gas);
		} else {
			this->Attack(gas);
		}  //if-elseif-else
	} //if-else
	return;
}

CIsland* CGas::ArrivedSomeIsland(void)
{
	//TODO: 碰撞检测是否自己计算不用它的包围盒？
    Ogre::AxisAlignedBox BoundingBox = this->m_Entity->getWorldBoundingBox(true);
    Ogre::DefaultAxisAlignedBoxSceneQuery sceneQuery(CWorldMap::GetSceneMgr());
    sceneQuery.setBox(BoundingBox);
    //sceneQuery.setQueryMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
    Ogre::SceneQueryResult& result = sceneQuery.RegionSceneQuery::execute();
    Ogre::SceneQueryResultMovableList::iterator iter = result.movables.begin();
    if (!result.movables.empty()){
        for(;iter!=result.movables.end();++iter){
            if ((*iter)->getMovableType()!="Entity"){
                continue;
            } //if
            CIsland *island =CIsland::GetMappedIsland(dynamic_cast<Ogre::Entity*>(*iter));
            if (island!=NULL){
                return island;
            } //if
        } //for
    } //if
	return NULL;
}

void CGas::Attack(CGas* defender)
{
	assert(defender!=NULL);
	double val = this->m_Num*this->GetStrength() - defender->m_Num*defender->GetStrength();
	if (val<0){
		this->SendDeathMsg();
		defender->m_Num = -1*val/defender->GetStrength();
	} else {
		this->SetBelongedIsland(defender->GetBelongedIsland());
		this->m_TargetIsland = NULL;
		defender->SendDeathMsg();
		this->m_Num = val/this->GetStrength();
	} //if-else
	return;
}

void CGas::Update(double timeSinceLast)
{
	//TODO:
    if (this->m_TargetIsland==NULL){
        this->Breed(timeSinceLast);
    } else {
        this->Move(timeSinceLast);
		if (mGasAnimState!=NULL && !(mGasAnimState->hasEnded()))
		{
			mGasAnimState->addTime(timeSinceLast);
		}
        CIsland *island = this->ArrivedSomeIsland();
        if (island!=NULL) {
            this->DealWithConfict(island);
        } //if
    } //if-else
}

void CGas::Merge(CGas *preislandgas)
{
	assert(this->m_Race==preislandgas->m_Race);
	assert(this->m_TargetIsland == preislandgas->m_BelongedIsland);

	preislandgas->m_Num += this->m_Num;
	this->SendDeathMsg();
}

void CGas::SetEntity(Ogre::Entity* entity)
{
	this->m_Entity = entity;
}

Ogre::Real CGas::GetCurScale()
{
	return curScale;
}

void CGas::SetCurScale(Ogre::Real sc)
{
	curScale=sc;
}
Ogre::Entity* CGas::GetEntity()
{
	return m_Entity;
}
void CGas::MoveAnim()
{
	using namespace Ogre;
	SceneNode* gasNode=psNode->getParentSceneNode();
	Animation* gasAnim= CWorldMap::GetSceneMgr()->createAnimation(m_Entity->getName()+"Track",1.6f);
	gasAnim->setInterpolationMode(Animation::IM_SPLINE);
	NodeAnimationTrack* track=gasAnim->createNodeTrack(0,psNode);

	track->createNodeKeyFrame(0)->setTranslate(Vector3(0, 0, 0));
	track->createNodeKeyFrame(0.4f)->setTranslate(Vector3(5, 3, -5));
	track->createNodeKeyFrame(1.2f)->setTranslate(Vector3(-5, -2, 5));
	track->createNodeKeyFrame(1.6f)->setTranslate(Vector3(0, 0, 0));

	mGasAnimState=CWorldMap::GetSceneMgr()->createAnimationState(m_Entity->getName()+"Track");
	mGasAnimState->setEnabled(true);
	mGasAnimState->setLoop(true);
}