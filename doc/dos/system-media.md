# システム媒体と連携操作

標準のDOSシステム媒体は、FORMAT、DISKCOPY、SUBMIT、DULL、LEDTの5つのYKEをルートに収録します。1.44 MB（80×2×18×512）と1.25 MB（77×2×8×1024）の両形式で、起動、編集、コンパイル、ファイル操作ができます。RAWとD88は同じセクタ内容です。

実行用フォルダの`dos/dos.d88`または`dos/dos-1.25.d88`をドライブ0へ接続してください。起動方法と書込み結果の保存方法は[エミュレータ説明書](../emulator.md)を参照してください。配布媒体の作業用コピーを使い、変更を保持するときは保存先を指定します。

## 編集から一括コンパイル・実行まで

書込み可能な媒体で、次の順に入力します。`Ctrl+Z`の行では、その文字列ではなくCtrlとZを押してEnterを入力します。LEDTの`I`は挿入、`E`は保存終了です。

```text
LEDT HELLO.DUL
I
program;
var h,i;
begin
  h:=fopen("RESULT.TXT",1);
  i:=0;
  while i<160 do begin fwriteln(h,"integrated ",i);i:=i+1;end;
  fclose(h);
  writeln("integration OK");
end.
Ctrl+Z
E
LEDT BUILD.SUB
I
DULL HELLO.DUL HELLO.YKE
HELLO
Ctrl+Z
E
SUBMIT BUILD
DIR
```

SUBMITの実行中はキーを入力せず、`DULL: compiled`、`integration OK`とDOSプロンプトを待ってからDIRを入力してください。先行入力は投入の中止キーとして扱われます。HELLO.YKEとRESULT.TXTが作成され、RESULT.TXTは複数クラスタにまたがる160行のテキストになります。SUBMITはBUILD.SUBの行を順に実行します。投入中のコマンドがエラーになった場合は、[コマンドリファレンス](commands.md)に従って原因を確認してください。

生成したHELLO.YKEはDULLを収録していない媒体へコピーしても実行できます。プログラム自身のファイル操作には既存DOS APIを使い、ホスト側の開発ツールは必要ありません。言語と保存失敗時の扱いは[PL/DULL説明書](pldull.md)、編集とバックアップの扱いは[LEDT説明書](ledt.md)を参照してください。

## FORMAT・SYS・COPY・DISKCOPYの役割

FORMATはデータ媒体を初期化します。起動システムや5つのYKEは作成しません。SYSはドライブ0から別のドライブへ起動システムを転送し、転送先のユーザーファイルを保持します。YKEの追加はCOPYで行います。

同じ形式の書込み可能な作業用媒体をドライブ1へ接続した例です。FORMATはドライブ1の既存ファイルを消去します。

```text
FORMAT 1: /Q /Y
SYS 1:
COPY FORMAT.YKE 1:\FORMAT.YKE
COPY DISKCOPY.YKE 1:\DISKCOPY.YKE
COPY SUBMIT.YKE 1:\SUBMIT.YKE
COPY DULL.YKE 1:\DULL.YKE
COPY LEDT.YKE 1:\LEDT.YKE
```

保存した転送先をドライブ0として接続すると起動できます。DISKCOPYは同じ物理形式の媒体全体を複製するため、起動領域と5コマンドも一緒に転送します。使用書式、媒体交換、確認入力は[コマンドリファレンス](commands.md)を参照してください。
