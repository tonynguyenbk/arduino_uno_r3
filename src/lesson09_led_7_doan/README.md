# Bài 9: LED 7 đoạn — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **Hiển thị số đơn giản, rẻ, sáng rõ, nhìn được từ xa:** đồng hồ điện tử, lò vi sóng, nồi cơm điện, điều hòa (hiển thị nhiệt độ), cân điện tử, đồng hồ cây xăng, bảng số tầng thang máy, đồng hồ đếm ngược đèn giao thông.
- **Thiết bị đo công nghiệp:** bộ điều khiển nhiệt độ, đồng hồ đo áp suất, đồng hồ vạn năng — vì đọc được trong môi trường sáng, góc nhìn rộng, bền hàng chục năm.

## Kiến thức của bài trong hệ thống nhúng
- **Mã hóa dữ liệu thành bit:** mỗi chữ số = 1 byte, mỗi bit = 1 thanh. Tư duy "một byte chứa nhiều cờ bật/tắt" chính là cách đọc ghi **thanh ghi** của vi điều khiển (bài 23).
- **Quét hiển thị (multiplexing):** đồng hồ 4 chữ số không dùng 4×8 = 32 chân, mà bật lần lượt từng chữ số thật nhanh (mỗi chữ số vài mili giây), mắt thấy như sáng cùng lúc. Tiết kiệm chân và dòng điện — cũng là cách màn hình LED quảng cáo khổng lồ hoạt động.
- **IC chuyên dụng:** sản phẩm thật dùng IC driver (MAX7219, TM1637, 74HC595) để giảm số chân và gánh dòng điện, vi điều khiển chỉ gửi dữ liệu qua 2–3 dây.

## Dự án thực tế
- **Đồng hồ đếm số thứ tự** ở ngân hàng, bệnh viện; **đồng hồ đếm ngược** đèn giao thông.
- **Bộ điều khiển nhiệt độ công nghiệp** (lò sấy, lò ấp trứng): 2 dãy LED 7 đoạn hiển thị nhiệt độ thực tế và nhiệt độ cài đặt.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Lúc khởi động, thanh sáng sai thứ tự a → g | Dây của thanh đó cắm nhầm chân | So thứ tự sáng với bảng chân trong `main.cpp` |
| Không thanh nào sáng | Chân chung chưa nối GND, hoặc LED loại anode chung (mã ...B...) | Kiểm tra dây GND; anode chung thì nối 5V và đảo HIGH/LOW |
| Số hiện bị lộn ngược | Cắm LED xoay 180° | Dấu chấm DP phải ở góc **dưới phải** |
| Vài thanh tối hơn | Điện trở các thanh khác giá trị | Dùng cùng loại 220Ω |

## Bài tập mở rộng
1. **Dễ:** hiển thị thêm chữ A, b, C, d, E, F (số hệ 16).
2. **Vừa:** nhấn nút thì số tăng 1, giữ nút 1 giây thì về 0.
3. **Khó:** dùng chân DP nháy mỗi giây như dấu `:` của đồng hồ, và hiện số bằng 1 lệnh ghi thẳng thanh ghi cổng (xem trước bài 23).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** seven-segment display, common cathode/anode, multiplexing, bitmask, shift register 74HC595, MAX7219.
- **Datasheet ATmega328P:** mục **I/O-Ports** — 8 chân của 1 cổng (ví dụ PORTD = chân 0–7) ghi được cùng lúc bằng 1 lệnh, rất hợp với "1 byte = 8 thanh".

## Câu hỏi phỏng vấn
- **Làm sao hiển thị 4 chữ số mà chỉ dùng 12 chân?** Gợi ý: quét (multiplexing) — 8 chân thanh dùng chung, 4 chân chọn chữ số, bật lần lượt từng chữ số vài ms.
- **Kiểm tra bit thứ 3 của một byte `x` có bằng 1 không?** Gợi ý: `(x >> 3) & 1` hoặc `x & (1 << 3)`.
