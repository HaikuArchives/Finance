#ifndef RegisterViewH
#define RegisterViewH

#include <set>
#include <ListView.h>
#include <View.h>
#include "Tra.h"

#define RegisterViewSelectedMSG		301
#define RegisterViewInvokedMSG		302

class RegisterView : public BView
{
	Prefs* prefs;
	const BBitmap* check;
	const BBitmap* boxcheck;
	BListView* lv;
	Tra tra;
public:
	RegisterView(BRect frame, Prefs* prefs);
	~RegisterView();
	void AddTra(Tra* tra, uint16 pos);
	void DeselectAll();
	void Draw(BRect updateRect);
	void DeleteTra(uint16 pos);
	void EditTra();
	Tra* GetSelectedTra();
	void InvalidateLV();
	void MessageReceived(BMessage* message);
};

#endif
