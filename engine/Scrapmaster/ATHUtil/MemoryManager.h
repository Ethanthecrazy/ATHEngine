#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <assert.h>

const int MM_NUM_ALLOC_COUNTERS = 32;

static int MM_ALLOCATED = 0;
static int MM_DEALLOCATED = 0;

struct AllocCounter
{
	char m_szName[16];
	unsigned int m_unAmount;
};

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

		unsigned int m_unNamedIndex;
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
	AllocCounter m_Usages[MM_NUM_ALLOC_COUNTERS];

	unsigned int AdjustUsage( char* _szUsage, int _amount );
	
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

	void DebugString();
};

#endif