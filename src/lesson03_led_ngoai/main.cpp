// BÀI 3: LED NGOÀI + BREADBOARD, if/else, vòng lặp for
// Nối dây: chân 8 -> điện trở 220 ohm -> chân DÀI của LED; chân NGẮN của LED -> GND

#include <Arduino.h>

// const = hằng số: giá trị không bao giờ đổi. Đặt tên cho chân giúp dễ sửa về sau.
const int LED_PIN = 8;

int vong = 0;  // đếm số vòng loop() đã chạy

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Bai 3: LED ngoai");
}

void loop() {
  vong++;  // viết tắt của: vong = vong + 1

  // --- if / else: chọn 1 trong 2 nhánh tùy điều kiện ---
  int soLanNhay;
  int thoiGian;
  if (vong % 3 == 0) {  // == là SO SÁNH bằng (khác với = là GÁN giá trị)
    soLanNhay = 5;      // vòng chia hết cho 3: nháy nhanh 5 lần
    thoiGian = 100;
  } else {
    soLanNhay = 2;      // các vòng khác: nháy chậm 2 lần
    thoiGian = 400;
  }

  Serial.print("Vong ");
  Serial.print(vong);
  Serial.print(": nhay ");
  Serial.print(soLanNhay);
  Serial.println(" lan");

  // --- for: lặp lại một khối lệnh với số lần xác định ---
  // (khởi tạo i = 0; lặp khi i < soLanNhay; sau mỗi lần thì i++)
  for (int i = 0; i < soLanNhay; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(thoiGian);
    digitalWrite(LED_PIN, LOW);
    delay(thoiGian);
  }

  delay(1500);  // nghỉ giữa các vòng cho dễ quan sát
}
