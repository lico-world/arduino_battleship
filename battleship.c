#define GAME_BOARD_SIZE 5

bool PLAYER_ARRAY[GAME_BOARD_SIZE * GAME_BOARD_SIZE] = {true, false, false, false, true, false, true, true, false, true, true, false, false, false, true, false, true, true, false, true, false, true, true, false, true};
bool OPPONENT_ARRAY[GAME_BOARD_SIZE * GAME_BOARD_SIZE] = {true, false, false, false, true, false, true, true, false, true, false, false, false, true, true, false, true, false, true, true, false, false, false, true, false};;
bool * CURRENT_ARRAY;

bool isDisplaySubjectThePlayer = true;

void setup()
{
  Serial.begin(9600);

  CURRENT_ARRAY = PLAYER_ARRAY;

  // Verify the array reading navigation functionality
  debugCurrentArray();
  changeArray();
  debugCurrentArray();

  // Verify the LED array functionality
  displayCurrentArray();
  changeArray();
  displayCurrentArray();
}

void loop()
{
  
}

void displayCurrentArray()
{
  for(int i = 0 ; i < GAME_BOARD_SIZE ; i++)
  {
    for(int j = 0 ; j < GAME_BOARD_SIZE ; j++)
    {
      
    }
  }
}

void debugCurrentArray()
{
  for (int i = 0 ; i < GAME_BOARD_SIZE ; i++)
  {
    for (int j = 0 ; j < GAME_BOARD_SIZE ; j++)
    {
      Serial.print(CURRENT_ARRAY[j + GAME_BOARD_SIZE * i] ? '#' : '~');
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
