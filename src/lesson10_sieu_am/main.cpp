// BÀI 10: Cảm biến siêu âm HC-SR04 - đo khoảng cách, làm "cảm biến lùi xe"
// Mô phỏng: wokwi/wokwi.toml (bấm vào cảm biến khi đang chạy để chỉnh khoảng cách)
//
// Nối dây (cảm biến cắm thẳng vào Arduino bằng 4 dây đực-cái):
//   HC-SR04 VCC  -> 5V
//   HC-SR04 TRIG -> A0   (chân analog vẫn dùng làm chân digital được: A0 = chân số 14)
//   HC-SR04 ECHO -> A1
//   HC-SR04 GND  -> GND
//   LED + điện trở 220Ω giống bài 7: chân 9 -> a4, điện trở b4-b10, LED e10(+)/e11(-), GND -> a11
//
// Cách cảm biến hoạt động:
//   1. Arduino đưa TRIG lên HIGH 10 micro giây -> cảm biến phát 8 xung siêu âm (40kHz, tai người không nghe được)
//   2. Sóng âm bay tới vật, dội lại. Trong lúc chờ, cảm biến giữ ECHO ở HIGH.
//   3. Arduino đo ECHO HIGH kéo dài bao lâu bằng pulseIn() -> thời gian đi + về.
//   Âm thanh đi 343 m/s = 0.0343 cm/micro giây. Đi + về nên chia 2:
//     khoảng cách (cm) = thời gian (us) * 0.0343 / 2  ~  thời gian / 58

#include <Arduino.h>

const int TRIG_PIN = A0;
const int ECHO_PIN = A1;
const int LED_PIN = 9;

const unsigned long CHO_TOI_DA = 30000;  // us; 30ms ~ 5m. Quá lâu không có tiếng vọng -> coi như không có vật

unsigned long lanCuoiDo = 0;
unsigned long lanCuoiNhay = 0;
bool ledSang = false;
int khoangCach = -1;  // cm, -1 = không đo được

// Đo khoảng cách, trả về cm (số nguyên). Trả về -1 nếu không có vật trong tầm.
int doKhoangCach() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);  // xung kích 10 micro giây
  digitalWrite(TRIG_PIN, LOW);

  // pulseIn: chờ ECHO lên HIGH, đo nó HIGH bao lâu (micro giây). Hết CHO_TOI_DA mà không thấy -> trả về 0.
  unsigned long thoiGian = pulseIn(ECHO_PIN, HIGH, CHO_TOI_DA);
  if (thoiGian == 0) {
    return -1;
  }
  return thoiGian / 58;
}

// Khoảng cách -> LED: gần dưới 10cm sáng liên tục, xa hơn thì nháy, càng gần nháy càng nhanh.
void capNhatLed(unsigned long bayGio) {
  if (khoangCach < 0 || khoangCach > 100) {
    digitalWrite(LED_PIN, LOW);  // không có vật / xa hơn 1m: tắt
    return;
  }
  if (khoangCach < 10) {
    digitalWrite(LED_PIN, HIGH);  // rất gần: sáng liên tục
    return;
  }
  unsigned long nhip = khoangCach * 10;  // 10cm -> 100ms, 100cm -> 1000ms
  if (bayGio - lanCuoiNhay >= nhip) {
    lanCuoiNhay = bayGio;
    ledSang = !ledSang;
    digitalWrite(LED_PIN, ledSang);
  }
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Bai 10: HC-SR04 - cam bien lui xe");
}

void loop() {
  unsigned long bayGio = millis();

  // Đo 10 lần mỗi giây. (pulseIn chờ tối đa 30ms -> không nên đo liên tục)
  if (bayGio - lanCuoiDo >= 100) {
    lanCuoiDo = bayGio;
    khoangCach = doKhoangCach();
    Serial.print("Khoang cach: ");
    if (khoangCach < 0) {
      Serial.println("ngoai tam do");
    } else {
      Serial.print(khoangCach);
      Serial.println(" cm");
    }
  }

  capNhatLed(bayGio);
}
