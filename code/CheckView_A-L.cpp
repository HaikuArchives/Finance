#include <stdexcept>
#include <stdio.h>
#include <Alert.h>
#include <Button.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <StringView.h>
#include <TextControl.h>

#include "Acc.h"
#include "CheckView.h"
#include "Pye.h"
#include "SplCat.h"
#include "Tra.h"

#define NumWidth		40
#define DateWidth		60
#define PyeWidth		150
#define PaymentWidth	60
#define DepositWidth	60
#define CatWidth		225
#define MemoWidth		225

CheckView::CheckView(BRect frame, cas* catS, pas* pyeS, u16l* nxtChkL, til* recDatL, 
	pal* recPyeL, u32l* recPamL, u32l* recDepL, acl* accL, Acc* acc, Prefs* prefs) : 
	BView(frame, "CheckView", B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_WILL_DRAW)
{
	#ifdef DEBUG
	printf("CheckView::CheckView\n");
	#endif

	this->catS = catS;
	this->pyeS = pyeS;
	this->nxtChkL = nxtChkL;
	this->recDatL = recDatL;
	this->recPyeL = recPyeL;
	this->recPamL = recPamL;
	this->recDepL = recDepL;
	this->accL = accL;
	this->acc = acc;
	this->prefs = prefs;

	uint16 xpos = 20;
	uint16 ypos = 20;
	SetViewColor(236, 240, 255);
	frame.OffsetTo(0, 0);

	quadruplet<BTextControl*, BPopUpMenu*, BMenuField*, BStringView*> p = 
		MakeField(NumWidth, "Num", &xpos, &ypos);
	numText = p.first;
	numPop = p.second;
	numText->SetModificationMessage(new BMessage(CheckNumTextModMSG));

	p = MakeField(DateWidth, "Date", &xpos, &ypos);
	datText = p.first;
	datPop = p.second;
	datText->SetModificationMessage(new BMessage(CheckDatTextModMSG));

	p = MakeField(PyeWidth, "Payee", &xpos, &ypos);
	pyeText = p.first;
	pyePop = p.second;
	pyeSV = p.fourth;
	pyeText->SetModificationMessage(new BMessage(CheckPyeTextModMSG));

	p = MakeField(PaymentWidth, "Payment", &xpos, &ypos);
	pamText = p.first;
	pamPop = p.second;

	p = MakeField(DepositWidth, "Deposit", &xpos, &ypos);
	depText = p.first;
	depPop = p.second;

	xpos = 20;
	ypos += 40;
	p = MakeField(CatWidth, "Category", &xpos, &ypos);
	catText = p.first;
	catPop = p.second;
	catMF = p.third;
	catSV = p.fourth;
	catText->SetModificationMessage(new BMessage(CheckCatTextModMSG));

	BStringView* sv = new BStringView(BRect(xpos, ypos, xpos + MemoWidth, ypos + 10),
		"MemoText", "Memo");
	BFont font;
	sv->GetFont(&font);
	font.SetSize(10);
	sv->SetFont(&font);
	AddChild(sv);
	memText = new BTextControl(BRect(xpos - 5, ypos + 10, 
		xpos + MemoWidth, ypos + 10), "MemoField", "", "", 0);
	xpos += MemoWidth;
	memText->SetDivider(0);
	AddChild(memText);
	
	ypos += 50;
	xpos = 20;
	
	BButton* b = new BButton(BRect(xpos, ypos, xpos + 100, ypos),
		"SplitB", "Split...", new BMessage(CheckSplitPressMSG));
	AddChild(b);
	xpos += 125;
	b = new BButton(BRect(xpos, ypos, xpos + 100, ypos),
		"NewB", "New", new BMessage(CheckNewPressMSG));
	AddChild(b);
	xpos += 125;
	deleteB = new BButton(BRect(xpos, ypos, xpos + 100, ypos),
		"DeleteB", "Delete", new BMessage(CheckDeletePressMSG));
	deleteB->SetEnabled(false);
	AddChild(deleteB);
	xpos += 125;
	enterB = new BButton(BRect(xpos, ypos, xpos + 100, ypos),
		"EnterB", "Enter", new BMessage(CheckEnterPressMSG));
	enterB->MakeDefault(true);
	AddChild(enterB);

	UpdateTra(0);
}

CheckView::~CheckView()
{
	#if DEBUG
	printf("CheckView::~CheckView\n");
	#endif
	if (theSplCat)
		delete theSplCat;
}

void CheckView::AddMenuItemToPopUp(uint32 command, BPopUpMenu* pu,
	string textstr, bool enabled = true)
{
	BMessage* msg = new BMessage(command);
	msg->AddString("text", textstr.c_str());
	BMenuItem* mi = new BMenuItem(textstr.c_str(), msg);
	pu->AddItem(mi);
	mi->SetEnabled(enabled);
}

void CheckView::Draw(BRect updateRect)
{
	BRect frame = Frame();
	frame.OffsetTo(0, 0);
	SetHighColor(156, 156, 156);
	StrokeRect(frame);
}

void CheckView::EmptyMenu(BMenu* menu)
{
	BMenuItem* mi;
	while (menu->CountItems() > 0)
	{
		if (menu->SubmenuAt(0))
			EmptyMenu(menu->SubmenuAt(0));
		mi = menu->RemoveItem((int32)0);
		if (mi)
			delete mi;
	}
}

pair<uint32, uint8> CheckView::GetAmount()
{
	//Returns <amt, pam?>
	//pam = 0 for dep
	//			1 for pam
	//			2 for neither
	//			3 for error
	string amttext;
	string ptext = pamText->Text();
	string dtext = depText->Text();
	if (ptext.length() > 0 && dtext.length() > 0)
	{
		BAlert* alert = new BAlert("Invalid Transaction", 
			"Invalid Transaction.  Must have a deposit or payment.", "OK");
		alert->Go();
		pair<uint32, uint8> p(0, 3);
		return p;
	}
	if (ptext.length() == 0 && dtext.length() == 0)
	{
		pair<uint32, uint8> p(0, 2);
		return p;
	}
	pair<uint32, uint8> p;
	if (ptext.length() > 0)
	{
		amttext = ptext;
		p.second = 1;
	}
	else
	{
		amttext = dtext;
		p.second = 0;
	}

	int32 val = prefs->stringToCurrency(amttext);
	if (val < 0)
	{
		BAlert* alert = new BAlert("Invalid Transaction", 
			"Invalid Transaction.  Amount cannot be negative.", "OK");
		alert->Go();
		p.first = 0;
		p.second = 3;
		return p;
	}
	
	p.first = val;
	return p;
}

Cat* CheckView::GetCat()
{
	if (theSplCat)
		return theSplCat;
	Cat c(catText->Text());
	cas::iterator csi = catS->find(&c);
	if (csi == catS->end())
		return 0;
	return *csi;
}

string CheckView::GetCatText()
{
	return string(catText->Text());
}

Pye* CheckView::GetPye()
{
	Pye p;
	p.name = pyeText->Text();
	pas::iterator psi = pyeS->find(&p);
	if (psi == pyeS->end())
		return 0;
	return *psi;
}

string CheckView::GetPyeText()
{
	return string(pyeText->Text());
}

SplCat* CheckView::GetSplCat()
{
	return theSplCat;
}

Tra* CheckView::GetTra(Cat* cat, Pye* pye, bool alert = true)
{
	#if DEBUG
	printf("CheckView::GetTra\n");
	#endif
	Tra* tra;
	try
	{
		tra = new Tra(numText->Text(),
			datText->Text(), pye, pamText->Text(),
			depText->Text(), cat, memText->Text(), 0,
			tsf, tsfAcc, prefs);
	}
	catch(invalid_argument ia)
	{
		if (alert)
		{
			BAlert* a = new BAlert("Invalid Transaction", 
				ia.what(), "OK");
			a->Go();
		}
		return 0;
	}
	return tra;
}

bool CheckView::IsTransfer()
{
	return tsf;
}