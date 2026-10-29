// BÀI 29: Transistor / MOSFET - đóng cắt tải lớn: quạt 12V điều tốc bằng PWM
// Mô phỏng: wokwi/wokwi.toml (Wokwi không có transistor -> LED đứng thay cho quạt, độ sáng = tốc độ)
// Mạch thật: xem so_do_mach_that.svg (NPN cho tải nhỏ, MOSFET cho tải lớn)
//
// Nối dây (mạch thật, MOSFET IRLZ44N):
//   Chân 9 (PWM) -> điện trở 220Ω -> chân G (gate) của MOSFET; điện trở 10kΩ từ G xuống GND
//   S (source) -> GND chung;  D (drain) -> dây (-) của quạt;  dây (+) của quạt -> +12V
//   Diode 1N4007 song song với quạt: vạch (cathode) về phía +12V
//   GND của nguồn 12V nối chung GND Arduino (bắt buộc!)
//   Biến trở chỉnh tốc độ -> A0
//
// Chân Arduino chỉ cho ra ~20mA ở 5V. Quạt 12V cần 200mA, dải LED 2A, động cơ vài A
//   -> chân Arduino chỉ "ra lệnh", transistor/MOSFET mới là "công tắc" chịu dòng lớn.

#include <Arduino.h>

const int QUAT_PIN = 9;          // PWM
const int BIEN_TRO_PIN = A0;

const int NGUONG_TAT = 10;       // vặn dưới 10% coi như tắt (vùng chết của núm vặn)
const int DUTY_TOI_THIEU = 30;   // quạt/động cơ thật dưới ~30% PWM không đủ lực để quay
const unsigned long THOI_GIAN_KICK = 300;   // lúc khởi động: chạy 100% trong 300ms để "đá" cho quay
const unsigned long BUOC_RAMP = 10;          // mỗi 10ms đổi duty 1 đơn vị -> tăng/giảm tốc êm

int dutyHienTai = 0;             // 0..255 đang xuất ra chân PWM
unsigned long lanCuoiRamp = 0, lanCuoiIn = 0, lucKick = 0;
bool dangKick = false;

// In bảng tính toán linh kiện - đây là việc kỹ sư làm trước khi chọn linh kiện
void inTinhToan() {
  Serial.println(F("=== Tinh dien tro base cho NPN 2N2222 (tai 200mA) ==="));
  const float Ic = 0.2, hFE = 100, Vbe = 0.7, Vpin = 5.0;
  float IbToiThieu = Ic / hFE;                 // 2mA
  float Ib = IbToiThieu * 5;                   // cho dư 5 lần -> transistor bão hòa (đóng hẳn)
  float Rb = (Vpin - Vbe) / Ib;
  Serial.print(F("Ib toi thieu = ")); Serial.print(IbToiThieu * 1000); Serial.println(F(" mA"));
  Serial.print(F("Ib chon (x5)  = ")); Serial.print(Ib * 1000); Serial.println(F(" mA"));
  Serial.print(F("Rb = (5 - 0.7) / Ib = ")); Serial.print(Rb); Serial.println(F(" ohm -> chon 330 ohm"));

  Serial.println(F("=== Nhiet toa ra tren MOSFET IRLZ44N (tai 3A) ==="));
  const float I = 3.0, RdsOn = 0.025;          // Rds(on) ~25 mOhm khi Vgs = 5V
  Serial.print(F("P = I^2 * Rds(on) = ")); Serial.print(I * I * RdsOn); Serial.println(F(" W -> khong can tan nhiet"));
  Serial.print(F("So sanh NPN (Vce sat 0.3V): P = 0.3 * I = ")); Serial.print(0.3 * I); Serial.println(F(" W -> nong"));
}

void setup() {
  pinMode(QUAT_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("Bai 29: quat 12V dieu toc bang MOSFET + PWM"));
  inTinhToan();
}

void loop() {
  unsigned long ms = millis();

  // ===== Đọc núm vặn -> tốc độ mong muốn (%) =====
  int phanTram = map(analogRead(BIEN_TRO_PIN), 0, 1023, 0, 100);
  int dutyMucTieu;
  if (phanTram < NGUONG_TAT) {
    dutyMucTieu = 0;
  } else {
    // 10..100% của núm -> 30..100% PWM (bỏ qua vùng quạt không quay nổi)
    dutyMucTieu = map(phanTram, NGUONG_TAT, 100, DUTY_TOI_THIEU * 255 / 100, 255);
  }

  // ===== Kick start: đang đứng yên mà được lệnh quay -> đá 100% một chút =====
  if (dutyHienTai == 0 && dutyMucTieu > 0 && !dangKick) {
    dangKick = true;
    lucKick = ms;
    analogWrite(QUAT_PIN, 255);
    Serial.println(F(">> Khoi dong: kick 100%"));
  }
  if (dangKick) {
    if (ms - lucKick < THOI_GIAN_KICK) return;     // đang đá, chưa làm gì khác
    dangKick = false;
    dutyHienTai = dutyMucTieu;                      // kick xong: nhảy thẳng tới mức yêu cầu
    if (dutyHienTai == 0) analogWrite(QUAT_PIN, 0); // núm bị vặn về 0 trong lúc kick
  }

  // ===== Ramp: tiến dần về mục tiêu, tránh giật và sụt áp nguồn =====
  if (ms - lanCuoiRamp >= BUOC_RAMP) {
    lanCuoiRamp = ms;
    if (dutyHienTai < dutyMucTieu) dutyHienTai++;
    else if (dutyHienTai > dutyMucTieu) dutyHienTai--;
    // dưới mức tối thiểu thì tắt hẳn, khỏi để động cơ "kêu ù" mà không quay
    if (dutyMucTieu == 0 && dutyHienTai < DUTY_TOI_THIEU * 255 / 100) dutyHienTai = 0;
    analogWrite(QUAT_PIN, dutyHienTai);
  }

  // ===== In trạng thái =====
  if (ms - lanCuoiIn >= 500) {
    lanCuoiIn = ms;
    Serial.print(F("Num: ")); Serial.print(phanTram);
    Serial.print(F("%  PWM: ")); Serial.print(dutyHienTai * 100 / 255);
    Serial.println(F("%"));
  }
}
