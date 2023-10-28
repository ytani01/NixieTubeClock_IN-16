/**
 * Copyright (c) 2022 Yoichi Tanibayashi
 */
#ifndef _OLED_MENU_H_
#define _OLED_MENU_H_

#include <vector>
#include <esp32-hal-log.h>
#include "common.h"
#include "Display.h"

constexpr int MENU_TITLE_TEXT_SIZE = 2;
constexpr int MENU_ENT_TEXT_SIZE = 1;

static constexpr int TITLE_LEN = 16;

class OledMenu; // 不完全型: OledMenuEntからの相互参照のために必要

typedef enum {
              OLED_MENU_DST_TYPE_FUNC,
              OLED_MENU_DST_TYPE_MENU,
              OLED_MENU_DST_TYPE_MODE,
              OLED_MENU_DST_TYPE_TEXT,
              OLED_MENU_DST_TYPE_NULL
} OledMenuDstType_t;
static const char *OLED_MENU_DST_TYPE_STR[] = {"FUNC", "MENU", "MODE", "TEXT",
                                               "NULL"};

static constexpr int MENT_DST_TEXT_SIZE = 32;
typedef struct {
  OledMenuDstType_t type;
  union {
    void (*func)(void *param);
    OledMenu *menu;
    Mode_t mode;
    char text[MENT_DST_TEXT_SIZE];
  } obj;
  void *param;
} OledMenuDst_t;

extern OledMenu *OledMenu_curMenu;

/**
 *
 */
class OledMenuEnt {
public:
  String title;
  OledMenuDst_t dst;

  OledMenuEnt(String title);
  OledMenuEnt(String title, void (*func)(void *param), void *func_param);
  OledMenuEnt(String title, OledMenu *menu);
  OledMenuEnt(String title, Mode_t mode);
  OledMenuEnt(String title, const char *text);

  const char *title_str();
}; // class OledMEnuEnt

/**
 *
 */
class OledMenu {
public:
  String title;
  std::vector<OledMenuEnt> ent;
  int cur_ent = 0;
  int disp_top_ent = 0;
  int menu_ent_text_size;

  OledMenu(String title, int menu_ent_text_size=MENU_ENT_TEXT_SIZE);

  void init();
  const char *title_str();
  int addEnt(OledMenuEnt *ment);
  void clean();
  int change_text_size(int text_size=0);

  OledMenuDst_t select();
  void cursor_up();
  void cursor_down();

  void display(Display_t *disp);
}; // OledMenu

#endif // _OLED_MENU_H_
