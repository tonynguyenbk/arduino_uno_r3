// BÀI 4: NÚT NHẤN - digitalRead, INPUT_PULLUP, chống dội phím
// Nối dây (giữ nguyên LED ở chân 8 của bài 3):
//   chân 2 -> một chân nút nhấn; chân CHÉO ĐỐI DIỆN của nút -> GND

#include <Arduino.h>

const int NUT_PIN = 2;
const int LED_PIN = 8;

// Thí nghiệm: chạy với false trước, quan sát; sau đó đổi thành true và nạp lại.
const bool CHONG_DOI = true;

bool ledSang = false;        // LED đang sáng hay tắt
int soLanNhan = 0;           // đếm số lần nhấn
int trangThaiTruoc = HIGH;   // trạng thái nút ở lần đọc trước (HIGH = đang nhả)

void setup() {
  // INPUT_PULLUP: bật điện trở kéo lên bên trong chip.
  // Nút NHẢ -> chân 2 được kéo lên 5V -> đọc được HIGH
  // Nút NHẤN -> chân 2 nối thẳng xuống GND -> đọc được LOW   (ngược với trực giác!)
  pinMode(NUT_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Bai 4: Nut nhan - moi lan nhan se dao trang thai LED");
}

void loop() {
  int trangThai = digitalRead(NUT_PIN);  // đọc chân 2: HIGH hoặc LOW

  // Chỉ xử lý khi trạng thái THAY ĐỔI so với lần trước
  if (trangThai != trangThaiTruoc) {     // != nghĩa là "khác"
    if (trangThai == LOW) {              // vừa chuyển từ nhả -> nhấn
      soLanNhan++;
      ledSang = !ledSang;                // ! = phủ định: true <-> false
      digitalWrite(LED_PIN, ledSang);    // true = HIGH, false = LOW

      Serial.print("Lan nhan thu ");
      Serial.print(soLanNhan);
      Serial.print(" -> LED ");
      if (ledSang) {
        Serial.println("SANG");
      } else {
        Serial.println("TAT");
      }
    }

    if (CHONG_DOI) {
      delay(50);  // chờ 50ms cho tiếp điểm kim loại hết rung rồi mới đọc tiếp
    }
  }

  trangThaiTruoc = trangThai;  // nhớ lại để so sánh ở vòng sau
}
