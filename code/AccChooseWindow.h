#include <list>
#include <Window.h>
#include "Acc.h"
#include "Prefs.h"

#define AccChooseOKMSG			901
#define AccChooseSkipTraMSG		902

class AccChooseWindow : public BWindow
{
	sem_id clickWait;
	Acc* acc;
	BListView* lv;
public:
	AccChooseWindow(list<Acc*>* accL, string title, string text1, string text2,
		Acc* acc, Prefs* prefs);
	Acc* GetAcc();
	void MessageReceived(BMessage* msg);
};