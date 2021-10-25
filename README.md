# M5ATOMシリースでRTC Unitを使うサンプルプログラム

M5Stack社のM5ATOMシリーズ（[Matrix](https://docs.m5stack.com/en/core/atom_matrix)／[Lite](https://docs.m5stack.com/en/core/atom_lite)）で[RTC Unit](https://shop.m5stack.com/products/real-time-clock-rtc-unit-hym8563)を使ってNTPサーバと同期してRTCに時刻を記録するサンプルプログラムです。VSCode上のPlatformIO環境でビルドできます。

## ビルドの設定

`platformio.ino`の`[env]`セクションにある`build_flags`で`ENABLE_SERIAL_MONITOR`を定義すると，`common.h`にあるシリアル周りのマクロ定義が有効になり，シリアル表示のコードを挿入します。また，MACアドレスを表示するためのマクロ定義も追加されます。このマクロ定義を使ってデバッグ用の表示コードを書いておくと，不要なときは表示コードを挿入しないようにすることができます。

## WiFi・NTPサーバの設定

`data/settings.json`に以下のようにWiFiとNTPサーバの設定を書いてSPIFFSに転送することで，M5ATOMのNVS（不揮発記憶装置）に記録し，再起動時にNVSから設定を読み込んでWiFiやNTPサーバに接続できるようになります。

```data/settings.json
{
    "ssid": "SSID",
    "password": "Password",
    "ntp_server": "ntp.jst.mfeed.ad.jp"
}
```

`settings.json`をM5ATOMのSPIFFSに転送するには，VSCodeのPlatformIOのプロジェクトタスクから「m5atom/Platform/Upload Filesystem」を選びます。

![PlatformIOプロジェクトタスク](https://i.gyazo.com/41b42c0a6a6377485767a8dca70b8d9a.png)

コマンドラインから実行することもできます。以下のコマンドを実行します。

```bash
pio run --target uploadfs
```

M5ATOMのSPIFFSにファイルが転送されると，M5ATOMが再起動します。
M5ATOMはSPIFFSの`/settings.json`を見つけると，JSONに書かれているキーと値を名前空間`RTCUnit`のNVSに記録し，`settings.json`を削除します。
これ以降は再起動されるたびにNVSに記録された接続情報を利用してWiFiに接続します。

設定を変更するには，`settings.json`の内容を変更し，再度M5ATOMにファイルを転送してください。
