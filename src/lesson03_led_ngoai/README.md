# Bài 3: LED ngoài, breadboard, điện trở — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **Breadboard:** dùng để thử mạch nhanh (prototype) trước khi đặt làm bo mạch in (PCB). Mọi sản phẩm điện tử đều bắt đầu từ mạch thử kiểu này.
- **Điện trở hạn dòng cho LED:** có trong mọi thiết bị có đèn báo. Với đèn LED công suất lớn (đèn đường, đèn pha ô tô) người ta dùng **mạch driver dòng không đổi** thay vì điện trở để đỡ tốn điện.

## Kiến thức của bài trong hệ thống nhúng
- **Định luật Ohm + giới hạn dòng của chân chip:** datasheet ATmega328P ghi mỗi chân tối đa 40mA, tổng cả chip khoảng 200mA. Kỹ sư phần cứng phải lập **bảng ngân sách dòng (power budget)** cho cả bo mạch: mỗi LED, cảm biến tốn bao nhiêu, nguồn có đủ không.
- **Chân vi điều khiển không phải nguồn điện:** muốn bật tải lớn (relay, động cơ, dải LED) phải qua transistor/MOSFET/driver — kiến thức dùng lại ở bài 14, 15.
- **`if/else`, `for`:** logic điều khiển có điều kiện và lặp là nền của mọi firmware.

## Dự án thực tế
- **Đèn giao thông:** bộ điều khiển nhúng bật tắt các nhóm đèn theo kịch bản thời gian (sẽ gặp lại ở bài 16 — máy trạng thái).
- **Bảng LED quảng cáo, đèn trang trí:** hàng nghìn LED điều khiển bằng vi điều khiển + IC driver.
- **Thiết bị y tế, công nghiệp:** đèn cảnh báo màu theo chuẩn (đỏ = lỗi nguy hiểm, vàng = cảnh báo, xanh = bình thường).

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| LED không sáng | Cắm ngược chiều LED | Chân **dài** (+) về phía chân Arduino, chân ngắn về GND |
| LED không sáng | 2 chân LED cắm cùng 1 cột (bị nối tắt) | Mỗi chân 1 cột khác nhau |
| LED rất mờ | Nhầm điện trở 22kΩ/220kΩ thay vì 220Ω | Đọc lại vạch màu: đỏ–đỏ–**nâu** = 220Ω |
| LED cháy, chân Arduino nóng | Không có điện trở hạn dòng | Luôn mắc điện trở nối tiếp với LED |

## Bài tập mở rộng
1. **Dễ:** đèn giao thông 3 LED: xanh 5s → vàng 2s → đỏ 5s.
2. **Vừa:** thay điện trở 330Ω, 1kΩ: tính dòng điện theo định luật Ohm rồi so độ sáng.
3. **Khó:** "đèn chạy" 4 LED sáng lần lượt qua lại bằng vòng `for`.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** Ohm's law, current limiting resistor, LED forward voltage, sink/source current.
- **Datasheet ATmega328P:** mục **Electrical Characteristics → Absolute Maximum Ratings** (tối đa 40mA mỗi chân) và **DC Characteristics** (điện áp ra mức HIGH/LOW khi có tải).

## Câu hỏi phỏng vấn
- **Tính điện trở cho LED đỏ (sụt áp 2V, dòng 10mA) từ nguồn 5V?** Gợi ý: (5 − 2) / 0.01 = 300Ω → chọn giá trị chuẩn gần nhất 330Ω.
- **Muốn bật relay 12V từ chân 5V của vi điều khiển thì làm sao?** Gợi ý: dùng transistor/MOSFET làm công tắc, thêm diode chống điện áp ngược song song cuộn relay. Chân chip chỉ chịu vài chục mA.
