#ifndef AccListViewH
#define AccListViewH

#include <map>
#include <View.h>

class Acc;
class AccListItem;

#define AccSelectedMSG	601

class AccListView : public BView
{
	Prefs* prefs;
	BListView* lv;
	map<Acc*, AccListItem*> accMap;
public:
	AccListView(BRect frame, Prefs* prefs);
	void AddAcc(Acc* acc);
	void InvalidateAccLI(Acc* acc);
	void InvalidateLV();
	Acc* GetSelectedAcc();
	void RemoveAll();
};

#endif