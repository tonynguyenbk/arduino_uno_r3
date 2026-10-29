# Bài 26: Núm xoay vô cực (rotary encoder) — ứng dụng thực tế

## Tín hiệu cầu phương (quadrature)
```
          ┌───┐   ┌───┐   ┌───        A (CLK)
xoay phải ┘   └───┘   └───┘
            ┌───┐   ┌───┐   ┌─      B (DT)  ← B trễ hơn A 1/4 chu kỳ
          ──┘   └───┘   └───┘
 trạng thái AB: 11 → 01 → 00 → 10 → 11  (1 nấc)    xoay trái: 11 → 10 → 00 → 01 → 11
```
Nhìn **tín hiệu nào đổi trước** là biết chiều quay. Bảng `BANG_CHUYEN[cũ][mới]` tra ra +1/−1 cho mọi cặp trạng thái hợp lệ, cặp không hợp lệ (nhiễu, nảy tiếp điểm) cho 0 → tự lọc được nhiễu.

## Phần cứng này dùng ở đâu
- **Núm vặn không có điểm dừng:** núm âm lượng ampli/xe hơi, núm lò vi sóng, núm chọn menu máy in 3D, máy hàn, bàn trộn âm thanh, nồi chiên không dầu.
- **Con lăn chuột máy tính:** encoder cơ hoặc quang.
- **Encoder quang công nghiệp** (hàng trăm đến hàng nghìn xung/vòng) gắn trên trục động cơ: đo tốc độ và vị trí cho máy CNC, robot, băng tải, servo công nghiệp (bài 13), động cơ có encoder trong robot (bài 15).

## Kiến thức của bài trong hệ thống nhúng
- **Ngắt (interrupt):** phần cứng tự gọi hàm khi chân đổi mức, không bỏ lỡ xung dù `loop()` đang bận. Đây là cách hệ thống nhúng phản ứng với sự kiện bên ngoài trong thời gian thực. Bài 36 sẽ tự cấu hình ngắt bằng thanh ghi (EICRA, EIMSK).
- **Quy tắc viết hàm ngắt (ISR):** thật ngắn; không `Serial.print`, không `delay`; chỉ cập nhật biến rồi thoát — xử lý nặng để `loop()` làm.
- **`volatile`:** bắt trình biên dịch đọc lại biến từ RAM mỗi lần, vì biến có thể bị ngắt sửa bất cứ lúc nào.
- **Đọc nguyên tử (atomic):** `long` 4 byte, CPU 8 bit đọc trong 4 lần; nếu ngắt chen giữa → đọc ra số "nửa cũ nửa mới". Giải pháp: tắt ngắt trong lúc đọc (`noInterrupts()` / `interrupts()`) — gọi là **vùng găng (critical section)**.
- **Bộ giải mã quadrature phần cứng:** vi điều khiển cao cấp (STM32 Timer ở chế độ Encoder) tự đếm xung cầu phương bằng phần cứng, CPU không tốn công — dùng cho động cơ quay nhanh hàng chục nghìn xung/giây.

## Dự án thực tế
- **Điều khiển vị trí động cơ (servo công nghiệp, robot):** encoder trên trục → đếm vị trí → bộ PID tính lệnh PWM cho cầu H (bài 15).
- **Thiết bị âm thanh số:** núm volume encoder + màn hình, giá trị lưu EEPROM khi tắt máy.
- **Giao diện máy in 3D, máy CNC mini:** 1 núm xoay + nhấn để duyệt toàn bộ menu trên LCD — tiết kiệm nút bấm.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Xoay 1 nấc mà nhảy 2 hoặc 4 bước | Loại encoder mỗi nấc = 2 hoặc 4 lần đổi trạng thái khác nhau | Đổi số chia `/ 4` thành `/ 2` hoặc bỏ |
| Xoay nhanh bị đếm thiếu/sai chiều | Đọc bằng `loop()` thay vì ngắt, hoặc ISR quá chậm | Dùng ngắt, ISR chỉ vài dòng |
| Chiều quay bị ngược | Đảo dây CLK/DT | Đổi 2 dây, hoặc đổi dấu trong bảng |
| Số nhảy lung tung khi không xoay | Tiếp điểm cơ nảy; thiếu điện trở kéo lên | `INPUT_PULLUP`; module KY-040 có sẵn trở; thêm tụ 10nF từ mỗi chân xuống GND |
| Đọc giá trị thỉnh thoảng sai hẳn | Đọc biến `long` khi ngắt đang sửa | Đọc trong vùng tắt ngắt (`docSoBuoc()`) |

## Bài tập mở rộng
1. **Dễ:** dùng núm xoay chỉnh độ sáng LED (PWM, bài 6) thay vì in Serial.
2. **Vừa:** "xoay nhanh tăng nhanh": nếu 2 nấc cách nhau dưới 50ms thì mỗi nấc tăng 10 thay vì 1 (gia tốc như núm máy hàn).
3. **Khó:** menu trên LCD (bài 12): xoay chọn mục (Độ sáng / Âm lượng / Thoát), nhấn để vào chỉnh, nhấn lần nữa để lưu vào EEPROM.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** rotary encoder, quadrature encoding, incremental vs absolute encoder, interrupt service routine (ISR), volatile, critical section, atomic access, encoder mode timer.
- **Datasheet ATmega328P:** mục **External Interrupts** (INT0 = chân 2, INT1 = chân 3; thanh ghi EICRA chọn kiểu sườn, EIMSK bật ngắt) và **Pin Change Interrupt** (ngắt trên mọi chân).

## Câu hỏi phỏng vấn
- **Vì sao biến dùng chung giữa ISR và chương trình chính phải là `volatile`? `volatile` có đủ để an toàn không?** Gợi ý: `volatile` chỉ ngăn trình biên dịch cache giá trị; không đảm bảo nguyên tử — biến nhiều byte vẫn cần tắt ngắt khi đọc/ghi.
- **Những điều không nên làm trong ISR?** Gợi ý: chờ đợi (delay, vòng lặp chờ), in Serial (dùng ngắt bên trong, có thể treo), tính toán nặng, cấp phát bộ nhớ. Đặt cờ rồi để vòng lặp chính xử lý.
- **Encoder tương đối (incremental) và tuyệt đối (absolute) khác nhau thế nào?** Gợi ý: tương đối chỉ đếm thay đổi, mất điện là mất vị trí (cần về gốc); tuyệt đối cho mã vị trí duy nhất ngay khi bật máy.
