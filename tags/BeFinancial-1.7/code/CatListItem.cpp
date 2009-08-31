#include <stdio.h>

#include "Cat.h"
#include "CatListItem.h"

CatListItem::CatListItem(Cat* cat, uint8 lvl = 0) : BStringItem(cat->LeafName().c_str(), lvl)
{
	this->cat = cat;
}