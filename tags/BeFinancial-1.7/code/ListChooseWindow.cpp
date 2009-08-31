#include <Button.h>
#include <ListView.h>
#include <ScrollView.h>
#include <StringView.h>
#include "ListChooseWindow.h"
#include "SyncUtil.h"

ListChooseWindow::ListChooseWindow(list<string>* strL, string title, 
	string text1, string text2, string str) : 
	BWindow(BRect(200, 200, 415, 400), title.c_str(),
	B_FLOATING_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, 
	B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE | B_NOT_CLOSABLE)
{
	BRect rect(200, 200, 415, 400);
	rect.OffsetTo(0, 0);
	BView* backview = new BView(rect, "ListChooseBackView", 0, B_WILL_DRAW);
	backview->SetViewColor(222, 222, 222);
	AddChild(backview);
	
	BStringView* sv = new BStringView(BRect(15, 5, 185, 20),
		"ListChooseSV", text1.c_str());
	backview->AddChild(sv);
	sv = new BStringView(BRect(15, 20, 185, 33),
		"ListChooseSV", text2.c_str());
	backview->AddChild(sv);

	lv = new BListView(BRect(15, 40, 185, 153), "ListChooseLV");
	backview->AddChild(new BScrollView("scrollregister", lv,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true));

	BStringItem* si;
	for (list<string>::iterator iter = strL->begin(); iter !=
		strL->end(); iter++)
	{
		si = new BStringItem((*iter).c_str());
		lv->AddItem(si);
		if (*iter == str)
			si->SetEnabled(false);
	}
	
	BButton* button = new BButton(BRect(15, 165, 95, 185), "okB",
		"OK", new BMessage(ListChooseOKMSG));
	backview->AddChild(button);
	button->MakeDefault(true);
	button = new BButton(BRect(105, 165, 200, 185), "cancelB",
		"Cancel", new BMessage(ListChooseCancelMSG));
	backview->AddChild(button);
	
	clickWait = create_sem(0, "clickWait");
}

string ListChooseWindow::GetStr()
{
	Show();
	WaitForDelete(clickWait);
	return str;
}

void ListChooseWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case ListChooseOKMSG:
		{
			str = ((BStringItem*)lv->ItemAt(lv->CurrentSelection()))->Text();
			delete_sem(clickWait);
			Quit();
		}
		break;
		case ListChooseCancelMSG:
		{
			str = "";
			delete_sem(clickWait);
			Quit();
		}
		break;
	}
}
