#include <stdio.h>

#include <Alert.h>
#include <Application.h>
#include <File.h>
#include <FindDirectory.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <NodeInfo.h>
#include <Path.h>

#include "MainWindow.h"
#include "PrefsWindow.h"
#include "Proxy.h"
#include "StringInputWindow.h"
#include "TraMan.h"
#include "Version.h"

MainWindow::MainWindow(BRect frame) : BWindow(frame,
	"Finance", B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS | B_OUTLINE_RESIZE)
{
	openfp = 0;
	savefp = 0;
	openFile = false;
	fPrefs = 0;
	quit = false;
	newF = false;

	BRect menuFrame = frame;
	menuFrame.OffsetTo(0, 0);
	menuFrame.bottom = 0;
	BMenuBar* mb = new BMenuBar(menuFrame, "MainMenuBar");
	AddChild(mb);
	BMenu* m = new BMenu("File");
	BMenuItem* mi = new BMenuItem("New", new BMessage(FileNewMSG));
	m->AddItem(mi);
	mi = new BMenuItem("Open...", new BMessage(FileOpenMSG), 'O');
	m->AddItem(mi);
	FileSaveMI = new BMenuItem("Save", new BMessage(FileSaveMSG), 'S');
	FileSaveMI->SetEnabled(false);
	m->AddItem(FileSaveMI);
	FileSaveAsMI = new BMenuItem("Save As...", new BMessage(FileSaveAsMSG));
	FileSaveAsMI->SetEnabled(false);
	m->AddItem(FileSaveAsMI);
	m->AddSeparatorItem();
	mi = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q');
	m->AddItem(mi); 
	mb->AddItem(m);
	m = new BMenu("Account");
	mi = new BMenuItem("New...", new BMessage(AccNewMSG), 'N');
	m->AddItem(mi);
	ImpMI = new BMenuItem("Import...", new BMessage(AccImpMSG));
	ImpMI->SetEnabled(false);
	m->AddItem(ImpMI);
	ExpMI = new BMenuItem("Export...", new BMessage(AccExpMSG));
	ExpMI->SetEnabled(false);
	m->AddItem(ExpMI);
	RecMI = new BMenuItem("Reconcile", new BMessage(AccRecMSG), 'R');
	RecMI->SetEnabled(false);
	m->AddItem(RecMI);
	SetMI = new BMenuItem("Settings...", new BMessage(AccSetMSG));
	SetMI->SetEnabled(false);
	m->AddItem(SetMI);
	mb->AddItem(m);
	m = new BMenu("Manage");
	mi = new BMenuItem("Payees...", new BMessage(ManPyeMSG), 'P');
	m->AddItem(mi);
	mi = new BMenuItem("Categories...", new BMessage(ManCatMSG), 'T');
	m->AddItem(mi);
	m->AddSeparatorItem();
	mi = new BMenuItem("Preferences...", new BMessage(ManPrefsMSG));
	m->AddItem(mi);
	mb->AddItem(m);
	
	theProxy = new Proxy;
	tm = new TraMan;
	theProxy->SetTraMan(tm);
	frame.OffsetTo(0, 0);
	mv = new MainView(frame, theProxy);
	theProxy->SetMainView(mv);
	AddChild(mv);
}

MainWindow::~MainWindow()
{
	if (fPrefs)
	{
		if (openFile)
		{
			fPrefs->Seek(0, 0);
			fPrefs->Write(&openFile, sizeof(bool));
			BEntry entry = file_entry;
			BPath path(&entry);
			string strpath = path.Path();
			size_t st = strpath.size();
			fPrefs->Write(&st, sizeof(size_t));
			fPrefs->Write(strpath.c_str(), st);
		}
		else
		{
			fPrefs->Seek(0, 0);
			fPrefs->Write(&openFile, sizeof(bool));
		}
		delete fPrefs;
	}
	delete theProxy;
	delete tm;
	if (openfp)
		delete openfp;
	if (savefp)
		delete savefp;
}

void MainWindow::FrameResized(float w, float h)
{
	float newW = w;
	float newH = h;
	
	if (newW > 679)
	{
		if (newW < 795)
			newW = 795;
	}
	else
	{
		newW = 563;
	}
	
	if (newH < 550)
		newH = 550;
	
	if (newW != w || newH != h)
		ResizeTo(newW, newH);
}

void MainWindow::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		case RegisterViewSelectedMSG:
		case CheckPopNumMSG:
		case CheckPopDateMSG:
		case CheckPopPyeMSG:
		case CheckPopPaymentMSG:
		case CheckPopDepositMSG:
		case CheckPopCatMSG:
		case CheckEnterPressMSG:
		case CheckDeletePressMSG:
		case CheckNewPressMSG:
		case CheckSplitPressMSG:
		case CheckPopTransferMSG:
		case CheckPyeTextModMSG:
		case CheckCatTextModMSG:
		case CheckNumTextModMSG:
		case CheckDatTextModMSG:
		case ClearSplitMSG:
		case AccSelectedMSG:
		case ReconcileMSG:
		case RegisterViewInvokedMSG:
		case ReconcileCancelMSG:
		case ReconcileFinishMSG:
		case ReconcileModifiedMSG:
		case ClearTransferMSG:
		case AccReturnMSG:
		case AccSetMSG:
		case SettingsMSG:
		case ManPyeMSG:
		case ManCatMSG:
		case PyeManagerWindowExitMSG:
		case CatManagerWindowExitMSG:
		case PieZoomMSG:
		case LineZoomMSG:
			mv->MessageReceived(message);
		break;
		case FileNewMSG:
		{
			if (theProxy->DataExists())
			{
				BAlert* alert = new BAlert("New File", "Save before New?",
					"Cancel", "Don't save", "Save", B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);				uint8 res = alert->Go();
				if (res == 2)
				{
					BMessage msg;
					if (openFile)
					{
						msg.what = FileSaveMSG;
					}
					else
					{
						msg.what = FileSaveAsMSG;
						newF = true;
					}
					MessageReceived(&msg);
					if (openFile)
					{
						openFile = false;
						theProxy->ClearData();
						FileSaveMI->SetEnabled(false);
						FileSaveAsMI->SetEnabled(false);
						ImpMI->SetEnabled(false);
						RecMI->SetEnabled(false);
						SetMI->SetEnabled(false);
					}
				}
				else if (res == 0)
				{
					break;
				}
				else if (res == 1)
				{
					openFile = false;
					theProxy->ClearData();
					FileSaveMI->SetEnabled(false);
					FileSaveAsMI->SetEnabled(false);
					ImpMI->SetEnabled(false);
					RecMI->SetEnabled(false);
					SetMI->SetEnabled(false);
				}
			}
		}
		break;
		case FileSaveAsMSG:
		{
			#ifdef FREE
				BAlert* alert = new BAlert("Free Version", "Save is Disabled for the Free Version.  See www.geocities.com/bunru_software/index.html for purchase details.",
					"OK");
				alert->Go();
				break;
			#endif
			BMessage msg(FileSaveAsNameMSG);
			if (!savefp)
			{
				BMessenger msngr(this);
				savefp = new BFilePanel(B_SAVE_PANEL, &msngr, 0, 0, false,
					&msg);
			}
			else
				savefp->SetMessage(&msg);
			savefp->SetSaveText(".fin");
			savefp->Show();
		}
		break;
		case AccNewMSG:
		{
			StringInputWindow* siw = new StringInputWindow("New Account Name",
				new BMessage(AccNewNameMSG), this);
			siw->Show();
		}
		break;
		case AccNewNameMSG:
		{
			const char* str;
			message->FindString("string", &str);
			theProxy->NewAcc(str);
			FileSaveAsMI->SetEnabled(true);
			RecMI->SetEnabled(true);
			ImpMI->SetEnabled(true);
			SetMI->SetEnabled(true);
		}
		break;
		case ImportMSG:
		case AccImpMSG:
		{
			BMessage msg(AccImpNameMSG);
			if (!openfp)
			{
				BMessenger msngr(this);
				openfp = new BFilePanel(B_OPEN_PANEL, &msngr, 0, 0, false,
					&msg);
			}
			else
				openfp->SetMessage(&msg);
			openfp->Show();
		}
		break;
		case ExportMSG:
		case AccExpMSG:
		{
			BMessage msg(AccExpNameMSG);
			if (!savefp)
			{
				BMessenger msngr(this);
				savefp = new BFilePanel(B_SAVE_PANEL, &msngr, 0, 0, false,
					&msg);
			}
			else
				savefp->SetMessage(&msg);
			savefp->SetSaveText((mv->CurAccName() + ".qif").c_str());
			savefp->Show();
		}
		break;
		case AccImpNameMSG:
		{
			message->what = ImportMSG;
			mv->MessageReceived(message);
		}
		break;
		case AccExpNameMSG:
		{
			message->what = ExportMSG;
			mv->MessageReceived(message);
		}
		break;
		case FileSaveAsNameMSG:
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
					BNodeInfo* node = new BNodeInfo(&file);
					node->SetType("application/x-vnd.Finance.File");
					node->SetPreferredApp("application/x-vnd.Finance");
					delete node;

					directory.FindEntry(name, &entry);
					theProxy->Save(&entry);
					openFile = true;
					FileSaveMI->SetEnabled(true);
					file_entry = entry;
				}
			}
			if (quit)
				be_app->PostMessage(B_QUIT_REQUESTED);
			if (newF)
			{
				openFile = false;
				theProxy->ClearData();
				FileSaveMI->SetEnabled(false);
				FileSaveAsMI->SetEnabled(false);
				ImpMI->SetEnabled(false);
				RecMI->SetEnabled(false);
				SetMI->SetEnabled(false);
				newF = false;
			}
		}
		break;
		case FileOpenMSG:
		{
			if (theProxy->DataExists())
			{
				BAlert* alert = new BAlert("Open File", "Save Existing Data before Opening File?",
					"Cancel", "Don't save", "Save", B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
				uint8 res = alert->Go();
				if (res == 2)
				{
					BMessage msg;
					if (openFile)
						msg.what = FileSaveMSG;
					else
						msg.what = FileSaveAsMSG;
					MessageReceived(&msg);
				}
				else if (res == 0)
				{
					break;
				}
			}
			BMessage msg(FileOpenNameMSG);
			if (!openfp)
			{
				BMessenger msngr(this);
				openfp = new BFilePanel(B_OPEN_PANEL, &msngr, 0, 0, false,
					&msg);
			}
			else
				openfp->SetMessage(&msg);
			openfp->Show();
		}	
		break;
		case FileOpenNameMSG:
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
			Lock();
			if (!theProxy->Open(&entry))
			{
				BAlert* alert = new BAlert("Open Warning",
					"File Has Incorrect Format", "OK");
				alert->Go();
			}
			else
			{
				openFile = true;
				file_entry = entry;
				FileSaveMI->SetEnabled(true);
				FileSaveAsMI->SetEnabled(true);
			}
			Unlock();
		}
		break;
		case FileSaveMSG:
		{
			#ifdef FREE
				BAlert* alert = new BAlert("Free Version", "Save is Disabled for the Free Version.  See www.geocities.com/bunru_software/index.html for purchase details.",
					"OK");
				alert->Go();
				break;
			#endif
			theProxy->Save(&file_entry);
		}	
		break;
		case CanRecMSG:
			RecMI->SetEnabled(true);
		break;
		case CannotRecMSG:
			RecMI->SetEnabled(false);
		break;
		case CanImpMSG:
			ImpMI->SetEnabled(true);
			ExpMI->SetEnabled(true);
		break;
		case CanSetMSG:
			SetMI->SetEnabled(true);
		break;
		case AccSetReturnMSG:
		{
			Acc* acc;
			message->FindPointer("acc", (void**)&acc);
			theProxy->AccUpdated(acc);
		}
		break;
		case ManPrefsMSG:
		{
			PrefsWindow* pw = new PrefsWindow(this, theProxy->GetPrefs());
			pw->Show();
		}
		break;
		case PrefsReturnMSG:
		{
			Prefs* newPrefs = 0;
			message->FindPointer("prefs", (void**)&newPrefs);
			int8 decPlaceChange = newPrefs->curDecPlace - theProxy->GetPrefs()->curDecPlace;
			theProxy->GetPrefs()->Set(newPrefs);
			theProxy->PrefsUpdated(decPlaceChange);
			delete newPrefs;
		}
		break;
		default:
			BWindow::MessageReceived(message);
	}
}

void MainWindow::OpenPrev()
{
	BPath path;
	BDirectory dir;
	BEntry entry;

	find_directory(B_USER_SETTINGS_DIRECTORY, &path, true);
	dir.SetTo(path.Path());
	if (dir.FindEntry("BeFinancial_data", &entry) == B_NO_ERROR)
	{
		fPrefs = new BFile(&entry, O_RDWR);
		if (fPrefs->InitCheck() == B_NO_ERROR)
		{
			bool openFile;
			fPrefs->Read(&openFile, sizeof(bool));
			if (openFile)
			{
				size_t st;
				fPrefs->Read(&st, sizeof(size_t));
				if (st > 0 && st < 1024)
				{
					char* str = new char[st + 1];
					fPrefs->Read(str, st);
					str[st] = '\0';
					BMessage msg(FileOpenNameMSG);
					entry_ref ref;
					get_ref_for_path(str, &ref);
					msg.AddRef("refs", &ref);
					MessageReceived(&msg);
					delete str;
				}
			}
		}
	}
	else
	{
		fPrefs = new BFile();
		if (dir.CreateFile("BeFinancial_data", fPrefs) != B_NO_ERROR)
		{
			delete fPrefs;
			fPrefs = 0;
		}
	}
}

bool MainWindow::QuitRequested()
{
	if (quit)
		return true;
	if (theProxy->DataExists())
	{
		BAlert* alert = new BAlert("Quit Finance", "Save before Quitting?",
			"Cancel", "Don't save", "Save", B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
		uint8 res = alert->Go();
		if (res == 2)
		{
			BMessage msg;
			if (openFile)
			{
				msg = FileSaveMSG;
				MessageReceived(&msg);
				be_app->PostMessage(B_QUIT_REQUESTED);
				return true;
			}
			else
			{
				msg = FileSaveAsMSG;
				MessageReceived(&msg);
				quit = true;
				return false;
			}
		}
		else if (res == 0)
			return false;
	}
	
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}
