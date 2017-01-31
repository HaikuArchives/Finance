#include <stdio.h>

#include "Cat.h"
#include "CatListItem.h"

CatListItem::CatListItem(Cat* cat, uint8 lvl) : BStringItem(cat->LeafName().c_str(), lvl)
{
	this->cat = cat;
}
