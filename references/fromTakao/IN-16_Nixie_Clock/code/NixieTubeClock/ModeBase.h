/*
 * (c) 2020 Yoichi Tanibayashi
 */
#ifndef MODE_BASE_H
#define MODE_BASE_H
#include <Arduino.h>
#include "NixieArray.h"
#include "Button.h"

class ModeBase {
 public:
  static const unsigned long TICK_MS = 5000; // ms
  static const unsigned long EFFECT_TICK_MS = 200; // ms

  static const stat_t STAT_NONE      = 0x00;  // 
  static const stat_t STAT_SKIP      = 0x01;  // loop()実行をskip
  static const stat_t STAT_DONE      = 0x02;  // ??
  static const stat_t STAT_BACK_MODE = 0x10;  // モード終了(前のモードへ)
  static const stat_t STAT_NEXT_MODE = 0x11;  // モード終了(次のモードへ)

  stat_t stat = STAT_NONE;

  ModeBase(NixieArray *nxa, String name, unsigned long tick_ms);

  String name();
  boolean tick(unsigned long cur_ms);

  // loop()内でのモード初期化
  virtual void init(unsigned long start_ms, DateTime& now,
                    int init_val[NIXIE_NUM_N]);

  // loop()内での処理
  virtual stat_t loop(unsigned long cur_ms, DateTime& now);

  // ボタン・ハンドラ
  virtual void btn_intr_hdr(unsigned long cur_ms, Button *btn); // ボタン処理
  virtual void btn_loop_hdr(unsigned long cur_ms, Button *btn); // ボタン処理

 protected:
  String        _name;         // モード名
  NixieArray   *_nxa;          // ニキシー管アレイ
  unsigned long _start_ms;
  unsigned long _tick_ms;      // tick間隔(msec): loop()で処理する間隔
  unsigned long _tick;         // tickカウント
  unsigned long _prev_tick;

  unsigned _num[NIXIE_NUM_N];
};
#endif // MODE_BASE_H
