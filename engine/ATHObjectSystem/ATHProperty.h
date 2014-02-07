#ifndef ATHPROPERTY_H
#define ATHPROPERTY_H

enum ATHPropertyType{ APT_VOID, APT_INT, APT_FLOAT, APT_STRING };

class ATHProperty
{
private:

	void*			m_pData;
	unsigned int	m_unDataSize;
	ATHPropertyType m_Type;

	ATHProperty& operator=(const ATHProperty&);

public:

	ATHProperty();
	~ATHProperty();
	ATHProperty(const ATHProperty& _source);

	void SetData(void* _pData, ATHPropertyType _type, unsigned int _unSize = 0);

	ATHPropertyType GetPropertyType() { return m_Type; }
	unsigned int GetDataSize() { return m_unDataSize; }

	int		GetAsInt();
	float	GetAsFloat();
	char*	GetAsString();

	void null();

	friend class ATHProperty;
};

#endif