# Bài 23: Bàn phím ma trận 4×4 — khóa cửa mật mã — ứng dụng thực tế

## Quét bàn phím (scan) — 16 phím chỉ cần 8 chân
```
            C1   C2   C3   C4      ← 4 cột: INPUT_PULLUP (bình thường HIGH)
       R1 ──1────2────3────A──
       R2 ──4────5────6────B──     Lần lượt kéo 1 hàng xuống LOW:
       R3 ──7────8────9────C──     cột nào đọc LOW → phím (hàng, cột) đang nhấn
       R4 ──*────0────#────D──
```
Bấm 1 phím = nối 1 hàng với 1 cột. Quét cả 4 hàng chỉ mất vài chục micro giây → làm hàng trăm lần/giây, không bỏ sót phím.

## Phần cứng này dùng ở đâu
- **Khóa cửa điện tử, két sắt, tủ gửi đồ siêu thị**, hệ thống báo động (nhập mã để tắt).
- **Máy ATM, máy POS thanh toán thẻ**, máy bán hàng tự động, máy bán vé.
- **Điện thoại bàn, máy tính bỏ túi, bảng điều khiển thang máy, lò vi sóng.**
- **Bàn phím máy tính:** hơn 100 phím cũng được quét theo ma trận bằng 1 vi điều khiển bên trong.

## Kiến thức của bài trong hệ thống nhúng
- **Quét ma trận đầu vào:** cùng ý tưởng quét LED (bài 9, 21) nhưng cho nút nhấn. Đọc N×M nút chỉ với N+M chân.
- **Bóng ma (ghosting):** nhấn 3 phím tạo thành 3 góc hình chữ nhật → phím thứ 4 "ảo" cũng đọc là nhấn. Bàn phím cao cấp (gaming) có diode ở mỗi phím để chống — gọi là *N-key rollover*.
- **Chuỗi ký tự trong C:** mảng `char` kết thúc bằng `'\0'`; `strlen`, `strcmp`, `strcpy`, `snprintf` — thao tác chuỗi không dùng `String` của Arduino để tránh phân mảnh RAM.
- **Bảo mật cơ bản:** giới hạn số lần thử sai + khóa tạm (chống dò mã — *brute force*); không hiện mã khi nhập (`****`). Hệ thống thật còn lưu mã dạng **băm (hash)**, không lưu mã gốc, và so sánh trong thời gian cố định để chống tấn công đo thời gian.
- **`static` trong hàm:** biến giữ giá trị giữa các lần gọi mà không cần khai báo toàn cục.
- **Quét bằng ngắt:** sản phẩm chạy pin không quét liên tục, mà để tất cả hàng LOW, bật ngắt thay đổi chân trên các cột (bài 26) → có phím nhấn mới thức dậy quét — tiết kiệm điện.

## Dự án thực tế
- **Khóa cửa căn hộ thông minh:** bàn phím cảm ứng + thẻ RFID (bài 22) + vân tay, chạy pin AA cả năm nhờ ngủ sâu và đánh thức bằng ngắt.
- **Hệ thống báo động nhà/kho:** bàn phím nhập mã để bật/tắt, cảm biến cửa (bài 4), còi (bài 17), gửi tin nhắn qua module SIM.
- **Thiết bị công nghiệp (HMI đơn giản):** bàn phím màng chống nước + LCD để nhập thông số máy.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Phím nhận sai ký tự | Bàn phím của kit có thứ tự chân khác | Đọc chữ in trên bảng mạch (C4 C3 C2 C1 R1 R2 R3 R4), sửa mảng `CHAN_HANG`/`CHAN_COT` hoặc `BAN_PHIM` |
| Cả 1 hàng/cột không nhận | Dây hàng/cột đó lỏng | Kiểm tra từng dây; in `quetPhim()` ra Serial để thử từng phím |
| Nhấn 1 lần ra 2 ký tự | Dội phím | Tăng thời gian chống dội (30ms → 50ms) |
| Nhấn nhiều phím cùng lúc ra phím lạ | Ghosting của ma trận không có diode | Bình thường; chỉ xử lý 1 phím tại một thời điểm |
| Quên mật mã đã đổi | Mã lưu trong EEPROM | Xóa byte dấu hiệu: `EEPROM.write(0, 0)` → về mã mặc định 1234 |

## Bài tập mở rộng
1. **Dễ:** phím `B` = hiện số lần mở cửa đã đếm (lưu EEPROM).
2. **Vừa:** "máy tính bỏ túi": nhập số, `A` = cộng, `B` = trừ, `C` = nhân, `D` = chia, `#` = bằng, hiện kết quả trên LCD.
3. **Khó:** kết hợp bài 22: mở cửa cần **cả** thẻ RFID đúng **và** mã PIN đúng (xác thực 2 yếu tố).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** keypad matrix scanning, ghosting, N-key rollover, debounce, C string (strcmp/strcpy), brute-force protection, password hashing, pin change interrupt.
- **Datasheet ATmega328P:** mục **I/O-Ports** (cấu hình chân hàng OUTPUT, chân cột INPUT với pull-up) và **External Interrupts → Pin Change Interrupt** (đánh thức khi có phím).

## Câu hỏi phỏng vấn
- **Bàn phím 4×4 cần mấy chân? 8×8 thì sao? Có cách nào ít chân hơn?** Gợi ý: 8 và 16 chân. Ít hơn: IC mở rộng I/O qua I2C (PCF8574), thanh ghi dịch, hoặc mạch chia áp mỗi phím một điện trở khác nhau → đọc bằng 1 chân ADC.
- **Tại sao lưu mật mã dạng băm (hash) thay vì mã gốc?** Gợi ý: ai đọc được bộ nhớ (dump EEPROM/Flash) cũng không lấy được mã; khi kiểm tra chỉ cần băm mã vừa nhập rồi so 2 giá trị băm.
