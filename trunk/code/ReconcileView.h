#include <View.h>
#include "Prefs.h"

#define ReconcileCancelMSG		801
#define ReconcileFinishMSG		802
#define ReconcileModifiedMSG	803

class ReconcileView : public BView
{
	Prefs* prefs;
	int32 recBalAtStart;
	int32 balance;
	BTextControl* starting;
	BTextControl* ending;
	BButton* finishButton;
	BButton* cancelButton;
	BStringView* currentRecBal;
	BStringView* diffInBal;
public:
	ReconcileView(BRect frame, int32 balance, Prefs* prefs);
	void Draw(BRect updateRect);
	void RedoBals();
	void SetRecBal(int32 balance);
	void SetRecBalAtStart(int32 balance);
};