# Bài 14: Động cơ bước 28BYJ-48 + ULN2003 — ứng dụng thực tế

## Phần cứng này dùng ở đâu
- **28BYJ-48 cụ thể:** rất phổ biến trong **cánh đảo gió điều hòa**, van tiết lưu điện tử, rèm cửa tự động, máy cho cá ăn — nơi cần quay chậm, chính xác, rẻ.
- **Động cơ bước nói chung:** **máy in 3D, máy CNC, máy khắc laser** (mỗi trục 1 động cơ bước), máy in giấy, máy scan, ổ đĩa quang, **lấy nét ống kính máy ảnh**, kim đồng hồ trên táp-lô ô tô.
- **ULN2003:** IC "công tắc dòng lớn" 7 kênh, dùng để bật relay, van điện từ, đèn, còi từ chân vi điều khiển.

## Kiến thức của bài trong hệ thống nhúng
- **Điều khiển vòng hở (open-loop):** biết vị trí chỉ bằng cách đếm bước, không cần cảm biến — rẻ, đơn giản. Rủi ro: nếu tải quá nặng làm **mất bước**, firmware không hề biết. Vì vậy máy in 3D cần công tắc hành trình (bài 4) để về gốc mỗi lần khởi động.
- **Driver và khuếch đại dòng:** chân vi điều khiển chỉ ra lệnh, không cấp công suất — nguyên tắc chung cho mọi tải lớn.
- **Vi bước (microstepping):** driver hiện đại (A4988, TMC2209) chia 1 bước thành 16–256 bước nhỏ bằng cách chỉnh dòng từng cuộn theo hình sin (lại là bảng tra sin + PWM, bài 6, 8) → chạy êm, chính xác hơn. Máy in 3D "chạy êm" là nhờ driver TMC.
- **Bộ đếm kiểu `long`:** máy CNC đếm hàng triệu bước — chọn sai kiểu dữ liệu là tràn số.

## Dự án thực tế
- **Bơm tiêm điện, bơm truyền dịch** trong bệnh viện: động cơ bước đẩy pít-tông với lưu lượng chính xác từng micro-lít.
- **Máy ATM, máy bán hàng tự động:** cơ cấu nhả tiền, nhả hàng.
- **Máy gắp linh kiện (pick-and-place)** trong nhà máy lắp ráp điện tử.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Động cơ rung tại chỗ, không quay | Sai thứ tự chân | `Stepper(BUOC, IN1, IN3, IN2, IN4)` = `(8, 10, 9, 11)` |
| Vặn nhanh thì động cơ đứng, kêu ù | Vượt tốc độ tối đa (28BYJ-48 ~15 vòng/phút) | `VONG_PHUT_TOI_DA = 15`, `BUOC_MOI_VONG = 2048` |
| 4 đèn LED trên mạch ULN2003 không sáng | Thiếu nguồn cho mạch ULN2003 | Nối chân `+` và `−` của mạch vào 5V, GND |
| Động cơ nóng khi đứng yên | Cuộn dây vẫn được cấp điện để giữ vị trí | Tắt 4 chân khi không cần giữ (bài tập 3) |

## Bài tập mở rộng
1. **Dễ:** mỗi lần nhấn nút, quay đúng 90° (2048 / 4 = 512 bước).
2. **Vừa:** tự viết hàm đi 1 bước bằng mảng 4 trạng thái cuộn dây + `millis()`, không dùng thư viện (không chặn CPU).
3. **Khó:** chế độ nửa bước (half-step, 8 trạng thái) → 4096 bước/vòng, chạy êm hơn; và tắt cuộn dây khi đứng yên.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** stepper motor, full-step, half-step, microstepping, open-loop, missed steps, holding torque, Darlington array.
- **Datasheet:** **ULN2003** (mỗi kênh chịu 500mA, có diode chống ngược sẵn) và thông số **28BYJ-48** (5V, tỉ số truyền ~64:1).

## Câu hỏi phỏng vấn
- **"Mất bước" là gì? Phát hiện và phòng tránh thế nào?** Gợi ý: tải quá nặng/tăng tốc quá nhanh làm trục không theo kịp lệnh. Phòng: ramp tăng tốc, đủ dòng; phát hiện: encoder, công tắc gốc, driver có tính năng phát hiện kẹt (StallGuard của TMC).
- **Khi nào chọn động cơ bước, servo, DC?** Gợi ý: bước = định vị chính xác giá rẻ, tốc độ thấp; servo = vòng kín, phản hồi nhanh; DC/BLDC = tốc độ cao, công suất lớn, cần thêm cảm biến nếu muốn định vị.
