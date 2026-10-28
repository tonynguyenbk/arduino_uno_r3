// BÀI 22: RFID RC522 + thẻ từ - khóa cửa thẻ từ có thẻ chủ để thêm/xóa thẻ, lưu EEPROM
// Mô phỏng: wokwi/wokwi.toml - Wokwi KHÔNG có đầu đọc RC522 -> 3 nút nhấn giả lập 3 lần quẹt thẻ.
//
// Nối dây (mạch thật):
//   RC522: SDA(SS) -> 10, SCK -> 13, MOSI -> 11, MISO -> 12, RST -> 9, GND -> GND,
//          3.3V -> 3.3V  (CHÚ Ý: RC522 chạy 3.3V, KHÔNG cắm vào 5V!)   chân IRQ để trống.
//   LED xanh (cửa mở) -> 4,  LED đỏ (từ chối) -> 5  (mỗi LED 1 điện trở 220Ω),  chuông -> 8
//   Mô phỏng: 3 nút ở A1 (thẻ chủ), A2 (thẻ A), A3 (thẻ B), nút còn lại nối GND.
//
// RFID: đầu đọc phát sóng 13.56MHz qua cuộn dây ăng-ten. Thẻ/móc khóa không có pin: lấy năng lượng
//   từ chính sóng đó, rồi gửi lại mã UID (thường 4 byte, mỗi thẻ 1 mã khi sản xuất).
// SPI: Arduino nói chuyện với RC522 qua 4 dây: SCK (nhịp), MOSI (Arduino gửi), MISO (RC522 trả lời),
//   SS (chọn chip). Gửi và nhận CÙNG LÚC, từng bit theo nhịp SCK - nhanh hơn I2C nhiều.
//
// Cách dùng:
//   - Quẹt thẻ có trong danh sách -> đèn xanh 3 giây + bíp cao (mở cửa). Thẻ lạ -> đèn đỏ + bíp trầm.
//   - Quẹt THẺ CHỦ -> vào chế độ HỌC (2 đèn nháy). Quẹt 1 thẻ: chưa có thì THÊM, có rồi thì XÓA.
//     Quẹt thẻ chủ lần nữa hoặc chờ 10 giây -> thoát chế độ học.
//   - Danh sách lưu trong EEPROM: rút điện cắm lại vẫn còn.

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

// Chạy mô phỏng: true (dùng 3 nút).  Nạp vào board thật có RC522: false.
const bool MO_PHONG = true;

const int SS_PIN = 10, RST_PIN = 9;
const int LED_XANH = 4, LED_DO = 5, CHUONG = 8;
const int NUT_GIA_LAP[3] = {A1, A2, A3};

// Thẻ chủ: mạch thật -> quẹt thẻ của bạn, đọc UID trên Serial rồi sửa dòng này
const byte THE_CHU[4] = {0xDE, 0xAD, 0xBE, 0xEF};
// UID giả cho 3 nút mô phỏng: nút 1 = thẻ chủ, nút 2 = thẻ A, nút 3 = thẻ B
const byte UID_GIA_LAP[3][4] = {{0xDE, 0xAD, 0xBE, 0xEF}, {0x12, 0x34, 0x56, 0x78}, {0xA1, 0xB2, 0xC3, 0xD4}};

// ----- EEPROM: byte 0 = dấu hiệu đã khởi tạo, byte 1 = số thẻ, từ byte 2: mỗi thẻ 4 byte -----
const byte DAU_HIEU = 0x42;
const int SO_THE_TOI_DA = 10;

MFRC522 dauDoc(SS_PIN, RST_PIN);

enum CheDo { BINH_THUONG, HOC_THE };
CheDo cheDo = BINH_THUONG;
unsigned long batDauHoc = 0, lanCuoiNhay = 0, lucMoCua = 0, lanQuetCuoi = 0;
bool cuaDangMo = false, nhay = false;
int nutTruoc[3] = {HIGH, HIGH, HIGH};

int soThe() { return EEPROM.read(1); }

void docThe(int viTri, byte uid[4]) {
  for (int i = 0; i < 4; i++) uid[i] = EEPROM.read(2 + viTri * 4 + i);
}

bool giongNhau(const byte a[4], const byte b[4]) {
  for (int i = 0; i < 4; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

// Trả về vị trí thẻ trong danh sách, -1 nếu không có
int timThe(const byte uid[4]) {
  byte daLuu[4];
  for (int i = 0; i < soThe(); i++) {
    docThe(i, daLuu);
    if (giongNhau(uid, daLuu)) return i;
  }
  return -1;
}

void themThe(const byte uid[4]) {
  int n = soThe();
  if (n >= SO_THE_TOI_DA) {
    Serial.println("  Danh sach day!");
    return;
  }
  // EEPROM.update: chỉ ghi khi giá trị khác -> đỡ hao EEPROM (mỗi ô chịu ~100.000 lần ghi)
  for (int i = 0; i < 4; i++) EEPROM.update(2 + n * 4 + i, uid[i]);
  EEPROM.update(1, n + 1);
}

void xoaThe(int viTri) {
  int n = soThe();
  byte cuoi[4];
  docThe(n - 1, cuoi);  // chép thẻ cuối vào chỗ bị xóa, rồi bớt 1 -> không phải dời cả danh sách
  for (int i = 0; i < 4; i++) EEPROM.update(2 + viTri * 4 + i, cuoi[i]);
  EEPROM.update(1, n - 1);
}

void inUid(const byte uid[4]) {
  for (int i = 0; i < 4; i++) {
    if (uid[i] < 0x10) Serial.print('0');
    Serial.print(uid[i], HEX);
    Serial.print(i < 3 ? " " : "");
  }
}

// Lấy UID của lần quẹt mới. Trả về true nếu có thẻ.
bool coTheMoi(byte uid[4]) {
  if (MO_PHONG) {
    for (int i = 0; i < 3; i++) {
      int doc = digitalRead(NUT_GIA_LAP[i]);
      bool vuaNhan = (doc == LOW && nutTruoc[i] == HIGH);
      nutTruoc[i] = doc;
      if (vuaNhan) {
        for (int j = 0; j < 4; j++) uid[j] = UID_GIA_LAP[i][j];
        return true;
      }
    }
    return false;
  }
  if (!dauDoc.PICC_IsNewCardPresent() || !dauDoc.PICC_ReadCardSerial()) return false;
  for (int j = 0; j < 4; j++) uid[j] = dauDoc.uid.uidByte[j];
  dauDoc.PICC_HaltA();  // bảo thẻ "ngủ" -> không đọc lặp lại cùng 1 lần đặt thẻ
  return true;
}

void xuLyThe(const byte uid[4]) {
  Serial.print("Quet the: ");
  inUid(uid);
  Serial.println();

  if (giongNhau(uid, THE_CHU)) {
    cheDo = (cheDo == HOC_THE) ? BINH_THUONG : HOC_THE;
    batDauHoc = millis();
    tone(CHUONG, 1500, 100);
    Serial.println(cheDo == HOC_THE ? "  >> CHE DO HOC: quet the can them/xoa" : "  >> Thoat che do hoc");
    if (cheDo == BINH_THUONG) { digitalWrite(LED_XANH, LOW); digitalWrite(LED_DO, LOW); }
    return;
  }

  int viTri = timThe(uid);
  if (cheDo == HOC_THE) {
    if (viTri >= 0) {
      xoaThe(viTri);
      Serial.println("  >> DA XOA the khoi danh sach");
    } else {
      themThe(uid);
      Serial.println("  >> DA THEM the vao danh sach");
    }
    Serial.print("  So the dang luu: ");
    Serial.println(soThe());
    tone(CHUONG, 2000, 300);
    batDauHoc = millis();  // gia hạn chế độ học
    return;
  }

  if (viTri >= 0) {
    Serial.println("  >> HOP LE - MO CUA");
    digitalWrite(LED_XANH, HIGH);
    digitalWrite(LED_DO, LOW);
    tone(CHUONG, 2500, 150);
    cuaDangMo = true;
    lucMoCua = millis();
  } else {
    Serial.println("  >> THE LA - TU CHOI");
    digitalWrite(LED_DO, HIGH);
    tone(CHUONG, 300, 600);
    cuaDangMo = true;  // dùng chung bộ đếm 3 giây để tắt đèn đỏ
    lucMoCua = millis();
  }
}

void setup() {
  pinMode(LED_XANH, OUTPUT);
  pinMode(LED_DO, OUTPUT);
  for (int i = 0; i < 3; i++) pinMode(NUT_GIA_LAP[i], INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Bai 22: khoa cua the tu RFID");

  if (EEPROM.read(0) != DAU_HIEU) {  // lần chạy đầu: EEPROM còn rác -> khởi tạo danh sách rỗng
    EEPROM.update(0, DAU_HIEU);
    EEPROM.update(1, 0);
  }
  Serial.print("So the dang luu trong EEPROM: ");
  Serial.println(soThe());

  if (!MO_PHONG) {
    SPI.begin();
    dauDoc.PCD_Init();
    Serial.print("RC522 phien ban chip: 0x");
    Serial.println(dauDoc.PCD_ReadRegister(MFRC522::VersionReg), HEX);  // 0x91/0x92 = OK, 0x00/0xFF = sai dây
  } else {
    Serial.println("MO PHONG: nut 1 = the chu, nut 2 = the A, nut 3 = the B");
  }
}

void loop() {
  unsigned long bayGio = millis();
  byte uid[4];

  if (bayGio - lanQuetCuoi >= 300 && coTheMoi(uid)) {
    lanQuetCuoi = bayGio;
    xuLyThe(uid);
  }

  // Đóng cửa / tắt đèn sau 3 giây
  if (cuaDangMo && bayGio - lucMoCua >= 3000) {
    cuaDangMo = false;
    digitalWrite(LED_XANH, LOW);
    digitalWrite(LED_DO, LOW);
    Serial.println("  (dong cua)");
  }

  // Chế độ học: 2 đèn nháy luân phiên, quá 10 giây thì tự thoát
  if (cheDo == HOC_THE) {
    if (bayGio - lanCuoiNhay >= 250) {
      lanCuoiNhay = bayGio;
      nhay = !nhay;
      digitalWrite(LED_XANH, nhay);
      digitalWrite(LED_DO, !nhay);
    }
    if (bayGio - batDauHoc >= 10000) {
      cheDo = BINH_THUONG;
      digitalWrite(LED_XANH, LOW);
      digitalWrite(LED_DO, LOW);
      Serial.println("  >> Het gio, thoat che do hoc");
    }
  }
}
