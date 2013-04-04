#pragma once
#include "stdafx.h"

class CForceController
{
public:
	CForceController(void);
	virtual ~CForceController(void);
	virtual void DelDeadGas(CGas *);
	virtual  std::list<CGas*>& GetCurrGasList(void);
    virtual void AddGas(CGas*);
    virtual void Update(double timeSinceLastFrame);
	virtual char GetRaceType(void);
protected:
	char m_RaceType;
    std::list<CGas*> m_listGasControlled;
};
