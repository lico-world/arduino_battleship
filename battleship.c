#include <Adafruit_NeoPixel.h>

#define GAME_BOARD_SIDE_SIZE 3
#define LED_PIN 6

const int GAME_BOARD_SIZE = GAME_BOARD_SIDE_SIZE * GAME_BOARD_SIDE_SIZE;

bool PLAYER_ARRAY[GAME_BOARD_SIZE] = {true, true, true, false, false, false, true, true, true}; //{true, false, false, false, true, false, true, true, false, true, true, false, false, false, true, false, true, true, false, true, false, true, true, false, true};
bool OPPONENT_ARRAY[GAME_BOARD_SIZE] = {true, false, true, true, false, true, true, false, true}; //{true, false, false, false, true, false, true, true, false, true, false, false, false, true, true, false, true, false, true, true, false, false, false, true, false};;
bool * CURRENT_ARRAY;

bool isDisplaySubjectThePlayer = true;

Adafruit_NeoPixel screen(GAME_BOARD_SIZE, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);

  screen.begin();
  screen.clear();
  screen.setBrightness(10);

  CURRENT_ARRAY = isDisplaySubjectThePlayer ? PLAYER_ARRAY : OPPONENT_ARRAY;
}

void loop()
{
  // Verify the array reading navigation functionality
  debugCurrentArray();
  displayCurrentArray();
  
  changeArray();
  delay(1000);
  
  debugCurrentArray();
  displayCurrentArray();

  changeArray();
  delay(1000);
}

void displayCurrentArray()
{
  screen.clear();
  for(int i = 0 ; i < GAME_BOARD_SIDE_SIZE ; i++)
  {
    for(int j = 0 ; j < GAME_BOARD_SIDE_SIZE ; j++)
    {
      int coord = j + GAME_BOARD_SIDE_SIZE * i;
      if(CURRENT_ARRAY[coord]) screen.setPixelColor(coord, 255, 0, 0);
    }
  }
  screen.show();
}

void debugCurrentArray()
{
  for (int i = 0 ; i < GAME_BOARD_SIDE_SIZE ; i++)
  {
    for (int j = 0 ; j < GAME_BOARD_SIDE_SIZE ; j++)
    {
      Serial.print(CURRENT_ARRAY[j + GAME_BOARD_SIDE_SIZE * i] ? '#' : '~');
    }
    Serial.print("\n");
  }
  Serial.print("\n");
}

void changeArray()
{
  // Invert the flag that discriminate who between player and opponent should be displayed
  isDisplaySubjectThePlayer = !isDisplaySubjectThePlayer;

  // Select the good array
  if(isDisplaySubjectThePlayer) CURRENT_ARRAY = PLAYER_ARRAY;
  else CURRENT_ARRAY = OPPONENT_ARRAY;
}

bool playOn(int x, int y)
{
  if (OPPONENT_ARRAY[y + GAME_BOARD_SIDE_SIZE * x] == true) return false; 
  
  OPPONENT_ARRAY[y + GAME_BOARD_SIDE_SIZE * x] = true;
  return true;
}
