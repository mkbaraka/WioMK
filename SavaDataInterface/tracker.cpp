#include "SmartInterface.h"

Tracker::Tracker()
{
    for (int i = 0; i < 4; i++)
    {
        accumulative_values[i] = 0;
        min_values[i] = 0;
        max_values[i] = 0;
        average_values[i] = 0;
    }
}

void Tracker::gather_data(float h, int l, float a, float t)
{
    accumulative_values[0] += h;
    accumulative_values[1] += static_cast<float>(l);
    accumulative_values[2] += a;
    accumulative_values[3] += t;
    n_entries += 1;
    float data[4] = {h, static_cast<float>(l), a, t};
    check_limits(data);
}


void Tracker::calculate_averages()
{
    for (int i = 0; i < 4; i++)
    {
        average_values[i] = accumulative_values[i] / n_entries;
    }
    accumulative_values[0] = 0;
    accumulative_values[1] = 0;
    accumulative_values[2] = 0;
    accumulative_values[3] = 0;
    n_entries = 0;
}

float Tracker::get_humidity_average()
{
    return average_values[0];
}

float Tracker::get_light_average()
{
    return average_values[1];
}

float Tracker::get_air_quality_average()
{
    return average_values[2];
}

float Tracker::get_temperature_average()
{
    return average_values[3];
}


void Tracker::check_limits(float data[4])
{
    for (int i = 0; i < 4; i++)
    {
        if (data[i] > max_values[i])
            max_values[i] = data[i];
        if (data[i] < min_values[i])
            min_values[i] = data[i];
    }
}

// #include "SD/Seeed_SD.h"
// #include "RTC_SAMD51.h"
// #include "DateTime.h"

 void Tracker::write_data()
 {
    DateTime now = DateTime(F(__DATE__), F(__TIME__));
    rtc.adjust(now);
    now = rtc.now();
    String nameFile = "measured_data";
    String csv = ".csv";
    calculate_averages();
    nameFile.concat(csv);

    String data_txt = String(average_values[0]) + ","
            + String(max_values[0]) + ","
            + String(min_values[0]) + ","
            + String(average_values[1]) + ","
            + String(max_values[1]) + ","
            + String(min_values[1]) + ","
            + String(average_values[2]) + ","
            + String(max_values[2]) + ","
            + String(min_values[2]) + ","
            + String(average_values[3]) + ","
            + String(max_values[3]) + ","
            + String(min_values[3]);

    now = rtc.now();
    String datetime = now.timestamp(DateTime::TIMESTAMP_FULL);
    String save_txt = datetime + "," + data_txt;

    Serial.println("Saved Data: "+ String(save_txt));
    File myFile;
    myFile = SD.open(nameFile, FILE_APPEND);
    myFile.println(save_txt);
    myFile.close();
 }
