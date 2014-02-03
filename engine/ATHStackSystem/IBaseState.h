#ifndef IBASESTATE_H_
#define IBASESTATE_H_

class IBaseState
{
	bool m_bCascadeTime;
	char m_cName;			// Name of the state.

public:

	enum eStateNames {MAINMENU = 0, GAMEPLAY, OPTIONS, NUM_STATES};

	virtual void Start() = 0;
	virtual void Update(float _fDT) = 0;
	virtual void Shutdown() = 0;
	char GetStateName()		{return m_cName;}
	bool GetCascadeTime()	{return m_bCascadeTime;}
};

#endif /*IBASESTATE_H_*/