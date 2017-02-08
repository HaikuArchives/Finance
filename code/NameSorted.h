#ifndef NameSortedH
#define NameSortedH

#include <string>

using namespace std;

class NameSorted
{
public:
	string name;
	int operator()(const NameSorted* n1, const NameSorted* n2) const;
};

#endif
