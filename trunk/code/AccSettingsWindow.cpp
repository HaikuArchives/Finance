#include <stdio.h>
#include <Alert.h>
#include <Button.h>
#include <String.h>
#include <TextControl.h>
#include "AccSettingsWindow.h"

AccSettingsWindow::AccSettingsWindow(Acc* acc, BHandler* returnHandler,
	Prefs* prefs) :	BWindow(BRect(200, 200, 415, 300), (acc->name + " Settings").c_str(),
	B_FLOATING_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, 
	B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE)
{
	this->returnHandler = returnHandler;
	this->acc = acc;
	this->prefs = prefs;

	BRect rect(200, 200, 415, 400);
	rect.OffsetTo(0, 0);
	BView* backview = new BView(rect, "AccChooseBackView", 0, B_WILL_DRAW);
	backview->SetViewColor(222, 222, 222);
	AddChild(backview);

	openBalTC = new BTextControl(BRect(15, 15, 200, 15), "openBalTC",
		"Opening Balance", prefs->currencyToString(acc->openBal).c_str(), 0);
	openBalTC->SetAlignment(B_ALIGN_LEFT, B_ALIGN_RIGHT);
	backview->AddChild(openBalTC);

	BButton* button = new BButton(BRect(15, 65, 100, 85), "okB",
		"OK", new BMessage(AccSetOKButtonMSG));
	backview->AddChild(button);
	button->MakeDefault(true);
	button = new BButton(BRect(115, 65, 200, 85), "cancelB",
		"Cancel", new BMessage(AccSetCancelButtonMSG));
	backview->AddChild(button);
}

void AccSettingsWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case AccSetOKButtonMSG:
		{
			string openbaltext = openBalTC->Text();
			acc->openBal = prefs->stringToCurrency(openbaltext);

			#ifdef DEBUG
			printf("%s %f %li\n", openbaltext.c_str(), val, acc->openBal);
			#endif
			BMessenger msngr(returnHandler);
			BMessage* msg = new BMessage(AccSetReturnMSG);
			msg->AddPointer("acc", acc);
			msngr.SendMessage(msg);
			Quit();
		}
		break;
		case AccSetCancelButtonMSG:
			Quit();
		break;
	}
}