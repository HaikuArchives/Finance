#ifndef NameSortedH
#define NameSortedH

#include <string>

class NameSorted
{
public:
	string name;
	int operator()(const NameSorted* n1, const NameSorted* n2) const;
};

#endif