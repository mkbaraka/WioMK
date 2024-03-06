#include "ClassAssistant.h"

// Define class durations
float magistral_duration[3] = {2, 1, 2};
float magistral_time_in_a_row = 2;

float interactive_duration[3] = {1, 1, 1};
float interactive_time_in_a_row = 2;

float exercises_duration[3] = {0, 1, 2};
float exercises_time_in_a_row = 2;

float laboratory_duration[3] = {5, 1, 2};
float laboratory_time_in_a_row = 2;


float convertMillisToHourMin(unsigned long millisTime)
{
    // Calculate hours and minutes
    millisTime /= 1000;
    float hours = millisTime / 3600;
    float minutes = (millisTime / 60) - (static_cast<int>(hours) * 60);

    // Combine hours and minutes
    float combinedTime = hours + (minutes / 100.0);

    return combinedTime;
}

unsigned long convertHourMinToMillis(float hour_min)
{
    // Get hours and minutes from input
    int hours = static_cast<int>(hour_min);
    float fractionalHours = hour_min - hours;
    int minutes = static_cast<int>(fractionalHours * 60);

    // Convert hours to seconds
    unsigned long hoursMillis = hours * 3600;

    // Convert minutes to seconds
    unsigned long minutesMillis = minutes * 60;

    // Sum the seconds for hours and minutes and convert to millis
    unsigned long totalMillis = (hoursMillis + minutesMillis) * 1000;

    // Return the total milliseconds
    return totalMillis;
}


//String print_time(DateTime timestamp) 
//{
//  int Year = timestamp.year();
//  int Month = timestamp.month();
//  int Day = timestamp.day();
//  int Hour = timestamp.hour();
//  int Minute = timestamp.minute();
//  int Second = timestamp.second();
//  randomSeed(analogRead(0));
//  String message = String(Day) + "-" + String(Month) + "-" + String(Year) + "_" + String(random(1, 1000));
//  Serial.println(message);
//  return message;
//}
