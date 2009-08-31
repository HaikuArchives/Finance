#include "Cat.h"
#include "StringInputWindow.h"

#define CatSelectionWindowPopCatMSG 1301

class CatSelectionWindow : public StringInputWindow
{
	typedef set<Cat*, NameSorted> cas;
	BPopUpMenu* catPop;
	BMenuItem* MakeCatMenu(Cat* cat, bool enabled = true);
public:
	CatSelectionWindow(const char* title, BMessage* returnMessage,
		BHandler* returnHandler, cas* catS);
	void MessageReceived(BMessage* message);
};