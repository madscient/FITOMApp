# SCCISharp

## 概要

SCCISharp は [SCCI](http://www.pyonpyon.jp/~gasshi/fm/) を C# などから扱えるようにしたバインド/ラッパライブラリです。

## ビルド方法

C++/CLI を使用しているため、Visual Studioなどでビルドしてください。__Windows 以外ではビルドおよび実行はできません。__

## 使用方法

1. SCCISharp.dll をアセンブリ参照に追加
2. scci.dll、scciconfig.exe、License.txt を出力ディレクトリにコピーするよう設定
    * Visual Studio の場合は上記ファイルを追加後、右クリックして [プロパティ(R)]-[出力ディレクトリにコピー] の項目を 「常にコピーする」または「新しい場合はコピーする」に設定
3. 名前空間 `SCCISharp` 以下から各種機能が利用できます
4. (実行前に scciconfig.exe の実行をお忘れなく)

## ダウンロード

[リリース](https://github.com/nanase/SCCISharp/releases) ページから最新のバイナリをご利用ください。zip ファイルがコンパイル済みバイナリです。

特に表記がない場合、すべて __x86 (32 bit)__ バイナリです。動作確認環境、SCCIバージョンはその都度ご確認ください。

## 謝辞

SCCI 作者の [餓死3さん](https://twitter.com/hozuki72) によりこのライブラリの開発は成り立っています。この場を借りて御礼申し上げます。

## 連絡先

* 使用報告は不要です。
* プログラムに関する報告は issue からお願いします。

## ライセンス

__[the MIT License](https://github.com/nanase/SCCISharp/blob/master/LICENSE.sccisharp)__

* ライブラリの使用に伴う損害に関しましては、私どもは何ら責任を負いません。
