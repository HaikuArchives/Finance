#include <ListItem.h>
#include "ColorMaker.h"
#include "Prefs.h"
class Acc;

class AccListItem : public BListItem
{
	bool disabled;
	ColorMaker cm;
	Prefs* prefs;
public:
	Acc* acc;
	AccListItem(Acc* acc, Prefs* prefs);
	void Disable();
	void DrawItem(BView *owner, BRect frame, bool complete = false);
	void Update(BView* owner, const BFont* finfo);
};