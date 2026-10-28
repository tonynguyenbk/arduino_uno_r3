// BÀI 21: LED ma trận 8x8 + chip MAX7219 - vẽ hình, hoạt hình, chữ chạy
// Mô phỏng: wokwi/wokwi.toml
//
// Nối dây (5 chân phía "IN" của module): VCC -> 5V, GND -> GND, DIN -> 11, CS -> 10, CLK -> 13
//
// 64 LED xếp 8 hàng x 8 cột. Điều khiển thẳng cần 16 chân và phải QUÉT liên tục (bài 9).
// Chip MAX7219 tự quét + tự giới hạn dòng; Arduino chỉ gửi dữ liệu nối tiếp qua 3 dây:
//   mỗi lệnh 16 bit = [4 bit bỏ qua][4 bit địa chỉ: hàng 1..8 hoặc thanh ghi cài đặt][8 bit dữ liệu]
//   bit dữ liệu = 1 -> LED ở cột đó sáng.  (giao thức gần giống SPI - bài 22, 28)
//
// BỘ ĐỆM KHUNG HÌNH: mảng manHinh[8], mỗi byte = 1 hàng, mỗi bit = 1 LED.
//   Vẽ vào mảng trước, xong mới gửi cả khung ra LED -> không bị nhấp nháy dở dang.
//   Module mỗi loại lắp xoay 1 kiểu: hình bị xoay/lật thì chỉnh XOAY_90 / LAT_NGANG / LAT_DOC.

#include <Arduino.h>
#include <LedControl.h>
#include <avr/pgmspace.h>

const int DIN_PIN = 11, CLK_PIN = 13, CS_PIN = 10;
const bool XOAY_90 = false;
const bool LAT_NGANG = false;
const bool LAT_DOC = false;

LedControl matran(DIN_PIN, CLK_PIN, CS_PIN, 1);  // 1 module

byte manHinh[8];  // bộ đệm: manHinh[hàng], bit 7 = cột trái ... bit 0 = cột phải

// Hình 8x8 vẽ bằng số nhị phân: nhìn thấy luôn hình dạng trong code
const byte TRAI_TIM[8] = {
  0b00000000,
  0b01100110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000,
};
const byte MAT_CUOI[8] = {
  0b00111100,
  0b01000010,
  0b10100101,
  0b10000001,
  0b10100101,
  0b10011001,
  0b01000010,
  0b00111100,
};

// Phông chữ 5 cột x 7 hàng cho A..Z và dấu cách. Mỗi chữ = 5 byte, mỗi byte = 1 CỘT (bit 0 = hàng trên).
// PROGMEM: để mảng trong bộ nhớ Flash (32KB) thay vì RAM (chỉ 2KB). Đọc ra bằng pgm_read_byte().
const byte PHONG_CHU[27][5] PROGMEM = {
  {0x00, 0x00, 0x00, 0x00, 0x00},  // dấu cách
  {0x7E, 0x09, 0x09, 0x09, 0x7E},  // A
  {0x7F, 0x49, 0x49, 0x49, 0x36},  // B
  {0x3E, 0x41, 0x41, 0x41, 0x22},  // C
  {0x7F, 0x41, 0x41, 0x22, 0x1C},  // D
  {0x7F, 0x49, 0x49, 0x49, 0x41},  // E
  {0x7F, 0x09, 0x09, 0x09, 0x01},  // F
  {0x3E, 0x41, 0x49, 0x49, 0x7A},  // G
  {0x7F, 0x08, 0x08, 0x08, 0x7F},  // H
  {0x00, 0x41, 0x7F, 0x41, 0x00},  // I
  {0x20, 0x40, 0x41, 0x3F, 0x01},  // J
  {0x7F, 0x08, 0x14, 0x22, 0x41},  // K
  {0x7F, 0x40, 0x40, 0x40, 0x40},  // L
  {0x7F, 0x02, 0x0C, 0x02, 0x7F},  // M
  {0x7F, 0x04, 0x08, 0x10, 0x7F},  // N
  {0x3E, 0x41, 0x41, 0x41, 0x3E},  // O
  {0x7F, 0x09, 0x09, 0x09, 0x06},  // P
  {0x3E, 0x41, 0x51, 0x21, 0x5E},  // Q
  {0x7F, 0x09, 0x19, 0x29, 0x46},  // R
  {0x46, 0x49, 0x49, 0x49, 0x31},  // S
  {0x01, 0x01, 0x7F, 0x01, 0x01},  // T
  {0x3F, 0x40, 0x40, 0x40, 0x3F},  // U
  {0x1F, 0x20, 0x40, 0x20, 0x1F},  // V
  {0x3F, 0x40, 0x38, 0x40, 0x3F},  // W
  {0x63, 0x14, 0x08, 0x14, 0x63},  // X
  {0x07, 0x08, 0x70, 0x08, 0x07},  // Y
  {0x61, 0x51, 0x49, 0x45, 0x43},  // Z
};
const char* CHU_CHAY = "XIN CHAO ARDUINO   ";

// ===== Bộ đệm -> LED (có xoay / lật) =====
bool layDiem(int hang, int cot) {
  return (manHinh[hang] >> (7 - cot)) & 1;
}

void xuatRaLed() {
  for (int hang = 0; hang < 8; hang++) {
    byte duLieu = 0;
    for (int cot = 0; cot < 8; cot++) {
      int h = hang, c = cot;
      if (XOAY_90) { int tam = h; h = 7 - c; c = tam; }
      if (LAT_NGANG) c = 7 - c;
      if (LAT_DOC) h = 7 - h;
      if (layDiem(h, c)) duLieu |= (1 << (7 - cot));  // phép toán bit: bật bit của cột này
    }
    matran.setRow(0, hang, duLieu);
  }
}

void veHinh(const byte hinh[8]) {
  for (int i = 0; i < 8; i++) manHinh[i] = hinh[i];
  xuatRaLed();
}

// ===== Chữ chạy: dịch cả khung sang trái 1 cột, thêm 1 cột mới ở bên phải =====
int viTriChu = 0;    // đang ở ký tự thứ mấy
int cotTrongChu = 0; // cột thứ mấy của ký tự đó (0..4 = nét chữ, 5 = khoảng trống giữa 2 chữ)

byte layCotKeTiep() {
  char kyTu = CHU_CHAY[viTriChu];
  int chiSo = (kyTu == ' ') ? 0 : kyTu - 'A' + 1;  // 'A' -> 1, 'B' -> 2 ...
  byte cot = (cotTrongChu < 5) ? pgm_read_byte(&PHONG_CHU[chiSo][cotTrongChu]) : 0;
  cotTrongChu++;
  if (cotTrongChu > 5) {
    cotTrongChu = 0;
    viTriChu++;
    if (CHU_CHAY[viTriChu] == '\0') viTriChu = 0;  // '\0' = ký tự kết thúc chuỗi -> quay lại đầu
  }
  return cot;
}

void dichChu() {
  byte cotMoi = layCotKeTiep();
  for (int hang = 0; hang < 8; hang++) {
    manHinh[hang] <<= 1;                                  // dịch hàng sang trái 1 cột
    if ((cotMoi >> hang) & 1) manHinh[hang] |= 1;         // điểm mới ở cột phải cùng
  }
  xuatRaLed();
}

// ===== Chương trình: tim đập -> mặt cười -> chữ chạy, lặp lại =====
enum Canh { TIM_DAP, CUOI, CHU };
Canh canh = TIM_DAP;
unsigned long batDauCanh = 0, lanCuoi = 0;
int buocHoatHinh = 0;

void doiCanh(Canh moi) {
  canh = moi;
  batDauCanh = millis();
  buocHoatHinh = 0;
  for (int i = 0; i < 8; i++) manHinh[i] = 0;
  viTriChu = 0;
  cotTrongChu = 0;
  Serial.println(moi == TIM_DAP ? "Tim dap" : (moi == CUOI ? "Mat cuoi" : "Chu chay"));
}

void setup() {
  Serial.begin(9600);
  Serial.println("Bai 21: LED ma tran 8x8 MAX7219");
  matran.shutdown(0, false);   // MAX7219 khởi động ở chế độ ngủ -> đánh thức
  matran.setIntensity(0, 4);   // độ sáng 0..15 (MAX7219 tự PWM)
  matran.clearDisplay(0);
  doiCanh(TIM_DAP);
}

void loop() {
  unsigned long bayGio = millis();
  unsigned long daQua = bayGio - batDauCanh;

  switch (canh) {
    case TIM_DAP:
      // Tim đập: đổi độ sáng theo nhịp, không cần vẽ lại hình
      if (bayGio - lanCuoi >= 120) {
        lanCuoi = bayGio;
        if (buocHoatHinh == 0) veHinh(TRAI_TIM);
        const byte NHIP[] = {15, 4, 15, 2, 1, 1, 1, 1};
        matran.setIntensity(0, NHIP[buocHoatHinh % 8]);
        buocHoatHinh++;
      }
      if (daQua >= 4000) { matran.setIntensity(0, 4); doiCanh(CUOI); }
      break;
    case CUOI:
      if (buocHoatHinh == 0) { veHinh(MAT_CUOI); buocHoatHinh = 1; }
      if (daQua >= 2000) doiCanh(CHU);
      break;
    case CHU:
      if (bayGio - lanCuoi >= 80) {
        lanCuoi = bayGio;
        dichChu();
        if (viTriChu == 0 && cotTrongChu == 0 && daQua > 1000) doiCanh(TIM_DAP);
      }
      break;
  }
}
