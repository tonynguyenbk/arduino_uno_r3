// BÀI 20: Remote hồng ngoại - điều khiển đèn LED bằng remote
// Mô phỏng: wokwi/wokwi.toml (bấm chuột vào các nút của remote trong Wokwi)
//
// Nối dây:
//   Mắt thu hồng ngoại (module 3 chân, mắt đen VS1838B): S/OUT -> chân 2, "+" -> 5V, "-" -> GND
//   LED + điện trở 220Ω: chân 9 (PWM) -> GND
//
// Remote gửi ánh sáng hồng ngoại (mắt người không thấy) nhấp nháy ở 38kHz, mã hóa theo giao thức NEC:
//   [xung mở đầu 9ms + 4.5ms] [8 bit địa chỉ] [8 bit địa chỉ đảo] [8 bit lệnh] [8 bit lệnh đảo]
//   Bit 0/1 phân biệt bằng độ dài khoảng nghỉ (0.56ms = 0, 1.69ms = 1).
//   Giữ nút: remote gửi "mã lặp lại" ngắn mỗi 110ms thay vì gửi lại cả mã.
// Mắt thu đã lọc sóng 38kHz, đưa ra chân S các xung dữ liệu sạch (lúc có tín hiệu thì S = LOW).
// Thư viện IRremote đo độ dài các xung (dùng Timer2 -> tone() và PWM chân 3, 11 không dùng được).
//
// Phím: 1..9 = độ sáng 10%..90%,  0 = tắt,  + / - = sáng hơn / tối hơn (giữ nút để tăng liên tục),
//       nút nguồn (Wokwi) / CH (remote kit) = bật/tắt.

#include <Arduino.h>
#include <IRremote.hpp>

const int IR_PIN = 2;
const int LED_PIN = 9;

// Remote của Wokwi và remote đi kèm kit (loại "Car MP3" 21 phím) có MÃ NÚT KHÁC NHAU.
// Chạy mô phỏng: true.   Nạp vào board thật với remote của kit: false.
const bool REMOTE_WOKWI = true;

enum Lenh { BAT_TAT, SANG_HON, TOI_HON, DAT_MUC };
struct Phim {       // struct: gom nhiều giá trị thành 1 kiểu dữ liệu mới
  uint8_t ma;       // mã lệnh (command) remote gửi
  Lenh lenh;
  uint8_t muc;      // dùng cho DAT_MUC: 0..9
  const char* ten;
};

const Phim PHIM_WOKWI[] = {
  {0xA2, BAT_TAT, 0, "POWER"}, {0x02, SANG_HON, 0, "+"}, {0x98, TOI_HON, 0, "-"},
  {0x68, DAT_MUC, 0, "0"}, {0x30, DAT_MUC, 1, "1"}, {0x18, DAT_MUC, 2, "2"}, {0x7A, DAT_MUC, 3, "3"},
  {0x10, DAT_MUC, 4, "4"}, {0x38, DAT_MUC, 5, "5"}, {0x5A, DAT_MUC, 6, "6"}, {0x42, DAT_MUC, 7, "7"},
  {0x4A, DAT_MUC, 8, "8"}, {0x52, DAT_MUC, 9, "9"},
};
const Phim PHIM_KIT[] = {
  {0x46, BAT_TAT, 0, "CH"}, {0x15, SANG_HON, 0, "VOL+"}, {0x07, TOI_HON, 0, "VOL-"},
  {0x16, DAT_MUC, 0, "0"}, {0x0C, DAT_MUC, 1, "1"}, {0x18, DAT_MUC, 2, "2"}, {0x5E, DAT_MUC, 3, "3"},
  {0x08, DAT_MUC, 4, "4"}, {0x1C, DAT_MUC, 5, "5"}, {0x5A, DAT_MUC, 6, "6"}, {0x42, DAT_MUC, 7, "7"},
  {0x52, DAT_MUC, 8, "8"}, {0x4A, DAT_MUC, 9, "9"},
};

int doSang = 0;       // 0..255
int doSangTruocKhiTat = 128;

// Tìm mã trong bảng. Trả về con trỏ tới phần tử tìm thấy, hoặc nullptr nếu không có.
const Phim* timPhim(uint8_t ma) {
  const Phim* bang = REMOTE_WOKWI ? PHIM_WOKWI : PHIM_KIT;
  int soPhim = REMOTE_WOKWI ? sizeof(PHIM_WOKWI) / sizeof(Phim) : sizeof(PHIM_KIT) / sizeof(Phim);
  for (int i = 0; i < soPhim; i++) {
    if (bang[i].ma == ma) return &bang[i];
  }
  return nullptr;
}

void xuLy(const Phim* phim, bool laLapLai) {
  // Giữ nút: chỉ cho phép lặp với + / -, các nút khác bỏ qua mã lặp
  if (laLapLai && phim->lenh != SANG_HON && phim->lenh != TOI_HON) return;

  switch (phim->lenh) {
    case BAT_TAT:
      if (doSang > 0) {
        doSangTruocKhiTat = doSang;
        doSang = 0;
      } else {
        doSang = doSangTruocKhiTat;
      }
      break;
    case SANG_HON: doSang = min(doSang + 15, 255); break;
    case TOI_HON:  doSang = max(doSang - 15, 0); break;
    case DAT_MUC:  doSang = phim->muc * 255 / 10; break;  // 1 -> 10%, 9 -> 90%
  }
  analogWrite(LED_PIN, doSang);
  Serial.print("  -> Phim ");
  Serial.print(phim->ten);
  Serial.print(laLapLai ? " (giu)" : "");
  Serial.print(", do sang = ");
  Serial.print(doSang * 100 / 255);
  Serial.println("%");
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Bai 20: remote hong ngoai");
  Serial.println(REMOTE_WOKWI ? "Bang ma: remote Wokwi" : "Bang ma: remote cua kit");
  // ENABLE_LED_FEEDBACK: đèn L trên board nháy mỗi khi nhận được tín hiệu -> dễ kiểm tra mắt thu
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (!IrReceiver.decode()) return;  // chưa nhận được gì

  IRData& duLieu = IrReceiver.decodedIRData;  // & = tham chiếu: dùng thẳng dữ liệu, không chép
  bool laLapLai = duLieu.flags & IRDATA_FLAGS_IS_REPEAT;

  if (duLieu.protocol == UNKNOWN) {
    Serial.println("Nhan duoc tin hieu la (nhieu hoac remote khong ho tro)");
  } else {
    Serial.print("Giao thuc: ");
    Serial.print(getProtocolString(duLieu.protocol));
    Serial.print("  Dia chi: 0x");
    Serial.print(duLieu.address, HEX);
    Serial.print("  Lenh: 0x");
    Serial.print(duLieu.command, HEX);
    Serial.println(laLapLai ? "  (lap lai)" : "");

    const Phim* phim = timPhim(duLieu.command);
    if (phim != nullptr) {
      xuLy(phim, laLapLai);
    } else if (!laLapLai) {
      Serial.println("  -> Nut chua co trong bang ma: them vao PHIM_WOKWI / PHIM_KIT neu muon dung");
    }
  }
  IrReceiver.resume();  // sẵn sàng nhận mã tiếp theo
}
