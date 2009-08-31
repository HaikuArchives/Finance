#include <stdio.h>
#include <Alert.h>
#include <Button.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <RadioButton.h>
#include <StringView.h>
#include <TextControl.h>

#include "Proxy.h"
#include "SplCat.h"
#include "SplitWindow.h"

SplitWindow::SplitWindow(BRect frame, Proxy* theProxy, BHandler* targetHandler, Prefs* prefs,
	uint32 total = 0, uint8 pam = 1, SplCat* sc = 0) : 
	BWindow(frame, "Transaction Split", B_FLOATING_WINDOW_LOOK, 
	B_MODAL_APP_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE)
{
	#ifdef DEBUG
	printf("SplitWindow::SplitWindow\n");
	printf("\tsc = %p\n", sc);
	#endif

	this->prefs = prefs;

//	char buffer[20];
	frame.OffsetTo(0, 0);
	v = new BView(frame, "backview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	v->SetViewColor(222, 222, 222);
	AddChild(v);
	BButton* b = new BButton(BRect(20, frame.bottom - 40, 100,
		frame.bottom - 20), "AddCatButton", "Add Category",
		new BMessage(AddCatMSG), B_FOLLOW_BOTTOM);
	v->AddChild(b);
	b = new BButton(BRect(frame.right - 100, frame.bottom - 40,
		frame.right - 20, frame.bottom - 20), "SplitEnter", "Enter",
		new BMessage(SplitEnterMSG), B_FOLLOW_BOTTOM);
	b->MakeDefault(true);
	v->AddChild(b);
	BStringView* sv = new BStringView(BRect(frame.right - 150,
		frame.bottom - 75, frame.right - 70, frame.bottom - 55),
		"TotalField", "Total", B_FOLLOW_BOTTOM);
	v->AddChild(sv);
	sv = new BStringView(BRect(frame.right - 150, frame.bottom - 95, 
		frame. right - 100, frame.bottom - 75), "DifferenceField", 
		"Difference", B_FOLLOW_BOTTOM);
	v->AddChild(sv);

	totalText = new BStringView(BRect(frame.right - 85,
		frame.bottom - 75, frame.right - 20, frame.bottom - 55),
		"TotalNumField", prefs->currencyToString(total).c_str(), B_FOLLOW_BOTTOM);
	totalText->SetAlignment(B_ALIGN_RIGHT);
	v->AddChild(totalText);
	diffText = new BStringView(BRect(frame.right - 85,
		frame.bottom - 95, frame.right - 20, frame.bottom - 75),
		"DiffNumField", prefs->currencyToString(total).c_str(), B_FOLLOW_BOTTOM);
	diffText->SetAlignment(B_ALIGN_RIGHT);
	v->AddChild(diffText);
	depRadio = new BRadioButton(BRect(120, frame.bottom - 45,
		180, frame.bottom - 45), "DepositRadioButton", "Deposit", 0,
		B_FOLLOW_BOTTOM);
	v->AddChild(depRadio);
	pamRadio = new BRadioButton(BRect(120, frame.bottom - 25,
		180, frame.bottom - 25), "PaymentRadioButton", "Payment", 0,
		B_FOLLOW_BOTTOM);
	v->AddChild(pamRadio);
	if (pam == 0)
		depRadio->SetValue(1);
	else if (pam == 1)
		pamRadio->SetValue(1);

	ypos = 25;
	this->total = total;
	this->pam = pam;
	this->theProxy = theProxy;
	this->catS = theProxy->GetCatS();
	this->targetHandler = targetHandler;
	catnum = 0;

	if (!sc)
		AddCat();
	else
	{
		list<int32>::iterator itera = sc->amtL->begin();

		for (cal::iterator iterc = sc->catL->begin(); iterc !=
			sc->catL->end(); iterc++)
		{
			AddCat();
			(*catTexts.rbegin())->SetText((*iterc)->Name().c_str());
			(*amtTexts.rbegin())->SetText(prefs->currencyToString(*itera).c_str());
			itera++;
		}
	}
}

void SplitWindow::AddCat()
{
	ResizeBy(0, 25);
	uint16 xpos = 20;
	uint16 width = 150;
	BTextControl* tc = new BTextControl(BRect(xpos - 5, ypos + 10, 
		xpos + width, ypos + 10), "CatField", "", "", 0);
	xpos += width;
	tc->SetDivider(0);
	catTexts.push_back(tc);
	v->AddChild(tc);
	BPopUpMenu* pu = new BPopUpMenu("", true, false);
	BMenuField* mf = new BMenuField(BRect(xpos + 2, ypos + 9, 
		xpos + 2, ypos + 9), "CatMenu", "", pu);
	mf->SetDivider(0);
	v->AddChild(mf);
	xpos += 30;
	tc = new BTextControl(BRect(xpos - 5, ypos + 10, 
		xpos + 75, ypos + 10), "AmountField", "", "", 0);
	tc->SetDivider(0);
	tc->SetAlignment(B_ALIGN_LEFT, B_ALIGN_RIGHT);
	tc->SetModificationMessage(new BMessage(AmountModifiedMSG));
	amtTexts.push_back(tc);
	v->AddChild(tc);
	ypos += 25;

	for (cas::iterator ci = catS->begin(); ci != catS->end(); ci++)
	{
		if ((*ci)->IsRoot())
		{
			pu->AddItem(MakeCatMenu(*ci));
		}
	}
	catnum++;
}

Cat* SplitWindow::GetCat(string text)
{
	Cat* cat = 0;
	for (cas::iterator cli = catS->begin(); cli != catS->end(); cli++)
	{
		if ((*cli)->Name() == text)
			cat = *cli;
	}
	return cat;
}

BMenuItem* SplitWindow::MakeCatMenu(Cat* cat)
{
	BMenuItem* mi;
	BMessage* msgs = new BMessage(CatPopUpMSG);
	msgs->AddString("text", cat->Name().c_str());
	msgs->AddInt8("num", catnum);
	cas* childcats = cat->SetOfChildren();
	
	if (childcats->empty())
	{
		mi = new BMenuItem(cat->LeafName().c_str(), msgs);
	}
	else
	{
		BMenu* m = new BMenu(cat->LeafName().c_str());
		mi = new BMenuItem(m, msgs);
		for (cas::iterator ci = childcats->begin();	ci != childcats->end(); ci++)
		{
			m->AddItem(MakeCatMenu(*ci));
		}
	}
	
	return mi;
}

void SplitWindow::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case AddCatMSG:
			AddCat();
		break;
		case AmountModifiedMSG:
		{
			int32 splitSum = SplitSum();
			if (total == 0)
			{
				totalText->SetText(prefs->currencyToString(splitSum).c_str());
			}
			else
			{
				splitSum = total - splitSum;
				diffText->SetText(prefs->currencyToString(splitSum).c_str());
			}
		}
		break;
		case CatPopUpMSG:
		{
			int8 i8;
			const char* str;
			msg->FindInt8("num", &i8);
			msg->FindString("text", &str);
			catTexts[i8]->SetText(str);
		}
		break;
		case SplitEnterMSG:
		{
			#ifdef DEBUG
			printf("SplitWindow::MessageReceived:SplitEnterMSG\n");
			#endif
			if (!(depRadio->Value() || pamRadio->Value()))
			{
				BAlert* alert = new BAlert("Split Error", 
					"Must choose either Payment or Deposit", "OK");
				alert->Go();
				return;
			}

			int32 splitSum = SplitSum();
			if (total < 0)
			{
				BAlert* alert = new BAlert("Split Error",
					"Total Amount is Negative.",
					"Change Payment/Deposit & Continue", "Cancel");
				if (alert->Go() == 1)
					return;
				else
					pamRadio->SetEnabled(depRadio->Value());
			}
			if (total != 0 && splitSum != total)
			{
				BAlert* alert = new BAlert("Split Error",
					"Total of Categories does not equal Total Amount given.",
					"Change Total & Continue", "Cancel");
				if (alert->Go() == 1)
					return;
			}

			tcv::iterator iterc = catTexts.begin();
			uint8 i = 1;
			cal* scatlist = new list<Cat*>;
			i32l* amtlist = new list<int32>;
			int32 amt;
			string sumtext;
			for (tcv::iterator itera = amtTexts.begin(); itera != amtTexts.end();
				itera++)
			{
				sumtext = (*itera)->Text();
				amt = prefs->stringToCurrency(sumtext);
				#ifdef DEBUG
				printf("\t%li\n", amt);
				#endif
				string cattext = (*iterc)->Text();
				if (cattext.length() == 0 && amt != 0)
				{
					char buffer[30];
					sprintf(buffer, "Amount but no Category given for Line %i", i);
					BAlert* alert = new BAlert("Split Error", buffer, "OK");
					alert->Go();
					delete scatlist;
					return;
				}
				if (amt != 0)
				{
					Cat* thisCat;
					if ((thisCat = GetCat(cattext)) == 0)
						thisCat = theProxy->AddCat(cattext);
					scatlist->push_back(thisCat);
					amtlist->push_back((splitSum < 0 ? -1 : 1) * amt);
				}
				i++;
				iterc++;
			}
			Cat* sc;
			if (scatlist->size() == 0)
			{
				BAlert* alert = new BAlert("Split Error", "No Valid Categories", "OK");
				alert->Go();
				delete scatlist;
				delete amtlist;
				return;
			}
			else if (scatlist->size() == 1)
			{
				sc = scatlist->front();
			}
			else
			{
				sc = new SplCat(scatlist, amtlist, prefs);
			}
			BMessenger msngr(targetHandler);
			BMessage* msg = new BMessage(SplitMSG);
			msg->AddPointer("cat", sc);
			msg->AddBool("pam", pamRadio->Value());
			msg->AddInt32("amt", splitSum);
			msngr.SendMessage(msg);
			Lock();
			Quit();
		}
		break;
		default:
			BWindow::MessageReceived(msg);
	}
}

int32 SplitWindow::SplitSum()
{
	int32 splitSum = 0;
	string sumtext;
	for (tcv::iterator iter = amtTexts.begin(); iter != amtTexts.end();
		iter++)
	{
		sumtext = (*iter)->Text();
		splitSum += prefs->stringToCurrency(sumtext);
	}
	return splitSum;
}