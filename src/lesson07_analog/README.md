# Bài 7: Đọc analog — biến trở, quang trở, map(), constrain() — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **Biến trở:** núm âm lượng ampli, tay ga xe máy điện, cần điều khiển (joystick) tay cầm game, cảm biến vị trí chân ga ô tô đời cũ.
- **Trimpot (biến trở nhỏ 103):** chỉnh 1 lần khi sản xuất: độ tương phản LCD, ngưỡng cảm biến, cân chỉnh điện áp bộ nguồn.
- **Quang trở và cảm biến ánh sáng:** đèn đường tự bật khi trời tối, đèn ngủ cảm ứng, điện thoại tự chỉnh độ sáng màn hình.

## Kiến thức của bài trong hệ thống nhúng
- **ADC (chuyển đổi tương tự → số):** cầu nối giữa thế giới thật (nhiệt độ, áp suất, ánh sáng, âm thanh đều là điện áp liên tục) và vi điều khiển. Bài 26 sẽ điều khiển ADC bằng thanh ghi.
- **Mạch chia áp:** cách phổ biến nhất để biến 1 cảm biến điện trở (nhiệt điện trở NTC, quang trở, cảm biến lực) thành điện áp đọc được.
- **Độ phân giải và nhiễu:** 10 bit = 1024 mức, mỗi mức ~4.9mV. Dao động ±1 như log bài 7 là bình thường; thiết bị đo chính xác dùng ADC 16–24 bit và lọc trung bình.
- **Hiệu chuẩn (calibration):** hằng số `A1_KHI_SANG/A1_KHI_TOI` là ví dụ nhỏ của việc hiệu chuẩn — sản phẩm thật lưu hệ số hiệu chuẩn riêng cho từng thiết bị vào EEPROM khi xuất xưởng.

## Dự án thực tế
- **Hệ thống quản lý pin (BMS)** trong xe điện, xe máy điện, pin lưu trữ: đo điện áp từng cell pin bằng ADC liên tục để chống sạc quá/xả quá.
- **Sạc pin điện thoại:** đo nhiệt độ pin bằng nhiệt điện trở NTC qua mạch chia áp, quá nóng thì giảm dòng sạc.
- **Nông nghiệp thông minh:** cảm biến độ ẩm đất (dạng điện trở/điện dung) đọc qua ADC để tự động tưới.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Giá trị nhảy lung tung dù không vặn | Chân analog thả nổi, dây lỏng | Kiểm tra dây chân giữa; chân không dùng không đọc |
| Chỉ lên tới ~512 | Chưa vặn hết cỡ, hoặc chân ngoài nối 3.3V/qua điện trở | Vặn hết; 2 chân ngoài nối thẳng 5V và GND |
| Vặn ngược chiều mong muốn | Đảo 5V và GND ở 2 chân ngoài | Đổi 2 dây ngoài cho nhau, hoặc `map()` ngược |
| Đèn ngủ chập chờn ở ngưỡng tối | Giá trị dao động quanh ngưỡng | Dùng **trễ (hysteresis)**: bật ở 600, tắt ở 550 |

## Bài tập mở rộng
1. **Dễ:** đọc 10 lần lấy trung bình để giảm nhiễu, so log trước/sau.
2. **Vừa:** đèn ngủ có trễ: tối hơn ngưỡng A thì bật, sáng hơn ngưỡng B (B ≠ A) mới tắt.
3. **Khó:** tự hiệu chuẩn: nhấn nút lúc phòng sáng, nhấn lần nữa lúc che tay → chương trình tự nhớ 2 mốc thay cho hằng số.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** ADC, resolution, voltage divider, reference voltage (AREF/AVCC), hysteresis, calibration, moving average.
- **Datasheet ATmega328P:** mục **Analog-to-Digital Converter** (thanh ghi ADMUX, ADCSRA; mỗi lần đo ~13 chu kỳ xung ADC, ~100µs với Arduino).

## Câu hỏi phỏng vấn
- **ADC 10 bit, điện áp tham chiếu 5V, đọc được 300 → bao nhiêu volt?** Gợi ý: 300 × 5 / 1024 ≈ 1.46V.
- **Giảm nhiễu khi đo analog bằng cách nào?** Gợi ý: phần cứng (tụ lọc, tách mass analog/digital, dây ngắn), phần mềm (trung bình, trung vị, lọc thông thấp), cho chip ngủ trong lúc đo (ADC Noise Reduction mode).
