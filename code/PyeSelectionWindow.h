#include <set>
#include "Pye.h"
#include "StringInputWindow.h"

#define PyeSelectionWindowPopPyeMSG 1301

class BPopUpMenu;

class PyeSelectionWindow : public StringInputWindow
{
	typedef set<Pye*, NameSorted> pas;
	BPopUpMenu* pyePop;
public:
	PyeSelectionWindow(const char* title, BMessage* returnMessage,
		BHandler* returnHandler, pas* pyeS);
	void MessageReceived(BMessage* message);
};
