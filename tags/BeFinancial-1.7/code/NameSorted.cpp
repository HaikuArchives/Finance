#include <stdio.h>
#include "NameSorted.h"

int NameSorted::operator()(const NameSorted* n1, const NameSorted* n2) const
{
	bool ret = n1->name < n2->name;
	return ret;
}
