#include "Pye.h"
#include "PyeListItem.h"

PyeListItem::PyeListItem(Pye* pye) : BStringItem(pye->name.c_str())
{
	this->pye = pye;
}