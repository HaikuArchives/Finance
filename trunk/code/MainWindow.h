#include <string>
#include <FilePanel.h>
#include <Window.h>

#include "MainView.h"

#define RegisterViewSelectedMSG		301
#define RegisterViewInvokedMSG		302
#define CheckPopNumMSG				401
#define CheckPopDateMSG				402
#define CheckPopPyeMSG				403
#define CheckPopPaymentMSG			404
#define CheckPopDepositMSG			405
#define CheckPopCatMSG				406
#define CheckEnterPressMSG			410
#define CheckNewPressMSG			411
#define CheckSplitPressMSG			412
#define ClearSplitMSG				413
#define CheckDeletePressMSG			414
#define CheckPopTransferMSG			415
#define ClearTransferMSG			416
#define CheckPyeTextModMSG			417
#define CheckCatTextModMSG			418
#define CheckNumTextModMSG			419
#define CheckDatTextModMSG			420
#define FileNewMSG					501
#define FileOpenMSG					502
#define FileSaveMSG					503
#define FileSaveAsMSG				504
#define AccNewMSG					505
#define FileSaveAsNameMSG			506
#define AccNewNameMSG				507
#define FileOpenNameMSG				508
#define AccImpMSG					509
#define AccImpNameMSG				510
#define AccRecMSG					511
#define ManPyeMSG					512
#define ManCatMSG					513
#define AccExpMSG					514
#define AccExpNameMSG				515
#define ManPrefsMSG					516
#define AccSelectedMSG				601
#define ReconcileCancelMSG			801
#define ReconcileFinishMSG			802
#define ReconcileModifiedMSG		803
#define AccReturnMSG				903
#define AccSetReturnMSG				1003
#define PyeManagerWindowExitMSG		1101
#define CatManagerWindowExitMSG		1201

class Proxy;
class TraMan;

class MainWindow : public BWindow
{
	bool newF;
	bool quit;
	BMenuItem* FileSaveAsMI;
	BMenuItem* FileSaveMI;
	BMenuItem* RecMI;
	BMenuItem* ImpMI;
	BMenuItem* ExpMI;
	BMenuItem* SetMI;
	BFilePanel *openfp, *savefp;
	MainView* mv;
	Proxy* theProxy;
	TraMan* tm;
	BFile *fPrefs;
	void FrameResized(float w, float h);
	bool QuitRequested();
public:
	BEntry file_entry;
	bool openFile;
	MainWindow(BRect frame);
	~MainWindow();
	void OpenPrev();
	void MessageReceived(BMessage* message);
};
