# Bài 20: Remote hồng ngoại — ứng dụng thực tế

## Remote gửi tín hiệu thế nào (giao thức NEC)
```
LED hồng ngoại nhấp nháy 38kHz ──► mắt thu lọc 38kHz ──► chân S: xung dữ liệu sạch

 ┌── 9ms ──┐ 4.5ms ┌ 8 bit ┐┌ 8 bit ┐┌ 8 bit ┐┌ 8 bit ┐
 ▀▀▀▀▀▀▀▀▀▀         địa chỉ  địa chỉ   LỆNH    lệnh
   mở đầu                    (đảo)             (đảo)   ← phần "đảo" để kiểm tra lỗi
 bit 0: xung 0.56ms + nghỉ 0.56ms      bit 1: xung 0.56ms + nghỉ 1.69ms
 Giữ nút: chỉ gửi "mã lặp lại" ngắn mỗi 110ms
```

## Phần cứng này dùng ở đâu
- **Remote TV, điều hòa, quạt, đầu thu, loa, đèn LED trang trí** — vẫn là cách điều khiển rẻ và phổ biến nhất.
- **Điều hòa:** remote gửi **toàn bộ trạng thái** (nhiệt độ, chế độ, tốc độ quạt...) trong 1 bản tin dài, không chỉ mã nút — vì điều hòa không báo ngược lại cho remote.
- **Cảm biến hồng ngoại khác cùng họ:** cảm biến vật cản (robot dò line), cảm biến chuyển động PIR (bật đèn khi có người), đo nhiệt độ không chạm (súng đo nhiệt), chuột quang.

## Kiến thức của bài trong hệ thống nhúng
- **Điều chế sóng mang (carrier modulation):** nhấp nháy ở 38kHz để mắt thu phân biệt với ánh sáng mặt trời, bóng đèn. Cùng ý tưởng với radio AM/FM, wifi — tín hiệu "cưỡi" trên sóng mang.
- **Mã hóa bằng độ dài xung (pulse distance coding):** giống DHT (bài 11), servo (bài 13). Thư viện đo thời gian giữa các cạnh tín hiệu bằng timer — bài 24 (ngắt) và 25 (timer) sẽ giải thích cơ chế.
- **Phát hiện lỗi đơn giản:** gửi kèm bản đảo của địa chỉ và lệnh — nếu 2 phần không khớp thì bỏ bản tin.
- **Bảng tra dữ liệu (`struct` + mảng):** ánh xạ mã → hành động nằm trong bảng, đổi remote chỉ đổi bảng, không sửa logic. Kỹ thuật "table-driven design" rất phổ biến trong firmware.
- **Xử lý giữ nút (auto-repeat):** phân biệt nhấn mới với mã lặp lại → nút +/− giữ thì tăng liên tục, nút số thì không bị nhân đôi.

## Dự án thực tế
- **Điều khiển điều hòa thông minh (IR blaster):** thiết bị wifi học mã remote, rồi phát lại để bật điều hòa từ điện thoại — sản phẩm thương mại rất phổ biến (Broadlink, Xiaomi...). Dùng ESP32 + LED hồng ngoại + thư viện IRremote.
- **Hệ thống nhà thông minh:** gom tất cả remote trong nhà vào 1 ứng dụng.
- **Đếm sản phẩm trên băng chuyền, cổng an ninh:** cặp phát–thu hồng ngoại, vật đi qua chắn tia → đếm/cảnh báo.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Đèn L không nháy khi bấm remote | Mắt thu chưa có nguồn / sai chân, remote hết pin | Kiểm tra 3 dây; nhìn LED remote qua camera điện thoại (thấy tím nhấp nháy là remote còn chạy) |
| Nhận mã nhưng LED không phản ứng | Bảng mã không khớp remote | Đặt `REMOTE_WOKWI` đúng; nút lạ thì đọc mã trên Serial và thêm vào bảng |
| Nhận "tín hiệu lạ" liên tục | Nhiễu từ đèn huỳnh quang, nắng chiếu thẳng | Tránh nắng trực tiếp; thêm tụ 10µF giữa + và − của mắt thu |
| `tone()` hoặc PWM chân 3/11 hết chạy | IRremote dùng Timer2 | Dùng chân PWM khác (5, 6, 9, 10) |
| Bấm 1 lần nhảy 2 mức | Xử lý cả mã lặp lại cho nút số | Bỏ qua mã lặp với các nút không cần giữ (đã làm trong code) |

## Bài tập mở rộng
1. **Dễ:** thêm nút điều khiển servo (bài 13): `|<<` / `>>|` xoay trái/phải 10°.
2. **Vừa:** "đèn RGB điều khiển từ xa": 3 LED đỏ/xanh lá/xanh dương, nút 1/2/3 chọn màu, +/− chỉnh độ sáng màu đó.
3. **Khó:** chế độ **học mã**: giữ nút trên board 2 giây → bấm 1 nút bất kỳ trên remote → lưu mã đó làm nút bật/tắt vào EEPROM (không mất khi tắt điện).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** infrared remote, NEC protocol, 38kHz carrier, IR receiver VS1838B / TSOP38238, pulse distance encoding, IR blaster, table-driven design.
- **Datasheet:** **VS1838B / TSOP382xx** (mắt thu: tần số sóng mang, góc thu, mạch lọc nguồn đề nghị). Tài liệu giao thức NEC (định thời từng bit).

## Câu hỏi phỏng vấn
- **Tại sao remote hồng ngoại dùng sóng mang 38kHz thay vì bật LED liên tục?** Gợi ý: phân biệt với ánh sáng môi trường (mặt trời, đèn) vốn gần như không đổi; mắt thu có bộ lọc thông dải chỉ nhận 38kHz; LED chỉ bật một phần thời gian → tiết kiệm pin, cho dòng xung lớn → phát xa hơn.
- **Giải mã một giao thức dựa trên độ dài xung, dùng polling hay ngắt?** Gợi ý: ngắt khi đổi cạnh + đọc timer để lấy thời điểm, lưu vào bộ đệm, giải mã ở vòng lặp chính — không chặn CPU và không lỡ xung.
