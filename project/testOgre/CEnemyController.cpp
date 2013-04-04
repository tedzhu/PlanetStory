#include "stdafx.h"

#define PI 3.1415927

CEnemyController::CEnemyController(CWorldMap *worldmap, char RaceType)
	:world(worldmap)
	,islands(worldmap->GetIslands())
	,forces(worldmap->GetForces())
{	
	this->m_RaceType=RaceType;
	this->finished=false;
	this->level=worldmap->GetHardLevel();
	this->interval=0;
	this->time=0;
	this->lastActionTime=-10000;	

	nislands=islands.size();	
	r=world->GetEarthRadius();
	for(int i=0;i<nislands;i++){
		theta[i][i]=0;
		for(int j=0;j<i;j++){
			Ogre::Vector3 p1=islands.at(i)->GetPosition();
			Ogre::Vector3 p2=islands.at(j)->GetPosition();
			theta[i][j]=theta[j][i]=acos(p1.dotProduct(p2)/(r*r));
		}
	}
}

void CEnemyController::giveCommand(int src, int dst)
{
	islands.at(src)->GetDomGas()->SetTargetIsland(islands.at(dst));
}

void CEnemyController::calculateMyIslands()
{
	nmine=0;
	for(int i=0;i<nislands;i++){
		char type=islands.at(i)->GetDomGas()->GetRace();
		if(type==m_RaceType){
			mine[nmine++]=i;
		}		
	}
	finished= nmine==0 || nislands==nmine;
}

/*** ai0:random (my) source, random destination, plain and simple ***/
void CEnemyController::ai0()
{	
	if(time-lastActionTime<interval)
		return;

	lastActionTime=time;
	interval=rand()%30+1;	

	int src=mine[rand()%nmine];
	int dst;
	while(1){
		dst=rand()%nislands;
		if(dst!=src)break;
	}

	giveCommand(src, dst);
}

/*** ai1:taking 4 factors into consideration, one troop ***/
void CEnemyController::ai1()
{
	if(time-lastActionTime<interval)
		return;

	lastActionTime=time;
	interval=rand()%3+5;

	double maxscore=-10000;
	int src,dst;
	for(int i=0;i<nislands;i++){		
		double score=0;
		CIsland *island=islands.at(i);

		//Race of the island
		char type=islands.at(i)->GetDomGas()->GetRace();
		if(type==m_RaceType){
			score-=100;
		}else if(type!=0){		
			score+=100;
		}else{
		}
		
		//Growing speed of the island
		score+=island->GetGrowBonus()*100;

		//Gas amount of the island
		score-=island->GetDomGas()->GetStrength();
		
		//Distance from our islands		
		for(int t=0;t<nmine;t++){
			int j=mine[t];
			score+=(1-theta[i][j]/PI)*r;
		}

		if(score>maxscore){
			maxscore=score;
			dst=i;
		}
	}

	double mintheta=PI;	
	for(int t=0;t<nmine;t++){
		int j=mine[t];
		if(theta[dst][j]<mintheta){
			mintheta=theta[dst][j];
			src=j;
		}
	}

	giveCommand(src, dst);
}

void CEnemyController::ai2()
{
	if(time-lastActionTime<interval)
		return;

	lastActionTime=time;
	interval=rand()%2+2;

	double maxscore=-10000;
	int src,dst;
	for(int i=0;i<nislands;i++){		
		double score=0;
		CIsland *island=islands.at(i);

		//Race of the island
		char type=islands.at(i)->GetDomGas()->GetRace();
		if(type==m_RaceType){
			score=-10000;
		}else if(type!=0){		
			score+=100;
		}else{
		}
		
		//Growing speed of the island
		score+=island->GetGrowBonus()*100;

		//Gas amount of the island
		score-=island->GetDomGas()->GetStrength()*10;
		
		//Distance from our islands		
		for(int t=0;t<nmine;t++){
			int j=mine[t];
			score+=(1-theta[i][j]/PI)*r;
		}

		if(score>maxscore){
			maxscore=score;
			dst=i;
		}
	}

	double mintheta=PI;	
	for(int t=0;t<nmine;t++){
		int j=mine[t];
		src=j;
		giveCommand(src, dst);
	}
	
}

void CEnemyController::Update(double timeSinceLastFrame)
{
	CForceController::Update(timeSinceLastFrame);
	time+=timeSinceLastFrame;

	calculateMyIslands();
	if(finished)return;

	switch(level){
		case 0:ai0();break;
		case 1:ai1();break;
		case 2:ai2();break;
	}
    return;
}

CEnemyController::~CEnemyController()
{
}