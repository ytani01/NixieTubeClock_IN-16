/**
 * Copyright (c) 2023 Yoichi Tanibayashi
 */
#include "Mode_Setclock.h"

/**
 *
 */
Mode_Setclock::Mode_Setclock(): Mode() {
} // Mode_Setclock::Mode_Setclock()

/** virtual
 *
 */
void Mode_Setclock::enter() {
  log_i("enter mode: %s", this->name.c_str());

  struct tm *tm = SysClock::now_tm();

  this->pos = SETCLOCK_POS_YEAR;
  this->prev_pos = SETCLOCK_POS_N;
  this->flag_nx_update = true;

  this->val[VAL_IDX_YEAR] = (tm->tm_year + 1900) - 2000;
  this->val[VAL_IDX_MONTH] = tm->tm_mon + 1;
  this->val[VAL_IDX_DAY] = tm->tm_mday;
  this->val[VAL_IDX_HOUR] = tm->tm_hour;
  this->val[VAL_IDX_MINUTE] = tm->tm_min;
  this->val[VAL_IDX_SEC] = (tm->tm_sec / 10) * 10;

  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setTextColor(WHITE, BLACK);

  Disp->setFont(NULL);
  Disp->setTextSize(2);
  Disp->setCursor(0, 0);
  Disp->setTextWrap(true);
  Disp->printf("%s", this->name.c_str());
  Disp->display();
} // Mode_Setclock::enter()

/** virtual
 *
 */
void Mode_Setclock::exit() {
  Nxa->end_all_effect();
} // Mode_Setclock::exit()

/** virtual
 *
 */
void Mode_Setclock::loop() {
  unsigned long cur_ms = millis();

  if ( ! this->flag_nx_update ) {
    delayOrChangeMode(this->LOOP_DELAY_MS);
    return;
  }
  this->flag_nx_update = false;
    
  //
  // Disp
  //
  Disp->fillRect(0, 0, DISPLAY_W, DISPLAY_H, BLACK);
  Disp->setCursor(0, 0);
  //Disp->setTextSize(2);

  char yy_str[3], mm_str[3], dd_str[3], HH_str[3], MM_str[3], SS_str[3];
  
  Disp->setTextColor(WHITE, BLACK);
  Disp->printf("20");
  if ( this->pos == SETCLOCK_POS_YEAR ) {
    Disp->setTextColor(BLACK, WHITE);
  } else {
    Disp->setTextColor(WHITE, BLACK);
  }
  Disp->printf("%02d", this->val[VAL_IDX_YEAR]);
  Disp->setTextColor(WHITE, BLACK);
  Disp->printf("/");
    
  if ( this->pos == SETCLOCK_POS_MONTH ) {
    Disp->setTextColor(BLACK, WHITE);
  } else {
    Disp->setTextColor(WHITE, BLACK);
  }
  Disp->printf("%02d", this->val[VAL_IDX_MONTH]);
  Disp->setTextColor(WHITE, BLACK);
  Disp->printf("/");
    
  if ( this->pos == SETCLOCK_POS_DAY ) {
    Disp->setTextColor(BLACK, WHITE);
  } else {
    Disp->setTextColor(WHITE, BLACK);
  }
  Disp->printf("%02d", this->val[VAL_IDX_DAY]);
  Disp->setTextColor(WHITE, BLACK);
  Disp->printf("\n");
    
  Disp->printf(" ");
  if ( this->pos == SETCLOCK_POS_HOUR ) {
    Disp->setTextColor(BLACK, WHITE);
  } else {
    Disp->setTextColor(WHITE, BLACK);
  }
  Disp->printf("%02d", this->val[VAL_IDX_HOUR]);
  Disp->setTextColor(WHITE, BLACK);
  Disp->printf(":");
    
  if ( this->pos == SETCLOCK_POS_MINUTE ) {
    Disp->setTextColor(BLACK, WHITE);
  } else {
    Disp->setTextColor(WHITE, BLACK);
  }
  Disp->printf("%02d", this->val[VAL_IDX_MINUTE]);
  Disp->setTextColor(WHITE, BLACK);
  Disp->printf(":");
    
  if ( this->pos == SETCLOCK_POS_SEC ) {
    Disp->setTextColor(BLACK, WHITE);
  } else {
    Disp->setTextColor(WHITE, BLACK);
  }
  Disp->printf("%02d", this->val[VAL_IDX_SEC]);

  Disp->display();

  //
  // Nixie_TubeArray
  //
  char *nx_fmt = NX_FMT_HMS;
  int num1, num2, num3;

  switch ( this->pos ) {
  case SETCLOCK_POS_YEAR:
  case SETCLOCK_POS_MONTH:
  case SETCLOCK_POS_DAY:
    nx_fmt = Mode_Setclock::NX_FMT_ymd;
    num1 = this->val[VAL_IDX_YEAR];
    num2 = this->val[VAL_IDX_MONTH];
    num3 = this->val[VAL_IDX_DAY];
    break;
  case SETCLOCK_POS_HOUR:
  case SETCLOCK_POS_MINUTE:
  case SETCLOCK_POS_SEC:
    nx_fmt = Mode_Setclock::NX_FMT_HMS;
    num1 = this->val[VAL_IDX_HOUR];
    num2 = this->val[VAL_IDX_MINUTE];
    num3 = this->val[VAL_IDX_SEC];
    break;
  default:
    log_e("pos = %d", (int)this->pos);
    break;
  } // switch (clock_mode)
  
  char nx_str[16];
  sprintf(nx_str, nx_fmt, num1, num2, num3);
  log_d("nx_str = %s", nx_str);

  Nxa->end_all_effect();
  Nxa->set_string(nx_str);

  //
  // blink current position
  //
  switch ( this->pos ) {
  case SETCLOCK_POS_YEAR:
  case SETCLOCK_POS_HOUR:
    Nxa->num[0].ef["blink"]->start(BLINK_INTERVAL_MS);
    Nxa->num[1].ef["blink"]->start(BLINK_INTERVAL_MS);
    break;
  case SETCLOCK_POS_MONTH:
  case SETCLOCK_POS_MINUTE:
    Nxa->num[2].ef["blink"]->start(BLINK_INTERVAL_MS);
    Nxa->num[3].ef["blink"]->start(BLINK_INTERVAL_MS);
    break;
  case SETCLOCK_POS_DAY:
  case SETCLOCK_POS_SEC:
    Nxa->num[4].ef["blink"]->start(BLINK_INTERVAL_MS);
    Nxa->num[5].ef["blink"]->start(BLINK_INTERVAL_MS);
    break;
  } // switch(pos)

  //
  this->prev_pos = this->pos;
  delayOrChangeMode(this->LOOP_DELAY_MS);
} // Mode_Setclock::loop()

/**
 *
 */
void Mode_Setclock::cbBtn(const ButtonInfo_t& bi,
                         const std::map<std::string, ButtonInfo_t>& btn_info) {
  log_d("%s", Button::info2String(bi).c_str());

  // ダブルクリックが次のモードに影響を与えないように
  static bool flag_mode_change = false;
  
  if ( flag_mode_change ) {
    if ( String(bi.name) == "Btn0" && bi.click_count > 0 ) {
      log_i("mode change");
      flag_mode_change = false;
      Mode::set("Mode_Clock");
    }
    return;
  }

  // enumを整数に変換
  int pos_i = static_cast<int>(this->pos);
  int pos_n = static_cast<int>(SETCLOCK_POS_N);

  if ( String(bi.name) == "Btn0" ) {
    if ( bi.value == Button::ON ) {
      if ( bi.long_pressed ) {
        if ( bi.repeat_count == 0 ) {
          flag_mode_change = false;
          Mode::set("Mode_Clock");
          return;
        }
      } else {
        if ( this->pos == SETCLOCK_POS_SEC ) {
          // 時刻設定完了
          struct tm tm;
          tm.tm_year = val[VAL_IDX_YEAR] + 2000 - 1900;
          tm.tm_mon = val[VAL_IDX_MONTH] - 1;
          tm.tm_mday = val[VAL_IDX_DAY];
          tm.tm_hour = val[VAL_IDX_HOUR];
          tm.tm_min = val[VAL_IDX_MINUTE];
          tm.tm_sec = val[VAL_IDX_SEC];

          SysClock::set(&tm);
          log_i("SysClock : %s", SysClock::now_string().c_str());

          Rtc->adjust(&tm);
          DateTime now_dt = Rtc->now();
          log_i("RTC      : %s", datetime2string(&now_dt).c_str());

          if ( TaskNtp->info.sntp_stat == SNTP_SYNC_STATUS_COMPLETED ) {
            //
            // NTP
            //
            TaskNtp->start_sync();
            delay(100);
        
            log_i("== adjust RTC from NTP");

            struct tm *tm_sys = SysClock::now_tm();
            log_i("  src Sys(NTP): %s",
                  tm2string(tm_sys, "%Y-%m-%d(%a),%H:%M:%S").c_str());

            Rtc->adjust(tm_sys);
    
            now_dt = Rtc->now();
            log_i("  dst RTC     : %s",
                  datetime2string(&now_dt, "%Y-%m-%d(%a),%H:%M:%S").c_str());
          }

          //Mode::set("Mode_Clock");
          flag_mode_change = true;
          log_i("flag_mode_change = %s",flag_mode_change ? "true" : "false");
          return;
        } // if (POS_SEC)

        // next pos
        pos_i++;
        log_i("pos_i = %d", pos_i);

        this->pos = static_cast<setclock_pos_t>(pos_i);

        // 「日」の場合、年月から、最終日を修正
        if ( this->pos == SETCLOCK_POS_DAY ) {
          this->val_max[VAL_IDX_DAY] = last_day(val[VAL_IDX_YEAR] + 2000,
                                                val[VAL_IDX_MONTH]);

          if ( this->val[VAL_IDX_DAY] > this->val_max[VAL_IDX_DAY] ) {
            this->val[VAL_IDX_DAY] = this->val_max[VAL_IDX_DAY];
          }
        }

        this->flag_nx_update = true;
      }
    }
    return;
  } // if (Btn0)

  if ( String(bi.name) == "Btn1" && bi.value == Button::ON ) {
    // increment
    this->val[pos_i]++;

    // 分の場合、長押しで、10秒単位に
    if ( this->pos == SETCLOCK_POS_MINUTE ) {
      if ( this->val[pos_i] % 10 == 1 ) {
        if ( bi.long_pressed ) {
          this->val[pos_i] += 9;
        }
      }
    }

    // 秒の場合、10秒ずつ
    if ( this->pos == SETCLOCK_POS_SEC ) {
      this->val[pos_i] += 9;
    }

    // 範囲の判断
    if ( this->val[pos_i] > this->val_max[pos_i] ) {
      this->val[pos_i] = this->val_min[pos_i];
      log_d("val[%d] = %d", pos_i, this->val[pos_i]);
    }

    this->flag_nx_update = true;
    return;
  } // if (Btn1)

  if ( String(bi.name) == "Btn2" && bi.value == Button::ON ) {
    // 月の最終日
    this->val_max[VAL_IDX_DAY] = last_day(val[VAL_IDX_YEAR] + 2000,
                                          val[VAL_IDX_MONTH]);
    log_d("val_max[VAL_IDX_DAY] = %d", this->val[VAL_IDX_DAY]);

    // decrement
    this->val[pos_i]--;

    // 分の場合、長押しで、10秒単位に
    if ( this->pos == SETCLOCK_POS_MINUTE ) {
      if ( this->val[pos_i] % 10 == 9 ) {
        if ( bi.long_pressed ) {
          this->val[pos_i] -= 9;
        }
      }
    }

    // 秒の場合、10秒ずつ
    if ( this->pos == SETCLOCK_POS_SEC ) {
      this->val[pos_i] -= 9;
    }

    // 範囲の調整
    if ( this->val[pos_i] < this->val_min[pos_i] ) {
      this->val[pos_i] = this->val_max[pos_i];
      log_d("val[%d] = %d", pos_i, this->val[pos_i]);
    }
    if ( this->pos == SETCLOCK_POS_SEC ) {
      if ( this->val[pos_i] == this->val_max[pos_i] ) {
        this->val[pos_i] = (this->val_max[pos_i] / 10) * 10;
      }
    } // if (POS_SEC)
    
    this->flag_nx_update = true;
    return;
  } // if (Btn2)

} // Mode_Setclock::cbBtn()
