#include <list>
#include <string>
#include <Window.h>

#define ListChooseOKMSG			1301
#define ListChooseCancelMSG		1302

using namespace std;

class ListChooseWindow : public BWindow
{
	sem_id clickWait;
	string str;
	BListView* lv;
public:
	ListChooseWindow(list<string>* strL, string title, string text1, string text2,
		string str);
	string GetStr();
	void MessageReceived(BMessage* msg);
};
