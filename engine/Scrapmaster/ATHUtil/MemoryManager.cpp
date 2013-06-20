#include "MemoryManager.h"
#include <malloc.h>
#include <assert.h>
#include <iostream>

MemoryManager* MemoryManager::mInstance = nullptr;

MemoryManager::MemoryManager()
{
	mPool = nullptr;
}

MemoryManager::~MemoryManager()
{
	free( mPool );
}


void MemoryManager::AdjustUsage( std::string _usage, int _amount )
{
	std::string testString = std::string( _usage );

	if( m_mapUsageCount.count( testString ) > 0 )
		m_mapUsageCount[ testString ] += _amount;
	else
		m_mapUsageCount[ testString ] = _amount;

	// Cleanup
	if( m_mapUsageCount[ testString ] == 0 )
	{
		m_mapUsageCount.erase( testString );
	}

	if( m_mapUsageCount[ testString ] < 0 ) 
	{
		assert( false && "MemoryManager trakcing error: Removing more than was added." );
	}

	std::cout << DebugString();

}

MemoryManager* MemoryManager::GetInstance()
{
	if( mInstance == nullptr )
	{
		mInstance = new ( malloc( sizeof( MemoryManager ) ) ) MemoryManager();
	}

	return mInstance;
}

void MemoryManager::FreeInstance()
{
	if( mInstance )
	{
		mInstance->~MemoryManager();
		free( (void*)mInstance );
		mInstance = nullptr;
	}
}

void MemoryManager::Init(unsigned int poolSizeInBytes)
{

	mPool = (char*)malloc( poolSizeInBytes );

	mTotalPoolSize = poolSizeInBytes;

	FindFreeMemory = &MemoryManager::FirstAvailable;

	((Header*)(mPool))->size = poolSizeInBytes - ( sizeof( Header ) + sizeof( Footer ) );
	((Header*)(mPool))->prev = ((Header*)(mPool));
	((Header*)(mPool))->next = ((Header*)(mPool));

	mFreeHead = (Header*)mPool;

	char* end = mPool;
	end += sizeof( Header );
	end += poolSizeInBytes;

	((Footer*)(end))->size = ((Header*)(mPool))->size;
	mEndPoolFooter = ((Footer*)(end));

	std::cout << "MemoryManager: Initialized with " << poolSizeInBytes << " bytes\n";

}

char * MemoryManager::Allocate(unsigned int allocSize, char* _usage )
{

	Header* check = ((this->*FindFreeMemory)( allocSize ));

	if( !check )
		return nullptr;

	char* parser = (char*)check;

	if( check->size - allocSize > 16 )
	{

		unsigned int newAllocSize = allocSize + sizeof( Footer ) + sizeof( Header );

		if( ( newAllocSize < ((Header*)(parser))->size ) )
			((Header*)(parser))->size -= newAllocSize;
		else
			return nullptr;

		parser += sizeof( Header );
		parser += check->size;

		((Footer*)(parser))->size = check->size;

		parser += sizeof( Footer );

		((Header*)(parser))->next = nullptr;
		((Header*)(parser))->prev = nullptr;
		((Header*)(parser))->size = allocSize;
		((Header*)(parser))->size |= (1<<31);

		parser += sizeof( Header );
		parser += allocSize;

		((Footer*)(parser))->size = allocSize;
		((Footer*)(parser))->size |= (1<<31);

		parser -= allocSize;

		// Save off the usage amounts for tracking.
		if( _usage != NULL )
		{
			AdjustUsage( _usage, (int)allocSize ); 
			m_mapPtrTable[ (void*)parser ] = std::string( _usage );
		}

		return parser;
	}
	else
	{
		check->next->prev = check->prev;
		check->prev->next = check->next;

		mFreeHead = mFreeHead->next;

		if( check == check->next )
		{
			mFreeHead = nullptr;
		}

		parser += sizeof( Header );
		parser += check->size;

		((Footer*)(parser))->size |= ( 1<<31 );

		parser -= check->size;

		check->size |= ( 1<<31 );

		// Save off the usage amounts for tracking.
		if( _usage != NULL )
		{
			AdjustUsage( _usage, (int)allocSize ); 
			m_mapPtrTable[ (void*)parser ] = std::string( _usage );
		}

		return parser;
	}	

	return nullptr;

}

MemoryManager::Header * MemoryManager::FirstAvailable(unsigned int allocSize)
{
	Header* stuff;

	stuff = mFreeHead;

	if( !stuff )
		return nullptr;

	if( stuff->size >= allocSize )
	{
		return stuff;
	}

	stuff = stuff->next;

	while( true )
	{	
		if( stuff == mFreeHead )
			return nullptr;
		else
		{
			if( stuff->size >= allocSize )
				return stuff;
		}

		stuff = stuff->next;
	}
}

void MemoryManager::DeAllocate(void * target)
{
	char* data = (char*)target;
	bool MergeUp	= false;
	bool MergeDown	= false;

	Header* AboveHeader = nullptr;
	Header*	Target = nullptr;
	Header* BelowHeader = nullptr;

	//CheckUp

	//Save tracking data
	void* szSourcePointer = (void*)data;

	//Journey to get the pointer;

	data -= sizeof( Header );
	Target = (Header*)data;

	if( ( Target->size & (1<<31) ) )
		Target->size &= ~(1<<31);
	else
		return;

	// Save the data pointer for tracking cleanup.
	int nAdjSize = (int)Target->size;

	// Cleanup the tracking data.
	if( m_mapPtrTable.count( szSourcePointer ) > 0 )
	{
		std::string toAdjust = m_mapPtrTable[ szSourcePointer ];
		m_mapPtrTable.erase( szSourcePointer );

		AdjustUsage( toAdjust, -nAdjSize );
	}

	if( data != mPool )
	{
		data -= sizeof( Footer );

		if( !( ((Footer*)(data))->size & (1<<31) ) )
		{
			MergeUp = true;

			data -= ((Footer*)(data))->size;
			data -= sizeof( Header );
			AboveHeader = ((Header*)(data));
		}
	}

	data = (char*)Target;
	if( data + sizeof( Header ) + Target->size != (char*)mEndPoolFooter )
	{
		data += sizeof( Header );
		data += Target->size;
		data += sizeof( Footer );

		if( !( ((Header*)(data))->size & (1<<31) ) )
		{
			MergeDown = true;

			BelowHeader = (Header*)data;
		}
	}

	data = (char*)Target;
	data += sizeof( Header );
	data += Target->size;
	((Footer*)(data))->size &= ~(1<<31);

	if( MergeDown )
	{
		if( BelowHeader == mFreeHead )
		{
			mFreeHead = mFreeHead->next;

			if( mFreeHead == mFreeHead->prev )
				mFreeHead = nullptr;
		}

		BelowHeader->next->prev = BelowHeader->prev;
		BelowHeader->prev->next = BelowHeader->next;

		Glom( Target, BelowHeader );
	}

	if( MergeUp )
	{
		Glom( AboveHeader, Target );
		return;
	}

	if( mFreeHead )
	{

		Target->next = mFreeHead;
		Target->prev = mFreeHead->prev;

		Target->next->prev = Target;
		Target->prev->next = Target;
	}
	else
	{
		mFreeHead = Target;
		mFreeHead->next = mFreeHead;
		mFreeHead->prev = mFreeHead;
	}


}

//void MemoryManager::TrackDeAllocate( char* data )
//{
//
//	Header*	Target = nullptr;
//	char* tempData = data - sizeof( Header );
//	Target = (Header*)tempData;
//	unsigned int allocSize = 0;
//
//	if( ( Target->size & (1<<31) ) )
//	{
//		allocSize = Target->size;
//		allocSize &= ~(1<<31);
//	}
//	else
//		return;
//
//	m_mapUsageCount[ m_mapPtrTable[ data ] ] -= allocSize;
//
//	std::cout << DebugString();
//
//	DeAllocate( data );
//
//}

void MemoryManager::Glom( Header* freeMemory, Header* ToBeFree )
{

	//ToBeFree->size &= ~(1<<31);

	int TotalSize = freeMemory->size + ToBeFree->size;
	TotalSize += ( sizeof( Header ) + sizeof( Footer ) );

	freeMemory->size = TotalSize;

	char* parser = (char*)freeMemory;
	parser += sizeof( Header );

	parser += freeMemory->size;

	((Footer*)(parser))->size = freeMemory->size;

}

std::string MemoryManager::DebugString()
{
	std::string toReturn = std::string();

	char buffer[64];

	toReturn += "Name\tAllocation\t\n";
	toReturn += "--------------------\n";
	std::map< std::string, int >::iterator itrCurr = m_mapUsageCount.begin();

	while( itrCurr != m_mapUsageCount.end() )
	{

		toReturn += (*itrCurr).first;
		toReturn += "\t";

		sprintf_s( buffer, 64, "%d", (*itrCurr).second );

		toReturn += buffer;
		toReturn += "\n";

		itrCurr++;
	}

	toReturn += "\n";

	return toReturn;
}


