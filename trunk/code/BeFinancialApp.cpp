#include <stdio.h>
#include <Alert.h>
#include <Bitmap.h>
#include <Mime.h>

#include "BeFinancialApp.h"
#include "Icons.h"
#include "MainWindow.h"

BeFinancialApp::BeFinancialApp() : BApplication("application/x-vnd.BeFinancial")
{
	mw = new MainWindow(BRect(100, 100, 895, 650));
	mw->Show();
	
	BBitmap	large_icon(BRect(0, 0, B_LARGE_ICON - 1, B_LARGE_ICON - 1), B_COLOR_8_BIT);
	BBitmap	mini_icon(BRect(0, 0, B_MINI_ICON - 1, B_MINI_ICON - 1), B_COLOR_8_BIT);

	BMimeType mime("BeFinancial File");
	mime.SetType("application/x-vnd.BeFinancial.File");
	bool install = !mime.IsInstalled();
	if (!install)
	{
		BBitmap* junkptr = 0;
		if (mime.GetIcon(junkptr, B_LARGE_ICON))
			install = true;
	}
	if (install)
	{
		mime.Install();
		large_icon.SetBits(kLargeBeFinanceFileIcon, large_icon.BitsLength(), 0, B_COLOR_8_BIT);
		mini_icon.SetBits(kSmallBeFinanceFileIcon, mini_icon.BitsLength(), 0, B_COLOR_8_BIT);
		mime.SetShortDescription("BeFinancial File");
		mime.SetLongDescription("Accounts & Transactions from BeFinancial");
		mime.SetIcon(&large_icon, B_LARGE_ICON);
		mime.SetIcon(&mini_icon, B_MINI_ICON);
		mime.SetPreferredApp("application/x-vnd.BeFinancial");
		BMessage msg;
		msg.AddString("extensions", "fin");
		mime.SetFileExtensions(&msg);
	}

	refReceived = false;	
}

BeFinancialApp::~BeFinancialApp()
{
}

void BeFinancialApp::ReadyToRun()
{
	if (!refReceived)
		mw->OpenPrev();
}

void BeFinancialApp::RefsReceived(BMessage* message)
{
	message->what = FileOpenNameMSG;
	mw->MessageReceived(message);
	refReceived = true;
}

int main()
{
	BeFinancialApp app;
	app.Run();
}
