#define PIN_HV5812_CLK 7
#define PIN_HV5812_STROBE 5
#define PIN_HV5812_DATA 6
#define PIN_HV5812_BLANK 4
#define PIN_COLON_R 2
#define PIN_COLON_L 1

bool nixArray[60];
uint8_t n1, n2, n3, n4, n5, n6;

uint8_t d1[] = {9, 0, 6, 2, 3, 4, 5, 1, 7, 8};
uint8_t d2[] = {15, 14, 18, 12, 11, 10, 19, 13, 17, 16};
uint8_t d3[] = {35, 20, 38, 22, 23, 24, 39, 21, 37, 36};
uint8_t d4[] = {29, 34, 26, 32, 31, 30, 25, 33, 27, 28};
uint8_t d5[] = {55, 54, 58, 52, 51, 50, 59, 53, 57, 56};
uint8_t d6[] = {49, 40, 46, 42, 43, 44, 45, 41, 47, 48};

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("begin");
  
  uint8_t pins[] = {PIN_HV5812_CLK, PIN_HV5812_STROBE, PIN_HV5812_DATA, 
                      PIN_HV5812_BLANK, PIN_COLON_R, PIN_COLON_L,
                    };
                    
  for(int i=0; i<6; i++){
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }

  digitalWrite(PIN_COLON_L, HIGH);
  digitalWrite(PIN_COLON_R, HIGH);

  n1 = d1[1];
  n2 = d2[7];
  n3 = d3[4];
  n4 = d4[2];
  n5 = d5[3];
  n6 = d6[0];

  Serial.println("AAA");
  displayNix();
}

void loop() {
  // put your main code here, to run repeatedly:
  // for (int i=0; i<10; i++){
  //   n1 = d1[i];
  //   n2 = d2[i];
  //   n3 = d3[i];
  //   n4 = d4[i];
  //   n5 = d5[i];
  //   n6 = d6[i];
    
  //   displayNix();
    
  //   delay(500);
  // }
}

void displayNix(){
  for(int i = 59; i >= 0; i--){
    nixArray[i] = 1;
  }

  nixArray[n1] = 0;
  nixArray[n2] = 0;
  nixArray[n3] = 0;
  nixArray[n4] = 0;
  nixArray[n5] = 0;
  nixArray[n6] = 0;

  for (int i = 59; i >= 0; i--){
    digitalWrite(PIN_HV5812_DATA, nixArray[i]);
    digitalWrite(PIN_HV5812_CLK, HIGH);
    delay(1);
    digitalWrite(PIN_HV5812_CLK, LOW);
    delay(1);
  }

  digitalWrite(PIN_HV5812_STROBE, HIGH);
  delay(1);
  digitalWrite(PIN_HV5812_STROBE, LOW);
  delay(1);
}
