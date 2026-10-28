// BÀI 11: Cảm biến nhiệt độ - độ ẩm DHT11, dùng THƯ VIỆN
// Mô phỏng: wokwi/wokwi.toml (bấm vào cảm biến khi đang chạy để chỉnh nhiệt độ, độ ẩm)
//
// Nối dây: DHT VCC -> 5V, DATA -> chân 2, GND -> GND.
//   Nếu là module 3 chân (có bảng mạch nhỏ): đã có sẵn điện trở kéo lên, cắm thẳng.
//   Nếu là cảm biến rời 4 chân: thêm điện trở 10kΩ nối giữa DATA và 5V.
//
// THƯ VIỆN = code người khác viết sẵn, mình chỉ việc gọi hàm.
//   DHT gửi dữ liệu theo giao thức riêng với thời gian tính bằng micro giây,
//   tự viết khá mệt -> dùng thư viện "DHT sensor library" của Adafruit.
//   Thư viện được khai báo ở platformio.ini (lib_deps), PlatformIO tự tải về khi build.

#include <Arduino.h>
#include <DHT.h>  // #include: "mượn" code của thư viện

const int DHT_PIN = 2;

// Wokwi chỉ có cảm biến DHT22 (bản xịn hơn của DHT11, cách đọc dữ liệu khác).
// Chạy mô phỏng: DHT22.  Nạp vào board thật với DHT11 của kit: đổi thành DHT11.
const uint8_t LOAI_CAM_BIEN = DHT22;

// Tạo 1 "đối tượng" tên camBien thuộc kiểu DHT. Từ giờ gọi camBien.tenHam() để dùng.
DHT camBien(DHT_PIN, LOAI_CAM_BIEN);

unsigned long lanCuoiDoc = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Bai 11: DHT - nhiet do, do am");
  camBien.begin();
}

void loop() {
  // DHT11 đo chậm: tối đa 1 lần/giây (DHT22: 1 lần/2 giây). Đọc nhanh hơn chỉ nhận lại số cũ.
  if (millis() - lanCuoiDoc < 2000) {
    return;  // return trong loop(): thoát sớm, loop() sẽ được gọi lại ngay
  }
  lanCuoiDoc = millis();

  float doAm = camBien.readHumidity();        // %
  float nhietDo = camBien.readTemperature();  // độ C

  // isnan = "is not a number": đọc lỗi (lỏng dây, sai loại cảm biến) thì thư viện trả về NaN
  if (isnan(doAm) || isnan(nhietDo)) {
    Serial.println("Loi doc cam bien! Kiem tra day va LOAI_CAM_BIEN");
    return;
  }

  // Chỉ số cảm nhận nhiệt: trời nóng + ẩm thì cơ thể thấy nóng hơn nhiệt độ thật
  float camNhan = camBien.computeHeatIndex(nhietDo, doAm, false);  // false = độ C

  Serial.print("Nhiet do: ");
  Serial.print(nhietDo, 1);
  Serial.print(" C\t Do am: ");
  Serial.print(doAm, 1);
  Serial.print(" %\t Cam nhan: ");
  Serial.print(camNhan, 1);
  Serial.println(" C");
}
