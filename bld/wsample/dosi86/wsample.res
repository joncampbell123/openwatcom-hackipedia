ﾗﾁﾔﾃﾏﾍﾒﾃﾚ              Fatal Error: Disk full."Error: some samples have been lost/Error: Out of disk space. Sample file truncated/Expecting either a '=' or a '#' instead found: Invalid character after number(number is outside of its allowable rangeCan't allocate sample bufferCan't allocate callgraph buffer4Can't work with this version of the operating systemCan't open sample fileCan't find program: $Buffer size too small for timer rateinvalid option: 5Usage: wsample [options] program [program arguments]
	Options:
B    -b=<size> specify the buffer size for samples (default: 63KB)
I    -c        callgraph information is to be included in the sample file
>    -f=<file> specify the output filename for the sample file
(    -r=<rate> specify the sampling rate
@    -i=<intr> specify an interrupt number for assigning samples
B    -d        disable assignment of DOS interrupts to application
  A
    <size>    is a number in the range 1 to 63 (default: 63 KB)
E    <rate>    is a number (range 1 to 55) specifying the approximate
L              time interval (in milliseconds) between samples (default: 55)
D    <intr>    is a hex (base 16) number (range 20 to ff) specifying
B              an interrupt number to treat in a special manner - 
I              any samples during the execution of the software interrupt
I              are assigned to the instruction that invoked the interrupt
;              (default: the DOS interrupt 21 is monitored)
   :
   '#' may be used instead of '=' in options (eg. -b#10)
Too many sections in programLInterrupts 34 to 3d are reserved for 80x87 emulation and cannot be monitored)Cannot monitor more than eight interrupts                  #重大なエラー:ディスクがいっぱいです/エラー:サンプリング・データの一部が失われましたCエラー:ディスクの空き領域が不足です.サンプルファイルを切り詰めます 7'=' または '#' があるべき箇所に次のコードがありました: 数字の後に無効な文字があります数字が有効範囲内にありません.サンプリング用バッファのメモリが確保できません.コールグラフ用バッファのメモリを確保できません@このバージョンのオペレーティング・システムの上では動作できません$サンプルファイルをオープンできませんプログラムが見つかりません: *タイマー間隔に対してバッファが小さすぎます無効なオプション: 8使用方法: wsample [options] program [program arguments]
オプション:
:    -b=<size> サンプリング用バッファサイズ (ﾃﾞﾌｫﾙﾄ: 63KB)
;    -c        コールグラフ情報をサンプルファイルに含めます
+    -f=<file> サンプルファイルのファイル名
    -r=<rate> サンプリング間隔
+    -i=<intr> サンプリングを行う割込み番号
$    -d        DOS割込みの監視の禁止
  ;
    <size>    は 1 から 63 までの範囲です (ﾃﾞﾌｫﾙﾄ: 63 KB)
=    <rate>    はサンプリング間のおよその時間間隔(ﾐﾘ秒単位)を
>              指定する数値(範囲:1から55まで)です (ﾃﾞﾌｫﾙﾄ: 55)
H    <intr>    は16進数(範囲:20からffまで)で、以下の特別な方法で処理する
               割込み番号です - 
O              ソフトウェア割込み実行中のサンプリングは、割込みを発生した命令に
              割り当てます
9              (ﾃﾞﾌｫﾙﾄ: DOS 割込み 21 がﾓﾆﾀｰされています)
   D
   オプションの中では '=' の代りに '#' を使ってください (例 -b#10)
&プログラムの中のセクションが多すぎますU割込み 34 から 3d は 80x87 エミュレーションのために予約されており、モニターできません#9個以上の割込みはモニターできません             0   :         0V  f         0ｼ  ﾛ     @     0�  r    A     0	  Y    B     0b
  x  