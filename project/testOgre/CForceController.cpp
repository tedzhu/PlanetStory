#include "stdafx.h"

CForceController::CForceController(void)
{

}

CForceController::~CForceController(void)
{	
	for (std::list<CGas*>::iterator iter = this->m_listGasControlled.begin();
        iter != this->m_listGasControlled.end();
        ++iter){
            delete (*iter);
    } //for
    this->m_listGasControlled.clear();
}

void CForceController::AddGas(CGas* gas)
{
    //assert(gas!=NULL && gas->GetRace()==this->m_RaceType);
    this->m_listGasControlled.push_back(gas);
    return;
}

std::list<CGas*>& CForceController::GetCurrGasList(void)
{
	return this->m_listGasControlled;
}

void CForceController::DelDeadGas(CGas * gas)
{
    //assert(gas!=NULL && gas->GetRace()==this->m_RaceType);
    delete gas;
    this->m_listGasControlled.remove(gas);
    return;
}

void CForceController::Update(double timeSinceLastFrame)
{
    for(std::list<CGas*>::iterator iter = this->m_listGasControlled.begin();
        iter != this->m_listGasControlled.end();
        ){
            (*(iter++))->Update(timeSinceLastFrame);
    } //for
    return;
} 

char CForceController::GetRaceType(void)
{
	return this->m_RaceType;
}