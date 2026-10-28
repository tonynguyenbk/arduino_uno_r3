// BÀI 23: Bàn phím ma trận 4x4 - tự viết hàm quét, làm khóa cửa mật mã
// Mô phỏng: wokwi/wokwi.toml (bấm chuột vào các phím)
//
// Nối dây:
//   Bàn phím: R1 -> 9, R2 -> 8, R3 -> 7, R4 -> 6, C1 -> 5, C2 -> 4, C3 -> 3, C4 -> 2
//     (bàn phím xanh của kit: hàng chân ghi C4 C3 C2 C1 R1 R2 R3 R4)
//   LCD 1602 I2C: GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5
//   Chuông: (+) -> 10, (-) -> GND
//
// 16 phím nằm ở giao điểm 4 HÀNG x 4 CỘT. Nhấn 1 phím = nối hàng đó với cột đó.
// Cách QUÉT (scan):
//   - 4 chân cột là INPUT_PULLUP (bình thường đọc HIGH).
//   - Lần lượt kéo TỪNG HÀNG xuống LOW (3 hàng kia để HIGH), rồi đọc 4 cột:
//     cột nào đọc LOW -> phím ở (hàng đang kéo, cột đó) đang được nhấn.
//   Quét 4 hàng mất vài chục micro giây -> làm hàng trăm lần mỗi giây, không sót phím nào.
//
// Khóa mật mã: nhập số rồi nhấn # để mở. * = xóa. Sai 3 lần -> khóa bàn phím 30 giây.
// Đang mở cửa, nhấn A -> đổi mật mã mới (nhập mã mới + #), lưu vào EEPROM.

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

const int CHAN_HANG[4] = {9, 8, 7, 6};
const int CHAN_COT[4] = {5, 4, 3, 2};
const char BAN_PHIM[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};
const int CHUONG = 10;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== Quét bàn phím =====
// Trả về ký tự của phím đang nhấn, hoặc 0 nếu không có phím nào
char quetPhim() {
  for (int h = 0; h < 4; h++) {
    digitalWrite(CHAN_HANG[h], LOW);           // chỉ hàng này LOW
    delayMicroseconds(5);                      // chờ tín hiệu ổn định
    for (int c = 0; c < 4; c++) {
      if (digitalRead(CHAN_COT[c]) == LOW) {
        digitalWrite(CHAN_HANG[h], HIGH);
        return BAN_PHIM[h][c];
      }
    }
    digitalWrite(CHAN_HANG[h], HIGH);          // trả hàng về HIGH trước khi sang hàng sau
  }
  return 0;
}

// Chống dội + chỉ báo 1 lần mỗi lần nhấn (giống bài 4-5, nhưng cho cả bàn phím)
char phimTruoc = 0, phimOnDinh = 0;
unsigned long thoiDiemDoi = 0;

char docPhimMoi() {
  char phim = quetPhim();
  if (phim != phimTruoc) {
    phimTruoc = phim;
    thoiDiemDoi = millis();
  }
  if (millis() - thoiDiemDoi >= 30 && phim != phimOnDinh) {
    phimOnDinh = phim;
    return phim;  // trả về phím vừa nhấn (hoặc 0 khi vừa nhả - bỏ qua ở dưới)
  }
  return 0;
}

// ===== Mật mã trong EEPROM: byte 0 = dấu hiệu, byte 1 = độ dài, từ byte 2 = các chữ số =====
const byte DAU_HIEU = 0x23;
const int DO_DAI_TOI_DA = 8;
char matMa[DO_DAI_TOI_DA + 1];  // +1 cho ký tự kết thúc chuỗi '\0'

void docMatMa() {
  if (EEPROM.read(0) != DAU_HIEU) {  // lần đầu: mật mã mặc định 1234
    const char* macDinh = "1234";
    EEPROM.update(0, DAU_HIEU);
    EEPROM.update(1, 4);
    for (int i = 0; i < 4; i++) EEPROM.update(2 + i, macDinh[i]);
  }
  int doDai = EEPROM.read(1);
  for (int i = 0; i < doDai; i++) matMa[i] = EEPROM.read(2 + i);
  matMa[doDai] = '\0';
}

void luuMatMa(const char* moi) {
  int doDai = strlen(moi);  // strlen: đếm số ký tự tới '\0'
  EEPROM.update(1, doDai);
  for (int i = 0; i < doDai; i++) EEPROM.update(2 + i, moi[i]);
  strcpy(matMa, moi);       // strcpy: chép chuỗi
}

// ===== Máy trạng thái (bài 16) =====
enum TrangThai { NHAP_MA, DA_MO, DOI_MA, BI_KHOA };
TrangThai trangThai;
char daNhap[DO_DAI_TOI_DA + 1];
int soKyTu = 0;
int soLanSai = 0;
unsigned long thoiDiemVao = 0;

void hienDong(int dong, const char* chu) {
  lcd.setCursor(0, dong);
  lcd.print(chu);
  for (int i = strlen(chu); i < 16; i++) lcd.print(' ');  // xóa phần còn lại của dòng
}

void hienSoDaNhap() {
  char sao[17];
  for (int i = 0; i < soKyTu; i++) sao[i] = (trangThai == DOI_MA) ? daNhap[i] : '*';  // mã mới thì hiện số
  sao[soKyTu] = '\0';
  hienDong(1, sao);
}

void vao(TrangThai moi) {
  trangThai = moi;
  thoiDiemVao = millis();
  soKyTu = 0;
  switch (moi) {
    case NHAP_MA: hienDong(0, "Nhap ma, # = OK"); hienDong(1, ""); break;
    case DA_MO:   hienDong(0, "CUA DA MO"); hienDong(1, "A = doi ma"); break;
    case DOI_MA:  hienDong(0, "Ma moi, # = luu"); hienDong(1, ""); break;
    case BI_KHOA: hienDong(0, "SAI 3 LAN!"); break;
  }
}

void xuLyPhim(char phim) {
  tone(CHUONG, 2000, 30);  // tiếng "tách" mỗi lần bấm
  Serial.print("Phim: ");
  Serial.println(phim);

  if (trangThai == DA_MO) {
    if (phim == 'A') vao(DOI_MA);
    return;
  }
  if (phim == '*') {  // xóa
    soKyTu = 0;
    hienSoDaNhap();
    return;
  }
  if (phim == '#') {  // xác nhận
    daNhap[soKyTu] = '\0';
    if (trangThai == DOI_MA) {
      if (soKyTu >= 4) {
        luuMatMa(daNhap);
        hienDong(0, "Da luu ma moi");
        Serial.println("Doi ma thanh cong");
        tone(CHUONG, 2500, 300);
        vao(NHAP_MA);
      } else {
        hienDong(0, "Ma it nhat 4 so");
      }
      return;
    }
    if (strcmp(daNhap, matMa) == 0) {  // strcmp: so sánh 2 chuỗi, bằng nhau trả về 0
      Serial.println("DUNG MA - mo cua");
      soLanSai = 0;
      tone(CHUONG, 2500, 300);
      vao(DA_MO);
    } else {
      soLanSai++;
      Serial.print("SAI MA, lan ");
      Serial.println(soLanSai);
      tone(CHUONG, 300, 500);
      if (soLanSai >= 3) {
        vao(BI_KHOA);
      } else {
        hienDong(0, "Sai ma! Nhap lai");
        soKyTu = 0;
        hienSoDaNhap();
      }
    }
    return;
  }
  if (phim >= '0' && phim <= '9' && soKyTu < DO_DAI_TOI_DA) {  // chỉ nhận chữ số
    daNhap[soKyTu++] = phim;
    hienSoDaNhap();
  }
}

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(CHAN_HANG[i], OUTPUT);
    digitalWrite(CHAN_HANG[i], HIGH);
    pinMode(CHAN_COT[i], INPUT_PULLUP);
  }
  Serial.begin(9600);
  Serial.println("Bai 23: ban phim 4x4 - khoa mat ma (ma mac dinh 1234)");
  lcd.init();
  lcd.backlight();
  docMatMa();
  vao(NHAP_MA);
}

void loop() {
  unsigned long daQua = millis() - thoiDiemVao;

  if (trangThai == DA_MO && daQua >= 5000) vao(NHAP_MA);       // tự khóa lại sau 5 giây
  if (trangThai == DOI_MA && daQua >= 15000) vao(NHAP_MA);     // bỏ dở đổi mã
  if (trangThai == BI_KHOA) {
    static unsigned long lanCuoiHien = 0;  // static: giữ giá trị giữa các lần gọi loop()
    if (millis() - lanCuoiHien >= 500) {
      lanCuoiHien = millis();
      char dong[17];
      snprintf(dong, sizeof(dong), "Cho %lus", (30000 - daQua) / 1000 + 1);  // ghép chuỗi có số
      hienDong(1, dong);
    }
    if (daQua >= 30000) {
      soLanSai = 0;
      vao(NHAP_MA);
    }
    return;  // đang bị khóa: bỏ qua bàn phím
  }

  char phim = docPhimMoi();
  if (phim != 0) xuLyPhim(phim);
}
