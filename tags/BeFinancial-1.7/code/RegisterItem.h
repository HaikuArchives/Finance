#include <ListItem.h>
#include "Prefs.h"

class Tra;

class RegisterItem : public BListItem
{
	const BBitmap* check;
	const BBitmap* boxcheck;
	bool disabled;
	Prefs* prefs;
public:
	Tra* tra;
	RegisterItem(Tra* tra, const BBitmap* check,
		const BBitmap* boxcheck, Prefs* prefs);
	void Disable();
	void DrawItem(BView *owner, BRect frame, bool complete = false);
	void Update(BView* owner, const BFont* finfo);
};
