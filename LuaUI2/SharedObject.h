#pragma once

#include <unordered_map>

class CSharedObject
{
public:
	CSharedObject(void);
	~CSharedObject(void);

private:
	UINT m_type;
	double m_number;
	std::string m_str;
	std::tr1::unordered_map<std::string, CSharedObject*> m_map;
	std::vector<CSharedObject *> m_vector;
};
