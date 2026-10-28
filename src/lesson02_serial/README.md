# Bài 2: Serial Monitor, biến, kiểu dữ liệu — ứng dụng thực tế

## Phần cứng này dùng ở đâu
Cổng Serial (UART: truyền nối tiếp không đồng bộ) là "cổng nói chuyện" phổ biến nhất của vi điều khiển:
- **Module giao tiếp bằng lệnh AT:** module SIM gửi SMS/gọi điện (SIM800), module wifi ESP8266, module Bluetooth HC-05.
- **GPS:** module GPS gửi vị trí liên tục qua UART theo chuẩn văn bản NMEA.
- **Nạp firmware:** chính Arduino được nạp code qua UART (bootloader nhận dữ liệu từ máy tính).

## Kiến thức của bài trong hệ thống nhúng
- **Debug bằng log (printf debugging):** phần lớn thiết bị nhúng không có màn hình, kỹ sư cắm dây vào cổng UART "ẩn" trên bo mạch để đọc log. Router, camera, TV đều có cổng debug UART bên trong.
- **Kiểu dữ liệu và tràn số** là nguồn gốc của những lỗi rất đắt giá:
  - **Ariane 5 (1996):** tên lửa nổ 37 giây sau khi phóng vì đổi 1 số thực 64 bit sang số nguyên 16 bit bị tràn. Thiệt hại khoảng 370 triệu USD.
  - **Tên lửa Patriot (1991):** sai số do lưu 0.1 giây bằng số nhị phân 24 bit, cộng dồn 100 giờ thành lệch 0.34 giây, hệ thống bắn hụt.
- **Phép chia số nguyên, `%`:** dùng liên tục trong nhúng để tách chữ số hiển thị, chia vòng tròn bộ đệm (ring buffer), tính chu kỳ.

## Dự án thực tế
- **Công nghiệp:** chuẩn **Modbus RTU** chạy trên UART + đường truyền RS-485, nối hàng chục cảm biến, biến tần, đồng hồ điện trong nhà máy với nhau bằng 1 cặp dây xa tới 1200m.
- **Ô tô:** máy chẩn đoán đọc lỗi xe qua giao tiếp nối tiếp (K-Line, sau này là CAN).
- **Thương mại:** máy POS, máy in hóa đơn, đầu đọc mã vạch ở siêu thị phần lớn giao tiếp kiểu nối tiếp.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Serial Monitor hiện ký tự rác `⸮⸮⸮` | Tốc độ baud hai bên không khớp | `Serial.begin(9600)` phải bằng `monitor_speed = 9600` |
| Upload báo `Access is denied` | Serial Monitor đang giữ cổng COM | Đóng Monitor, hoặc dùng nút Upload (tự đóng) |
| `5 / 2` ra `2` | Chia 2 số nguyên thì bỏ phần lẻ | Viết `5 / 2.0` hoặc ép kiểu `(float)5 / 2` |
| `int` đếm tới 32767 rồi thành -32768 | `int` trên AVR chỉ 16 bit, bị tràn | Dùng `long` hoặc `unsigned long` |

## Bài tập mở rộng
1. **Dễ:** in bảng cửu chương 2–9 cho thẳng cột (dùng `\t`).
2. **Vừa:** nhận lệnh từ bàn phím: gõ `1` bật LED, `0` tắt LED (`Serial.available()`, `Serial.read()`).
3. **Khó:** in thời gian đã chạy dạng `giờ:phút:giây` (dùng `/` và `%` trên `millis()`).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** UART, baud rate, TX/RX, start bit/stop bit, integer overflow, type casting.
- **Datasheet ATmega328P:** mục **USART0** (bài 39 sẽ tự viết `Serial` bằng thanh ghi).

## Câu hỏi phỏng vấn
- **UART cần mấy dây? Khác I2C, SPI thế nào?** Gợi ý: TX, RX và GND chung. Không có dây xung nhịp nên 2 bên phải thống nhất baud trước. Chỉ nối 2 thiết bị với nhau (điểm–điểm).
- **`int` trên AVR bao nhiêu bit? Tại sao code nhúng hay viết `int16_t`, `uint32_t`?** Gợi ý: 16 bit, khác PC (32 bit). Ghi rõ số bit để code chạy đúng trên mọi chip.
