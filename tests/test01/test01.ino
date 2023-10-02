/*
 * 
 */
#include <Arduino.h>
#include <esp32-hal-log.h>
#include "common.h"
#include "Button.h"

static int loopCount = 0;
CommonData_t commonData;  // see commo.h

constexpr int PIN_SDA = 8;
constexpr int PIN_SCL = 9;

constexpr int PIN_BTN_UP = 18;
constexpr int PIN_BTN_DOWN = 17;
constexpr int PIN_BTN_MODE = 14;

Button *btn_up, *btn_down, *btn_mode;
Button **Btn[] = {&btn_up, &btn_down, &btn_mode};

void IRAM_ATTR btn_intr_hdr(void *arg_btn) {
  Button *btn = static_cast<Button *>(arg_btn);
  static unsigned long __prev_ms = 0;
  unsigned long __cur_ms = millis();
  if ( __cur_ms - __prev_ms < Button::DEBOUNCE ) {
    return;
  }
  __prev_ms = __cur_ms;
  if ( ! btn->get() ) {
    return;
  }
  //log_d("btn->info.name=%s", btn->info.name);
  
  app_btn_intr_hdr(btn);
} // btn_intr_hdr()

void app_btn_intr_hdr(Button *btn) {
  //log_i("%s:%d", btn->get_name(), btn->get_value());
}

void setup() {
  Serial.begin(115200);
  for (int i=0; i < 15; i++) {
    delay(200);
    // Serial.println("AAA");
    log_i("Start:%d", i);
  }

  log_i("Btn size:%d", sizeof(Btn)/sizeof(Button **));
  btn_up = new Button("UP", PIN_BTN_UP, btn_intr_hdr);
  btn_down = new Button("DOWN", PIN_BTN_DOWN, btn_intr_hdr);
  btn_mode = new Button("MODE", PIN_BTN_MODE, btn_intr_hdr);

  Wire.setPins(PIN_SCL,PIN_SDA);
  Disp = new Display_t(DISPLAY_W, DISPLAY_H, &Wire, -1);
  Disp->DispBegin(DISPLAY_I2C_ADDR); // 0x3C in Display.h
  //delay(2000);
  Disp->setFont(NULL);
  Disp->setTextColor(WHITE, BLACK);
  Disp->setTextWrap(true);
  Disp->setRotation(0);
  Disp->clearDisplay();
  Disp->setCursor(0, 0);
  Disp->setTextSize(1);
  Disp->printf("AAA:%d\n", loopCount);
  Disp->printf("BBB:%d\n", loopCount);
  Disp->display();

}

void loop() {
  unsigned long cur_msec = millis();
  static unsigned long prev_btn_msec = 0;
  static constexpr unsigned long DISP_BTN_DELAY = 2000;

  if ( prev_btn_msec > 0 && cur_msec - prev_btn_msec > DISP_BTN_DELAY ) {
      Disp->setCursor(0, 1 * DISPLAY_CH_H);
      Disp->setTextSize(1);
      Disp->printf("%-21s", " ");
      Disp->display();

      prev_btn_msec = 0;
  }    

  for (int i=0; i < sizeof(Btn)/sizeof(Button *); i++) {
    if ( (*Btn[i])->get() ) {
      String btn_info = (*Btn[i])->toString_short();
      log_i("%s", btn_info.c_str());

      Disp->setCursor(0, 1 * DISPLAY_CH_H);
      Disp->setTextSize(1);
      Disp->printf("%-21s", btn_info.c_str());
      Disp->display();
      prev_btn_msec = cur_msec;
    }
  }

  //log_i("loop:%d", loopCount);

  loopCount++;
  delayMicroseconds(1);
  //delayMicroseconds(3000000);
  //delay(3000);
}
