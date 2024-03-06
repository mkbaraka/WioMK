#ifndef CLASS_ASSISTANT
#define CLASS_ASSISTANT

#include "Arduino.h"


#if defined(ARDUINO_ARCH_AVR)
    #pragma message("Defined architecture for ARDUINO_ARCH_AVR.")
    #define SERIAL Serial
#elif defined(ARDUINO_ARCH_SAM)
    #pragma message("Defined architecture for ARDUINO_ARCH_SAM.")
    #define SERIAL SerialUSB
#elif defined(ARDUINO_ARCH_SAMD)
    #pragma message("Defined architecture for ARDUINO_ARCH_SAMD.")  
    #define SERIAL SerialUSB
#elif defined(ARDUINO_ARCH_STM32F4)
    #pragma message("Defined architecture for ARDUINO_ARCH_STM32F4.")
    #define SERIAL SerialUSB
#else
    #pragma message("Not found any architecture.")
    #define SERIAL Serial
#endif



// Define class blocks duration
extern float magistral_duration[3];
extern float magistral_time_in_a_row;

extern float interactive_duration[3];
extern float interactive_time_in_a_row;

extern float exercises_duration[3];
extern float exercises_time_in_a_row;

extern float laboratory_duration[3];
extern float laboratory_time_in_a_row;


#define NOTIFICATION_EXTRA_BLOCK_TIME 0.3
#define MAX_TIME_IN_A_ROW 2


enum ClassTypes {
    magistral,
    interactive,
    exercises,
    laboratory
};

enum ClassTimeInDay {
    morning,
    noon,
    afternoon
};

enum Day {
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};


class ClassAssistant
{
    public:
        ClassAssistant();
        ~ClassAssistant();
        void fill_data(ClassTypes class_type, int students, ClassTimeInDay class_time_in_day, Day day);

        void start_class();
        void change_class_state();
        void end_class();
        void rate_class(int punctuation);

        void change_class_block();
        bool check_current_time_in_a_row();
        bool check_current_block_time();

        String get_current_block();
        bool get_class_is_running();

        void write_class_data();

    private:
        String class_type = "";
        float* class_duration_guide = nullptr;
        unsigned long class_duration_guide_in_millis[3];
        int students = 0;
        ClassTimeInDay class_time_in_day;
        String time_type;
        Day day;
        String day_type;
        int class_punctuation = 0;

        int current_class_block = 0;
        unsigned long current_class_block_time = 0;
        float class_duration[3];
        unsigned long last_check;
        int breaks = 0;
        unsigned long total_class_time = 0;
        unsigned long break_time = 0;
        bool class_is_running = false;
        unsigned long extra_time_to_notify = 0;
        unsigned long max_time_in_a_row = 0;

};


float convertMillisToHourMin(unsigned long millisTime);
unsigned long convertHourMinToMillis(float hour_min);

// String print_time(DateTime timestamp);


#endif
