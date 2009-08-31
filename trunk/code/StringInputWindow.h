#include <string>
#include <Window.h>

#define OKButtonMSG		601

class StringInputWindow : public BWindow
{
	bool modal;
	string str;
	sem_id clickWait;
	BMessage* returnMessage;
	BHandler* returnHandler;
protected:
	BView* backdrop;
	BTextControl* stringTC;
public:
	StringInputWindow(const char* title, BMessage* returnMessage,
		BHandler* returnHandler, string text = "", bool modal = false);
	string GetString();
	void MessageReceived(BMessage* message);
};