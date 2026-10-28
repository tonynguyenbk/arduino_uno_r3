// BÀI 14: Động cơ bước 28BYJ-48 + mạch ULN2003 - vặn biến trở để chỉnh tốc độ và chiều quay
// Mô phỏng: wokwi/wokwi.toml
//
// Nối dây (mạch thật):
//   ULN2003 IN1 -> 8,  IN2 -> 9,  IN3 -> 10,  IN4 -> 11
//   ULN2003 "+" (5-12V) -> 5V,  "-" -> GND.   Động cơ cắm vào ổ trắng 5 lỗ trên mạch ULN2003.
//   Biến trở: chân giữa -> A0, 2 chân ngoài -> 5V và GND
//
// Động cơ bước: quay từng BƯỚC nhỏ chính xác, bằng cách bật lần lượt các cuộn dây bên trong.
//   Không cần cảm biến vẫn biết đã quay bao nhiêu: đếm số bước là ra góc.
//   28BYJ-48: 2048 bước / vòng (có hộp số giảm tốc bên trong -> chậm nhưng khỏe, tối đa ~15 vòng/phút)
// ULN2003: Arduino chỉ cấp được ~20mA/chân, cuộn dây cần ~100-200mA -> ULN2003 làm "công tắc"
//   khuếch đại dòng: Arduino bật/tắt ULN2003, ULN2003 bật/tắt cuộn dây.
//
// Biến trở ở giữa = đứng yên; vặn sang phải = quay thuận, càng xa càng nhanh; sang trái = quay ngược.

#include <Arduino.h>
#include <Stepper.h>

// Wokwi chỉ có động cơ bước 200 bước/vòng. Board thật với 28BYJ-48: đổi thành 2048 và MAX 15.
const int BUOC_MOI_VONG = 200;
const int VONG_PHUT_TOI_DA = 60;

const int BIEN_TRO_PIN = A0;
const int VUNG_CHET = 50;  // quanh điểm giữa ±50 coi như đứng yên (tránh rung vì nhiễu ADC)

// Thứ tự chân IN1, IN3, IN2, IN4 (không phải 1,2,3,4!) - do cách thư viện bật cuộn dây
// khớp với cách ULN2003 nối vào 28BYJ-48.
Stepper dongCo(BUOC_MOI_VONG, 8, 10, 9, 11);

long tongSoBuoc = 0;  // long: số nguyên lớn (-2 tỉ .. 2 tỉ), quay nhiều vòng không bị tràn
unsigned long lanCuoiIn = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Bai 14: dong co buoc - vang bien tro chinh toc do / chieu");
}

void loop() {
  int lech = analogRead(BIEN_TRO_PIN) - 512;  // -512 .. +511, 0 = giữa

  if (abs(lech) > VUNG_CHET) {
    int vongPhut = map(abs(lech), VUNG_CHET, 512, 1, VONG_PHUT_TOI_DA);
    dongCo.setSpeed(vongPhut);
    int chieu = (lech > 0) ? 1 : -1;
    dongCo.step(chieu);  // đi đúng 1 bước. Hàm này CHỜ đủ thời gian của 1 bước rồi mới trả về.
    tongSoBuoc += chieu;
  }

  if (millis() - lanCuoiIn >= 500) {
    lanCuoiIn = millis();
    float goc = tongSoBuoc * 360.0 / BUOC_MOI_VONG;
    Serial.print("Tong so buoc: ");
    Serial.print(tongSoBuoc);
    Serial.print("\t = ");
    Serial.print(goc, 1);
    Serial.println(" do");
  }
}
