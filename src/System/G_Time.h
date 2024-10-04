#pragma once

#include <ZC/Tools/Math/ZC_Math.h>

struct G_Time
{
    unsigned int seconds = 0;
    unsigned int minutes = 0;
    unsigned int hours = 0;

        //  add seonds
    void PlusSeconds(unsigned int _seconds);
        //  add minutes
    void PlusMinutes(unsigned int _minutes);
        //  subtract seonds
    void MinusSeconds(unsigned int _seconds);
        //  subtract minutes
    void MinusMinutes(unsigned int _minutes);
        //  return total time in seconds
    unsigned int GetInSeconds();

private:
    /*
    Add value in 60 measure (seconds, minutes...). If measure overflow, overflow will returned with 60 measure.

    Params:
    - rVal - reeference on value to update.
    - add_val - value to add

    Return:
    Overflow in 60 measure. Example with seconds: if rVal = 10, add_val 124 => rVal = 14, return 2.
    */
    unsigned int AddValue(unsigned int& rVal, unsigned int add_val);
    //  work like AddValue but in subtract
    unsigned int SubtractValue(unsigned int& rVal, unsigned int sub_val);
};