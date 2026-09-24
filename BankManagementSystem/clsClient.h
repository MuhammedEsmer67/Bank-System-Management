#pragma once
#include "clsString.h"
#include "clsPerson.h"
#include "GlobalUser.h"
#include <fstream>
#include <string>

class clsClient : public clsPerson
{
public:

	struct stTransferLogInfo
	{
		string Date;
		string SourceAccountNumber;
		string DestinationAccountNumber;
		float TransferAmount;
		float SourceAccountBalance;
		float DestinationAccountBalance;
		string UserName;
	};
	
private:

	enum enMode { EmptyClient = 1, UpdateClient = 2, AddNewClient = 3 };

	enMode _Mode;
	string _AccountNumber;
	string _PinCode;
	float _AccountBalance;
	bool _MarkToDelete = false;

	static clsClient _ConvertLineToClientObject(string Line, string Delim = "#//#")
	{
		vector <string> vClient = clsString::Split(Line, Delim);
		return clsClient(enMode::UpdateClient, vClient[0], vClient[1], vClient[2], vClient[3], vClient[4], vClient[5], stof(vClient[6]));
	}

	static clsClient _GetEmptyClientObject()
	{
		return clsClient(enMode::EmptyClient, "", "", "", "", "", "", 0);
	}

	static vector <clsClient> _LoadClientsData()
	{
		vector <clsClient> vClients;
		fstream File;

		File.open("Clients.txt", ios::in);

		if (File.is_open())
		{
			string RecordLine = "";

			while (getline(File, RecordLine))
			{
				clsClient TempClient = _ConvertLineToClientObject(RecordLine);
				vClients.push_back(TempClient);
			}
		}

		File.close();

		return vClients;
	}

	string _ConvertClientObjectToLine(clsClient Client, string Delim = "#//#")
	{
		string DataLine = Client.getFirstName() + Delim;
		DataLine += Client.getLastName() + Delim;
		DataLine += Client.getEmail() + Delim;
		DataLine += Client.getPhoneNumber() + Delim;
		DataLine += Client.getAccountNumber() + Delim;
		DataLine += Client.getPinCode() + Delim;
		DataLine += to_string(Client.getAccountBalance());

		return DataLine;
	}

	void _SaveClientsData(vector <clsClient>& vClients)
	{
		fstream File;

		File.open("Clients.txt", ios::out);

		if (File.is_open())
		{
			string DataLine = "";

			for (clsClient& Client : vClients)
			{
				if (!(Client._MarkToDelete))
				{
					DataLine = _ConvertClientObjectToLine(Client);
					File << DataLine << endl;
				}
			}
		}

		File.close();
	}

	void _AddClientData(string DataLine)
	{
		fstream File;

		File.open("Clients.txt", ios::out | ios::app);

		if (File.is_open())
			File << DataLine << endl;

		File.close();
	}

	void _Update()
	{
		vector <clsClient> vClients = _LoadClientsData();

		for (clsClient& Client : vClients)
		{
			if (Client.getAccountNumber() == _AccountNumber)
			{
				Client = *this;
				break;
			}
		}

		_SaveClientsData(vClients);
	}

	void _AddNewClient()
	{
		_AddClientData(_ConvertClientObjectToLine(*this));
	}

	string _ConvertTransferLogToLine(clsClient ClientTo, float TransferAmount, string Delim = "#//#")
	{
		string LocalTime = clsDate::GetSystemTime();
		string LocalDate = clsDate::ToString(clsDate::GetSystemDate());

		string RegisterLine = LocalDate + " - " + LocalTime + Delim + _AccountNumber + Delim +
			ClientTo.getAccountNumber() + Delim + to_string(TransferAmount) + Delim + to_string(_AccountBalance) 
			+ Delim + to_string(ClientTo.getAccountBalance()) + Delim + CurrentUser.getUserName();

		return RegisterLine;
	}

	void _SaveTransfersData(clsClient ClientTo, float TransferAmount, string Delim = "#//#")
	{
		fstream File;

		File.open("TransferLog.txt", ios::app | ios::out);

		if (File.is_open())
		{
			string TransferLogLine = _ConvertTransferLogToLine(ClientTo, TransferAmount, Delim);

			File << TransferLogLine << endl;
		}

		File.close();
	}

	static stTransferLogInfo _ConvertLineToTransferLog(string TransferLogData, string Delim = "#//#")
	{
		vector <string> vTransferLogInfo = clsString::Split(TransferLogData, Delim);
		stTransferLogInfo TransferLog;

		TransferLog.Date = vTransferLogInfo[0];
		TransferLog.SourceAccountNumber = vTransferLogInfo[1];
		TransferLog.DestinationAccountNumber = vTransferLogInfo[2];
		TransferLog.TransferAmount = stof(vTransferLogInfo[3]);
		TransferLog.SourceAccountBalance = stof(vTransferLogInfo[4]);
		TransferLog.DestinationAccountBalance = stof(vTransferLogInfo[5]);
		TransferLog.UserName = vTransferLogInfo[6];

		return TransferLog;
	}

public:

	clsClient(enMode Mode, string FirstName, string LastName, string Email, string PhoneNumber, string AccountNumber,
		string PinCode, float AccountBalance) : clsPerson(FirstName, LastName, Email, PhoneNumber)
	{
		_Mode = Mode;
		_AccountNumber = AccountNumber;
		_PinCode = PinCode;
		_AccountBalance = AccountBalance;
	}

	void setPinCode(string PinCode) { _PinCode = PinCode; }

	void setAccountBalance(float AccountBalance) { _AccountBalance = AccountBalance; }

	string getAccountNumber() { return _AccountNumber; }

	string getPinCode() { return _PinCode; }

	float getAccountBalance() { return _AccountBalance; }

	bool isEmpty()
	{
		return (_Mode == enMode::EmptyClient);
	}

	static clsClient Find(string AccountNumber)
	{
		vector <clsClient> vClients = _LoadClientsData();

		for (clsClient& Client : vClients)
		{
			if (Client.getAccountNumber() == AccountNumber)
				return Client;
		}

		return _GetEmptyClientObject();
	}

	static clsClient Find(string AccountNumber, string PinCode)
	{
		vector <clsClient> vClients = _LoadClientsData();

		for (clsClient& Client : vClients)
		{
			if (Client.getAccountNumber() == AccountNumber && Client.getPinCode() == PinCode)
				return Client;
		}

		return _GetEmptyClientObject();
	}

	static bool isClientExist(string AccountNumber)
	{
		clsClient Client = Find(AccountNumber);
		return !(Client.isEmpty());
	}

	enum enSaveResults { EmptyObject = 1, FilledObject = 2, AccountNumberExist = 3 };

	enSaveResults Save()
	{
		switch (_Mode)
		{
		case enMode::EmptyClient:
			return enSaveResults::EmptyObject;
		case enMode::AddNewClient:
		{
			if (isClientExist(_AccountNumber))
				return enSaveResults::AccountNumberExist;

			_AddNewClient();
			_Mode = enMode::UpdateClient;

			return enSaveResults::FilledObject;
		}
		case enMode::UpdateClient:
		{
			_Update();
			return enSaveResults::FilledObject;
		}
		}
	}

	static clsClient GetNewClient(string AccountNumber)
	{
		return clsClient(enMode::AddNewClient, "", "", "", "", AccountNumber, "", 0);
	}

	void DeleteClient()
	{
		vector <clsClient> vClients = _LoadClientsData();

		for (clsClient& Client : vClients)
		{
			if (Client.getAccountNumber() == _AccountNumber)
			{
				Client._MarkToDelete = true;
				break;
			}
		}

		_SaveClientsData(vClients);

		*this = _GetEmptyClientObject();
	}

	static vector <clsClient> GetClientsList() { return _LoadClientsData(); }

	static float GetClientsTotalBalances()
	{
		vector <clsClient> vClients = _LoadClientsData();
		float TotalBalances = 0;

		for (clsClient& Client : vClients)
			TotalBalances += Client._AccountBalance;

		return TotalBalances;
	}

	void Deposit(float DepositAmount)
	{
		_AccountBalance += DepositAmount;
		Save();
	}

	void Transfer(clsClient& ClientTo, float TransferAmount)
	{
		Deposit(-TransferAmount);
		ClientTo.Deposit(TransferAmount);
		_SaveTransfersData(ClientTo, TransferAmount);
	}

	bool isBalanceExceed(float BalanceAmount)
	{
		if (BalanceAmount > _AccountBalance)
			return true;

		return false;
	}

	static vector <stTransferLogInfo> LoadTransferData(string Delim = "#//#")
	{
		vector <stTransferLogInfo> vTransferLogData;
		fstream File;

		File.open("TransferLog.txt", ios::in);

		if (File.is_open())
		{
			string TransferLogLine = "";

			while (getline(File, TransferLogLine))
			{
				stTransferLogInfo TransferLog = _ConvertLineToTransferLog(TransferLogLine);
				vTransferLogData.push_back(TransferLog);
			}
		}

		File.close();
		return vTransferLogData;
	}

};

