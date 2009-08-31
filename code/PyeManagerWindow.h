#include <set>
#include <Window.h>
#include "Pye.h"

#define PyeManagerWindowExitMSG			1101
#define PyeManagerWindowCloseMSG		1102
#define PyeManagerWindowDeleteMSG		1103
#define PyeManagerWindowNewMSG			1104
#define PyeManagerWindowRenameMSG		1105
#define PyeManagerWindowNewNameMSG		1106
#define PyeManagerWindowDeleteNameMSG	1107
#define PyeManagerWindowRenameNameMSG	1108

class Proxy;

class PyeManagerWindow : public BWindow
{
	typedef set<Pye*, NameSorted> pas;
	BHandler* returnHandler;
	BListView* lv;
	Proxy* theProxy;
	pas* pyeS;
	void FillList();
public:
	PyeManagerWindow(BHandler* returnHandler, Proxy* theProxy);
	void MessageReceived(BMessage* message);
	bool QuitRequested();
	void RedoList();
};