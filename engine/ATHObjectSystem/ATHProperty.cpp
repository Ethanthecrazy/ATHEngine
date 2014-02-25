#include "ATHProperty.h"
#include <iostream>

ATHProperty::ATHProperty() : m_pData(nullptr), m_unDataSize(0), m_Type( APT_VOID )
{}
//================================================================================
ATHProperty::~ATHProperty()
{
	if (m_pData)
		delete m_pData;
}
//================================================================================
ATHProperty::ATHProperty(const ATHProperty& _source)
{
	m_pData = nullptr;
	m_Type = _source.m_Type;
	m_unDataSize = _source.m_unDataSize;

	if (!_source.m_pData)
		return;
	if (m_unDataSize == 0)
		return;

	m_pData = new char[m_unDataSize];
	memcpy_s(m_pData, m_unDataSize, _source.m_pData, m_unDataSize);
}
//================================================================================
void ATHProperty::SetData(void* _pData, ATHPropertyType _type, unsigned int _unSize)
{
	
	if (_pData == nullptr)
		return;

	if (m_pData)
	{
		delete m_pData;
		m_pData = nullptr;
	}
		
	// If its a type with a size, set it
	switch (_type)
	{
	case APT_INT:
		_unSize = sizeof(int);
		break;
	case APT_FLOAT:
		_unSize = sizeof(float);
		break;
	}

	m_pData = new char[_unSize];

	memcpy_s(m_pData, _unSize, _pData, _unSize);

	m_unDataSize = _unSize;
	m_Type = _type;

}
//================================================================================
int	ATHProperty::GetAsInt()
{
	return *((int*)(m_pData));
}
//================================================================================
float ATHProperty::GetAsFloat()
{
	return *((float*)(m_pData));
}
//================================================================================
char* ATHProperty::GetAsString()
{
	return (char*)m_pData;
}
//================================================================================
void ATHProperty::null()
{
	if (m_pData)
		delete m_pData;

	m_pData = nullptr;
	m_Type = APT_VOID;
	m_unDataSize = 0;

}
//================================================================================