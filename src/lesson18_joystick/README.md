# Bài 18: Joystick — ứng dụng thực tế

## Bên trong joystick
Joystick = **2 biến trở** (bài 7) đặt vuông góc, 1 cho trục ngang, 1 cho trục dọc + **1 nút nhấn** (bài 4) dưới cần gạt. Thả tay thì lò xo kéo cần về giữa, cả 2 trục đọc được ~512.

| Chân module | Nối | Ý nghĩa |
|---|---|---|
| GND, +5V | GND, 5V | Nguồn cho 2 biến trở |
| VRx | A2 | Trục ngang 0..1023 |
| VRy | A3 | Trục dọc 0..1023 |
| SW | A1 | Nút nhấn (nhấn = LOW, dùng `INPUT_PULLUP`) |

## Phần cứng này dùng ở đâu
- **Tay cầm game** (PlayStation, Xbox, Nintendo Switch): mỗi cần analog là đúng loại joystick này (hoặc cảm biến Hall cho bền hơn).
- **Điều khiển từ xa** drone, xe/máy bay mô hình, camera PTZ (xoay–nghiêng–zoom) ở hệ thống an ninh.
- **Xe lăn điện:** người dùng điều khiển hướng và tốc độ bằng 1 cần joystick.
- **Máy công trình, cần cẩu, máy xúc:** cần điều khiển công nghiệp hoạt động cùng nguyên lý, có thêm cơ chế an toàn.

## Kiến thức của bài trong hệ thống nhúng
- **Đọc nhiều kênh ADC:** chip chỉ có 1 bộ ADC, dùng **bộ chọn kênh (multiplexer)** để đo lần lượt từng chân — gọi `analogRead()` cho trục nào thì bộ chọn chuyển sang kênh đó (bài 38 sẽ tự cấu hình).
- **Vùng chết + tỉ lệ:** joystick thật không bao giờ về đúng 512. Vùng chết chống "trôi" (drift) — lỗi nổi tiếng của tay cầm game khi biến trở mòn. Tốc độ tỉ lệ với độ gạt cho cảm giác điều khiển mượt.
- **Hai hệ tọa độ:** joystick "lên" là giá trị tăng, màn hình "lên" là y giảm → phải đổi dấu. Chuyển đổi hệ tọa độ giữa cảm biến và cơ cấu chấp hành là việc thường gặp (robot, drone, máy CNC).
- **Hiệu năng giao tiếp:** đẩy 1KB ra OLED qua I2C 100kHz mất ~100ms, ở 400kHz chỉ ~25ms → chọn tốc độ bus phù hợp, và chỉ cập nhật màn hình khi có thay đổi.

## Dự án thực tế
- **Tay cầm điều khiển drone/xe RC:** đọc 2 joystick (4 trục), gửi không dây qua sóng 2.4GHz cho bộ nhận.
- **Bảng điều khiển camera an ninh PTZ:** joystick gửi lệnh xoay theo giao thức Pelco-D qua RS-485 (UART — bài 2).
- **Robot công nghiệp — tay dạy (teach pendant):** kỹ thuật viên dùng joystick di chuyển tay robot tới từng vị trí rồi lưu lại thành chương trình.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Thả tay mà bút vẫn tự chạy | Joystick không về đúng 512 | Tăng `VUNG_CHET` (ví dụ 80–100) |
| Gạt lên mà bút đi xuống | Mỗi module lắp chiều trục khác nhau | Đổi `DAO_TRUC_DOC = true` |
| Gạt ngang mà bút đi dọc | Đảo dây VRx và VRy | Đổi 2 dây A2/A3 cho nhau |
| Nhấn núm không xóa màn hình | Quên `INPUT_PULLUP` cho chân SW | `pinMode(NUT_SW, INPUT_PULLUP)` |
| Vẽ giật cục | Cập nhật toàn bộ màn hình quá chậm | `Wire.setClock(400000)`; chỉ `display()` khi bút di chuyển |

## Bài tập mở rộng
1. **Dễ:** gạt joystick điều khiển servo (bài 13): trục ngang → góc 0–180°.
2. **Vừa:** trò chơi "rắn săn mồi" trên OLED: joystick đổi hướng, ăn mồi thì dài ra.
3. **Khó:** tự hiệu chuẩn lúc khởi động: đọc 20 lần lúc chưa chạm cần, lấy trung bình làm "tâm" thay cho 512.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** analog joystick, potentiometer, dead zone, stick drift, ADC multiplexer, coordinate transform, teach pendant, PTZ camera.
- **Datasheet ATmega328P:** mục **Analog-to-Digital Converter → ADC Input Channels** (thanh ghi ADMUX chọn kênh A0–A7).

## Câu hỏi phỏng vấn
- **Đọc liên tiếp 2 kênh ADC khác nhau, kênh sau bị "dính" giá trị kênh trước. Tại sao, sửa thế nào?** Gợi ý: tụ lấy mẫu bên trong ADC chưa kịp nạp theo điện áp kênh mới (nhất là khi nguồn có trở kháng cao). Sửa: đọc bỏ 1 lần sau khi đổi kênh, hoặc thêm thời gian chờ, giảm trở kháng nguồn.
- **"Stick drift" trên tay cầm game là gì? Giải pháp phần mềm/phần cứng?** Gợi ý: biến trở mòn → tâm lệch, giá trị nhảy. Phần mềm: vùng chết, hiệu chuẩn lại; phần cứng: cảm biến Hall (không tiếp xúc, không mòn).
