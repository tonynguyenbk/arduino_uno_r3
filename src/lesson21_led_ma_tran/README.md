# Bài 21: LED ma trận 8×8 + MAX7219 — ứng dụng thực tế

## Vì sao cần chip MAX7219
- 64 LED nối kiểu ma trận: 8 chân hàng + 8 chân cột = 16 chân, và phải **quét** liên tục (bật từng hàng thật nhanh — như LED 7 đoạn bài 9) → tốn hết chân và thời gian CPU.
- **MAX7219** làm hộ việc quét, tự giới hạn dòng cho LED (1 điện trở chung trên module), chỉnh được độ sáng 16 mức. Arduino chỉ gửi lệnh qua **3 dây** (DIN, CLK, CS). Ghép nối tiếp nhiều module (chân DOUT → DIN module sau) vẫn chỉ 3 dây.

## Phần cứng này dùng ở đâu
- **Bảng chữ chạy LED:** biển hiệu cửa hàng, bảng giờ tàu xe, bảng báo giá vàng/tỷ giá ngân hàng, bảng số thứ tự.
- **Đồng hồ LED trang trí**, bảng tỉ số thể thao, đèn hiệu trên xe buýt (số tuyến, điểm đến).
- **Màn hình LED quảng cáo khổng lồ:** cùng nguyên lý quét ma trận, nhân lên hàng triệu LED RGB với IC driver chuyên dụng.

## Kiến thức của bài trong hệ thống nhúng
- **Bộ đệm khung hình (frame buffer):** vẽ vào RAM trước, xuất ra 1 lần — giống OLED (bài 12), mọi màn hình máy tính, điện thoại. Tách "vẽ" khỏi "xuất" giúp đổi phần cứng hiển thị chỉ cần sửa hàm xuất.
- **Biến đổi tọa độ:** xoay/lật trong hàm xuất → 1 code chạy với mọi kiểu module. Kỹ thuật cùng loại với xoay màn hình điện thoại.
- **PROGMEM:** phông chữ, hình ảnh là dữ liệu hằng → để trong Flash, đọc bằng `pgm_read_byte()`. Thói quen bắt buộc trên chip ít RAM.
- **Phông chữ bitmap:** mỗi ký tự là 1 mảng bit — cách hiển thị chữ trên mọi màn hình LCD/OLED/LED đơn giản.
- **Giao tiếp nối tiếp đồng bộ (có dây xung nhịp CLK):** MAX7219 nhận dữ liệu từng bit theo nhịp CLK, chốt khi CS lên cao — chính là ý tưởng của **SPI** (bài 22, 28).
- **Chip chuyên dụng giảm tải cho CPU:** thay vì vi điều khiển tự quét, giao cho IC chuyên dụng — cách thiết kế phổ biến (driver LED, driver động cơ, codec âm thanh).

## Dự án thực tế
- **Bảng thông tin hành khách** ở bến xe, nhà ga: nhiều module MAX7219 (hoặc driver tương đương) ghép thành dải dài, nhận nội dung qua mạng.
- **Bảng tỉ giá/giá vàng** ở ngân hàng, tiệm vàng: vi điều khiển nhận số liệu qua RS-485/wifi và hiển thị.
- **Đồng hồ thời gian thực + nhiệt độ** treo tường: ma trận LED + RTC (DS1307/DS3231 qua I2C) + cảm biến nhiệt.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Không sáng gì | MAX7219 đang ngủ, hoặc cắm vào phía DOUT | `shutdown(0, false)`; cắm dây vào phía **DIN** (IN) |
| Hình bị xoay 90° hoặc lật | Module lắp ma trận theo chiều khác | Đổi `XOAY_90`, `LAT_NGANG`, `LAT_DOC` |
| Sáng lốm đốm ngẫu nhiên, nhiễu | Dây dài, nguồn yếu | Dây ngắn; thêm tụ 10µF + 100nF gần VCC/GND của module |
| Board reset khi sáng hết LED | Dòng 64 LED quá lớn cho nguồn USB | Giảm `setIntensity`; nhiều module thì dùng nguồn 5V riêng (chung GND) |
| Chữ hiện ra rác | Đọc mảng PROGMEM như mảng thường | Dùng `pgm_read_byte(&mang[i][j])` |

## Bài tập mở rộng
1. **Dễ:** vẽ thêm 2 hình của riêng bạn (mũi tên, con ma Pac-Man) bằng số nhị phân.
2. **Vừa:** thêm chữ số 0–9 vào phông chữ, hiển thị nhiệt độ DHT (bài 11) chạy ngang.
3. **Khó:** trò chơi "rắn săn mồi" trên ma trận 8×8 điều khiển bằng joystick (bài 18).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** LED matrix, multiplexing, MAX7219, frame buffer, bitmap font, PROGMEM, daisy chain, shift register.
- **Datasheet:** **MAX7219** (bảng thanh ghi: Digit 0–7, Decode Mode, Intensity, Scan Limit, Shutdown, Display Test; giản đồ thời gian DIN/CLK/LOAD).

## Câu hỏi phỏng vấn
- **Ghép nối tiếp 4 module MAX7219, gửi dữ liệu cho module cuối cùng thế nào?** Gợi ý: kéo CS xuống, gửi 4 gói 16 bit (gói cho module cuối gửi trước, các module khác gửi lệnh No-Op), rồi kéo CS lên — mỗi chip chốt đúng gói đang nằm trong thanh ghi dịch của nó.
- **Vì sao hiển thị bằng frame buffer tránh được hiện tượng nhấp nháy/xé hình?** Gợi ý: màn hình chỉ nhận khung đã vẽ xong hoàn chỉnh; không ai nhìn thấy trạng thái đang vẽ dở. Màn hình lớn còn dùng 2 bộ đệm luân phiên (double buffering).
