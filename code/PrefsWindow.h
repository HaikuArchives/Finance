#include <Window.h>
#include "Acc.h"
#include "Prefs.h"

#define PrefsOKButtonMSG		1401
#define PrefsCancelButtonMSG	1402
#define PrefsReturnMSG			1403
#define PrefsCurrencyChangeMSG	1404
#define PrefsDateChangeMSG		1405

class BTextControl;
class BStringView;

class PrefsWindow : public BWindow
{
	BHandler* returnHandler;
	BTextControl* curSymTC;
	BMenuField* curPlaceMF;
	BTextControl* curSepTC;
	BTextControl* curDecTC;
	BMenuField* curDecPlaceMF;
	BMenuField* dateFormatMF;
	BTextControl* dateSepTC;
	BStringView* curSV;
	BStringView* dateSV;
	Prefs* prefs;
public:
	PrefsWindow(BHandler* returnHandler, Prefs* oldPrefs);
	void MessageReceived(BMessage* msg);
};
