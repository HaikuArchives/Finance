#ifndef MainViewH
#define MainViewH

#include <list>
#include <map>
#include <View.h>

class AccListView;
class Proxy;
class Acc;
class CheckView;
class LineGraph;
class PieGraph;
class ReconcileView;
class RegisterView;
class Tra;
class PyeManagerWindow;
class CatManagerWindow;

#define ReconcileMSG	701
#define ImportMSG		702
#define CanRecMSG		703
#define CannotRecMSG	704
#define CanImpMSG		705
#define CanSetMSG		706
#define AccSetMSG		707
#define SettingsMSG		708
#define ExportMSG		709
#define PieZoomMSG		710
#define LineZoomMSG		711
#define ManPyeMSG		512
#define ManCatMSG		513

class BButton;

struct AccViewSet
{
	CheckView* cv;
	RegisterView* rv;
	PieGraph* pg;
	LineGraph* lg;
	ReconcileView* rcv;
	bool recon;
	bool pieZoomed;
	bool lineZoomed;
};

class MainView : public BView
{
	typedef map<Acc*, AccViewSet*> avm;
	avm AccVws;
	AccListView* alv;
	BButton* reconB;
	BButton* impB;
	BButton* expB;
	BButton* setB;
	Acc* curAcc;
	AccViewSet* curAvs;
	Proxy* theProxy;
	PyeManagerWindow* pmw;
	CatManagerWindow* cmw;
	Tra* GetNewTra(Acc* acc, bool alert = true);
	void ZoomPie();
	void UnzoomPie();
	void ZoomLine();
	void UnzoomLine();
	void FrameResized(float w, float h);
public:
	MainView(BRect frame, Proxy* theProxy);
	~MainView();
	void AccUpdated(Acc* acc);
	void AddTra(Tra* tra, uint16 pos);
	void AddTra(Acc* acc, Tra* transaction, uint16 pos);
	void CatLChanged();
	void ClearViews();
	string CurAccName();
	void DeleteTra(uint16 pos);
	void DeleteTra(Acc* acc, uint16 pos);
	void EditTra();
	void EditTra(Acc* acc);
	Acc* GetAccMapping(string str, Acc* acc);
	string GetStrChoice(list<string>* strL, string str, Acc* acc);
	bool GetOpenBalChange(string amt);
	void MessageReceived(BMessage* message);
	void NewAcc(Acc* acc);
	void NumLChanged();
	void PrefsUpdated();
	void PyeLChanged();
	void RegItemsChanged();
	void SetRecBal(int32 recBal);
};

#endif
