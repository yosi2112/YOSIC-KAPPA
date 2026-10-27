# YOSIC-KAPPA

32ビットCPU、コンソール、4台のフロッピーディスク装置を再現するWindows x64用エミュレータです。CLIとハードウェアモニタ・デバッガを備えます。
全15命令、整数式とマクロを使うアセンブラ、raw/D88/KFD媒体、uPD765A FDC、IPLからの起動に対応します。

## ビルドと起動

Visual Studioのx64 Developer Command Promptで、リポジトリのルートから実行します。C++20、CMake、Ninja、Python 3が必要です。

```bat
chcp 65001
cmake -S src -B out/build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build out/build
out\build\yosic-sim.exe --rom out\build\boot\ipl.rom --disk 0 out\build\boot\boot.d88 --steps 1000000
```

実端末では最初にモニタの`-`プロンプトが表示されます。`G`を入力するとゲストを実行します。直接起動する場合は`--no-monitor`を追加してください。

起動すると`YOSIC-KAPPA INITIAL PROGRAM LOADER V1.0` → `IPL: BOOT` → `STAGE2 OK`と表示されます。
通常実行は命令上限なしです。この例では`--steps 1000000`で停止します。上限を省略した場合はCtrl+Cで停止できます。

## 実行用フォルダ

```bat
cmake --install out/build --prefix out/portable
```

実行ファイル、起動媒体、マニュアルを`out/portable`へ配置します。そのフォルダから実行します。実行時にはVisual StudioやPythonは不要です。

```bat
cd out\portable
bin\yosic-sim.exe --rom boot\ipl.rom --disk 0 boot\boot.d88 --steps 1000000
```

媒体は既定で読み取り専用です。PowerShellでは実行ファイルの前に`.\`を付けます。

## マニュアル

- [エミュレータ](doc/emulator.md)：起動、入力、ディスク、ビルド、CLI一覧
- [ハードウェアモニタ・デバッガ](doc/monitor.md)：状態表示、SYMDEB形式の操作、レジスタ・メモリ・ブレークポイント
- [アセンブラ](doc/assembler.md)：文法、式、マクロ、INCLUDE/BINCLUDE、リスティング
- [アーキテクチャ](doc/architecture-reference.md)：レジスタ、命令、メモリ、仮想時刻
- [インターフェース](doc/interface-reference.md)：コンソール、FDC、媒体形式、IPL契約
- [YOSIC-DOS 1.0K](doc/dos/README.md)：コマンド、外部アプリ、システムコール、ファイルハンドル
- [ビルド・開発用マニュアル](doc/build.md)：ソースからのビルドと開発作業

ライセンスは[MIT](license)です。

## DOSシステム媒体

通常ビルドは`out/build/dos`へ、実行用フォルダへの配置は`dos`へ、1.44 MBと1.25 MBのDOSシステム媒体を用意します。FORMAT、DISKCOPY、SUBMIT、DULL、LEDTの5コマンドを収録しています。

```bat
bin\yosic-sim.exe --rom boot\ipl.rom --disk 0 dos\dos.d88 --interactive --no-monitor
```

書込み結果を保持するには、媒体の作業用コピーを用意し、`--disk-copy 0 <保存先.d88>`を指定します。編集から一括コンパイル・実行までの手順は[システム媒体と連携操作](doc/dos/system-media.md)、言語仕様は[PL/DULL説明書](doc/dos/pldull.md)を参照してください。
