# Bài 10: Cảm biến siêu âm HC-SR04 — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **Cảm biến lùi xe ô tô:** các "nút tròn" trên cản xe chính là cảm biến siêu âm, kêu bíp nhanh dần khi gần vật cản — đúng như bài 10 làm với LED.
- **Robot tránh vật cản, robot hút bụi đời đầu.**
- **Đo mức chất lỏng:** gắn trên nắp bồn nước, bồn xăng dầu, silo chứa hạt để đo khoảng cách tới mặt chất lỏng mà không cần chạm vào.
- **Máy đếm người** ở cửa ra vào, **vòi nước/máy sấy tay tự động.**

## Kiến thức của bài trong hệ thống nhúng
- **Đo thời gian bay (time of flight):** phát tín hiệu, đo thời gian quay về, nhân vận tốc. Cùng nguyên lý với **radar** ô tô tự lái (sóng vô tuyến), **LiDAR** (ánh sáng laser), máy đo độ sâu đáy biển (sonar), và cảm biến ToF trong camera điện thoại.
- **Đo độ rộng xung:** `pulseIn()` chờ bằng vòng lặp, chặn CPU tới 30ms. Firmware chuyên nghiệp dùng **Input Capture của Timer** (bài 27): phần cứng tự ghi lại thời điểm cạnh lên/xuống, CPU rảnh làm việc khác.
- **Bù sai số môi trường:** tốc độ âm thanh đổi theo nhiệt độ (~0.6 m/s mỗi độ C). Thiết bị đo chính xác đo thêm nhiệt độ (bài 11) để hiệu chỉnh.

## Dự án thực tế
- **Công nghiệp:** máy đo mức siêu âm (ultrasonic level transmitter) trong nhà máy nước, xử lý nước thải, gửi kết quả về phòng điều khiển qua chuẩn 4–20mA hoặc Modbus.
- **Bãi đỗ xe thông minh:** cảm biến trên trần mỗi ô đỗ báo đèn xanh/đỏ còn chỗ hay không.
- **Drone:** giữ độ cao ổn định khi bay sát mặt đất.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Luôn báo "ngoài tầm đo" | Đảo dây TRIG và ECHO, hoặc thiếu nguồn 5V | Kiểm tra lại 4 dây |
| Số nhảy lung tung | Vật mềm (vải, rèm) hấp thụ âm, vật nghiêng làm sóng dội đi chỗ khác | Thử với vật cứng, phẳng, đặt vuông góc |
| Vật rất gần (< 2cm) đo sai | Cảm biến có vùng mù gần | Giới hạn đo từ 2–3cm |
| Dùng 2 cảm biến cùng lúc thì sai | Cảm biến này nghe nhầm tiếng của cảm biến kia | Kích lần lượt, cách nhau ≥ 30ms |

## Bài tập mở rộng
1. **Dễ:** thêm còi buzzer kêu bíp theo nhịp của LED.
2. **Vừa:** đo 5 lần, lấy **giá trị trung vị** (median) để loại số nhiễu.
3. **Khó:** hiển thị khoảng cách lên LCD (kết hợp bài 12) hoặc gắn cảm biến lên servo quay quét như radar (bài 13).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** ultrasonic sensor, time of flight, echo, input capture, median filter, LiDAR.
- **Datasheet ATmega328P:** mục **16-bit Timer/Counter1 → Input Capture Unit** (chân ICP1 = chân 8 Arduino) — cách đo độ rộng xung không chặn CPU.

## Câu hỏi phỏng vấn
- **`pulseIn()` có nhược điểm gì? Thay bằng gì?** Gợi ý: chặn CPU suốt thời gian chờ. Thay bằng ngắt thay đổi chân hoặc Input Capture của timer.
- **Tại sao lọc trung vị tốt hơn trung bình cho cảm biến này?** Gợi ý: trung bình bị kéo lệch bởi 1 giá trị sai rất lớn; trung vị loại hẳn giá trị đột biến.
