#ifndef IBASESTATE_H_
#define IBASESTATE_H_

class IBaseState
{
	bool m_bCascadeTime;

public:
	virtual void Start() = 0;
	virtual void Update(float _fDT) = 0;
	virtual void Shutdown() = 0;

};

#endif /*IBASESTATE_H_*/