
[![Build and Test](https://github.com/Synesthesias/libplateau/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/Synesthesias/libplateau/actions/workflows/build-and-test.yml)

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

## ビルド
ビルドの方法について、全OSで共通の留意点を記したあと、  
OSごとのビルド方法を記載する。

### 共通
- C++ の libplateau_c をビルドすると DLL ができる。
- その後 C# の LibPLATEAU.NET をビルドすると自動で上述のDLLがコピーされ、C#側で利用可能になる。
- C++とC#のビルド設定を合わせる必要がある。(C++でRelease 設定でビルドしたなら C# も Release、Debug なら C# も Debug。これを間違うと古いDLLがコピーされてしまう。）

### 3つのOS向けにまとめてビルド
- Github Actions で```Upload Dlls``` というワークフローを手動実行すると、  
  CIの成果として3つのOS向けにビルドしたライブラリをまとめてダウンロードできる。

### Windowsでの手動ビルド
#### C++のビルド
* Visual Studioのローカルフォルダーを開くからcloneしたリポジトリを開く。
* 一度cmakeこけるので再度cmakeする。(CMakeLists.txt開いてCtrl+S)
* ビルド実行する。(Ctrl+Shift+B)
* `plateau_test`を実行することでユニットテストを実行可能
#### C#のビルド
* ```wrappers/csharp/LibPLATEAU.NET.sln``` を開く。
* ビルドする。ただしC++側に変更があった場合、  
  最新のDLLをC#側にコピーするため「ビルド」ではなく「リビルド」を選択する。
* C#ユニットテストも合わせて実行可能。

### Linuxでの手動ビルド
利用する Linux は、Unityの対応OSに合わせて Ubuntu 18 とする。  
それより新しいバージョンのUbuntuでビルドすると、Ubuntu 18 には存在しないライブラリに依存してしまうのでUnityで実行不能となってしまう。
#### C++のビルド
* Ubuntuに入っているデフォルトの cmake ではバージョンが古い可能性がある。  
  その場合は新しいcmakeをマシンにインストールする。
* Ubuntu 18 のデフォルトのコンパイラは g++-7 となっているが、それでは古いので g++-9 を導入する。
* OpenGL API が必要なので、なければ以下のコマンドでインストールする。
```
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev
```

* 以下のコマンドを実行する。
```
cd (プロジェクトのルートディレクトリ)
cmake -S . -B ./out/build/x64-Release/ -G "Ninja" -D CMAKE_BUILD_TYPE:STRING="RelWithDebInfo" -D CMAKE_INSTALL_PROGRAM="ninja" -D CMAKE_CXX_FLAGS="-w"
cmake --build ./out/build/x64-Release/ --config RelWithDebInfo
```
#### C#のビルド
* マシンに dotnet をインストールする。
* 以下のコマンドを実行する。
```
cd (プロジェクトのルートディレクトリ）
cd ./wrappers/csharp/LibPLATEAU.NET
dotnet build -c Release
```
* 合わせてユニットテストもする場合は以下を実行する。
```
dotnet test -c Release
```

## サンプル
### log_skipped_elements
パース出来なかった要素をすべて列挙してくれる。
Visual Studioの実行ターゲットを`log_skipped_elements.exe`にして実行する。

### export_obj
.obj, .mtlをエクスポートしてくれる。

## デプロイ
### Unity
PlateauUnitySDKへの導入については、そちらのREADMEを参照のこと。

## ディレクトリ構成
- 3rdparty
  - 外部ライブラリはすべてここにsubmoduleで追加する。
  - libcitygml
    - citygmlパースライブラリ。変更するため本家をフォークしている。
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
- test
  - ユニットテスト
- wrappers
  - 他言語向けのwrapper実装を置く。
- .github/workflows
  - Github Actions で自動テストを行うための手順を記載する。

## テストデータ
テストデータの詳細については ```data/README.md``` に記載している。

# 実装上の注意
## 文字コード
- gmlのパース結果はC++の内部では UTF-8 で保持しています。
  - パーサー xerces-c は本家の挙動ではAnsiに変換して保持しますが、その挙動をUTF-8に変えました。Unityから日本語文字を扱う都合上の改変です。

## CI (継続的インテグレーション)
Github Actions によるCIを導入しています。  
Windows, Mac, Linux でのテストと成果物のダウンロードができます。
- push時、自動でビルドおよびユニットテストが行われます。
- git tagを付けた時、またはgithubサイトから手動で Upload DLLs を実行したときにビルドが走り、 
  成果物となるDLL等を3つのOS向けにダウンロードできます。
  githubサイトから手動で実行するには、 Actions → Workflows から Upload DLLs を選択 → Run workflow からブランチを選んで実行します。
  成果物を ```all-library``` という名前のzipでダウンロードできます。

# コード規約
- 変数名は snake_case , 関数名は lowerCamel
- private static メソッドはヘッダファイルに書かず、.cppの無名名前空間に書く
- 外に見せる必要の無いクラス (C#でいう internal クラス) のヘッダーファイルの配置ディレクトリは include ではなく src 
- `[[nodiscard]]` は書かない


# ライセンス
- libplateau本体： 未定
- xerces-c： Apacheライセンス , 要 著作権表示 , 要 改変したことの告知
- libcitygml：  GNU LESSER GENERAL PUBLIC LICENSE
