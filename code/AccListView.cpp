#include <ListView.h>
#include <ScrollBar.h>
#include <ScrollView.h>
#include "AccListItem.h"
#include "AccListView.h"

AccListView::AccListView(BRect frame, Prefs* prefs) : BView(frame,
	"AccListView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM, B_WILL_DRAW)
{
	frame.OffsetTo(0, 0);
	frame.Set(frame.left + 2, frame.top + 2, frame.right - 2 -
		B_V_SCROLL_BAR_WIDTH, frame.bottom - 2);

	this->prefs = prefs;
	
	lv = new BListView(frame, "AccListViewLV", B_SINGLE_SELECTION_LIST,
		B_FOLLOW_ALL_SIDES);
	lv->SetSelectionMessage(new BMessage(AccSelectedMSG));
	BFont font;
	lv->GetFont(&font);
	font.SetSize(10);
	lv->SetFont(&font);
	AddChild(new BScrollView("scrollalv", lv,
		B_FOLLOW_ALL_SIDES, 0, false, true));
}

void AccListView::AddAcc(Acc* acc)
{
	AccListItem* aLI = new AccListItem(acc, prefs);
	lv->AddItem(aLI);
	lv->Select(lv->CountItems() - 1);
	accMap.insert(pair<Acc*, AccListItem*>(acc, aLI));
}

Acc* AccListView::GetSelectedAcc()
{
	int32 cs = lv->CurrentSelection();
	if (cs < 0)
		return 0;
	return ((AccListItem*)lv->ItemAt(cs))->acc;
}

void AccListView::InvalidateAccLI(Acc* acc)
{
	lv->InvalidateItem(lv->IndexOf(accMap[acc]));
}

void AccListView::InvalidateLV()
{
	lv->Invalidate();
}

void AccListView::RemoveAll()
{
	while (lv->CountItems())
	{
		((AccListItem*)lv->ItemAt(0))->Disable();
		lv->RemoveItem((int32)0);
	}
}