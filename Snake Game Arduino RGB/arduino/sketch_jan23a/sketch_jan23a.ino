#include <MD_MAX72xx.h>
#include <Adafruit_NeoPixel.h>


#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_PIN 6

#define LED_COUNT 256

#define BRIGHTNESS 50

Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


#define CLK_PIN 13
#define DATA_PIN 11
#define CS_PIN 10


#define UP_BUTTON_PIN 0
#define DOWN_BUTTON_PIN 1
#define LEFT_BUTTON_PIN 2
#define RIGHT_BUTTON_PIN 3

#define GAME_SPEED 200

int map_array[16][16] = {0};
int tail[2] = {3,8};
int mid[2] = {4,8};
int head[2] = {5,8};
int seed[2];


bool ate = true;
bool end = false;
int move_direction = DOWN_BUTTON_PIN;


MD_MAX72XX mx = MD_MAX72XX(MD_MAX72XX::PAROLA_HW,DATA_PIN,CLK_PIN,CS_PIN);

void find_tail() {
  int temp = map_array[head[0]][head[1]];

  for (byte i = 0; i < 16; i = i + 1) {
    for (byte j = 0; j < 16; j = j + 1) {
      if(map_array[i][j] > 0 & map_array[i][j] < temp) {
        temp = map_array[i][j];
        tail[0] = i;
        tail[1] = j;
      }
    }
  }
}

bool check_wrong_direction(int column,int row,int direction,int move_direction) {
  column = head[0];
  row = head[1];

  if(direction == move_direction) {
    return true;
  }


  if(direction == UP_BUTTON_PIN) {
    column = column - 1;
  }
  else if(direction == DOWN_BUTTON_PIN) {
    column = column + 1;
  }
  else if(direction == LEFT_BUTTON_PIN) {
    row = row - 1;
  }
  else if(direction == RIGHT_BUTTON_PIN) {
    row = row + 1;
  }

  if((column == mid[0] | row == mid[1]) & map_array[mid[0]][mid[1]] != map_array[head[0]][head[1]] - 1) {
      return true;
  }
  else {
    return false;
  }
}

bool check_lose() {
  if(map_array[head[0]][head[1]] > 0) {
    set_zero();
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    pixels.setPixelColor(3, pixels.Color(255, 0, 0));
    pixels.show();
    end = true;
    return true;
  }
  else if(head[0] > 15 | head[0] < 0 | head[1] > 15 | head[1] < 0 ) {
    set_zero();
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    pixels.setPixelColor(3, pixels.Color(255, 0, 0));
    pixels.show();
    end = true;
    return true;
  }
  else {
    return false;
  }
}

bool set_zero() {
  for (int i = 0; i < 16; i = i + 1) {
    for (int j = 0; j < 16; j = j + 1) {
      pixels.setPixelColor(i*16+j, pixels.Color(0, 0, 0));
    }
  }
  pixels.show();
}

bool check_end() {
  for (byte i = 0; i < 16; i = i + 1) {
    for (byte j = 0; j < 16; j = j + 1) {
      if(!(map_array[i][j] > 0 | map_array[i][j] == -2)) {
        return false;
      }
    }
  }
}

bool eat() {
  if(map_array[head[0]][head[1]] == -2) {
    ate = true;
    return true;
  }
  else {
    return false;
  }
}

void put_seed() {
  do {
    seed[0] = random(8);
    seed[1] = random(8);
  } while (map_array[seed[0]][seed[1]] > 0);
  

  pixels.setPixelColor(seed[0]*16+seed[1], pixels.Color(0, 255, 0));
  map_array[seed[0]][seed[1]] = -2;

  pixels.show();
}


bool movement(int direction) {
  int headNum = 0;
  bool grow = false;

  if(direction == UP_BUTTON_PIN) {
    headNum = map_array[head[0]][head[1]];
    mid[0] = head[0];
    mid[1] = head[1];
    head[0] = head[0] - 1;

    if(check_lose()) {
      return false;
    }

    grow = eat();

    map_array[head[0]][head[1]] = headNum + 1;
    // mx.setPoint(head[0],head[1],true);
    pixels.setPixelColor(head[0]*16+head[1], pixels.Color(255, 0, 0));

    if(!grow) {
      map_array[tail[0]][tail[1]] = 0;
      //mx.setPoint(tail[0],tail[1],false);
      pixels.setPixelColor(tail[0]*16+tail[1], pixels.Color(0, 0, 0));
      find_tail();
    }
  }
  else if(direction == DOWN_BUTTON_PIN) {
    headNum = map_array[head[0]][head[1]];
    mid[0] = head[0];
    mid[1] = head[1];
    head[0] = head[0] + 1;

    if(check_lose()) {
      return false;
    }
    grow = eat();

    map_array[head[0]][head[1]] = headNum + 1;
    //mx.setPoint(head[0],head[1],true);
    pixels.setPixelColor(head[0]*16+head[1], pixels.Color(255, 0, 0));

    if(!grow) {
      map_array[tail[0]][tail[1]] = 0;
      //mx.setPoint(tail[0],tail[1],false);
      pixels.setPixelColor(tail[0]*16+tail[1], pixels.Color(0, 0, 0));
      find_tail();
    }
  }
  else if(direction == LEFT_BUTTON_PIN) {
    headNum = map_array[head[0]][head[1]];
    mid[0] = head[0];
    mid[1] = head[1];
    head[1] = head[1] - 1;

    if(check_lose()) {
      return false;
    }
    grow = eat();

    map_array[head[0]][head[1]] = headNum + 1;
    //mx.setPoint(head[0],head[1],true);
    pixels.setPixelColor(head[0]*16+head[1], pixels.Color(255, 0, 0));

    if(!grow) {
      map_array[tail[0]][tail[1]] = 0;
      //mx.setPoint(tail[0],tail[1],false);
      pixels.setPixelColor(tail[0]*16+tail[1], pixels.Color(0, 0, 0));
      find_tail();
    }
  }
  else if(direction == RIGHT_BUTTON_PIN) {
    headNum = map_array[head[0]][head[1]];
    mid[0] = head[0];
    mid[1] = head[1];
    head[1] = head[1] + 1;

    if(check_lose()) {
      return false;
    }
    grow = eat();

    map_array[head[0]][head[1]] = headNum + 1;
    //mx.setPoint(head[0],head[1],true);
    pixels.setPixelColor(head[0]*16+head[1], pixels.Color(255, 0, 0));
    
    if(!grow) {
      map_array[tail[0]][tail[1]] = 0;
      //mx.setPoint(tail[0],tail[1],false);
      pixels.setPixelColor(tail[0]*16+tail[1], pixels.Color(0, 0, 0));
      find_tail();
    }
  }
  pixels.show();
}



void setup() {
  pinMode(UP_BUTTON_PIN,INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN,INPUT_PULLUP);
  pinMode(LEFT_BUTTON_PIN,INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN,INPUT_PULLUP);


  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  pixels.begin();

  pixels.setPixelColor(3*16+8, pixels.Color(255, 0, 0));
  pixels.setPixelColor(4*16+8, pixels.Color(255, 0, 0));
  pixels.setPixelColor(5*16+8, pixels.Color(255, 0, 0));
  pixels.show();


  map_array[3][8] = 1;
  map_array[4][8] = 2;
  map_array[5][8] = 3;
}




void loop() {
  if(end) {
    return;
  }

  if(digitalRead(UP_BUTTON_PIN) == LOW) {
    if(!check_wrong_direction(head[0],head[1],UP_BUTTON_PIN,move_direction)) {
      move_direction = UP_BUTTON_PIN;
      movement(move_direction);
    }
  }
  else if(digitalRead(DOWN_BUTTON_PIN) == LOW) {
    if(!check_wrong_direction(head[0],head[1],DOWN_BUTTON_PIN,move_direction)) {
      move_direction = DOWN_BUTTON_PIN;
      movement(move_direction);
    }
  }
  else if(digitalRead(LEFT_BUTTON_PIN) == LOW) {
    if(!check_wrong_direction(head[0],head[1],LEFT_BUTTON_PIN,move_direction)) {
      move_direction = LEFT_BUTTON_PIN;
      movement(move_direction);
    }
  }
  else if(digitalRead(RIGHT_BUTTON_PIN) == LOW) {
    if(!check_wrong_direction(head[0],head[1],RIGHT_BUTTON_PIN,move_direction)) {
      move_direction = RIGHT_BUTTON_PIN;
      movement(move_direction);
    }
  }
  else {
    movement(move_direction);
  }

  if(check_end()) {
    set_zero();
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.setPixelColor(1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(3, pixels.Color(0, 255, 0));
    pixels.show();
  }

 
  
  if(ate) {
    put_seed();
    ate = false;
  }

  delay(GAME_SPEED);
}
