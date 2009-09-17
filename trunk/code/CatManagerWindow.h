#include <set>
#include <Window.h>
#include "Cat.h"

#define CatManagerWindowExitMSG			1201
#define CatManagerWindowCloseMSG		1202
#define CatManagerWindowDeleteMSG		1203
#define CatManagerWindowNewMSG			1204
#define CatManagerWindowRenameMSG		1205
#define CatManagerWindowNewNameMSG		1206
#define CatManagerWindowDeleteNameMSG	1207
#define CatManagerWindowRenameNameMSG	1208

class BOutlineListView;

class Proxy;

class CatManagerWindow : public BWindow
{
	typedef set<Cat*, NameSorted> cas;
	cas* catS;
	BHandler* returnHandler;
	BOutlineListView* lv;
	Proxy* theProxy;
	void FillList();
	void RecAddCat(uint8 lvl, Cat* cat);
	bool QuitRequested();
public:
	CatManagerWindow(BHandler* returnHandler, Proxy* theProxy);
	void MessageReceived(BMessage* message);
	void RedoList();
};
