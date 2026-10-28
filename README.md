# Học Arduino Uno R3 — từ cơ bản đến nhúng chuyên sâu

Công cụ: VS Code + PlatformIO. Board ở cổng `COM5` (khai báo trong `platformio.ini`; COM4 trên máy này là webcam).

## Cách nạp một bài
- **Cách 1 (bấm nút):** thanh trạng thái dưới cùng VS Code → chọn env (vd `env:lesson01`) → bấm mũi tên **→ (Upload)**. Bấm biểu tượng **ổ cắm (Serial Monitor)** để xem dữ liệu board gửi lên.
- **Cách 2 (lệnh):** bấm nút `>_` (PlatformIO Core CLI) trên thanh trạng thái để mở terminal, rồi gõ `pio run -e lesson01 -t upload` và `pio device monitor`

**Mỗi bài một thư mục** `src/lessonNN_*/`: code `main.cpp`, ảnh sơ đồ nối dây (`so_do*.png` chụp từ Wokwi, hoặc `*.svg`), và nếu có mô phỏng thì `wokwi.toml` + `diagram.json`.
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
9. [ ] LED 7 đoạn
10. [ ] Cảm biến siêu âm HC-SR04 — `pulseIn`
11. [ ] DHT11 — dùng thư viện
12. [ ] LCD 1602 / OLED qua I2C
13. [ ] Servo SG90
14. [ ] Động cơ bước 28BYJ-48 + ULN2003
15. [ ] Động cơ DC + L298N
16. [ ] Máy trạng thái (state machine) — dự án tổng hợp

### Phần 3 — Nhúng chuyên sâu (AVR ATmega328P)
17. [ ] Bên trong `digitalWrite`: thanh ghi `DDRx`, `PORTx`, `PINx`, phép toán bit
18. [ ] Ngắt ngoài (INT0/INT1), `volatile`, ISR
19. [ ] Timer/Counter — tự tạo `millis()`, PWM bằng thanh ghi
20. [ ] ADC bằng thanh ghi
21. [ ] UART bằng thanh ghi (tự viết `Serial`)
22. [ ] I2C / SPI cấp thấp
23. [ ] Tiết kiệm năng lượng, watchdog
24. [ ] Lập trình AVR thuần (không dùng Arduino framework), đọc datasheet
