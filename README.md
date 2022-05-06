
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
- C++を Release 設定でビルドしたなら C# も Release 設定でビルドする必要がある。  
- Debug設定 なら C# も Debug にする。これを間違うと古いDLLがコピーされてしまう。
- ビルドの成果物（DLL）は後述のデプロイで利用可能。

### Windowsでのビルド
#### C++のビルド
* Visual Studioのローカルフォルダーを開くからcloneしたリポジトリを開く。
* 一度cmakeこけるので再度cmakeする。(CMakeLists.txt開いてCtrl+S)
* ビルド実行する。(Ctrl+Shift+B)
#### C#のビルド
* ```wrappers/csharp/LibPLATEAU.NET.sln``` を開く。
* ビルドする。ただしC++側に変更があった場合、  
  最新のDLLをC#側にコピーするため「ビルド」ではなく「リビルド」を選択する。
* ユニットテストも合わせて実行可能。

### Linuxでのビルド
#### C++のビルド
* Ubuntuに入っているデフォルトの cmake などではバージョンが古い可能性がある。  
  その場合は新しいcmakeをマシンにインストールする。
* OpenGL API が必要なので、なければ以下のコマンドでインストールする。
```
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev
```

* 以下のコマンドを実行する。
```
cd (プロジェクトのルートディレクトリ)
cmake -S . -B ./out/build/x64-Release/ -G "Ninja" -D CMAKE_BUILD_TYPE:STRING="RelWithDebInfo" -D CMAKE_INSTALL_PREFIX:PATH="./out/install/x64-Release" -D CMAKE_INSTALL_PROGRAM="ninja" -D CMAKE_CXX_FLAGS="-w"
cmake --build ./out/build/x64-Release/ --config RelWithDebInfo
```
#### C#のビルド
* マシンに dotnet 6.0以上 をインストールする。
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
- wrappers
  - 他言語向けのwrapper実装を置く。
- .github/workflows
  - Github Actions で自動テストを行うための手順を記載する。

## テストデータ
テストデータの詳細については ```data/README.md``` を参照してください。