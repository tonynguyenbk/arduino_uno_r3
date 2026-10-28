# Bài 8: Hàm, mảng, bảng tra gamma — ứng dụng thực tế

## Kiến thức của bài trong hệ thống nhúng
- **Bảng tra (lookup table, LUT):** đổi bộ nhớ lấy tốc độ — kỹ thuật kinh điển của nhúng vì vi điều khiển yếu và thường không có bộ tính số thực (bài 8 đo được tra bảng nhanh hơn `pow()` ~460 lần). Ví dụ:
  - **Bảng sin** trong biến tần, điều khiển động cơ xe điện: tạo dòng xoay chiều từ PWM mà không cần tính `sin()`.
  - **Bảng CRC:** tính mã kiểm tra lỗi khi truyền dữ liệu (Modbus, Ethernet, file nén) nhanh gấp nhiều lần.
  - **Bảng tuyến tính hóa cảm biến:** nhiệt điện trở NTC có đường cong phi tuyến, dùng bảng để đổi ADC ra độ C.
  - **Bảng gamma** trong mọi màn hình.
- **Tối ưu bộ nhớ:** chip lớn hơn có thể đặt bảng trong bộ nhớ Flash (`PROGMEM` trên AVR) để không tốn RAM — sẽ gặp lại khi RAM chật.
- **Hàm = tổ chức code thành lớp:** firmware chuyên nghiệp chia thành các lớp: **HAL** (lớp trừu tượng phần cứng: `docNut()`, `datDoSang()`) → **driver** → **ứng dụng**. Đổi chip chỉ phải viết lại lớp HAL.
- **Đo thời gian thực thi** bằng `micros()`: kỹ sư nhúng luôn phải biết đoạn code tốn bao lâu, đặc biệt trong ngắt (bài 18).

## Dự án thực tế
- **Bộ điều khiển động cơ BLDC/xe điện:** vòng điều khiển chạy 10.000–20.000 lần/giây, gần như mọi phép toán nặng đều thay bằng bảng tra hoặc số nguyên.
- **Thư viện của hãng chip (STM32 HAL, ESP-IDF):** chính là tập hợp hàng nghìn hàm chia lớp như trên.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Biến khác tự dưng đổi giá trị, chương trình chạy kỳ lạ | Ghi ra ngoài mảng (ví dụ `bang[256]` với mảng 256 phần tử) | Chỉ số hợp lệ là 0 → N−1; kiểm tra điều kiện vòng `for` |
| Build lỗi `'tenHam' was not declared` | Gọi hàm trước khi định nghĩa (file `.cpp` không tự thêm khai báo như Arduino IDE) | Đặt hàm phía trên chỗ gọi, hoặc khai báo trước: `int tenHam(int);` |
| Chạy một lúc thì treo/reset | Hết RAM (2KB): mảng lớn + chuỗi ký tự | Xem `RAM:` khi build; đưa hằng số, chuỗi sang Flash (`PROGMEM`, `F("...")`) |

## Bài tập mở rộng
1. **Dễ:** viết hàm `trungBinh(int mang[], int soPhanTu)` trả về trung bình cộng.
2. **Vừa:** chuyển `bangGamma` sang `const PROGMEM` + `pgm_read_byte()`, so RAM trước/sau khi build.
3. **Khó:** bảng sin 64 phần tử để LED "thở" theo đường sin, đo thời gian so với gọi `sin()`.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** lookup table, PROGMEM, function prototype, stack, heap, buffer overflow, HAL (hardware abstraction layer).
- **Datasheet ATmega328P:** mục **AVR Memories** (Flash 32KB chứa code, SRAM 2KB chứa biến, EEPROM 1KB lưu dữ liệu khi mất điện).

## Câu hỏi phỏng vấn
- **Khi nào nên dùng bảng tra thay vì tính toán?** Gợi ý: hàm tốn thời gian, miền giá trị đầu vào nhỏ và cố định, còn dư bộ nhớ, cần thời gian chạy ổn định.
- **Biến cục bộ và biến toàn cục nằm ở đâu trong RAM?** Gợi ý: biến toàn cục ở vùng `.data`/`.bss` cố định; biến cục bộ ở **stack**, tự giải phóng khi hàm kết thúc. Stack tràn sang vùng biến toàn cục là lỗi kinh điển trên chip ít RAM.
