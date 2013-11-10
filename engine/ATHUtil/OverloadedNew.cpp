#include "OverloadedNew.h"

//================================================================================
void* operator new( size_t sz ) 
{
	char* allocation = MemoryManager::GetInstance()->Allocate( (int)sz );
	assert( allocation );
	return (void*)allocation;
}
//================================================================================
void* operator new( size_t sz, char* _szUsage ) 
{
	char* allocation = MemoryManager::GetInstance()->Allocate( (int)sz, _szUsage );
	assert( allocation );
	return (void*)allocation;
}
//================================================================================
void* operator new[]( size_t sz ) 
{
	char* allocation = MemoryManager::GetInstance()->Allocate( (int)sz );
	assert( allocation );
	return (void*)allocation;
}
//================================================================================
void* operator new[]( size_t sz, char* _szUsage ) 
{
	char* allocation = MemoryManager::GetInstance()->Allocate( (int)sz, _szUsage );
	assert( allocation );
	return (void*)allocation;
}
//================================================================================
void operator delete( void* m )
{
	MemoryManager::GetInstance()->DeAllocate( m );
}
//================================================================================
void operator delete[]( void* m )
{
	MemoryManager::GetInstance()->DeAllocate( m );
}
