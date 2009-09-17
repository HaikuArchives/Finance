#ifndef CheckViewH
#define CheckViewH

#include <list>
#include <set>
#include <string>
#include <PopUpMenu.h>
#include <View.h>
#include "NameSorted.h"
#include "Prefs.h"
#include "quadruplet.h"

class BMenuField;
class BStringView;
class BTextControl;

class Acc;
class Pye;
class Tra;
class Cat;
class SplCat;

#define CheckPopNumMSG			401
#define CheckPopDateMSG			402
#define CheckPopPyeMSG			403
#define CheckPopPaymentMSG		404
#define CheckPopDepositMSG		405
#define CheckPopCatMSG			406
#define CheckEnterPressMSG		410
#define CheckNewPressMSG		411
#define CheckSplitPressMSG		412
#define ClearSplitMSG			413
#define CheckDeletePressMSG		414
#define CheckPopTransferMSG		415
#define ClearTransferMSG		416
#define CheckPyeTextModMSG		417
#define CheckCatTextModMSG		418
#define CheckNumTextModMSG		419
#define CheckDatTextModMSG		420

class CheckView : public BView
{
	Prefs* prefs;
	typedef set<Cat*, NameSorted> cas;
	typedef list<Cat*> cal;
	typedef set<Pye*, NameSorted> pas;
	typedef list<Pye*> pal;
	typedef list<uint16> u16l;
	typedef list<uint32> u32l;
	typedef list<int32> i32l;
	typedef list<time_t> til;
	typedef list<Acc*> acl;
	bool tsf;
	Acc* acc;
	Acc* tsfAcc;
	BButton* deleteB;
	BButton* enterB;
	SplCat* theSplCat;
	set<Cat*, NameSorted>* catS;
	list<uint16>* nxtChkL;
	set<Pye*, NameSorted>* pyeS;
	list<Acc*>* accL;
	list<time_t>* recDatL;
	list<Pye*>* recPyeL;
	list<uint32>* recPamL;
	list<uint32>* recDepL;
	BTextControl* numText;
	BTextControl* datText;
	BTextControl* pyeText;
	BTextControl* pamText;
	BTextControl* depText;
	BTextControl* catText;
	BTextControl* memText;
	BPopUpMenu* numPop;
	BPopUpMenu* datPop;
	BPopUpMenu* pyePop;
	BPopUpMenu* pamPop;
	BPopUpMenu* depPop;
	BPopUpMenu* catPop;
	BMenuField* catMF;
	BStringView* pyeSV;
	BStringView* catSV;
	string pyePrevStr;
//	string pyePrevGuessStr;
	string catPrevStr;
//	string catPrevGuessStr;
	
	Tra* curTra;
	void AddMenuItemToPopUp(uint32 command, BPopUpMenu* pu,
		string textstr, bool enabled = true);
	void EmptyMenu(BMenu* menu);
	BMenuItem* MakeCatMenu(Cat* cat, bool enabled = true);
	quadruplet<BTextControl*, BPopUpMenu*, BMenuField*, BStringView*> MakeField(uint16 width,
		string name, uint16* xpos, uint16* ypos);
public:
	CheckView(BRect frame, cas* catS, pas* pyeset, u16l* nextchecklist, til* recDatL, 
		pal* recPyeL, u32l* recPamL, u32l* recDepL, acl* acclist, Acc* acc, Prefs* prefs);
	~CheckView();
	void Draw(BRect updateRect);
	pair<uint32, uint8> GetAmount();
	Cat* GetCat();
	string GetCatText();
	Pye* GetPye();
	string GetPyeText();
	SplCat* GetSplCat();
	Tra* GetTra(Cat* cat, Pye* pye, bool alert = true);
	bool IsTransfer();
	void MakeEnterDefault();
	void MessageReceived(BMessage* message);
	void RedoCatPopUp();
	void RedoDatPopUp();
	void RedoDepPopUp();
	void RedoNumPopUp();
	void RedoPyePopUp();
	void RedoPamPopUp();
	void SetCat(Cat* cat, bool pam, int32 amt);
	void SetSplCat(SplCat* cat, bool pam);
	void SetCatL(cal* catlist);
	void SetPyeL(pal* pyelist);
	void UpdatePopUps();
	void UpdateTra(Tra* tra);
};

#endif
