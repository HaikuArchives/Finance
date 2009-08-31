#include <Alert.h>
#include <Button.h>
#include <Message.h>
#include <OutlineListView.h>
#include <ScrollView.h>
#include <stdio.h>

#include "CatListItem.h"
#include "CatManagerWindow.h"
#include "Proxy.h"
#include "CatSelectionWindow.h"

CatManagerWindow::CatManagerWindow(BHandler* returnHandler, Proxy* theProxy) :
	BWindow(BRect(200, 200, 415, 500), "Category Manager",
	B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 
	B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE)
{
	this->returnHandler = returnHandler;
	this->theProxy = theProxy;
	catS = theProxy->GetCatS();

	BRect frame = Frame();
	frame.OffsetTo(0, 0);	
	BView* backdrop = new BView(frame, "backview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	backdrop->SetViewColor(222, 222, 222);
	AddChild(backdrop);
	
	lv = new BOutlineListView(BRect(10, 10, frame.Width() - 10 - 15, frame.Height() - 70),
		"CatManagerLV");
	backdrop->AddChild(new BScrollView("scrolllv", lv,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true));
	
	BButton* b = new BButton(BRect(10, frame.bottom - 60, 100, frame.bottom - 40), 
		"NewB", "New...", new BMessage(CatManagerWindowNewMSG));
	backdrop->AddChild(b);
	b = new BButton(BRect(110, frame.bottom - 60, 200, frame.bottom - 40), 
		"RenameB", "Rename...", new BMessage(CatManagerWindowRenameMSG));
	backdrop->AddChild(b);
	b = new BButton(BRect(10, frame.bottom - 30, 100, frame.bottom - 10), 
		"DeleteB", "Delete...", new BMessage(CatManagerWindowDeleteMSG));
	backdrop->AddChild(b);
	b = new BButton(BRect(110, frame.bottom - 30, 200, frame.bottom - 10), 
		"CloseB", "Close", new BMessage(CatManagerWindowCloseMSG));
	backdrop->AddChild(b);
	
	FillList();
}

void CatManagerWindow::FillList()
{
	BListItem* li;
	while (!lv->IsEmpty())
	{
		li = lv->LastItem();
		lv->RemoveItem(li);
		delete li;
	}

	for (cas::iterator iter = catS->begin(); iter != catS->end(); iter++)
	{
		if ((*iter)->IsRoot())
		{
			if (!(*iter)->SetOfChildren()->empty())
				RecAddCat(0, *iter);
			else
				lv->AddItem(new CatListItem(*iter));
		}
	}
}

void CatManagerWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		case CatManagerWindowNewMSG:
		{
			string str = "";
			if (lv->CurrentSelection() >= 0)
				str = ((CatListItem*)lv->ItemAt(lv->CurrentSelection()))->cat->Name();
			StringInputWindow* siw = new StringInputWindow("Name of Category",
				new BMessage(CatManagerWindowNewNameMSG), this, str);
			siw->Show();
		}
		break;
		case CatManagerWindowNewNameMSG:
		{
			string str;
			const char* chstr;	
			message->FindString("string", &chstr);
			str = chstr;
			Cat c(str);
			if (catS->find(&c) == catS->end())
				theProxy->AddCat(str);
			else
			{
				BAlert* alert = new BAlert("", "Category already exists.  Try entering a category that doesn't exist.", "OK");
				alert->Go();
			}
		}
		break;
		case CatManagerWindowDeleteMSG:
		{
			CatSelectionWindow* csw = new CatSelectionWindow(
				"Select a Category to Replace the Deleted Category",
				new BMessage(CatManagerWindowDeleteNameMSG), this, theProxy->GetCatS());
			csw->Show();
		}
		break;
		case CatManagerWindowCloseMSG:
		{
			BMessage msg(CatManagerWindowExitMSG);
			returnHandler->MessageReceived(&msg);
			Quit();
		}
		break;
		case CatManagerWindowDeleteNameMSG:
		{
			const char* text;
			message->FindString("string", &text);
			Cat c(text);
			cas::iterator iter = catS->find(&c);
			if (iter == catS->end())
			{
				BAlert* alert = new BAlert("", "Must give a valid category to replace the deleted category in transactions", "OK");
				alert->Go();
				break;
			}
			Cat* ncat = *iter;
			Cat* cat = ((CatListItem*)lv->ItemAt(lv->CurrentSelection()))->cat;
			theProxy->DeleteCat(cat, ncat);
		}
		break;
		case CatManagerWindowRenameMSG:
		{
			if (lv->CurrentSelection() >= 0)
			{
				string str = ((CatListItem*)lv->ItemAt(lv->CurrentSelection()))->cat->Name();
				StringInputWindow* siw = new StringInputWindow("New Name of Category",
					new BMessage(CatManagerWindowRenameNameMSG), this, str);
				siw->Show();
			}
		}
		break;
		case CatManagerWindowRenameNameMSG:
		{
			Cat* cat = ((CatListItem*)lv->ItemAt(lv->CurrentSelection()))->cat;
			string str;
			const char* chstr;	
			message->FindString("string", &chstr);
			str = chstr;
			Cat c(str);
			Cat* ncat = 0;
			if (catS->find(&c) == catS->end())
				ncat = theProxy->AddCat(str, false);
			else
			{
				BAlert* alert = new BAlert("", "Category already exists.  Try entering a category that doesn't exist.", "OK");
				alert->Go();
				break;
			}
			// Delete and change all transactions over to the new one
			theProxy->DeleteCat(cat, ncat);
		}
		break;
		default:
			BWindow::MessageReceived(message);
		break;
	}
}

bool CatManagerWindow::QuitRequested()
{
	BMessage msg(CatManagerWindowExitMSG);
	returnHandler->MessageReceived(&msg);
	return true;
}

void CatManagerWindow::RecAddCat(uint8 lvl, Cat* cat)
{
	lv->AddItem(new CatListItem(cat, lvl));
	if (!cat->SetOfChildren()->empty())
	{
		for (cas::iterator iter = cat->SetOfChildren()->begin(); 
			iter != cat->SetOfChildren()->end(); iter++)
			RecAddCat(lvl + 1, *iter);
	}
}

void CatManagerWindow::RedoList()
{
	Lock();
	FillList();
	Unlock();
}