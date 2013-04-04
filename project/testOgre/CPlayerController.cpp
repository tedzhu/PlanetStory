#include "stdafx.h"

CPlayerController::CPlayerController(char RaceType)
{
	m_RaceType = RaceType;
	m_bMovedAfterCorresponedingMousePressEvent = false;
}

CPlayerController::~CPlayerController(void)
{
}


void CPlayerController::OnMousePressed(CIsland *island)
{
	this->m_bMovedAfterCorresponedingMousePressEvent = false;
    return;
}

void CPlayerController::OnMouseReleased(CIsland *island)
{
	if (island == NULL) { //鼠标按下处无任何岛屿
		this->m_SelectedGas.clear();
		return;
	} //if
	PlaySound("sound\\ok.wav", NULL, SND_FILENAME | SND_ASYNC);
	if (island->GetDomGas()->GetRace() == this->GetRaceType()) {//己方岛屿
		if (this->m_bMovedAfterCorresponedingMousePressEvent) {//对应的鼠标按下操作之后移动过鼠标
			this->SetTargetOfSelectedGas(island);
		} else { //对应的鼠标按下操作之后未移动过鼠标
			//PlaySound("sound\\menu.wav", NULL, SND_FILENAME | SND_ASYNC);
			if (this->m_SelectedGas.size() == CWorldMap::GetMaxSelectLimit() //已经选了最多的岛屿
				|| !this->m_SelectedGas.insert( island->GetDomGas() ).second ){//加入选中列表失败（已经在之前被选中）
					this->SetTargetOfSelectedGas(island);
			} //if
		} //if-else 对己方岛屿的不同操作
	} else { //敌方岛屿		
		this->SetTargetOfSelectedGas(island);
	}
}

void CPlayerController::SetTargetOfSelectedGas(CIsland *island)
{
	assert(island!=NULL);
	for (std::set<CGas*>::iterator iter = this->m_SelectedGas.begin();
		iter != this->m_SelectedGas.end();
		++iter) {
			(*iter)->SetTargetIsland(island); //增兵
	} //for
	this->m_SelectedGas.clear();
}

void CPlayerController::OnMouseMoved(bool LButtonDown,CIsland* island)//bool值无意义，之后修改
{
	this->m_bMovedAfterCorresponedingMousePressEvent = true;
	if (island == NULL //无岛屿
		|| island->GetDomGas()->GetRace() != this->GetRaceType() //敌方岛屿
		|| this->m_SelectedGas.size()==CWorldMap::GetMaxSelectLimit()) { //达到最大选中的岛屿数
		return;
	} //if
	//PlaySound("sound\\menu.wav", NULL, SND_FILENAME | SND_ASYNC);
	this->m_SelectedGas.insert(island->GetDomGas());
	return;
}
std::set<CGas*>& CPlayerController::GetSelectedGas(void)
{
	return this->m_SelectedGas;
}