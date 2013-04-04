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
	if (island == NULL) { //��갴�´����κε���
		this->m_SelectedGas.clear();
		return;
	} //if
	PlaySound("sound\\ok.wav", NULL, SND_FILENAME | SND_ASYNC);
	if (island->GetDomGas()->GetRace() == this->GetRaceType()) {//��������
		if (this->m_bMovedAfterCorresponedingMousePressEvent) {//��Ӧ����갴�²���֮���ƶ������
			this->SetTargetOfSelectedGas(island);
		} else { //��Ӧ����갴�²���֮��δ�ƶ������
			//PlaySound("sound\\menu.wav", NULL, SND_FILENAME | SND_ASYNC);
			if (this->m_SelectedGas.size() == CWorldMap::GetMaxSelectLimit() //�Ѿ�ѡ�����ĵ���
				|| !this->m_SelectedGas.insert( island->GetDomGas() ).second ){//����ѡ���б�ʧ�ܣ��Ѿ���֮ǰ��ѡ�У�
					this->SetTargetOfSelectedGas(island);
			} //if
		} //if-else �Լ�������Ĳ�ͬ����
	} else { //�з�����		
		this->SetTargetOfSelectedGas(island);
	}
}

void CPlayerController::SetTargetOfSelectedGas(CIsland *island)
{
	assert(island!=NULL);
	for (std::set<CGas*>::iterator iter = this->m_SelectedGas.begin();
		iter != this->m_SelectedGas.end();
		++iter) {
			(*iter)->SetTargetIsland(island); //����
	} //for
	this->m_SelectedGas.clear();
}

void CPlayerController::OnMouseMoved(bool LButtonDown,CIsland* island)//boolֵ�����壬֮���޸�
{
	this->m_bMovedAfterCorresponedingMousePressEvent = true;
	if (island == NULL //�޵���
		|| island->GetDomGas()->GetRace() != this->GetRaceType() //�з�����
		|| this->m_SelectedGas.size()==CWorldMap::GetMaxSelectLimit()) { //�ﵽ���ѡ�еĵ�����
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