# Bài 27: Cảm biến chuyển động PIR — ứng dụng thực tế

## PIR hoạt động thế nào
- **PIR = Passive InfraRed:** không phát ra gì, chỉ "nghe" bức xạ hồng ngoại (nhiệt) mà cơ thể người tỏa ra.
- Bên trong có **2 nửa cảm biến**. Người đi ngang → 2 nửa nhận nhiệt lệch nhau → mạch báo **có chuyển động**. Người đứng yên hoàn toàn thì không báo (đèn tự tắt khi ngồi yên trong toilet là vì vậy).
- **Vòm trắng (thấu kính Fresnel)** chia vùng nhìn thành nhiều ô → người đi qua các ô tạo tín hiệu rõ hơn, nhìn xa hơn (3–7m).
- **Module HC-SR501:** chân OUT = HIGH khi có chuyển động. 2 núm vặn: **Tx** thời gian giữ HIGH (3s–5 phút), **Sx** độ nhạy. Jumper **H** (gặp chuyển động tiếp thì kéo dài) / **L** (báo 1 lần). Cần 30–60 giây "làm quen" sau khi cấp điện.

## Phần cứng này dùng ở đâu
- **Đèn tự động:** hành lang, cầu thang, nhà vệ sinh công cộng, bãi đỗ xe, đèn sân vườn năng lượng mặt trời.
- **Báo trộm, camera an ninh:** chỉ quay/ghi khi có chuyển động → tiết kiệm bộ nhớ và pin.
- **Cửa tự động** siêu thị (thường kết hợp radar vi sóng), **máy lạnh/TV tự tắt khi không có người**, đếm người ra vào.

## Kiến thức của bài trong hệ thống nhúng
- **Hướng sự kiện bằng ngắt:** ISR chỉ **bật cờ** (`coChuyenDong = true`), `loop()` thấy cờ mới xử lý — mẫu thiết kế "ISR đặt cờ, vòng lặp chính xử lý" (deferred processing) dùng khắp nơi trong firmware.
- **Hẹn giờ có kéo dài (retriggerable timer):** mỗi lần có chuyển động, tính lại 10 giây từ đầu — giống watchdog (bài 41): "còn tín hiệu thì còn sống".
- **Đánh thức từ chế độ ngủ:** thiết bị chạy pin cho vi điều khiển **ngủ sâu** (vài µA), chân PIR nối ngắt ngoài để **đánh thức** khi có người → pin dùng được hàng năm (bài 41).
- **Cảm biến cần thời gian ổn định:** bỏ qua tín hiệu trong giai đoạn khởi động — mọi cảm biến thật đều có đặc tính này (cảm biến khí bài 19 cần làm nóng, PIR cần làm quen).
- **Báo nhầm (false alarm):** gió nóng, nắng chiếu, thú nuôi → hệ thống thật kết hợp nhiều cảm biến (PIR + vi sóng = "dual-tech"), hoặc yêu cầu 2 lần phát hiện liên tiếp.

## Dự án thực tế
- **Hệ thống chiếu sáng thông minh tòa nhà:** cảm biến chiếm chỗ (occupancy sensor) ở trần mỗi phòng điều khiển đèn và điều hòa — tiết kiệm điện 30–50%.
- **Camera an ninh chạy pin (Ring, Arlo...):** PIR đánh thức chip xử lý ảnh, quay vài giây rồi ngủ lại.
- **Hệ thống báo trộm nhà:** PIR + cảm biến cửa (bài 4) + bàn phím mã (bài 23) + còi (bài 17) + gửi SMS.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Mới cắm điện đã báo liên tục | Đang trong thời gian làm quen 30–60s | Chờ 1 phút; hoặc thêm vào code: bỏ qua mọi tín hiệu trong 60 giây đầu sau `setup()` |
| Báo 1 lần rồi lâu sau mới báo lại | Núm Tx vặn thời gian giữ HIGH quá dài | Vặn Tx ngược chiều kim đồng hồ (ngắn nhất ~3s) |
| Không phát hiện ở xa | Độ nhạy Sx thấp, hoặc đi thẳng về phía cảm biến | Tăng Sx; PIR nhạy nhất khi đi **ngang qua** vùng nhìn |
| Báo nhầm khi không có ai | Nắng chiếu, gió nóng từ điều hòa/quạt sưởi, nguồn nhiễu | Đổi hướng lắp; dây nguồn ổn định; lọc bằng 2 lần phát hiện |
| Ngắt không được gọi | Nối OUT vào chân không có ngắt ngoài | Uno chỉ có ngắt ngoài ở chân 2, 3 (chân khác dùng pin-change) |

## Bài tập mở rộng
1. **Dễ:** kết hợp quang trở (bài 7): đèn hành lang chỉ bật khi **trời tối**.
2. **Vừa:** chế độ báo trộm có **thời gian trễ**: kích hoạt xong chờ 10 giây mới canh (để chủ nhà kịp ra khỏi cửa); có chuyển động thì cho 10 giây nhập mã (bài 23) trước khi hú còi.
3. **Khó:** cho Arduino **ngủ** (`LowPower` / `sleep_mode()`), PIR đánh thức qua ngắt chân 2; đo và so sánh dòng điện khi thức/ngủ (chuẩn bị cho bài 41).

## Từ khóa và datasheet
- **Từ khóa tra cứu:** passive infrared sensor, pyroelectric, Fresnel lens, occupancy sensor, retriggerable, deferred interrupt processing, wake-up from sleep, dual-tech sensor.
- **Datasheet:** **HC-SR501** (núm chỉnh, jumper, góc nhìn 110°, thời gian khởi động) và chip xử lý **BISS0001**. ATmega328P mục **External Interrupts** và **Power Management and Sleep Modes**.

## Câu hỏi phỏng vấn
- **Vì sao ISR chỉ nên đặt cờ thay vì xử lý luôn?** Gợi ý: ISR chạy lâu sẽ chặn các ngắt khác, làm lệch `millis()`, mất dữ liệu UART; các hàm như `Serial.print` không an toàn trong ISR.
- **Thiết kế thiết bị báo động chạy pin 1 năm, dùng PIR — ý tưởng chính?** Gợi ý: vi điều khiển ngủ sâu gần như toàn thời gian, chỉ thức khi PIR kích ngắt; tắt nguồn ngoại vi không dùng; truyền tin không dây bằng giao thức tiết kiệm năng lượng (LoRa, BLE).
