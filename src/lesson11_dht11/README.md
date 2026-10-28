# Bài 11: Cảm biến nhiệt độ, độ ẩm DHT — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **Điều hòa, máy hút ẩm, máy tạo ẩm, máy lọc không khí.**
- **Nhà kính, trang trại nấm, chuồng trại:** tự bật quạt, phun sương khi nóng/khô.
- **Trạm thời tiết mini, thiết bị nhà thông minh** (đo nhiệt độ ẩm từng phòng).
- **Kho lạnh, xe chở vaccine, thực phẩm (chuỗi cung ứng lạnh):** thiết bị ghi dữ liệu nhiệt độ suốt hành trình, vượt ngưỡng là có bằng chứng.

## Kiến thức của bài trong hệ thống nhúng
- **Dùng thư viện / driver:** kỹ sư nhúng hiếm khi tự viết giao thức cho mọi linh kiện, mà biết **tìm, đánh giá, và tích hợp** thư viện: kiểm tra giấy phép (license), RAM/Flash tốn bao nhiêu, có chặn CPU không, còn được bảo trì không.
- **Giao thức 1 dây tự định nghĩa:** DHT gửi 40 bit dữ liệu bằng độ dài xung HIGH (khoảng 26µs = bit 0, 70µs = bit 1) — mở `DHT.cpp` sẽ thấy. Có kèm **checksum** (byte kiểm tra) để phát hiện dữ liệu hỏng — mọi giao thức truyền tin đều có cơ chế này.
- **Xử lý lỗi:** `isnan()` — firmware thật không bao giờ tin dữ liệu cảm biến tuyệt đối: kiểm tra lỗi, đọc lại, báo lỗi cảm biến hỏng thay vì điều khiển theo số sai.
- **DHT11 so với cảm biến công nghiệp:** DHT11 rẻ nhưng sai số ±2°C, ±5% độ ẩm. Sản phẩm thương mại dùng SHT3x, BME280 (giao tiếp I2C — bài 12), chính xác và có hiệu chuẩn sẵn từ nhà máy.

## Dự án thực tế
- **Phòng máy chủ (data center):** hàng trăm cảm biến nhiệt độ ẩm, vượt ngưỡng là gửi cảnh báo.
- **IoT nông nghiệp:** cảm biến chạy pin gửi dữ liệu qua LoRa/NB-IoT về máy chủ mỗi 15 phút, chạy được 1–2 năm nhờ chế độ ngủ tiết kiệm điện (bài 23).

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Luôn in "Loi doc cam bien" | Sai loại cảm biến trong code | Board thật dùng DHT11: `LOAI_CAM_BIEN = DHT11` |
| Lỗi đọc thất thường | Cảm biến rời 4 chân thiếu điện trở kéo lên | Thêm 10kΩ giữa DATA và 5V (module 3 chân đã có sẵn) |
| Số không đổi dù thổi hơi vào | Đọc nhanh hơn 1 lần/giây → nhận lại số cũ | Đọc cách nhau ≥ 1–2 giây |
| Build lỗi `Adafruit_Sensor.h: No such file` | Thiếu thư viện phụ | Thêm `Adafruit Unified Sensor` vào `lib_deps` |

## Bài tập mở rộng
1. **Dễ:** ghi nhớ và in nhiệt độ thấp nhất, cao nhất từ lúc khởi động.
2. **Vừa:** bật LED (giả làm quạt) khi trên 30°C, tắt khi dưới 28°C (có trễ, giống điều hòa thật).
3. **Khó:** hiển thị nhiệt độ, độ ẩm lên LCD/OLED (kết hợp bài 12).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** single-wire protocol, checksum, library dependency, sensor accuracy, thermostat hysteresis.
- **Datasheet:** đọc **datasheet DHT11** (không phải ATmega): sơ đồ thời gian (timing diagram) gói 40 bit, so với code trong `DHT.cpp`.

## Câu hỏi phỏng vấn
- **Chọn thư viện bên thứ ba cho sản phẩm cần cân nhắc gì?** Gợi ý: giấy phép (MIT/BSD hay GPL), dung lượng Flash/RAM, có chặn CPU/tắt ngắt không, còn được bảo trì, có xử lý lỗi không.
- **Checksum là gì? Khác CRC thế nào?** Gợi ý: checksum = cộng các byte, phát hiện lỗi đơn giản; CRC dựa trên phép chia đa thức, phát hiện được nhiều kiểu lỗi hơn (lỗi chùm, đảo bit).
