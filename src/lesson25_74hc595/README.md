# Bài 25: IC 74HC595 — thanh ghi dịch — ứng dụng thực tế

## Bên trong 74HC595
```
 DS (dữ liệu) ──►┌─┬─┬─┬─┬─┬─┬─┬─┐──► Q7S (nối sang chip thứ 2)
 SHCP (nhịp) ───►│ THANH GHI DỊCH 8 bit │   mỗi xung SHCP: đọc 1 bit ở DS, đẩy cả hàng sang 1 ô
                 └─┴─┴─┴─┴─┴─┴─┴─┘
 STCP (chốt) ───►┌─┬─┬─┬─┬─┬─┬─┬─┐
                 │ THANH GHI CHỐT 8 bit │   xung STCP: chép cả 8 bit ra Q0..Q7 CÙNG LÚC
                 └─┴─┴─┴─┴─┴─┴─┴─┘
                  Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7  ──► 8 LED
```
Sơ đồ chân (nhìn từ trên, vết khuyết bán nguyệt quay lên): bên trái từ trên xuống chân 1–8 (Q1–Q7, GND), bên phải từ dưới lên chân 9–16 (Q7S, MR, SHCP, STCP, OE, DS, Q0, VCC).

## Phần cứng này dùng ở đâu
- **Mở rộng chân ra:** bảng LED, dải đèn trang trí, LED 7 đoạn nhiều chữ số, ma trận LED lớn — mỗi chip thêm 8 đầu ra, nối tiếp không giới hạn mà vẫn chỉ 3 chân vi điều khiển.
- **Điều khiển nhiều relay** trong tủ điện, bảng điều khiển máy (một module 8 relay thường có 74HC595 hoặc chip tương tự).
- **Chip "anh em" 74HC165:** thanh ghi dịch chiều ngược lại — **mở rộng chân vào** (đọc 8 nút/công tắc bằng 3 chân). Tay cầm điện tử NES (Nintendo đời đầu) đọc 8 nút bằng đúng chip loại này.

## Kiến thức của bài trong hệ thống nhúng
- **Chuyển đổi nối tiếp → song song:** gửi từng bit trên 1 dây, nhận 8 đầu ra song song. Ngược lại là song song → nối tiếp. Đây là bản chất của mọi giao tiếp nối tiếp (UART, SPI, I2C, USB...).
- **Đồng hồ (clock) và chốt (latch):** dữ liệu được lấy mẫu ở **cạnh lên** của xung nhịp; tách "đang nạp" khỏi "xuất ra" bằng chốt → đầu ra không nhấp nháy giá trị trung gian. Cùng ý tưởng với bộ đệm khung hình (bài 21) và thanh ghi đệm đôi trong timer (bài 37).
- **Bit-banging:** tự tạo giao thức bằng cách bật tắt chân trong code (`guiByte()`). Linh hoạt nhưng tốn CPU, chậm. 74HC595 cũng dùng được với **SPI phần cứng** (DS → MOSI chân 11, SHCP → SCK chân 13) — nhanh gấp hàng chục lần. Bài này dùng DS ở chân 10 để mạch mô phỏng gọn, còn chân 11 (PWM) dành cho OE.
- **Chân OE (Output Enable) + PWM:** bật/tắt cả 8 đầu ra cùng lúc thật nhanh → chỉnh độ sáng tất cả LED chỉ bằng 1 chân PWM. OE hoạt động ở mức **thấp** (active-low) nên phải đảo giá trị: `analogWrite(OE, 255 - doSang)`.
- **Phép dịch bit:** `1 << buoc`, `0xFF >> n`, `(giaTri >> bit) & 1` — cùng các phép ở bài 9, 21, chuẩn bị cho thanh ghi (bài 35).

## Dự án thực tế
- **Bảng điểm, đồng hồ đếm ngược LED 7 đoạn cỡ lớn:** mỗi chữ số 1 chip 74HC595 (loại công suất TPIC6B595 cho LED lớn 12V).
- **Bảng điều khiển công nghiệp:** hàng chục đèn báo trạng thái điều khiển bằng chuỗi thanh ghi dịch.
- **Máy in nhiệt, máy in hóa đơn:** đầu in nhận 1 hàng chấm qua thanh ghi dịch dài hàng trăm bit rồi chốt in 1 lần.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Không LED nào sáng | OE nối 5V (tắt đầu ra) hoặc MR nối GND (luôn xóa) | OE → GND, MR → 5V |
| LED sáng lung tung khi mới cắm điện | Thanh ghi chứa rác lúc khởi động | Gửi `guiByte(0)` trong `setup()`; hoặc điều khiển OE bằng 1 chân |
| Thứ tự LED bị ngược | Gửi bit thấp trước thay vì bit cao trước | Đổi thứ tự vòng lặp, hoặc `LSBFIRST`/`MSBFIRST` trong `shiftOut` |
| Đèn nhấp nháy khi đang cập nhật | Quên dùng chốt (STCP nối cố định) | STCP phải ở LOW khi dịch, lên HIGH sau khi gửi xong |
| Chip nóng, LED mờ khi sáng nhiều | Tổng dòng 8 LED vượt giới hạn chip (~70mA) | Điện trở lớn hơn (330Ω–1kΩ) hoặc dùng TPIC6B595 |

## Bài tập mở rộng
1. **Dễ:** dùng `shiftOut()` của Arduino thay cho `guiByte()` tự viết, kết quả phải y hệt.
2. **Vừa:** hiển thị giá trị biến trở (bài 7) dạng thanh mức 8 LED.
3. **Khó:** nối tiếp 2 chip (Q7S chip 1 → DS chip 2, chung SHCP và STCP) để điều khiển 16 LED; gửi 2 byte rồi mới chốt.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** shift register, serial-in parallel-out (SIPO), latch, clock edge, daisy chain, bit-banging, 74HC165 (PISO), TPIC6B595.
- **Datasheet:** **74HC595** (bảng chức năng, giản đồ thời gian: thời gian thiết lập dữ liệu trước cạnh nhịp, dòng ra tối đa mỗi chân và cả chip).

## Câu hỏi phỏng vấn
- **Tại sao 74HC595 có 2 thanh ghi (dịch và chốt) thay vì 1?** Gợi ý: để đầu ra không thay đổi theo từng bit trong lúc dịch; chỉ cập nhật đồng thời khi dữ liệu đã đủ.
- **Bit-banging và dùng ngoại vi phần cứng (SPI) khác nhau thế nào?** Gợi ý: bit-banging linh hoạt (chân nào cũng được), nhưng tốn CPU, tốc độ thấp và bị ngắt làm sai thời gian; ngoại vi phần cứng nhanh, chính xác, CPU rảnh làm việc khác nhưng chỉ dùng được chân cố định.
