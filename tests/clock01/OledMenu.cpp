/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#include "OledMenu.h"

/** constructor
 *
 */
OledMenuEnt::OledMenuEnt(String title) {
  this->title = title;
  this->dst.type = OLED_MENU_DST_TYPE_NULL;
  log_i("type=%s", OLED_MENU_DST_TYPE_STR[this->dst.type]);
} // OledMenuEnt::OledMenuEnt()

/** constructor
 *
 */
OledMenuEnt::OledMenuEnt(String title,
                         void (*func)(void *param), void *func_param) {
  this->title = title;
  this->dst.type = OLED_MENU_DST_TYPE_FUNC;
  this->dst.obj.func = func;
  this->dst.param = func_param;
  log_i("type=%s(%d)", OLED_MENU_DST_TYPE_STR[this->dst.type], this->dst.type);
} // OledMenuEnt::OledMenuEnt()

/** constructor
 *
 */
OledMenuEnt::OledMenuEnt(String title, OledMenu *menu) {
  this->title = title;
  this->dst.type = OLED_MENU_DST_TYPE_MENU;
  this->dst.obj.menu = menu;
  log_i("type=%s(%d): %s",
        OLED_MENU_DST_TYPE_STR[this->dst.type], this->dst.type,
        this->dst.obj.menu->title_str());
} // OledMenuEnt::OledMenuEnt()

/** constructor
 *
 */
OledMenuEnt::OledMenuEnt(String title, Mode_t mode) {
  this->title = title;
  this->dst.type = OLED_MENU_DST_TYPE_MODE;
  this->dst.obj.mode = mode;
  log_i("type=%s(%d): %s",
        OLED_MENU_DST_TYPE_STR[this->dst.type], this->dst.type,
        MODE_T_STR[this->dst.obj.mode]);
} // OledMenuEnt::OledMenuEnt()

/** constructor
 *
 */
OledMenuEnt::OledMenuEnt(String title, const char *text) {
  this->title = title;
  this->dst.type = OLED_MENU_DST_TYPE_TEXT;
  strcpy(this->dst.obj.text, text);
  log_i("type=%s(%d): %s",
        OLED_MENU_DST_TYPE_STR[this->dst.type], this->dst.type,
        this->dst.obj.text);
} // OledMenuEnt::OledMenuEnt()

/**
 *
 */
const char *OledMenuEnt::title_str() {
    return this->title.c_str();
} // OledMenuEnt::title_str()


/**
 *
 */
OledMenu::OledMenu(String title, int menu_ent_text_size) {
  this->title = title;
  this->init();
  this->menu_ent_text_size = menu_ent_text_size;
} // OledMenu::OledMenu()

/**
 *
 */
void OledMenu::init() {
  this->cur_ent = 0;
  this->disp_top_ent = 0;
} // OledMenu::init()

/**
 *
 */
const char *OledMenu::title_str() {
  return this->title.c_str();
} // OledMenu::title_str()

/**
 *
 */
int OledMenu::addEnt(OledMenuEnt *ment) {
    this->ent.push_back(*ment);
    return this->ent.size();
} // OledMenu::addEnt()

/**
 *
 */
void OledMenu::clean() {
  log_i("");
  this->ent.clear();
  this->ent.shrink_to_fit();

  this->cur_ent = 0;
  this->disp_top_ent = 0;
} // OledMenu::clear()

/**
 *
 */
int OledMenu::change_text_size(int text_size) {
  if ( text_size == 1 || text_size == 2 ) {
    this->menu_ent_text_size = text_size;
    return this->menu_ent_text_size;
  }

  if ( this->menu_ent_text_size == 1 ) {
    this->menu_ent_text_size = 2;
  } else {
    this->menu_ent_text_size = 1;
  }
  return this->menu_ent_text_size;
} // OledMenu::change_text_size()

/**
 * @return  destination menu
 */
OledMenuDst_t OledMenu::select() {
  OledMenuEnt ment = this->ent[this->cur_ent];

  log_i("ment[%s]", ment.title_str());

  switch ( ment.dst.type ) {
  case OLED_MENU_DST_TYPE_FUNC:
    if ( ment.dst.obj.func != NULL ) {
      log_i("==> call func()");

      ment.dst.obj.func(ment.dst.param);
    }
    break;

  case OLED_MENU_DST_TYPE_MENU:
    if ( ment.dst.obj.menu != NULL ) {
      log_i("==> Menu:[%s]", ment.dst.obj.menu->title_str());

      ment.dst.obj.menu->init();
    }
    break;

  case OLED_MENU_DST_TYPE_MODE:
    if ( ment.dst.obj.mode < MODE_N ) {
      log_i("==> Mode:[%s]", MODE_T_STR[ment.dst.obj.mode]);
    }
    break;

  case OLED_MENU_DST_TYPE_TEXT:
    if ( ment.dst.obj.text != NULL ) {
      log_i("==> Text:\"%s\"", ment.dst.obj.text);
    }
    break;

  default:
    log_e("ment.dst.type=%d ??", ment.dst.type);
    break;
  } // switch (ment.dst.type)
  
  return ment.dst;
} // OledMenu::select()

/**
 *
 */
void OledMenu::cursor_up() {
  int ent_n = this->ent.size();

  // XXX 循環させる場合
  // this->cur_ent = (this->cur_ent - 1 + ent_n) % ent_n;
  if ( this->cur_ent > 0 ) {
    this->cur_ent--;
  }
  
  if ( this->cur_ent < this->disp_top_ent ) {
    this->disp_top_ent = this->cur_ent;
  }
  log_i("ent=%d/%d top=%d", this->cur_ent, ent_n - 1, this->disp_top_ent);

  if ( this->ent[this->cur_ent].dst.type == OLED_MENU_DST_TYPE_NULL
       && this->cur_ent > 0 ) {
    this->cursor_up();
  }
} // OledMenu::cursor_up()

/**
 *
 */
void OledMenu::cursor_down() {
  int ent_n = this->ent.size();

  // XXX 循環させる場合
  // this->cur_ent = (this->cur_ent - 1 + ent_n) % ent_n;
  if ( this->cur_ent < (ent_n - 1) ) {
    this->cur_ent++;
  }
  
  int disp_ent_n
    = (DISPLAY_H - (DISPLAY_CH_H * MENU_TITLE_TEXT_SIZE))
    / (DISPLAY_CH_H * this->menu_ent_text_size);
  if ( this->cur_ent > this->disp_top_ent + (disp_ent_n - 1) ) {
    this->disp_top_ent = this->cur_ent - (disp_ent_n - 1);
  }
  log_i("ent=%d/%d top=%d", this->cur_ent, ent_n - 1, this->disp_top_ent);

  if ( this->ent[this->cur_ent].dst.type == OLED_MENU_DST_TYPE_NULL
       && this->cur_ent < (ent_n - 1) ) {
    this->cursor_down();
  }
} // OledMenu::cursor_down()

/**
 *
 */
void OledMenu::display(Display_t *disp) {
  disp->setTextWrap(false);
  
  disp->setFont(&FreeSans9pt7b);
  disp->setTextSize(1);
  int h = 12;
  disp->setCursor(0, h);
  disp->printf("%s\n", this->title_str());
  
  disp->drawFastHLine(0, h + 3, DISPLAY_W, WHITE);

  disp->setFont(NULL);
  disp->setTextSize(this->menu_ent_text_size);
  disp->setCursor(0, h + 5);
  int disp_ent_n
    = (DISPLAY_H - h - 5) / (DISPLAY_CH_H * this->menu_ent_text_size);
  for (int i=this->disp_top_ent;
       i <= this->disp_top_ent+disp_ent_n;
       i++) {
    if ( i >= this->ent.size() ) {
      break;
    }
    
    disp->setTextColor(WHITE, BLACK);
    if ( i == this->cur_ent ) {
      disp->setTextColor(BLACK, WHITE);
    }
    disp->printf("%s\n", this->ent[i].title_str());
    disp->setTextColor(WHITE, BLACK);
  } // for (i)
  
  //disp->display();
} // OledMenu::display()
