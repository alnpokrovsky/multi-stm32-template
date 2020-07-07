#include <time.h>

time_t time( time_t* timep )
{
    int hour = 0 ;
    int minute = 0 ;
    int second = 0 ;
    int day_of_month = 0 ;
    int month = 0 ; 
    int year = 0 ;

    // Your code here to fill time/date from clock source
    
    
    // Normalise to time.h library epoch time_t (normally Unix epoch)
    struct tm timeinfo;
    timeinfo.tm_mon  = month - 1 ;   // check assumption here Jan = 0 in tm
    timeinfo.tm_mday = day_of_month ;
    timeinfo.tm_year = year + 100 ;  // check assumption here years start from 1900 in tm 
    timeinfo.tm_hour = hour ;
    timeinfo.tm_min  = minute;
    timeinfo.tm_sec  = second;

    // Convert to timestamp
    time_t t = mktime(&timeinfo);
    if( timep != NULL )
    {
        *timep = t ; 
    }

    return t; 
}
