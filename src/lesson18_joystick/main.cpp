// BÀI 18: Joystick - "bảng vẽ điện tử" trên màn OLED
// Mô phỏng: wokwi/wokwi.toml (kéo cần joystick bằng chuột, bấm vào giữa = nhấn núm)
//
// Nối dây:
//   Joystick: VCC (+5V) -> 5V, GND -> GND, VRx/HORZ -> A2, VRy/VERT -> A3, SW/SEL -> A1
//   OLED I2C: GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
//
// Joystick = 2 BIẾN TRỞ (bài 7) đặt vuông góc + 1 NÚT NHẤN (bài 4) dưới cần gạt:
//   - Trục ngang và trục dọc: mỗi trục đọc 0..1023, thả tay thì về giữa (~512)
//   - Ấn cần xuống: nút SW nối xuống GND (dùng INPUT_PULLUP, nhấn = LOW)
// Gạt càng xa tâm -> bút chạy càng nhanh. Nhấn núm -> xóa màn hình.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

const int TRUC_NGANG = A2;
const int TRUC_DOC = A3;
const int NUT_SW = A1;

const int VUNG_CHET = 60;      // quanh tâm ±60 coi như đứng yên (joystick thả tay không về đúng 512)
const int TOC_DO_TOI_DA = 3;   // điểm ảnh mỗi lần cập nhật
// Mỗi joystick lắp một kiểu: nếu gạt lên mà bút đi xuống thì đổi thành true
const bool DAO_TRUC_DOC = false;
const bool DAO_TRUC_NGANG = false;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

int butX = 64, butY = 32;  // vị trí bút trên màn 128 x 64
unsigned long lanCuoiVe = 0, lanCuoiIn = 0;
int lanDocTruoc = HIGH, trangThaiNut = HIGH;
unsigned long thoiDiemThayDoi = 0;

// Đổi giá trị 1 trục (0..1023) thành số điểm ảnh cần di chuyển (-3..+3), có vùng chết
int tinhBuoc(int giaTri, bool dao) {
  int lech = giaTri - 512;
  if (dao) lech = -lech;
  if (abs(lech) <= VUNG_CHET) return 0;
  int buoc = map(abs(lech), VUNG_CHET, 512, 1, TOC_DO_TOI_DA);
  return lech > 0 ? buoc : -buoc;
}

bool nutVuaNhan() {
  int doc = digitalRead(NUT_SW);
  if (doc != lanDocTruoc) {
    thoiDiemThayDoi = millis();
    lanDocTruoc = doc;
  }
  if (millis() - thoiDiemThayDoi >= 50 && doc != trangThaiNut) {
    trangThaiNut = doc;
    return trangThaiNut == LOW;
  }
  return false;
}

void xoaManHinh() {
  oled.clearDisplay();
  oled.drawRect(0, 0, 128, 64, SSD1306_WHITE);  // khung viền
  oled.display();
}

void setup() {
  pinMode(NUT_SW, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Bai 18: joystick - bang ve tren OLED");
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Khong tim thay OLED! Kiem tra day SDA/SCL.");
  }
  Wire.setClock(400000);  // I2C nhanh 400kHz: đẩy 1KB ra màn hình mất ~25ms thay vì ~100ms
  xoaManHinh();
}

void loop() {
  if (nutVuaNhan()) {
    xoaManHinh();
    Serial.println("Xoa man hinh");
  }

  int ngang = analogRead(TRUC_NGANG);
  int doc = analogRead(TRUC_DOC);

  // Cập nhật vẽ mỗi 40ms (~25 lần/giây)
  if (millis() - lanCuoiVe >= 40) {
    lanCuoiVe = millis();
    int dx = tinhBuoc(ngang, DAO_TRUC_NGANG);
    int dy = -tinhBuoc(doc, DAO_TRUC_DOC);  // màn hình: y tăng là đi XUỐNG, nên đảo dấu
    if (dx != 0 || dy != 0) {
      butX = constrain(butX + dx, 1, 126);  // không cho bút ra khỏi khung
      butY = constrain(butY + dy, 1, 62);
      oled.fillRect(butX - 1, butY - 1, 2, 2, SSD1306_WHITE);  // nét bút dày 2 điểm
      oled.display();
    }
  }

  if (millis() - lanCuoiIn >= 300) {
    lanCuoiIn = millis();
    Serial.print("Ngang: ");
    Serial.print(ngang);
    Serial.print("\t Doc: ");
    Serial.print(doc);
    Serial.print("\t But: (");
    Serial.print(butX);
    Serial.print(", ");
    Serial.print(butY);
    Serial.println(")");
  }
}
