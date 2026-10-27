// BÀI 2: SERIAL MONITOR - cho Arduino "nói chuyện" với máy tính
// Không cần nối dây: dữ liệu đi qua chính sợi cáp USB.
// Lưu ý: chuỗi in ra nên viết KHÔNG DẤU để tránh lỗi font.

#include <Arduino.h>

// Biến khai báo NGOÀI hàm = biến toàn cục: giữ giá trị suốt chương trình.
int dem = 0;

void setup() {
  Serial.begin(9600);  // Mở cổng Serial, tốc độ 9600 bit/giây (phải khớp monitor_speed)

  Serial.println("Xin chao! Toi la Arduino Uno.");  // println = in rồi xuống dòng

  // --- Biến và kiểu dữ liệu ---
  int soNguyen = 25;        // số nguyên (trên Uno: -32768 .. 32767)
  float soThuc = 3.14;      // số thực (có phần thập phân)
  char kyTu = 'A';          // một ký tự, dùng dấu nháy đơn
  bool dung = true;         // đúng (true) / sai (false)

  Serial.print("So nguyen: ");  // print = in nhưng KHÔNG xuống dòng
  Serial.println(soNguyen);
  Serial.print("So thuc: ");
  Serial.println(soThuc);
  Serial.print("Ky tu: ");
  Serial.println(kyTu);
  Serial.print("Dung hay sai: ");
  Serial.println(dung);  // true in ra là 1, false là 0

  // --- Phép toán ---
  int a = 7;
  int b = 2;
  Serial.print("7 + 2 = ");
  Serial.println(a + b);
  Serial.print("7 - 2 = ");
  Serial.println(a - b);
  Serial.print("7 * 2 = ");
  Serial.println(a * b);
  Serial.print("7 / 2 = ");
  Serial.println(a / b);  // chia 2 số nguyên -> bỏ phần thập phân!
  Serial.print("7 % 2 = ");
  Serial.println(a % b);  // % = phần dư của phép chia
  Serial.print("7.0 / 2 = ");
  Serial.println(7.0 / 2);  // có số thực -> kết quả có thập phân

  Serial.println("--- Bat dau dem ---");
}

void loop() {
  Serial.print("Lan dem thu: ");
  Serial.println(dem);
  dem = dem + 1;  // tăng biến dem thêm 1 (viết tắt: dem++)
  delay(1000);
}
