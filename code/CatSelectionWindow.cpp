#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <TextControl.h>
#include "CatSelectionWindow.h"

CatSelectionWindow::CatSelectionWindow(const char* title, 
	BMessage* returnMessage, BHandler* returnHandler, cas* catS) :
	StringInputWindow(title, returnMessage, returnHandler, "")
{
	ResizeBy(20, 0);
	
	catPop = new BPopUpMenu("", true, false);
	BMenuField* mf = new BMenuField(BRect(285, 25, 285, 25),
		"catmenu", "", catPop);
	backdrop->AddChild(mf);

	for (cas::iterator ci = catS->begin(); ci != catS->end(); ci++)
	{
		if ((*ci)->IsRoot())
		{
			catPop->AddItem(MakeCatMenu(*ci));
		}
	}
}

BMenuItem* CatSelectionWindow::MakeCatMenu(Cat* cat, bool enabled)
{
	BMenuItem* mi;
	BMessage* msgs = new BMessage(CatSelectionWindowPopCatMSG);
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

void CatSelectionWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		case CatSelectionWindowPopCatMSG:
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
