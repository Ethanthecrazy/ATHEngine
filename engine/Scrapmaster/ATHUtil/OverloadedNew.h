#ifndef OVERLOADEDNEW_H
#define OVERLOADEDNEW_H

// The compiler can become confused about the overload
// and issue a warning about in-place new, even though 
// the overload is being used.
#pragma warning( disable : 4291 )

#include "MemoryManager.h"

void* operator new( size_t sz );

void* operator new( size_t sz, char* _szUsage );

void* operator new[]( size_t sz );

void* operator new[]( size_t sz, char* _szUsage );

void operator delete( void* m );

void operator delete[]( void* m );

#endif