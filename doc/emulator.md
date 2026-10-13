# エミュレータマニュアル

YOSIC-KAPPAは、32ビットCPUとフロッピーディスク装置を再現するWindows x64用のプログラムです。
画面上のボタンではなく、コマンドを入力して使います。CPU命令、コンソール、4台のドライブ、uPD765A FDC、IPLからの起動に対応しています。
FDCはフロッピーディスクを制御する装置です。IPLは起動時に最初に動き、ディスクから次のプログラム（stage2）を読み込みます。

この本は起動とファイルの扱いを説明します。プログラムを書くときは[アセンブラ](assembler.md)、CPUの決まりは[アーキテクチャ](architecture-reference.md)、機器と媒体の形式は[インターフェース](interface-reference.md)を参照してください。

## 1. 実行用フォルダから起動する

実行用フォルダを用意します。ソースからの作成方法は[ビルド・開発用マニュアル](build.md)を参照してください。Windowsのコマンドプロンプト（cmd）を開き、`out/portable`へ移動してください。以下の操作例は、特記がない限り**実行用フォルダのルートでcmdから実行**します。空白を含むファイル名は二重引用符で囲みます。
実行にはVisual StudioやPythonは不要です。PowerShellでexeを直接実行する場合は、`bin\`の前に`.\`を付けます。バイナリの入出力例はcmdで実行してください。

```bat
bin\yosic-sim.exe --rom boot\ipl.rom --disk 0 boot\boot.d88 --steps 1000000
```

実端末では最初にモニタの`-`プロンプトが出ます。`G`を入力するとゲストが動き始めます。次の3行が出れば、ディスクからstage2まで進んでいます。

```text
KAPPA IPL
IPL: BOOT
STAGE2 OK
```

`stop=limit`は明示した`--steps`の命令数まで実行したという意味です。サンプルは最後に同じ命令を繰り返すため、この表示は正常です。通常実行には命令上限がなく、`--steps`を省略するとCtrl+Cまたはブレークポイントで止めるまで動き続けます。CPUや入出力の異常でも停止します。

## 2. ファイルを見分ける

| 保存場所・拡張子 | 内容 |
|---|---|
| `bin` | `yosic-sim.exe`（実行）、`yosic-as.exe`（組立て）、`yosic-mkimage.exe`（起動媒体の作成） |
| `boot` | 生成済みのIPL ROM、stage2、起動媒体、リスティング、シンボルマップ |
| `src/firmware` / `src/examples` | IPL、stage2、練習用プログラムのアセンブリソース |
| `doc` | この4冊のマニュアル |
| `.asm` | 人が書くプログラム |
| `.rom` / `.bin` | CPUが読む命令やデータ。拡張子による内容検査はしない |
| `.img` / `.d88` / `.kfd` | ディスクイメージ。実行時はこの拡張子で形式を選ぶ |
| `.lst` / `.map` | 生成byteとアドレス／記号の値を確認するテキスト |

ROMは4～65536バイトで、4バイトの倍数にします。ROMの物理先頭は`0x00FF0000`です。短いROMの残りは0になります。
RAMは起動のたびに0になります。RAMやレジスタは、終了後のファイルに自動保存されません。
`SHA256SUMS.txt`には同梱ファイルのハッシュ値（内容を確認する値）があります。一覧自身は対象外です。

## 3. 少しだけ実行して調べる

新しい作業フォルダ`work`を作ります。以下の例はこのフォルダを使い続けます。

```bat
mkdir work
```

```bat
bin\yosic-as.exe src\examples\count.asm -o work\count.rom
bin\yosic-sim.exe --rom work\count.rom --steps 4 --registers
```

`R1=0x7`を確認します。`--steps 4`は4命令まで、`--step`は1命令だけ実行します。実行するたびにリセット状態から始まります。

```bat
bin\yosic-sim.exe --rom work\count.rom --step --registers --dump 0x00FF0000 1
bin\yosic-sim.exe --rom work\count.rom --break 0x00FF0004
```

最初の実行は`R1=0x5`、`mem[0xff0000]=0x442f0005`を表示します。次の実行は`stop=breakpoint steps=1`になります。
ブレークポイントは物理アドレスです。その場所の命令を実行する**前**に止まります。
`--dump`の最後の数値はワード数です。1ワードは4バイトです。機器の入力を消費しないpeekで読みます。
`--registers`の`SEG0`～`SEG3`は順にCS、DS、SS、ESです。

## 4. コンソールへ入力する

ゲストはエミュレータの中で動くプログラム、ホストはWindows上の実行プログラムです。
ゲストの出力は標準出力（stdout）、診断やトレースは標準エラー出力（stderr）へ出ます。

次の例は入力をそのまま返します。cmdの`>`は既存ファイルを上書きするので、ここでは新しい`work`を使います。

```bat
echo Hello>work\input.bin
bin\yosic-as.exe src\examples\console\echo.asm -o work\echo.rom
bin\yosic-sim.exe --rom work\echo.rom --input work\input.bin --steps 10000 > work\output.bin
fc /b work\input.bin work\output.bin
```

`fc /b`で相違がなければ成功です。入力には行末の改行も含まれます。
`--input`は最大16 MiBを先に読み込みます。EOF（入力の終わり）になっても、残ったバイトはゲストから読めます。
入力指定がなければ、最初からEOFです。echoの例はEOFの後に待機ループへ入り、上の例では明示した`--steps 10000`でホストが終了します。上限を指定しない実行はEOFでも自動終了しません。

実端末で手入力する場合だけ、次を使います。この操作は人がキーを押す必要があります。

```bat
bin\yosic-sim.exe --rom work\echo.rom --interactive
```

Ctrl+ZはEOF、Ctrl+Cは実行停止です。`--input`と`--interactive`は併用できません。
端末の文字コードは変換しません。バイト列を正確に確認する場合はファイル入力を使います。

## 5. ディスクを接続する

`--disk UNIT FILE`で接続します。UNITは0～3です。IPLはドライブ0を使います。

```bat
bin\yosic-sim.exe --rom boot\ipl.rom --disk 0 boot\boot.d88 --disk 1 boot\boot.d88 --disk 2 boot\boot.d88 --disk 3 boot\boot.d88 --steps 1000000 --dump 0x400 1
```

同じファイルを4台へ読み取り専用で接続し、ドライブ0から起動します。別のドライブを読むには、ゲストがFDCコマンドで番号を選びます。

raw（`.img`）には形状情報がないため、`--geometry C H S N`を付けます。Cはシリンダ数、Hは面数、Sは1トラックのセクタ数です。1セクタのバイト数は`128 << N`です。
C=1～82、H=1～2、S=1～255、N=0～6を指定します。geometryは同じ実行内のrawと新規媒体で共通です。rawへのコピー保存にも必要です。

```bat
bin\yosic-sim.exe --rom boot\ipl.rom --disk 0 boot\boot.img --geometry 80 2 18 2 --steps 1000000 --dump 0x400 1
```

これも`STAGE2 OK`を表示します。D88とKFDには媒体情報があるため、読込みだけならgeometryは不要です。
複数枚を連結したD88では、`--disk-index UNIT INDEX`で使う1枚を選びます。INDEXは0始まり、既定は0です。

```bat
copy /b boot\boot.d88+boot\boot.d88 work\pair.d88
bin\yosic-sim.exe --rom boot\ipl.rom --disk 0 work\pair.d88 --disk-index 0 1 --steps 1000000 --dump 0x400 1
```

この例は2枚を連結して2枚目から起動します。コピー保存時は、選択しなかったディスクも保持します。

## 6. 書込みとコピー保存

通常の媒体は読み取り専用（ゲストから書き込めない設定）です。書くときは`--disk-copy UNIT NEWFILE`で新しい保存先を指定します。
原本は変更しません。D88ヘッダの書込み保護がある媒体には、この指定を付けても書けません。

次の例は起動媒体の空きセクタC0/H0/R18へ`0x41`を512バイト書きます。ヘッダR1とプログラムR2～17を保つ例です。
保存先`work\written.d88`はまだ存在しない名前にしてください。

```bat
bin\yosic-as.exe src\examples\fdc\write-boot-spare.asm -o work\write.rom
bin\yosic-sim.exe --rom work\write.rom --disk 0 boot\boot.d88 --disk-copy 0 work\written.d88 --steps 500000
bin\yosic-sim.exe --rom boot\ipl.rom --disk 0 work\written.d88 --steps 1000000 --dump 0x400 1
```

保存したコピーでも`STAGE2 OK`を確認します。書込みサンプルは正常媒体を前提とし、IPLほど詳しい異常診断は備えていません。
保存は命令上限、ブレークポイント、Ctrl+Cによる通常停止時に行います。CPU fault（実行を続けられない異常）や、保存へ進む前のI/O失敗では保存しません。
既存の出力や重複する出力パスは拒否します。全ドライブの形式変換を検査してからファイルを作ります。ただし、保存中にホストのI/Oが失敗すると、先に保存できたファイルが残る場合があります。
raw/D88へ情報を失わず変換できない媒体には`.kfd`を使います。[保存制限](interface-reference.md#媒体形式)も確認してください。

空の媒体を作る場合は`--disk-new`を使います。次の例はドライブ0に空の媒体を作って保存します。起動プログラムは含まれません。

```bat
bin\yosic-sim.exe --rom work\count.rom --step --disk-new 0 --geometry 80 2 18 2 --disk-copy 0 work\blank.d88
```

## 7. IPLとstage2を作り直す

同梱ソースからIPLとstage2を組み立て、起動用D88を作れます。作業場所は引き続き実行用フォルダのルートです。

```bat
bin\yosic-as.exe src\firmware\ipl\ipl.asm -o work\ipl.rom --list work\ipl.lst --map work\ipl.map
bin\yosic-as.exe src\firmware\stage2\stage2.asm -o work\stage2.bin
bin\yosic-mkimage.exe work\stage2.bin -o work\boot.d88
bin\yosic-sim.exe --rom work\ipl.rom --disk 0 work\boot.d88 --steps 1000000 --dump 0x400 1
```

元と同じ3行とRAMの値が出ます。自作stage2に置き換える場合も、先頭命令をRAM`0x2000`で実行できるようにします。
payload（読み込むプログラム本体）は4～8192バイト、4バイトの倍数です。大きいstage2を有限実行で確認する場合は`--steps 6000000`を指定してください。通常実行では`--steps`を省略し、Ctrl+Cで停止します。
引渡し状態やヘッダは[IPL契約](interface-reference.md#ipl契約)を参照してください。

`yosic-mkimage INPUT -o OUTPUT [--overwrite]`は`.img`と`.d88`を作れます。既存出力は拒否します。
意図して作り直す場合だけ`--overwrite`を付けます。入力と同じファイル、別名のhardlink、大小文字だけ違う同じパスは拒否します。
不正なサイズなどは出力を開く前に検査します。書込み途中のI/O失敗まで、元の出力を保証する機能ではありません。
終了コードは成功0、引数・入力・出力のエラー2です。`--help`は単独で使用します。

## 8. トレースと停止理由

トレースは、実行した命令や機器へのアクセスの記録です。cmdの`2>`で診断だけをファイルへ保存できます。

```bat
bin\yosic-sim.exe --rom work\count.rom --steps 4 --trace 2>work\cpu-trace.txt
bin\yosic-sim.exe --rom boot\ipl.rom --disk 0 boot\boot.d88 --steps 1000000 --trace-fdc 2>work\fdc-trace.txt
```

CPUの`time_ns`は命令実行後の仮想時刻です。正常な1命令で1000 ns（1 µs）進みます。
FDCトレースはDATA/DACK/CONTROLのアクセスを記録します。MSRの待機読出しは省きます。
FDCの`time_ns`と`phase`はアクセス時刻とアクセス前の段階です。phaseは0=idle、1=command、2=execution、3=resultです。`msr`と`signals`はアクセス後の値です。
固定のROM、媒体、ファイル入力、命令上限なら、同じ仮想時刻と結果になります。端末操作やCtrl+Cの到着時刻は人の操作で変わります。

| 表示・終了コード | 意味と次の確認 |
|---|---|
| `stop=limit` / 0 | 命令上限。必要なら`--steps`を増やす |
| `stop=breakpoint` / 0 | 指定したアドレスで停止 |
| `stop=monitor-quit` / 0 | モニタのQで終了 |
| `stop=interrupted` / 0 | Ctrl+Cなどの停止要求を命令境界で受理 |
| `error:` / 2 | 引数が不正。オプション、形状、保存先を確認 |
| `fault=` または実行中の`error:` / 3 | ROM・入出力・CPU・バスの異常。診断のアドレスと命令語を確認 |

ゲストのIPLがエラーを表示して待機していても、ホストが命令上限で終了するとコードは0です。終了コードだけで起動成功とは判断せず、`STAGE2 OK`を確認してください。

## 9. 困ったとき

| 状況 | 確認すること |
|---|---|
| 実行が終わらない | 通常実行は上限なし。Ctrl+Cで停止するか、`--steps N`または`--break ADDRESS`を指定する |
| `cannot open ROM` | 作業場所と`--rom`のファイルが合っているか |
| `invalid ROM size` | 4～65536バイト、4の倍数になっているか |
| `raw/new disk requires --geometry` | C/H/S/Nを付けたか。raw保存時も必要 |
| `disk output exists` | 新しいコピー名を選ぶ。原本を保存先にしない |
| `fdc-handshake` | ゲストがMSRのRQM/DIOと転送モードを確認したか |
| `illegal-instruction` | 命令以外のデータへ分岐していないか。予約ビットやレジスタ番号も確認 |
| IPLの`NO DRIVE` | ドライブ0に媒体を接続したか |
| IPLの`BAD HEADER` / `CHECKSUM` | `yosic-mkimage`で正しいstage2から媒体を作り直す |
| IPLの`FDC ERROR` / `TIMEOUT` | セクタ、CRC、密度、速度がIPLの条件に合うか |
| 文字が化ける | 端末の文字コードとゲストのバイト列を確認。ファイルへ出して比較する |

faultでは失敗した命令のCPUレジスタ・PC・FLAGSを更新しません。媒体の途中書込みなど、すでに受理された機器の動作とは区別してください。

## 10. ハードウェアモニタとデバッグ

実端末からの起動時は、同じコンソールにハードウェアモニタを表示して停止します。SYMDEB形式の`-`プロンプトで`G`を入力すると、ゲストが動き始めます。ゲスト実行中のCtrl+Breakでモニタへ戻り、`R`でレジスタ表示、`T`で1命令実行、`?`でヘルプ、`Q`でエミュレータを終了します。[ハードウェアモニタ・デバッガ](monitor.md)に詳しい書式をまとめています。

直接ゲストを起動する場合は`--no-monitor`を指定します。入力ファイルやパイプからの実行では、既定でモニタを使いません。モニタ利用中のCPU・バスのfaultは、その位置で一時停止します。

## CLI一覧

実行時は`--rom`が必須です。数値のうち命令数・アドレスは10進か`0x`付き16進、ディスク番号・index・geometryは10進です。負数や範囲外は拒否します。

| オプション | 意味・範囲 |
|---|---|
| `--help` / `--version` | 単独で使う。引数なしでもヘルプを表示 |
| `--monitor` / `--no-monitor` | CLIモニタを使う／使わない。実端末では既定で使用し、Gまで停止。明示した--monitorには実端末が必要 |
| `--rom FILE` | ROMを読み込む |
| `--steps N` / `--step` | 明示する最大命令数（0は実行なし）／1命令。省略時は上限なし。両方は指定せず一方を選ぶ |
| `--break ADDRESS` | 4バイト整列した32ビット物理アドレスで停止 |
| `--registers` | 停止時のレジスタを表示 |
| `--dump ADDRESS WORDS` | 整列した場所から0～4096ワードを表示。32ビットアドレス末端を越えない |
| `--trace` / `--trace-fdc` | CPU／FDCアクセスの記録。併用可 |
| `--input FILE` / `--interactive` | 最大16 MiBの入力ファイル／実端末入力。併用不可 |
| `--disk UNIT FILE` / `--disk-new UNIT` | 0～3のドライブへ既存媒体／空の新規媒体を接続。同一UNITに重ねて指定しない |
| `--disk-copy UNIT NEWFILE` | 終了時の新規保存先。書込みを許可する |
| `--disk-index UNIT INDEX` | D88の中の0始まりの番号。接続なしでは指定不可 |
| `--geometry C H S N` | raw読込み・raw保存・新規媒体の形状。1実行で1組 |

用語: ROM=読み取り専用の命令領域、RAM=書き換え可能な作業領域、MMIO=メモリアドレス経由の機器操作、ワード=4バイト、仮想時刻=エミュレータ内の時計です。
