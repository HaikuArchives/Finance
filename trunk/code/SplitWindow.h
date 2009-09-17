#include <list>
#include <string>
#include <vector>
#include <Window.h>
#include "NameSorted.h"

#define	AddCatMSG			301
#define SplitEnterMSG		302
#define CatPopUpMSG			303
#define AmountModifiedMSG	304
#define SplitMSG			305

class BRadioButton;

class Proxy;
class Cat;
class SplCat;

class SplitWindow : public BWindow
{
	Prefs* prefs;
	typedef vector<BTextControl*> tcv;
	typedef set<Cat*, NameSorted> cas;
	typedef list<Cat*> cal;
	typedef list<int32> i32l;
	Proxy* theProxy;
	BHandler* targetHandler;
	cas* catS;
	tcv catTexts;
	tcv amtTexts;
	uint8 pam;
	int32 total;
	BView* v;
	BStringView* diffText;
	BStringView* totalText;
	BRadioButton* depRadio;
	BRadioButton* pamRadio;
	uint16 ypos;
	uint8 catnum;
	void AddCat();
	Cat* GetCat(string text);
	BMenuItem* MakeCatMenu(Cat* cat);
	int32 SplitSum();
public:
	SplitWindow(BRect frame, Proxy* theProxy, BHandler* targetHandler,
		Prefs* prefs, uint32 total, uint8 pam, SplCat* sc);
	void MessageReceived(BMessage* msg);
};
