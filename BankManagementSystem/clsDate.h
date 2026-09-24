#pragma once
#pragma warning (disable : 4996)
#include <string>
#include "clsString.h"

using namespace std;

class clsDate
{
private:

	short _Day;
	short _Month;
	short _Year;
	char _Delim;

public:

	enum enCompareDates { Before = -1, Equal = 0, After = 1 };

	clsDate() { *this = GetSystemDate(); }

	clsDate(string DateAsString)
	{
		ToDate(DateAsString);

		if (!isValidDate())
			*this = GetSystemDate();
	}

	clsDate(short Day, short Month, short Year)
	{
		_Day = Day;
		_Month = Month;
		_Year = Year;

		if (!isValidDate())
			*this = GetSystemDate();
	}

	clsDate(short DayOrder, short Year)
	{
		*this = ConvertDayOrder(DayOrder, Year);

		if (!isValidDate())
			*this = GetSystemDate();
	}

	void setDay(short Day) { if (!isInvalidDay(_Year, _Month, Day)) _Day = Day; }

	void setMonth(short Month) { if (!isInvalidMonth(Month)) _Month = Month; }

	void setYear(short Year) { if (!isInvalidYear(Year)) _Year = Year; }

	short getDay() { return _Day; }

	short getMonth() { return _Month; }

	short getYear() { return _Year; }

	static clsDate GetSystemDate()
	{
		short Day = 0, Month = 0, Year = 0;

		time_t TimeInSec = time(0);
		tm* stTime = localtime(&TimeInSec);

		Day = stTime->tm_mday;
		Month = stTime->tm_mon + 1;
		Year = stTime->tm_year + 1900;

		return clsDate(Day, Month, Year);
	}

	static string GetSystemTime()
	{
		short Hour = 0, Minutes = 0, Seconds = 0;
		string Time = "";

		time_t TimeInSec = time(0);
		tm* stTime = localtime(&TimeInSec);

		Hour = stTime->tm_hour;
		Minutes = stTime->tm_min;
		Seconds = stTime->tm_sec;

		Time = to_string(Hour) + ":" + to_string(Minutes) + ":" + to_string(Seconds);

		return Time;
	}

	static short MonthDays(short Month, short Year)
	{
		int MonthsDaysArray[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };

		return (Month == 2) ? (isLeapYear(Year) ? 29 : 28) : MonthsDaysArray[Month];
	}

	short MonthDays()
	{
		return MonthDays(_Month, _Year);
	}

	static bool isInvalidDay(short Year, short Month, short Day)
	{
		return (Day > MonthDays(Month, Year) || Day < 1);
	}

	static clsDate ConvertDayOrder(short DayOrder, short Year)
	{
		clsDate Date;
		Date._Month = 0;
		Date._Year = Year;

		while (DayOrder > 0)
			DayOrder -= MonthDays(++Date._Month, Year);

		DayOrder += MonthDays(Date._Month, Year);
		Date._Day = DayOrder;

		return Date;
	}

	static clsDate ToDate(string DateAsString)
	{
		vector <string> vDate;
		clsDate Date;

		vDate = clsString::Split(DateAsString, "/");

		Date._Day = stoi(vDate[0]);
		Date._Month = stoi(vDate[1]);
		Date._Year = stoi(vDate[2]);

		return Date;
	}

	static string ToString(clsDate Date, string Delim = "/")
	{
		return to_string(Date._Day) + Delim + to_string(Date._Month) + Delim + to_string(Date._Year);
	}

	string ToString(string Delim = "/")
	{
		return ToString(*this, Delim);
	}

	static string FormatDate(clsDate Date, string Format = "dd/mm/yyyy")
	{
		string FormattedDate = "";

		FormattedDate = clsString::ReplaceWord(Format, "dd", to_string(Date._Day));
		FormattedDate = clsString::ReplaceWord(FormattedDate, "mm", to_string(Date._Month));
		FormattedDate = clsString::ReplaceWord(FormattedDate, "yyyy", to_string(Date._Year));

		return FormattedDate;
	}

	string FormatDate(string Format = "dd/mm/yyyy")
	{
		return FormatDate(*this, Format);
	}

	static bool isInvalidMonth(short Month)
	{
		return (Month > 12 || Month < 1);
	}

	static bool isInvalidYear(short Year)
	{
		return (Year < 0);
	}

	static bool isLeapYear(short Year)
	{
		return (Year % 400 == 0) || (Year % 100 != 0 && Year % 4 == 0);
	}

	bool isLeapYear()
	{
		return isLeapYear(_Year);
	}

	static bool isValidDate(clsDate Date)
	{
		return (MonthDays(Date._Month, Date._Year) < Date._Day) ? false : ((12 < Date._Month) ? false : true);
	}

	bool isValidDate()
	{
		return isValidDate(*this);
	}

	static short DaysInYear(short Year)
	{
		return (isLeapYear(Year)) ? 366 : 365;
	}

	short DaysInYear()
	{
		return DaysInYear(_Year);
	}

	static short HoursInYear(short Year)
	{
		return 24 * DaysInYear(Year);
	}

	short HoursInYear()
	{
		return HoursInYear(_Year);
	}

	static int MinutesInYear(short Year)
	{
		return 60 * HoursInYear(Year);
	}

	int MinutesInYear()
	{
		return MinutesInYear(_Year);
	}

	static int SecondsInYear(short Year)
	{
		return 60 * MinutesInYear(Year);
	}

	int SecondsInYear()
	{
		return SecondsInYear(_Year);
	}

	static short HoursInMonth(short Month, short Year)
	{
		return 24 * MonthDays(Month, Year);
	}

	short HoursInMonth()
	{
		return HoursInMonth(_Month, _Year);
	}

	static int MinutesInMonth(short Month, short Year)
	{
		return 60 * HoursInMonth(Month, Year);
	}

	int MinutesInMonth()
	{
		return MinutesInMonth(_Month, _Year);
	}

	static int SecondsInMonth(short Month, short Year)
	{
		return 60 * MinutesInMonth(Month, Year);
	}

	int SecondsInMonth()
	{
		return SecondsInMonth(_Month, _Year);
	}

	static short GetDayIndex(short Day, short Month, short Year)
	{
		short a = (14 - Month) / 12;
		short y = Year - a;
		short m = Month + 12 * a - 2;

		return (Day + y + (y / 4) - (y / 100) + (y / 400) + ((31 * m) / 12)) % 7;
	}

	short GetDayIndex()
	{
		return GetDayIndex(_Day, _Month, _Year);
	}

	static string PrintDay(short Day, short Month, short Year)
	{
		string DaysArray[7] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };

		return DaysArray[GetDayIndex(Day, Month, Year)];
	}

	string PrintDay()
	{
		return PrintDay(_Day, _Month, _Year);
	}

	static string PrintMonth(short Month)
	{
		string MonthsArray[13] = { "","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };

		return MonthsArray[Month];
	}

	string PrintMonth()
	{
		return PrintMonth(_Month);
	}

	static void PrintCalender(short Month, short Year)
	{
		short DaysInMonth = MonthDays(Month, Year);
		short DayIndex = GetDayIndex(Year, Month, 1);

		cout << "\n ________________" << PrintMonth(Month) << "________________\n\n";
		cout << "  Sun  Mon  Tue  Wed  Thu  Fri  Sat\n";

		for (short i = 1; i <= DayIndex; i++)
			cout << "     ";

		for (short j = 1; j <= DaysInMonth; j++)
		{
			printf("%5d", j);

			if (++DayIndex == 7)
			{
				DayIndex = 0;
				cout << endl;
			}
		}

		cout << "\n ____________________________________\n";
	}

	void PrintCalender()
	{
		PrintCalender(_Month, _Year);
	}

	static short GetTotalDays(short Day, short Month, short Year)
	{
		short TotalDays = 0;

		for (short i = 1; i < Month; i++)
			TotalDays += MonthDays(i, Year);

		TotalDays += Day;

		return TotalDays;
	}

	short GetTotalDays()
	{
		return GetTotalDays(_Day, _Month, _Year);
	}

	static clsDate AddDays(short Day, short Month, short Year, short DaysToAdd)
	{
		clsDate DateInfo;
		DateInfo._Month = Month - 1;
		DateInfo._Year = Year;
		DaysToAdd += Day;

		while (DaysToAdd > 0)
		{
			if (++DateInfo._Month > 12)
			{
				DateInfo._Month = 1;
				DateInfo._Year++;
			}

			DaysToAdd -= MonthDays(DateInfo._Month, DateInfo._Year);
		}

		DaysToAdd += MonthDays(DateInfo._Month, DateInfo._Year);
		DateInfo._Day = DaysToAdd;

		return DateInfo;
	}

	void AddDays(short DaysToAdd)
	{
		*this = AddDays(_Day, _Month, _Year, DaysToAdd);
	}

	static bool isDateAfterDate2(clsDate StartDate, clsDate EndDate)
	{
		return (StartDate._Year > EndDate._Year ? true : (StartDate._Year == EndDate._Year ? (StartDate._Month > EndDate._Month ? true : (StartDate._Month == EndDate._Month ? (StartDate._Day > EndDate._Day) : false)) : false));
	}

	bool isDateAfterDate2(clsDate EndDate)
	{
		return isDateAfterDate2(*this, EndDate);
	}

	static bool areDatesEqual(clsDate StartDate, clsDate EndDate)
	{
		return (StartDate._Year == EndDate._Year && StartDate._Month == EndDate._Month && StartDate._Day == EndDate._Day);
	}

	bool areDatesEqual(clsDate EndDate)
	{
		return areDatesEqual(*this, EndDate);
	}

	static bool isLastDayInMonth(short Day, short Month, short Year)
	{
		return (MonthDays(Month, Year) == Day);
	}

	bool isLastDayInMonth()
	{
		return isLastDayInMonth(_Day, _Month, _Year);
	}

	static bool isLastMonth(short Month)
	{
		return (Month == 12);
	}

	bool isLastMonth()
	{
		return isLastMonth(_Month);
	}

	static clsDate OneDayIncrement(clsDate Date)
	{
		isLastDayInMonth(Date._Day, Date._Month, Date._Year) ? (Date._Day = 1, (isLastMonth(Date._Month)) ? Date._Month = 1, Date._Year++ : Date._Month++) : Date._Day++;

		return Date;
	}

	void OneDayIncrement()
	{
		*this = OneDayIncrement(*this);
	}

	static bool isDateBeforeDate2(clsDate StartDate, clsDate EndDate)
	{
		return !(isDateAfterDate2(StartDate, EndDate) || areDatesEqual(StartDate, EndDate));
	}

	bool isDateBeforeDate2(clsDate EndDate)
	{
		return isDateBeforeDate2(*this, EndDate);
	}

	static short DatesDifference(clsDate StartDate, clsDate EndDate, bool isIncludeLastDay = false)
	{
		short SwapFlagValue = 1;
		short DaysDiffCounter = 0;
		clsDate Temp;

		if (isDateAfterDate2(StartDate, EndDate))
		{
			Temp = StartDate;
			StartDate = EndDate;
			EndDate = Temp;

			SwapFlagValue = -1;
		}

		while (!areDatesEqual(StartDate, EndDate))
		{
			DaysDiffCounter++;
			StartDate = OneDayIncrement(StartDate);
		}

		return isIncludeLastDay ? ++DaysDiffCounter * SwapFlagValue : DaysDiffCounter * SwapFlagValue;
	}

	short DatesDifference(clsDate EndDate, bool isIncludeLastDay = false)
	{
		return DatesDifference(*this, EndDate, isIncludeLastDay);
	}

	static clsDate DaysIncrement(clsDate Date, short Days)
	{
		for (short i = 1; i <= Days; i++)
			Date = OneDayIncrement(Date);

		return Date;
	}

	void DaysIncrement(short Days)
	{
		*this = DaysIncrement(*this, Days);
	}

	static clsDate OneMonthIncrement(clsDate Date)
	{
		isLastMonth(Date._Month) ? Date._Month = 1, Date._Year++ : Date._Month++;

		short NewMonthDays = MonthDays(Date._Month, Date._Year);

		if (Date._Day > NewMonthDays)
			Date._Day = NewMonthDays;

		return Date;
	}

	void OneMonthIncrement()
	{
		*this = OneMonthIncrement(*this);
	}

	static clsDate MonthsIncrement(clsDate Date, short Months)
	{
		for (short i = 1; i <= Months; i++)
			Date = OneMonthIncrement(Date);

		return Date;
	}

	void MonthsIncrement(short Months)
	{
		*this = MonthsIncrement(*this, Months);
	}

	static clsDate YearsIncrement(clsDate Date, short Years)
	{
		Date._Year += Years;

		if (!(isLeapYear(Date._Year)) && Date._Month == 2 && Date._Day == 29)
		{
			Date._Day = 1;
			Date._Month = 3;
		}

		return Date;
	}

	void YearsIncrement(short Years)
	{
		*this = YearsIncrement(*this, Years);
	}

	static bool isFirstMonth(short Month)
	{
		return (Month == 1);
	}

	bool isFirstMonth()
	{
		return isFirstMonth(_Month);
	}

	static bool isFirstDay(short Day)
	{
		return (Day == 1);
	}

	bool isFirstDay()
	{
		return isFirstDay(_Day);
	}

	static clsDate OneDayDecrement(clsDate Date)
	{
		isFirstDay(Date._Day) ? (isFirstMonth(Date._Month) ? Date._Year--, Date._Month = 12, Date._Day = 31 : Date._Month--, Date._Day = MonthDays(Date._Month, Date._Year)) : Date._Day--;

		return Date;
	}

	void OneDayDecrement()
	{
		*this = OneDayDecrement(*this);
	}

	static clsDate DaysDecrement(clsDate Date, short Days)
	{
		for (short i = 1; i <= Days; i++)
			Date = OneDayDecrement(Date);

		return Date;
	}

	void DaysDecrement(short Days)
	{
		*this = DaysDecrement(*this, Days);
	}

	static clsDate OneMonthDecrement(clsDate Date)
	{
		isFirstMonth(Date._Month) ? Date._Month = 12, Date._Year-- : Date._Month--;

		short NewMonthDays = MonthDays(Date._Month, Date._Year);

		if (Date._Day > NewMonthDays)
			Date._Day = NewMonthDays;

		return Date;
	}

	void OneMonthDecrement()
	{
		*this = OneMonthDecrement(*this);
	}

	static clsDate MonthsDecrement(clsDate Date, short Months)
	{
		for (short i = 1; i <= Months; i++)
			Date = OneMonthDecrement(Date);

		return Date;
	}

	void MonthsDecrement(short Months)
	{
		*this = MonthsDecrement(*this, Months);
	}

	static clsDate YearsDecrement(clsDate Date, short Years)
	{
		Date._Year -= Years;

		if (!(isLeapYear(Date._Year)) && Date._Month == 2 && Date._Day == 29)
			Date._Day = 28;

		return Date;
	}

	void YearsDecrement(short Years)
	{
		*this = YearsDecrement(*this, Years);
	}

	static bool isEndOfWeek(clsDate Date)
	{
		short DayIndex = GetDayIndex(Date._Year, Date._Month, Date._Day);

		return (DayIndex == 6);
	}

	bool isEndOfWeek()
	{
		return isEndOfWeek(*this);
	}

	static bool isWeekEnd(clsDate Date)
	{
		short DayIndex = GetDayIndex(Date._Year, Date._Month, Date._Day);

		return (DayIndex == 0 || DayIndex == 6);
	}

	bool isWeekEnd()
	{
		return isWeekEnd(*this);
	}

	static bool isBusinessDay(clsDate Date)
	{
		return !isWeekEnd(Date);
	}

	bool isBusinessDay()
	{
		return isBusinessDay(*this);
	}

	static short DaysUntilEndOfWeek(clsDate Date)
	{
		short DayIndex = GetDayIndex(Date._Year, Date._Month, Date._Day);

		return 6 - DayIndex;
	}

	short DaysUntilEndOfWeek()
	{
		return DaysUntilEndOfWeek(*this);
	}

	static short DaysUntilEndOfMonth(clsDate Date)
	{
		return MonthDays(Date._Month, Date._Year) - Date._Day;
	}

	short DaysUntilEndOfMonth()
	{
		return DaysUntilEndOfMonth(*this);
	}

	static short DaysUntilEndOfYear(clsDate Date)
	{
		return DaysInYear(isLeapYear(Date._Year)) - GetTotalDays(Date._Day, Date._Month, Date._Year);
	}

	short DaysUntilEndOfYear()
	{
		return DaysUntilEndOfYear(*this);
	}

	static short GetVacationDays(clsDate StartDate, clsDate EndDate)
	{
		short VacationDays = 0;

		while (!areDatesEqual(StartDate, EndDate))
		{
			if (isBusinessDay(StartDate))
				VacationDays++;

			StartDate = OneDayIncrement(StartDate);
		}

		return VacationDays;
	}

	short GetVacationDays(clsDate EndDate)
	{
		return GetVacationDays(*this, EndDate);
	}

	static clsDate GetVacationReturnDay(clsDate Date, short VacationDays)
	{
		while (VacationDays > 0)
		{
			if (isBusinessDay(Date))
				VacationDays--;

			Date = OneDayIncrement(Date);
		}

		while (isWeekEnd(Date))
			Date = OneDayIncrement(Date);

		return Date;
	}

	static enCompareDates CompareDates(clsDate StartDate, clsDate EndDate)
	{
		return (areDatesEqual(StartDate, EndDate) ? enCompareDates::Equal : (isDateAfterDate2(StartDate, EndDate) ? enCompareDates::After : enCompareDates::Before));
	}

	enCompareDates CompareDates(clsDate EndDate)
	{
		return CompareDates(*this, EndDate);
	}

};

