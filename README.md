# LibPLATEAU

- クロスプラットフォーム(iOS, android, windows, ubuntu)
- C++, C#, python向けapiの提供

## 開発環境
- CMake 3.8以降

### Windows
- Visual Studio 2019

## インストール
### リポジトリクローン
```
git clone https://github.com/Synesthesias/libplateau
cd libplateau
git submodule update --init --recursive
```

### Windows
Visual Studioのローカルフォルダーを開くからcloneしたリポジトリを開く。

一度cmakeこけるので再度cmakeする。(CMakeLists.txt開いてCtrl+S)

ビルド実行する。(Ctrl+Shift+B)

## サンプル
### log_skipped_elements
取り合えず作ったサンプル。Visual Studioの実行ターゲットを`log_skipped_elements.exe`にして実行する。

パース出来なかった要素をすべて列挙してくれる。

## ディレクトリ構成
- 3rdparty
  - 外部ライブラリはすべてここにsubmoduleで追加する。
  - libcitygml
    - citygmlパースライブラリ。恐らく変更するため本家をフォークしている。
  - xerces-c
    - libcitygmlの依存ライブラリ。xmlパースライブラリ
- data
  - テスト用のデータを置く。ビルド時に出力先ディレクトリにコピーされる。
- examples
  - サンプルを置く。
- include
  - 外部向けヘッダを置く。
- src
  - 内部実装のソースコードを置く。
- wrappers
  - 他言語向けのwrapper実装を置く。
