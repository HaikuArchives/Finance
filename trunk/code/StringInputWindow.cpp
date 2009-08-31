#include <Button.h>
#include <TextControl.h>
#include "StringInputWindow.h"
#include "SyncUtil.h"

StringInputWindow::StringInputWindow(const char* title, BMessage*
	returnMessage, BHandler* returnHandler, string text = "",
	bool modal = false) : BWindow(BRect(200, 200, 500, 300), title, 
	B_FLOATING_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, 
	B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE)
{
	this->returnMessage = returnMessage;
	this->returnHandler = returnHandler;
	this->modal = modal;
	backdrop = new BView(BRect(0, 0, 300, 100), "view", B_FOLLOW_ALL_SIDES, 0);
	backdrop->SetViewColor(222, 222, 222);
	AddChild(backdrop);
	BButton* OKbutton = new BButton(BRect(125, 65, 175, 85), "OKButton",
		"OK", new BMessage(OKButtonMSG));
	backdrop->AddChild(OKbutton);
	OKbutton->MakeDefault(true);
	stringTC = new BTextControl(BRect(25, 25, 275, 25), "stringTC",
		"", text.c_str(), new BMessage(OKButtonMSG));
	stringTC->SetDivider(0);
	backdrop->AddChild(stringTC);
	stringTC->MakeFocus(true);

	if (modal)
		clickWait = create_sem(0, "clickWait");
}

string StringInputWindow::GetString()
{
	if (!modal)
		return "";
	Show();
	WaitForDelete(clickWait);
	return str;
}

void StringInputWindow::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		case OKButtonMSG:
		{
			str = stringTC->Text();
			if (!modal)
			{
				BMessenger msngr(returnHandler);
				returnMessage->AddString("string", str.c_str());
				msngr.SendMessage(returnMessage);
			}
			else
				delete_sem(clickWait);
			Quit();
		}
		break;
		default:
			BWindow::MessageReceived(message);
	}
}