#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <Alert.h>
#include <Button.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <NodeInfo.h>

#include "Acc.h"
#include "AccChooseWindow.h"
#include "AccSettingsWindow.h"
#include "AccListView.h"
#include "Cat.h"
#include "CatManagerWindow.h"
#include "CheckView.h"
#include "LineGraph.h"
#include "ListChooseWindow.h"
#include "MainView.h"
#include "Pye.h"
#include "PieGraph.h"
#include "Proxy.h"
#include "PyeManagerWindow.h"
#include "ReconcileView.h"
#include "RegisterView.h"
#include "SplitWindow.h"

MainView::MainView(BRect frame, Proxy* theProxy) : BView(frame, "MainView", 
	B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS)
{
	#if DEBUG
	printf("MainView::MainView\n");
	#endif
	this->theProxy = theProxy;
	alv = new AccListView(BRect(563, 430, 690, 535), theProxy->GetPrefs());
	AddChild(alv);
	
	reconB = new BButton(BRect(695, 430, 790, 450), "reconB",
		"Reconcile", new BMessage(ReconcileMSG), B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT);
	AddChild(reconB);
	reconB->SetEnabled(false);
	impB = new BButton(BRect(695, 460, 790, 480), "impB",
		"Import...", new BMessage(ImportMSG), B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT);
	AddChild(impB);
	impB->SetEnabled(false);
	expB = new BButton(BRect(695, 520, 790, 540), "expB",
		"Export...", new BMessage(ExportMSG), B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT);
	AddChild(expB);
	expB->SetEnabled(false);
	setB = new BButton(BRect(695, 490, 790, 510), "setB",
		"Settings...", new BMessage(SettingsMSG), B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT);
	AddChild(setB);
	setB->SetEnabled(false);
	
	curAvs = 0;
	curAcc = 0;
	
	pmw = 0;
	cmw = 0;
}

MainView::~MainView()
{
}

void MainView::AccUpdated(Acc* acc)
{
	#ifdef DEBUG
	printf("MainView::AccUpdated\n");
	#endif
	AccVws[acc]->rv->InvalidateLV();
	AccVws[acc]->lg->InvalidateData();
	alv->InvalidateAccLI(acc);
}

void MainView::AddTra(Acc* acc, Tra* tra, uint16 pos)
{
	#ifdef DEBUG
	printf("MainView::AddTra - acc = %p, tra = %p, pos = %i\n", acc, tra, pos);
	#endif
	AccViewSet* avs = AccVws[acc];
	avs->rv->AddTra(tra, pos);
	alv->InvalidateAccLI(acc);
}

void MainView::AddTra(Tra* tra, uint16 pos)
{
	#if DEBUG
	printf("MainView::AddTra\n");
	#endif
	if (!curAvs)
		throw logic_error("Pointer is Null");
	curAvs->rv->AddTra(tra, pos);
	curAvs->pg->Invalidate();
	curAvs->lg->InvalidateData();
	for (map<Acc*, AccViewSet*>::iterator iter = AccVws.begin();
		iter != AccVws.end(); iter++)
	{
		(*iter).second->cv->UpdatePopUps();
	}
	alv->InvalidateAccLI(curAcc);
}

void MainView::CatLChanged()
{
	for (map<Acc*, AccViewSet*>::iterator iter = AccVws.begin();
		iter != AccVws.end(); iter++)
		(*iter).second->cv->RedoCatPopUp();
	if (cmw)
		cmw->RedoList();
	Window()->Lock();
	curAvs->pg->Invalidate();
	Window()->Unlock();
}

void MainView::ClearViews()
{
	for (map<Acc*, AccViewSet*>::iterator iter = AccVws.begin();
		iter != AccVws.end(); iter++)
	{
		RemoveChild((*iter).second->cv);
		RemoveChild((*iter).second->rv);
		RemoveChild((*iter).second->pg);
		RemoveChild((*iter).second->lg);
		RemoveChild((*iter).second->rcv);
	}
	AccVws.erase(AccVws.begin(), AccVws.end());
	alv->RemoveAll();
	reconB->SetEnabled(false);
	curAvs = 0;
	curAcc = 0;
}

string MainView::CurAccName()
{
	return curAcc->name;
}

void MainView::DeleteTra(uint16 pos)
{
	curAvs->rv->DeleteTra(pos);
	curAvs->pg->Invalidate();
	curAvs->lg->InvalidateData();
	alv->InvalidateAccLI(curAcc);
}

void MainView::DeleteTra(Acc* acc, uint16 pos)
{
	AccViewSet* avs = AccVws[acc];
	avs->rv->DeleteTra(pos);
	alv->InvalidateAccLI(acc);
}

void MainView::EditTra()
{
	#if DEBUG
	printf("MainView::EditTra\n");
	#endif
	curAvs->rv->EditTra();
	curAvs->pg->Invalidate();
	curAvs->lg->InvalidateData();
	alv->InvalidateAccLI(curAcc);
}

void MainView::EditTra(Acc* acc)
{
	AccViewSet* avs = AccVws[acc];
	avs->rv->EditTra();
	avs->pg->Invalidate();
	avs->lg->InvalidateData();
	alv->InvalidateAccLI(acc);
}

void MainView::FrameResized(float w, float h)
{
	if (w < 795)
	{
		if (!reconB->IsHidden())
		{
			reconB->Hide();
			impB->Hide();
			expB->Hide();
			setB->Hide();
		}
		for (map<Acc*, AccViewSet*>::iterator iter = AccVws.begin();
			iter != AccVws.end(); iter++)
		{
			float fw = Frame().Width();
			float fh = Frame().Height();
			if ((*iter).second->pieZoomed)
				(*iter).second->pg->ResizeTo(fw - 20, fh - 40);
			if ((*iter).second->lineZoomed)
				(*iter).second->lg->ResizeTo(fw - 20, fh - 40);
		}
	}
	else
	{
		if (reconB->IsHidden())
		{
			reconB->Show();
			impB->Show();
			expB->Show();
			setB->Show();
		}
		float pgw = 0;
		float lgw = 0;
		float fh = Frame().Height();
		float halfht = (fh - 550) / 2;
		for (map<Acc*, AccViewSet*>::iterator iter = AccVws.begin();
			iter != AccVws.end(); iter++)
		{
			if (!(*iter).second->pieZoomed)
			{
				pgw = (*iter).second->pg->Frame().Width();
				(*iter).second->pg->ResizeTo(pgw, 193 + halfht);
			}
			else
			{
				curAvs->pg->ResizeTo(545, fh - 45);
			}
			if (!(*iter).second->lineZoomed)
			{
				lgw = (*iter).second->lg->Frame().Width();
				(*iter).second->lg->ResizeTo(pgw, 193 + halfht);
				(*iter).second->lg->MoveTo(563, 230 + halfht);
			}
			else
			{
				curAvs->lg->ResizeTo(545, fh - 45);
			}
		}
	}
}

Tra* MainView::GetNewTra(Acc* acc, bool alert)
{
	#if DEBUG
	printf("MainView::GetNewTra\n");
	#endif
	Pye* pye = 0;
	Cat* cat = 0;
	if (!curAvs->cv->IsTransfer())
	{
		pye = curAvs->cv->GetPye();
		if (!pye)
		{
			string pyetext = curAvs->cv->GetPyeText();
			if (pyetext.length() == 0)
			{
				if (alert)
				{
					BAlert* a = new BAlert("Invalid Transaction",
						"Invalid Transaction.  Please enter a Payee", "OK");
					a->Go();
				}
				return 0;
			}
			pye = new Pye;
			pye->name = pyetext;
			theProxy->AddPye(pye);
		}
		cat = curAvs->cv->GetCat();
		if (!cat)
		{
			string cattext = curAvs->cv->GetCatText();
			if (cattext.length() == 0)
			{
				if (alert)
				{
					BAlert* a = new BAlert("Invalid Transaction",
						"Invalid Transaction.  Please enter a Category", "OK");
					a->Go();
				}
				return 0;
			}
			cat = theProxy->AddCat(cattext);
		}
	}
	return curAvs->cv->GetTra(cat, pye);
}

Acc* MainView::GetAccMapping(string str, Acc* acc)
{
	AccChooseWindow* acw = new AccChooseWindow(theProxy->GetAccL(), "QIF Import",
		"To what account does", "\"" + str + "\" correspond?", acc, theProxy->GetPrefs());
	return acw->GetAcc();
}

bool MainView::GetOpenBalChange(string amt)
{
	BAlert* alert = new BAlert("Change Open Balance?", ("Change Opening Balance on this account to " 
		+ amt + "?").c_str(), "No", "Yes");
	return alert->Go();
}

string MainView::GetStrChoice(list<string>* strL, string str, Acc* acc)
{
	ListChooseWindow* lcw = new ListChooseWindow(strL, "BeMyCheckbook Import",
		"Which BMC account should be", "imported to the " + acc->name + " account", str);
	return lcw->GetStr();
}

void MainView::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		case RegisterViewSelectedMSG:
		{
			Tra* t = curAvs->rv->GetSelectedTra();
			curAvs->cv->UpdateTra(t);
		}
		break;
		case CheckPopNumMSG:
		case CheckPopDateMSG:
		case CheckPopPyeMSG:
		case CheckPopPaymentMSG:
		case CheckPopDepositMSG:
		case CheckPopCatMSG:
		case ClearSplitMSG:
		case CheckPopTransferMSG:
		case ClearTransferMSG:
		case CheckPyeTextModMSG:
		case CheckCatTextModMSG:
		case CheckNumTextModMSG:
		case CheckDatTextModMSG:
			curAvs->cv->MessageReceived(message);
		break;
		case CheckEnterPressMSG:
		{
			Tra* newT = GetNewTra(curAcc);
			if (newT)
			{
				Tra* currentT = curAvs->rv->GetSelectedTra();
				if (currentT)
					theProxy->EditTra(curAcc, currentT, newT);
				else
					theProxy->AddTra(curAcc, newT);
				curAvs->rv->DeselectAll();
				curAvs->cv->UpdateTra(0);
			}
		}
		break;
		case CheckNewPressMSG:
		{
			Tra* newT = GetNewTra(curAcc);
			if (newT)
			{
				BAlert* alert = new BAlert("Save Changes?", 
					"Save Changes to Transaction?", "Don't Save", "Save");
				uint8 bi = alert->Go();
				if (bi == 1)
				{
					Tra* currentT = curAvs->rv->GetSelectedTra();
					if (currentT)
						theProxy->EditTra(curAcc, currentT, newT);
					else
						theProxy->AddTra(curAcc, newT);
				}
			}
			curAvs->rv->DeselectAll();
			curAvs->cv->UpdateTra(0);
		}
		break;
		case CheckSplitPressMSG:
		{
			pair<uint32, uint8> p = curAvs->cv->GetAmount();
			if (p.second != 3)
			{
				SplitWindow* sw = new SplitWindow(BRect(200, 200, 500, 330),
					theProxy, this, theProxy->GetPrefs(), p.first, p.second, curAvs->cv->GetSplCat());
				sw->Show();
			}
		}
		break;
		case CheckDeletePressMSG:
		{
			Tra* currentT = curAvs->rv->GetSelectedTra();
			theProxy->DeleteTra(curAcc, currentT);
		}
		break;
		case SplitMSG:
		{
			Cat* sc;
			bool pam;
			int32 amt;
			message->FindPointer("cat", (void**)&sc);
			message->FindBool("pam", &pam);
			message->FindInt32("amt", &amt);
			if (sc->spl)
				curAvs->cv->SetSplCat((SplCat*)sc, pam);
			else
				curAvs->cv->SetCat(sc, pam, amt);
		}
		break;
		case AccSelectedMSG:
		{
			Acc* acc = alv->GetSelectedAcc();
			if (!acc)
				break;
			AccViewSet* avs = AccVws[acc];
			if (curAvs)
			{
				curAvs->cv->Hide();
				curAvs->rv->Hide();
				if (!curAvs->pg->IsHidden())
					curAvs->pg->Hide();
				curAvs->lg->Hide();
				if (!curAvs->rcv->IsHidden())
					curAvs->rcv->Hide();
			}
			if (avs->cv->IsHidden())
				avs->cv->Show();
			avs->cv->MakeEnterDefault();
			if (avs->rv->IsHidden())
				avs->rv->Show();
			if (!avs->recon && avs->pg->IsHidden())
				avs->pg->Show();
			if (avs->lg->IsHidden())
				avs->lg->Show();
			if (avs->recon && avs->rcv->IsHidden())
				avs->rcv->Show();
			curAvs = avs;
			curAcc = acc;
			reconB->SetEnabled(!curAvs->recon);
			BMessenger msngr(Window());
			BMessage msg;
			if (curAvs->recon)
				msg.what = CannotRecMSG;
			else
				msg.what = CanRecMSG;
			msngr.SendMessage(&msg);
		}
		break;
		case ReconcileMSG:
		{
			curAvs->rcv->Show();
			curAvs->pg->Hide();
			reconB->SetEnabled(false);
			BMessenger msngr(Window());
			BMessage msg(CannotRecMSG);
			msngr.SendMessage(&msg);
		 	int32 balance = theProxy->GetRecBal(curAcc);
			if (!curAvs->recon)
			{
				curAvs->rcv->SetRecBalAtStart(balance);
				curAvs->rcv->SetRecBal(balance);
				curAvs->recon = true;
			}
		}
		break;
		case RegisterViewInvokedMSG:
		{
			if (!curAvs->recon)
				break;
			Tra* t = curAvs->rv->GetSelectedTra();
			if (t->rec == 0)
			{
				t->rec = 1;
				theProxy->ReconTra(curAcc, t);
				curAvs->rv->EditTra();
			}
			else if (t->rec == 1)
			{
				t->rec = 0;
				theProxy->ReconTra(curAcc, t);
				curAvs->rv->EditTra();
			}
			else if (t->rec == 2)
			{
				BAlert* alert = new BAlert("Reconciliation Error",
					"Change status of Confirmed Transaction?",
					"Yes", "No");
				if (alert->Go() == 0)
				{
					t->rec = 0;
					theProxy->ReconTra(curAcc, t);
					curAvs->rv->EditTra();
				}
			}
		}
		break;
		case ReconcileCancelMSG:
		{
			curAvs->rcv->Hide();
			curAvs->pg->Show();
			reconB->SetEnabled(true);
			BMessenger msngr(Window());
			BMessage msg(CanRecMSG);
			msngr.SendMessage(&msg);
		}
		break;
		case ReconcileFinishMSG:
		{
			theProxy->ReconFinish(curAcc);
			curAvs->rv->InvalidateLV();
			curAvs->rcv->Hide();
			curAvs->pg->Show();
			reconB->SetEnabled(true);
			curAvs->recon = false;
			BMessenger msngr(Window());
			BMessage msg(CanRecMSG);
			msngr.SendMessage(&msg);
		}
		break;
		case ReconcileModifiedMSG:
			curAvs->rcv->RedoBals();
		break;
		case ImportMSG:
		{
			entry_ref er;
			if (message->FindRef("refs", &er) != B_OK)
				break;
			BEntry entry(&er);
			if (!entry.Exists())
			{
				BAlert* alert = new BAlert("Open Warning",
					"File Does Not Exist", "OK");
				alert->Go();
				break;
			}
			BNode n(&entry);
			BNodeInfo ni(&n);
			char* type = new char[B_MIME_TYPE_LENGTH];
			ni.GetType(type);
			if (!strcmp(type, "account/x-TMS-BMC"))
				theProxy->BMCImport(curAcc, &entry);
			else
				try
				{
					Window()->Lock();
					theProxy->Import(curAcc, &entry);
					Window()->Unlock();
				}
				catch (invalid_argument)
				{
					BAlert* alert = new BAlert("Import Warning",
						"File Could not be Imported", "OK");
					alert->Go();
				}
			delete[] type;
		}
		break;
		case ExportMSG:
		{
			BDirectory directory;
			BFile file;
			BEntry entry;
			entry_ref dir;
			const char* name;
			if (message->FindRef("directory", &dir) != B_OK)
				break;
			if (message->FindString("name", &name) != B_OK)
				break;
			directory.SetTo(&dir);
			if (directory.InitCheck() == B_NO_ERROR)
			{
				directory.CreateFile(name, &file);
				if (file.InitCheck() == B_NO_ERROR)
				{
					directory.FindEntry(name, &entry);
					theProxy->Export(curAcc, &entry);
				}
			}
		}
		break;
		case AccSetMSG:
		case SettingsMSG:
		{
			AccSettingsWindow* asw = new AccSettingsWindow(curAcc, this, theProxy->GetPrefs());
			asw->Show();
		}
		break;
		case ManPyeMSG:
		{
			if (!pmw)
				pmw = new PyeManagerWindow(this, theProxy);
			pmw->Show();
		}
		break;
		case ManCatMSG:
		{
			if (!cmw)
				cmw = new CatManagerWindow(this, theProxy);
			cmw->Show();
		}
		break;
		case PyeManagerWindowExitMSG:
			pmw = 0;
		break;
		case CatManagerWindowExitMSG:
			cmw = 0;
		break;
		case PieZoomMSG:
		{
			if (curAvs->pieZoomed)
			{
				UnzoomPie();
				curAvs->rv->Show();
				curAvs->cv->Show();
			}
			else
			{
				if (curAvs->lineZoomed)
				{
					UnzoomLine();
				}
				else
				{
					curAvs->rv->Hide();
					curAvs->cv->Hide();
				}
				ZoomPie();
			}
		}
		break;
		case LineZoomMSG:
		{
			if (curAvs->lineZoomed)
			{
				UnzoomLine();
				curAvs->rv->Show();
				curAvs->cv->Show();
			}
			else
			{
				if (curAvs->pieZoomed)
				{
					UnzoomPie();
				}
				else
				{
					curAvs->rv->Hide();
					curAvs->cv->Hide();
				}
				ZoomLine();
			}
		}
		break;
		case 1347638341:
			rgb_color* color;
			ssize_t st;
			message->FindData("RGBColor", 'RGBC', (const void**)&color, &st);
			SetViewColor(*color);
			Invalidate();
		break;
		default:
			BView::MessageReceived(message);
	}
}

void MainView::ZoomPie()
{
	BRect frame = Frame();
	float fht = frame.Height();

	curAvs->pg->MoveTo(10, 30);
	curAvs->pg->ResizeTo(545, fht - 45);
	curAvs->pieZoomed = true;
}

void MainView::UnzoomPie()
{
	BRect frame = Frame();
	float fht = frame.Height();
	float fwdth = frame.Width();
	float halfht = (fht - 550) / 2;

	curAvs->pg->MoveTo(563, 30);
	curAvs->pg->ResizeTo(-568 + fwdth, 193 + halfht);
	curAvs->pieZoomed = false;
}

void MainView::ZoomLine()
{
	BRect frame = Frame();
	float fht = frame.Height();

	curAvs->lg->MoveTo(10, 30);
	curAvs->lg->ResizeTo(545, fht - 45);
	curAvs->lineZoomed = true;
}

void MainView::UnzoomLine()
{
	BRect frame = Frame();
	float fht = frame.Height();
	float fwdth = frame.Width();
	float halfht = (fht - 550) / 2;

	curAvs->lg->MoveTo(563, 230 + halfht);
	curAvs->lg->ResizeTo(-568 + fwdth, 193 + halfht);
	curAvs->lineZoomed = false;
}

void MainView::NewAcc(Acc* acc)
{
	for (avm::iterator iter = AccVws.begin();
		iter != AccVws.end(); iter++)
		(*iter).second->cv->RedoPyePopUp();
	AccViewSet* avs = new AccViewSet;
	avs->pieZoomed = false;
	avs->lineZoomed = false;
	BRect frame = Frame();
	float fht = frame.Height();
	float fwdth = frame.Width();
	avs->cv = new CheckView(BRect(10, fht - 170, 555, fht - 15), theProxy->GetCatS(), 
		theProxy->GetPyeS(), theProxy->GetNxtChkL(acc), theProxy->GetRecDatL(),
		theProxy->GetRecPyeL(),	theProxy->GetRecPamL(), theProxy->GetRecDepL(), 
		theProxy->GetAccL(), acc, theProxy->GetPrefs());
	AddChild(avs->cv);
	avs->cv->Hide();
	avs->rv = new RegisterView(BRect(10, 30, 555, fht - 175), theProxy->GetPrefs());
	AddChild(avs->rv);
	avs->rv->Hide();
	float halfht = (fht - 550) / 2;
	avs->pg = new PieGraph(BRect(563, 30, fwdth - 5, 223 + halfht), theProxy->GetCatS());
	AddChild(avs->pg);
	avs->pg->Hide();
	avs->lg = new LineGraph(BRect(563, 230 + halfht, fwdth - 5, fht - 127), theProxy->GetAccL(), theProxy->GetPrefs());
	AddChild(avs->lg);
	avs->lg->Hide();
	avs->rcv = new ReconcileView(BRect(563, 30, fwdth - 5, 223 + halfht), theProxy->
		GetRecBal(acc), theProxy->GetPrefs());
	AddChild(avs->rcv);
	avs->rcv->Hide();
	AccVws.insert(pair<Acc*, AccViewSet*>(acc, avs));
	alv->AddAcc(acc);
	avs->recon = false;
	impB->SetEnabled(true);
	expB->SetEnabled(true);
	setB->SetEnabled(true);
	BMessenger msngr(Window());
	BMessage msg(CanImpMSG);
	msngr.SendMessage(&msg);
	msg.what = CanSetMSG;
	msngr.SendMessage(&msg);
}

void MainView::NumLChanged()
{
	curAvs->cv->RedoNumPopUp();
}

void MainView::PrefsUpdated()
{
	if (curAcc)
	{
		curAvs->rv->InvalidateLV();
		alv->InvalidateLV();
	}
}

void MainView::PyeLChanged()
{
	if (curAcc)
	{
		Window()->Lock();
		curAvs->cv->RedoPyePopUp();
		Window()->Unlock();
	}
	if (pmw)
		pmw->RedoList();
}

void MainView::RegItemsChanged()
{
	Window()->Lock();
	curAvs->rv->InvalidateLV();
	Window()->Unlock();
}

void MainView::SetRecBal(int32 recBal)
{
	curAvs->rcv->SetRecBal(recBal);
}
