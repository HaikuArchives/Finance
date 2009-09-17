#include <Window.h>
#include "Acc.h"

#define AccSetOKButtonMSG		1001
#define AccSetCancelButtonMSG	1002
#define AccSetReturnMSG			1003

class BTextControl;

class AccSettingsWindow : public BWindow
{
	Acc* acc;
	Prefs* prefs;
	BHandler* returnHandler;
	BTextControl* openBalTC;
public:
	AccSettingsWindow(Acc* acc, BHandler* returnHandler, Prefs* prefs);
	void MessageReceived(BMessage* msg);
};
