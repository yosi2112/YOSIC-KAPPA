# インターフェースリファレンスマニュアル

本書はコンソール、FDC、ディスク媒体、IPLの契約を定義します。
実行時のファイル指定は[エミュレータ](emulator.md)、CPUのアクセス規則は[アーキテクチャ](architecture-reference.md)、ゲストのソースは[アセンブラ](assembler.md)を参照してください。
uPD765Aの内部動作と、YOSIC-KAPPA基板が追加したレジスタを区別します。

## 1. MMIOの共通規則

MMIOは、特定のメモリアドレスを読み書きして機器を操作する方式です。LRM/SRMは32ビットですが、8ビットの機器データへは1回だけアクセスします。
DATAなどの書込みは下位8ビットを使い、読出しは上位を0で埋めます。4回の転送には分けません。
BOARD_CONTROLとSIGNALSは複数ドライブのビットを持つ基板レジスタで、bit8以上も使用します。
アドレスは4バイト整列です。未定義アドレス、方向の誤り、機器の転送条件違反はfaultです。MMIOの命令fetchも許可しません。
デバッガのpeekは状態を読むだけです。入力やFDC結果を消費せず、IRQも消しません。

## 2. コンソール

| 物理アドレス | 名前 | 方向・幅 | 値と副作用 |
|---|---|---|---|
| `0x00FE0000` | STATUS | 読出し32ビット | bit0 RX_READY、bit1 TX_READY、bit2 INPUT_EOF。他は0。副作用なし |
| `0x00FE0004` | RX_DATA | 読出し32ビット | 先頭1バイトを0拡張して返し、消費。空なら0 |
| `0x00FE0008` | TX_DATA | 書込み32ビット | 下位8ビットをstdoutへ出力 |

RX_READY=1なら読めるバイトがあります。空のRX_DATAから返る0と、入力されたNULはSTATUSで見分けます。
TX_READYは常に1です。出力先へ同期的に書きます。INPUT_EOFとRX_READYは独立し、EOFの後にも未読バイトを消費できます。
ファイル入力は全体を供給してEOFにします。無指定なら最初からEOFです。入力待ちでホストを止めることはありません。
対話入力ではCtrl+ZがEOF、Ctrl+Cはゲストへ03hとして渡します。バイト列を変換せず、文字コード変換は行いません。

次はDSを設定して`A`を1文字出す最小例です。実行用フォルダのルートで作業用`work`を作り、`work\put-a.asm`へ保存します。

```asm
        ADI R0,RZR,0FEh
        LSEG DS,R0
        ADI R1,RZR,'A'
        SRM R1,[RZR+8]
stop:   JMP stop
```

```bat
bin\yosic-as.exe work\put-a.asm -o work\put-a.rom
bin\yosic-sim.exe --rom work\put-a.rom --steps 4
```

出力は改行なしの`A`です。診断はstderrへ出ます。

## 3. FDC基板レジスタ

FDCはフロッピーディスクコントローラです。DATAへコマンドを送り、転送要求に応じてデータを送り返します。

| 物理アドレス | 名前 | 方向 | 定義・副作用 |
|---|---|---|---|
| `0x00FE0100` | MSR | R | 下位8ビットの状態。読出しで消えない |
| `0x00FE0104` | DATA | R/W | commandを書き、executionで転送し、resultを読む。段階に合う方向だけ有効 |
| `0x00FE0108` | SIGNALS | R | bit0 IRQ、bit1 DRQ、bit2はいずれかのhead loaded、bits4～7はdrive0～3 ready、bits8～11はindex。他は0。読出しで消えない |
| `0x00FE010C` | BOARD_CONTROL | W | bit0 RESET（レベル）、bit1 TC（パルス）、bits4～7はmotor0～3、bit8は速度（0=500 kbps、1=250 kbps）。他ビットは未使用 |
| `0x00FE0110` | DMA_ACK_DATA | R/W | DACK付き1バイト転送。読みは0拡張、書きは下位8ビット |

全レジスタへのCPUアクセス幅は32ビットです。SIGNALS、BOARD_CONTROL、DMA_ACK_DATAはKAPPA基板の追加機能です。
BOARD_CONTROLは全値を書きます。TCだけを送る場合もmotorと速度を含めます。たとえばdrive0のmotorを回したままTCを送る値は`0x12`です。
RESET=1の間はコマンドを受け付けません。RESET解除後はreadyの巡回確認を再開します。SPECIFYの値はRESETをまたいで保持します。

上記以外のオフセットは未定義で、読出し・書込み・非破壊読出しのいずれもfaultになります。媒体交換用のロックや世代番号レジスタはありません。DOSによる交換検出はディレクトリのチェックサムで行います。

MSRのビットは次のとおりです。

| マスク | 名前 | 意味 |
|---|---|---|
| `0x80` | RQM | ホストとの転送を受け付ける |
| `0x40` | DIO | 1ならFDC→ホスト、0ならホスト→FDC |
| `0x20` | NDMA | non-DMAのexecution中 |
| `0x10` | CB | コマンド処理中。resultを読み終えるまで1 |
| `0x0F` | drive busy | bits0～3が各ドライブのseek完了回収待ち |

## 4. command、execution、result

commandは命令byteと引数を送る段階、executionは媒体との転送、resultは結果byteを読む段階です。
1バイトごとにMSRを確認します。コマンド送信はRQM=1/DIO=0、結果受信はRQM=1/DIO=1です。
non-DMAのexecutionではNDMAも確認し、DATAで転送します。SPECIFYのND=1がnon-DMAです。
DMAのexecutionではDRQを待ち、DMA_ACK_DATAでDACK付き転送を行います。DMA転送中のRQMをDATA転送の許可と解釈しません。
結果は通常DATAで最後まで読みます。resultが残っている間、次のコマンドを送ってはいけません。
方向違反、要求前の転送、DATAとDACK窓の取り違えは`fdc-handshake` faultです。
IRQは、結果通知、seek/readyの未回収通知、non-DMA転送要求で立ちます。SIGNALSを読むだけでは消えません。
resultの最初のbyteを読むと結果IRQを下げます。seekなどはSENSE INTERRUPTで回収します。
CPUに割込み命令はありません。ゲストはSIGNALSやMSRをポーリング（繰返し確認）します。

## 5. コマンド一覧

数値はこの節では16進です。入力byte数は先頭のコマンドを含みます。
`U`はdrive番号（bits1:0）、`HD`は物理head（bit2）、`HU=(HD<<2)|U`です。
`C,H,R,N`は媒体のID（シリンダ、面、セクタ番号、サイズコード）、`EOT`は最終セクタ、`GPL`はギャップ長、`DTL`はN=0時の転送長です。
サイズは通常`128<<N`バイトです。N=0のREAD/WRITEはDTLを使い、DTL=0なら128バイトです。
実データが転送長より短いとDE/DDで終了します。
上位bit7=MT（複数面）、bit6=MF（1=MFM、0=FM）、bit5=SK（マークが合わないセクタを飛ばす）です。
READ/SCANはMT/MF/SK、WRITEはMT/MF（SKは禁止）、READ ID/FORMATはMF（MT/SKは禁止）です。
READ TRACKはMFを使い、MTは禁止、SKは読み進める判定には使いません。SPECIFY/SENSE/SEEK/RECALIBRATEの上位ビットは無視します。
禁止ビットの組合せや未知コマンドは1バイトの`80`を返します。

データ系の標準結果7バイトを`ST0,ST1,ST2,C,H,R,N`と書きます。

| コマンド | low5 | 入力byte列（先頭はフラグ付きopcode） | 入力／結果 | 動作 |
|---|---|---|---|---|
| READ DATA | `06` | op,HU,C,H,R,N,EOT,GPL,DTL | 9／7 | 通常データを読む |
| READ DELETED DATA | `0C` | op,HU,C,H,R,N,EOT,GPL,DTL | 9／7 | 削除マークのデータを読む |
| WRITE DATA | `05` | op,HU,C,H,R,N,EOT,GPL,DTL | 9／7 | 通常マークで書く |
| WRITE DELETED DATA | `09` | op,HU,C,H,R,N,EOT,GPL,DTL | 9／7 | 削除マークで書く |
| READ DIAGNOSTIC / READ TRACK | `02` | op,HU,C,H,R,N,EOT,GPL,DTL | 9／7 | index後から物理順にEOT個を読む。CRC異常を記録して継続 |
| READ ID | `0A` | op,HU | 2／7 | 次のIDを結果CHRNへ返す。データ転送なし |
| FORMAT TRACK | `0D` | op,HU,N,SC,GPL,D | 6／7 | SC個のCHRNをexecutionで各4バイト受け、Dでデータを埋める |
| SCAN EQUAL | `11` | op,HU,C,H,R,N,EOT,GPL,STP | 9／7 | 媒体とホストの全バイトが等しいか比較 |
| SCAN LOW OR EQUAL | `19` | op,HU,C,H,R,N,EOT,GPL,STP | 9／7 | 各媒体バイト≤対応ホストバイトか比較 |
| SCAN HIGH OR EQUAL | `1D` | op,HU,C,H,R,N,EOT,GPL,STP | 9／7 | 各媒体バイト≥対応ホストバイトか比較 |
| RECALIBRATE | `07` | op,HU | 2／0 | 最大77stepでtrack0へ移動。SISで結果回収 |
| SEEK | `0F` | op,HU,NCN | 3／0 | 指定シリンダへ移動。SISで結果回収 |
| SENSE INTERRUPT STATUS（SIS） | `08` | op | 1／2 | ST0,PCN。未回収通知がなければ1バイト`80` |
| SPECIFY | `03` | op,SRT/HUT,HLT/ND | 3／0 | 時間とDMAモードを設定 |
| SENSE DRIVE STATUS | `04` | op,HU | 2／1 | ST3を返す |
| VERSION / INVALID | `10` / 未知 | op | 1／1 | A型では`80` |

SCANはホストから比較バイトを送ります。STPは1か2です。比較はバイトごとで、通常の多バイト整数や文字列の辞書順比較ではありません。
全て等しいとSH、条件が成立しないまま終了するとSNを立てます。N=0でもSCANは128バイトです。
READとREAD DELETEDでマークが違う場合、SK=0なら転送後にCM、SK=1ならそのセクタを飛ばします。
書込みはデータマークを作り直します。保護はNW、未準備はNR、書込み時のドライブ故障はECで通知します。
不正NやSTP、SC=0は本モデルではNDとして有限に終了します。資料にない値を成功とみなす拡張ではありません。

### 1セクタを読む例

ゲスト側の手順例です。drive0へ500 kbps/MFMのKPB1媒体を接続した状態を前提にします。
まずBOARD_CONTROLへ`11`、次に`10`を書いてRESETとmotorを設定します。MSRを見ながら`03 DF 03`（SPECIFY、non-DMA）を送ります。
readyになるのを待ち、SISで通知を回収します。`07 00`でRECALIBRATEし、seek完了をSISで回収します。
`46 00 00 00 01 02 01 1B FF`を送り、C0/H0/R1の512バイトを読みます。
各byteの要求を確認して受信し、512バイトを受けたらBOARD_CONTROLへ`12`を書いてTCを出します。
7バイトの結果を読み、ST0/ST1/ST2とCHRNを検査します。この条件の正常TC結果は`00 00 00 01 00 01 02`です。
有限の待機カウンタとエラー処理を含む実行可能な例は[IPLソース](../src/firmware/ipl/ipl.asm)です。
小さい[read-sector.asm](../src/examples/fdc/read-sector.asm)と[write-sector.asm](../src/examples/fdc/write-sector.asm)はN=0の128バイト媒体用です。512バイトの起動媒体へそのまま適用しないでください。

### EOT、MT、TC

通常のREAD/WRITEがTCなしでEOTまで達するとENを立てます。単に「最後まで読んだので全ステータス0」とはなりません。
MT=1でhead0のEOTへ達するとhead1、R=1へ進みます。head1のEOTでは終了します。
正常TCによるREAD/WRITEの結果CHRNは次の規則です。Nは維持します。

| TC時点 | 結果のC/H/R |
|---|---|
| R≠EOT | C/Hは維持、Rを1増やす |
| R=EOT、MT=0 | Cを1増やす、Hは維持、R=1 |
| R=EOT、MT=1、物理head0 | Cは維持、Hを反転、R=1 |
| R=EOT、MT=1、物理head1 | Cを1増やす、Hを反転、R=1 |

TCはREADの残データとCRC時間を待ちます。WRITEでは残りを`00`で埋めてCRCを完了します。
SCANのTCは、それまでに比較したバイトで結果を決めます。READ TRACKはこの次R更新とは別です。
途中RESETは保留イベントを無効にしますが、すでに書いたデータや途中のFORMAT IDは媒体に残ります。途中WRITEではCRC不正が残り得ます。

## 6. ST0～ST3

表の値はビットマスクです。データ系結果はST0だけでなくST1/ST2も確認します。

| レジスタ | ビット・名前 | 意味 |
|---|---|---|
| ST0 | `C0` IC | 00=正常、40=異常終了、80=無効コマンド、C0=ready変化の通知 |
| ST0 | `20` SE / `10` EC / `08` NR | seek終了／装置検査異常／未準備 |
| ST0 | `04` HD / `03` US | head／drive番号 |
| ST1 | `80` EN | EOT到達による終了 |
| ST1 | `20` DE | IDまたはデータのCRC異常 |
| ST1 | `10` OR | 転送期限を越えた（overrun） |
| ST1 | `04` ND / `02` NW / `01` MA | 該当データなし／書込み禁止／マークなし |
| ST2 | `40` CM | データの通常／削除マークが要求と違う |
| ST2 | `20` DD | データ部のCRC異常 |
| ST2 | `10` WC / `02` BC | シリンダ不一致／不一致のCがFF |
| ST2 | `08` SH / `04` SN | SCANが全byte一致／SCAN条件不成立 |
| ST2 | `01` MD | データアドレスマークなし |
| ST3 | `80` FT / `40` WP / `20` RY | ドライブ故障／書込み保護／ready |
| ST3 | `10` T0 / `08` TS | 物理track0／両面ドライブ |
| ST3 | `04` HD / `03` US | head／drive番号 |

本実装では書込み前に未準備、ドライブ故障、保護の順で確認します。異常の組合せで結果が変わる点も試験対象です。
ID CRCはDE、データCRCはDE+DD、IDがない場合はMA、データマーク欠落はMA+MDです。READ IDでIDがない場合はMA+NDです。
要求H/R/Nの候補IDが見つかってもCが異なればND+WC、C=FFならBCも立ちます。
未回収seek通知はdriveごとに1つです。同じdriveの新しい通知は更新し、異なるdriveの通知順は保持します。SISで回収すると該当driveのbusyを解除します。

## 7. 時間とA型の差分

時刻の単位はnsです。1 µs=1000 ns、1 ms=1000000 nsです。機器はCPUと同じ仮想時計を使います。
readyは媒体装着・present・motorのすべてが成立すると真です。ready確認は1台256 µs、4台で1.024 msです。idle/command中に巡回します。
実機のmotor立上がり時間はドライブモデルへ抽象化しています。各driveのcylinder、motor、保護、両面能力は独立です。

| 時間 | 500 kbps設定での式 |
|---|---|
| SRT（1step） | `(16-(SPECIFY byte1>>4)) ms` |
| HLT（head load） | `(SPECIFY byte2>>1)*2 ms` |
| HUT（head unload） | `(SPECIFY byte1 & 15)*16 ms`。0なら256 ms |
| 1byteの周期 | MFM=16 µs、FM=32 µs |
| READ/SCANのサービス期限 | MFM=13 µs、FM=27 µs |
| WRITE/FORMATのサービス期限 | MFM=15 µs、FM=31 µs |

byte1/byte2はSPECIFY opcodeの後の2バイトです。250 kbps設定では表の時間が2倍になります。回転とindexの周期は独立です。
期限**より前**に転送を済ませます。期限と同時刻では機器イベントが先なのでoverrunです。
SEEKはdrive間で並行に進みます。RECALIBRATEは77stepでtrack0に達しなければECを報告します。
既定回転は200 ms、indexパルス幅は2 msです。回転位置に応じて次のIDを探すため、セクタ番号の大小と転送順は同じとは限りません。
FORMATはindexから次のindexまでにID、fill、gap、CRCを処理します。終端結果のRは最後のIDから1増えます。

A型ではVERSIONに`80`を返します。READ/WRITEの最終byteのoverrunはST1のORを立てない場合があります。SCANではORを立てます。FORMATではIDの4番目のbyteがこの最終byte判定になります。
DMA overrunで残ったDRQはDACKかRESETまで保持します。通常のDATAで結果を読んでも消えません。
DRQが残ったresultでDACK読出しすると、結果のST0をデータとして取り出す場合があります。通常転送とこのA型の挙動を混同しないでください。

## 媒体形式

媒体モデルには164物理トラックを持ちます。番号は`cylinder*2+head`です。各トラックには物理順の可変長セクタ列を持ちます。
CHRNの重複や欠番、Nと実データ長の違いを保持します。ID/dataのマーク、各CRCの正常性、密度、削除マーク、回転位置、gap、速度を別々に扱います。
CRCはCCITT多項式`0x1021`、初期値`0xFFFF`です。異常は状態フラグとして保存し、壊れたCRCの具体的なビット列を推測しません。

### raw（.img）

形状C/H/S/Nが必要です。C=1～82、H=1～2、S=1～255、N=0～6で、ファイル長は厳密に`C*H*S*(128<<N)`バイトです。全体が64 MiBを超える組合せは拒否します。
並びはC、H、Rの順で、Rは1からSです。rawにはmetadataがないので、異常、保護、予約情報、非標準配置などを持つ媒体の保存は拒否します。

### D88（.d88）

整数はlittle-endianです。標準の`0x2B0`バイトヘッダ、164本のトラックオフセットに対応します。

| ディスク先頭からのbyte位置 | 長さ | 内容 |
|---|---|---|
| `0x00` | 17 | ディスク名の領域 |
| `0x11` | 9 | 予約。先頭26バイトをそのまま保持 |
| `0x1A` | 1 | 保護フラグ |
| `0x1B` | 1 | 媒体type |
| `0x1C` | 4 | このディスクの全バイト数 |
| `0x20` | 164×4 | 各トラックの先頭位置。ディスク先頭基準、0は未使用 |

各セクタは16バイトヘッダと実データです。

| セクタ先頭からのbyte位置 | 長さ | 内容 |
|---|---|---|
| 0～3 | 各1 | C,H,R,N |
| 4 | 2 | トラック内セクタ数。各セクタで同じ値 |
| 6 | 1 | density。00=MFM、40=FM |
| 7 | 1 | deleted。10=削除マーク |
| 8 | 1 | status |
| 9 | 5 | 予約領域。保持する |
| 14 | 2 | 実データ長 |
| 16 | 実データ長 | バイト列 |

statusは00/10=正常、A0=ID CRC異常、B0=data CRC異常、E0=IDマーク欠落、F0=dataマーク欠落です。deletedは独立です。
未知statusもファイルとして保持しますが、該当セクタをFDCで使う場合は意味を推測せず明示エラーとします。
連結D88は各ディスクのサイズで区切ります。未変更ディスクの全バイトをそのまま保持します。変更時はサイズ・オフセット・セクタ数を再計算し、名前・予約・type・保護・密度・deleted・statusを保持します。
モデルを直接変更する呼出し側はdirtyを立てます。FDCは書込み開始時に立てます。
上限は全体64 MiB、連結256枚、1トラック4096セクタです。
旧160トラックヘッダ、余剰padding、重複・範囲外offset、切れたデータ、セクタ数の不一致は拒否します。
同時に複数の異常がある場合や詳細配置を残す場合はD88へ保存できず、KFD2を使います。

### KFD2（.kfd）

本実装の内部保存形式です。複数の異常や詳細な配置を保持します。整数はlittle-endian、時間はnsです。

| 部分 | 順に並ぶフィールド |
|---|---|
| 先頭32バイト | magic=`KFD2`（4）、D88と同じ先頭metadata（26）、protect（1）、type（1） |
| 各track（164個） | revolution:u32、explicit_layout:u8、rate:u32、sector_count:u16 |
| 各sector | CHRN:4、density:1、deleted:1、status:1、reserved:5、flags:u8、position:u32、gap:u32、length:u16、data:length |

u32/u16/u8は符号なし32/16/8ビットです。flagsのbit0=ID mark、bit1=data mark、bit2=ID CRC正常、bit3=data CRC正常です。1が存在または正常、上位4ビットは0です。
positionはindexからの位置で、revolution未満です。revolutionは1000000 ns以上です。explicit_layoutは0か1、sector_countは4096以下、実長は65535以下です。
rateはMFMでのbit/sで、0、250000、500000です。0はtypeから導きます。FMの実データ速度は半分です。
KFD1も読み込めます。KFD1のtrackにはrateがなく、0として扱います。新しい保存はKFD2です。
未知版、不正長、不正値、末尾の余分なバイトを拒否します。上限は64 MiBです。

### 物理情報がないときの補完

raw/D88で位置が未指定なら、IBM形式の正規配置を使います。type=`20`なら500 kbps、他は250 kbpsで、FM/MFMはsector densityによります。
最初のCHRNのC位置はMFMで162、FMで80 byte時間です。次のC位置までは`データ長 + (MFM 62 / FM 33) + GPL` byte時間です。

| N | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|---|
| MFMのGPL（16進） | 1B | 36 | 54 | 74 | FF | FF | FF |
| FMのGPL（16進） | 1B | 2A | 3A | 8A | FF | FF | FF |

1回転に収まらないraw/D88は等間隔の位置を割り当てます。実媒体の配置を復元したという意味ではなく、欠けた情報を補う決まりです。
FORMAT後の位置、gap、rateが正規配置と厳密に一致する場合だけ、暗黙配置へ戻してD88へ保存できます。合わない場合はKFD2を使います。
ホストの原本は自動更新しません。コピー保存の既存出力は拒否します。[操作手順](emulator.md#6-書込みとコピー保存)も参照してください。

## IPL契約

IPLは[ipl.asm](../src/firmware/ipl/ipl.asm)を組み立てたROMです。CS=`0xFF`、PC=0から実行し、FDCのnon-DMA MMIOでstage2を読みます。
ホストがstage2をRAMへ直接置く方式ではありません。drive0、500 kbps、MFM、80シリンダ×2面×18セクタ、N=2（512バイト）を使います。
`yosic-mkimage`で作る標準KPB1媒体では、ヘッダはC0/H0/R1、payloadはR2～17の連続領域です。最大16セクタを使い、トラックはまたぎません。DOS用のFAT12媒体では、標準BPBを保つためKPB1拡張をセクタ0のbyte 64へ配置し、ヘッダに記録された位置からローダーを読みます。raw/D88の選択はホストが行い、IPLはファイル形式を認識しません。

### KPB1ヘッダ

標準KPB1媒体の先頭セクタの32バイトは、little-endianの32ビットワード8個です。

| byte位置 | フィールド・固定値 |
|---|---|
| 0 | magic=`KPB1`、ワード`0x3150424B` |
| 4 | version=1 |
| 8 | payload長。4～8192バイト、4の倍数 |
| 12 | load=`0x00002000` |
| 16 | entry=`0x00002000` |
| 20 | payloadのlittle-endian32ビットワード加算和、mod 2^32 |
| 24 | payloadのセクタ数。`ceil(バイト数/512)`、1～16 |
| 28 | flags/reserved=0 |

標準KPB1媒体の先頭セクタの残り480バイトは生成時0で埋め、IPLは予約域として無視します。DOS用FAT12媒体の拡張はbyte 64からの48バイトで、先頭7ワードは上表と同じです。続く5ワードは512バイト単位のトラック長、payloadのLBA、シリンダ、ヘッド、セクタ番号です。1.44 MB媒体のトラック長は18、1.25 MB媒体は16で、DOSローダーはどちらもシリンダ1の先頭から配置します。
payloadの最終セクタも0埋めします。checksumとRAMコピーの対象はpayload長だけです。checksumは偶発的な破損用で、暗号学的な認証ではありません。

### RAM配置と引渡し

| 用途 | アドレス |
|---|---|
| ヘッダコピー | `0x1800`～`0x19FF` |
| payload | `0x2000`～`0x3FFF` |
| 受信作業域 | `0x10000`～`0x107FF`。1受信byteを1wordで保持 |
| スタック初期値 | SS=0、SP=`0xF000` |

転送中は1バイトずつ作業域へ置き、TCと結果検査の後にADDで倍加し、ORRで4バイトを結合します。CPUにバイト命令を追加せずに実装しています。
検証後、ROM offset=`0x1FFC`の`LSEG CS,RZR`で切り替えます。旧PC+4を使うため、次はRAM`0x2000`をfetchします。
引渡し時はCS=DS=SS=ES=0、PC=`0x2000`、SP=`0xF000`です。他の汎用レジスタとFLAGSは未規定です。motorは停止します。
stage2は先頭がentryです。サンプルは相対分岐を使い、必要な絶対データアドレスを明示しています。

### 起動順と異常

順番はreset/motor、SPECIFY、通知回収、RECALIBRATE/SIS、ヘッダREAD、ヘッダ検査、payload READ、checksum、制御移行です。
各512バイトを受信してTCを発行し、結果7バイトのST0/ST1/ST2とCHRNを検査します。
再試行は0回です。すべての待機に上限があり、READは1セクタあたり最大約1.05秒の待機です。

| 表示 | 意味 |
|---|---|
| `YOSIC-KAPPA INITIAL PROGRAM LOADER V1.0` | IPLが始まった |
| `IPL: BOOT` | 検査に成功しstage2へ移る |
| `IPL: NO DRIVE` | readyを確認できない |
| `IPL: FDC ERROR` | FDCの結果が条件を満たさない |
| `IPL: BAD HEADER` | KPB1ヘッダが不正 |
| `IPL: CHECKSUM` | payloadの加算和が不一致 |
| `IPL: TIMEOUT` | handshake待機の上限に達した |

異常表示の後はmotorを止め、ROM内で繰り返します。HALTはありません。ホストは`--steps`、ブレーク、Ctrl+Cで終了します。
通常実行は命令上限なしです。有限実行で確認する場合、通常の小さいstage2は`--steps 1000000`、最大payloadには`--steps 6000000`を使います。ゲスト異常でも明示した命令上限によるホスト終了コードは0です。

## 対応範囲と根拠

uPD765Aのコマンドと状態の根拠はNEC「µPD765A/µPD765B Floppy Disk Controller」の表4、MSR/ST、A/B差分です。TC後CHRNと時間境界の補助はIntel「8272A Preliminary」、table 8です。
媒体のD88構造はD88作者実装（yas-sim/floppy_image_library_d88_d77）とMAMEのD88 readerも参照した形式です。基板レジスタ、KFD2、KPB1は本実装の契約です。
FDCはセクタと仮想時間のモデルです。磁束、ビットセル列、PLL、電気波形、setup/holdやゲート遅延、実機全品種の差までは再現しません。

用語: IRQ=通知信号、DRQ=DMA転送要求、DACK=DMA要求への応答、TC=転送終了通知、CHRN=セクタIDの4値、CRC=誤り検出符号、index=1回転の基準位置、peek=副作用なしの観測です。
