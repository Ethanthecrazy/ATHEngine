#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <map>
#include <string>
#include <iostream>
#include <assert.h>

class MemoryManager
{
public:
	struct Header
	{
		// How much memory is free INSIDE the block : excludes header and footer size
		// High order bit of 1 == used memory block
		// High order bit of 0 == free memory block
		unsigned int size;
		// Pointer to the previous free header in the Cyclic Doubly Linked List
		Header * prev;
		// Pointer to the next free header in the Cyclic Doubly Linked List
		Header * next;
	};
	struct Footer
	{
		// How much memory is free INSIDE the block : excludes header and footer size
		// High order bit of 1 == used memory block
		// High order bit of 0 == free memory block
		unsigned int size;
	};
private:
	// Singleton
	static MemoryManager * mInstance;
	MemoryManager();
	MemoryManager(const MemoryManager &);
	MemoryManager & operator=(const MemoryManager &);

	// Entire Memory Pool
	char * mPool;
	// The Last footer
	Footer * mEndPoolFooter;
	// The Entry Point to the free list (Cyclic Doubly-Linked List)
	Header * mFreeHead;
	// How much memory was allocated for this pool.
	unsigned int mTotalPoolSize;
	// Function pointer for the allocation behavior
	Header *(MemoryManager::*FindFreeMemory)(unsigned int allocSize);
	// Allocation Behaviors
	Header * FirstAvailable(unsigned int allocSize);

	// Keep tabs on how the memory is used.
	std::map< std::string, int > m_mapUsageCount;
	std::map< void*, std::string > m_mapPtrTable;

	void AdjustUsage( std::string _usage, int _amount );
	
public:
	// Singleton Interface
	static MemoryManager * GetInstance();
	static void FreeInstance();

	~MemoryManager();

	// Methods
	void Init(unsigned int poolSizeInBytes);

	char * Allocate(unsigned int allocSize, char* _usage = NULL );

	void DeAllocate(void * target);

	//void TrackDeAllocate( char* data );
	// Accessors for Lab Rendering Usage Only
	Header * GetPool(){return (Header *)mPool;}
	inline char * GetFreeHead() {return (char *)mFreeHead;}

	void Glom( Header* freeMemory, Header* ToBeFree );

	std::string DebugString();
};

// Utility Functions
// Allocate with this
template< typename allocType >
allocType* ATHNew( char* _usage = NULL )
{
	allocType* toReturn = NULL;
	unsigned int nAllocSize = sizeof( allocType );
	void* allocation = (void*)MemoryManager::GetInstance()->Allocate(  nAllocSize, _usage );
	if( allocation )
		toReturn = new ( allocation ) allocType();
	else
	{
		assert( false && "Could not allocate object" );
	}

	return toReturn;

}

// Allocate an array with this
template< typename allocType >
allocType* ATHNew( unsigned int _count, char* _usage = NULL )
{
	allocType* toReturn = NULL;

	if( _count > 0 )
	{
		unsigned int nAllocSize = sizeof( allocType ) * _count;
		void* allocation = (void*)MemoryManager::GetInstance()->Allocate(  nAllocSize, _usage );
		if( allocation )
		{
			allocType* tracker = (allocType*)(allocation);
			for( unsigned int i = 0; i < _count; ++i )
			{
				tracker = new ( tracker ) allocType();
				tracker++;
			}

			toReturn = (allocType*)allocation;

		}
		else
		{
			assert( false && "Could not allocate object" );
		}

	}
	return toReturn;

}

// Delete with this
template< typename allocType >
void ATHDelete( allocType* target )
{
	MemoryManager::GetInstance()->DeAllocate( (void*)target ); 
}




#endif