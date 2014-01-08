#ifndef ATHEVENTLISTENER_H
#define ATHEVENTLISTENER_H

#include "ATHEvent.h"

class ATHEventListener
{
private:
public:
	ATHEventListener( void ) {}
	virtual ~ATHEventListener( void ) {}

	virtual void HandleEvent( const ATHEvent* _pEvent ) = 0;
};

#endif