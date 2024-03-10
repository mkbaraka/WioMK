#include "SmartInterface.h"

NotificationAlert::NotificationAlert(int show_time, int delay_time, String text)
{
    this->show_time = show_time * 1000;
    this-> delay_time = delay_time * 1000;
    this->text = text;
}

void NotificationAlert::activate()
{
    // Activate notification if is not activated and delay time is passed if it is not first notification
    if (!activated)
    {
        if (last_notification == 0 || (last_notification > 0 && millis() - last_notification > delay_time))
        activated = true;
        activation_time = millis();
        last_notification = millis();
    }
}

void NotificationAlert::deactivate()
{
    activated = false;
}

void NotificationAlert::check_notification_time()
{
    if (activated && millis() - activation_time > show_time)
        deactivate();
}

bool NotificationAlert::get_activation()
{
    return activated;
}

String NotificationAlert::get_text()
{
    return text;
}
