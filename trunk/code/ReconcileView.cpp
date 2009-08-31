#include <stdexcept>
#include <stdio.h>
#include <string>
#include <Button.h>
#include <StringView.h>
#include <TextControl.h>
#include "ReconcileView.h"

ReconcileView::ReconcileView(BRect frame, int32 balance, Prefs* prefs) : BView(frame,
	"ReconcileView", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW)
{
	this->prefs = prefs;
	SetViewColor(236, 240, 255);
	BStringView* sv = new BStringView(BRect(10, 15, 210, 25), "Statementsv",
		"Statement Info:");
	AddChild(sv);
	starting = new BTextControl(BRect(10, 35, 210, 35),
		"startingTC", "Starting Bal:", "", 0);
	starting->SetDivider(130);
	starting->SetAlignment(B_ALIGN_LEFT, B_ALIGN_RIGHT);
	starting->SetModificationMessage(new BMessage(ReconcileModifiedMSG));
	AddChild(starting);
	ending = new BTextControl(BRect(10, 60, 210, 60),
		"endingTC", "Ending Bal:", "", 0);
	ending->SetDivider(130);
	ending->SetAlignment(B_ALIGN_LEFT, B_ALIGN_RIGHT);
	ending->SetModificationMessage(new BMessage(ReconcileModifiedMSG));
	AddChild(ending);
	finishButton = new BButton(BRect(15, 150, 105, 170), "finishButton",
		"Finish", new BMessage(ReconcileFinishMSG));
	AddChild(finishButton);
	finishButton->SetEnabled(false);
	cancelButton = new BButton(BRect(120, 150, 210, 170), "cancelButton",
		"Cancel", new BMessage(ReconcileCancelMSG));
	AddChild(cancelButton);
	sv = new BStringView(BRect(10, 95, 135, 105), "currentRecBalsv",
		"Current Bal:");
	AddChild(sv);
	sv = new BStringView(BRect(10, 120, 135, 130), "diffInBalsv",
		"Difference:");
	AddChild(sv);
	currentRecBal = new BStringView(BRect(140, 95, 210, 105),
		"currentRecBalsv", prefs->currencyToString(balance).c_str());
	currentRecBal->SetAlignment(B_ALIGN_RIGHT);
	AddChild(currentRecBal);
	diffInBal = new BStringView(BRect(140, 120, 210, 130),
		"diffInBalsv", prefs->currencyToString(-balance).c_str());
	diffInBal->SetAlignment(B_ALIGN_RIGHT);
	AddChild(diffInBal);
	this->balance = balance;
	recBalAtStart = balance;
}

void ReconcileView::Draw(BRect updateRect)
{
	BRect frame = Frame();
	frame.OffsetTo(0, 0);
	SetHighColor(156, 156, 156);
	StrokeRect(frame);
}

void ReconcileView::RedoBals()
{
	currentRecBal->SetText(prefs->currencyToString(balance).c_str());

	string endingText = ending->Text();
	int32 endingAmount = prefs->stringToCurrency(endingText.c_str());

	string startingText = starting->Text();
	int32 startingAmount = prefs->stringToCurrency(startingText.c_str());
	
	int32 diff = (endingAmount - balance) - (startingAmount - recBalAtStart);
	diffInBal->SetText(prefs->currencyToString(diff).c_str());
	
	if (diff)
		finishButton->SetEnabled(false);
	else
		finishButton->SetEnabled(true);
}

void ReconcileView::SetRecBalAtStart(int32 balance)
{
	recBalAtStart = balance;
	starting->SetText("");
	ending->SetText("");
}

void ReconcileView::SetRecBal(int32 balance)
{
	this->balance = balance;
	RedoBals();
}