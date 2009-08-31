#include <Application.h>

class MainWindow;

class BeFinancialApp : public BApplication
{
	MainWindow* mw;
	bool refReceived;
public:
	BeFinancialApp();
	~BeFinancialApp();
	void ReadyToRun();
	void RefsReceived(BMessage* message);
};
