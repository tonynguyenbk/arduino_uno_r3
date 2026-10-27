// BÀI 5 - BẢN DEBUG: giống hệt bài 5 nhưng dùng debugger avr-stub thay cho Serial.
// Chạy: chọn env:lesson05_debug -> mở Run and Debug (Ctrl+Shift+D) -> F5.
// Debugger chiếm cổng Serial nên KHÔNG được dùng Serial.print trong file này.
// Debugger dùng chân 3 (AVR8_SWINT_SOURCE=1 trong platformio.ini): không cắm gì vào chân 3.
//
// LƯU Ý: Uno không có chip debug, nên breakpoint "chấm đỏ" của VS Code làm chương trình
// chậm đi hàng nghìn lần (millis() gần như đứng yên). Thay vào đó, dùng lệnh breakpoint()
// ngay trong code: chương trình chạy tốc độ thật và chỉ dừng khi gặp lệnh này.

#include <Arduino.h>
#include "avr8-stub.h"

const int NUT_PIN = 2;
const int LED_PIN = 8;
const unsigned long THOI_GIAN_CHONG_DOI = 50;  // ms

unsigned long lanCuoiL = 0;
bool denLSang = false;

unsigned long lanCuoiNgoai = 0;
unsigned long nhipNgoai = 500;
bool ledNgoaiSang = false;

int lanDocTruoc = HIGH;
int trangThaiNut = HIGH;
unsigned long thoiDiemThayDoi = 0;

void setup() {
  debug_init();  // khởi động debugger - phải gọi ĐẦU TIÊN
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(NUT_PIN, INPUT_PULLUP);
}

void loop() {
  unsigned long bayGio = millis();

  // ===== Việc 1: đèn L =====
  if (bayGio - lanCuoiL >= 500) {
    lanCuoiL = bayGio;
    denLSang = !denLSang;
    digitalWrite(LED_BUILTIN, denLSang);
  }

  // ===== Việc 2: LED ngoài =====
  if (bayGio - lanCuoiNgoai >= nhipNgoai) {
    lanCuoiNgoai = bayGio;
    ledNgoaiSang = !ledNgoaiSang;
    digitalWrite(LED_PIN, ledNgoaiSang);
    breakpoint();  // DỪNG tại đây mỗi lần LED đổi trạng thái -> xem biến, rồi F5 để chạy tiếp
  }

  // ===== Việc 3: nút nhấn =====
  int docNut = digitalRead(NUT_PIN);
  if (docNut != lanDocTruoc) {
    thoiDiemThayDoi = bayGio;
    lanDocTruoc = docNut;
  }
  if (bayGio - thoiDiemThayDoi >= THOI_GIAN_CHONG_DOI && docNut != trangThaiNut) {
    trangThaiNut = docNut;
    if (trangThaiNut == LOW) {
      if (nhipNgoai == 500) {
        nhipNgoai = 250;
      } else if (nhipNgoai == 250) {
        nhipNgoai = 100;
      } else {
        nhipNgoai = 500;
      }
    }
  }
}
