# Bài 22: RFID RC522 + thẻ từ — khóa cửa thẻ từ — ứng dụng thực tế

## RFID hoạt động thế nào
- Đầu đọc RC522 phát sóng vô tuyến **13.56MHz** qua cuộn dây ăng-ten in trên bảng mạch.
- Thẻ/móc khóa **không có pin**: cuộn dây trong thẻ "hứng" năng lượng từ sóng đó (cảm ứng điện từ), đủ để con chip trong thẻ chạy và gửi lại dữ liệu. Chỉ hoạt động khi đặt gần (~1–5cm).
- Mỗi thẻ có **UID** (thường 4 byte) ghi khi sản xuất. Thẻ MIFARE Classic 1K còn có 1KB bộ nhớ đọc/ghi được, chia thành các khối có khóa bảo vệ.

## Nối dây RC522 (quan trọng)
| RC522 | Arduino | Ghi chú |
|---|---|---|
| SDA (SS) | 10 | Chọn chip SPI |
| SCK | 13 | Nhịp SPI (cố định trên Uno) |
| MOSI | 11 | Arduino → RC522 (cố định) |
| MISO | 12 | RC522 → Arduino (cố định) |
| RST | 9 | Reset |
| GND | GND | |
| **3.3V** | **3.3V** | **Không cắm 5V** — chip chỉ chịu 3.3V |
| IRQ | — | Để trống |

## Phần cứng này dùng ở đâu
- **Khóa cửa thẻ từ:** khách sạn, căn hộ, văn phòng, tủ đồ, thang máy (quẹt thẻ mới bấm được tầng).
- **Thẻ xe buýt, thẻ tàu điện, thẻ thanh toán không tiếp xúc**, thẻ căn cước gắn chip (cùng họ chuẩn NFC 13.56MHz với ISO 14443).
- **Chấm công, kiểm soát ra vào, thẻ thư viện, thẻ gửi xe.**
- **Quản lý kho, chống trộm hàng hóa** (tem RFID dán trên sản phẩm), thẻ ETC thu phí không dừng trên ô tô (RFID tần số UHF, đọc xa hàng mét).

## Kiến thức của bài trong hệ thống nhúng
- **SPI:** 4 dây, có dây nhịp riêng (SCK), gửi và nhận **cùng lúc** (full-duplex), nhanh (RC522 tới 10MHz). Mỗi thiết bị thêm chỉ cần 1 chân SS riêng. So với I2C (bài 12): nhanh hơn, tốn chân hơn, không cần địa chỉ. Bài 28 sẽ điều khiển SPI bằng thanh ghi.
- **Mức điện áp logic:** RC522 dùng 3.3V, Arduino Uno dùng 5V. Cấp nguồn 3.3V là bắt buộc; chân tín hiệu thường vẫn chạy được trong thực tế nhưng thiết kế chuẩn phải có **mạch chuyển mức (level shifter)**. Vấn đề rất phổ biến khi ghép linh kiện hiện đại (đa số chạy 3.3V hoặc 1.8V).
- **EEPROM:** bộ nhớ 1KB giữ dữ liệu khi mất điện — lưu cấu hình, danh sách thẻ, số lần dùng. Mỗi ô chỉ chịu ~100.000 lần ghi → dùng `EEPROM.update()` (chỉ ghi khi khác) và tránh ghi liên tục trong `loop()`.
- **Bố cục dữ liệu bền vững:** byte dấu hiệu (biết EEPROM đã được khởi tạo chưa), byte số lượng, rồi mảng dữ liệu — kiểu tổ chức giống hệ thống file thu nhỏ.
- **Bảo mật:** UID có thể bị sao chép (thẻ "magic" ghi được UID) → khóa cửa thật cần thẻ có mã hóa (MIFARE DESFire, xác thực bằng khóa bí mật), không chỉ so UID.
- **Tách nguồn dữ liệu khỏi logic:** `coTheMoi()` lấy UID từ đầu đọc thật hoặc từ nút giả lập — phần xử lý giữ nguyên. Kỹ thuật giúp **test firmware khi chưa có phần cứng** (hardware abstraction, mocking).

## Dự án thực tế
- **Hệ thống khóa khách sạn:** máy lễ tân ghi quyền vào phòng + thời hạn lên thẻ; khóa cửa chạy pin, đọc thẻ, kiểm tra thời hạn, lưu nhật ký mở cửa.
- **Kiểm soát ra vào tòa nhà:** đầu đọc ở cửa gửi UID về bộ điều khiển trung tâm (qua RS-485 hoặc mạng), trung tâm quyết định mở và ghi log.
- **Bãi gửi xe thông minh:** thẻ RFID + camera đọc biển số, barie tự mở.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Serial in phiên bản chip `0x00` hoặc `0xFF` | Sai dây SPI, chưa cấp nguồn | Kiểm tra 13/11/12/10/9 và 3.3V; `0x91`/`0x92` là đúng |
| Đọc lúc được lúc không | Nguồn 3.3V yếu, dây dài, chân hàn lỏng | Dây ngắn; hàn chắc chân header của module |
| Module nóng, rồi hỏng | Cấp 5V vào chân 3.3V | Chỉ cấp 3.3V |
| Đặt thẻ 1 lần mà xử lý nhiều lần | Đọc lặp lại cùng thẻ | `PICC_HaltA()` sau khi đọc + thời gian chờ giữa 2 lần quẹt |
| Thẻ chủ không có tác dụng (mạch thật) | `THE_CHU` vẫn là UID giả | Quẹt thẻ của bạn, chép UID từ Serial vào `THE_CHU` |
| Mô phỏng: chạy lại thì danh sách thẻ mất | Wokwi có thể xóa EEPROM mỗi lần chạy | Bình thường; board thật giữ nguyên |

## Bài tập mở rộng
1. **Dễ:** thêm servo (bài 13) làm chốt cửa: mở 90° trong 3 giây rồi đóng lại.
2. **Vừa:** hiển thị trạng thái lên LCD (bài 12): "Moi quet the", "Xin chao!", "The khong hop le".
3. **Khó:** khóa tạm 30 giây nếu quẹt thẻ sai 3 lần liên tiếp (chống dò thẻ), và lưu nhật ký 10 lần mở cửa gần nhất vào EEPROM dạng **bộ đệm vòng** (ring buffer).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** RFID, NFC, 13.56MHz, ISO 14443A, MIFARE Classic, UID, SPI (SCK/MOSI/MISO/SS), logic level shifting, EEPROM wear, hardware abstraction / mocking.
- **Datasheet:** **MFRC522** (thanh ghi VersionReg, giao tiếp SPI), **ATmega328P** mục **SPI – Serial Peripheral Interface** và **EEPROM Data Memory**.

## Câu hỏi phỏng vấn
- **So sánh UART, I2C, SPI?** Gợi ý: UART 2 dây không đồng bộ, điểm–điểm; I2C 2 dây, có địa chỉ, nhiều thiết bị, chậm; SPI 4 dây + 1 SS mỗi thiết bị, full-duplex, nhanh nhất, không có xác nhận (ACK).
- **Nối linh kiện 3.3V với vi điều khiển 5V thế nào cho an toàn?** Gợi ý: nguồn 3.3V riêng; tín hiệu 5V → 3.3V dùng cầu chia áp hoặc IC chuyển mức; 3.3V → 5V thường đủ mức HIGH (kiểm tra VIH trong datasheet) hoặc dùng chuyển mức MOSFET 2 chiều cho I2C.
- **Tại sao không nên ghi EEPROM/Flash trong vòng lặp liên tục?** Gợi ý: số lần ghi có hạn (~100k EEPROM, ~10k Flash) → hỏng ô nhớ sau vài phút/giờ; chỉ ghi khi dữ liệu đổi, gom ghi, dàn đều vị trí ghi (wear leveling).
