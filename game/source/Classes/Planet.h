#ifndef PLANET_H
#define PLANET_H

#include "../../../engine/ATHObjectSystem/ATHObject.h"
#include <list>

class b2Body;
class Planet : public ATHObject
{
private:

	std::list< b2Body* > m_liGravityTargets;
	// Planets are kinematic, but we need mass for gravity calculations;
	float m_fMass;

public:

	Planet();
	virtual void FixedUpdate();
	virtual void OnCollisionEnter(b2Contact* _pContact);
	virtual void OnCollisionExit(b2Contact* _pContact);

	float GetMass() { return m_fMass; }
	void SetMass(float _fMass) { m_fMass = _fMass; }

};

#endif