#pragma once
#include "clsClient.h"
#include "clsUtility.h"
#include "clsScreen.h"
#include <iomanip>

class clsClientListScreen : protected clsScreen
{
private:

	static void _PrintClientsRecordLine(clsClient Client)
	{
		cout << clsUtility::Tabs(1) << " | " << left << setw(15) << Client.getAccountNumber();
		cout << clsUtility::Tabs(1) << " | " << left << setw(20) << Client.GetFullName();
		cout << clsUtility::Tabs(1) << " | " << left << setw(12) << Client.getPhoneNumber();
		cout << clsUtility::Tabs(1) << " | " << left << setw(20) << Client.getEmail();
		cout << clsUtility::Tabs(1) << " | " << left << setw(10) << Client.getPinCode();
		cout << clsUtility::Tabs(1) << " | " << left << setw(12) << Client.getAccountBalance() << endl;
	}

public:

	static void ShowClientListScreen()
	{
		if (!_CheckAccessRights(clsUser::enPermissions::ListClients))
			return;

		vector <clsClient> vClients = clsClient::GetClientsList();

		string Title = "Client List Screen";
		string SubTitle = "(" + to_string(vClients.size()) + ")" + " Client(s)";

		_DrawScreenHeader(Title, SubTitle, 4);
		cout << clsUtility::Tabs(1) << "___________________________________________________________________________________________________________________________\n\n";
		cout << clsUtility::Tabs(1) << " | " << left << setw(15) << "Account Number";
		cout << clsUtility::Tabs(1) << " | " << left << setw(20) << "Client Name";
		cout << clsUtility::Tabs(1) << " | " << left << setw(12) << "Phone Number";
		cout << clsUtility::Tabs(1) << " | " << left << setw(20) << "Email Address";
		cout << clsUtility::Tabs(1) << " | " << left << setw(10) << "Pin Code";
		cout << clsUtility::Tabs(1) << " | " << left << setw(12) << "Account Balance\n";
		cout << clsUtility::Tabs(1) << "___________________________________________________________________________________________________________________________\n\n";

		if (vClients.size() == 0)
			cout << clsUtility::Tabs(7) << "No clients available on the system.\n";
		else
		{
			for (clsClient& Client : vClients)
				_PrintClientsRecordLine(Client);

			cout << clsUtility::Tabs(1) << "___________________________________________________________________________________________________________________________\n\n";
		}

	}

};

