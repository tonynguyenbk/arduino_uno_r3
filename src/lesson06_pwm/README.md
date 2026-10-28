# Bài 6: PWM, làm mờ LED, hiệu chỉnh gamma — ứng dụng thực tế

## Phần cứng/kỹ thuật này dùng ở đâu
PWM (điều chế độ rộng xung) có lẽ là kỹ thuật được dùng nhiều nhất trong điện tử công suất:
- **Chỉnh độ sáng:** đèn LED dimmer, đèn nền màn hình điện thoại, laptop, TV.
- **Chỉnh tốc độ động cơ:** quạt máy tính, quạt trần inverter, drone (bài 15).
- **Bộ nguồn xung:** sạc điện thoại, bộ nguồn máy tính đóng ngắt hàng trăm nghìn lần/giây và chỉnh độ rộng xung để giữ điện áp ra ổn định.
- **Biến tần (inverter):** điều hòa inverter, xe điện, inverter điện mặt trời tạo ra dòng xoay chiều từ PWM tần số cao.
- **Âm thanh:** ampli class-D trong loa bluetooth, tai nghe.

## Kiến thức của bài trong hệ thống nhúng
- **PWM bằng phần cứng Timer:** chip tự tạo xung, CPU không tốn công — bài 37 sẽ tự cấu hình thanh ghi Timer để tạo PWM.
- **Hiệu chỉnh gamma:** mắt người cảm nhận ánh sáng không tuyến tính. Màn hình TV, điện thoại, bảng LED đều có bảng gamma. Bài 8 đã tối ưu bằng bảng tra.
- **Tần số PWM quan trọng:** quá thấp thì đèn nhấp nháy (gây mỏi mắt — các hãng điện thoại quảng cáo "chống nhấp nháy"), động cơ kêu rè rè ở dải tai nghe được.

## Dự án thực tế
- **Đèn đường LED thông minh:** giảm độ sáng sau nửa đêm bằng PWM để tiết kiệm điện.
- **Ô tô:** đèn hậu, điều khiển bơm nhiên liệu, quạt két nước đều dùng PWM.
- **Lò nhiệt công nghiệp:** bật tắt relay bán dẫn (SSR) theo tỉ lệ thời gian — PWM tần số rất thấp (vài giây/chu kỳ) để giữ nhiệt độ.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| LED chỉ bật/tắt, không mờ được | Chân không có PWM (ví dụ chân 8) | Dùng chân có dấu `~`: 3, 5, 6, 9, 10, 11 |
| Đang dùng servo/`tone()` thì PWM chân khác hỏng | Các tính năng dùng chung timer | Xem bảng chân–timer (bài 37), chọn chân khác |
| `analogWrite(chan, 300)` sáng sai | Giá trị chỉ được 0–255 | Dùng `constrain()` hoặc `map()` trước |
| Sáng dần nhưng mắt thấy "vọt" ở đầu | Mắt cảm nhận phi tuyến | Hiệu chỉnh gamma |

## Bài tập mở rộng
1. **Dễ:** 2 nút chỉnh độ sáng tăng/giảm từng nấc 10%.
2. **Vừa:** LED RGB (3 chân PWM) đổi màu dần qua cầu vồng.
3. **Khó:** LED "thở" theo đường sin dùng bảng tra 64 phần tử (kết hợp bài 8).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** PWM, duty cycle, PWM frequency, gamma correction, output compare.
- **Datasheet ATmega328P:** mục **8-bit Timer/Counter0 with PWM → Modes of Operation (Fast PWM, Phase Correct PWM)**. Arduino mặc định: chân 5, 6 ~980Hz; chân 3, 9, 10, 11 ~490Hz.

## Câu hỏi phỏng vấn
- **Cấp PWM 50% cho động cơ 12V có giống cấp 6V không?** Gợi ý: về trung bình gần giống, vì cuộn dây và quán tính "làm phẳng" xung. Nhưng dòng thực tế là xung, nên vẫn cần chọn tần số và linh kiện phù hợp.
- **Chọn tần số PWM cho động cơ thế nào?** Gợi ý: trên ~20kHz để tai không nghe tiếng rít, nhưng không quá cao vì tổn hao đóng ngắt trên MOSFET tăng theo tần số.
