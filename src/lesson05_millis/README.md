# Bài 5: millis() thay delay(), làm nhiều việc cùng lúc — ứng dụng thực tế

## Kiến thức của bài trong hệ thống nhúng
- **Siêu vòng lặp không chặn (superloop):** kiến trúc của phần lớn firmware nhỏ: 1 vòng `loop()` chạy rất nhanh, mỗi "việc" tự kiểm tra đã đến lúc làm chưa. Bài 5 đo được ~84.000 vòng/giây.
- **Nền tảng của RTOS:** hệ điều hành thời gian thực (FreeRTOS, Zephyr) cũng dựa trên một bộ đếm thời gian (tick) giống `millis()`, chỉ khác là tự chia CPU cho các tác vụ.
- **Timeout:** mọi giao tiếp (chờ cảm biến trả lời, chờ wifi kết nối) đều phải có giới hạn thời gian, viết đúng kiểu `if (bayGio - batDau >= TIMEOUT)`.
- **Tràn bộ đếm thời gian:** `millis()` 32 bit quay về 0 sau ~49.7 ngày. Phép trừ `bayGio - lanCuoi` với `unsigned long` vẫn đúng khi tràn. Viết sai sẽ gây lỗi rất khó phát hiện vì chỉ xảy ra sau nhiều ngày:
  - **Windows 95/98:** treo sau đúng 49.7 ngày chạy liên tục vì bộ đếm mili giây 32 bit bị tràn.
  - **Boeing 787 (2015):** cơ quan hàng không Mỹ (FAA) yêu cầu khởi động lại máy bay định kỳ vì bộ điều khiển máy phát điện có bộ đếm tràn sau 248 ngày, có thể làm mất toàn bộ điện xoay chiều.

## Debug bằng avr-stub (bản lesson05_debug)
- Kỹ sư nhúng chuyên nghiệp debug bằng **mạch debug phần cứng** (JTAG/SWD, debugWIRE trên AVR): dừng chip, xem biến, xem thanh ghi mà không làm chậm chương trình. avr-stub là cách "giả lập" việc đó qua cổng Serial.

## Dự án thực tế
- **Bộ điều khiển máy giặt, lò vi sóng:** cùng lúc đếm giờ, đọc phím, cập nhật màn hình, điều khiển động cơ — thường là 1 superloop như bài này.
- **Thiết bị đo trong nhà máy:** đọc cảm biến mỗi 100ms, gửi dữ liệu mỗi 1s, nháy đèn mỗi 500ms — mỗi việc 1 nhịp riêng.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Chạy tốt vài tuần rồi đứng hình | Viết `if (millis() > lanCuoi + T)` → sai khi `millis()` tràn | Luôn viết `if (millis() - lanCuoi >= T)` |
| Sau ~32 giây mọi thứ loạn | Lưu `millis()` vào biến `int` (16 bit) | Dùng `unsigned long` |
| 1 việc chạy chậm kéo trễ các việc khác | Còn `delay()` hoặc vòng chờ bên trong 1 việc | Bỏ hết `delay()`, mỗi việc chỉ "kiểm tra rồi thoát" |
| Debugger dừng ở breakpoint đỏ mãi không tới | avr-stub làm chương trình chậm hàng nghìn lần | Dùng `breakpoint()` trong code (xem `lesson05_debug`) |

## Bài tập mở rộng
1. **Dễ:** thêm việc thứ 4: in chữ `tick` mỗi 3 giây.
2. **Vừa:** LED nháy không đều: sáng 100ms, tắt 900ms (2 khoảng thời gian khác nhau).
3. **Khó:** viết hàm `boolean daDenLuc(unsigned long &lanCuoi, unsigned long chuKy)` để mỗi việc chỉ còn 1 dòng `if`.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** superloop, non-blocking code, cooperative multitasking, timer overflow, RTOS tick, JTAG/debugWIRE.
- **Datasheet ATmega328P:** mục **8-bit Timer/Counter0** — `millis()` đếm được là nhờ ngắt tràn của Timer0 (bài 27 sẽ tự làm lại).

## Câu hỏi phỏng vấn
- **Tại sao `millis() - lanCuoi >= T` vẫn đúng khi `millis()` tràn về 0?** Gợi ý: phép trừ số không dấu là số học modulo 2³², hiệu vẫn ra đúng khoảng thời gian.
- **Superloop và RTOS khác nhau thế nào? Khi nào cần RTOS?** Gợi ý: superloop đơn giản, ít tốn RAM nhưng 1 việc chậm làm trễ cả vòng. Cần RTOS khi có nhiều việc độ ưu tiên khác nhau, có ngăn xếp mạng (wifi, TCP/IP), hoặc yêu cầu thời gian đáp ứng chặt.
