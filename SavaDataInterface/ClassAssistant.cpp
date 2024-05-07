#include "ClassAssistant.h"

// Constructor
ClassAssistant::ClassAssistant()
{
    class_duration_guide = magistral_duration;
    max_time_in_a_row = convertHourMinToMillis(magistral_time_in_a_row);

    for(int i = 0; i < 3; i++){
        class_duration_guide_in_millis[i] = convertHourMinToMillis(class_duration_guide[i]);
        class_duration[i] = 0.0;
    }

    this->students = 0;
    this->class_time_in_day = morning;
    this->day = Monday;
    extra_time_to_notify = convertHourMinToMillis(NOTIFICATION_EXTRA_BLOCK_TIME);
}

// Destructor
ClassAssistant::~ClassAssistant() {
    delete[] class_duration_guide;
}


void ClassAssistant::fill_data(ClassTypes class_type_value, int students, ClassTimeInDay class_time_in_day, Day day)
{

    this->class_time_in_day = class_time_in_day;
    this->day = day;
    this->students = students;

    switch (class_time_in_day)
    {
    case morning:
        time_type = "morning";
        break;
    case noon:
        time_type = "noon";
        break;
    case afternoon:
        time_type = "afternoon";
        break;
    default:
        break;
    }

    switch (day)
    {
    case Monday:
        day_type = "Monday";
        break;
    case Tuesday:
        day_type = "Tuesday";
        break;
    case Wednesday:
        day_type = "Wednesday";
        break;
    case Thursday:
        day_type = "Thursday";
        break;
    case Friday:
        day_type = "Friday";
        break;
    case Saturday:
        day_type = "Saturday";
        break;
    case Sunday:
        day_type = "Sunday";
        break;
    default:
        break;
    }

    switch (class_type_value) {
        case magistral:
            class_type = String("magistral");
            class_duration_guide = magistral_duration;
            max_time_in_a_row = convertHourMinToMillis(magistral_time_in_a_row);
            break;
        case interactive:
            class_type = String("interactive");
            class_duration_guide = interactive_duration;
            max_time_in_a_row = convertHourMinToMillis(interactive_time_in_a_row);
            break;
        case exercises:
            class_type = String("exercises");
            class_duration_guide = exercises_duration;
            max_time_in_a_row = convertHourMinToMillis(exercises_time_in_a_row);
            break;
        case laboratory:
            class_type = String("laboratory");
            class_duration_guide = laboratory_duration;
            max_time_in_a_row = convertHourMinToMillis(laboratory_time_in_a_row);
            break;
        default:
            break;
    }
    for(int i = 0; i < 3; i++){
        class_duration_guide_in_millis[i] = convertHourMinToMillis(class_duration_guide[i]);
    }
    extra_time_to_notify = convertHourMinToMillis(NOTIFICATION_EXTRA_BLOCK_TIME);
}


void ClassAssistant::start_class()
{
    last_check = millis();
    class_is_running = true;
    class_has_started = true;
}

void ClassAssistant::change_class_state()
{
    if (class_is_running){
        total_class_time += millis() - last_check;
        current_class_block_time += millis() - last_check;
        breaks += 1;
        class_is_running = false;
    }
    else
    {
        break_time += millis() - last_check;
        class_is_running = true;
    }
    last_check = millis();
}

void ClassAssistant::end_class()
{
    total_class_time += millis() - last_check;
    class_is_running = false;
    class_has_started = false;
}

void ClassAssistant::rate_class(int punctuation)
{
    class_punctuation = punctuation;
}

void ClassAssistant::change_class_block()
{
    current_class_block_time += millis() - last_check;
    class_duration[current_class_block] = convertMillisToHourMin(current_class_block_time);
    current_class_block = current_class_block != 2 ? current_class_block + 1 : 2;
}

/*
    Function to check if current time in a row is between defined range
    returns:
        - true: current block time is over defined period
        - false: current block time is in defined period
*/
bool ClassAssistant::check_current_time_in_a_row()
{
    return millis() - last_check > max_time_in_a_row;
}

/*
    Function to check if current block time is in range
    returns:
        - true: current block time is over defined period
        - false: current block time is in defined period
*/
bool ClassAssistant::check_current_block_time()
{
    return class_duration_guide_in_millis[current_class_block] + extra_time_to_notify < current_class_block_time;
}

String ClassAssistant::get_current_block()
{
    switch (current_class_block)
    {
    case 0:
        return String("theory");
        break;
    case 1:
        return String("practice");
        break;
    case 2:
        return String("exercises resolution");
        break;
    default:
        break;
    }
    return String("theory");
}

bool ClassAssistant::get_class_is_running()
{
    return class_is_running;
}

bool ClassAssistant::get_class_has_started()
{
    return class_has_started;
}

 void ClassAssistant::write_class_data()
 {
     DateTime now = DateTime(F(__DATE__), F(__TIME__));
     rtc.adjust(now);
     now = rtc.now();
     String nameFile = "class_data.csv";

     String data_txt = class_type + ","
             + String(students) + ","
             + time_type + ","
             + day_type+ ","
             + String(class_duration[0]) + ","
             + String(class_duration[1]) + ","
             + String(class_duration[2]) + ","
             + String(breaks) + ","
             + String(class_punctuation);

     now = rtc.now();
     String datetime = now.timestamp(DateTime::TIMESTAMP_FULL);
     String save_txt = datetime + data_txt;
    
     Serial.println("Saved Data: "+ String(save_txt));
     File myFile;
     myFile = SD.open(nameFile, FILE_APPEND);
     myFile.println(save_txt);
     myFile.close();

 }
