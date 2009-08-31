#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <TextControl.h>
#include "PyeSelectionWindow.h"

PyeSelectionWindow::PyeSelectionWindow(const char* title, 
	BMessage* returnMessage, BHandler* returnHandler, pas* pyeS) :
	StringInputWindow(title, returnMessage, returnHandler, "")
{
	ResizeBy(20, 0);
	
	pyePop = new BPopUpMenu("", true, false);
	BMenuField* mf = new BMenuField(BRect(285, 25, 285, 25),
		"pyemenu", "", pyePop);
	backdrop->AddChild(mf);

	for (pas::iterator pi = pyeS->begin(); pi != pyeS->end(); pi++)
	{
		BMessage* msgs = new BMessage(PyeSelectionWindowPopPyeMSG);
		msgs->AddString("text", (*pi)->name.c_str());
		pyePop->AddItem(new BMenuItem((*pi)->name.c_str(), msgs));
	}
}

void PyeSelectionWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		case PyeSelectionWindowPopPyeMSG:
		{
			const char* text;
			message->FindString("text", &text);
			stringTC->SetText(text);
		}
		break;
		default:
			StringInputWindow::MessageReceived(message);
		break;
	}
}