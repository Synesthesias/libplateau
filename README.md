# LibPLATEAU

- クロスプラットフォーム(iOS, android, windows, ubuntu)
- C++, C#, python向けapiの提供

## 開発環境
- CMake 3.8以降

### Windows
- Visual Studio 2022
  - cmake使うためVisual Studio Installerから`C++によるデスクトップ開発`のインストールが必要

## セットアップ
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

### export_obj
.obj, .mtlをエクスポートしてくれる。

## デプロイ
### Unity
1. libplateauビルド(Release)
2. wrappers/csharp/LibPLATEAU.NET/LibPLATEAU.NET.slnを開きReleaseでビルド
3. 以下のファイルをUnityプロジェクトにコピー
   1. wrappers/csharp/LibPLATEAU.NET/LibPLATEAU.NET/bin/Release/net6.0/LibPLATEAU.NET.dll
   2. out/build/x64-Release/bin/citygml.dll
   3. out/build/x64-Release/bin/plateau.dll
   4. out/build/x64-Release/bin/plateau_c.dll

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
  - ヘッダファイル一式を置く。
- src
  - 内部実装のソースコードを置く。
- wrappers
  - 他言語向けのwrapper実装を置く。
