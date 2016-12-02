# ofxGrbl
[Beta] This is an add-on for using Grbl(CNC milling controll software for Arduino) with openframeworks. 
[Beta] Arduino用CNCコントロールソフトウェア「Grbl」をopenFrameworksから制御するためのアドオン。

[![ofxGrbl : Grbl( CNC milling controll software for Arduino) with openFrameworks demo](http://img.youtube.com/vi/3CR-sZpXvfI/0.jpg)](http://www.youtube.com/watch?v=3CR-sZpXvfI "ofxGrbl : Grbl( CNC milling controll software for Arduino) with openFrameworks demo")

## 必要なもの
### ソフトウェア

* ofxGrbl（oFアドオン本体）

 [https://github.com/TsubokuLab/ofxGrbl](https://github.com/TsubokuLab/ofxGrbl) 

* Grblソフトウェア（Arduino用スケッチ）

 [https://github.com/grbl/grbl](https://github.com/grbl/grbl) 

### ハードウェア

#### 基盤系

* Arduino UNO
* CNC Shileld

 Amazonで600円くらい。
 [http://amzn.asia/cXCFI9b](http://amzn.asia/cXCFI9b) 

* A4988（ステッピングモータードライバ）

 Amazonで1700円くらい。
 [http://amzn.asia/gfEnJ2x](http://amzn.asia/gfEnJ2x) 

* 必要に応じてエンドリミットとか

 ホーミング（自動ホーム位置合わせ）使わない場合はなくて大丈夫

#### レールとか

少し高めだけどOpenBuildsのV-Slotシリーズがオススメ。
オープンソースハードウェアなので関連パーツを3Dプリントしたりできる。

* V-Slot­® Linear Rail

 V型の溝がついたライナーレール
 [http://openbuildspartstore.com/v-slot-linear-rail/](http://openbuildspartstore.com/v-slot-linear-rail/) 

* Solid V Wheel™ Kit

 V型の溝に沿って稼働するホイールキット
 [http://openbuildspartstore.com/solid-v-wheel-kit/](http://openbuildspartstore.com/solid-v-wheel-kit/) 

* NEMA 17 Stepper Motor

 200 Stepのステッピングモーター。DC 12-24V/1.68 A。
 [http://openbuildspartstore.com/nema-17-stepper-motor/](http://openbuildspartstore.com/nema-17-stepper-motor/) 

* タイミングベルト　GT2

 2mmピッチのタイミングベルト。

* プーリー

 タイミングベルトに合わせて2mmピッチのものを選ぶ。この歯数によってスピード・トルクに影響が出る。

* 質は落ちるけど上記が大体Aliexpressにあるので、こっちで買うと安くて良い感じ
 [https://www.aliexpress.com/store/123598](https://www.aliexpress.com/store/123598) 

## 関連アドオン

* ofxGrbl

 oFでGrbl使うアドオンの本体

* ofxUI

 UI表示に使っている

* ofxXmlSettings

 ofxUIの設定保存に使っている

## 使い方（途中）

[![GrblManager : ofxGrbl example ](http://img.youtube.com/vi/54ps6AzPNp4/0.jpg)](http://www.youtube.com/watch?v=54ps6AzPNp4 "GrblManager : ofxGrbl example ")

## Grblの設定方法

設定を変更するにはシリアルコマンドで行う。
ArduinoのシリアルモニタでもOK!(改行コードCR付きでコマンドを送る事)

### 現在の設定を表示

```
$$
```
こんなのが返ってくる

```
$0=3 (step pulse, usec)
$1=1 (step idle delay, msec)
$2=0 (step port invert mask:00000000)
$3=0 (dir port invert mask:00000000)
$4=0 (step enable invert, bool)
$5=0 (limit pins invert, bool)
$6=0 (probe pin invert, bool)
$10=3 (status report mask:00000011)
$11=10.000 (junction deviation, mm)
$12=0.002 (arc tolerance, mm)
$13=0 (report inches, bool)
$20=0 (soft limits, bool)
$21=0 (hard limits, bool)
$22=0 (homing cycle, bool)
$23=0 (homing dir invert mask:00000000)
$24=25.000 (homing feed, mm/min)
$25=500.000 (homing seek, mm/min)
$26=250 (homing debounce, msec)
$27=1.000 (homing pull-off, mm)
$100=20.000 (x, step/mm)
$101=20.000 (y, step/mm)
$102=20.000 (z, step/mm)
$110=20000.000 (x max rate, mm/min)
$111=20000.000 (y max rate, mm/min)
$112=20000.000 (z max rate, mm/min)
$120=500.000 (x accel, mm/sec^2)
$121=500.000 (y accel, mm/sec^2)
$122=500.000 (z accel, mm/sec^2)
$130=1301.470 (x max travel, mm)
$131=833.460 (y max travel, mm)
$132=994.120 (z max travel, mm)
ok
```

### 設定を変更する

番号と数値をコマンドで送信する。
* 例)X軸の最高移動速度を15000mm/minに設定する場合。


```
$110=15000
```
という感じ。

### ステップ数の計算方法

移動距離を合わせるため、

* ステッピングモーター1回転のステップ数(steps)
* ステッピングモータードライバのマイクロステップ数(micros)
* ベルトピッチ(mm)
* プーリー1回転の歯数(teeth)

から、1mmあたりのステップ数を計算する必要がある。
計算式は、

```
 ( [1回転のステップ数(steps)] * [マイクロステップ数(micros)] ) / ( [ベルトピッチ(mm)] * [プーリーの歯数(teeth)] ) = 1mmあたりのステップ数(step/mm)
```
なので、たとえば

| 項目名 | 値 |
| ------------- | ------------- |
| 1回転のステップ数  | 200 steps  |
| マイクロステップ数  | 4 micros |
| ベルトピッチ  | 2 mm  |
| プーリーの歯数  | 20 teeth  |
の条件の場合は、

```
(200 steps * 4 micros) / (2 mm * 20 teeth) = 20 (step/mm)
```
というような感じ。
この値を、XYZ軸それぞれシリアルコマンドでGrblの設定に反映する事で移動距離がピッタリ揃う。

```
$100=20
$101=20
$102=20
```
## イラレデータからパスを読み込みたいとき
順序としては、以下の流れ。
**イラレデータ（.ai）→DXFデータ（.dxf）→GCodeデータ（.gcode / .ngc / .nc）**

### 必要ソフト
dxf2gcode
:  DXFファイルからGCodeを生成するフリーソフト
    [https://sourceforge.net/projects/dxf2gcode/](https://sourceforge.net/projects/dxf2gcode/)

### 手順
1. イラレからパスをDXF書き出しする。参考： [http://mathrax.sakura.ne.jp/kuze_jp/p-comp/cad/page003.html](http://mathrax.sakura.ne.jp/kuze_jp/p-comp/cad/page003.html) 
2. DXFファイルをdxf2gcodeで読み込んで、.gcode / .ngc 形式で書き出す。
3. 出力された .gcode / .ngcファイルを、LOADアイコンもしくはアプリにドラッグ＆ドロップして読み込む。 
