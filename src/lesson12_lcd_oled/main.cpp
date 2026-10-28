// BÀI 12: Màn hình LCD 1602 (có mạch I2C phía sau) và OLED 0.96" - giao tiếp I2C
// Mô phỏng: wokwi_12a/ (LCD), wokwi_12b/ (OLED). Cùng 1 code.
//
// Nối dây (cả LCD và OLED đều 4 chân, nối giống nhau - có thể nối CẢ HAI cùng lúc):
//   GND -> GND,  VCC -> 5V,  SDA -> A4,  SCL -> A5
//
// I2C: nhiều thiết bị dùng chung 2 dây SDA (dữ liệu) + SCL (nhịp đồng hồ).
// Mỗi thiết bị có 1 ĐỊA CHỈ 7 bit. Arduino gọi địa chỉ nào thì thiết bị đó trả lời.
//   LCD 1602 I2C: thường 0x27 (hoặc 0x3F)     OLED SSD1306: thường 0x3C
//   0x.. là cách viết số hệ 16 (hex): 0x27 = 2*16 + 7 = 39
//
// Chương trình: (1) dò xem có thiết bị nào trên dây I2C, (2) hiện chữ lên màn hình tìm thấy.
// LCD: chỉnh độ tương phản bằng biến trở xanh (trimpot!) sau lưng nếu không thấy chữ.

#include <Arduino.h>
#include <Wire.h>               // thư viện I2C có sẵn của Arduino
#include <LiquidCrystal_I2C.h>  // LCD 1602 qua I2C
#include <Adafruit_SSD1306.h>   // OLED

const uint8_t DIA_CHI_OLED = 0x3C;

uint8_t diaChiLcd = 0;   // 0 = không có LCD
bool coOled = false;

// Con trỏ (*): chỉ tạo màn hình khi thật sự tìm thấy -> không tốn RAM vô ích.
// OLED cần 1024 byte RAM để vẽ (128 x 64 điểm / 8 điểm mỗi byte) = 1/2 RAM của Uno!
LiquidCrystal_I2C* lcd = nullptr;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);

unsigned long lanCuoiVe = 0;

// Hỏi thử 1 địa chỉ: thiết bị có trả lời (ACK) không?
bool coThietBi(uint8_t diaChi) {
  Wire.beginTransmission(diaChi);
  return Wire.endTransmission() == 0;  // 0 = có trả lời
}

void doTimI2C() {
  Serial.println("Do tim thiet bi I2C...");
  for (uint8_t diaChi = 1; diaChi < 127; diaChi++) {
    if (coThietBi(diaChi)) {
      Serial.print("  Tim thay o dia chi 0x");
      Serial.println(diaChi, HEX);  // HEX: in ra dạng hệ 16
      if (diaChi == 0x27 || diaChi == 0x3F) diaChiLcd = diaChi;
      if (diaChi == DIA_CHI_OLED) coOled = true;
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Bai 12: LCD / OLED qua I2C");
  Wire.begin();  // Arduino làm "chủ" (master) của dây I2C
  doTimI2C();

  if (diaChiLcd != 0) {
    lcd = new LiquidCrystal_I2C(diaChiLcd, 16, 2);  // 16 cột, 2 dòng
    lcd->init();
    lcd->backlight();  // bật đèn nền
    lcd->setCursor(0, 0);  // (cột, dòng) - đếm từ 0
    lcd->print("Xin chao!");
    Serial.println("-> Dung LCD");
  }
  if (coOled) {
    if (oled.begin(SSD1306_SWITCHCAPVCC, DIA_CHI_OLED)) {
      Serial.println("-> Dung OLED");
    } else {
      coOled = false;
      Serial.println("OLED: khong du RAM!");
    }
  }
  if (diaChiLcd == 0 && !coOled) {
    Serial.println("Khong tim thay man hinh nao! Kiem tra 4 day.");
  }
}

void loop() {
  if (millis() - lanCuoiVe < 500) return;
  lanCuoiVe = millis();
  unsigned long giay = millis() / 1000;

  if (lcd != nullptr) {
    lcd->setCursor(0, 1);
    lcd->print("Thoi gian: ");
    lcd->print(giay);
    lcd->print("s   ");  // vài dấu cách xóa ký tự thừa của số cũ dài hơn
  }

  if (coOled) {
    // OLED vẽ vào bộ nhớ đệm trước, rồi display() mới đẩy cả màn hình ra một lần
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(2);       // chữ to gấp đôi
    oled.setCursor(0, 0);      // (x, y) tính bằng điểm ảnh
    oled.println("Xin chao!");
    oled.setTextSize(1);
    oled.setCursor(0, 24);
    oled.print("Thoi gian: ");
    oled.print(giay);
    oled.println(" s");
    // Thanh chạy: dài thêm mỗi giây, đủ 10 giây thì lại từ đầu
    int dai = (giay % 10 + 1) * 12;
    oled.drawRect(0, 44, 122, 12, SSD1306_WHITE);   // khung
    oled.fillRect(1, 45, dai, 10, SSD1306_WHITE);   // phần đặc bên trong
    oled.display();
  }
}
