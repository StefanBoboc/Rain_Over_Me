/*
0123456789012345
   Welcome to
  RAIN OVER ME
0123456789012345
      MENU     
>Start game    +
>Highscores    +
>Settings      +
>About         +
>How to play   +
-----
0123456789012345
-  HIGHSCORES  
1 STEFAN 99999 +
2 Play2  99999 +
3 Play3  99999 +
4 Play4  99999 +
5 Play5  99999 +
-----
0123456789012345
-   SETTINGS
>Name: STEFAN  +
>Difficulty: 3 +
>LCD contrs: 9 +
>LCD bright: 5 +
>Mat bright: 5 +
>Sound: ON/OFF +
-----
0123456789012345
-    ABOUT
CATCH THE EGGS by Stefan Boboc, StefanBoboc on GitHub / 
-----
0123456789012345
- HOW TO PLAY
Descriere...
*/

#include <LiquidCrystal.h>
#include <LedControl.h>
#include <EEPROM.h>

#define RS 9
#define EN 8
#define D4 7
#define D5 6
#define D6 5
#define D7 4

#define JOYSTICK_X_PIN A0
#define JOYSTICK_Y_PIN A1
#define JOYSTICK_SW_PIN 2

#define JOYSTICK_MIN_TRESHOLD 400
#define JOYSTICK_MAX_TRESHOLD 600

#define JOYSTICK_STILL 0
#define JOYSTICK_UP 1
#define JOYSTICK_DOWN -1
#define JOYSTICK_RIGHT 1
#define JOYSTICK_LEFT -1
#define JOYSTICK_SW_ON 1
#define JOYSTICK_SW_OFF 0
#define JOYSTICK_FAKE_TRIGGER 2

#define DEBOUNCE_DELAY_SW 100
#define DEBOUNCE_DELAY_CURSOR 700
#define DEBOUNCE_DELAY_SHIFTING 700

#define MENU_STATE -1
#define MENU_TITLE "MENU"

#define START_GAME_STATE 0

#define HIGHSCORE_STATE 1
#define HIGHSCORE_TITLE "HIGHSCORES"

#define SETTINGS_STATE 2
#define SETTINGS_TITLE "SETTINGS"
#define NAME_STATE 0
#define DIFFICULTY_STATE 1
#define LCD_CONTRAST_STATE 2
#define LCD_BRIGHTNESS_STATE 3
#define MATRIX_BRIGHTNESS_STATE 4
#define SOUND_STATE 5

#define DIFFICULTY_LOW_BOUND 1
#define DIFFICULTY_HIGH_BOUND 3
#define LCD_CONTRAST_LOW_BOUND 1
#define LCD_CONTRAST_HIGH_BOUND 9
#define LCD_BRIGHTNESS_LOW_BOUND 1
#define LCD_BRIGHTNESS_HIGH_BOUND 9
#define MATRIX_BRIGHTNESS_LOW_BOUND 1
#define MATRIX_BRIGHTNESS_HIGH_BOUND 9
#define SOUND_LOW_BOUND 0
#define SOUND_HIGH_BOUND 1

#define ABOUT_STATE 3
#define ABOUT_TITLE "ABOUT"
#define ABOUT_TEXT "RAIN OVER ME by Stefan Boboc, StefanBoboc on GitHub / "
#define ABOUT_TEXT_LENGTH 53

#define HOW_TO_PLAY_STATE 4
#define HOW_TO_PLAY_TITLE "HOW TO PLAY"
#define HOW_TO_PLAY_TEXT "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book / "
#define HOW_TO_PLAY_TEXT_LENGTH 245

#define LCD_PIN_COUNT 16

byte INDICATOR_IMG[8] = {
  B10000,
  B11000,
  B11100,
  B11110,
  B11100,
  B11000,
  B10000,
  B00000
};

byte TOP_SCROLLBAR[8] = {
  0b11111,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};

byte MIDDEL_SCROLLBAR[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};

byte BOTTOM_SCROLLBAR[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b11111,
  0b11111
};

byte EDIT_MODE[8] = {
  B00100,
  B00100,
  B00100,
  B11100,
  B11100,
  B00100,
  B00100,
  B00100
};

byte O[8] = {
  B11111,
  B11111,
  B10001,
  B01110,
  B01110,
  B01110,
  B10001,
  B11111
};

byte N[8] = {
  B11111,
  B11111,
  B01001,
  B00110,
  B01110,
  B01110,
  B01110,
  B11111
};

byte F[8] = {
  B11001,
  B10110,
  B10111,
  B00011,
  B10111,
  B10111,
  B10111,
  B11111
};

/*------------------------*/

byte menuIcons[][8] = {

  { B00111000,
    B01111110,
    B11111111,
    B01111110,
    B00000000,
    B01001001,
    B10010010,
    B00000000 },
  { B00111100,
    B11111111,
    B10111101,
    B10111101,
    B01111110,
    B00111100,
    B00011000,
    B00111100 },
  { B00100100,
    B01100110,
    B01100110,
    B01111110,
    B00111100,
    B00011000,
    B00011000,
    B00011000 },
  { B00011000,
    B00011000,
    B00000000,
    B00111000,
    B00011000,
    B00011000,
    B00011000,
    B00111100 },
  { B00111100,
    B01111110,
    B01100110,
    B00001100,
    B00011000,
    B00011000,
    B00000000,
    B00011000 }
};

byte gameOverIcon[8] = {
  B00111100,
  B01111110,
  B10011001,
  B10011001,
  B01111110,
  B01111110,
  B01011010,
  B01011010
};

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

const byte dinPin = 12;
const byte clockPin = 10;
const byte loadPin = 13;
const byte matrixSize = 8;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  //DIN, CLK, LOAD, No. DRIVER
byte matrixBrightness = 2;

void setMatrixIcon(byte image[]) {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, image[i]);
  }
}

/*-----GAME INTRO-----*/
const String welcomeMsg[] = { "Welcome to", "RAIN OVER ME" };
void gameIntro() {
  lcd.clear();
  lcd.setCursor(3, 0);
  for (int i = 0; i < 10; i++) {
    lcd.print(welcomeMsg[0][i]);
    delay(300);
  }

  lcd.setCursor(2, 1);
  lcd.print(welcomeMsg[1]);

  delay(3000);
}

/*-----TAKE JOYSTICK INPUT-----*/
int8_t joystickX;
int8_t joystickY;
bool joystickSW;

short readJoystickX;
short readJoystickY;

void joystickReading() {
  readJoystickX = analogRead(JOYSTICK_X_PIN);
  readJoystickY = analogRead(JOYSTICK_Y_PIN);
  bool readJoystickSW = digitalRead(JOYSTICK_SW_PIN);



  if (JOYSTICK_MIN_TRESHOLD <= readJoystickX && readJoystickX <= JOYSTICK_MAX_TRESHOLD) {
    joystickX = JOYSTICK_STILL;
  } else if (readJoystickX < JOYSTICK_MIN_TRESHOLD) {
    joystickX = JOYSTICK_RIGHT;
  } else {
    joystickX = JOYSTICK_LEFT;
  }

  if (JOYSTICK_MIN_TRESHOLD <= readJoystickY && readJoystickY <= JOYSTICK_MAX_TRESHOLD) {
    joystickY = 0;
  } else if (readJoystickY < JOYSTICK_MIN_TRESHOLD) {
    joystickY = JOYSTICK_DOWN;
  } else {
    joystickY = JOYSTICK_UP;
  }

  joystickSW = !readJoystickSW;
}

bool joystickMovedX = false;
int8_t joystickInputX() {
  if (joystickX == JOYSTICK_STILL) {
    joystickMovedX = false;
    return joystickX;
  }

  if (!joystickMovedX) {
    joystickMovedX = true;
    return joystickX;
  }

  return JOYSTICK_STILL;
}

bool joystickMovedY = false;
int8_t joystickInputY() {
  if (joystickY == JOYSTICK_STILL) {
    joystickMovedY = false;
    return joystickY;
  }

  if (!joystickMovedY) {
    joystickMovedY = true;
    return joystickY;
  }

  return JOYSTICK_STILL;
}

bool lastJoystickSW = LOW;
bool currentJoystickSW = LOW;
unsigned long lastDebounceTimeSW = 0;
bool swInput() {
  if (joystickSW == HIGH and lastJoystickSW == LOW) {
    lastDebounceTimeSW = millis();
  }

  if (millis() - lastDebounceTimeSW > DEBOUNCE_DELAY_SW) {
    if (joystickSW != currentJoystickSW) {
      currentJoystickSW = joystickSW;

      if (currentJoystickSW == HIGH) {
        return 1;
      }
    }
  }

  lastJoystickSW = joystickSW;
  return 0;
}

int8_t state = -1;
int8_t inputAxisX;
int8_t inputAxisY;
bool inputSW;

bool editMode = false;
void displayMenuScrollbar(int itemsCount, int item) {
  lcd.setCursor(15, 1);

  if (editMode == true) {
    lcd.write(byte(0));
  } else if (item == 0) {
    lcd.write(byte(2));
  } else if (item == itemsCount - 1) {
    lcd.write(byte(4));
  } else {
    lcd.write(byte(3));
  }
}

void displayMenuSetup(String title, bool backArrow, bool indicator) {
  lcd.clear();

  if (backArrow == true) {
    lcd.setCursor(0, 0);
    lcd.write((char)127);
  }

  int pos = (16 - title.length()) / 2;
  lcd.setCursor(pos, 0);
  lcd.print(title);

  if (indicator == true) {
    lcd.setCursor(0, 1);
    lcd.write(byte(1));
  }
}

struct settingsStruct {
  char name[8];
  short difficulty;
  short lcdContrs;
  short lcdBright;
  short matrBright;
  short sound;
} settings;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  pinMode(JOYSTICK_X_PIN, INPUT);
  pinMode(JOYSTICK_Y_PIN, INPUT);
  pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);

  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                    // clear screen

  lcd.createChar(1, INDICATOR_IMG);
  lcd.createChar(2, TOP_SCROLLBAR);
  lcd.createChar(3, MIDDEL_SCROLLBAR);
  lcd.createChar(4, BOTTOM_SCROLLBAR);
  lcd.createChar(5, O);
  lcd.createChar(6, N);
  lcd.createChar(7, F);
  lcd.createChar(0, EDIT_MODE);

  pinMode(3, OUTPUT);
  //pinMode(11, OUTPUT);

  analogWrite(3, 200);
  //analogWrite(11, 49);

  EEPROM.get(0, settings);

  //gameIntro();
}

/*-----MENU-----*/
const int menuItemsCount = 5;
const String menuItems[menuItemsCount] = { "Start Game", "Highscores", "Settings", "About", "How to play" };
int8_t menuItemPosition = 0;
int8_t lastMatrixIcon = -1;


void menuUpdate() {
  displayMenuSetup(MENU_TITLE, false, true);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuItemPosition]);
  displayMenuScrollbar(menuItemsCount, menuItemPosition);
}
void matrixUpdate() {
  lc.shutdown(0, false);
  lc.clearDisplay(0);
  setMatrixIcon(menuIcons[menuItemPosition]);
}

bool needToUpdate = true;
void mainMenuManager() {
  if (needToUpdate == true) {
    needToUpdate = false;
    menuUpdate();
    if (lastMatrixIcon != menuItemPosition) {
      lastMatrixIcon = menuItemPosition;
      matrixUpdate();
    }
  }

  if (inputAxisY == JOYSTICK_UP and 0 < menuItemPosition) {
    menuItemPosition -= 1;
    needToUpdate = true;
  }

  if (inputAxisY == JOYSTICK_DOWN and menuItemPosition < menuItemsCount - 1) {
    menuItemPosition += 1;
    needToUpdate = true;
  }

  if (inputSW == JOYSTICK_SW_ON) {
    state = menuItemPosition;
    needToUpdate = true;
  }
}

/*-----HIGHSCORES-----*/
const int8_t highscoreItemsCount = 5;
int8_t highscoreItemPosition = 0;

struct highscoreStruct {
  char name[7];
  int score;
} highscorePlayers[highscoreItemsCount] = { { "GEORGE", 12345 }, { "ANDREI", 1234 }, { "TEODOR", 123 }, { "IOANAA", 12 }, { "CORINA", 1 } };

void highscoreUpdate() {
  char buffer[5];

  displayMenuSetup(HIGHSCORE_TITLE, true, false);

  lcd.setCursor(0, 1);
  lcd.print(highscoreItemPosition + 1);

  lcd.setCursor(2, 1);
  lcd.print(highscorePlayers[highscoreItemPosition].name);

  lcd.setCursor(9, 1);
  sprintf(buffer, "%05d", highscorePlayers[highscoreItemPosition].score);
  lcd.print(buffer);

  displayMenuScrollbar(highscoreItemsCount, highscoreItemPosition);
}

void highscoresManager() {
  if (needToUpdate == true) {
    needToUpdate = false;
    highscoreUpdate();
  }

  if (inputAxisY == JOYSTICK_UP and 0 < highscoreItemPosition) {
    highscoreItemPosition -= 1;
    needToUpdate = true;
  }

  if (inputAxisY == JOYSTICK_DOWN and highscoreItemPosition < highscoreItemsCount - 1) {
    highscoreItemPosition += 1;
    needToUpdate = true;
  }

  if (inputAxisX == JOYSTICK_LEFT) {
    highscoreItemPosition = 0;
    state = MENU_STATE;
    needToUpdate = true;
  }
}

/*-----SETTINGS-----*/
/*
0123456789012345
    SETTINGS
>Name: STEFAN  +
>Difficulty: 3 +
>LCD contrs: 9 +
>LCD bright: 5 +
>Mat bright: 5 +
>Sound: ON/OFF +
*/
const int8_t settingsItemsCount = 6;
const String settingsItems[settingsItemsCount] = { "Name: ", "Difficulty: ", "LCD contrs: ", "LCD bright: ", "Mat bright: ", "Sound: " };
int8_t settingsItemPosition = 0;

void saveSettings() {
  EEPROM.put(0, settings);
}

int8_t letterIndex = 0;
void selectLetterIndex() {
  if (inputAxisX == JOYSTICK_RIGHT and letterIndex < 5) {
    letterIndex += 1;
  }

  if (inputAxisX == JOYSTICK_LEFT and 0 < letterIndex) {
    letterIndex -= 1;
  }
}

void changeLetter() {
  if (inputAxisY == 1) {
    settings.name[letterIndex] += 1;
  }

  if (inputAxisY == -1) {
    settings.name[letterIndex] -= 1;
  }

  if (settings.name[letterIndex] == 'Z' + 1) {
    settings.name[letterIndex] = 'A';
  } else if (settings.name[letterIndex] == 'A' - 1) {
    settings.name[letterIndex] = 'Z';
  }
}

unsigned int startMillisCursor;
bool blink = false;
void updateName() {
  selectLetterIndex();

  lcd.setCursor(letterIndex + 7, 1);

  unsigned int currentMillis = millis();
  if (currentMillis - startMillisCursor >= DEBOUNCE_DELAY_CURSOR)  //test whether the period has elapsed
  {
    blink = !blink;
    startMillisCursor = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }

  if (blink == true) {
    lcd.cursor();
  } else {
    lcd.noCursor();
  }

  changeLetter();
}

void updateSettingsValues(short& value, int lowThreshold, int highThreshold) {
  if (inputAxisY == 1 and value < highThreshold) {
    value = value + 1;
  }
  if (inputAxisY == -1 and lowThreshold < value) {
    value = value - 1;
  }
}


void settingsUpdate() {
  String settingMessage = "";
  settingMessage.concat(settingsItems[settingsItemPosition]);

  lcd.setCursor(1, 1);

  switch (settingsItemPosition) {
    case NAME_STATE:
      settingMessage.concat(settings.name);
      break;
    case DIFFICULTY_STATE:
      settingMessage.concat(settings.difficulty);
      break;
    case LCD_CONTRAST_STATE:
      settingMessage.concat(settings.lcdContrs);
      break;
    case LCD_BRIGHTNESS_STATE:
      settingMessage.concat(settings.lcdBright);
      break;
    case MATRIX_BRIGHTNESS_STATE:
      settingMessage.concat(settings.matrBright);
      break;
    case SOUND_STATE:
      lcd.print(settingMessage);

      if (settings.sound == 1) {
        lcd.write(byte(5));
        lcd.write(byte(6));
        lcd.print("/");
        lcd.print("off");
      } else {
        lcd.print("on");
        lcd.print("/");
        lcd.write(byte(5));
        lcd.write(byte(7));
        lcd.write(byte(7));
      }

      displayMenuScrollbar(settingsItemsCount, settingsItemPosition);
      return;
  }

  lcd.print(settingMessage);
  displayMenuScrollbar(settingsItemsCount, settingsItemPosition);
}

void settingsManager() {
  if (needToUpdate == true) {
    needToUpdate = false;
    displayMenuSetup(SETTINGS_TITLE, true, true);
    inputAxisY = JOYSTICK_FAKE_TRIGGER;
  }

  if (inputSW == JOYSTICK_SW_ON) {
    editMode = !editMode;
  }

  if (editMode == false) {
    if (inputAxisY == JOYSTICK_UP and 0 < settingsItemPosition) {
      settingsItemPosition -= 1;
    }

    if (inputAxisY == JOYSTICK_DOWN and settingsItemPosition < settingsItemsCount - 1) {
      settingsItemPosition += 1;
    }

    if (inputAxisX == JOYSTICK_LEFT) {
      settingsItemPosition = 0;
      state = MENU_STATE;
      needToUpdate = true;
      saveSettings();
    }
  }

  if (editMode == true) {
    switch (settingsItemPosition) {
      case NAME_STATE:
        updateName();
        break;

      case DIFFICULTY_STATE:
        updateSettingsValues(settings.difficulty, DIFFICULTY_LOW_BOUND, DIFFICULTY_HIGH_BOUND);
        break;

      case LCD_CONTRAST_STATE:
        updateSettingsValues(settings.lcdContrs, LCD_CONTRAST_LOW_BOUND, LCD_CONTRAST_HIGH_BOUND);
        break;

      case LCD_BRIGHTNESS_STATE:
        updateSettingsValues(settings.lcdBright, LCD_BRIGHTNESS_LOW_BOUND, LCD_BRIGHTNESS_HIGH_BOUND);
        break;

      case MATRIX_BRIGHTNESS_STATE:
        updateSettingsValues(settings.matrBright, MATRIX_BRIGHTNESS_LOW_BOUND, MATRIX_BRIGHTNESS_HIGH_BOUND);
        break;

      case SOUND_STATE:
        updateSettingsValues(settings.sound, SOUND_LOW_BOUND, SOUND_HIGH_BOUND);
        break;
    }
  }

  if (inputAxisY != JOYSTICK_STILL or inputSW != JOYSTICK_SW_OFF) {
    settingsUpdate();
  }
}

/*-----ABOUT and HOW TO PLAY-----*/
short lastMessageCharacter = 0;
const char pedding[] = "                ";
char textPiece[] = "                ";
unsigned int startMillisShifting;
void displayMessage(char text[], int textLen) {
  unsigned int currentMillis = millis();
  if (currentMillis - startMillisShifting >= DEBOUNCE_DELAY_SHIFTING)  //test whether the period has elapsed
  {
    for (int i = 0; i < LCD_PIN_COUNT - 1; i++) {
      textPiece[i] = textPiece[i + 1];
    }

    textPiece[LCD_PIN_COUNT - 1] = text[lastMessageCharacter];
    lastMessageCharacter++;
    if (lastMessageCharacter > textLen) {
      lastMessageCharacter = 0;
    }

    lcd.setCursor(0, 1);
    lcd.print(textPiece);

    startMillisShifting = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
}

void aboutHowToPlayManager(String menuTitle, char wholeText[], int textLen, bool backArrow, bool indicator) {
  if (needToUpdate == true) {
    needToUpdate = false;
    displayMenuSetup(menuTitle, backArrow, indicator);
  }

  displayMessage(wholeText, textLen);

  if (inputAxisX == JOYSTICK_LEFT) {
    state = MENU_STATE;
    needToUpdate = true;
    lastMessageCharacter = 0;
    strcpy(textPiece, pedding);
  }
}

/*-----START_GAME-----*/
/*
0123456789012345
Drops:xx E:+1|-1
Combo:xx T:xx:xx
99000
 1000
-----
0123456789012345
Congratulations!
U survived 01:23
---
0123456789012345
 NO HIGHSCORE..
Tm:01.23 Rain:xx
---
0123456789012345
 ! HIGHSCORE !
Tm:01.23 Rain:xx 
*/

int8_t gameState;
int8_t gameOverState;
int8_t checkBonusState;
int8_t checkPowerupStatus;

struct entityPosition {
  int8_t column;
  int8_t row;
} player, raindrop;

int8_t raindropCounter;
int8_t raindropCombo;
int8_t raindropsMax;

unsigned long gameStartTime;
unsigned long previousRaindropMillis;
unsigned long previousPlayerMillis;
unsigned long previousBonusMillis;
unsigned long gameOverTimer;
unsigned long minutesPassedBonusSignal;
unsigned long minutesPassedPowerup;

struct difficultyLevel {
  char name;
  int8_t raindropCaught;
  int8_t raindropMissed;
  unsigned playerSpeed;
  unsigned raindropSpeed;
  // TBA
} easyDifficulty = { .name = 'E', .raindropCaught = +1, .raindropMissed = -1, .playerSpeed = 100, .raindropSpeed = 100 },
  mediumDifficulty = { .name = 'M', .raindropCaught = +1, .raindropMissed = -1, .playerSpeed = 50, .raindropSpeed = 100 },
  hardDifficulty = { .name = 'H', .raindropCaught = +1, .raindropMissed = -2, .playerSpeed = 50, .raindropSpeed = 50 };

difficultyLevel currentDifficulty;

int8_t minutesScore;
int8_t secondsScore;

int8_t minutesPassFosBonus; 

bool blinkBonus;
bool powerupEnabled;


void printLPaddZero(int8_t valueToBePadd, int8_t cursorColPos, int8_t cursorRowPos) {
  lcd.setCursor(cursorColPos, cursorRowPos);

  if (valueToBePadd < 10) {
    lcd.print('0');
  }

  lcd.print(valueToBePadd);
}

void formatTimeScore(unsigned long timeScoreMillis, int8_t* addMinutes, int8_t* addSeconds) {
  unsigned long seconds = timeScoreMillis / 1000;
  unsigned long minutes = seconds / 60;
  seconds %= 60;
  minutes %= 60;

  *addMinutes = int8_t(minutes);
  *addSeconds = int8_t(seconds);
}

void playerMovement() {
  if (powerupEnabled == false){
    lc.setLed(0, player.row, player.column, true);
  }
  else{
    lc.setLed(0, player.row, player.column, true);
    lc.setLed(0, player.row, player.column+1, true);
  }

  unsigned int currentMillis = millis();

  if (joystickX != 0 and currentMillis - previousPlayerMillis >= currentDifficulty.playerSpeed)  //test whether the period has elapsed
  {

    if (powerupEnabled == false){
      if ((joystickX == -1 and 0 < player.column) or (joystickX == 1 and player.column < 7)) {
        lc.setLed(0, player.row, player.column, false);
        player.column += joystickX;
        lc.setLed(0, player.row, player.column, true);
      }
    }
    else {
      if ((joystickX == -1 and 0 < player.column) or (joystickX == 1 and player.column < 6)) {
        lc.setLed(0, player.row, player.column, false);
        lc.setLed(0, player.row, player.column + 1, false);
        player.column += joystickX;
        lc.setLed(0, player.row, player.column, true);
        lc.setLed(0, player.row, player.column+1, true);
      }
    }

    previousPlayerMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
}

void raindropMovement() {
  unsigned int currentMillis = millis();

  if (currentMillis - previousRaindropMillis >= currentDifficulty.raindropSpeed)  //test whether the period has elapsed
  {
    lc.setLed(0, raindrop.row, raindrop.column, false);
    raindrop.row++;
    lc.setLed(0, raindrop.row, raindrop.column, true);

    if (powerupEnabled == false){
      if (player.column == raindrop.column and player.row == raindrop.row) {
        raindropCounter += currentDifficulty.raindropCaught;
        raindropCombo += 1;
      } else if (player.column != raindrop.column and 7 == raindrop.row) {
        raindropCounter += currentDifficulty.raindropMissed;
        raindropCombo = 0;
      }
    }
    else{
      if ((player.column == raindrop.column and player.row == raindrop.row) or (player.column + 1 == raindrop.column and player.row == raindrop.row)) {
        raindropCounter += currentDifficulty.raindropCaught;
      } else if ((player.column != raindrop.column and 7 == raindrop.row) or (player.column + 1 != raindrop.column and 7 == raindrop.row)) {
        raindropCounter += currentDifficulty.raindropMissed;
      }
    }

    if (raindrop.row == 8) {
      raindrop.row = -1;
      raindrop.column = random(0, 8);
    }

    previousRaindropMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
}

void gameOverScreen(String text1, String text2) {
  lcd.clear();

  int pos;

  pos = (16 - text1.length()) / 2;
  lcd.setCursor(pos, 0);
  lcd.print(text1);

  pos = (16 - text2.length()) / 2;
  lcd.setCursor(pos, 1);
  lcd.print(text2);
}

void displayGameOverCongrats() {
  lcd.setCursor(0, 0);
  lcd.print("Good job ");
  lcd.print(settings.name);
  lcd.print("!");

  lcd.setCursor(0, 1);
  lcd.print("U're stats are..");
}

void displayGameOverHighscore() {
  lcd.setCursor(0, 0);
  lcd.print("HIGHSCORE! ");

  printLPaddZero(minutesScore, 11, 0);
  lcd.print(":");
  printLPaddZero(secondsScore, 14, 0);
}

void displayGameOverNoHighscore() {
  lcd.setCursor(0, 0);
  lcd.print("Time: ");

  printLPaddZero(minutesScore, 6, 0);
  lcd.print(":");
  printLPaddZero(secondsScore, 9, 0);
}

void gameOver() {
  switch (gameOverState) {
    case 0:
      lc.clearDisplay(0);

      setMatrixIcon(gameOverIcon);

      displayGameOverCongrats();

      gameOverTimer = millis();
      gameOverState = 1;

      break;

    case 1:
      if (millis() - gameOverTimer >= 5000) {
        lc.clearDisplay(0);

        // TBA - check if it is highscore

        bool isHighscore = true;
        if (isHighscore == true) {
          displayGameOverHighscore();
        } else {
          displayGameOverNoHighscore();
        }

        lcd.setCursor(0, 1);
        lcd.print("Max raindrops:");
        lcd.print(raindropsMax);

        gameOverState = 0;
        gameState = 3;
      }

      break;
  }
}

void setupGameParameters() {
  player.column = 3;
  player.row = 7;

  raindrop.column = random(0, 8);
  raindrop.row = -1;

  raindropCounter = 10;
  raindropCombo = 0;
  raindropsMax = 0;

  gameStartTime = millis();
  previousPlayerMillis = 0;
  previousRaindropMillis = 0;

  switch (settings.difficulty) {
    case 1:
      currentDifficulty = easyDifficulty;
      break;

    case 2:
      currentDifficulty = mediumDifficulty;
      break;

    case 3:
      currentDifficulty = hardDifficulty;
      break;
  }

  minutesPassFosBonus = 1;

  blinkBonus = true;
  powerupEnabled = false;

  checkBonusState = 0;
  checkPowerupStatus = 0;
}

void displayGameTextStats() {
  lcd.setCursor(0, 0);
  lcd.print("Drops.");

  lcd.setCursor(10, 0);
  lcd.print(".+ /");

  lcd.setCursor(0, 1);
  lcd.print("Combo.");

  lcd.setCursor(9, 1);
  lcd.print("T.  :");
}


void displayGamePlayerStats() {
  printLPaddZero(raindropCounter, 6, 0);

  lcd.setCursor(9, 0);
  lcd.print(currentDifficulty.name);

  lcd.setCursor(12, 0);
  lcd.print(currentDifficulty.raindropCaught);

  lcd.setCursor(14, 0);
  lcd.print(currentDifficulty.raindropMissed);

  printLPaddZero(raindropCombo, 6, 1);

  formatTimeScore(millis() - gameStartTime, &minutesScore, &secondsScore);
  printLPaddZero(minutesScore, 11, 1);
  printLPaddZero(secondsScore, 14, 1);
}

void getRaindropsMax() {
  if (raindropCounter > raindropsMax) {
    raindropsMax = raindropCounter;
  }
}

void changeDifficulty(){
  if ((currentDifficulty.name == 'E') and (minutesScore > 1 or raindropsMax >= 30)){
    currentDifficulty = mediumDifficulty;
  }

  if ((currentDifficulty.name == 'M') and (minutesScore > 2 or raindropsMax >= 45)){
    currentDifficulty = hardDifficulty;
  }
}

void checkBonus(){
  switch (checkBonusState){
    case 0:
      if (minutesScore >= minutesPassFosBonus){
        minutesPassFosBonus += 1;
        raindropCounter += 3;

        minutesPassedBonusSignal = millis();
        checkBonusState = 1;
      }
      break;

    case 1:      
        unsigned long currentMillis = millis();
        if (currentMillis - previousBonusMillis >= 500){
          
          if (blinkBonus == true){
            lcd.setCursor(8,0);
            lcd.print("+");
            blinkBonus = false;
          }
          else {
            lcd.setCursor(8,0);
            lcd.print(" ");
            blinkBonus = true;
          }
          previousBonusMillis = currentMillis;
        }

        if ( millis() - minutesPassedBonusSignal >= 3000 ){
          previousBonusMillis = 0;
          checkBonusState = 0;
        }

       break; 
  }
}

void checkPowerup(){
  switch(checkPowerupStatus){
    case 0:
      if (raindropCombo >= 5){
        lcd.setCursor(8, 1);
        lcd.print("*");

        checkPowerupStatus = 1;
      }
      break;
    
    case 1:
      if (inputSW == JOYSTICK_SW_ON) {
        powerupEnabled = true;

        minutesPassedPowerup = millis();
        checkPowerupStatus = 2;
      }
      break;

    case 2:
      if(millis() - minutesPassedPowerup >= 10000){
        powerupEnabled = false;

        lcd.setCursor(8, 1);
        lcd.print(" ");

        // we make sure that the second lit led is 100% off when the powerup stops
        lc.setLed(0, player.row, player.column + 1, false);

        raindropCombo = 0;
        checkPowerupStatus = 0;
      }
      break;
  }
}

void startGameManager() {
  switch (gameState) {
    case 0:
      lc.clearDisplay(0);
      lcd.clear();

      setupGameParameters();
      displayGameTextStats();
      displayGamePlayerStats();

      gameState = 1;
      break;

    case 1:
      playerMovement();
      raindropMovement();
      displayGamePlayerStats();
      getRaindropsMax();
      changeDifficulty();
      checkBonus();
      checkPowerup();

      if (raindropCounter <= 0) {
        gameState = 2;
      }
      break;

    case 2:
      gameOver();
      break;

    case 3:
      if (inputSW == JOYSTICK_SW_ON) {
        lastMatrixIcon = -1;
        gameState = 0;
        state = MENU_STATE;
      }
      break;
  }
}

void loop() {
  joystickReading();

  inputAxisX = joystickInputX();
  inputAxisY = joystickInputY();
  inputSW = swInput();

  switch (state) {
    case MENU_STATE:
      mainMenuManager();
      break;

    case START_GAME_STATE:
      startGameManager();
      break;

    case HIGHSCORE_STATE:
      highscoresManager();
      break;

    case SETTINGS_STATE:
      settingsManager();
      break;

    case ABOUT_STATE:
      aboutHowToPlayManager(ABOUT_TITLE, ABOUT_TEXT, ABOUT_TEXT_LENGTH, true, false);
      break;

    case HOW_TO_PLAY_STATE:
      aboutHowToPlayManager(HOW_TO_PLAY_TITLE, HOW_TO_PLAY_TEXT, HOW_TO_PLAY_TEXT_LENGTH, true, false);
      break;
  }
}