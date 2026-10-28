// BÀI 13: Động cơ servo SG90 - vặn biến trở để xoay servo 0..180 độ
// Mô phỏng: wokwi/wokwi.toml
//
// Nối dây:
//   Servo dây NÂU (GND) -> GND,  dây ĐỎ (V+) -> 5V,  dây CAM (tín hiệu) -> chân 9
//   Biến trở: chân giữa -> A0, 2 chân ngoài -> 5V và GND (giống bài 7)
//
// Servo khác động cơ thường: bên trong có mạch tự giữ đúng GÓC được yêu cầu.
// Cách ra lệnh: cứ 20ms gửi 1 xung HIGH, độ dài xung quyết định góc:
//   ~0.5ms -> 0 độ,   ~1.5ms -> 90 độ,   ~2.5ms -> 180 độ
// Thư viện Servo tự tạo các xung này (dùng Timer1 của chip - xem bài 27).
// Hệ quả: khi dùng Servo, analogWrite() trên chân 9 và 10 KHÔNG còn chạy được.
//
// Lưu ý nguồn: servo kéo dòng lớn khi quay (có thể vài trăm mA). 1 servo SG90 cấp từ 5V
// của Uno qua USB là được; nhiều servo hoặc servo to thì phải dùng nguồn riêng.

#include <Arduino.h>
#include <Servo.h>

const int SERVO_PIN = 9;
const int BIEN_TRO_PIN = A0;

Servo servo;  // đối tượng servo (giống camBien ở bài 11)
int gocCu = -1;

void setup() {
  Serial.begin(9600);
  Serial.println("Bai 13: vang bien tro de xoay servo");
  servo.attach(SERVO_PIN);  // bắt đầu gửi xung ra chân 9
}

void loop() {
  int giaTri = analogRead(BIEN_TRO_PIN);   // 0..1023
  int goc = map(giaTri, 0, 1023, 0, 180);  // -> 0..180 độ

  // Chỉ ra lệnh khi góc đổi từ 2 độ trở lên: ADC hay nhiễu ±1,
  // ra lệnh liên tục sẽ làm servo rung rung tại chỗ.
  if (abs(goc - gocCu) >= 2) {  // abs = trị tuyệt đối: abs(-3) = 3
    gocCu = goc;
    servo.write(goc);
    Serial.print("Goc: ");
    Serial.print(goc);
    Serial.print(" do  -> xung ~");
    Serial.print(servo.readMicroseconds());  // độ dài xung thư viện đang gửi
    Serial.println(" us");
  }
  delay(15);  // cho servo kịp quay (servo cần ~10-20ms để nhận 1 xung mới)
}
