# Bài 17: Chuông (buzzer) — ứng dụng thực tế

## Chuông chủ động hay thụ động?
| | Chủ động (active) | Thụ động (passive) |
|---|---|---|
| Nhìn mặt dưới | Bịt kín nhựa đen, thường có tem dán | Thấy bảng mạch xanh |
| Bên trong | Có sẵn mạch dao động | Chỉ là loa nhỏ (màng rung) |
| Cách làm kêu | `digitalWrite(chan, HIGH)` | `tone(chan, tanSo)` |
| Đổi được nốt nhạc? | Không, 1 tiếng bíp cố định | Có |

## Phần cứng này dùng ở đâu
- **Tiếng bíp xác nhận:** lò vi sóng, máy giặt, điều hòa (bíp khi nhận lệnh remote), máy tính tiền, cửa từ.
- **Cảnh báo:** báo khói, báo cháy, báo rò rỉ gas, báo trộm, cảm biến lùi xe (bài 10), thiết bị y tế (máy đo nhịp tim, máy thở báo động).
- **Âm báo có nhạc:** chuông cửa, đồ chơi trẻ em, thiệp nhạc.

## Kiến thức của bài trong hệ thống nhúng
- **Tạo sóng vuông bằng Timer:** `tone()` cấu hình Timer2 để tự đảo chân theo tần số mong muốn — CPU rảnh. Bài 27 sẽ tự làm bằng thanh ghi. Đây cũng là lý do `tone()` làm hỏng PWM chân 3, 11 (cùng dùng Timer2) — xung đột tài nguyên phần cứng giống thư viện Servo ở bài 13.
- **Dữ liệu tách khỏi code:** giai điệu nằm trong 2 mảng (`NOT_NHAC`, `TRUONG_DO`), muốn đổi bài chỉ sửa dữ liệu. `sizeof(mang)/sizeof(mang[0])` để tự đếm số phần tử — không phải đếm tay.
- **Không chặn CPU:** mỗi chế độ chỉ kiểm tra "đã đến lúc đổi nốt chưa" (như bài 5) → nút luôn phản hồi ngay dù đang phát nhạc.
- **Âm thanh trong nhúng:** sản phẩm cao cấp hơn dùng **PWM + bộ lọc** hoặc chip DAC/I2S để phát âm thanh thật (giọng nói, nhạc), ví dụ thang máy đọc "Tầng 5", máy ATM hướng dẫn bằng giọng nói.

## Dự án thực tế
- **Báo cháy theo tiêu chuẩn:** âm báo động có mẫu nhịp quy định (ví dụ mẫu "Temporal-3": bíp bíp bíp – nghỉ) để mọi người nhận ra ngay là báo cháy.
- **Thiết bị y tế:** tiêu chuẩn IEC 60601-1-8 quy định âm báo động theo mức độ ưu tiên (cao/trung bình/thấp) có giai điệu khác nhau.
- **Còi xe cứu thương, cứu hỏa:** tần số trượt lên xuống như chế độ "còi hú" của bài này (hiệu ứng wail/yelp).

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Chỉ kêu bíp đều, không ra giai điệu | Đang dùng chuông **chủ động** | Dùng chuông thụ động (đáy thấy bảng mạch) |
| Không kêu gì | Cắm ngược chân (+)/(−) với chuông chủ động | Chân dài / có dấu `+` vào chân 8 |
| Kêu rất nhỏ | Chuông thụ động kêu to nhất ở tần số cộng hưởng (~2–4kHz) | Thử tần số cao hơn; hoặc thêm transistor để tăng dòng |
| `analogWrite` chân 3, 11 không chạy khi đang kêu | `tone()` chiếm Timer2 | Dùng chân PWM khác (5, 6, 9, 10) |
| 2 nốt giống nhau liền nhau nghe dính thành 1 | Không có khoảng lặng giữa các nốt | Kêu 90% thời lượng nốt (đã làm trong code) |

## Bài tập mở rộng
1. **Dễ:** thêm 1 bài nhạc của riêng bạn (2 mảng mới) và 1 chế độ phát bài đó.
2. **Vừa:** kết hợp bài 10: còi lùi xe bíp nhanh dần khi lại gần vật cản.
3. **Khó:** "đàn piano" bằng Serial: gõ phím `a s d f g h j` trên máy tính thì chuông phát nốt Đô Rê Mi Fa Sol La Si.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** active buzzer, passive buzzer, piezo, square wave, frequency, musical note frequency, resonant frequency, Temporal-3 alarm pattern.
- **Datasheet ATmega328P:** mục **8-bit Timer/Counter2 with PWM → Clear Timer on Compare Match (CTC) Mode** — cách `tone()` tạo tần số.

## Câu hỏi phỏng vấn
- **Tạo sóng vuông 1kHz trên vi điều khiển có những cách nào?** Gợi ý: vòng lặp + delay (chặn CPU, không chính xác), ngắt timer đảo chân, chế độ CTC/PWM của timer tự đảo chân (chính xác, không tốn CPU).
- **Tại sao chuông piezo kêu to nhất ở một tần số nhất định?** Gợi ý: màng rung có tần số cộng hưởng; chuông báo động thường thiết kế ở ~2.7–4kHz, vùng tai người nhạy nhất.
