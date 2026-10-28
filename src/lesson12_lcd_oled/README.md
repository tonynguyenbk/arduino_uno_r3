# Bài 12: LCD 1602, OLED qua I2C — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **LCD ký tự 1602/2004:** máy in 3D, máy POS, máy bán vé, bộ điều khiển công nghiệp, máy photocopy, khóa cửa điện tử, máy đo huyết áp đời cũ.
- **OLED nhỏ:** vòng đeo tay thông minh, máy đo nồng độ oxy kẹp ngón tay (SpO2), thuốc lá điện tử, thiết bị cầm tay chạy pin — vì không cần đèn nền, tương phản cao, tốn ít điện khi hiển thị ít điểm sáng.

## Kiến thức của bài trong hệ thống nhúng
- **Bus I2C có mặt khắp nơi trên bo mạch:** cảm biến (gia tốc, áp suất, nhiệt độ), EEPROM lưu cấu hình, đồng hồ thời gian thực (RTC), chip quản lý nguồn trong điện thoại. Một điện thoại có hàng chục thiết bị I2C.
- **Địa chỉ và xung đột địa chỉ:** 2 linh kiện cùng địa chỉ không thể chung 1 bus — kỹ sư phải chọn linh kiện có chân đổi địa chỉ (như A0/A1/A2 trên mạch I2C của LCD) hoặc dùng IC chia bus.
- **Dò thiết bị (I2C scan):** bước debug đầu tiên khi bring-up bo mạch mới: "chip có trả lời không?".
- **Bộ đệm khung hình (frame buffer):** OLED cần 1KB RAM để vẽ trước rồi đẩy ra — mọi màn hình đồ họa đều vậy, màn hình lớn hơn thì cần chip có nhiều RAM hơn hoặc vẽ từng phần.
- **HMI (giao diện người – máy):** thiết kế menu, nút bấm, hiển thị lỗi là một mảng lớn của firmware sản phẩm.

## Dự án thực tế
- **Pin laptop, bộ nguồn máy chủ:** giao tiếp với máy tính qua **SMBus/PMBus** — biến thể của I2C — báo % pin, nhiệt độ, công suất.
- **Máy móc công nghiệp:** màn hình LCD hiển thị thông số, thông báo lỗi bằng mã cho kỹ thuật viên.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| LCD sáng đèn nền nhưng không thấy chữ | Độ tương phản chưa chỉnh | Vặn trimpot xanh sau lưng LCD đến khi thấy chữ |
| "Khong tim thay man hinh nao" | Đảo SDA/SCL, thiếu GND | SDA → A4, SCL → A5, kiểm tra GND |
| LCD tìm thấy ở 0x3F, không phải 0x27 | Mạch I2C phía sau dùng chip khác | Code đã tự dò; nếu viết tay thì đổi địa chỉ |
| OLED không hiện, Serial báo thiếu RAM | OLED cần 1KB RAM cho bộ đệm | Giảm biến/mảng khác; dùng thư viện vẽ từng trang (U8g2) |

## Bài tập mở rộng
1. **Dễ:** hiển thị nhiệt độ, độ ẩm của DHT lên LCD (bài 11 + 12).
2. **Vừa:** menu 2 mục, nút nhấn để chọn, mục đang chọn có dấu `>`.
3. **Khó:** vẽ đồ thị giá trị biến trở theo thời gian trên OLED (cuộn từ phải sang trái).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** I2C, SDA/SCL, slave address, ACK/NACK, open-drain, pull-up resistor, frame buffer, HMI.
- **Datasheet ATmega328P:** mục **2-wire Serial Interface (TWI)** — tên gọi I2C của hãng Atmel (bài 40 sẽ điều khiển bằng thanh ghi).

## Câu hỏi phỏng vấn
- **So sánh I2C và SPI?** Gợi ý: I2C 2 dây, có địa chỉ, nhiều thiết bị chung bus, chậm hơn (100–400kHz). SPI 4 dây, mỗi thiết bị 1 chân chọn riêng, nhanh hơn nhiều (vài MHz–hàng chục MHz), hay dùng cho màn hình lớn, thẻ nhớ.
- **Tại sao bus I2C cần điện trở kéo lên?** Gợi ý: các thiết bị chỉ kéo dây xuống LOW (open-drain), không đẩy lên HIGH, để tránh chập khi 2 thiết bị cùng lái bus.
