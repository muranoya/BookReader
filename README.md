BookReader
==========

## Requirement
* libarchive >= 3.2.0
* Qt >= 5.6
* C++11に対応しているC++コンパイラ

## Build
#### On Linux
  1. libarchive-dev のインストール
  1. src ディレクトリで qmake する
  1. Makefile が生成されるので make する
  1. 実行ファイルが生成される

#### On Windows
  1. libarchive をビルド
   * Note: zip などを扱うには別途 zlib などのインストールが必要
   * Note: MSVCでのビルドのほうが容易
  1. src ディレクトリの BookReader.pro を開きINCLUDEPATHとLIBSにそれぞれ
  libarchive の include と lib ディレクトリのパスを追加
  1. src ディレクトリの BookReader.pro を Qt Creator で開きビルド
   * MSVC版よりもMinGW版Qtのほうが容易にビルド可能
  1. 実行ファイルが生成される
  1. windeployqt.exeを使うとデプロイが簡単
  
## TODO
* 設定の保存と読み込み (済み 2014/3/14)
* スライドショーの実装 (済み 2014/3/19)
* 設定ウィンドウの実装 (済み 2014/8/19)
* ルーペの実装
* 画像の拡大時の補完を実装 (済み 2014/4/10)
 * NearestNeighbor (済み 2014/4/10)
 * Bilinear (済み 2014/4/10)
 * Bicubic (済み 2014/4/10)
* BicubicのSIMD化
* 重たい画像処理の並列処理化
* フィルタの実装
 * アンシャープ
 * アンチエイリアス
 * 非線形フィルタ
 * 二値化(RGB) (判別分析法、モード法、3σ法、Kittler法、p-tile法)
 * グレイスケール
 * 平均化
* ZIPなどの圧縮された形式を扱えるようにする (済み 2016/5/30)
 * 暗号化されたファイルへの対応
 * エントリ名のエンコーディング対応
* アプリケーショのアイコンを追加する
* 見開き表示に対応する (済み 2016/2/28)
* 画像ファイルのプリフェッチ (済み 2016/5/18)
* 背景色指定
* apngなどアニメーション対応

