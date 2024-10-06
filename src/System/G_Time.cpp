#include "G_Time.h"

void G_Time::PlusSeconds(unsigned int _seconds)
{
    PlusMinutes(AddValue(seconds, _seconds));
}

void G_Time::PlusMinutes(unsigned int _minutes)
{
    hours += AddValue(minutes, _minutes);
}

void G_Time::MinusSeconds(unsigned int _seconds)
{
    MinusMinutes(SubtractValue(seconds, _seconds));
}

void G_Time::MinusMinutes(unsigned int _minutes)
{
    unsigned int minus_hours = SubtractValue(minutes, _minutes);
    if (hours < minus_hours)
    {
        hours = 0;
        minutes = 0;
        seconds = 0;
        return;
    }
    hours -= minus_hours;
}

unsigned int G_Time::GetInSeconds()
{
    return hours * 3600 + minutes * 60 + seconds;
}

unsigned int G_Time::AddValue(unsigned int& rVal, unsigned int add_val)
{
    static const unsigned int measure = 60;

    if (add_val == 0) return 0;

    unsigned int new_val = rVal + add_val;
    rVal = new_val % measure;
    return new_val / measure;
}

unsigned int G_Time::SubtractValue(unsigned int& rVal, unsigned int sub_val)
{
    static const unsigned int measure = 60;

    if (sub_val == 0) return 0;

    unsigned int minus_val = sub_val % measure;
    if (rVal >= minus_val)
    {
        rVal -= minus_val;
        return sub_val / measure;
    }
    else 
    {
        rVal += measure - minus_val;
        return sub_val / measure  + 1;
    }
}