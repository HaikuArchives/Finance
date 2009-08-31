#include <stdio.h>
#include <Alert.h>
#include <Button.h>
#include <Message.h>
#include <ListView.h>
#include <ScrollView.h>

#include "Proxy.h"
#include "PyeListItem.h"
#include "PyeManagerWindow.h"
#include "PyeSelectionWindow.h"

PyeManagerWindow::PyeManagerWindow(BHandler* returnHandler, Proxy* theProxy) :
	BWindow(BRect(200, 200, 415, 500), "Payee Manager",
	B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 
	B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE)
{
	this->returnHandler = returnHandler;
	this->theProxy = theProxy;
	pyeS = theProxy->GetPyeS();

	BRect frame = Frame();
	frame.OffsetTo(0, 0);	
	BView* backdrop = new BView(frame, "backview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	backdrop->SetViewColor(222, 222, 222);
	AddChild(backdrop);
	
	lv = new BListView(BRect(10, 10, frame.Width() - 10 - 15, frame.Height() - 70),
		"PyeManagerLV");
	backdrop->AddChild(new BScrollView("scrolllv", lv,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true));
	
	FillList();
	
	BButton* b = new BButton(BRect(10, frame.bottom - 60, 100, frame.bottom - 40), 
		"NewB", "New...", new BMessage(PyeManagerWindowNewMSG));
	backdrop->AddChild(b);
	b = new BButton(BRect(110, frame.bottom - 60, 200, frame.bottom - 40), 
		"RenameB", "Rename...", new BMessage(PyeManagerWindowRenameMSG));
	backdrop->AddChild(b);
	b = new BButton(BRect(10, frame.bottom - 30, 100, frame.bottom - 10), 
		"DeleteB", "Delete...", new BMessage(PyeManagerWindowDeleteMSG));
	backdrop->AddChild(b);
	b = new BButton(BRect(110, frame.bottom - 30, 200, frame.bottom - 10), 
		"CloseB", "Close", new BMessage(PyeManagerWindowCloseMSG));
	backdrop->AddChild(b);
}

void PyeManagerWindow::FillList()
{
	BListItem* li;
	while (!lv->IsEmpty())
	{
		li = lv->LastItem();
		lv->RemoveItem(li);
		delete li;
	}

	for (pas::iterator iter = pyeS->begin(); iter != pyeS->end(); iter++)
	{
		lv->AddItem(new PyeListItem(*iter));
	}
}

void PyeManagerWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		case PyeManagerWindowNewMSG:
		{
			string str = "";
			if (lv->CurrentSelection() >= 0)
				str = ((PyeListItem*)lv->ItemAt(lv->CurrentSelection()))->pye->name;
			StringInputWindow* siw = new StringInputWindow("Name of Payee",
				new BMessage(PyeManagerWindowNewNameMSG), this, str);
			siw->Show();
		}
		break;
		case PyeManagerWindowNewNameMSG:
		{
			string str;
			const char* chstr;	
			message->FindString("string", &chstr);
			str = chstr;
			Pye p;
			p.name = str;
			if (pyeS->find(&p) == pyeS->end())
			{
				Pye* p = new Pye;
				p->name = str;
				theProxy->AddPye(p);
			}
			else
			{
				BAlert* alert = new BAlert("", "Payee already exists.  Try entering a payee that doesn't exist.", "OK");
				alert->Go();
			}
		}
		break;
		case PyeManagerWindowDeleteMSG:
		{
			PyeSelectionWindow* psw = new PyeSelectionWindow(
				"Select a Payee to Replace the Deleted Payee",
				new BMessage(PyeManagerWindowDeleteNameMSG), this, theProxy->GetPyeS());
			psw->Show();
		}
		break;
		case PyeManagerWindowDeleteNameMSG:
		{
			const char* text;
			message->FindString("string", &text);
			Pye p;
			p.name = text;
			pas::iterator iter = pyeS->find(&p);
			if (iter == pyeS->end())
			{
				BAlert* alert = new BAlert("", "Must give a valid payee to replace the deleted payee in transactions", "OK");
				alert->Go();
				break;
			}
			Pye* npye = *iter;
			Pye* pye = ((PyeListItem*)lv->ItemAt(lv->CurrentSelection()))->pye;
			theProxy->DeletePye(pye, npye);
		}
		break;
		case PyeManagerWindowRenameMSG:
		{
			if (lv->CurrentSelection() >= 0)
			{
				string str = ((PyeListItem*)lv->ItemAt(lv->CurrentSelection()))->pye->name;
				StringInputWindow* siw = new StringInputWindow("New Name of Payee",
					new BMessage(PyeManagerWindowRenameNameMSG), this, str);
				siw->Show();
			}
		}
		break;
		case PyeManagerWindowRenameNameMSG:
		{
			Pye* pye = ((PyeListItem*)lv->ItemAt(lv->CurrentSelection()))->pye;
			string str;
			const char* chstr;	
			message->FindString("string", &chstr);
			str = chstr;
			Pye p;
			p.name = str;
			Pye* npye = new Pye;
			npye->name = str;
			if (pyeS->find(&p) == pyeS->end())
				theProxy->AddPye(npye, false);
			else
			{
				BAlert* alert = new BAlert("", "Payee already exists.  Try entering a payee that doesn't exist.", "OK");
				alert->Go();
				break;
			}
			// Delete and change all transactions over to the new one
			theProxy->DeletePye(pye, npye);
		}
		break;
		case PyeManagerWindowCloseMSG:
		{
			BMessage msg(PyeManagerWindowExitMSG);
			returnHandler->MessageReceived(&msg);
			Quit();
		}
		break;
		default:
			BWindow::MessageReceived(message);
	}
}

bool PyeManagerWindow::QuitRequested()
{
	BMessage msg(PyeManagerWindowExitMSG);
	returnHandler->MessageReceived(&msg);
	return true;
}

void PyeManagerWindow::RedoList()
{
	Lock();
	FillList();
	Unlock();
}