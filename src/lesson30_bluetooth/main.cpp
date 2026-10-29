// BÀI 30: Bluetooth HC-05 - điều khiển đèn và quạt bằng điện thoại
// Mô phỏng: wokwi/wokwi.toml (Wokwi không có HC-05 -> ô Serial đóng vai điện thoại)
// Mạch thật: xem so_do_hc05.svg
//
// Nối dây (mạch thật):
//   HC-05: VCC -> 5V, GND -> GND, TXD -> chân 10, RXD <- chân 11 qua cầu chia áp 1kΩ/2kΩ
//     (chân RXD của HC-05 chạy 3.3V; Arduino xuất 5V -> chia còn ~3.3V)
//   Đèn phòng khách -> 8, quạt (LED thay, PWM) -> 5, đèn phòng ngủ -> 2   (mỗi LED kèm điện trở 220Ω)
//
// Điện thoại: Android cài app "Serial Bluetooth Terminal", ghép đôi HC-05 (mã PIN 1234 hoặc 0000).
//   iPhone KHÔNG kết nối được HC-05 (Bluetooth cổ điển) -> phải dùng module BLE như HM-10.
//
// Lệnh (gõ rồi gửi, chữ hoa/thường đều được):
//   khach bat | khach tat | ngu bat | ngu tat | quat 0..100 | tat het | ?
// Phím tắt 1 ký tự (cho app dạng nút bấm): K/k = khách bật/tắt, N/n = ngủ bật/tắt, 0..5 = quạt 0..100%

#include <Arduino.h>
#include <SoftwareSerial.h>

const bool MO_PHONG = true;      // mạch thật có HC-05 thì đổi thành false

const int DEN_KHACH = 8, QUAT = 5, DEN_NGU = 2;

// SoftwareSerial: "giả lập" thêm 1 cổng UART bằng phần mềm trên 2 chân bất kỳ.
// Cổng UART phần cứng (chân 0, 1) đã dùng để nạp code + Serial Monitor với máy tính.
SoftwareSerial bt(10, 11);       // RX, TX (nhìn từ phía Arduino)

// Con trỏ tới lớp cha Stream: Serial và bt đều là Stream -> code bên dưới không cần biết đang dùng cái nào
Stream* kenh;

bool khachBat = false, nguBat = false;
int tocDoQuat = 0;               // 0..100 %

char lenh[24];
byte doDai = 0;

void capNhatDauRa() {
  digitalWrite(DEN_KHACH, khachBat);
  digitalWrite(DEN_NGU, nguBat);
  analogWrite(QUAT, map(tocDoQuat, 0, 100, 0, 255));
}

void guiTrangThai() {
  kenh->print(F("Khach: "));  kenh->print(khachBat ? F("BAT") : F("tat"));
  kenh->print(F(" | Ngu: ")); kenh->print(nguBat ? F("BAT") : F("tat"));
  kenh->print(F(" | Quat: ")); kenh->print(tocDoQuat); kenh->println('%');
}

// Trả về true nếu hiểu lệnh
bool xuLyLenh(char* s) {
  for (char* p = s; *p; p++) *p = tolower(*p);   // đổi hết sang chữ thường

  int phanTram;
  if (strcmp(s, "khach bat") == 0)      khachBat = true;
  else if (strcmp(s, "khach tat") == 0) khachBat = false;
  else if (strcmp(s, "ngu bat") == 0)   nguBat = true;
  else if (strcmp(s, "ngu tat") == 0)   nguBat = false;
  else if (strcmp(s, "tat het") == 0)   { khachBat = nguBat = false; tocDoQuat = 0; }
  else if (strcmp(s, "?") == 0)         { /* chỉ gửi trạng thái */ }
  else if (sscanf(s, "quat %d", &phanTram) == 1 && phanTram >= 0 && phanTram <= 100) tocDoQuat = phanTram;
  else return false;
  return true;
}

bool xuLyPhimTat(char c) {
  switch (c) {
    case 'K': khachBat = true;  return true;
    case 'k': khachBat = false; return true;
    case 'N': nguBat = true;    return true;
    case 'n': nguBat = false;   return true;
  }
  if (c >= '0' && c <= '5') { tocDoQuat = (c - '0') * 20; return true; }
  return false;
}

void setup() {
  pinMode(DEN_KHACH, OUTPUT);
  pinMode(DEN_NGU, OUTPUT);
  pinMode(QUAT, OUTPUT);
  Serial.begin(9600);
  if (MO_PHONG) {
    kenh = &Serial;
  } else {
    bt.begin(9600);                // HC-05 mặc định 9600 baud ở chế độ dữ liệu
    kenh = &bt;
    Serial.println(F("Dang cho lenh tu dien thoai qua HC-05..."));
  }
  kenh->println(F("Bai 30: nha thong minh qua Bluetooth. Go ? de xem lenh/trang thai"));
  capNhatDauRa();
}

void loop() {
  while (kenh->available()) {
    char c = kenh->read();
    if (c == '\n' || c == '\r') {
      if (doDai == 0) continue;
      lenh[doDai] = '\0';
      // 1 ký tự: thử phím tắt TRƯỚC (xuLyLenh đổi sang chữ thường sẽ làm mất phân biệt K/k)
      bool hieu = (doDai == 1) ? xuLyPhimTat(lenh[0]) || xuLyLenh(lenh) : xuLyLenh(lenh);
      doDai = 0;
      if (hieu) {
        capNhatDauRa();
        guiTrangThai();              // phản hồi lại điện thoại: lệnh đã được thực hiện
      } else {
        kenh->println(F("Lenh la. Dung: khach bat|tat, ngu bat|tat, quat 0..100, tat het, ?"));
      }
      if (!MO_PHONG) {               // mạch thật: ghi nhật ký ra máy tính để theo dõi
        Serial.print(F("BT> "));
        Serial.println(hieu ? F("ok") : F("lenh la"));
      }
    } else if (doDai < sizeof(lenh) - 1) {
      lenh[doDai++] = c;
    }
  }
}
