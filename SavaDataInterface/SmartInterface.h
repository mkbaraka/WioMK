#ifndef SMART_INTERFACE
#define SMART_INTERFACE

#include "ClassAssistant.h"
#include "TFT_eSPI.h" //TFT LCD library 
#include "SCD30.h"

// #include "SD/Seeed_SD.h"
// #include "RTC_SAMD51.h"
// #include "DateTime.h"


extern TFT_eSPI tft; //Initializing TFT LCD
extern TFT_eSprite spr;
extern ClassAssistant classAssistant;

void show_measured_data();
void DMAC_1_Handler() ;
void plotNeedle();
void analogMeter();



enum ProgramState{
    show_data,
    noise,
    start_class,
    change_block
};

class NotificationAlert
{
    public:
        NotificationAlert(int show_time, int delay_time, String text);
        void activate();
        void deactivate();
        void check_notification_time();
        bool get_activation();
        String get_text();

    private:
        unsigned long show_time = 0;
        unsigned long delay_time = 0;
        String text;
        
        unsigned long activation_time = 0;
        unsigned long last_notification = 0;
        bool activated = false;
};

class Tracker
{
    public:
        Tracker();
        void gather_data(float h, int l, float a, float t);
        void calculate_averages();
        float get_humidity_average();
        float get_light_average();
        float get_air_quality_average();
        float get_temperature_average();
        void write_data();
    
    private:
        void check_limits(float data[4]);

        // humidity, light, air quality, temperature
        float accumulative_values[4] = {0, 0, 0, 0};
        float max_values[4] = {0, 0, 0, 0};
        float min_values[4] = {9999, 999999, 9999999, 999999};
        float average_values[4] = {0.0, 0.0, 0.0, 0.0};
        unsigned long n_entries = 0;  

};

void start_class_menu();
bool change_block_menu(String block);
int end_class_menu();
int increment_with_check(int n, int delta, int min, int max);

#endif
