#include <ListItem.h>

class Pye;

class PyeListItem : public BStringItem
{
public:
	Pye* pye;
	PyeListItem(Pye* pye);
};