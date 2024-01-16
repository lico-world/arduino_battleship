#include <Adafruit_NeoPixel.h>
#include <Wire.h>

#define GAME_BOARD_SIDE_SIZE 3
#define LED_PIN 6
#define BUTTON_PIN 7

// Size of Game Boards arrays
const int GAME_BOARD_SIZE = GAME_BOARD_SIDE_SIZE * GAME_BOARD_SIDE_SIZE;
int verticalLine;

// Game Boards arrays
bool PLAYER_ARRAY[GAME_BOARD_SIZE] = {false};
bool BOAT_PLAYER_ARRAY[GAME_BOARD_SIZE] = {false};
bool OPPONENT_ARRAY[GAME_BOARD_SIZE] = {false};
bool * CURRENT_ARRAY; // Will point toward either PLAYER_ARRAY or OPPONENT_ARRAY

// /!\ THE ONLY VARIABLE TO CHANGE BETWEEN THE TWO BOARDS
bool isPlayerTurn = false;

bool dataReceived = false, hit = false;
int data[2];

int playerBoatCellAmount = 2;
int opponentBoatCellAmount = 2; // 5 + 2*4 + 2

Adafruit_NeoPixel screen(GAME_BOARD_SIZE, LED_PIN, NEO_GRB + NEO_KHZ800);

enum ORIENTATION {HORIZONTAL, VERTICAL};

void setup()
{
  Serial.begin(9600);

  pinMode(6, OUTPUT);
  pinMode(7, INPUT);

  screen.begin();
  screen.clear();
  screen.setBrightness(255);

  Wire.begin(10);
  Wire.onReceive(receiveEvent);
  //Wire.requestFrom(9, 2 * sizeof(int));

  // Select the current array according to the value of 'isPlayerTurn' variable
  CURRENT_ARRAY = isPlayerTurn ? OPPONENT_ARRAY : PLAYER_ARRAY;
}

void loop()
{
  if (isPlayerTurn) // Player is playing
  {
    int x, y;

    do
    {
      y = waitInput(VERTICAL);
      verticalLine = y;
      delay(200);
      x = waitInput(HORIZONTAL);
      verticalLine = 0;
    } while (!playOn(x, y)); // Only accept valid plays
  }
  else // Opponent is playing
  {
    waitOpponentPlay();
  }

  displayCurrentArray();
  delay(1000);

  isFinished();

  // debugCurrentArray();
  changeArray(); // Change turn
}

void waitOpponentPlay()
{
  while (!dataReceived) {}
  dataReceived = false;

  PLAYER_ARRAY[data[1]*GAME_BOARD_SIZE + data[0]] = true;

  if (BOAT_PLAYER_ARRAY[data[1]*GAME_BOARD_SIZE + data[0]] == true)
    playerBoatCellAmount--;

  Serial.println("Transmitting...");
  Wire.beginTransmission(9);
  Wire.write(BOAT_PLAYER_ARRAY[data[1]*GAME_BOARD_SIZE + data[0]]);
  Wire.endTransmission();

  displayCurrentArray();
  delay(1500);
}

int waitInput(ORIENTATION orientation)
{
  int coord;
  drawLine(coord, orientation); // draw the first line

  // Push for first coordinate, release for the second one
  while (digitalRead(7) != (orientation == VERTICAL ? HIGH : LOW))
  {
    coord = (coord + 1) % GAME_BOARD_SIDE_SIZE;
    drawLine(coord, orientation); // Draw the line moving
    delay(500); // Delay for the line speed
  }

  return coord;
}

void displayCurrentArray()
{
  screen.clear();
  for (int i = 0 ; i < GAME_BOARD_SIDE_SIZE ; i++)
  {
    for (int j = 0 ; j < GAME_BOARD_SIDE_SIZE ; j++)
    {
      int coord = j + GAME_BOARD_SIDE_SIZE * i;

      // Light the LED only if the bool is true on this coordinates
      if (CURRENT_ARRAY[coord]) screen.setPixelColor(coord, 127, 0, 0);
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
  CURRENT_ARRAY = isPlayerTurn ? OPPONENT_ARRAY : PLAYER_ARRAY;
}

bool playOn(int x, int y)
{
  // If the play is not valid
  if (OPPONENT_ARRAY[y + GAME_BOARD_SIDE_SIZE * x] == true) return false;

  int sendData[2] = {x, y};
  Wire.beginTransmission(9);
  Wire.write((byte)sendData);
  Wire.endTransmission();

  while (!dataReceived) {}
  if (hit)
    opponentBoatCellAmount--;

  // Play on the provided coordinates
  OPPONENT_ARRAY[y + GAME_BOARD_SIDE_SIZE * x] = true;
  return true;
}

void drawLine(int linePosition, ORIENTATION orientation)
{
  // Display the already played locations
  displayCurrentArray();

  // Since it's always VERTICAL then HORIZONTAL
  // Display the first line when the second on is moving
  if (orientation == HORIZONTAL)
  {
    for (int i = 0 ; i < GAME_BOARD_SIDE_SIZE ; i++)
      screen.setPixelColor(verticalLine + GAME_BOARD_SIDE_SIZE * i, screen.getPixelColor(verticalLine + GAME_BOARD_SIDE_SIZE * i) + 32512);
  }

  for (int i = 0 ; i < GAME_BOARD_SIDE_SIZE ; i++)
  {
    int coord;

    // Select the line orientation
    switch (orientation)
    {
      case HORIZONTAL:
        coord = linePosition * GAME_BOARD_SIDE_SIZE + i;
        break;
      case VERTICAL:
        coord = linePosition + GAME_BOARD_SIDE_SIZE * i;
        break;
    }

    // Add 127 to the green value of the pixel
    screen.setPixelColor(coord, screen.getPixelColor(coord) + 32512);
  }
  screen.show();
}

void initializeBoatBoard()
{
  for (int i = 0 ; i < playerBoatCellAmount ; i++)
  {
    int x, y;
    do
    {
      x = random(9);
      y = random(9);
    } while (PLAYER_ARRAY[y + GAME_BOARD_SIDE_SIZE * x]);

    PLAYER_ARRAY[y + GAME_BOARD_SIDE_SIZE * x] = true;
  }
}

void receiveEvent(int bytes)
{
  Serial.println("Receiving...");
  
  if (Wire.available() >= 2)
  {
    for (int i = 0; i < 2; i++)
    {
      data[i] = Wire.read();
      Serial.print(data[i]);
    }
  } else if (Wire.available() == 1)
  {
    hit = Wire.read();
    Serial.print(hit);

    if (hit) 
    {
      hit = false;
      opponentBoatCellAmount--;
    }   
  }
  dataReceived = true;
}

void isFinished()
{
  bool finished = (opponentBoatCellAmount <= 0) || (playerBoatCellAmount <= 0);
  if (!finished) return;

  for (int i = 0 ; i < GAME_BOARD_SIZE ; i++)
    screen.setPixelColor(i, 255, 0, 0);
}
