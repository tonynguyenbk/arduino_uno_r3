# Bài 4: Nút nhấn, INPUT_PULLUP, chống dội — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **Nút bấm, bàn phím:** điều khiển TV, bàn phím máy tính, thang máy, máy ATM, lò vi sóng.
- **Công tắc hành trình (limit switch):** máy in 3D, máy CNC dùng để biết trục đã chạy về điểm gốc (homing). Cửa thang máy, cửa gara dùng để biết cửa đã đóng kín.
- **Cảm biến cửa, cảm biến từ:** báo trộm, tủ lạnh (đèn tắt khi đóng cửa).

## Kiến thức của bài trong hệ thống nhúng
- **GPIO input + điện trở kéo lên (pull-up):** chân input không nối gì sẽ "thả nổi" và đọc lung tung (bạn đã thấy ở chân A1 bài 7). Mọi thiết kế phải đảm bảo chân input luôn có mức xác định.
- **Chống dội phím (debounce):** tiếp điểm cơ khí luôn nảy vài mili giây. Giải pháp:
  - Phần mềm: như bài này (chờ ổn định 50ms).
  - Phần cứng: mạch RC + cổng Schmitt trigger, dùng khi không muốn tốn thời gian CPU.
- **Phát hiện cạnh (edge detection):** chỉ phản ứng lúc trạng thái *thay đổi*, không phải lúc *đang ở* trạng thái đó — tư duy dùng lại ở ngắt (bài 26).

## Dự án thực tế
- **PLC công nghiệp:** ngõ vào số thường dùng mức 24V và **cách ly quang (optocoupler)** để chống nhiễu từ động cơ, biến tần trong nhà máy. Bên trong vẫn là ý tưởng đọc HIGH/LOW như bài này.
- **Nút dừng khẩn cấp (E-stop):** trên máy công nghiệp, nút này **ngắt điện trực tiếp bằng phần cứng**, không chỉ báo cho phần mềm — vì phần mềm có thể treo. Đây là nguyên tắc an toàn chức năng (tiêu chuẩn IEC 61508 / ISO 13849).
- **Bàn phím ma trận:** 16 phím chỉ cần 8 chân bằng cách quét hàng/cột — dùng trong khóa cửa điện tử, máy POS.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Nhấn 1 lần mà đếm 2–3 lần | Tiếp điểm bị dội | Chống dội 20–50ms |
| Nút luôn đọc LOW dù không nhấn | 2 chân đang dùng là 2 chân **luôn thông nhau** của nút 4 chân | Xoay nút 90°, hoặc nối 2 chân **chéo nhau** |
| Không nhấn mà giá trị lúc 0 lúc 1 | Quên `INPUT_PULLUP` → chân thả nổi | `pinMode(chan, INPUT_PULLUP)` |
| Logic bị ngược | Với pull-up: nhấn = LOW | So sánh với `LOW` khi kiểm tra nhấn |

## Bài tập mở rộng
1. **Dễ:** đếm số lần nhấn và in ra Serial.
2. **Vừa:** phân biệt **nhấn nhả nhanh** và **nhấn giữ** trên 1 giây (long press) — mỗi kiểu làm 1 việc khác.
3. **Khó:** 2 nút tăng/giảm 1 con số 0–9, giữ nút thì số tự tăng liên tục (auto-repeat như nút chỉnh âm lượng).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** pull-up/pull-down resistor, floating input, debounce, edge detection, long press, Schmitt trigger.
- **Datasheet ATmega328P:** mục **I/O-Ports → Configuring the Pin** (bit DDxn, PORTxn quyết định chân là input có/không pull-up) và **DC Characteristics** (điện trở pull-up bên trong 20–50kΩ).

## Câu hỏi phỏng vấn
- **Tại sao chân input cần điện trở kéo lên hoặc kéo xuống?** Gợi ý: chân input trở kháng rất cao, không nối gì sẽ "bắt" nhiễu và đọc ngẫu nhiên.
- **Có những cách chống dội nào? Ưu nhược điểm?** Gợi ý: phần cứng (mạch RC + Schmitt trigger: không tốn CPU nhưng tốn linh kiện), phần mềm (chờ ổn định / lấy mẫu nhiều lần: miễn phí nhưng tốn thời gian xử lý).
