#include <ctime>
#include <stdio.h>
#include <string.h>
#include <Region.h>
#include <View.h>
#include "Acc.h"
#include "Cat.h"
#include "Pye.h"
#include "RegisterItem.h"
#include "Tra.h"

#define NumWidth		50
#define DateWidth		75
#define PyeWidth		150
#define PaymentWidth	75
#define DepositWidth	75
#define BalWidth		75
#define CatWidth		150
#define MemoWidth		150
#define RowHeight		14

RegisterItem::RegisterItem(Tra* tra, const BBitmap* check,
	const BBitmap* boxcheck, Prefs* prefs) : BListItem()
{
	#ifdef DEBUG
	printf("RegisterItem::RegisterItem - tra = %p\n", tra);
	#endif
	this->tra = tra;
	this->check = check;
	this->boxcheck = boxcheck;
	this->prefs = prefs;
	disabled = false;
}

void RegisterItem::Disable()
{
	disabled = true;
}

void RegisterItem::DrawItem(BView *owner, BRect frame, 
	bool complete)
{
	if (disabled)
		return;
	#ifdef DEBUG
//	printf("RegisterItem::DrawItem\n");
//	printf("\ttra = %p, gry = %i, num.type = %i, date = %li, tsf = %i, amt = %li, pam = %i\n",
//		tra, tra->gry, tra->num.type, tra->date, tra->tsf, tra->amt, tra->pam);
//	printf("\ttsfAcc = %p, cat = %p, pye = %p\n", tra->tsfAcc, tra->cat, tra->pye);
	#endif
	
	char buffer[100];
//	struct tm* timestruct;
	
	if (tra->gry)
	{
		owner->SetHighColor(236, 240, 255);
		owner->SetLowColor(236, 240, 255);
	}
	else
	{
		owner->SetHighColor(255, 255, 255);
		owner->SetLowColor(255, 255, 255);
	}
	owner->FillRect(frame);
	float top = frame.top;
	float bottom = frame.bottom;
	if (IsSelected())
	{
		frame.bottom -= 2;
		frame.right--;
		owner->SetPenSize(2);
		owner->SetHighColor(222, 223, 222);
		owner->StrokeRect(frame);
		frame.bottom += 2;
	}
	owner->SetPenSize(1);
	owner->SetHighColor(222, 223, 222);
	owner->StrokeLine(BPoint(frame.left, frame.bottom),
		BPoint(frame.right, frame.bottom));
	uint16 xpos = 2;
	uint16 ypos = (uint16)frame.top + RowHeight;
	owner->SetHighColor(0, 0, 0);
	if (tra->num.type == 0)
	{
		sprintf(buffer, "%i", tra->num.chknum);
	}
	else if (tra->num.type == 1)
		strcpy(buffer, "ATM");
	else if (tra->num.type == 2)
		strcpy(buffer, "EFT");
	else if (tra->num.type == 3)
		strcpy(buffer, "Xfer");
	else if (tra->num.type == 4)
		strcpy(buffer, "");
	owner->DrawString(buffer, BPoint(xpos + 10, ypos));
	xpos += NumWidth;
	owner->SetHighColor(222, 223, 222);
	//Line Between Num & Date
	owner->StrokeLine(BPoint(xpos, top), BPoint(xpos, bottom));
	owner->SetHighColor(0, 0, 0);
//	timestruct = localtime(&tra->date);
//	strftime(buffer, 20, "%m/%d/%Y", timestruct);
	owner->DrawString(prefs->dateToString(tra->date).c_str(), BPoint(xpos + 10, ypos));
	xpos += DateWidth;
	owner->SetHighColor(222, 223, 222);
	//Line Between Date & Payee
	owner->StrokeLine(BPoint(xpos, top), BPoint(xpos, bottom));
	owner->SetHighColor(0, 0, 0);
	owner->ConstrainClippingRegion(new BRegion(BRect(xpos + 10, ypos - 10, 
		xpos + PyeWidth - 5, ypos + 5)));
	if (tra->tsf)
	{
		strcpy(buffer, ("Transfer with: " + tra->tsfAcc->name).c_str());
	}
	else
		strcpy(buffer, tra->pye->name.c_str());
	owner->DrawString(buffer, BPoint(xpos + 10, ypos));
	owner->ConstrainClippingRegion(0);
	xpos += PyeWidth;
	owner->SetHighColor(222, 223, 222);
	//Line Between Payee & Payment
	owner->StrokeLine(BPoint(xpos, top), BPoint(xpos, top + RowHeight + 2));
	owner->StrokeLine(BPoint(xpos, top + RowHeight + 2), 
		BPoint(xpos + PaymentWidth + DepositWidth, top + RowHeight + 2));
	owner->SetHighColor(0, 0, 0);
	if (tra->pam)
		owner->DrawString(prefs->currencyToString(tra->amt).c_str(), BPoint(xpos + 10, ypos));
	xpos += PaymentWidth;
	owner->SetHighColor(222, 223, 222);
	//Line Between Payment & Deposit
	owner->StrokeLine(BPoint(xpos, top), BPoint(xpos, top + RowHeight + 2));
	owner->SetHighColor(0, 0, 0);
	if (!tra->pam)
		owner->DrawString(prefs->currencyToString(tra->amt).c_str(), BPoint(xpos + 10, ypos));
	xpos += DepositWidth;
	owner->SetHighColor(222, 223, 222);
	//Line Between Deposit & Balance
	owner->StrokeLine(BPoint(xpos, top), BPoint(xpos, bottom));
	owner->SetHighColor(0, 0, 0);
	owner->DrawString(prefs->currencyToString(tra->balFol).c_str(), BPoint(xpos + 10, ypos));
	xpos += BalWidth;
	owner->SetHighColor(222, 223, 222);
	//Line Between Balance & Check
	owner->StrokeLine(BPoint(xpos, top), BPoint(xpos, bottom));
	owner->SetHighColor(0, 0, 0);
	if (tra->rec == 1)
	{
		owner->SetDrawingMode(B_OP_MIN);
		owner->DrawBitmap(check, BPoint(xpos + 1, ypos - 3));
		owner->SetDrawingMode(B_OP_COPY);
	}
	else if (tra->rec == 2)
	{
		owner->SetDrawingMode(B_OP_MIN);
		owner->DrawBitmap(boxcheck, BPoint(xpos + 1, ypos - 3));
		owner->SetDrawingMode(B_OP_COPY);
	}
	
	xpos = 2 + NumWidth + DateWidth;
	ypos += RowHeight;
	owner->SetHighColor(0, 0, 0);
	if (tra->tsf)
	{
		strcpy(buffer, "Transfer");
		owner->DrawString(buffer, BPoint(xpos + 10, ypos));
	}
	else
	{
		owner->ConstrainClippingRegion(new BRegion(BRect(xpos + 10, ypos - 10, 
			xpos + CatWidth + PaymentWidth + DepositWidth - 5, ypos + 5)));
		owner->DrawString(tra->cat->Name().c_str(), BPoint(xpos + 10, ypos));
		owner->ConstrainClippingRegion(0);
	}
	xpos += CatWidth;
	xpos += PaymentWidth;
	xpos += DepositWidth;
	owner->SetHighColor(125, 125, 125);
	owner->DrawString(prefs->currencyToString(tra->recBalFol).c_str(), BPoint(xpos + 10, ypos));
	ypos += RowHeight;
	xpos = 2 + NumWidth + DateWidth;
	owner->SetHighColor(0, 0, 0);
	owner->ConstrainClippingRegion(new BRegion(BRect(xpos + 10, ypos - 10, 
		xpos + MemoWidth + PaymentWidth + DepositWidth - 5, ypos + 5)));
	owner->DrawString(tra->mem.c_str(), BPoint(xpos + 10, ypos));
	owner->ConstrainClippingRegion(0);
}

void RegisterItem::Update(BView *owner, const BFont *finfo)
{
	BListItem::Update(owner, finfo);
	SetHeight(finfo->Size() * 3 + 12);
}
