ﾗﾁﾔﾃﾏﾍﾒﾃz	              Fatal Error: Disk full."Error: some samples have been lost/Error: Out of disk space. Sample file truncated/Expecting either a '=' or a '#' instead found: Invalid character after number(number is outside of its allowable rangeCan't allocate sample bufferCan't allocate callgraph buffer4Can't work with this version of the operating systemCan't open sample fileCan't find program: $Buffer size too small for timer rateinvalid option: 5Usage: wsample [options] program [program arguments]
	Options:
B    -b=<size> specify the buffer size for samples (default: 63KB)
I    -c        callgraph information is to be included in the sample file
>    -f=<file> specify the output filename for the sample file
(    -r=<rate> specify the sampling rate
    A
    <size>    is a number in the range 1 to 63 (default: 63 KB)
G    <rate>    is a number (range 1 to 1000) specifying the approximate
L              time interval (in milliseconds) between samples (default: 10)
        :
   '#' may be used instead of '=' in options (eg. -b#10)
Executing: internal error: Cannot execute fileptrace() failedUnable to attach to processptrace() failed               #重大なエラー:ディスクがいっぱいです/エラー:サンプリング・データの一部が失われましたCエラー:ディスクの空き領域が不足です.サンプルファイルを切り詰めます 7'=' または '#' があるべき箇所に次のコードがありました: 数字の後に無効な文字があります数字が有効範囲内にありません.サンプリング用バッファのメモリが確保できません.コールグラフ用バッファのメモリを確保できません@このバージョンのオペレーティング・システムの上では動作できません$サンプルファイルをオープンできませんプログラムが見つかりません: *タイマー間隔に対してバッファが小さすぎます無効なオプション: 8使用方法: wsample [options] program [program arguments]
オプション:
:    -b=<size> サンプリング用バッファサイズ (ﾃﾞﾌｫﾙﾄ: 63KB)
;    -c        コールグラフ情報をサンプルファイルに含めます
+    -f=<file> サンプルファイルのファイル名
    -r=<rate> サンプリング間隔
    ;
    <size>    は 1 から 63 までの範囲です (ﾃﾞﾌｫﾙﾄ: 63 KB)
@    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を
E              指定する数値 (範囲:27 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 10)
        D
   オプションの中では '=' の代りに '#' を使ってください (例 -b#10)
実行中: 内部エラー: ファイルを実行できませんptrace() が失敗しましたプロセスに接触できませんでしたsetitimer() が失敗しました                          0   :         0V  �         0�  ｱ     @     0�  r    A     0  
    B     0  :    C     0P	  *   