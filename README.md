# ofxGrbl
[Beta] This is an add-on for using Grbl(CNC milling controll software for Arduino) with openframeworks. <br />
[Beta] Arduino用CNCコントロールソフトウェア「Grbl」をopenFrameworksから制御するためのアドオン。

[![ofxGrbl : Grbl( CNC milling controll software for Arduino) with openFrameworks demo](http://img.youtube.com/vi/3CR-sZpXvfI/0.jpg)](http://www.youtube.com/watch?v=3CR-sZpXvfI "ofxGrbl : Grbl( CNC milling controll software for Arduino) with openFrameworks demo")

## 必要なもの
### ソフトウェア

* ofxGrbl（oFアドオン本体）<br />
[https://github.com/TsubokuLab/ofxGrbl](https://github.com/TsubokuLab/ofxGrbl) 

* Grblソフトウェア（Arduino用スケッチ）<br />
 [https://github.com/grbl/grbl](https://github.com/grbl/grbl) 

### ハードウェア

#### 電子部品とか

| 名称 | 価格 |　備考 | 買った場所 |
| ------------- | ------------- | ------------- | ------------- |
| Arduino UNO | 1個 435円 | 互換基板でもなんでもOK | [http://amzn.to/2oNAEX2](http://amzn.to/2oNAEX2) |
| CNCシールド | 1個 225円 | 今回の主役となるシールド。かなり役に立つ。 | [http://amzn.to/2nLtFyg](http://amzn.to/2nLtFyg) |
| NEMA 17 Stepper Motor | 1個 $16.49 | 200 Stepのステッピングモーター。DC 12-24V/1.7 A | [http://bit.ly/2nLyXde](http://bit.ly/2nLyXde)  |
| A4988（ステッピングモータードライバ） | 5個で1000円くらい | 過電流等で割と頻繁に焼けるので、多めに買っておくのが吉。 | [http://amzn.to/2oNQD7r](http://amzn.to/2oNQD7r) |
| エンドリミットスイッチ | 1個 $0.44 | ホーミング（自動ホーム位置合わせ）する時に必要なスイッチ。レールが範囲外に行こうとして壊れるのを防いでくれる。機能を利用しない場合は必要ない。 | [http://bit.ly/2opXI1Q](http://bit.ly/2opXI1Q) |
| 12-24V 8A ACアダプタ | 1個 2100円 | ステッピングモーター1個あたり1.7Aなので8Aくらい欲しい。ちょっと作業中にショートしただけで一発で壊れるので、複数台必須。沢山壊した（涙） | [http://amzn.to/2oqFGN9](http://amzn.to/2oqFGN9) |
| 非常停止押ボタンスイッチ | 1個 788円 | モーターに負荷がかかったり基板が燃えそうになった時に緊急停止させるボタン。かなり重宝する。**あとロマン。** | [http://amzn.to/2oqHN3x](http://amzn.to/2oqHN3x) |

ちなみにArduino・CNCシールド・A4988はセットになっているのも売っていたのでこっちでもいいかも。

* Arduino+CNCシールド+A4988×4個セット 2630円<br />
[http://amzn.to/2nLkYUN](http://amzn.to/2nLkYUN)  

#### レールとか

少し高めだけどOpenBuildsのV-Slotシリーズがオススメ。

* OpenBuilds公式サイトはこちら<br />
[http://openbuildspartstore.com](http://openbuildspartstore.com)

オープンソースハードウェアなので関連パーツを3Dプリントしたりできる。

※基本は上記の公式ストアから買うのがオススメ。公式から買うほうが断然質が良いけど、ほどんどがAliexpressにもあるので安く揃えたい時はこちら。<br />
[https://www.aliexpress.com/store/123598](https://www.aliexpress.com/store/123598) 

| 名称 | 価格 |　備考 | 買った場所 |
| ------------- | ------------- | ------------- | ------------- |
| V-Slot® Linear Rail | 1mあたり $6 | V型の溝がついたライナーレール。1本の最長は1.5mまでだった気がする。 | 20mm x 20mm [http://bit.ly/2qD2H0e](http://bit.ly/2qD2H0e)<br />20mm x 40mm [http://bit.ly/2qcXwTz](http://bit.ly/2qcXwTz) |
| Solid V Wheel™ Kit | 20個セット $35 | V型の溝に沿って稼働するホイールキット | [http://bit.ly/2pIwxfD](http://bit.ly/2pIwxfD) |
| Smooth Idler Pulley wheel Kit | 20個入り $31.90 | アイドラープーリー（ベルト用滑車）のホイールキット。ベルトをレールに固定せずに | [http://bit.ly/2oO80or](http://bit.ly/2oO80or) |
| 偏心スペーサ6mm | 20個セット $19.90 | 中心が偏っていて、片側のホイールを締め付けるのに使うスペーサー | [http://bit.ly/2n1N4y7](http://bit.ly/2n1N4y7) |
| タイミングベルト　GT2 | 5m 1200円 | GT2（2mmピッチ）のタイミングベルト | [http://amzn.to/2nLpgf1](http://amzn.to/2nLpgf1) |
| プーリー | 40歯のもので680円 | タイミングベルトに合わせて2mmピッチのものを選ぶ。この歯数によってスピード・トルクに影響が出る。 | [http://amzn.to/2nu9enP](http://amzn.to/2nu9enP) |
| ティーナット　M5 | 25個入り $3.50 | ベルトやプレートを固定するときに必要なティーナット | [http://bit.ly/2n1Hufa](http://bit.ly/2n1Hufa) |
| 低頭ネジ　M5 x 6mm | 1個 $0.10 | ベルトをレールに固定するときに必要な6mm低頭ネジ | [http://bit.ly/2nOt6W6](http://bit.ly/2nOt6W6) |
| 低頭ネジ　M5 x 8mm | 100個入り $10.99 | プレートやコーナーパーツを固定するときに必要な8mm低頭ネジ | [http://bit.ly/2ntViu7](http://bit.ly/2ntViu7) |
| アイドラープーリープレート | 1枚 $3.75 | アイドラープーリー（ベルト滑車）用のプーリーをレールに固定するプレート | [http://bit.ly/2oqnR0d](http://bit.ly/2oqnR0d) |
| モーターマウントプレート | 1枚 $4.50 | ステッピングモーターをレールに固定するためのプレート | [http://bit.ly/2n1LWdT](http://bit.ly/2n1LWdT) |
| Gantryプレート Universal  | 1枚 $7.95 | 可動部等に色々物を固定して動かすための大きめのプレート。大きいものを載せなくていい場合は↓のプレートで良い。 | [http://bit.ly/2nOqKXA](http://bit.ly/2nOqKXA) |
| Gantryプレート 20mm | 1枚 $4.50 | 可動部等に色々物を固定して動かすための小さめのプレート。割と重宝する。 | [http://bit.ly/2oqmZZA](http://bit.ly/2oqmZZA) |
| 90度インサイドブラケット | 10個入り $5 | コーナー用レール内に入れて90度に固定するブラケット。角度の精度は高くないから場合によっては↓の方が良いかも？ | [http://bit.ly/2oy2LtY](http://bit.ly/2oy2LtY) |
| 90度キャストコーナー | 1個 $0.75 | レールにネジで固定するコーナーパーツ。ティーナットが必要なのと、低頭ネジじゃないとぶつかるので注意。 | [http://bit.ly/2oqqgs0](http://bit.ly/2oqqgs0) |

* 組み方等はOpenBuildsのサイトに知見が集合しているので眺めてるとだいたい分かってくる。<br />
[http://openbuilds.org/](http://openbuilds.org/)

## 関連アドオン

* ofxGrbl<br />
oFでGrbl使うアドオンの本体。（このアドオン）

* ofxUI<br />
UI表示に使っている

* ofxXmlSettings<br />
ofxUIの設定保存に使っている

## 使い方（途中）

* GrblManager : ofxGrbl example (Youtube)<br />
[![GrblManager : ofxGrbl example ](http://img.youtube.com/vi/54ps6AzPNp4/0.jpg)](http://www.youtube.com/watch?v=54ps6AzPNp4 "GrblManager : ofxGrbl example ")

## Grblの設定方法

設定を変更するにはシリアルコマンドで行う。
ArduinoのシリアルモニタでもOK!(改行コードCR付きでコマンドを送る事)
基本はここのページを参考にする。
 [https://github.com/grbl/grbl/wiki/Configuring-Grbl-v0.9](https://github.com/grbl/grbl/wiki/Configuring-Grbl-v0.9) 

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
| 1回転のステップ数 | 200 steps |
| マイクロステップ数 | 4 micros |
| ベルトピッチ | 2 mm |
| プーリーの歯数 | 20 teeth |

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
