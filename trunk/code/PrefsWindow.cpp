#include <stdio.h>
#include <Alert.h>
#include <Button.h>
#include <PopUpMenu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <String.h>
#include <StringView.h>
#include <TextControl.h>
#include "PrefsWindow.h"

PrefsWindow::PrefsWindow(BHandler* returnHandler, Prefs* oldPrefs) :
	BWindow(BRect(200, 200, 445, 485), "Preferences",
	B_FLOATING_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, 
	B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE)
{
	this->returnHandler = returnHandler;
	prefs = new Prefs;
	prefs->Set(oldPrefs);

	BRect rect(200, 200, 445, 485);
	rect.OffsetTo(0, 0);
	BView* backview = new BView(rect, "PrefsBackView", 0, B_WILL_DRAW);
	backview->SetViewColor(222, 222, 222);
	AddChild(backview);

	string str = "";
	str += prefs->curSym;
	curSymTC = new BTextControl(BRect(15, 15, 200, 15), "curSymTC",
		"Currency Symbol", str.c_str(), 0);
	curSymTC->SetDivider(160);
	curSymTC->SetModificationMessage(new BMessage(PrefsCurrencyChangeMSG));
	backview->AddChild(curSymTC);

	BMenuItem* mmi = 0;
	BPopUpMenu* m = new BPopUpMenu("");

	BMenuItem* mi = new BMenuItem("Before", new BMessage(PrefsCurrencyChangeMSG));
	if (prefs->curSymPlaceBefore)
		mmi = mi;
	m->AddItem(mi);

	mi = new BMenuItem("After", new BMessage(PrefsCurrencyChangeMSG));
	if (!prefs->curSymPlaceBefore)
		mmi = mi;
	m->AddItem(mi);

	curPlaceMF = new BMenuField(BRect(15, 39, 235, 39), "curPlaceMF",
		"Currency Placement", m);
	curPlaceMF->SetDivider(160);
	backview->AddChild(curPlaceMF);
	mmi->SetMarked(true);

	str = prefs->curSep;
	curSepTC = new BTextControl(BRect(15, 65, 200, 65), "curSepTC",
		"Currency Separator", str.c_str(), 0);
	curSepTC->SetDivider(160);
	curSepTC->SetModificationMessage(new BMessage(PrefsCurrencyChangeMSG));
	backview->AddChild(curSepTC);
	str = prefs->curDec;
	curDecTC = new BTextControl(BRect(15, 90, 200, 90), "curDecTC",
		"Currency Decimal", str.c_str(), 0);
	curDecTC->SetDivider(160);
	curDecTC->SetModificationMessage(new BMessage(PrefsCurrencyChangeMSG));
	backview->AddChild(curDecTC);

	m = new BPopUpMenu("");
	mi = new BMenuItem("2", new BMessage(PrefsCurrencyChangeMSG));
	if (prefs->curDecPlace == 2)
		mmi = mi;
	m->AddItem(mi);
	mi = new BMenuItem("1", new BMessage(PrefsCurrencyChangeMSG));
	if (prefs->curDecPlace == 1)
		mmi = mi;
	m->AddItem(mi);
	mi = new BMenuItem("0", new BMessage(PrefsCurrencyChangeMSG));
	if (prefs->curDecPlace == 0)
		mmi = mi;
	m->AddItem(mi);

	curDecPlaceMF = new BMenuField(BRect(15, 114, 230, 114), "curDecPlaceMF",
		"Digits after Decimal in Currency", m);
	curDecPlaceMF->SetDivider(160);
	backview->AddChild(curDecPlaceMF);
	mmi->SetMarked(true);
	
	curSV = new BStringView(BRect(15, 140, 230, 160), "curSV",	
		prefs->currencyToString(103425).c_str());
	curSV->SetAlignment(B_ALIGN_CENTER);
	backview->AddChild(curSV);

	m = new BPopUpMenu("");
	mi = new BMenuItem("mm-dd-yy", new BMessage(PrefsDateChangeMSG));
	if (prefs->dateFormat == 0)
		mmi = mi;
	m->AddItem(mi);
	mi = new BMenuItem("dd-mm-yy", new BMessage(PrefsDateChangeMSG));
	if (prefs->dateFormat == 1)
		mmi = mi;
	m->AddItem(mi);

	dateFormatMF = new BMenuField(BRect(15, 164, 245, 164), "dateFormatMF",
		"Format of Date", m);
	dateFormatMF->SetDivider(160);
	backview->AddChild(dateFormatMF);
	mmi->SetMarked(true);

	str = prefs->dateSep;
	dateSepTC = new BTextControl(BRect(15, 189, 200, 189), "dateSepTC",
		"Date Separator", str.c_str(), 0);
	dateSepTC->SetDivider(160);
	dateSepTC->SetModificationMessage(new BMessage(PrefsDateChangeMSG));
	backview->AddChild(dateSepTC);

	struct tm timestruct;
	timestruct.tm_year = 104;
	timestruct.tm_mon = 9;
	timestruct.tm_mday = 25;
	timestruct.tm_sec = 0; 
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	timestruct.tm_isdst = -1;
	time_t datetime = mktime(&timestruct);

	dateSV = new BStringView(BRect(15, 215, 230, 235), "dateSV",
		prefs->dateToString(datetime).c_str());
	dateSV->SetAlignment(B_ALIGN_CENTER);
	backview->AddChild(dateSV);

	BButton* button = new BButton(BRect(15, 250, 115, 270), "okB",
		"OK", new BMessage(PrefsOKButtonMSG));
	backview->AddChild(button);
	button->MakeDefault(true);
	button = new BButton(BRect(130, 250, 230, 270), "cancelB",
		"Cancel", new BMessage(PrefsCancelButtonMSG));
	backview->AddChild(button);
}

void PrefsWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case PrefsOKButtonMSG:
		{
			BMessenger msngr(returnHandler);
			BMessage* msg = new BMessage(PrefsReturnMSG);
			msg->AddPointer("prefs", prefs);
			msngr.SendMessage(msg);
			Quit();
		}
		break;
		case PrefsCancelButtonMSG:
			Quit();
		break;
		case PrefsCurrencyChangeMSG:
		{
			prefs->curSym = curSymTC->Text();
			prefs->curSymPlaceBefore = 
				strcmp(curPlaceMF->Menu()->FindMarked()->Label(), "Before") ? false : true;
			prefs->curSep = curSepTC->Text();
			prefs->curDec = curDecTC->Text();
			prefs->curDecPlace = atoi(curDecPlaceMF->Menu()->FindMarked()->Label());
			curSV->SetText(prefs->currencyToString(103425).c_str());
		}
		break;
		case PrefsDateChangeMSG:
		{
			struct tm timestruct;
			timestruct.tm_year = 104;
			timestruct.tm_mon = 9;
			timestruct.tm_mday = 25;
			timestruct.tm_sec = 0; 
			timestruct.tm_min = 0; 
			timestruct.tm_hour = 0;
			timestruct.tm_isdst = -1;
			time_t datetime = mktime(&timestruct);
		
			prefs->dateFormat = strcmp(dateFormatMF->Menu()->FindMarked()->Label(), "mm-dd-yy") ? 1 : 0;
			prefs->dateSep = dateSepTC->Text();
			dateSV->SetText(prefs->dateToString(datetime).c_str());
		}
		break;
		default:
			BWindow::MessageReceived(msg);
	}
}