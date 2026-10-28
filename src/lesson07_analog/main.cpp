// BÀI 7: Đọc analog - biến trở (7A) và quang trở (7B)
// Nối dây: xem so_do_7a.png, so_do_7b.png (cùng thư mục).
// Mô phỏng: wokwi_7a/wokwi.toml (chỉ biến trở), wokwi_7b/wokwi.toml (thêm quang trở)
//   Điện trở 220Ω: b4-b10, dây chân 9 -> a4. LED: e10 (+), e11 (-). Nút: e18/e20/f18/f20.
//   7A - Biến trở: chân f27 (GND), f28 (giữa), f29 (VCC); dây GND -> j27, A0 -> j28, 5V -> j29
//   7B - Điện trở 10kΩ: h23-h29 (cột 29 = 5V); quang trở: g23-g27 (cột 27 = GND); dây A1 -> j23
//
// analogRead(chân) đo điện áp (0V..5V) và trả về số nguyên 0..1023
//   0V -> 0,   2.5V -> ~512,   5V -> 1023
// Vì sao 1023? Bộ ADC của Uno có 10 bit -> 2^10 = 1024 mức (0..1023).
//
// Nhấn nút để đổi chế độ: 0 = biến trở điều khiển LED, 1 = quang trở điều khiển LED (đèn ngủ)

#include <Arduino.h>

const int BIEN_TRO_PIN = A0;
const int QUANG_TRO_PIN = A1;
const int LED_PIN = 9;
const int NUT_PIN = 2;
const unsigned long THOI_GIAN_CHONG_DOI = 50;

// Quang trở: càng SÁNG -> điện trở càng NHỎ -> điện áp ở A1 càng THẤP.
// Hai mốc dưới đây là số đọc được ở A1 khi phòng sáng / khi lấy tay che kín.
// Chạy thử, xem log rồi sửa lại cho đúng với phòng của bạn.
const int A1_KHI_SANG = 100;
const int A1_KHI_TOI = 900;

int cheDo = 0;  // 0 = biến trở, 1 = quang trở
unsigned long lanCuoiIn = 0;

// --- Nút nhấn (chống dội giống bài 5, 6) ---
int lanDocTruoc = HIGH;
int trangThaiNut = HIGH;
unsigned long thoiDiemThayDoi = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(NUT_PIN, INPUT_PULLUP);
  // Chân analog không cần pinMode: mặc định đã là INPUT
  Serial.begin(9600);
  Serial.println("Bai 7: nhan nut de doi che do (bien tro <-> quang tro)");
}

void loop() {
  unsigned long bayGio = millis();

  // ===== Nút nhấn: đổi chế độ 0 <-> 1 =====
  int docNut = digitalRead(NUT_PIN);
  if (docNut != lanDocTruoc) {
    thoiDiemThayDoi = bayGio;
    lanDocTruoc = docNut;
  }
  if (bayGio - thoiDiemThayDoi >= THOI_GIAN_CHONG_DOI && docNut != trangThaiNut) {
    trangThaiNut = docNut;
    if (trangThaiNut == LOW) {
      cheDo = (cheDo + 1) % 2;
      Serial.println(cheDo == 0 ? "--- Che do 0: BIEN TRO ---" : "--- Che do 1: QUANG TRO ---");
    }
  }

  // ===== Đọc cả 2 cảm biến =====
  int giaTriA0 = analogRead(BIEN_TRO_PIN);   // 0..1023
  int giaTriA1 = analogRead(QUANG_TRO_PIN);  // 0..1023

  // ===== Tính độ sáng LED =====
  int doSang;
  if (cheDo == 0) {
    // map(x, tuMin, tuMax, denMin, denMax): đổi x từ thang này sang thang khác
    // ở đây: thang ADC 0..1023  ->  thang PWM 0..255
    doSang = map(giaTriA0, 0, 1023, 0, 255);
  } else {
    // Đèn ngủ: phòng sáng -> LED tắt, phòng tối -> LED sáng
    doSang = map(giaTriA1, A1_KHI_SANG, A1_KHI_TOI, 0, 255);
    // map() không tự giới hạn: số đọc nhỏ hơn A1_KHI_SANG sẽ ra số ÂM,
    // lớn hơn A1_KHI_TOI sẽ ra số > 255. constrain() kẹp lại trong 0..255.
    doSang = constrain(doSang, 0, 255);
  }
  analogWrite(LED_PIN, doSang);

  // ===== In ra mỗi 200ms =====
  if (bayGio - lanCuoiIn >= 200) {
    lanCuoiIn = bayGio;
    Serial.print(cheDo == 0 ? "[bien tro]  " : "[quang tro] ");
    Serial.print("A0 = ");
    Serial.print(giaTriA0);
    Serial.print("\t A1 = ");   // \t = phím Tab, giúp các cột thẳng hàng
    Serial.print(giaTriA1);
    Serial.print("\t PWM = ");
    Serial.println(doSang);
  }
}
