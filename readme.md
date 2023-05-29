# Pembukaan
ini adalah repo untuk mengontrol alat sterilisasi masker. alat ini menggunakan nodemcu sebagai main computenya, sensor suhu sebagai feedback loop untuk kontrol elemen pemanas, i2c lcd untuk interface HID, dan sensor ultrasonik untuk pendeteksi penuh tidaknya masker.

## Wiring
semua komponen pada sistem disupply menggunakan power 5v, dan power nodemcu didapatkan menggunakan adaptor smartphone, atau terhubung ke pc, atau menggunakan powerbank
- i2c LCD
  - D1 => SCL
  - D2 => SDA
- DHT 22
  - D4 => data DHT 22
- Ultrasonik 
  - D5 => trig
  - D6 => echo
- Relay 
  - D7 => data Relay

## keterangan lebih lanjut
- dokumen2 berupa foto ada di google drive di [laman ini](https://drive.google.com/drive/folders/1C07pm10zquFzI0jho24iSyTxRJuwMDl8?usp=sharing)
- website untuk monitoringnya ada di halaman
[berikut ini](https://atmasz.000webhostapp.com/)

## keterangan kode
- kode final yang sekarang ini berada di node mcu ada di src/main.cpp
- kode yang digunakan percobaan tadi (30 mei 23) ada di folder percobaan