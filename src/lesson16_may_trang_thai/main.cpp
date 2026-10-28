// BÀI 16: Máy trạng thái (state machine) - đèn giao thông NGÃ TƯ có pha cho người đi bộ
// Mô phỏng: wokwi/wokwi.toml - 4 cột đèn ở 4 góc ngã tư (đi bên phải như Việt Nam:
// mỗi cột đứng ở góc bên phải của làn xe đi tới).
//
// Nối dây (mạch thật: MỖI LED nối tiếp 1 điện trở 220Ω xuống GND). Mỗi cột đèn 1 bộ chân riêng:
//   Bắc-Nam  - cột góc Tây-Bắc:  đỏ 13, vàng 12, xanh 11
//            - cột góc Đông-Nam: đỏ 7,  vàng 6,  xanh 5
//   Đông-Tây - cột góc Đông-Bắc: đỏ 10, vàng 9,  xanh 8
//            - cột góc Tây-Nam:  đỏ 4,  vàng 3,  xanh 2
//   Người đi bộ: đỏ A2, xanh A1  (chân analog dùng làm chân digital)
//   Nút xin qua đường: A3 (INPUT_PULLUP) và GND
//   LCD 1602 I2C: GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
//
// Hoạt động:
//   Bắc-Nam xanh 10s -> vàng 3s -> Đông-Tây xanh 8s -> vàng 3s -> lặp lại.
//   Có người NHẤN nút: hết pha vàng hiện tại thì TẤT CẢ xe dừng, người đi 7s, đèn người nháy 3s,
//   rồi chuyển cho hướng xe tiếp theo.
//   GIỮ nút 2 giây: bật/tắt chế độ ĐÊM (vàng nháy cả 2 hướng).
//
// Sơ đồ trạng thái:
//   BN_XANH -(10s)-> BN_VANG -(3s)-> [có người chờ? NGUOI_DI : DT_XANH]
//   DT_XANH -(8s)--> DT_VANG -(3s)-> [có người chờ? NGUOI_DI : BN_XANH]
//   NGUOI_DI -(7s)-> NGUOI_NHAY -(3s)-> hướng xe tiếp theo
//   (bất kỳ) -(giữ nút 2s)-> DEM -(giữ nút 2s)-> BN_XANH

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Mảng 2 chiều: [cột đèn][màu]. Mỗi hướng có 2 cột đối diện, mỗi cột 3 màu {đỏ, vàng, xanh}.
const int DO = 0, VANG = 1, XANH = 2;
const int BAC_NAM[2][3] = {{13, 12, 11}, {7, 6, 5}};
const int DONG_TAY[2][3] = {{10, 9, 8}, {4, 3, 2}};
const int NGUOI_DO = A2, NGUOI_XANH = A1;
const int NUT_PIN = A3;

const unsigned long THOI_GIAN_BN_XANH = 10000;
const unsigned long THOI_GIAN_DT_XANH = 8000;
const unsigned long THOI_GIAN_VANG = 3000;
const unsigned long THOI_GIAN_NGUOI_DI = 7000;
const unsigned long THOI_GIAN_NGUOI_NHAY = 3000;
const unsigned long THOI_GIAN_GIU_NUT = 2000;

// enum: đặt TÊN cho các số 0, 1, 2... -> code dễ đọc, không nhầm "trạng thái 3 là gì?"
enum TrangThai { BN_XANH, BN_VANG, DT_XANH, DT_VANG, NGUOI_DI, NGUOI_NHAY, DEM };
const char* TEN_TRANG_THAI[] = {
  "Bac-Nam: di     ", "Bac-Nam: vang   ", "Dong-Tay: di    ", "Dong-Tay: vang  ",
  "Nguoi qua duong ", "Nguoi nhanh len ", "Che do dem      "};
// Thời gian của mỗi trạng thái (0 = không tự hết giờ)
const unsigned long THOI_GIAN[] = {
  THOI_GIAN_BN_XANH, THOI_GIAN_VANG, THOI_GIAN_DT_XANH, THOI_GIAN_VANG,
  THOI_GIAN_NGUOI_DI, THOI_GIAN_NGUOI_NHAY, 0};

enum SuKienNut { KHONG_CO, NHAN_NGAN, NHAN_GIU };

LiquidCrystal_I2C lcd(0x27, 16, 2);

TrangThai trangThai;
TrangThai huongTiepTheo = BN_XANH;  // sau pha người đi bộ thì cho hướng nào đi
unsigned long thoiDiemVao;          // lúc bắt đầu trạng thái hiện tại
bool coNguoiCho = false;
unsigned long lanCuoiNhay = 0;
bool nhaySang = false;
unsigned long lanCuoiLcd = 0;

// --- Nút nhấn: chống dội + phân biệt nhấn ngắn / nhấn giữ ---
int lanDocTruoc = HIGH, trangThaiNut = HIGH;
unsigned long thoiDiemThayDoi = 0, thoiDiemNhan = 0;
bool daBaoGiu = false;

SuKienNut docNut() {
  unsigned long bayGio = millis();
  int doc = digitalRead(NUT_PIN);
  if (doc != lanDocTruoc) {
    thoiDiemThayDoi = bayGio;
    lanDocTruoc = doc;
  }
  if (bayGio - thoiDiemThayDoi >= 50 && doc != trangThaiNut) {
    trangThaiNut = doc;
    if (trangThaiNut == LOW) {           // vừa nhấn xuống
      thoiDiemNhan = bayGio;
      daBaoGiu = false;
    } else if (!daBaoGiu) {              // vừa nhả ra, chưa tính là giữ -> nhấn ngắn
      return NHAN_NGAN;
    }
  }
  if (trangThaiNut == LOW && !daBaoGiu && bayGio - thoiDiemNhan >= THOI_GIAN_GIU_NUT) {
    daBaoGiu = true;                     // đang giữ đủ lâu -> báo ngay, không chờ nhả
    return NHAN_GIU;
  }
  return KHONG_CO;
}

// Đặt đèn cho 1 hướng: cả 2 cột đối diện sáng giống nhau
void datHuong(const int huong[2][3], bool doBat, bool vangBat, bool xanhBat) {
  for (int cot = 0; cot < 2; cot++) {
    digitalWrite(huong[cot][DO], doBat);
    digitalWrite(huong[cot][VANG], vangBat);
    digitalWrite(huong[cot][XANH], xanhBat);
  }
}

// Chuyển trạng thái: làm "hành động khi vào" (entry action) đúng 1 lần
void vaoTrangThai(TrangThai moi) {
  trangThai = moi;
  thoiDiemVao = millis();
  // switch: chọn 1 nhánh theo giá trị. Mỗi case kết thúc bằng break, quên là chạy tràn xuống nhánh dưới!
  switch (moi) {
    case BN_XANH: datHuong(BAC_NAM, 0, 0, 1); datHuong(DONG_TAY, 1, 0, 0); break;
    case BN_VANG: datHuong(BAC_NAM, 0, 1, 0); datHuong(DONG_TAY, 1, 0, 0); break;
    case DT_XANH: datHuong(BAC_NAM, 1, 0, 0); datHuong(DONG_TAY, 0, 0, 1); break;
    case DT_VANG: datHuong(BAC_NAM, 1, 0, 0); datHuong(DONG_TAY, 0, 1, 0); break;
    case NGUOI_DI:
    case NGUOI_NHAY:  // 2 case dùng chung 1 nhánh: tất cả xe đỏ
      datHuong(BAC_NAM, 1, 0, 0);
      datHuong(DONG_TAY, 1, 0, 0);
      coNguoiCho = false;
      break;
    case DEM:
      datHuong(BAC_NAM, 0, 0, 0);
      datHuong(DONG_TAY, 0, 0, 0);
      coNguoiCho = false;
      break;
  }
  bool nguoiDuocDi = (moi == NGUOI_DI || moi == NGUOI_NHAY);
  digitalWrite(NGUOI_XANH, nguoiDuocDi);
  digitalWrite(NGUOI_DO, !nguoiDuocDi && moi != DEM);

  Serial.print("[");
  Serial.print(thoiDiemVao);
  Serial.print(" ms] -> ");
  Serial.println(TEN_TRANG_THAI[moi]);
  lcd.setCursor(0, 0);
  lcd.print(TEN_TRANG_THAI[moi]);
}

// Hết pha vàng: có người chờ thì cho người đi, không thì sang hướng kia
void hetPhaVang(TrangThai huongKia) {
  if (coNguoiCho) {
    huongTiepTheo = huongKia;
    vaoTrangThai(NGUOI_DI);
  } else {
    vaoTrangThai(huongKia);
  }
}

// Hướng này có đang cho xe đi không (xanh hoặc vàng ở BẤT KỲ cột nào)
bool dangChoDi(const int huong[2][3]) {
  for (int cot = 0; cot < 2; cot++) {
    if (digitalRead(huong[cot][XANH]) || digitalRead(huong[cot][VANG])) return true;
  }
  return false;
}

// Kiểm tra an toàn ĐỘC LẬP với máy trạng thái: đọc lại chính các chân đang xuất.
// Nếu 2 hướng cùng đi, hoặc người và xe cùng đi (do lỗi code) -> về chế độ an toàn ngay.
void kiemTraAnToan() {
  bool bnDi = dangChoDi(BAC_NAM);
  bool dtDi = dangChoDi(DONG_TAY);
  bool nguoiDi = digitalRead(NGUOI_XANH);
  if (trangThai != DEM && ((bnDi && dtDi) || (nguoiDi && (bnDi || dtDi)))) {
    Serial.println("!!! XUNG DOT DEN - chuyen ve che do an toan");
    vaoTrangThai(DEM);
  }
}

void setup() {
  for (int cot = 0; cot < 2; cot++) {
    for (int mau = 0; mau < 3; mau++) {  // 2 vòng for lồng nhau duyệt mảng 2 chiều
      pinMode(BAC_NAM[cot][mau], OUTPUT);
      pinMode(DONG_TAY[cot][mau], OUTPUT);
    }
  }
  pinMode(NGUOI_DO, OUTPUT);
  pinMode(NGUOI_XANH, OUTPUT);
  pinMode(NUT_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Bai 16: den giao thong nga tu - may trang thai");
  lcd.init();
  lcd.backlight();
  vaoTrangThai(BN_XANH);
}

void loop() {
  unsigned long bayGio = millis();
  unsigned long daQua = bayGio - thoiDiemVao;  // đã ở trạng thái hiện tại bao lâu

  // ===== 1. Sự kiện từ nút =====
  SuKienNut suKien = docNut();
  if (suKien == NHAN_GIU) {
    vaoTrangThai(trangThai == DEM ? BN_XANH : DEM);
    return;
  }
  if (suKien == NHAN_NGAN && trangThai != DEM && trangThai != NGUOI_DI && !coNguoiCho) {
    coNguoiCho = true;
    Serial.println("   Co nguoi nhan nut xin qua duong");
  }

  // ===== 2. Chuyển trạng thái khi hết giờ =====
  if (THOI_GIAN[trangThai] > 0 && daQua >= THOI_GIAN[trangThai]) {
    switch (trangThai) {
      case BN_XANH:    vaoTrangThai(BN_VANG); break;
      case BN_VANG:    hetPhaVang(DT_XANH); break;
      case DT_XANH:    vaoTrangThai(DT_VANG); break;
      case DT_VANG:    hetPhaVang(BN_XANH); break;
      case NGUOI_DI:   vaoTrangThai(NGUOI_NHAY); break;
      case NGUOI_NHAY: vaoTrangThai(huongTiepTheo); break;
      case DEM:        break;
    }
  }

  // ===== 3. Việc làm liên tục trong trạng thái (nháy đèn) =====
  if ((trangThai == NGUOI_NHAY || trangThai == DEM) && bayGio - lanCuoiNhay >= 400) {
    lanCuoiNhay = bayGio;
    nhaySang = !nhaySang;
    if (trangThai == NGUOI_NHAY) {
      digitalWrite(NGUOI_XANH, nhaySang);
    } else {
      datHuong(BAC_NAM, 0, nhaySang, 0);
      datHuong(DONG_TAY, 0, nhaySang, 0);
    }
  }

  kiemTraAnToan();

  // ===== 4. Dòng 2 của LCD: đếm ngược + có người chờ không =====
  if (bayGio - lanCuoiLcd >= 250) {
    lanCuoiLcd = bayGio;
    lcd.setCursor(0, 1);
    if (trangThai == DEM) {
      lcd.print("Giu nut 2s: tat ");
    } else {
      unsigned long conLai = (THOI_GIAN[trangThai] - (bayGio - thoiDiemVao) + 999) / 1000;  // làm tròn lên
      lcd.print("Con ");
      lcd.print(conLai);
      lcd.print("s ");
      lcd.print(coNguoiCho ? "Nguoi cho " : "          ");
    }
  }
}
