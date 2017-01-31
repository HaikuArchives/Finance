#include <stdio.h>
#include <string.h>

#include <Button.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <StringView.h>
#include <TextControl.h>
#include <Window.h>

#include "Acc.h"
#include "CheckView.h"
#include "Pye.h"
#include "SplCat.h"
#include "Tra.h"

BMenuItem* CheckView::MakeCatMenu(Cat* cat, bool enabled)
{
	BMenuItem* mi;
	BMessage* msgs = new BMessage(CheckPopCatMSG);
	msgs->AddString("text", cat->Name().c_str());
	cas* childcats = cat->SetOfChildren();
	
	if (childcats->empty())
	{
		mi = new BMenuItem(cat->LeafName().c_str(), msgs);
		mi->SetEnabled(enabled);
	}
	else
	{
		BMenu* m = new BMenu(cat->LeafName().c_str());
		mi = new BMenuItem(m, msgs);
		mi->SetEnabled(enabled);
		for (cas::iterator ci = childcats->begin();	ci != childcats->end(); ci++)
		{
			m->AddItem(MakeCatMenu(*ci));
		}
	}
	
	return mi;
}

void CheckView::MakeEnterDefault()
{
	enterB->MakeDefault(true);
}

quadruplet<BTextControl*, BPopUpMenu*, BMenuField*, BStringView*> CheckView::MakeField(uint16 width,
	string name, uint16* xpos, uint16* ypos)
{
	BStringView* sv = new BStringView(BRect((*xpos), (*ypos), 
		(*xpos) + width, (*ypos) + 10),
		(name + "Text").c_str(), name.c_str());
	BFont font;
	sv->GetFont(&font);
	font.SetSize(10);
	sv->SetFont(&font);
	AddChild(sv);
	BTextControl* tc = new BTextControl(BRect((*xpos) - 5, (*ypos) + 10, 
		(*xpos) + width, (*ypos) + 10), (name + "Field").c_str(), 
		"", "", 0);
	(*xpos) += width;
	tc->SetDivider(0);
	AddChild(tc);
	BPopUpMenu* pu = new BPopUpMenu("", true, false);
	BMenuField* mf = new BMenuField(BRect((*xpos) + 2, (*ypos) + 9, 
		(*xpos) + 2, (*ypos) + 9), (name + "Menu").c_str(), "", pu);
	mf->SetDivider(0);
	AddChild(mf);
	(*xpos) += 30;
	
	return quadruplet<BTextControl*, BPopUpMenu*, BMenuField*, BStringView*>(tc, pu, mf, sv);
}

void CheckView::MessageReceived(BMessage* message)
{
	const char* text;
	switch(message->what)
	{
		case CheckPopNumMSG:
			if (!message->FindString("text", &text))
				numText->SetText(text);
		break;
		case CheckPopDateMSG:
			if (!message->FindString("text", &text))
				datText->SetText(text);
		break;
		case CheckPopPyeMSG:
			if (!message->FindString("text", &text))
				pyeText->SetText(text);
		break;
		case CheckPopPaymentMSG:
			if (!message->FindString("text", &text))
				pamText->SetText(text);
		break;
		case CheckPopDepositMSG:
			if (!message->FindString("text", &text))
				depText->SetText(text);
		break;
		case CheckPopCatMSG:
			if (!message->FindString("text", &text))
				catText->SetText(text);
		break;
		case ClearSplitMSG:
		{
			delete theSplCat;
			theSplCat = 0;
			catText->SetEnabled(true);
			catText->SetText("");
			RedoCatPopUp();
			pamText->SetEnabled(true);
			depText->SetEnabled(true);
			pamPop->SetEnabled(true);
			depPop->SetEnabled(true);
		}
		break;
		case CheckPopTransferMSG:
		{
			Acc* ta;
			message->FindPointer("acc", (void**)&ta);
			tsfAcc = ta;
			pyeText->SetText(("Transfer with: " + ta->name).c_str());
			pyeText->SetEnabled(false);
			catText->SetText("Transfer");
			catText->SetEnabled(false);
			tsf = true;
			RedoPyePopUp();
			if (theSplCat)
				delete theSplCat;
			theSplCat = 0;
		}
		break;
		case ClearTransferMSG:
			tsf = false;
			pyeText->SetText("");
			catText->SetText("");
			RedoPyePopUp();
			RedoCatPopUp();
			pyeText->SetEnabled(true);
			catText->SetEnabled(true);
		break;
		case CheckPyeTextModMSG:
		{
			// pyeStr = input text
			string pyeStr = pyeText->Text();
			if (pyeStr.length() < 1)
				break;
			Pye pye;
			pye.name = pyeStr;
			pas::iterator iter = pyeS->lower_bound(&pye);
			// (*iter)->name = guess text
			if (iter != pyeS->end())
			{
				string guessStr = (*iter)->name;
				if (pyeStr.length() < guessStr.length())
				{
					if (pyeStr == guessStr.substr(0, pyeStr.length()))
					{
						if (pyeStr != pyePrevStr)
						{
							pyeText->TextView()->Insert(guessStr.substr(pyeStr.length(), guessStr.length() -
								pyeStr.length()).c_str());
							pyeText->TextView()->Select(pyeStr.length(), guessStr.length());
						}
						pyePrevStr = pyeStr;
//						pyePrevGuessStr = guessStr;
					}
				}
			}
		}
		break;
		case CheckCatTextModMSG:
		{
			// pyeStr = input text
			string catStr = catText->Text();
			if (catStr.length() < 1)
				break;
			Cat cat(catStr);
			cas::iterator iter = catS->lower_bound(&cat);
			// (*iter)->name = guess text
			if (iter != catS->end())
			{
				string guessStr = (*iter)->name;
				if (catStr.length() < guessStr.length())
				{
					if (catStr == guessStr.substr(0, catStr.length()))
					{
						if (catStr != catPrevStr)
						{
							catText->TextView()->Insert(guessStr.substr(catStr.length(), guessStr.length() -
								catStr.length()).c_str());
							catText->TextView()->Select(catStr.length(), guessStr.length());
						}
						catPrevStr = catStr;
//						catPrevGuessStr = guessStr;
					}
				}
			}
		}
		break;
		case CheckNumTextModMSG:
		break;
		case CheckDatTextModMSG:
		break;
		default:
			BView::MessageReceived(message);
	}
}

void CheckView::RedoCatPopUp()
{
	EmptyMenu(catPop);
	if (tsf)
	{
		BMenuItem* mi = new BMenuItem("Clear Transfer", new BMessage(ClearTransferMSG));
		catPop->AddItem(mi);
		if (!catS->empty())
			catPop->AddSeparatorItem();
	}
	else if (theSplCat)
	{
		BMenuItem* mi = new BMenuItem("Clear Split", new BMessage(ClearSplitMSG));
		catPop->AddItem(mi);
		catPop->AddSeparatorItem();
	}
	if (curTra && !tsf && curTra->cat)
	{
		BMessage* msg = new BMessage(CheckPopCatMSG);
		msg->AddString("text", curTra->cat->Name().c_str());
		BMenuItem* mi = new BMenuItem(curTra->cat->Name().c_str(), msg);
		catPop->AddItem(mi);
		mi->SetEnabled(theSplCat == 0);
		catPop->AddSeparatorItem();
	}
	for (cas::iterator ci = catS->begin(); ci != catS->end(); ci++)
	{
		if ((*ci)->IsRoot())
		{
			catPop->AddItem(MakeCatMenu(*ci, theSplCat == 0 && 
				!tsf));
		}
	}
}

void CheckView::RedoDatPopUp()
{
	EmptyMenu(datPop);

	if (curTra)
	{
		AddMenuItemToPopUp(CheckPopDateMSG, datPop, prefs->dateToString(curTra->date));
		datPop->AddSeparatorItem();
	}

	time_t now = time(0);
	for (uint8 i = 0; i < 5; i++)
	{
		AddMenuItemToPopUp(CheckPopDateMSG, datPop, prefs->dateToString(now));
		now -= 86400;
	}

	if (!recDatL->empty())
		datPop->AddSeparatorItem();

	for (list<time_t>::iterator di = recDatL->begin();
		di != recDatL->end(); di++)
	{
		AddMenuItemToPopUp(CheckPopDateMSG, datPop, prefs->dateToString(*di));
	}
}

void CheckView::RedoDepPopUp()
{
	EmptyMenu(depPop);

	if (curTra)
	{
		if (!curTra->pam)
		{
			AddMenuItemToPopUp(CheckPopDepositMSG, depPop, prefs->currencyToString(curTra->amt));
			depPop->AddSeparatorItem();
		}
	}
	
	for (u32l::iterator di = recDepL->begin();
		di != recDepL->end(); di++)
	{
		AddMenuItemToPopUp(CheckPopDepositMSG, depPop, prefs->currencyToString(*di));
	}
}

void CheckView::RedoNumPopUp()
{
	char buffer[10];
	EmptyMenu(numPop);

	if (curTra)
	{
		if (curTra->num.type == 0)
		{
			sprintf(buffer, "%i", curTra->num.chknum);
			AddMenuItemToPopUp(CheckPopNumMSG, numPop, buffer);
			numPop->AddSeparatorItem();
		}
		else if (curTra->num.type == 1)
		{
			strcpy(buffer, "ATM");
			AddMenuItemToPopUp(CheckPopNumMSG, numPop, buffer);
			numPop->AddSeparatorItem();
		}
		else if (curTra->num.type == 2)
		{
			strcpy(buffer, "EFT");
			AddMenuItemToPopUp(CheckPopNumMSG, numPop, buffer);
			numPop->AddSeparatorItem();
		}
		else if (curTra->num.type == 3)
		{
			strcpy(buffer, "Xfer");
			AddMenuItemToPopUp(CheckPopNumMSG, numPop, buffer);
			numPop->AddSeparatorItem();
		}
	}

	for (u16l::iterator ci = nxtChkL->begin(); ci != nxtChkL->end(); ci++)
	{
		sprintf(buffer, "%i", *ci);
		AddMenuItemToPopUp(CheckPopNumMSG, numPop, buffer);
	}
	
	if (!nxtChkL->empty())
		numPop->AddSeparatorItem();
	AddMenuItemToPopUp(CheckPopNumMSG, numPop, "ATM");
	AddMenuItemToPopUp(CheckPopNumMSG, numPop, "EFT");
	AddMenuItemToPopUp(CheckPopNumMSG, numPop, "Xfer");
}

void CheckView::RedoPyePopUp()
{
	EmptyMenu(pyePop);

	if (tsf)
	{
		BMenuItem* mi = new BMenuItem("Clear Transfer", new BMessage(ClearTransferMSG));
		pyePop->AddItem(mi);
		pyePop->AddSeparatorItem();
	}

	if (curTra && !tsf && curTra->pye && pyeS->size() > 1)
	{
		AddMenuItemToPopUp(CheckPopPyeMSG, pyePop, 
			curTra->pye->name);
		pyePop->AddSeparatorItem();
	}
	
	for (pas::iterator pi = pyeS->begin(); pi != pyeS->end(); pi++)
	{
		AddMenuItemToPopUp(CheckPopPyeMSG, pyePop, (*pi)->name, !tsf);
	}

	if (!recPyeL->empty())
		pyePop->AddSeparatorItem();

	for (list<Pye*>::iterator pi = recPyeL->begin();
		pi != recPyeL->end(); pi++)
	{
		AddMenuItemToPopUp(CheckPopPyeMSG, pyePop, (*pi)->name, !tsf);
	}
	
	if (!pyeS->empty() && !recPyeL->empty() && accL->size() > 1)
		pyePop->AddSeparatorItem();

	for (acl::iterator iter = accL->begin(); iter != accL->end(); iter++)
	{
		if (*iter != acc)
		{
			BMessage* msg = new BMessage(CheckPopTransferMSG);
			msg->AddPointer("acc", *iter);
			BMenuItem* mi = new BMenuItem(("Transfer with: " + (*iter)->name).c_str(), msg);
			pyePop->AddItem(mi);
		}
	}
}

void CheckView::RedoPamPopUp()
{
	EmptyMenu(pamPop);

	if (curTra)
	{
		if (curTra->pam)
		{
			AddMenuItemToPopUp(CheckPopPaymentMSG, pamPop, prefs->currencyToString(curTra->amt));
			pamPop->AddSeparatorItem();
		}
	}

	for (u32l::iterator pi = recPamL->begin();
		pi != recPamL->end(); pi++)
	{
		AddMenuItemToPopUp(CheckPopPaymentMSG, pamPop, prefs->currencyToString(*pi));
	}
}
