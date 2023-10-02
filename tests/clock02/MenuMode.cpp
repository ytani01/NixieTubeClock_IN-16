/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "MenuMode.h"

/** constructor
 *
 */
MenuMode::MenuMode(String name, CommonData_t *common_data,
                   void (*cb)(String text))
  : Mode(name, common_data) {
  this->cb = cb;

} // MenuMode::MenuMode()

/**
 *
 */
void MenuMode::setup() {
  this->topMenu = new OledMenu("Top Menu");
  this->clockMenu = new OledMenu("Clock");
  this->thermometerMenu = new OledMenu("Thermo");
  this->wifiMenu = new OledMenu("WiFi");
  this->systemMenu = new OledMenu(String("System v") + String(VERSION_STR));

  OledMenuEnt *ment_mode_main = new OledMenuEnt("<< Exit Menu ", MODE_MAIN);
  OledMenuEnt *ment_mode_sto = new OledMenuEnt(" * Temp Offset ", MODE_SET_TEMP_OFFSET);
  OledMenuEnt *ment_mode_scan_ssid = new OledMenuEnt(" = SSID and Password ", MODE_SCAN_SSID);

  OledMenuEnt *ment_menu_top = new OledMenuEnt(" < Top Menu ", topMenu);
  OledMenuEnt *ment_menu_clock = new OledMenuEnt(" > Clock ", clockMenu);
  OledMenuEnt *ment_menu_thermometer = new OledMenuEnt(" > Thermometer ", thermometerMenu);
  OledMenuEnt *ment_menu_wifi = new OledMenuEnt(" > WiFi ", wifiMenu);
  OledMenuEnt *ment_menu_system = new OledMenuEnt(" > System ", systemMenu);

  OledMenuEnt *ment_text_clear_ssid = new OledMenuEnt("!! clear SSID ", "clear_ssid");
  OledMenuEnt *ment_text_restart_wifi = new OledMenuEnt(" @ restart WiFi ", "restart_wifi");
  OledMenuEnt *ment_text_reboot = new OledMenuEnt(" @ reboot ", "reboot");  
  OledMenuEnt *ment_text_dispfps = new OledMenuEnt(" i ON/OFF FPS ", "disp_fps");  

  OledMenuEnt *ment_line = new OledMenuEnt("--------------------");

  this->topMenu->addEnt(ment_mode_main);
  this->topMenu->addEnt(ment_line);
  this->topMenu->addEnt(ment_menu_clock);
  this->topMenu->addEnt(ment_menu_thermometer);
  this->topMenu->addEnt(ment_menu_wifi);
  this->topMenu->addEnt(ment_menu_system);

  this->clockMenu->addEnt(ment_menu_top);
  this->clockMenu->addEnt(ment_mode_main);
  this->clockMenu->addEnt(ment_line);

  this->thermometerMenu->addEnt(ment_menu_top);
  this->thermometerMenu->addEnt(ment_mode_main);
  this->thermometerMenu->addEnt(ment_line);
  this->thermometerMenu->addEnt(ment_mode_sto);

  this->wifiMenu->addEnt(ment_menu_top);
  this->wifiMenu->addEnt(ment_mode_main);
  this->wifiMenu->addEnt(ment_line);
  this->wifiMenu->addEnt(ment_mode_scan_ssid);
  this->wifiMenu->addEnt(ment_text_restart_wifi);
  this->wifiMenu->addEnt(ment_line);
  this->wifiMenu->addEnt(ment_text_clear_ssid);

  this->systemMenu->addEnt(ment_menu_top);
  this->systemMenu->addEnt(ment_mode_main);
  this->systemMenu->addEnt(ment_line);
  this->systemMenu->addEnt(ment_text_dispfps);
  this->systemMenu->addEnt(ment_line);
  this->systemMenu->addEnt(ment_text_reboot);

  this->curMenu = this->topMenu;
  this->curMenu->init();
}

/**
 *
 */
bool MenuMode::enter(Mode_t prev_mode) {
  Mode::enter(prev_mode);

  if ( prev_mode == MODE_MAIN ) {
    this->curMenu = this->topMenu;
    this->curMenu->init();
  }
  
  return true;
} // MenuMode()

/**
 * @return  destination mode
 */
Mode_t MenuMode::reBtn_cb(ButtonInfo_t *bi) {
  Mode_t dst_mode = MODE_N;

  if ( bi->click_count == 0 ) {
    return MODE_N;
  }

  if ( bi->click_count > 1 ) {
    return MODE_MAIN;
  }

  /*
   * bi->click_count == 1
   */
  OledMenuDst_t dst = this->curMenu->select();
  log_i("dst.type=%s", OLED_MENU_DST_TYPE_STR[dst.type]);

  switch ( dst.type ) {
  case OLED_MENU_DST_TYPE_MENU:
    log_i("dst.obj.menu=%s", dst.obj.menu->title_str());
    this->curMenu = dst.obj.menu;
    break;

  case OLED_MENU_DST_TYPE_MODE:
    log_i("dst.obj.mode=%s", MODE_T_STR[dst.obj.mode]);
    dst_mode = dst.obj.mode;
    break;

  case OLED_MENU_DST_TYPE_FUNC:
    log_i("call dst.obj.func(dst.param)");
    dst.obj.func(dst.param);
    dst_mode = MODE_MAIN;
    break;

  case OLED_MENU_DST_TYPE_TEXT:
    log_i("dst.obj.text=\"%s\"", dst.obj.text);

    if ( this->cb != NULL  ) {
      log_i("call this->cb(\"%s\")", dst.obj.text);
      (*(this->cb))(String(dst.obj.text));
    }
    break;

  default:
    log_e("dst.type=%d ??", dst.type);
    break;
  } // switch

  return dst_mode;
} // MenuMode::reBtn_cb()

/**
 *
 */
Mode_t MenuMode::obBtn_cb(ButtonInfo_t *bi) {
  if ( bi->click_count > 0 ) {
    _cd->msg = " Onboard Btn\n";
    _cd->msg += " click:" + String(bi->click_count);
  }
  return MODE_N;
} // MenuMode::obBtn_cb()

/**
 *
 */
void MenuMode::display(Display_t *disp) {
  this->curMenu->display(disp);
} // MenuMode::display()

/** protected
 *
 */
