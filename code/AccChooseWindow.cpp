#include <Button.h>
#include <ListView.h>
#include <ScrollView.h>
#include <StringView.h>
#include "AccChooseWindow.h"
#include "AccListItem.h"
#include "SyncUtil.h"

AccChooseWindow::AccChooseWindow(list<Acc*>* accL, string title, 
	string text1, string text2, Acc* acc, Prefs* prefs) : 
	BWindow(BRect(200, 200, 415, 400), title.c_str(),
	B_FLOATING_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, 
	B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE | B_NOT_CLOSABLE)
{
	BRect rect(200, 200, 415, 400);
	rect.OffsetTo(0, 0);
	BView* backview = new BView(rect, "AccChooseBackView", 0, B_WILL_DRAW);
	backview->SetViewColor(222, 222, 222);
	AddChild(backview);
	
	BStringView* sv = new BStringView(BRect(15, 5, 185, 20),
		"AccChooseSV", text1.c_str());
	backview->AddChild(sv);
	sv = new BStringView(BRect(15, 20, 185, 33),
		"AccChooseSV", text2.c_str());
	backview->AddChild(sv);

	lv = new BListView(BRect(15, 40, 185, 153), "AccChooseLV");
	backview->AddChild(new BScrollView("scrollregister", lv,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true));

	AccListItem* ali;
	for (list<Acc*>::iterator iter = accL->begin(); iter !=
		accL->end(); iter++)
	{
		ali = new AccListItem(*iter, prefs);
		lv->AddItem(ali);
		if (*iter == acc)
			ali->SetEnabled(false);
	}
	
	BButton* button = new BButton(BRect(15, 165, 95, 185), "okB",
		"OK", new BMessage(AccChooseOKMSG));
	backview->AddChild(button);
	button->MakeDefault(true);
	button = new BButton(BRect(105, 165, 200, 185), "skipB",
		"Skip Transaction", new BMessage(AccChooseSkipTraMSG));
	backview->AddChild(button);
	
	clickWait = create_sem(0, "clickWait");
}

Acc* AccChooseWindow::GetAcc()
{
	Show();
	WaitForDelete(clickWait);
	return acc;
}

void AccChooseWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case AccChooseOKMSG:
		{
			acc = ((AccListItem*)lv->ItemAt(lv->CurrentSelection()))->acc;
			delete_sem(clickWait);
			Quit();
		}
		break;
		case AccChooseSkipTraMSG:
		{
			acc = 0;
			delete_sem(clickWait);
			Quit();
		}
		break;
	}
}
