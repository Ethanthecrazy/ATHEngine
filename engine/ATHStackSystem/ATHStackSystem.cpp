#include "ATHStackSystem.h"

ATHStackSystem::ATHStackSystem()
{
	ClearAllStates();
}

void ATHStackSystem::PushState(IBaseState* _state, int _stackPosition)
{
	unsigned int listSize = m_lStates.size();

	if (_stackPosition > listSize)
	{
		_stackPosition = listSize;
	}

	list<IBaseState*>::iterator iter = m_lStates.begin();
	for (unsigned int i = 0; i < _stackPosition; ++i)
	{
		if (_state->GetStateName() == (*iter)->GetStateName())
			return;
		iter++;
	}
	m_lStates.insert(iter, _state);
}

void ATHStackSystem::PopState(int _stackPosition)
{
	unsigned int listSize = m_lStates.size();

	if (_stackPosition > listSize)
	{
		_stackPosition = listSize;
	}

	list<IBaseState*>::iterator iter = m_lStates.begin();
	for (unsigned int i = 0; i < _stackPosition; ++i)
	{
		iter++;
	}
	m_lStates.erase(iter);
}

void ATHStackSystem::PopState(char _stateName)
{
	unsigned int listSize = m_lStates.size();

	list<IBaseState*>::iterator iter = m_lStates.begin();
	for (unsigned int i = 0; i < listSize; ++i)
	{
		if ((*iter)->GetStateName() == _stateName)
		{
			m_lStates.erase(iter);
			return;
		}
		iter++;
	}
}

void ATHStackSystem::PushStateOnTop(IBaseState* _state)
{
	unsigned int listSize = m_lStates.size();

	list<IBaseState*>::iterator iter = m_lStates.begin();
	for (unsigned int i = 0; i < listSize; ++i)
	{
		if ((*iter)->GetStateName() == _state->GetStateName())
		{
			return;
		}
		iter++;
	}

	m_lStates.push_front(_state);
}

void ATHStackSystem::PushStateOnBottom(IBaseState* _state)
{
	unsigned int listSize = m_lStates.size();

	list<IBaseState*>::iterator iter = m_lStates.begin();
	for (unsigned int i = 0; i < listSize; ++i)
	{
		if ((*iter)->GetStateName() == _state->GetStateName())
		{
			return;
		}
		iter++;
	}

	m_lStates.push_back(_state);
}

void ATHStackSystem::PopTopState()
{
	m_lStates.pop_front();
}

void ATHStackSystem::ClearAllStates() 
{
	m_lStates.clear();
}

void ATHStackSystem::Update(float _fDT)
{
	unsigned int listSize = m_lStates.size();

	list<IBaseState*>::iterator iter = m_lStates.begin();

	(*iter)->Update(_fDT);
	
	if ((*iter)->GetCascadeTime())
	{
		iter++;

		for (; iter != m_lStates.end(); iter++)
		{
			(*iter)->Update(_fDT);
		}
	}
}

