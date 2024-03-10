#include "SmartInterface.h"


int font_size = 2;
int box_h = 20;
int box_gap_h = 5;

int corner_color = TFT_CYAN;

int box_w = 100;

int box_type[2] = {20, 80};
int box_type_content[2] = {180, 80};

int box_students[2] = {box_type[0], box_type[1] + box_h + box_gap_h};
int box_students_content[2] = {box_type_content[0], box_type_content[1] + box_h + box_gap_h};

int box_time[2] = {box_type[0], box_students[1] + box_h + box_gap_h};
int box_time_content[2] = {box_students_content[0], box_students_content[1] + box_h + box_gap_h};

int box_day[2] = {box_type[0], box_time[1] + box_h + box_gap_h};
int box_day_content[2] = {box_time_content[0], box_time_content[1] + box_h + box_gap_h};

int select_button_w = 100;
int select_button_h = 20;

int box_confirm[2] = {200, 200};
int box_cancel[2] = {100, 200};


int punctuation_box[2] = {60, 80};
int punctuation_box_w = 200;
int punctuation_box_h = 5;
int punctuation_gap = 3;
int color_scale[5] = {TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREENYELLOW, TFT_GREEN};



String possible_class_type[] = {"magistral", "interactive", "exercises", "laboratory"};
String possible_time_type[] = {"morning", "noon", "afternoon"};
String possible_day_type[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

void start_class_menu()
{
    SERIAL.println("Start class menu");  
    int selected_button = 0;

    int current_class_type = 0;
    int students = 0;
    int current_time_type = 0;
    int current_day_type = 0;

    bool confirmed = false;
    SERIAL.println("Starting loop");
    while (!confirmed)
    {
        if (digitalRead(WIO_5S_UP) == LOW) {
            delay(200);
            selected_button = increment_with_check(selected_button, -1, 0, 5);
            SERIAL.println("Button UP pressed");
            SERIAL.print("Current button state ");
            SERIAL.println(selected_button);
        }
        else if (digitalRead(WIO_5S_DOWN) == LOW) {
            delay(200);
            selected_button = increment_with_check(selected_button, 1, 0, 5);
            SERIAL.println("Button LOW pressed");
            SERIAL.print("Current button state ");
            SERIAL.println(selected_button);
        }
        else if (digitalRead(WIO_5S_LEFT) == LOW) {
            delay(200);
            switch (selected_button)
            {
                case 0:
                    current_class_type = increment_with_check(current_class_type, 1, 0, 3);
                    SERIAL.println("Button left pressed in class_type");
                    SERIAL.print("Current class type ");
                    SERIAL.println(selected_button);
                    break;
                case 1:
                    students = increment_with_check(students, 5, 0, 200);
                    SERIAL.println("Button left pressed in students");
                    SERIAL.print("Current students ");
                    SERIAL.println(selected_button);
                    break;
                case 2:
                    current_time_type = increment_with_check(current_time_type, 1, 0, 2);
                    SERIAL.println("Button left pressed in time type");
                    SERIAL.print("Current time type ");
                    SERIAL.println(current_time_type);
                    break;
                case 3:
                    current_day_type = increment_with_check(current_day_type, 1, 0, 6);
                    SERIAL.println("Button left pressed in day type");
                    SERIAL.print("Current day type ");
                    SERIAL.println(current_day_type);
                    break;
                default:
                    SERIAL.println("default break left in start class menu");
                    break;
            }
        }
        else if (digitalRead(WIO_5S_RIGHT) == LOW) {
            delay(200);
            switch (selected_button)
            {
                case 0:
                    current_class_type = increment_with_check(current_class_type, -1, 0, 3);
                    SERIAL.println("Button right pressed in class_type");
                    SERIAL.print("Current class type ");
                    SERIAL.println(selected_button);
                    break;
                case 1:
                    students = increment_with_check(students, -5, 0, 0);
                    SERIAL.println("Button right pressed in students");
                    SERIAL.print("Current students ");
                    SERIAL.println(selected_button);
                    break;
                case 2:
                    current_time_type = increment_with_check(current_time_type, -1, 0, 2);
                    SERIAL.println("Button right pressed in time type");
                    SERIAL.print("Current time type ");
                    SERIAL.println(current_time_type);
                    break;
                case 3:
                    current_day_type = increment_with_check(current_day_type, -1, 0, 6);
                    SERIAL.println("Button right pressed in day type");
                    SERIAL.print("Current day type ");
                    SERIAL.println(current_day_type);
                    break;
                default:
                    SERIAL.println("default break right in start class menu");
                    break;
            }
        }
        else if (digitalRead(WIO_5S_PRESS) == LOW) {
            delay(200);
            switch (selected_button)
            {
                case 4:
                    confirmed = true;
                    SERIAL.println("Press button in confirm box");
                    classAssistant.fill_data((ClassTypes)current_class_type, students, (ClassTimeInDay)current_time_type, (Day)current_day_type);
                    break;
                case 5:
                    confirmed = true;
                    SERIAL.println("Press button in cancel box");
                    break;
                default:
                    SERIAL.println("default break pressed in start class menu");
                    break;
            }
        }

        SERIAL.println("Starting menu display");
        
        spr.fillSprite(TFT_BLACK); //Fill background with white color
        spr.fillRect(0,0,320,50,TFT_DARKCYAN); //Rectangle fill with dark green 
        spr.setTextColor(TFT_WHITE); //Setting text color
        spr.setTextSize(3); //Setting text size 
        spr.drawString("Start class",37,15); //Drawing a text String 
        spr.setTextSize(font_size); //Setting text size 

        // Draw type box
        spr.drawRoundRect(box_type[0] , box_type[1], box_w, box_h , 10, TFT_WHITE);
        spr.drawString("type", box_type[0], box_type[1]);
        // Draw type box content
        spr.drawRoundRect(box_type_content[0] , box_type_content[1], box_w, box_h , 10, selected_button == 0 ? corner_color : TFT_WHITE);
        spr.drawString(possible_class_type[current_class_type], box_type_content[0], box_type_content[1]);

        // Draw students box
        spr.drawRoundRect(box_students[0] , box_students[1], box_w, box_h , 10, TFT_WHITE);
        spr.drawString("students", box_students[0], box_students[1]);
        // Draw students box content
        spr.drawRoundRect(box_students_content[0] , box_students_content[1], box_w, box_h , 10, selected_button == 1 ? corner_color : TFT_WHITE);
        spr.drawString(String(students), box_students_content[0], box_students_content[1]);

        // Draw time box
        spr.drawRoundRect(box_time[0] , box_time[1], box_w, box_h , 10, TFT_WHITE);
        spr.drawString("time", box_time[0], box_time[1]);
        // Draw time box content
        spr.drawRoundRect(box_time_content[0] , box_time_content[1], box_w, box_h , 10, selected_button == 2 ? corner_color : TFT_WHITE);
        spr.drawString(possible_time_type[current_time_type], box_time_content[0], box_time_content[1]);

        // Draw day box
        spr.drawRoundRect(box_day[0] , box_day[1], box_w, box_h , 10, TFT_WHITE);
        spr.drawString("day", box_day[0], box_day[1]);
        // Draw day box content
        spr.drawRoundRect(box_day_content[0] , box_day_content[1], box_w, box_h , 10, selected_button == 3 ? corner_color : TFT_WHITE);
        spr.drawString(possible_day_type[current_day_type], box_day_content[0], box_day_content[1]);

        //Draw confirm button
        spr.drawRoundRect(box_confirm[0] , box_confirm[1], select_button_w, select_button_h , 10, selected_button == 4 ? TFT_GREEN : TFT_WHITE);
        spr.drawString("confirm", box_confirm[0], box_confirm[1]);

        // Draw cancel button
        spr.drawRoundRect(box_cancel[0] , box_cancel[1], select_button_w, select_button_h , 10, selected_button == 5 ? TFT_RED : TFT_WHITE);
        spr.drawString("cancel", box_cancel[0], box_cancel[1]);
        spr.pushSprite(0,0);
        SERIAL.println("End start menu lcd display");
        delay(50);
    }
    SERIAL.println("End start menu loop");
}


bool change_block_menu(String block)
{
    int selected_button = 0;
    SERIAL.println("Starting change block menu loop");
    bool confirmed = false;
    bool return_value = false;
    while (!confirmed)
    {
        if (digitalRead(WIO_5S_LEFT) == LOW || digitalRead(WIO_5S_RIGHT) == LOW)
        {
            delay(200);
            selected_button = increment_with_check(selected_button, 1, 0, 1);
            SERIAL.println("Button left or right pressed in block menu");
            SERIAL.print("Current selected button ");
            SERIAL.println(selected_button);
        }
        else if (digitalRead(WIO_5S_PRESS) == LOW)
        {
            delay(200);
            SERIAL.println("Press button");
            confirmed = true;
            if (selected_button == 0)
            {
                SERIAL.println("Confirmed change block");
                return_value = true;
            }
            else
            {
                SERIAL.println("Canceled change block");
                return_value = false;
            }
        }

        SERIAL.println("Starting lcd change block information");
        spr.fillSprite(TFT_BLACK); //Fill background with white color
        spr.fillRect(0, 0, 320, 320, TFT_DARKGREY); //Rectangle fill with dark green 
        spr.setTextColor(TFT_WHITE); //Setting text color
        spr.setTextSize(2); //Setting text size 
        spr.drawString(classAssistant.get_current_block(),37,15);
        spr.drawString("block finished?", 37, 50);
        spr.setTextSize(font_size);


        //Draw confirm button
        spr.drawRoundRect(box_confirm[0] , box_confirm[1], select_button_w, select_button_h , 10, selected_button == 0 ? TFT_GREEN : TFT_WHITE);
        spr.drawString("confirm", box_confirm[0], box_confirm[1]);

        // Draw cancel button
        spr.drawRoundRect(box_cancel[0] , box_cancel[1], select_button_w, select_button_h , 10,  selected_button == 1? TFT_RED : TFT_WHITE);
        spr.drawString("cancel", box_cancel[0], box_cancel[1]);
        spr.pushSprite(0,0);
        SERIAL.println("Pushed lcd information");
        delay(50);
    }
    SERIAL.println("Finished change block menu");
    return return_value;
}

int end_class_menu()
{
    SERIAL.println("Started end class menu");
    bool confirmed = false;
    int rate = 5;
    int selected_button = 0;

    while(!confirmed)
    {
        if (selected_button == 0 && (WIO_5S_DOWN) == LOW)
        {
            delay(200);
            rate = increment_with_check(rate, -1, 1, 10);
            SERIAL.println("Button down pressed in end class menu");
            SERIAL.print("Current rate ");
            SERIAL.println(selected_button);
        }
        else if (selected_button == 0 && digitalRead(WIO_5S_UP) == LOW)
        {
            delay(200);
            rate = increment_with_check(rate, 1, 1, 10);
            SERIAL.println("Button up pressed in end class menu");
            SERIAL.print("Current rate ");
            SERIAL.println(selected_button);
        }
        else if (selected_button > 0 && (digitalRead(WIO_5S_LEFT) == LOW || digitalRead(WIO_5S_RIGHT) == LOW))
        {
            delay(200);
            selected_button = increment_with_check(selected_button, 1, 1, 2);
            SERIAL.println("Button left or right pressed in end class menu");
            SERIAL.print("Current selected button ");
            SERIAL.println(selected_button);
        }
        else if (digitalRead(WIO_5S_PRESS) == LOW)
        {
            delay(200);
            if (selected_button == 0)
            {
                selected_button = 1;
                SERIAL.println("Button pressed  going to confirm cancel buttons in end class menu");
                SERIAL.print("Current selected ");
                SERIAL.println(selected_button);
            }
            else if (selected_button == 1)
            {
                confirmed = true;
                SERIAL.println("Confirmed end class menu");
            }
            else
            {
                confirmed = true;
                rate = -1;
                SERIAL.println("Canceled end class menu");
            }

        }

        SERIAL.println("Start lcd showing for end class menu");

        spr.fillSprite(TFT_BLACK); //Fill background with white color
        spr.fillRect(0, 0, 320, 60, TFT_PURPLE); //Rectangle fill with dark green 
        spr.setTextColor(TFT_WHITE); //Setting text color
        spr.setTextSize(1); //Setting text size 
        spr.drawString("End of class.", 37, 15);
        spr.drawString("Rate the class between 0-10  ", 37, 40);
        spr.setTextSize(font_size); //Setting text size 


        for (int i=10; i > 0; i--)
        {
            SERIAL.print(i);
            spr.drawCircle(punctuation_box[0], punctuation_box[1] + (10 - i) * (2 *punctuation_box_h + punctuation_gap), punctuation_box_h, i <= rate ? color_scale[i/2] : TFT_WHITE);
            spr.fillCircle(punctuation_box[0], punctuation_box[1] + (10 - i) * (2 *punctuation_box_h + punctuation_gap), punctuation_box_h, i <= rate ? color_scale[i/2] : TFT_WHITE);
            // spr.drawRoundRect(punctuation_box[0] , punctuation_box[1] + i * punctuation_box_h, punctuation_box_w, punctuation_box_h , rate < i ? color_scale[i/2] : TFT_LIGHTGREY, TFT_WHITE);
            // spr.drawRoundRect(punctuation_box[0] , punctuation_box[1] + (10 - i) * (punctuation_box_h + punctuation_gap), punctuation_box_w, punctuation_box_h , 10, i <= rate ? color_scale[i/2] : TFT_WHITE);
        }
        SERIAL.println("Finish punctuation boxs");

        //Draw confirm button
        spr.drawRoundRect(box_confirm[0] , box_confirm[1], select_button_w, select_button_h , 10, selected_button == 1 ? TFT_GREEN : TFT_WHITE);
        spr.drawString("confirm", box_confirm[0], box_confirm[1]);

        // Draw cancel button
        spr.drawRoundRect(box_cancel[0] , box_cancel[1], select_button_w, select_button_h , 10, selected_button == 2 ? TFT_RED : TFT_WHITE);
        spr.drawString("cancel", box_cancel[0], box_cancel[1]);
        
        spr.pushSprite(0,0);
        SERIAL.println("Pushed lcd information");
        delay(50);
    }

    return rate + 1;
}


int increment_with_check(int n, int delta, int min, int max)
{
    n += delta;
    if (n < min)
        n = max;
    if (n > max)
        n = min;

    return n;
}
