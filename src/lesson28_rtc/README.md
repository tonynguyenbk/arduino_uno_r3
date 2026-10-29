# Bài 28: Module thời gian thực RTC DS1307 — đồng hồ báo thức

## RTC hoạt động thế nào
- **RTC = Real Time Clock:** chip chuyên đếm giờ, chạy bằng **thạch anh 32.768 kHz** (= 2^15 → chia đôi 15 lần được đúng 1 xung/giây).
- Có **pin CR2032** riêng: rút điện Arduino vẫn nhớ ngày giờ (vài năm). `millis()` thì về 0 mỗi lần reset và lệch vài giây mỗi giờ.
- **DS1307** giao tiếp **I2C địa chỉ 0x68**, lưu giờ dạng **BCD** (mỗi chữ số 4 bit), tự tính tháng 28/29/30/31 ngày, năm nhuận.
- **DS3231** (bản xịn hơn): thạch anh có **bù nhiệt** → lệch ~1 phút/năm (DS1307 lệch ~1 phút/tháng), có 2 báo thức phần cứng, cảm biến nhiệt. Code gần như giống hệt (`RTC_DS3231`).
- **Hai thiết bị trên một bus I2C:** RTC (0x68) và LCD (0x27) nối chung SDA/SCL — Arduino gọi theo địa chỉ, thiết bị nào đúng địa chỉ mới trả lời. Dùng chương trình dò I2C ở bài 12 sẽ thấy cả hai.

## Phần cứng này dùng ở đâu
- **Đồng hồ, lịch, hẹn giờ:** đồng hồ báo thức, hẹn giờ tưới cây/cho cá ăn, bật tắt đèn đường theo giờ, ổ cắm hẹn giờ.
- **Ghi dữ liệu có mốc thời gian (data logger):** trạm thời tiết, công tơ điện, máy chấm công ghi "ai, lúc mấy giờ".
- **Máy tính, điện thoại, xe ô tô:** bo mạch chủ có RTC + pin CMOS — tháo pin là máy hỏi lại ngày giờ.

## Kiến thức của bài trong hệ thống nhúng
- **Đồng hồ hệ thống vs đồng hồ thực:** `millis()` = thời gian *kể từ lúc bật*; RTC = *ngày giờ ngoài đời*. Firmware thật dùng cả hai: millis để đo khoảng ngắn, RTC để biết "bây giờ là mấy giờ".
- **Unix time:** đếm số giây kể từ 1/1/1970 (`t.unixtime()`). Cộng trừ thời gian dễ hơn nhiều so với tự xử lý giờ/ngày/tháng — `TimeSpan` của RTClib làm việc này.
- **Đặt giờ ban đầu:** `__DATE__`/`__TIME__` là giờ *biên dịch*; sản phẩm thật đồng bộ qua mạng (**NTP**), GPS hoặc sóng radio (đồng hồ DCF77 ở châu Âu).
- **Không đọc thiết bị liên tục khi không cần:** chỉ vẽ lại LCD khi sang giây mới → giảm lưu lượng I2C, giảm nhấp nháy.
- **Nhận lệnh từ Serial:** gom từng ký tự vào bộ đệm tới khi gặp Enter, rồi tách bằng `sscanf` — bài 32 sẽ làm bộ tách lệnh đầy đủ.

## Dự án thực tế
- **Bộ hẹn giờ tưới vườn:** RTC + relay (bài 24) + cảm biến độ ẩm đất, tưới 6h sáng và 17h chiều.
- **Máy chấm công RFID:** quẹt thẻ (bài 22) → ghi mã thẻ + ngày giờ vào thẻ nhớ SD.
- **Đồng hồ LED ma trận:** RTC + MAX7219 (bài 21) hiển thị giờ chữ chạy.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| "Khong tim thay RTC" | Sai dây SDA/SCL, thiếu nguồn | SDA → A4, SCL → A5; chạy dò I2C (bài 12) phải thấy 0x68 |
| Giờ về 1/1/2000 sau mỗi lần rút điện | Hết pin CR2032, hoặc lắp loại pin sạc LIR2032 trên module không có mạch sạc phù hợp | Thay pin; xem module có diode/điện trở sạc không |
| Mỗi lần nạp lại code, giờ bị đặt lại | Gọi `rtc.adjust()` vô điều kiện trong `setup()` | Chỉ adjust khi `!rtc.isrunning()` hoặc khi nhận lệnh |
| Giờ lệch vài phút sau vài tuần | DS1307 dùng thạch anh không bù nhiệt | Dùng DS3231, hoặc thỉnh thoảng chỉnh lại |
| Đọc ra 165/165/2165 | Mất kết nối I2C khi đang đọc | Kiểm tra dây, điện trở kéo lên SDA/SCL (module thường có sẵn 4.7kΩ) |
| Lỗi biên dịch `SPI.h: No such file` | RTClib cần Adafruit BusIO, BusIO cần SPI | Thêm `lib_ldf_mode = deep+` (đã có trong `platformio.ini`) |

## Bài tập mở rộng
1. **Dễ:** thêm chế độ **báo lại (snooze)**: nhấn nút khi đang reo → 5 phút sau reo tiếp; giữ nút 2 giây → tắt hẳn.
2. **Vừa:** báo thức chỉ reo **ngày thường T2–T6** (dùng `dayOfTheWeek()`); lưu giờ báo thức vào EEPROM (bài 22) để mất điện không mất.
3. **Khó:** bộ hẹn giờ nhiều mốc: mảng `struct { gio, phut, thoiGian, chan }` (bài 20) bật tắt 2 relay theo lịch; in nhật ký "ngày giờ – sự kiện" ra Serial.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** real time clock, 32.768 kHz crystal, BCD, unix timestamp, epoch, NTP, TCXO, I2C address, data logger, `RTClib`.
- **Datasheet:** **DS1307** (bảng thanh ghi 00h–07h, bit CH dừng đồng hồ, 56 byte RAM) và **DS3231** (độ chính xác ±2ppm, thanh ghi Alarm 1/2). Module "Tiny RTC" còn có EEPROM **AT24C32** ở địa chỉ 0x50.

## Câu hỏi phỏng vấn
- **Vì sao thạch anh đồng hồ là 32.768 kHz?** Gợi ý: 32768 = 2^15, bộ chia 15 tầng flip-flop cho đúng 1 Hz; tần số thấp nên tốn rất ít điện, thạch anh nhỏ.
- **Hai thiết bị I2C trùng địa chỉ thì làm sao?** Gợi ý: nhiều module có chân/jumper đổi địa chỉ (A0–A2); nếu không thì dùng bộ chọn kênh I2C (TCA9548A) hoặc bus I2C thứ hai.
- **Lưu thời gian dạng unix time hay giờ/phút/giây?** Gợi ý: unix time gọn (4 byte), so sánh/cộng trừ dễ; chỉ đổi sang ngày giờ khi hiển thị. Lưu ý múi giờ và sự cố năm 2038 với số 32 bit có dấu.
