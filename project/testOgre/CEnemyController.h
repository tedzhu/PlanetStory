#pragma once
#include "stdafx.h"

#define MAXISLANDS 100

class CEnemyController :
	public CForceController
{
public:
	CEnemyController(CWorldMap* worldmap,char RaceType);
	virtual ~CEnemyController(void);
	virtual void Update(double timeSinceLastFrame);

private:
	CWorldMap* world;
	double time;
	double lastActionTime;
	std::vector<CIsland*>& islands;
	std::vector<CForceController*>& forces;	
	double theta[MAXISLANDS][MAXISLANDS];
	int nislands;
	double r;

	int level;	
	static double level2interval[10];
	double interval;
	
	int mine[MAXISLANDS];
	int nmine;

	void ai0();
	void ai1();
	void ai2();
	void giveCommand(int src,int dst);

	void calculateMyIslands();//true if i own all or i own zero, need to exit
	bool finished;

/****************************************************************************/
/**** below here similar to CPlayerController.cpp.  Consider reconstruct. ***/
/****************************************************************************/
};
