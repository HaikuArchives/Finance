#include <ListItem.h>

class Cat;

class CatListItem : public BStringItem
{
public:
	Cat* cat;
	CatListItem(Cat* cat, uint8 lvl = 0);
};