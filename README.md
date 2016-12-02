# ofxGrbl
[Beta] This is an add-on for using Grbl(CNC milling controll software for Arduino) with openframeworks. 

[https://youtu.be/3CR-sZpXvfI](https://youtu.be/3CR-sZpXvfI)

Arduino用CNCコントロールスケッチ「Grbl」をopenFrameworksから制御するためのアドオン。

![image][1073977454]
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
* 質は落ちるけど上記が大体Aliexpressにあるので、こっちで買うと安くて良い感じ。

 [https://www.aliexpress.com/store/123598](https://www.aliexpress.com/store/123598) 

## 関連アドオン
* ofxGrbl
　oFでGrbl使うアドオンの本体
* ofxUI
　UI表示に使っている
* ofxXmlSettings
　ofxUIの設定保存に使っている

## 使い方（途中）
[https://youtu.be/54ps6AzPNp4](https://youtu.be/54ps6AzPNp4)