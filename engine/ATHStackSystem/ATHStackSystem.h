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
	


};

#endif /*ATHSTACKSYSTEM_H_*/