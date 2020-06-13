#include "GApp.h"

const static unsigned int days_in_months[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
const static unsigned int days_in_months_leap_year[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };
#define DATETIME_ERROR      0xFFFFFFFF
#define DAYS_IN_400_YEARS   146097
#define DAYS_IN_100_YEARS   36524
#define DAYS_IN_4_YEARS     (365+365+365+366)
unsigned int date_to_days(unsigned int year, unsigned int month, unsigned int day)
{
    if ((year < 0) || (year >= 3000))
        return DATETIME_ERROR;
    if ((month < 1) || (month > 12))
        return DATETIME_ERROR;
    if (day < 1)
        return DATETIME_ERROR;

    unsigned int result = (year / 400) * DAYS_IN_400_YEARS;
    unsigned int rest = year % 400;
    bool is_leap = (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
    result += (rest / 100) * DAYS_IN_100_YEARS;
    if (rest > 0) // primul an din grupup de 400 e bisect
        result++;

    rest = rest % 100;

    if (rest >= 5)
    {
        result += ((rest - 1) / 4)* DAYS_IN_4_YEARS;
        result += 365; // pentru anul 0
        rest = (rest - 1) % 4;
    }
    result += rest * 365;
    result += days_in_months[month - 1] + day - 1;
    if ((is_leap) && (month > 2))
        result++;
    return result;
}
bool days_to_date(unsigned int days, unsigned int &year, unsigned int &month, unsigned int &day)
{
    unsigned int y400 = ((days / DAYS_IN_400_YEARS) * 400);
    unsigned int rest = days % DAYS_IN_400_YEARS;
    unsigned int y100 = 0;
    unsigned int y4 = 0;
    unsigned int y1 = 0;

    if (rest >= (DAYS_IN_100_YEARS + 1)) // exact dupa primii 100 de ani de dupa 400x
    {
        rest = rest - (DAYS_IN_100_YEARS + 1);

        y100 = ((rest / DAYS_IN_100_YEARS) + 1) * 100;
        rest = rest % DAYS_IN_100_YEARS;
        // funct pe formatul 65 [65,65,65,66] x N
        if (rest >= 365)
        {
            rest = rest - 365;
            y4 = (rest / DAYS_IN_4_YEARS) * 4;
            rest = rest % DAYS_IN_4_YEARS;
            if (rest < (365 + 365 + 365)) {
                year = y400 + y100 + 1 + y4 + (rest / 365);
                rest = rest % 365;
            }
            else {
                // e un an bisect
                year = y400 + y100 + 4 + y4;
                rest = (rest - (365 + 365 + 365)) % 366;
            }
        }
        else {
            year = y400 + y100;
        }
    }
    else {
        // sunt in primii 100 de ani de dupa 400x
        if (rest >= 366)
        {
            // sunt dupa primul an (anul 0 din slot-ul de 400)
            rest = rest - 366;
            y4 = (rest / DAYS_IN_4_YEARS) * 4;
            rest = rest % DAYS_IN_4_YEARS;
            if (rest < (365 + 365 + 365)) {
                year = y400 + 1 + y4 + (rest / 365);
                rest = rest % 365;
            }
            else {
                // e un an bisect
                year = y400 + 4 + y4;
                rest = (rest - (365 + 365 + 365)) % 366;
            }
        }
        else {
            year = y400;
        }
    }
    // in rest am luna + zi
    bool is_leap = (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
    const unsigned int * d;
    if (is_leap)
        d = days_in_months_leap_year;
    else
        d = days_in_months;
    const unsigned int * e = d + 12;
    month = 1;
    for (; d < e; d++, month++)
    {
        if ((rest >= (*d)) && (rest < (d[1])))
        {
            // am gasit luna
            day = (rest - (*d)) + 1;
            return true;
        }
    }
    // nu e in lista ==> dau false
    return false;
}

unsigned int GApp::Utils::DateTime::ToDaysFromYear0()
{
    return date_to_days(Year, Month, Day);
}
bool GApp::Utils::DateTime::CreateFromDaysFromYear0(unsigned int daysFromYear0)
{
    unsigned int y, m, d;
    CHECK(days_to_date(daysFromYear0, y, m, d), false, "Fail to convert value: %08X to Y/M/D value !", daysFromYear0);
    Year = y;
    Month = m;
    Day = d;
    return true;
}