// BÀI 25: IC 74HC595 - thanh ghi dịch: điều khiển 8 LED chỉ bằng 3 chân Arduino
// Mô phỏng: wokwi/wokwi.toml
//
// Nối dây 74HC595 (chip 16 chân, vết khuyết hình bán nguyệt ở đầu chip = phía chân 1 và 16):
//   chân 14 DS   (dữ liệu)              -> Arduino 10
//   chân 11 SHCP (nhịp dịch)            -> Arduino 13
//   chân 12 STCP (chốt / latch)         -> Arduino 12
//   chân 13 OE   (cho phép xuất, LOW = bật)  -> Arduino 11 (PWM -> chỉnh độ sáng cả 8 LED)
//   chân 10 MR   (xóa, LOW = xóa hết)   -> Arduino A4 (dùng làm chân digital)
//   chân 16 VCC -> 5V,  chân 8 GND -> GND
//   Q0..Q7 (chân 15, 1, 2, 3, 4, 5, 6, 7) -> 8 LED, mỗi LED 1 điện trở 220Ω xuống GND
//   Cách nối tối giản (không chỉnh sáng, không xóa): OE -> GND, MR -> 5V.
//
// Bên trong 74HC595 có 2 "ngăn" 8 bit:
//   1. THANH GHI DỊCH: mỗi xung nhịp SHCP (LOW -> HIGH), chip đọc 1 bit ở chân DS và đẩy cả hàng
//      sang 1 ô (bit cũ ở ô 7 rơi ra chân Q7S để nối tiếp sang chip thứ 2).
//   2. THANH GHI CHỐT: khi STCP lên HIGH, chép cả 8 bit sang chân Q0..Q7 CÙNG LÚC.
//   -> Trong lúc đang dịch 8 bit, đèn KHÔNG nhấp nháy theo; xong mới "chốt" 1 lần.
// Gửi 1 byte = 8 lần (đặt DS + 1 xung SHCP) + 1 xung STCP. Nối tiếp nhiều chip: thêm 8 LED chỉ cần 0 chân!

#include <Arduino.h>

const int DS_PIN = 10;    // dữ liệu
const int SHCP_PIN = 13;  // nhịp dịch
const int STCP_PIN = 12;  // chốt
const int OE_PIN = 11;    // cho phép xuất (mức THẤP mới bật) - PWM để chỉnh độ sáng
const int MR_PIN = A4;    // xóa thanh ghi (mức THẤP = xóa)
const int DO_SANG = 120;  // 0..255

// Tự viết hàm gửi 1 byte (bit cao nhất gửi trước) - để thấy rõ từng bước
void guiByte(byte giaTri) {
  digitalWrite(STCP_PIN, LOW);                 // chuẩn bị: chưa chốt
  for (int bit = 7; bit >= 0; bit--) {
    digitalWrite(DS_PIN, (giaTri >> bit) & 1); // đặt bit lên chân dữ liệu
    digitalWrite(SHCP_PIN, HIGH);              // cạnh lên: chip đọc bit và dịch
    digitalWrite(SHCP_PIN, LOW);
  }
  digitalWrite(STCP_PIN, HIGH);                // cạnh lên: 8 bit ra chân Q0..Q7 cùng lúc
}
// Arduino có sẵn hàm làm đúng việc trên: shiftOut(DS_PIN, SHCP_PIN, MSBFIRST, giaTri);

void inNhiPhan(byte giaTri) {
  for (int bit = 7; bit >= 0; bit--) Serial.print((giaTri >> bit) & 1);
}

// ===== 3 hiệu ứng, mỗi hiệu ứng chạy 6 giây =====
enum HieuUng { DEM_NHI_PHAN, DEN_CHAY, THANH_DAY };
const char* TEN[] = {"Dem nhi phan 0..255", "Den chay qua lai", "Thanh day dan"};
HieuUng hieuUng = DEM_NHI_PHAN;
unsigned long batDau = 0, lanCuoi = 0;
int buoc = 0, chieu = 1;

byte tinhKhung() {
  switch (hieuUng) {
    case DEM_NHI_PHAN:
      return buoc & 0xFF;                       // 0, 1, 2, ... 255: thấy rõ cách đếm nhị phân
    case DEN_CHAY: {
      byte khung = 1 << buoc;                   // chỉ 1 bit sáng
      buoc += chieu;
      if (buoc == 7 || buoc == 0) chieu = -chieu;  // chạm đầu thì quay lại
      return khung;
    }
    case THANH_DAY:
      return (buoc % 9 == 0) ? 0 : (0xFF >> (8 - buoc % 9));  // 0b1, 0b11, 0b111 ... 0b11111111
  }
  return 0;
}

void setup() {
  pinMode(DS_PIN, OUTPUT);
  pinMode(SHCP_PIN, OUTPUT);
  pinMode(STCP_PIN, OUTPUT);
  pinMode(MR_PIN, OUTPUT);
  digitalWrite(MR_PIN, HIGH);  // không xóa
  // OE hoạt động ở mức THẤP: thời gian LOW càng dài thì LED càng sáng -> đảo giá trị PWM
  analogWrite(OE_PIN, 255 - DO_SANG);
  Serial.begin(9600);
  Serial.println("Bai 25: 74HC595 - 8 LED bang 3 chan");
  Serial.println(TEN[hieuUng]);
}

void loop() {
  unsigned long bayGio = millis();

  if (bayGio - batDau >= 6000) {
    batDau = bayGio;
    hieuUng = (HieuUng)((hieuUng + 1) % 3);     // ép kiểu số -> enum
    buoc = 0;
    chieu = 1;
    digitalWrite(MR_PIN, LOW);   // xung xóa: thanh ghi dịch về 0 ngay lập tức
    digitalWrite(MR_PIN, HIGH);
    Serial.println(TEN[hieuUng]);
  }

  unsigned long nhip = (hieuUng == DEM_NHI_PHAN) ? 150 : 100;
  if (bayGio - lanCuoi >= nhip) {
    lanCuoi = bayGio;
    byte khung = tinhKhung();
    if (hieuUng != DEN_CHAY) buoc++;
    guiByte(khung);
    Serial.print("  ");
    inNhiPhan(khung);
    Serial.print("  = ");
    Serial.println(khung);
  }
}
