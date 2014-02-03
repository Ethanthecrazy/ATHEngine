#ifndef ATHSTACKSYSTEM_H_
#define ATHSTACKSYSTEM_H_
#include "IBaseState.h"
#include <list>

using namespace std;

class ATHStackSystem
{
	list<IBaseState*> m_lStates;
	
	ATHStackSystem();
	~ATHStackSystem();
	ATHStackSystem(const ATHStackSystem& obj);
	ATHStackSystem& operator=(const ATHStackSystem& obj);

public:
	
	void PushState(IBaseState* _state, int _stackPosition = 0);
	void PopState(int _stackPosition = 0);
	void PopState(char _stateName);
	void PushStateOnTop(IBaseState* _state);
	void PushStateOnBottom(IBaseState* _state);
	void PopTopState();
	void ClearAllStates();
	void Update(float _fDT);

};

#endif /*ATHSTACKSYSTEM_H_*/