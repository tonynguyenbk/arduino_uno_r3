# Bài 13: Servo SG90 — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **Mô hình điều khiển từ xa:** bẻ lái xe, cánh lái máy bay, bánh lái tàu mô hình.
- **Robot:** khớp tay robot nhỏ, robot hình người, robot chân nhện.
- **Thiết bị dân dụng:** camera xoay quét (pan-tilt), khóa cửa thông minh (đẩy chốt), cửa lấy thức ăn tự động cho thú cưng, cánh đảo gió.

## Kiến thức của bài trong hệ thống nhúng
- **Điều khiển vòng kín (closed-loop):** bên trong servo có biến trở đo góc (giống bài 7) + mạch so sánh: lệch bao nhiêu thì đẩy động cơ bấy nhiêu cho đến khi đúng góc. Đây là ý tưởng của **bộ điều khiển PID** — dùng trong lò nhiệt, xe tự cân bằng, drone, máy bay tự động lái.
- **Giao thức "độ rộng xung 1–2ms, chu kỳ 20ms":** chuẩn từ thời điều khiển mô hình, đến nay vẫn dùng để ra lệnh cho **bộ điều tốc động cơ (ESC)** của drone, xe RC.
- **Xung đột tài nguyên phần cứng:** thư viện Servo chiếm Timer1 → mất PWM chân 9, 10. Thiết kế firmware thật luôn có bảng phân bổ: timer nào, ngắt nào, chân nào dành cho việc gì.
- **Nguồn cho tải động:** servo kéo dòng đột biến khi quay, có thể làm sụt áp, reset vi điều khiển — kỹ sư phải tách nguồn và thêm tụ lọc.

## Dự án thực tế
- **Van điều khiển, cửa gió tự động** trong hệ thống điều hòa trung tâm (HVAC) và ô tô dùng các bộ chấp hành có nguyên lý tương tự.
- **Servo công nghiệp (AC servo)** trong máy CNC, robot hàn, dây chuyền đóng gói: cùng ý tưởng vòng kín nhưng dùng encoder độ phân giải cao, điều khiển cả vị trí, tốc độ, mô-men xoắn.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Servo giật, board tự khởi động lại | Servo kéo dòng lớn làm sụt áp 5V | Thêm tụ 100–470µF gần servo; nhiều servo thì dùng nguồn riêng (chung GND) |
| Chỉ quay được ~0–160° hoặc kêu ở 2 đầu | Servo rẻ có dải xung khác chuẩn | `servo.attach(chan, 600, 2400)` chỉnh dải xung |
| `analogWrite` chân 9/10 không chạy nữa | Thư viện Servo chiếm Timer1 | Dùng chân PWM khác: 3, 5, 6, 11 |
| Servo rung nhẹ khi đứng yên | Nhiễu ADC làm góc đổi liên tục | Chỉ ra lệnh khi góc đổi ≥ 2° (đã có trong code) |

## Bài tập mở rộng
1. **Dễ:** servo tự quét 0° → 180° → 0° bằng `millis()`, không dùng `delay()`.
2. **Vừa:** "radar": gắn HC-SR04 lên servo, quét và in khoảng cách theo từng góc.
3. **Khó:** 2 servo pan-tilt điều khiển bằng 2 biến trở (hoặc joystick).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** servo motor, pulse width, 50Hz, closed-loop control, PID controller, ESC, encoder.
- **Datasheet ATmega328P:** mục **16-bit Timer/Counter1** — thư viện Servo dùng ngắt so sánh của Timer1 để tạo xung cho tối đa 12 servo.

## Câu hỏi phỏng vấn
- **Điều khiển vòng hở và vòng kín khác nhau thế nào?** Gợi ý: vòng hở ra lệnh rồi tin là đúng (động cơ bước); vòng kín đo kết quả và tự sửa sai lệch (servo, điều hòa).
- **Bộ điều khiển PID gồm những thành phần nào?** Gợi ý: P (tỉ lệ với sai lệch hiện tại), I (cộng dồn sai lệch quá khứ → hết sai lệch tĩnh), D (theo tốc độ thay đổi → giảm vọt lố).
