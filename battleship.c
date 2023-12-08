#include <Adafruit_NeoPixel.h>

#define GAME_BOARD_SIDE_SIZE 10
#define LED_PIN 6

const int GAME_BOARD_SIZE = GAME_BOARD_SIDE_SIZE * GAME_BOARD_SIDE_SIZE;

bool PLAYER_ARRAY[GAME_BOARD_SIZE] = {false};
bool OPPONENT_ARRAY[GAME_BOARD_SIZE] = {false};
bool * CURRENT_ARRAY; // Will point toward either PLAYER_ARRAY or OPPONENT_ARRAY

bool isPlayerTurn = true; // /!\ THE ONLY VARIABLE TO CHANGE BETWEEN THE TWO BOARDS

Adafruit_NeoPixel screen(GAME_BOARD_SIZE, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);

  screen.begin();
  screen.clear();
  screen.setBrightness(255);

  // Select the current array according to the value of 'isPlayerTurn' variable
  CURRENT_ARRAY = isPlayerTurn ? PLAYER_ARRAY : OPPONENT_ARRAY;
}

void loop()
{
  if(isPlayerTurn) // Player is playing
  {
    int x, y;

    do
    {
      x = waitInput();
      y = waitInput();
    } while(!playOn(x, y)); // Only accept valid plays
  }
  else // Opponent is playing
  {
    waitOpponentPlay();
  }

  displayCurrentArray();
  debugCurrentArray();
  changeArray();
}

void waitOpponentPlay()
{
  // delay for proof of concept
  delay(1000);
}

int waitInput()
{
  // random value for proof of concept
  delay(1000);
  return random(GAME_BOARD_SIDE_SIZE-1);
}

void displayCurrentArray()
{
  screen.clear();
  for(int i = 0 ; i < GAME_BOARD_SIDE_SIZE ; i++)
  {
    for(int j = 0 ; j < GAME_BOARD_SIDE_SIZE ; j++)
    {
      int coord = j + GAME_BOARD_SIDE_SIZE * i;

      // Light the LED only if the bool is true on this coordinates
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
  isPlayerTurn = !isPlayerTurn;

  // Select the good array
  if(isPlayerTurn) CURRENT_ARRAY = PLAYER_ARRAY;
  else CURRENT_ARRAY = OPPONENT_ARRAY;
}

bool playOn(int x, int y)
{
  // If the play is not valid
  if (OPPONENT_ARRAY[y + GAME_BOARD_SIDE_SIZE * x] == true) return false; 

  // Play on the provided coordinates
  OPPONENT_ARRAY[y + GAME_BOARD_SIDE_SIZE * x] = true;
  return true;
}
