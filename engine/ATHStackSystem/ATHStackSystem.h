#ifndef ATHSTACKSYSTEM_H_
#define ATHSTACKSYSTEM_H_
#include "IBaseState.h"
#include <list>

using namespace std;

class ATHStackSystem
{
	list<IBaseState*> m_lStates;

public:
	ATHStackSystem();
	~ATHStackSystem();


};

#endif /*ATHSTACKSYSTEM_H_*/