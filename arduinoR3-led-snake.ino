
//================== Directions ==================
enum Direction { UP = 1, DOWN, LEFT, RIGHT };

//================== Board size ==================
const int GRID_SIZE = 8;

//================ Joystick pins =================
const int VRxPin = A0; //X-axis connected to analog pin A0
const int VRyPin = A1; //Y-axis connected to analog pin A1
const int SWPin = 1;   // Push-button connected to digital pin 1
                        // WARNING: pin 1 is the hardware RX/TX line on Uno/Nano.
                        // Using Serial.begin() for debugging will conflict with this button.
                        // Consider moving it to a different digital pin if you need Serial.

//=============== LED matrix pins ================ 
const byte rowPin[GRID_SIZE] = {4, 2, 7, 3, 13, 6, 12, 9};
const byte colPin[GRID_SIZE] = {A3, 11, 10, 5, 8, A2, A4, A5};

//================== data struct ================== 
struct Position {int x, y;};

//================ game variables ================
int snakeLength;
int headIndex;
Position fruit;
Position snakeSegments[GRID_SIZE * GRID_SIZE];
bool diode[GRID_SIZE][GRID_SIZE] = {false};

//================ bit map ================
const bool sadFace[8][8] = {
  {0,0,1,1,1,1,0,0},
  {0,1,0,0,0,0,1,0},
  {1,0,1,0,0,1,0,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,1,1,0,0,1},
  {1,0,1,0,0,1,0,1},
  {0,1,0,0,0,0,1,0},
  {0,0,1,1,1,1,0,0}
};

//================ timing ================
unsigned long lastMoveTime = 0;
const unsigned long moveInterval = 150;

void clearDiodes(){
  for(int thisPin = 0; thisPin < GRID_SIZE; thisPin++){
    digitalWrite(rowPin[thisPin], LOW);
    digitalWrite(colPin[thisPin], HIGH);
  }
}

void setDiode(int x, int y) {
  digitalWrite(rowPin[x], HIGH);
  digitalWrite(colPin[y], LOW); 
}

void clearDiode(int x, int y) {
  digitalWrite(rowPin[x], LOW);
  digitalWrite(colPin[y], HIGH);
}

void turnOnDiode(int x, int y){
  diode[x][y] = true;
}

void turnOffDiode(int x, int y){
  diode[x][y] = false;
}

void refreshDiodes(){
  for(int i = 0; i < GRID_SIZE; i++) {
    for(int j = 0; j < GRID_SIZE; j++) {
      if(diode[i][j] == true) {
        setDiode(i, j);
        delayMicroseconds(5); //controls brightness via short on-time during multiplexing
        clearDiode(i, j);
      }
    }
  }
}

void moveSnake(int direction){
  int tailIndex = (headIndex + snakeLength - 1) % (snakeLength);
  turnOffDiode(snakeSegments[tailIndex].x, snakeSegments[tailIndex].y); //turn off last segment of the snake
  switch (direction) {
    case LEFT:
      if(snakeSegments[headIndex].y != 0){
        snakeSegments[tailIndex].x = snakeSegments[headIndex].x;         //Move last segment to the front
        snakeSegments[tailIndex].y = snakeSegments[headIndex].y - 1;
      }
      break;
    case RIGHT:
      if(snakeSegments[headIndex].y != GRID_SIZE - 1){
        snakeSegments[tailIndex].x = snakeSegments[headIndex].x;          //Move last segment to the front
        snakeSegments[tailIndex].y = snakeSegments[headIndex].y + 1;
      }
      break;
    case UP:
      if(snakeSegments[headIndex].x != 0){
        snakeSegments[tailIndex].x = snakeSegments[headIndex].x - 1;      //Move last segment to the front
        snakeSegments[tailIndex].y = snakeSegments[headIndex].y;
      }
      break;
    case DOWN:
      if(snakeSegments[headIndex].x != GRID_SIZE - 1){
        snakeSegments[tailIndex].x = snakeSegments[headIndex].x + 1;      //Move last segment to the front
        snakeSegments[tailIndex].y = snakeSegments[headIndex].y;
      }
      break;
  }
  headIndex = tailIndex; //Establish new head segment
  turnOnDiode(snakeSegments[headIndex].x, snakeSegments[headIndex].y);
}

void spawnFruit(){
  do {
    fruit.x = random(0, GRID_SIZE);
    fruit.y = random(0, GRID_SIZE);
  } while (diode[fruit.x][fruit.y] == true);
  turnOnDiode(fruit.x, fruit.y);
}

void checkCollisionOrFruit(int direction){
  int next_x;
  int next_y;

  switch (direction){
    case LEFT:
      next_x = snakeSegments[headIndex].x;
      next_y = snakeSegments[headIndex].y - 1;
      break;
    case RIGHT:
      next_x = snakeSegments[headIndex].x;
      next_y = snakeSegments[headIndex].y + 1;
      break;
    case UP:
      next_x = snakeSegments[headIndex].x - 1;
      next_y = snakeSegments[headIndex].y;
      break;
    case DOWN:
      next_x = snakeSegments[headIndex].x + 1;
      next_y = snakeSegments[headIndex].y;
      break;
  }

  if(fruit.x == next_x && fruit.y == next_y) {
    snakeLength++;
    spawnFruit();
    return;
  } //check for fruit and eat

  if (next_x < 0 || next_x > GRID_SIZE - 1 || next_y < 0 || next_y > GRID_SIZE - 1) { 
    gameover(); 
    return;
  } //check border collision

  for(int i = 0; i < snakeLength; i++){
    if(snakeSegments[i].x == next_x && snakeSegments[i].y == next_y){
      gameover();
      return;
    }
  } 

}

void reset(){
  clearDiodes();
  snakeLength = 1;
  headIndex = 0;
  snakeSegments[headIndex].x = 0;
  snakeSegments[headIndex].y = 0;
  memset(diode, false, sizeof(diode));
  turnOnDiode(snakeSegments[headIndex].x, snakeSegments[headIndex].y);
  spawnFruit();
}

void gameover(){
  unsigned long startTime = millis();
  clearDiodes();
  memcpy(diode, sadFace, sizeof(diode));
  while(millis() - startTime < 4000) {
    refreshDiodes();
  }
  reset();
}

int readDirection() {
  int xValue = analogRead(VRxPin);
  int yValue = analogRead(VRyPin);

  if (yValue > 600) return UP;
  if (xValue > 600) return RIGHT;
  if (yValue < 500) return DOWN;
  if (xValue < 500) return LEFT;
  return 0; //no movement
}


void setup() {
  pinMode(SWPin, INPUT_PULLUP);

  for(int thisPin = 0; thisPin < GRID_SIZE; thisPin++){
    pinMode(rowPin[thisPin], OUTPUT); 
    pinMode(colPin[thisPin], OUTPUT); 
  }

  clearDiodes();

  snakeLength = 1;
  headIndex = 0;
  snakeSegments[headIndex].x = 0;
  snakeSegments[headIndex].y = 0;
  turnOnDiode(snakeSegments[headIndex].x, snakeSegments[headIndex].y);
  spawnFruit();
  refreshDiodes();
}

void loop() {
  refreshDiodes();

  if (digitalRead(SWPin) == LOW) {reset();}

  if(millis() - lastMoveTime >= moveInterval) {
    lastMoveTime = millis();
    int direction = readDirection();
    if (direction != 0) {     
      checkCollisionOrFruit(direction);    
      moveSnake(direction);
    }
  }
}
