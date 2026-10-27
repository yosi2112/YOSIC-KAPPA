# ビルド・開発用マニュアル

本書はソースコードからYOSIC-KAPPAとYOSIC-DOS 1.0Kをビルドする人向けです。ソース構成、ビルド手順、テスト、品質確認をまとめています。操作方法は[エミュレータ](emulator.md)、[DOS説明書](dos/manual.md)を参照してください。

## 1. 開発環境と作業場所

Windows x64、C++20対応MSVC、CMake 3.25以上、Ninja、Python 3が必要です。Visual StudioのC++開発環境を推奨します。DOS媒体の生成とPythonテストにはPython 3も必要です。外部C++ライブラリは使用しません。

以下は`src`、`doc`、`readme.md`があるソース一式のルートで実行します。「Visual Studioのx64 Developer Command Prompt」を開いてください。`cl`、`cmake`、`ninja`が見つからない場合は、VSのC++ツールとCMakeツール、開発環境のPATHを確認します。

日本語版MSVCでは構成前に`chcp 65001`を実行します。以前のコードページで作ったキャッシュが問題になる場合は、新しいビルド先か`cmake --fresh`を使用します。

## 2. エミュレータのビルドと配置

```bat
chcp 65001
cmake -S src -B out/build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build out/build
```

`out/build`に`yosic-sim.exe`、`yosic-as.exe`、`yosic-mkimage.exe`を出力します。通常ビルドで`boot-images`と`dos-images`も作り、`out/build/boot`にIPL ROM、stage2、起動媒体、リスティング、マップを配置します。Debug構成は`-DCMAKE_BUILD_TYPE=Debug`で選択します。

実行用フォルダを作成する場合は次を実行します。

```bat
cmake --install out/build --prefix out/portable
```

`bin`に実行ファイル、`boot`にIPL診断媒体、`dos`に5コマンド入りDOSシステム媒体、`doc`にマニュアル、`src`にファームウェア・実行例・DOSソースを配置します。配置ファイルのSHA-256一覧は`SHA256SUMS.txt`です。構成は[`install.cmake`](../src/tools/distribution/install.cmake)で定義します。

## 3. IPLとstage2の起動確認

```bat
out\build\yosic-sim.exe --rom out\build\boot\ipl.rom --disk 0 out\build\boot\boot.d88 --steps 1000000 --dump 0x400 1
```

`KAPPA IPL`、`IPL: BOOT`、`STAGE2 OK`と、診断出力`mem[0x400]=0x13572468`を確認します。stage2の末尾はループなので、この例の`stop=limit`は正常です。

## 4. DOSのビルドとYKE作成

エミュレータとアセンブラをビルドした後、PowerShellから実行します。`python`がPATH上に必要です。

```powershell
.\src\dos\tools\build.ps1
.\out\build\yosic-sim.exe --rom out/build/boot/ipl.rom --disk 0 out/dos/dos.d88 --interactive
```

`out/dos`にDOS本体、ローダー、マップ、raw媒体、D88媒体を生成します。ビルドスクリプトはINCLUDEを展開してアセンブラへ渡し、本体のサイズ・整列・チェックサムからローダー用定数を作ります。媒体生成ではKPB1ヘッダ、チェックサム、本体配置、D88セクタを確認します。

YKE実行例を作成する場合は次を実行します。

```bat
out\build\yosic-as.exe src\dos\examples\file.asm -o out\dos\file.bin
python src\dos\tools\make_yke.py out\dos\file.bin out\dos\FILE.YKE
```

YKEファイルはFAT12媒体へ配置してDOSから起動します。形式とABIは[DOS説明書](dos/manual.md)を参照してください。

## 5. DOSのソース構成

| ソース | 責務 |
|---|---|
| [dos.asm](../src/dos/dos.asm) | 入力・プロンプト・内蔵コマンドの振分け |
| [loader.asm](../src/dos/loader.asm) | IPLからDOS本体へのロード |
| [exec.inc](../src/dos/exec.inc) | YKE1/.YKEの確認、FDC読込み、起動・終了 |
| [syscall.inc](../src/dos/syscall.inc) | R1/JMPシステムコールの振分け、引数・復帰位置の確認 |
| [file.inc](../src/dos/file.inc) | ハンドルの管理、OPEN/CLOSE/READ/WRITE、FAT12への反映 |
| [api.inc](../src/dos/api.inc) | アプリ用の番号定数・定数設定・呼出しマクロ |
| [media.inc](../src/dos/media.inc) | 媒体世代の確認、FAT・現在位置・ハンドルの更新 |
| [chdir.inc](../src/dos/chdir.inc) | CHDIR、ドライブごとの現在位置、移動対象の保護 |
| [core.inc](../src/dos/core.inc) | ファイルシステムと内蔵コマンドの正本アセンブリ |
| [core.py](../src/dos/core.py) | ファイルシステム処理の整数Pythonソースモデル |
| [runtime.inc](../src/dos/runtime.inc) | ネイティブ関数呼出しと定数展開 |
| [blockio.inc](../src/dos/blockio.inc) | FDCセクタ転送 |
| [arithmetic.inc](../src/dos/arithmetic.inc)、[answer.inc](../src/dos/answer.inc) | 算術と確認入力 |
| [make_yke.py](../src/dos/tools/make_yke.py) | YKE1ヘッダとチェックサムの作成 |
| [hello.asm](../src/dos/examples/hello.asm) | 表示・レジスタ変更・終了コード7でのEXIT |
| [syscall.asm](../src/dos/examples/syscall.asm) | システムコールによる表示・終了 |
| [file.asm](../src/dos/examples/file.asm) | ハンドルAPIによるバイナリファイル作成 |

`core.inc`は独立したソース入力として保持します。Pythonモデルからのコンパイラは含まれておらず、Pythonはゲスト実行時には使いません。YKEローダーの正本は`exec.inc`で、システムコールとハンドル処理もゲストの命令として実行します。

## 6. DOSのテスト

DOSをビルドした後、ソース一式のルートで実行します。公開ソースへ`__pycache__`を作らないよう、Pythonの`-B`オプションを付けます。

```bat
python -B src\dos\tests\verify-quality.py
python -B src\dos\tests\verify-drive.py
python -B src\dos\tests\verify-chdir.py
python -B src\dos\tests\verify-exec.py
python -B src\dos\tests\verify-commands.py
python -B src\dos\tests\verify-syscall.py
python -B src\dos\tests\verify-file.py
```

| テスト | 確認内容 |
|---|---|
| verify-quality.py | Python構文、INCLUDE、空白、YKE名称、文書リンク |
| verify-drive.py | 数値ドライブ選択、プロンプト、ヘルプ、不正指定、未接続媒体 |
| verify-chdir.py | ドライブごとの現在位置、相対パス、上限・不正入力、使用中ディレクトリ保護、相対YKE起動とハンドルI/O |
| verify-exec.py | 正常起動・EXIT、引数保持、分断チェーン、最大サイズ、入口、パス・ドライブ、ヘッダ・チェーン異常 |
| verify-commands.py | 内蔵9コマンド、コピー・移動・削除の保存、FATの2コピー、SYS転送後の再起動 |
| verify-syscall.py | コンソール・ドライブ・内蔵操作、レジスタ・スタック保持、ポインタ・復帰位置、入力とEOF、公開サンプル |
| verify-file.py | ハンドル読書き、バイナリ内容、ファイル位置、EOF、作成・切詰め・上書き、保存、解放・再利用、アクセス拒否、破損、容量不足 |

統合テストは実CPU/FDC上で実行します。入力、媒体、生成アプリ、保存媒体、ログは`out/dos`へ置き、公開ソースへ混在させません。`support.py`はDOSの`INPUT_DONE`をブレークポイントに設定し、単に命令上限で停止した状態を成功と扱わないようにしています。

## 7. モニタ・デバッガの検証

```bat
cmake --build out/build --target yosic-monitor-check yosic-media-check
out\build\yosic-monitor-check.exe
out\build\yosic-media-check.exe
python -B src\tools\tests\monitor-cli.py
python -B src\tools\tests\monitor-media.py
```

実CPU・バスを使い、SYMDEB形式の解析、レジスタ変更、バイト／16ビット／32ビットのメモリ操作、MMIOの非破壊読出し、逆アセンブル、T/P、ブレークポイントを確認します。検証実行ファイルは通常ビルドと配布対象には含めません。DOS検証は`--no-monitor`で実行します。`monitor-cli.py`は専用の実端末で、G/Tの連続入力、DOS入力、対象プロセスだけへのCtrl+Break、Qまでを確認し、GからDOSのプロンプトまでの実時間を`out/monitor-cli-result.txt`へ記録します。`yosic-media-check`はAT/DTの形式・引用符・保存・失敗時の維持・複数媒体D88と交換ロックを、`monitor-media.py`は実DOSでFAT・現在位置・ハンドル失効とロックを確認します。媒体・ログはout以下に生成します。手動ではCLIの`-`からGで起動し、Ctrl+Breakで戻り、R/T/G/Qと入力の振分けを確認します。

## 8. C++の品質確認と公開ソース確認

VSのx64開発環境で`chcp 65001`を実行した後、PowerShellから実行します。

```powershell
.\src\tools\check-quality.ps1
```

`clang-format --dry-run --Werror`による書式確認と、MSVC `/analyze`による静的解析を行います。clang-formatを明示する場合は`-ClangFormatPath`を指定します。解析ビルドの出力先は`out/analyze`です。

公開するGitツリーを確認するには、ソース一式のルートで次を実行します。

```powershell
.\src\tools\check-public-tree.ps1 -Root . -GitRef HEAD
```

公開ルートは`doc`、`src`、`readme.md`、`license`です。生成物・実行ファイル・ローカル設定・キャッシュは含めません。ソース一式を書き出すには`export-source.ps1 -Destination <新しいフォルダ>`を使用します。

ルートにローカル専用`tests/CMakeLists.txt`が存在する場合、CMakeの`BUILD_TESTING`でその回帰テストも組み込めます。公開ツリーにはこのディレクトリはなく、DOSのテストは上記のPythonスクリプトで実行します。

## 7. トランジェント共通基盤

`src/dos/transients`直下のコマンド用`.asm`をDOSと同じアセンブラで生成し、YKE1形式へ包装して両FD媒体のルートへ収録します。共通ライブラリの`.inc`だけが存在する場合、コマンドは追加されません。出力先は`out/dos/transients`です。媒体に入るのは今回のビルドで選ばれたソースのYKEだけです。

```powershell
.\src\dos\tools\build.ps1 -PythonPath C:\Python312\python.exe
```

`-PythonPath`で実行するPythonを、`-ToolDirectory`でアセンブラの場所を指定できます。既定ではPythonランチャーまたはPATHのPythonを使用します。`-TransientSourceDirectory`は別のコマンドソースディレクトリを指定する場合に使います。コマンド名は英数字と`_`の1〜8文字です。INCLUDEはソースを基準に再帰展開し、絶対パスにも対応します。

媒体生成はYKEの名前、重複、サイズ、整列、エントリ位置、ヘッダ、チェックサムを検査し、起動用の予約領域とKAPPA.SYSの後ろへFATチェーンを割り当てます。ルート項目数・媒体容量・FAT12上限を検査し、両媒体が構築できてからファイルを書き出します。追加YKEは`make-image.py <出力ディレクトリ> --transient <コマンド.YKE>`でも明示できます。

現行のYKEローダーは、PC-ATの512バイト物理セクタとPC-98の1024バイト物理セクタの双方で起動できます。DOS内部の512バイトセクタ番号とFATクラスタ番号は、ログイン時のクラスタ長に従って変換します。

| 共通ソース | 責務 |
|---|---|
| [common.inc](../src/dos/transients/common.inc) | アプリ内呼出し、既存システムコール、uint32、引数解析、入力編集、ファイルI/O |
| [fdc.inc](../src/dos/transients/fdc.inc) | 直接FDC操作、両FDの物理セクタ転送・READ ID・FORMAT、期限とidle復帰 |
| [block-device.inc](../src/dos/transients/block-device.inc) | 内部デバイス記述子、64ビット容量/LBA、物理/論理セクタ、FDバックエンド |
| [media-return.inc](../src/dos/media-return.inc) | 外部実行前後のBPB・ルートCSV確認と選択的なキャッシュ失効 |

アプリは最初のJMPの後に共通ライブラリをINCLUDEし、入口で`TrInit`を実行します。`TrCall`の呼出先は`tr_return`で復帰します。R6・R8〜R13とSPを保持し、R0〜R5・R7は作業用です。DS/SS/ESは0、R12は`0x88000`、スタックは`0xE0000`〜`0xEFFFF`とします。文字・転送データは1バイトを1語で表します。各関数の引数と上限はソース先頭および関数直前に記載しています。公開ABIは既存の番号だけを使用します。

内部block-device記述子は容量とLBAを下位/上位32ビットで保持し、物理セクタ長とDOSの512バイト論理セクタ長を分けます。現在のバックエンドはFDだけです。HDなど未対応のkindはMMIO操作前にエラー50を返します。FDCの停止は中断後のidle復帰であり、すでに行った書込みを元に戻す処理ではありません。

## 8. DOS媒体を含む配布

通常のCMakeビルドは`out/build/dos`に両FDプロファイルのRAW/D88を生成します。`dos-images`は毎回ソースから5コマンドを組み直し、古いYKE出力を収録しません。Pythonの場所を明示する場合は、CMake構成時に`-DPython3_EXECUTABLE=C:/Python312/python.exe`を指定します。DOS用PowerShellの呼出しに限りプロセス単位で実行ポリシーを設定します。

単独のDOSビルドは従来どおり`out/dos`を使用します。`-OutputDirectory <出力先>`で別の場所へ生成できます。CMakeはこの指定で構成ごとの出力先を分けます。インストール前に通常ビルドを完了してください。必要な媒体が欠けている場合、インストールは失敗します。

実行用フォルダには`dos/dos.d88`、`dos/dos.img`、`dos/dos-1.25.d88`、`dos/dos-1.25.img`を収録します。各媒体はFORMAT、DISKCOPY、SUBMIT、DULL、LEDTを含みます。使用例とSYSの役割は[システム媒体と連携操作](dos/system-media.md)を参照してください。配布後のDOS実行やDULLコンパイルにホストのPythonは不要です。

ソース書出しと実行用フォルダへのソース配置では、ローカル計画、再開記録、テスト、Agents.md、生成物、原典調査用のPLDULLフォルダを除外します。`export-source.ps1`は作業中の製品ソースを新規フォルダへ書き出し、公開対象検査を行います。公開Git候補の検査も、生成媒体やこれらの私用ファイルの混入を拒否します。
