// BÀI 1: NHÁY LED (Blink)
// Không cần nối dây: board Uno có sẵn 1 LED gắn với chân 13 (chữ "L" trên board).

#include <Arduino.h>  // Thư viện Arduino: cung cấp pinMode, digitalWrite, delay...

// Hàm setup() chạy MỘT lần khi board vừa cấp điện hoặc bấm nút RESET.
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Đặt chân 13 làm ngõ RA (xuất điện áp)
}

// Hàm loop() chạy LẶP LẠI mãi mãi sau khi setup() xong.
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // Xuất 5V ra chân 13 -> LED sáng
  delay(200);                      // Chờ 1000 mili-giây = 1 giây
  digitalWrite(LED_BUILTIN, LOW);   // Xuất 0V -> LED tắt
  delay(200);
}
