#include <stdio.h>
#include <ListView.h>
#include <View.h>
#include "Acc.h"
#include "AccListItem.h"

AccListItem::AccListItem(Acc* acc, Prefs* prefs) : BListItem()
{
	#ifdef DEBUG
	printf("AccListItem::AccListItem - %p\n", this);
	#endif
	this->acc = acc;
	this->prefs = prefs;
	disabled = false;
}

void AccListItem::Disable()
{
	disabled = true;
}

void AccListItem::DrawItem(BView *owner, BRect frame, bool complete)
{
	if (disabled)
		return;
	if (IsSelected())
	{
		rgb_color c = cm.ColorNumber(((BListView*)owner)->CurrentSelection());
		owner->SetHighColor(c);
		owner->SetLowColor(c);
	}
	else
	{
		owner->SetHighColor(255, 255, 255);
		owner->SetLowColor(255, 255, 255);
	}
	owner->FillRect(frame);
	if (IsSelected())
	{
		owner->SetHighColor(156, 156, 156);
		owner->StrokeRect(frame);
	}
	
	if (IsEnabled())
		owner->SetHighColor(0, 0, 0);
	else
		owner->SetHighColor(222, 222, 222);
	BFont font;
	owner->GetFont(&font);
	font.SetFace(B_BOLD_FACE);
	font.SetSize(11);
	owner->SetFont(&font);
	owner->DrawString(acc->name.c_str(), BPoint(frame.left + 5, frame.top + 14));
	font.SetFace(B_REGULAR_FACE);
	font.SetSize(10);
	owner->SetFont(&font);
	if (!acc->traS.empty())
	{
		string text = "Balance = ";
		text += prefs->currencyToString((*acc->traS.rbegin())->balFol);
		owner->DrawString(text.c_str(), BPoint(frame.left + 5, frame.top + 26));
		text = "Rec.Bal. = ";
		text += prefs->currencyToString((*acc->traS.rbegin())->recBalFol);
		owner->DrawString(text.c_str(), BPoint(frame.left + 5, frame.top + 38));
	}
}

void AccListItem::Update(BView* owner, const BFont* finfo)
{
	BListItem::Update(owner, finfo);
	SetHeight(finfo->Size() * 3 + 14);
}
