# Học Arduino Uno R3 — từ cơ bản đến nhúng chuyên sâu

Công cụ: VS Code + PlatformIO. Board ở cổng `COM5` (khai báo trong `platformio.ini`; COM4 trên máy này là webcam).

## Cách nạp một bài
- **Cách 1 (bấm nút):** thanh trạng thái dưới cùng VS Code → chọn env (vd `env:lesson01`) → bấm mũi tên **→ (Upload)**. Bấm biểu tượng **ổ cắm (Serial Monitor)** để xem dữ liệu board gửi lên.
- **Cách 2 (lệnh):** bấm nút `>_` (PlatformIO Core CLI) trên thanh trạng thái để mở terminal, rồi gõ `pio run -e lesson01 -t upload` và `pio device monitor`

**Mỗi bài một thư mục** `src/lessonNN_*/`: code `main.cpp`, ảnh sơ đồ nối dây (`so_do*.png` chụp từ Wokwi, hoặc `*.svg`), và nếu có mô phỏng thì `wokwi.toml` + `diagram.json`.
File `README.md` của mỗi bài: ứng dụng thực tế (phần cứng dùng ở đâu, kiến thức trong hệ thống nhúng, dự án công nghiệp/thương mại), lỗi thường gặp, bài tập mở rộng, từ khóa + mục datasheet, câu hỏi phỏng vấn.
Chạy mô phỏng (cần extension Wokwi Simulator): build env của bài → `Ctrl+Shift+P` → **Wokwi: Select Config File** → chọn `wokwi.toml` của bài → **Wokwi: Start Simulator**.

**Lưu ý**
- Mỗi lần mở Serial Monitor, nội dung được lưu vào file `logs/device-monitor-*.log`.
- Nút Upload của PlatformIO tự đóng Serial Monitor rồi mở lại sau khi nạp. Khi nạp bằng lệnh `pio run -t upload` thì phải tự đóng Serial Monitor trước (bấm vào terminal → `Ctrl+C`, hoặc 🗑 Kill Terminal), nếu không sẽ báo lỗi `Access is denied`.
- Khi có bài mới được thêm vào `platformio.ini`, chạy `Ctrl+Shift+P` → **Developer: Reload Window**, chờ PlatformIO tải xong rồi mới Upload. Nếu không, nút Upload có thể báo lỗi *"Cannot read properties of undefined (reading 'id')"*.

## Lộ trình

### Phần 1 — Làm quen (C++ cơ bản + chân I/O)
1. [x] Nháy LED có sẵn — `setup()`, `loop()`, `delay()`
2. [x] Serial Monitor — in chữ, biến, kiểu dữ liệu, phép toán
3. [x] LED ngoài + breadboard + điện trở — định luật Ohm, `if/else`, `for`
4. [x] Nút nhấn — `digitalRead`, `INPUT_PULLUP`, chống dội phím
5. [x] `millis()` thay `delay()` — làm nhiều việc cùng lúc
6. [x] PWM — làm mờ LED với `analogWrite`
7. [x] Đọc analog — biến trở / LDR, `analogRead`, `map()`, `constrain()` *(7B quang trở: mới mô phỏng, chưa cắm thật)*
8. [x] Hàm, mảng — tổ chức code gọn gàng, bảng tra gamma (nhanh hơn `pow()` ~460 lần)

### Phần 2 — Linh kiện ngoại vi
9. [x] LED 7 đoạn 5611AH — mảng chữ số, `bitRead` *(mô phỏng ✓, chưa cắm thật)*
10. [x] Cảm biến siêu âm HC-SR04 — `pulseIn`, cảm biến lùi xe *(mô phỏng ✓, chưa cắm thật)*
11. [x] DHT11 — dùng thư viện (`lib_deps`), đối tượng, `isnan` *(mô phỏng ✓ với DHT22, chưa cắm thật — đổi `LOAI_CAM_BIEN` thành DHT11)*
12. [x] LCD 1602 / OLED qua I2C — `Wire`, dò địa chỉ I2C, số hex, con trỏ *(mô phỏng ✓, chưa cắm thật)*
13. [x] Servo SG90 — thư viện Servo, độ dài xung, Timer1 *(mô phỏng ✓, chưa cắm thật)*
14. [x] Động cơ bước 28BYJ-48 + ULN2003 — thư viện Stepper, vùng chết, `long` *(mô phỏng ✓ với động cơ 200 bước, chưa cắm thật — đổi `BUOC_MOI_VONG` thành 2048)*
15. [x] Động cơ DC + L298N — cầu H, PWM tốc độ, tăng/giảm tốc từ từ *(mô phỏng bằng LED, chưa cắm thật)*
16. [x] Máy trạng thái (state machine) — đèn giao thông ngã tư, `enum`, `switch`, mảng 2 chiều, giám sát an toàn *(mô phỏng ✓, chưa cắm thật)*
17. [x] Chuông (buzzer) — `tone()`, chuông chủ động / thụ động, giai điệu bằng mảng *(mô phỏng ✓, chưa cắm thật)*
18. [x] Joystick — 2 trục analog + nút, vùng chết, bảng vẽ trên OLED *(mô phỏng ✓, chưa cắm thật)*
19. [x] Cảm biến nước — ngưỡng có trễ, chống ăn mòn bằng cấp nguồn lúc đo *(mô phỏng bằng biến trở trượt, chưa cắm thật)*
20. [x] Remote hồng ngoại — giao thức NEC, thư viện IRremote, `struct`, bảng tra mã nút *(mô phỏng ✓, chưa cắm thật — đổi `REMOTE_WOKWI = false`)*
21. [x] LED ma trận 8×8 MAX7219 — bộ đệm khung hình, phông chữ `PROGMEM`, chữ chạy *(mô phỏng ✓, chưa cắm thật)*
22. [x] RFID RC522 + thẻ từ — giao tiếp SPI, UID, EEPROM, khóa cửa thẻ từ *(mô phỏng bằng nút giả lập thẻ, chưa cắm thật — đổi `MO_PHONG = false`)*
23. [x] Bàn phím ma trận 4×4 — tự viết hàm quét hàng/cột, khóa mật mã, chuỗi C *(mô phỏng ✓, chưa cắm thật)*
24. [x] Relay — đóng cắt thiết bị điện, opto cách ly, kích mức cao/thấp, khóa cửa điện từ *(mô phỏng ✓, chưa cắm thật)*

### Phần 3 — Nhúng chuyên sâu (AVR ATmega328P)
25. [ ] Bên trong `digitalWrite`: thanh ghi `DDRx`, `PORTx`, `PINx`, phép toán bit *(đã soạn, tạm hoãn)*
26. [ ] Ngắt ngoài (INT0/INT1), `volatile`, ISR
27. [ ] Timer/Counter — tự tạo `millis()`, PWM bằng thanh ghi
28. [ ] ADC bằng thanh ghi
29. [ ] UART bằng thanh ghi (tự viết `Serial`)
30. [ ] I2C / SPI cấp thấp
31. [ ] Tiết kiệm năng lượng, watchdog
32. [ ] Lập trình AVR thuần (không dùng Arduino framework), đọc datasheet
