// BÀI 15: Động cơ DC + mạch cầu H L298N - chỉnh tốc độ (PWM) và đảo chiều
// Mô phỏng: wokwi/wokwi.toml - Wokwi không có động cơ DC / L298N nên dùng 3 LED thay:
//   LED vàng = ENA (độ sáng = tốc độ), LED xanh = IN1, LED đỏ = IN2 (chiều quay)
//
// Nối dây (mạch thật):
//   L298N ENA -> chân 5 (PWM)  [THÁO miếng jumper đang cắm trên chân ENA]
//   L298N IN1 -> chân 7,  IN2 -> chân 8
//   Động cơ -> OUT1, OUT2
//   Pin/nguồn ngoài (6-12V) -> cổng 12V của L298N,  GND của pin -> GND L298N
//   GND L298N -> GND Arduino   (BẮT BUỘC chung GND, nếu không tín hiệu không có mốc để so)
//   Biến trở: chân giữa -> A0, 2 chân ngoài -> 5V và GND
// KHÔNG lấy nguồn động cơ từ chân 5V của Arduino: động cơ kéo dòng lớn + gây nhiễu -> board bị reset.
//
// Cầu H: 4 công tắc bên trong L298N đảo chiều dòng điện qua động cơ:
//   IN1  IN2   động cơ
//   HIGH LOW   quay thuận
//   LOW  HIGH  quay ngược
//   LOW  LOW   dừng tự do (thả trôi)
//   HIGH HIGH  phanh (2 đầu động cơ bị nối chập -> dừng gấp)
// ENA = PWM: bật/tắt nguồn cho động cơ thật nhanh -> chỉnh tốc độ (giống làm mờ LED ở bài 6).

#include <Arduino.h>

const int ENA_PIN = 5;  // chân PWM (Timer0)
const int IN1_PIN = 7;
const int IN2_PIN = 8;
const int BIEN_TRO_PIN = A0;
const int VUNG_CHET = 50;

// Tăng / giảm tốc từ từ: đổi tốc độ đột ngột làm động cơ giật, kéo dòng rất lớn.
const int BUOC_TANG_TOC = 5;            // mỗi lần đổi tối đa 5 đơn vị PWM
const unsigned long NHIP_TANG_TOC = 20;  // mỗi 20ms -> 0 lên 255 mất ~1 giây

int tocDoHienTai = 0;  // -255 .. 255: dấu = chiều, độ lớn = tốc độ
unsigned long lanCuoiTangToc = 0;
unsigned long lanCuoiIn = 0;

// Ra lệnh cho L298N theo tốc độ có dấu
void dieuKhienDongCo(int tocDo) {
  if (tocDo > 0) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
  } else if (tocDo < 0) {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
  } else {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
  }
  analogWrite(ENA_PIN, abs(tocDo));
}

void setup() {
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Bai 15: dong co DC + L298N - vang bien tro chinh toc do / chieu");
}

void loop() {
  unsigned long bayGio = millis();

  // Biến trở -> tốc độ MONG MUỐN (-255..255), giữa = dừng
  int lech = analogRead(BIEN_TRO_PIN) - 512;
  int tocDoMucTieu = 0;
  if (lech > VUNG_CHET) {
    tocDoMucTieu = map(lech, VUNG_CHET, 511, 0, 255);
  } else if (lech < -VUNG_CHET) {
    tocDoMucTieu = map(lech, -VUNG_CHET, -512, 0, -255);
  }

  // Tiến dần tốc độ hiện tại về mục tiêu (đảo chiều thì tự giảm về 0 rồi mới tăng theo chiều kia)
  if (bayGio - lanCuoiTangToc >= NHIP_TANG_TOC) {
    lanCuoiTangToc = bayGio;
    if (tocDoHienTai < tocDoMucTieu) {
      tocDoHienTai = min(tocDoHienTai + BUOC_TANG_TOC, tocDoMucTieu);
    } else if (tocDoHienTai > tocDoMucTieu) {
      tocDoHienTai = max(tocDoHienTai - BUOC_TANG_TOC, tocDoMucTieu);
    }
    dieuKhienDongCo(tocDoHienTai);
  }

  if (bayGio - lanCuoiIn >= 300) {
    lanCuoiIn = bayGio;
    Serial.print("Muc tieu: ");
    Serial.print(tocDoMucTieu);
    Serial.print("\t Hien tai: ");
    Serial.print(tocDoHienTai);
    Serial.print("\t ");
    Serial.println(tocDoHienTai > 0 ? "QUAY THUAN" : (tocDoHienTai < 0 ? "QUAY NGUOC" : "DUNG"));
  }
}
