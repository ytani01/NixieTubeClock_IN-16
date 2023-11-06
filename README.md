# Nixie Tube Clock: IN-16

![](docs/figs/photo01.webp)

本物のニキシー管 IN-16 を使った置き時計です。
レトロな外界ですが、WiFi対応、OLED表示、エフェクトの切り替えなどの機能があります。

## ◆ 特徴

- WiFi対応で、インターネット(NTP)から常に正確な時刻を取得し、自動で時刻合わせします。
  - WiFiの設定は、スマートフォンから行います。
  - WiFiが使えないところでも、内部時計がバッテリーでバックアップされています。
- 底面に OLEDディスプレイがあり、WiFiの状態などを表示します。
- ニキシー管ならではの複数のエフェクトを切り替えることができます。
  (エフェクトなし、クロスフェード、フォグ(霧)、シャッフル)

### ◆ マニュアル

[マニュアル(PDF)](docs/Manual.pdf)

補足
- Reboot: [Button 0] + [Button 2]


## ◆ WiFi 設定

スマートフォンで、一旦、本機のアクセスポイントに接続してWiFiの設定を行います。

アクセスポイント名: NxClk_(固有のMACアドレス)

![WiFi Manger](docs/figs/WiFi_Manager.png)


## ◆ OLED表示

### NTP
![OLED-NTP](docs/figs/OLED-NTP.jpg)

### OLED: RTC
![OLED-RTC](docs/figs/OLED-RTC.jpg)


## ◆ Effects

### Crossfade
[![Crossfade](docs/movies/crossfade.jpg)](https://youtu.be/TvNmbtA_Buw?si=GQT42kCDqJkb_wn9)

### Fog
[![Fog](docs/movies/fog.jpg)](https://youtu.be/mSKaQIv3eQ8?si=7f47oo0_3DHgH0Qz)

### Shuffle
[![Shuffle](docs/movies/shuffle.jpg)](https://youtu.be/e5-Y6pX2xRI?si=2QsZdDAW-V6RgD0h)
