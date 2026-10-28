# Bài 15: Động cơ DC + cầu H L298N — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **Động cơ DC có chổi than:** xe đồ chơi, robot, máy khoan pin, **cửa kính điện, gạt mưa, ghế chỉnh điện ô tô**, máy cạo râu, bơm nước mini.
- **Cầu H:** bất cứ chỗ nào cần đảo chiều động cơ — cửa cuốn, rèm tự động, khóa cửa ô tô, robot hai bánh.
- **L298N** là đời cũ (tỏa nhiệt nhiều, sụt ~2V). Sản phẩm hiện đại dùng IC MOSFET hiệu suất cao (DRV8833, TB6612) hoặc tự thiết kế cầu H bằng MOSFET rời cho dòng hàng chục ampe.

## Kiến thức của bài trong hệ thống nhúng
- **PWM + cầu H = nền tảng điện tử công suất:**
  - Động cơ **BLDC không chổi than** (drone, quạt inverter, xe máy điện, ổ cứng) dùng **3 nhánh cầu** thay vì 2, vi điều khiển đổi pha liên tục.
  - **Bộ biến tần xe điện** (Tesla, VinFast) về bản chất là cầu 3 pha công suất rất lớn, điều khiển bằng PWM tần số cao và bảng sin.
- **Dốc tăng/giảm tốc (ramp):** chống giật cơ khí và dòng khởi động lớn — thang máy, băng tải, xe điện đều có. Biến tần công nghiệp có tham số "thời gian tăng tốc" để cài đặt.
- **Bảo vệ:** đo dòng động cơ để phát hiện kẹt (cửa kính ô tô tự đảo chiều khi kẹt tay — chống kẹt là tính năng an toàn bắt buộc ở nhiều nước), diode chống điện áp ngược khi ngắt cuộn dây.
- **Phanh và phanh tái sinh:** chập 2 đầu động cơ (IN1 = IN2 = HIGH) để phanh. Xe điện còn biến động năng thành điện nạp lại pin khi phanh.
- **Chung GND, tách nguồn công suất:** nguyên tắc thiết kế bắt buộc để tránh nhiễu và reset vi điều khiển.

## Dự án thực tế
- **Xe tự hành trong kho (AGV/AMR)** của các trung tâm logistics: động cơ DC/BLDC + cầu H + encoder đo tốc độ + PID.
- **Băng tải** trong dây chuyền đóng gói, phân loại hàng.
- **Xe lăn điện, xe scooter điện:** tay ga (biến trở/cảm biến Hall — bài 7) → vi điều khiển → cầu H công suất → động cơ, có ramp tăng tốc mượt.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Động cơ không quay dù LED trên L298N sáng | Chưa chung GND giữa Arduino và L298N | Nối GND Arduino với GND L298N |
| Luôn chạy hết tốc độ, PWM không có tác dụng | Còn cắm jumper trên chân ENA | Tháo jumper, nối ENA vào chân 5 |
| Arduino tự khởi động lại khi động cơ chạy | Lấy nguồn động cơ từ 5V của Arduino | Dùng pin/adapter riêng cho cổng 12V của L298N |
| Tốc độ thấp động cơ chỉ kêu, không quay | PWM nhỏ không thắng được ma sát | Đặt tốc độ tối thiểu (ví dụ 60/255) khi khởi động |
| Động cơ yếu, L298N rất nóng | L298N sụt khoảng 2V và tỏa nhiệt — đặc tính của chip | Dùng nguồn cao hơn 2V so với định mức động cơ, hoặc driver MOSFET |

## Bài tập mở rộng
1. **Dễ:** 3 nút: tiến, lùi, dừng (thay biến trở).
2. **Vừa:** thêm động cơ thứ 2 (ENB, IN3, IN4) → điều khiển robot 2 bánh: tiến, lùi, rẽ trái, rẽ phải.
3. **Khó:** kết hợp HC-SR04 (bài 10): tự giảm tốc khi gần vật cản, phanh gấp (IN1 = IN2 = HIGH) khi dưới 15cm.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** H-bridge, motor driver, PWM speed control, soft start/ramp, back-EMF, flyback diode, shoot-through, dead time, regenerative braking.
- **Datasheet:** **L298** (dòng tối đa 2A/kênh, điện áp bão hòa ~2V) — so với **TB6612FNG**, **DRV8833** để thấy vì sao sản phẩm mới không dùng L298 nữa.

## Câu hỏi phỏng vấn
- **"Shoot-through" trong cầu H là gì? Phòng tránh thế nào?** Gợi ý: 2 công tắc cùng một nhánh cùng dẫn → chập nguồn xuống GND, cháy MOSFET. Phòng bằng thời gian chết (dead time) giữa lúc tắt công tắc này và bật công tắc kia.
- **Tại sao phải có diode song song với cuộn dây động cơ/relay?** Gợi ý: khi ngắt dòng, cuộn cảm sinh điện áp ngược rất cao (back-EMF) có thể phá hỏng transistor; diode tạo đường thoát cho dòng này.
