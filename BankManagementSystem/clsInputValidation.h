#pragma once
#include <limits>
#include "clsUtility.h"

using namespace std;

class clsInputValidation
{
public:

	static bool isIntegerBetween(int Number, int From, int To)
	{
		if (From > To)
			clsUtility::Swap(From, To);

		return (Number >= From && Number <= To);
	}

	static bool isShortBetween(short Number, short From, short To)
	{
		if (From > To)
			clsUtility::Swap(From, To);

		return (Number >= From && Number <= To);
	}

	static bool isDoubleBetween(double Number, double From, double To)
	{
		if (From > To)
			clsUtility::Swap(From, To);

		return (Number >= From && Number <= To);
	}

	static bool isFloatBetween(float Number, float From, float To)
	{
		if (From > To)
			clsUtility::Swap(From, To);

		return (Number >= From && Number <= To);
	}

	static bool isDateBetween(clsDate Date, clsDate DateFrom, clsDate DateTo)
	{
		if (clsDate::isDateAfterDate2(DateFrom, DateTo))
			clsUtility::Swap(DateFrom, DateTo);

		return ((clsDate::isDateAfterDate2(Date, DateFrom) || clsDate::areDatesEqual(Date,DateFrom)) && 
			(clsDate::isDateBeforeDate2(Date, DateTo) || clsDate::areDatesEqual(Date, DateTo)));
	}

	static int ReadInteger(string ErrorMessage)
	{
		int Number = 0;

		while (!(cin >> Number))
		{
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			cout << ErrorMessage;
		}

		return Number;
	}

	static int ReadPositiveInteger(string FailedMessage, string ErrorMessage)
	{
		int Number = ReadInteger(ErrorMessage);

		while (Number < 0)
		{
			cout << FailedMessage;
			Number = ReadInteger(ErrorMessage);
		}

		return Number;
	}

	static int ReadIntegerBetween(int From, int To, string FailedMessage, string ErrorMessage)
	{
		int Number = ReadInteger(ErrorMessage);

		while (!isIntegerBetween(Number, From, To))
		{
			cout << FailedMessage;
			Number = ReadInteger(ErrorMessage);
		}

		return Number;
	}

	static short ReadShort(string ErrorMessage)
	{
		short Number = 0;

		while (!(cin >> Number))
		{
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			cout << ErrorMessage;
		}

		return Number;
	}

	static short ReadPositiveShort(string FailedMessage, string ErrorMessage)
	{
		short Number = ReadShort(ErrorMessage);

		while (Number < 0)
		{
			cout << FailedMessage;
			Number = ReadShort(ErrorMessage);
		}

		return Number;
	}

	static short ReadShortBetween(short From, short To, string FailedMessage, string ErrorMessage)
	{
		short Number = ReadShort(ErrorMessage);

		while (!isIntegerBetween(Number, From, To))
		{
			cout << FailedMessage;
			Number = ReadShort(ErrorMessage);
		}

		return Number;
	}

	static double ReadDouble(string ErrorMessage)
	{
		double Number = 0;

		cin >> Number;

		while (cin.fail())
		{
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			cout << ErrorMessage;
			cin >> Number;
		}

		return Number;
	}

	static double ReadPositiveDouble(string FailedMessage, string ErrorMessage)
	{
		double Number = ReadDouble(ErrorMessage);

		while (Number < 0)
		{
			cout << FailedMessage;
			Number = ReadDouble(ErrorMessage);
		}

		return Number;
	}

	static double ReadDoubleBetween(double From, double To, string FailedMessage, string ErrorMessage)
	{
		double Number = ReadDouble(ErrorMessage);

		while (!isDoubleBetween(Number, From, To))
		{
			cout << FailedMessage;
			Number = ReadDouble(ErrorMessage);
		}

		return Number;
	}

	static float ReadFloat(string ErrorMessage)
	{
		float Number = 0;

		cin >> Number;

		while (cin.fail())
		{
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			cout << ErrorMessage;
			cin >> Number;
		}

		return Number;
	}

	static float ReadPositiveFloat(string FailedMessage, string ErrorMessage)
	{
		float Number = ReadFloat(ErrorMessage);

		while (Number < 0)
		{
			cout << FailedMessage;
			Number = ReadFloat(ErrorMessage);
		}

		return Number;
	}

	static float ReadFloatBetween(float From, float To, string FailedMessage, string ErrorMessage)
	{
		float Number = ReadFloat(ErrorMessage);

		while (!isDoubleBetween(Number, From, To))
		{
			cout << FailedMessage;
			Number = ReadFloat(ErrorMessage);
		}

		return Number;
	}

	static char ReadCharacter()
	{
		char Char = ' ';
		cin >> Char;

		return Char;
	}

	static bool ReadBoolean()
	{
		bool Boolean = true;
		cin >> Boolean;

		return Boolean;
	}

	static string ReadString()
	{
		string String = "";
		cin >> String;

		return String;
	}

	static string ReadText()
	{
		string Text = "";
		getline(cin >> ws, Text);

		return Text;
	}

	static bool isValidDate(clsDate Date)
	{
		return clsDate::isValidDate(Date);
	}

};

