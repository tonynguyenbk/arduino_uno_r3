# Bài 1: Nháy LED có sẵn — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **Đèn báo trạng thái** có mặt trên gần như mọi thiết bị điện tử: router wifi (nguồn, mạng, internet), sạc điện thoại, máy giặt, ổ cắm thông minh.
- **Nháy theo mã** để báo lỗi khi thiết bị không có màn hình: điều hòa nháy đèn N lần, xe ô tô đời cũ (chuẩn OBD-I) nháy đèn "Check Engine" theo mã lỗi để thợ đọc.

## Kiến thức của bài trong hệ thống nhúng
- **GPIO output** (chân vào/ra đa năng, ở chế độ xuất): thao tác cơ bản nhất của mọi vi điều khiển. Bài 25 sẽ cho thấy `digitalWrite` thực chất là ghi 1 bit vào thanh ghi `PORTB`.
- **"Hello world" của phần cứng:** khi thiết kế 1 bo mạch mới, việc đầu tiên kỹ sư làm (gọi là *board bring-up*) là nháy được 1 LED. Nháy được nghĩa là chip đã có nguồn, xung nhịp chạy đúng, và nạp code thành công.
- **Heartbeat LED:** nhiều thiết bị công nghiệp nháy 1 LED đều đặn để báo "chương trình vẫn đang chạy". LED đứng im (sáng hoặc tắt) nghĩa là firmware đã treo.
- **`delay()` chặn cả chương trình:** chấp nhận được với bài này, nhưng là lý do phải học `millis()` (bài 5) và sau này là hệ điều hành thời gian thực (RTOS).

## Dự án thực tế
- **Tủ điện, PLC công nghiệp:** hàng đèn trạng thái RUN/STOP/ERROR cho mỗi kênh vào ra.
- **Thiết bị IoT:** nháy nhanh = đang kết nối wifi, sáng đứng = đã kết nối, nháy 3 lần = lỗi cảm biến.
- **Đèn cảnh báo hàng không** trên tòa nhà cao, cột điện: mạch nhúng nháy đúng chu kỳ theo quy chuẩn.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Nạp báo lỗi `not in sync` | Chọn sai cổng COM (máy này COM4 là webcam) | Kiểm tra `upload_port = COM5` |
| Sửa `delay` mà đèn vẫn nháy như cũ | Chưa nạp lại, hoặc đang chọn nhầm env | Xem env ở thanh dưới cùng, bấm Upload lại |
| Vừa cắm USB, đèn L nháy nhanh vài lần | Bootloader đang chờ code mới — bình thường | Không cần sửa |

## Bài tập mở rộng
1. **Dễ:** nháy theo mã Morse SOS (`... --- ...`): chấm 200ms, gạch 600ms.
2. **Vừa:** "heartbeat": nháy nhanh 2 lần (100ms) rồi nghỉ 1 giây, lặp lại.
3. **Khó:** mỗi vòng nháy nhanh dần (giảm `delay` 10%), đến 50ms thì quay lại chậm như ban đầu.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** GPIO, digital output, board bring-up, heartbeat LED, blocking delay.
- **Datasheet ATmega328P:** mục **I/O-Ports**. Chân 13 của Arduino chính là chân **PB5** (cổng B, bit 5) của chip.

## Câu hỏi phỏng vấn
- **Bo mạch mới không chạy, bạn kiểm tra gì đầu tiên?** Gợi ý: đo điện áp nguồn → kiểm tra thạch anh/xung nhịp có dao động → chân reset có bị giữ không → có nạp được không → thử nháy 1 LED.
- **`delay()` có vấn đề gì?** Gợi ý: chặn CPU, trong lúc chờ không phản ứng được với nút bấm hay dữ liệu đến. Thay bằng kiểm tra thời gian (`millis`), ngắt timer, hoặc RTOS.
