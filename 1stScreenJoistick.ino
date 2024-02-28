#include "TFT_eSPI.h" // Include the graphics library (needs to be installed)

TFT_eSPI tft = TFT_eSPI();  // Create a display object

enum Screen {
  CLASS_TYPE,
  STUDENT_NUMBER,
  CLASS_TIME,
  DAY_OF_WEEK,
  SCREEN_COUNT // Helper to keep track of the total number of screens
};

Screen currentScreen = CLASS_TYPE;
Screen previousScreen = currentScreen;

const char* classTypes[] = {"Lecture", "Interactive", "Exercises", "Lab", "Master Class"};
int classTypeIndex = 0;
int previousClassTypeIndex = classTypeIndex;
const int numClassTypes = sizeof(classTypes) / sizeof(classTypes[0]);

const char* studentNumbers[] = {"10", "20", "30", "40", "50"};
int studentNumberIndex = 0;
int previousStudentNumberIndex = studentNumberIndex;
const int numStudentNumbers = sizeof(studentNumbers) / sizeof(studentNumbers[0]);

const char* classTimes[] = {"Morning", "Noon", "Afternoon", "Evening"};
int classTimeIndex = 0;
int previousClassTimeIndex = classTimeIndex;
const int numClassTimes = sizeof(classTimes) / sizeof(classTimes[0]);

const char* weekDays[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
int weekDayIndex = 0;
int previousWeekDayIndex = weekDayIndex;
const int numWeekDays = sizeof(weekDays) / sizeof(weekDays[0]);

void setup() {
  tft.begin();
  tft.setRotation(3); // Adjust rotation according to your display setup
  tft.fillScreen(TFT_BLACK);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
}

void loop() {
  static unsigned long lastPress = 0;
  if (millis() - lastPress > 200) { // Basic debounce
    if (digitalRead(WIO_5S_RIGHT) == LOW) {
      currentScreen = static_cast<Screen>((currentScreen + 1) % SCREEN_COUNT);
      lastPress = millis();
    } else if (digitalRead(WIO_5S_LEFT) == LOW) {
      currentScreen = currentScreen == CLASS_TYPE ? DAY_OF_WEEK : static_cast<Screen>(currentScreen - 1);
      lastPress = millis();
    } else if (digitalRead(WIO_5S_UP) == LOW) {
      incrementIndex(-1);
      lastPress = millis();
    } else if (digitalRead(WIO_5S_DOWN) == LOW) {
      incrementIndex(1);
      lastPress = millis();
    }
  }

  if (currentScreen != previousScreen ||
      classTypeIndex != previousClassTypeIndex ||
      studentNumberIndex != previousStudentNumberIndex ||
      classTimeIndex != previousClassTimeIndex ||
      weekDayIndex != previousWeekDayIndex) {
    updateScreen();

    previousScreen = currentScreen;
    previousClassTypeIndex = classTypeIndex;
    previousStudentNumberIndex = studentNumberIndex;
    previousClassTimeIndex = classTimeIndex;
    previousWeekDayIndex = weekDayIndex;
  }
}

void incrementIndex(int delta) {
  switch (currentScreen) {
    case CLASS_TYPE:
      classTypeIndex = (classTypeIndex + delta + numClassTypes) % numClassTypes;
      break;
    case STUDENT_NUMBER:
      studentNumberIndex = (studentNumberIndex + delta + numStudentNumbers) % numStudentNumbers;
      break;
    case CLASS_TIME:
      classTimeIndex = (classTimeIndex + delta + numClassTimes) % numClassTimes;
      break;
    case DAY_OF_WEEK:
      weekDayIndex = (weekDayIndex + delta + numWeekDays) % numWeekDays;
      break;
  }
}

void updateScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);

  switch (currentScreen) {
    case CLASS_TYPE:
      tft.setTextColor(TFT_YELLOW);
      tft.drawCentreString("Class Type:", tft.width()/2, 15, 2);
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString(classTypes[classTypeIndex], tft.width()/2, 70, 2);
      break;
    case STUDENT_NUMBER:
      tft.setTextColor(TFT_YELLOW);
      tft.drawCentreString("Students:", tft.width()/2, 15, 2);
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString(studentNumbers[studentNumberIndex], tft.width()/2, 70, 2);
      break;
    case CLASS_TIME:
      tft.setTextColor(TFT_YELLOW);
      tft.drawCentreString("Time:", tft.width()/2, 15, 2);
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString(classTimes[classTimeIndex], tft.width()/2, 70, 2);
      break;
    case DAY_OF_WEEK:
      tft.setTextColor(TFT_YELLOW);
      tft.drawCentreString("Day:", tft.width()/2, 15, 2);
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString(weekDays[weekDayIndex], tft.width()/2, 70, 2);
      break;
  }
}
