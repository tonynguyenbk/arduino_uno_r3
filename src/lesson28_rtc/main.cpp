// BÀI 28: Module thời gian thực RTC DS1307 - đồng hồ báo thức trên LCD
// Mô phỏng: wokwi/wokwi.toml
//
// Nối dây:
//   RTC DS1307 và LCD I2C dùng CHUNG 2 dây I2C: SDA -> A4, SCL -> A5 (mỗi thiết bị một địa chỉ riêng)
//     RTC: 0x68, LCD: 0x27. VCC -> 5V, GND -> GND.
//   Đèn báo thức -> A0 (kèm điện trở 220Ω), chuông -> A1, nút -> A2 (và GND)
//
// RTC = Real Time Clock: chip đếm giờ riêng, chạy bằng thạch anh 32.768 kHz, có PIN CMOS (CR2032)
//   nên rút điện Arduino vẫn nhớ giờ. millis() thì về 0 mỗi lần reset và lệch vài giây mỗi giờ.
//   32768 = 2^15 -> chia đôi 15 lần là được đúng 1 xung mỗi giây.
//
// Lệnh qua Serial Monitor (gõ rồi Enter):
//   T 2026-10-30 07:29:50   -> chỉnh ngày giờ
//   A 07:30                 -> đặt giờ báo thức
// Nút: đang reo -> tắt chuông; không reo -> bật/tắt chế độ báo thức.

#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

const int DEN_PIN = A0, CHUONG_PIN = A1, NUT_PIN = A2;
const unsigned long THOI_GIAN_REO_TOI_DA = 60000;   // reo tối đa 1 phút rồi tự tắt

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* const THU[7] = {"CN", "T2", "T3", "T4", "T5", "T6", "T7"};  // dayOfTheWeek(): 0 = Chủ nhật

int gioBaoThuc = 7, phutBaoThuc = 30;
bool baoThucBat = true, dangReo = false;
unsigned long lucBatDauReo = 0, lanCuoiNhay = 0;
bool nhay = false;
int giayTruoc = -1;
int nutTruoc = HIGH;
unsigned long lanDoiNut = 0;

char lenh[32];
byte doDaiLenh = 0;

// In số 2 chữ số: 7 -> "07"
void in2So(Print& p, int so) {
  if (so < 10) p.print('0');
  p.print(so);
}

void veManHinh(const DateTime& t) {
  // Dòng 1: "T5 30/10/2026"
  lcd.setCursor(0, 0);
  lcd.print(THU[t.dayOfTheWeek()]);
  lcd.print(' ');
  in2So(lcd, t.day());   lcd.print('/');
  in2So(lcd, t.month()); lcd.print('/');
  lcd.print(t.year());
  lcd.print("   ");
  // Dòng 2: "07:29:55 BT07:30" hoặc "07:29:55 BT tat "
  lcd.setCursor(0, 1);
  in2So(lcd, t.hour());   lcd.print(':');
  in2So(lcd, t.minute()); lcd.print(':');
  in2So(lcd, t.second());
  if (dangReo) {
    lcd.print(nhay ? " DAY DI!" : "        ");
  } else if (baoThucBat) {
    lcd.print(" BT");
    in2So(lcd, gioBaoThuc); lcd.print(':');
    in2So(lcd, phutBaoThuc);
  } else {
    lcd.print(" BT tat ");
  }
}

void tatChuong() {
  dangReo = false;
  noTone(CHUONG_PIN);
  digitalWrite(DEN_PIN, LOW);
}

void xuLyLenh() {
  int nam, thang, ngay, gio, phut, giay;
  if (sscanf(lenh, "T %d-%d-%d %d:%d:%d", &nam, &thang, &ngay, &gio, &phut, &giay) == 6) {
    rtc.adjust(DateTime(nam, thang, ngay, gio, phut, giay));
    Serial.println("Da chinh gio.");
  } else if (sscanf(lenh, "A %d:%d", &gio, &phut) == 2 && gio < 24 && phut < 60) {
    gioBaoThuc = gio;
    phutBaoThuc = phut;
    baoThucBat = true;
    // Phép trừ thời gian: TimeSpan = DateTime - DateTime
    DateTime bayGio = rtc.now();
    DateTime lanToi(bayGio.year(), bayGio.month(), bayGio.day(), gio, phut, 0);
    if (lanToi <= bayGio) lanToi = lanToi + TimeSpan(1, 0, 0, 0);   // đã qua -> ngày mai
    TimeSpan conLai = lanToi - bayGio;
    Serial.print("Bao thuc luc ");
    in2So(Serial, gio); Serial.print(':'); in2So(Serial, phut);
    Serial.print(" - con ");
    Serial.print(conLai.hours());
    Serial.print(" gio ");
    Serial.print(conLai.minutes());
    Serial.println(" phut");
  } else {
    Serial.println("Lenh khong hop le. Vi du: T 2026-10-30 07:29:50  hoac  A 07:30");
  }
}

void setup() {
  pinMode(DEN_PIN, OUTPUT);
  pinMode(CHUONG_PIN, OUTPUT);
  pinMode(NUT_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {                 // không trả lời ở địa chỉ 0x68
    Serial.println("Khong tim thay RTC - kiem tra day SDA/SCL!");
    lcd.print("Loi: khong co RTC");
    while (true) {}
  }
  if (!rtc.isrunning()) {             // chip mới / hết pin: đồng hồ đang dừng
    // __DATE__, __TIME__ = lúc BIÊN DỊCH chương trình -> lấy làm giờ ban đầu
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("RTC chua chay - dat theo gio bien dich.");
  }
  Serial.println("Bai 28: dong ho bao thuc. Lenh: T yyyy-mm-dd hh:mm:ss | A hh:mm");
}

void loop() {
  unsigned long ms = millis();

  // ===== Nhận lệnh Serial từng ký tự, gặp Enter thì xử lý =====
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (doDaiLenh > 0) {
        lenh[doDaiLenh] = '\0';
        xuLyLenh();
        doDaiLenh = 0;
      }
    } else if (doDaiLenh < sizeof(lenh) - 1) {
      lenh[doDaiLenh++] = c;
    }
  }

  // ===== Nút =====
  int doc = digitalRead(NUT_PIN);
  if (doc != nutTruoc && ms - lanDoiNut > 50) {
    lanDoiNut = ms;
    nutTruoc = doc;
    if (doc == LOW) {
      if (dangReo) {
        tatChuong();
        Serial.println("Da tat chuong.");
      } else {
        baoThucBat = !baoThucBat;
        Serial.println(baoThucBat ? "Bat bao thuc" : "Tat bao thuc");
      }
      giayTruoc = -1;                 // vẽ lại màn hình ngay
    }
  }

  // ===== Đọc RTC: chỉ vẽ lại khi sang giây mới =====
  DateTime t = rtc.now();
  if (t.second() != giayTruoc) {
    giayTruoc = t.second();
    if (baoThucBat && !dangReo && t.hour() == gioBaoThuc && t.minute() == phutBaoThuc && t.second() == 0) {
      dangReo = true;
      lucBatDauReo = ms;
      Serial.println("!!! BAO THUC !!!");
    }
    veManHinh(t);
  }

  // ===== Đang reo: đèn nháy + chuông kêu ngắt quãng =====
  if (dangReo) {
    if (ms - lanCuoiNhay >= 250) {
      lanCuoiNhay = ms;
      nhay = !nhay;
      digitalWrite(DEN_PIN, nhay);
      if (nhay) tone(CHUONG_PIN, 2000); else noTone(CHUONG_PIN);
    }
    if (ms - lucBatDauReo >= THOI_GIAN_REO_TOI_DA) {
      tatChuong();
      Serial.println("Reo 1 phut - tu tat.");
    }
  }
}
