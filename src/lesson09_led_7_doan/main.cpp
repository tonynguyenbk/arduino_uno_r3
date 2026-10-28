// BÀI 9: LED 7 đoạn 5611AH (cathode chung)
// Sơ đồ chân: so_do_chan_5611AH.svg. Nối dây: so_do.png. Mô phỏng: wokwi/wokwi.toml
//
// Mỗi thanh nối qua 1 điện trở 220Ω tới 1 chân Arduino (chân chung -> GND):
//   thanh:  a   b   c   d   e   f   g
//   chân:  13  12   7   9   8  10  11
// (chân 13 nối với đèn L trên board -> đèn L sáng cùng thanh a, không sao cả)
// Số tự đếm 0 -> 9 mỗi giây.
// Lúc khởi động: bật lần lượt từng thanh a -> g để kiểm tra dây cắm đúng chưa.

#include <Arduino.h>

// MẢNG chân: CHAN_THANH[0] là chân của thanh a, CHAN_THANH[1] là thanh b, ...
const int CHAN_THANH[7] = {13, 12, 7, 9, 8, 10, 11};
const char TEN_THANH[7] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};

// MẢNG chữ số: mỗi số 0..9 là 1 byte, mỗi BIT cho biết 1 thanh bật (1) hay tắt (0).
// Viết dạng nhị phân 0b...: đọc từ PHẢI sang trái là thanh a, b, c, d, e, f, g.
//                    gfedcba
const byte CHU_SO[10] = {
  0b0111111,  // 0: a b c d e f
  0b0000110,  // 1: b c
  0b1011011,  // 2: a b d e g
  0b1001111,  // 3: a b c d g
  0b1100110,  // 4: b c f g
  0b1101101,  // 5: a c d f g
  0b1111101,  // 6: a c d e f g
  0b0000111,  // 7: a b c
  0b1111111,  // 8: tất cả
  0b1101111,  // 9: a b c d f g
};

int soDangHien = 0;
unsigned long lanCuoiDoiSo = 0;

// Hiện 1 chữ số lên LED: duyệt 7 thanh, thanh nào có bit = 1 thì bật.
void hienSo(int so) {
  byte mau = CHU_SO[so];
  for (int i = 0; i < 7; i++) {
    bool bat = bitRead(mau, i);  // bitRead(x, i): lấy bit thứ i của x (0 hoặc 1)
    digitalWrite(CHAN_THANH[i], bat ? HIGH : LOW);
  }
}

void tatHet() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(CHAN_THANH[i], LOW);
  }
}

// Bật lần lượt từng thanh để kiểm tra dây. Thanh nào không sáng / sáng sai chỗ -> dây cắm sai.
void kiemTraTungThanh() {
  for (int i = 0; i < 7; i++) {
    tatHet();
    digitalWrite(CHAN_THANH[i], HIGH);
    Serial.print("Thanh ");
    Serial.print(TEN_THANH[i]);
    Serial.print(" (chan ");
    Serial.print(CHAN_THANH[i]);
    Serial.println(")");
    delay(500);  // chỉ chạy 1 lần lúc khởi động nên dùng delay cho đơn giản
  }
  tatHet();
}

void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(CHAN_THANH[i], OUTPUT);
  }
  Serial.begin(9600);
  Serial.println("Bai 9: LED 7 doan - kiem tra tung thanh a..g");
  kiemTraTungThanh();
  Serial.println("Dem 0..9, moi giay 1 so");
  hienSo(soDangHien);
  lanCuoiDoiSo = millis();
}

void loop() {
  unsigned long bayGio = millis();
  if (bayGio - lanCuoiDoiSo >= 1000) {
    lanCuoiDoiSo = bayGio;
    soDangHien = (soDangHien + 1) % 10;  // 9 + 1 = 10 -> % 10 = 0: quay về 0
    hienSo(soDangHien);
    Serial.print("So ");
    Serial.println(soDangHien);
  }
}
