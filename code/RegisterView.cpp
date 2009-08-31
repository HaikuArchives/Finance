#include <stdio.h>
#include <Bitmap.h>
#include <ScrollView.h>
#include <StringView.h>
#include <TranslationUtils.h>
#include "RegisterItem.h"
#include "RegisterView.h"

#define NumWidth		50
#define DateWidth		75
#define PyeWidth		150
#define PaymentWidth	75
#define DepositWidth	75
#define CatWidth	150
#define MemoWidth		150
#define BalWidth	75

RegisterView::RegisterView(BRect frame, Prefs* prefs) : BView(frame, 
	"RegisterView", B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW)
{
	#ifdef DEBUG
	printf("RegisterView::RegisterView\n");
	#endif
	frame.OffsetTo(0, 0);
	frame.Set(frame.left + 2, frame.top + 54, frame.right - 2 -
		B_V_SCROLL_BAR_WIDTH, frame.bottom - 3);

	this->prefs = prefs;

	lv = new BListView(frame, "RegisterList", B_SINGLE_SELECTION_LIST,
		B_FOLLOW_TOP_BOTTOM);
	lv->SetSelectionMessage(new BMessage(RegisterViewSelectedMSG));
	lv->SetInvocationMessage(new BMessage(RegisterViewInvokedMSG));
	BFont font;
	lv->GetFont(&font);
	font.SetSize(12);
	lv->SetFont(&font);
	AddChild(new BScrollView("scrollregister", lv,
		B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, 0, false, true));

	BView* backdrop = new BView(BRect(frame.left - 1, frame.top - 53, 
		frame.right + B_V_SCROLL_BAR_WIDTH + 1, frame.top - 9), "backdrop", 
		B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
	backdrop->SetViewColor(236, 240, 255);
	AddChild(backdrop);

	uint16 xpos = 12;
	uint16 ypos = (uint16)frame.top - 50;
	BStringView* sv = new BStringView(BRect(xpos, ypos, xpos +
		NumWidth, ypos + 10), "NumTitle", "Num");
	font.SetSize(10);
	sv->SetFont(&font);
	backdrop->AddChild(sv);
	xpos += NumWidth;
	sv = new BStringView(BRect(xpos, ypos, xpos +
		DateWidth, ypos + 10), "DateTitle", "Date");
	sv->SetFont(&font);
	backdrop->AddChild(sv);
	xpos += DateWidth;
	sv = new BStringView(BRect(xpos, ypos, xpos +
		PyeWidth, ypos + 10), "PyeTitle", "Pye");
	sv->SetFont(&font);
	backdrop->AddChild(sv);
	xpos += PyeWidth;
	sv = new BStringView(BRect(xpos, ypos, xpos +
		PaymentWidth, ypos + 10), "PaymentTitle", "Payment");
	sv->SetFont(&font);
	backdrop->AddChild(sv);
	xpos += PaymentWidth;
	sv = new BStringView(BRect(xpos, ypos, xpos +
		DepositWidth, ypos + 10), "DepositTitle", "Deposit");
	sv->SetFont(&font);
	backdrop->AddChild(sv);
	xpos += DepositWidth;
	sv = new BStringView(BRect(xpos, ypos, xpos +
		BalWidth, ypos + 10), "BalTitle", "Balance");
	sv->SetFont(&font);
	backdrop->AddChild(sv);
	
	xpos = 12 + NumWidth + DateWidth;
	ypos += 14;
	sv = new BStringView(BRect(xpos, ypos, xpos +
		CatWidth, ypos + 10), "CatTitle", "Cat");
	sv->SetFont(&font);
	backdrop->AddChild(sv);
	xpos += PyeWidth + PaymentWidth + DepositWidth;
	sv = new BStringView(BRect(xpos, ypos, xpos +
		CatWidth, ypos + 10), "RecBalTitle", "Rec. Balance");
	sv->SetFont(&font);
	backdrop->AddChild(sv);
	ypos += 14;
	xpos = 12 + NumWidth + DateWidth;
	sv = new BStringView(BRect(xpos, ypos, xpos +
		MemoWidth, ypos + 10), "MemoTitle", "Memo");
	sv->SetFont(&font);
	backdrop->AddChild(sv);

	check = BTranslationUtils::GetBitmap('JPEG', "check0.jpg");
	boxcheck = BTranslationUtils::GetBitmap('JPEG', "check1.jpg");
}

RegisterView::~RegisterView()
{
	for (int32 i = 0; i < lv->CountItems(); i++)
	{
		delete lv->ItemAt(i);
	}
//	delete check;
//	delete boxcheck;
}

void RegisterView::AddTra(Tra* tra, uint16 pos)
{
	RegisterItem* ri = new RegisterItem(tra, check, boxcheck, prefs);
	lv->AddItem(ri, pos);
	for (uint16 i = pos + 1; i < lv->CountItems(); i++)
	{
		lv->InvalidateItem(i);
	}
}

void RegisterView::DeleteTra(uint16 position)
{
	((RegisterItem*)lv->ItemAt(position))->Disable();
	delete lv->RemoveItem(position);
}

void RegisterView::Draw(BRect updateRect)
{
	BRect frame = Frame();
	frame.OffsetTo(0, 0);
	frame.bottom = frame.top + 46;
	SetHighColor(156, 156, 156);
	StrokeRect(frame);
}

void RegisterView::EditTra()
{
	int32 cs = lv->CurrentSelection();
	lv->InvalidateItem(cs);
}

void RegisterView::DeselectAll()
{
	lv->DeselectAll();
}

Tra* RegisterView::GetSelectedTra()
{
	int32 cs = lv->CurrentSelection();
	if (cs < 0)
		return 0;
	return ((RegisterItem*)lv->ItemAt(cs))->tra;
}

void RegisterView::InvalidateLV()
{
	lv->Invalidate();
}

void RegisterView::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		default:
			BView::MessageReceived(message);
	}
}