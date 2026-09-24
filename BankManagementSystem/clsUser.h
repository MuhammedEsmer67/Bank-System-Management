#pragma once
#include "clsString.h"
#include "clsPerson.h"
#include "clsDate.h"
#include "clsUtility.h"
#include <fstream>
#include <string>

class clsUser : public clsPerson
{
public:

	struct stUserLoginData
	{
		string Date;
		string UserName;
		string Password;
		string Permissions;
	};

private:

	enum enMode { EmptyUser = 1, UpdateUser = 2, AddNewUser = 3 };

	enMode _Mode;
	string _UserName;
	string _Password;
	short _Permissions;
	bool _MarkToDelete = false;

	static clsUser _ConvertLineToUserObject(string Line, string Delim = "#//#")
	{
		vector <string> vUsers = clsString::Split(Line, Delim);
		return clsUser(enMode::UpdateUser, vUsers[0], vUsers[1], vUsers[2], vUsers[3], vUsers[4], clsUtility::DecryptText(vUsers[5], 16), stof(vUsers[6]));
	}

	static clsUser _GetEmptyUserObject()
	{
		return clsUser(enMode::EmptyUser, "", "", "", "", "", "", 0);
	}

	static vector <clsUser> _LoadUsersData()
	{
		vector <clsUser> vUsers;
		fstream File;

		File.open("Users.txt", ios::in);

		if (File.is_open())
		{
			string RecordLine = "";

			while (getline(File, RecordLine))
			{
				clsUser TempUser = _ConvertLineToUserObject(RecordLine);
				vUsers.push_back(TempUser);
			}
		}

		File.close();

		return vUsers;
	}

	string _ConvertUserObjectToLine(clsUser User, string Delim = "#//#")
	{
		string DataLine = User.getFirstName() + Delim;
		DataLine += User.getLastName() + Delim;
		DataLine += User.getEmail() + Delim;
		DataLine += User.getPhoneNumber() + Delim;
		DataLine += User.getUserName() + Delim;
		DataLine += clsUtility::EncryptText(User.getPassword(), 16) + Delim;
		DataLine += to_string(User.getPermissions());

		return DataLine;
	}

	void _SaveUsersData(vector <clsUser>& vUsers)
	{
		fstream File;

		File.open("Users.txt", ios::out);

		if (File.is_open())
		{
			string DataLine = "";

			for (clsUser& User : vUsers)
			{
				if (!(User._MarkToDelete))
				{
					DataLine = _ConvertUserObjectToLine(User);
					File << DataLine << endl;
				}
			}
		}

		File.close();
	}

	void _AddUserData(string DataLine)
	{
		fstream File;

		File.open("Users.txt", ios::out | ios::app);

		if (File.is_open())
			File << DataLine << endl;

		File.close();
	}

	void _Update()
	{
		vector <clsUser> vUsers = _LoadUsersData();

		for (clsUser& User : vUsers)
		{
			if (User.getUserName() == _UserName)
			{
				User = *this;
				break;
			}
		}

		_SaveUsersData(vUsers);
	}

	void _AddNewUser()
	{
		_AddUserData(_ConvertUserObjectToLine(*this));
	}

	string _ConvertUserToRegisterLine(string Delim = "#//#")
	{
		string LocalTime = clsDate::GetSystemTime();
		string LocalDate = clsDate::ToString(clsDate::GetSystemDate());

		string RegisterLine = LocalDate + " - " + LocalTime + Delim + _UserName + Delim +
			clsUtility::EncryptText(_Password, 16) + Delim + to_string(_Permissions);

		return RegisterLine;
	}

	static stUserLoginData _ConvertRegisterLineToUser(string Line, string Delim = "#//#")
	{
		vector <string> vUser = clsString::Split(Line, Delim);
		
		stUserLoginData UserData;
		UserData.Date = vUser[0];
		UserData.UserName = vUser[1];
		UserData.Password = clsUtility::DecryptText(vUser[2], 16);
		UserData.Permissions = vUser[3];

		return UserData;
	}

public:

	enum enPermissions { All = -1, ListClients = 1, AddNewClient = 2, DeleteClient = 4, UpdateClient = 8,
		FindClient = 16, Transactions = 32, ManageUsers = 64, LoginRegister = 128, CurrencyExchange = 256 };

	clsUser(enMode Mode, string FirstName, string LastName, string Email, string PhoneNumber, string UserName,
		string Password, short Permissions) : clsPerson(FirstName, LastName, Email, PhoneNumber)
	{
		_Mode = Mode;
		_UserName = UserName;
		_Password = Password;
		_Permissions = Permissions;
	}

	void setUserName(string UserName) { _UserName = UserName; }

	void setPassword(string Password) { _Password = Password; }

	void setPermissions(short Permissions) { _Permissions = Permissions; }

	string getUserName() { return _UserName; }

	string getPassword() { return _Password; }

	short getPermissions() { return _Permissions; }

	bool isEmpty()
	{
		return (_Mode == enMode::EmptyUser);
	}

	static clsUser Find(string UserName)
	{
		vector <clsUser> vUsers = _LoadUsersData();

		for (clsUser& User : vUsers)
		{
			if (User.getUserName() == UserName)
				return User;
		}

		return _GetEmptyUserObject();
	}

	static clsUser Find(string UserName, string Password)
	{
		vector <clsUser> vUsers = _LoadUsersData();

		for (clsUser& User : vUsers)
		{
			if (User.getUserName() == UserName && User.getPassword() == Password)
				return User;
		}

		return _GetEmptyUserObject();
	}

	static bool isUserExist(string UserName)
	{
		clsUser Client = Find(UserName);
		return !(Client.isEmpty());
	}

	enum enSaveResults { EmptyObject = 1, FilledObject = 2, UserNameExist = 3 };

	enSaveResults Save()
	{
		switch (_Mode)
		{
		case enMode::EmptyUser:
			return enSaveResults::EmptyObject;
		case enMode::AddNewUser:
		{
			if (isUserExist(_UserName))
				return enSaveResults::UserNameExist;

			_AddNewUser();
			_Mode = enMode::UpdateUser;

			return enSaveResults::FilledObject;
		}
		default:
		{
			_Update();
			return enSaveResults::FilledObject;
		}
		}
	}

	static clsUser GetNewUser(string UserName)
	{
		return clsUser(enMode::AddNewUser, "", "", "", "", UserName, "", 0);
	}

	void DeleteUser()
	{
		vector <clsUser> vUsers = _LoadUsersData();

		for (clsUser& User : vUsers)
		{
			if (User.getUserName() == _UserName)
			{
				User._MarkToDelete = true;
				break;
			}
		}

		_SaveUsersData(vUsers);

		*this = _GetEmptyUserObject();
	}

	static vector <clsUser> GetUsersList() { return _LoadUsersData(); }

	bool isPermissionAccessible(enPermissions Permission)
	{
		if (_Permissions == enPermissions::All)
			return true;

		return (Permission & _Permissions);
	}

	void SaveLoginRegister()
	{
		fstream File;

		File.open("LoginRegister.txt", ios::app | ios::out);

		if (File.is_open())
		{
			string RegisterLine = _ConvertUserToRegisterLine();

			File << RegisterLine << endl;
		}

		File.close();
	}

	static vector <stUserLoginData> LoadRegisterLoginData(string Delim = "#//#")
	{
		vector <stUserLoginData> vUser;
		stUserLoginData User;
		fstream File;

		File.open("LoginRegister.txt", ios::in);

		if (File.is_open())
		{
			string Line = "";
			stUserLoginData TempUser;

			while (getline(File, Line))
			{
				User = _ConvertRegisterLineToUser(Line, Delim);
				vUser.push_back(User);
			}
		}

		File.close();

		return vUser;
	}

};

