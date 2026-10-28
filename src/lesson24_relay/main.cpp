// BÀI 24: Relay - khóa cửa điện từ: nhập mã đúng / nhấn nút bên trong -> relay đóng -> khóa mở 5 giây
// Mô phỏng: wokwi/wokwi.toml (LED vàng = khóa điện từ, sáng = đang mở khóa)
//
// Nối dây:
//   Bàn phím, LCD, chuông: giống bài 23 (R1..R4 -> 9,8,7,6; C1..C4 -> 5,4,3,2; LCD I2C; chuông -> 10)
//   Module relay: VCC -> 5V, GND -> GND, IN -> 11
//   Nút "mở cửa từ bên trong": chân 12 và GND
//   Mạch tải (phía relay): nguồn khóa (+) -> COM, NO -> khóa điện từ (+), khóa (-) -> nguồn khóa (-)
//
// RELAY = công tắc cơ khí điều khiển bằng nam châm điện:
//   cuộn dây có điện -> hút lá đồng -> chân COM nối sang NO (thường hở) và rời NC (thường đóng). Nghe "tạch".
//   Phía cuộn dây (5V, ~70mA) và phía tiếp điểm (tới 250VAC/10A) CÁCH LY hoàn toàn về điện.
// Module relay có sẵn transistor (Arduino không cấp nổi 70mA), diode chống điện áp ngược, đèn báo.
// Module đen (SONGLE, có chip opto): thêm opto cách ly giữa chân IN và mạch relay; jumper J0/J1 chọn
//   kích bằng mức THẤP hay CAO. Module xanh nhỏ (TONGLING) thường kích mức THẤP.
//
// AN TOÀN: tập với tải điện áp thấp (khóa 12V, đèn LED, quạt 5V). KHÔNG tự đấu điện lưới 220V.

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int CHAN_HANG[4] = {9, 8, 7, 6};
const int CHAN_COT[4] = {5, 4, 3, 2};
const char BAN_PHIM[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};
const int CHUONG = 10;
const int RELAY_PIN = 11;
const int NUT_TRONG = 12;

// Module relay của Wokwi kích mức CAO. Module thật: xem jumper/thử - đèn relay sáng khi IN = LOW thì đặt true.
const bool RELAY_KICH_MUC_THAP = false;
const char* MAT_MA = "1234";
const unsigned long THOI_GIAN_MO = 5000;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Bật/tắt relay theo kiểu kích của module -> phần còn lại của code không cần quan tâm mức cao hay thấp
void datRelay(bool dong) {
  bool mucRa = RELAY_KICH_MUC_THAP ? !dong : dong;
  digitalWrite(RELAY_PIN, mucRa ? HIGH : LOW);
}

// ===== Bàn phím (giống bài 23) =====
char quetPhim() {
  for (int h = 0; h < 4; h++) {
    digitalWrite(CHAN_HANG[h], LOW);
    delayMicroseconds(5);
    for (int c = 0; c < 4; c++) {
      if (digitalRead(CHAN_COT[c]) == LOW) {
        digitalWrite(CHAN_HANG[h], HIGH);
        return BAN_PHIM[h][c];
      }
    }
    digitalWrite(CHAN_HANG[h], HIGH);
  }
  return 0;
}

char phimTruoc = 0, phimOnDinh = 0;
unsigned long thoiDiemDoiPhim = 0;
char docPhimMoi() {
  char phim = quetPhim();
  if (phim != phimTruoc) {
    phimTruoc = phim;
    thoiDiemDoiPhim = millis();
  }
  if (millis() - thoiDiemDoiPhim >= 30 && phim != phimOnDinh) {
    phimOnDinh = phim;
    return phim;
  }
  return 0;
}

int nutTruoc = HIGH;
bool nutTrongVuaNhan() {
  int doc = digitalRead(NUT_TRONG);
  bool vuaNhan = (doc == LOW && nutTruoc == HIGH);
  nutTruoc = doc;
  return vuaNhan;
}

// ===== Máy trạng thái =====
enum TrangThai { KHOA, MO_KHOA, BI_KHOA };
TrangThai trangThai;
unsigned long thoiDiemVao = 0;
char daNhap[9];
int soKyTu = 0, soLanSai = 0;

void hienDong(int dong, const char* chu) {
  lcd.setCursor(0, dong);
  lcd.print(chu);
  for (int i = strlen(chu); i < 16; i++) lcd.print(' ');
}

void vao(TrangThai moi, const char* lyDo) {
  trangThai = moi;
  thoiDiemVao = millis();
  soKyTu = 0;
  datRelay(moi == MO_KHOA);  // relay CHỈ đóng ở trạng thái mở khóa
  Serial.print(">> ");
  Serial.println(lyDo);
  switch (moi) {
    case KHOA:    hienDong(0, "Nhap ma, # = OK"); hienDong(1, ""); break;
    case MO_KHOA: hienDong(0, "DA MO KHOA"); hienDong(1, lyDo); tone(CHUONG, 2500, 300); break;
    case BI_KHOA: hienDong(0, "SAI 3 LAN!"); tone(CHUONG, 300, 800); break;
  }
}

void xuLyPhim(char phim) {
  tone(CHUONG, 2000, 30);
  if (phim == '*') {
    soKyTu = 0;
    hienDong(1, "");
  } else if (phim == '#') {
    daNhap[soKyTu] = '\0';
    if (strcmp(daNhap, MAT_MA) == 0) {
      soLanSai = 0;
      vao(MO_KHOA, "Ma dung");
    } else if (++soLanSai >= 3) {  // ++ đứng trước: tăng rồi mới so sánh
      vao(BI_KHOA, "Sai ma 3 lan - khoa ban phim");
    } else {
      tone(CHUONG, 300, 500);
      hienDong(0, "Sai ma! Nhap lai");
      soKyTu = 0;
      hienDong(1, "");
      Serial.println(">> Sai ma");
    }
  } else if (phim >= '0' && phim <= '9' && soKyTu < 8) {
    daNhap[soKyTu++] = phim;
    char sao[9];
    for (int i = 0; i < soKyTu; i++) sao[i] = '*';
    sao[soKyTu] = '\0';
    hienDong(1, sao);
  }
}

void setup() {
  // Đặt mức "relay nhả" TRƯỚC khi cho chân làm OUTPUT -> relay không đóng nhầm 1 nhịp lúc khởi động
  datRelay(false);
  pinMode(RELAY_PIN, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(CHAN_HANG[i], OUTPUT);
    digitalWrite(CHAN_HANG[i], HIGH);
    pinMode(CHAN_COT[i], INPUT_PULLUP);
  }
  pinMode(NUT_TRONG, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Bai 24: relay - khoa cua dien tu (ma 1234)");
  lcd.init();
  lcd.backlight();
  vao(KHOA, "Khoi dong - cua khoa");
}

void loop() {
  unsigned long daQua = millis() - thoiDiemVao;

  // Nút bên trong: luôn mở được cửa (kể cả khi bàn phím đang bị khóa) - yêu cầu an toàn thoát hiểm
  if (nutTrongVuaNhan()) {
    soLanSai = 0;
    vao(MO_KHOA, "Nut ben trong");
  }

  switch (trangThai) {
    case KHOA: {
      char phim = docPhimMoi();
      if (phim != 0) xuLyPhim(phim);
      break;
    }
    case MO_KHOA:
      if (daQua >= THOI_GIAN_MO) vao(KHOA, "Het 5 giay - khoa lai");
      break;
    case BI_KHOA:
      if (daQua >= 30000) {
        soLanSai = 0;
        vao(KHOA, "Het thoi gian khoa ban phim");
      }
      break;
  }
}
