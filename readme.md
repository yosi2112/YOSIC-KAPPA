# YOSIC-KAPPA

32ビットCPU、コンソール、4台のフロッピーディスク装置を再現するWindows x64用CLIエミュレータです。
全15命令、整数式とマクロを使うアセンブラ、raw/D88/KFD媒体、uPD765A FDC、IPLからの起動に対応します。

## ビルドと起動

Visual Studio 18のx64 Developer Command Promptで、リポジトリのルートから実行します。C++20、CMake、Ninjaが必要です。

```bat
chcp 65001
cmake -S src -B out/build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build out/build
out\build\yosic-sim.exe --rom out\build\boot\ipl.rom --disk 0 out\build\boot\boot.d88 --steps 1000000 --dump 0x400 1
```

`KAPPA IPL` → `IPL: BOOT` → `STAGE2 OK`と、診断側の`mem[0x400]=0x13572468`を確認します。
通常実行は命令上限なしです。この例では`--steps 1000000`で停止します。上限を省略した場合はCtrl+Cで停止できます。

## 実行用ZIP

```bat
cpack --config out/build/CPackConfig.cmake -B out/packages
```

ZIPを空のフォルダへ展開し、そのルートから実行します。Visual StudioやPythonは不要です。

```bat
bin\yosic-sim.exe --rom boot\ipl.rom --disk 0 boot\boot.d88 --steps 1000000 --dump 0x400 1
```

媒体は既定で読み取り専用です。PowerShellでは実行ファイルの前に`.\`を付けます。

## マニュアル

- [エミュレータ](doc/emulator.md)：起動、入力、ディスク、ビルド、CLI一覧
- [アセンブラ](doc/assembler.md)：文法、式、マクロ、INCLUDE/BINCLUDE、リスティング
- [アーキテクチャ](doc/architecture-reference.md)：レジスタ、命令、メモリ、仮想時刻
- [インターフェース](doc/interface-reference.md)：コンソール、FDC、媒体形式、IPL契約

ライセンスは[MIT](license)です。
