# Bài 19: Cảm biến nước — ứng dụng thực tế

## Cảm biến hoạt động thế nào
Bảng mạch đỏ có các **vạch đồng song song**, xen kẽ vạch nối nguồn và vạch nối chân S. Nước (có lẫn khoáng chất) dẫn điện yếu → nối các vạch lại. Ngập càng sâu, càng nhiều vạch được nối → điện áp ở chân S càng cao. Thực chất lại là **mạch chia áp** của bài 7, với "biến trở" là nước.

## Phần cứng này dùng ở đâu
- **Báo rò rỉ nước** dưới máy giặt, bình nóng lạnh, bồn rửa, phòng máy chủ — nhà thông minh gửi cảnh báo về điện thoại.
- **Báo ngập** tầng hầm, gara, hố thang máy, trạm bơm.
- **Cảm biến mưa** (cùng nguyên lý, bảng mạch dạng răng lược): tự đóng mái che, tắt tưới cây khi trời mưa.
- **Máy pha cà phê, máy tạo ẩm, bể cá:** báo hết nước.

## Kiến thức của bài trong hệ thống nhúng
- **Cấp nguồn theo nhu cầu (power gating):** chỉ bật cảm biến lúc đo — vừa chống ăn mòn, vừa **tiết kiệm điện** cho thiết bị chạy pin (bài 31). Dùng chân GPIO cấp nguồn được vì cảm biến chỉ cần vài mA; tải lớn hơn thì dùng transistor/MOSFET.
- **Độ trễ (hysteresis):** ngưỡng lên khác ngưỡng xuống → tránh trạng thái "nhảy qua lại" khi giá trị dao động quanh ngưỡng. Có ở bộ điều nhiệt điều hòa, tủ lạnh, phao bơm nước, cổng Schmitt trigger (bài 4).
- **Lượng tử hóa thành các mức:** giá trị liên tục 0–1023 → 4 mức rời rạc dễ hiểu cho người dùng.
- **Cảm biến công nghiệp tốt hơn:** cảm biến điện dung (không chạm nước, không ăn mòn), phao từ, cảm biến áp suất thủy tĩnh, siêu âm (bài 10), radar.

## Dự án thực tế
- **Van chặn nước thông minh:** cảm biến dưới sàn phát hiện rò rỉ → vi điều khiển đóng van tổng bằng động cơ (bài 15) + gửi thông báo.
- **Trạm quan trắc mực nước sông, hồ chứa, đô thị chống ngập:** đo mực nước định kỳ, gửi về trung tâm qua GSM/LoRa; vượt ngưỡng thì bật còi, đèn cảnh báo.
- **Điều khiển bơm tự động** cho bể chứa nước tòa nhà: mức thấp bật bơm, mức cao tắt bơm — chính là hysteresis 2 ngưỡng.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Sau vài tuần đo sai, vạch đồng xỉn màu/ăn mòn | Cấp điện liên tục → điện phân | Cấp nguồn từ chân GPIO, chỉ bật lúc đo (code bài này) |
| Ngập sâu vẫn chỉ đọc ~500–600 | Đặc tính cảm biến + độ dẫn của nước | Đo thử thực tế rồi chỉnh mảng `NGUONG[]` |
| Đèn nhấp nháy đổi mức liên tục | Giá trị dao động quanh ngưỡng | Tăng `TRE` |
| Luôn đọc 0 | Quên cấp nguồn (chân 7) trước khi đọc, hoặc chờ quá ngắn | Bật chân nguồn, `delay(10)` rồi mới `analogRead` |
| Ngập phần linh kiện phía trên → hỏng | Nhúng quá vạch giới hạn | Chỉ cho nước chạm vùng vạch đồng |

## Bài tập mở rộng
1. **Dễ:** thêm chuông (bài 17) kêu báo động khi lên mức 3.
2. **Vừa:** điều khiển "máy bơm" (LED hoặc động cơ bài 15): bật khi mức ≤ 1, tắt khi mức 3 — kiểu phao bơm 2 ngưỡng.
3. **Khó:** đo 5 lần liên tiếp (bật nguồn 1 lần), bỏ giá trị lớn nhất và nhỏ nhất, lấy trung bình 3 giá trị còn lại.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** water level sensor, electrolysis, corrosion, power gating, hysteresis, capacitive level sensor, leak detection.
- **Datasheet ATmega328P:** mục **Electrical Characteristics → DC Characteristics** (chân I/O cấp được bao nhiêu dòng khi dùng làm nguồn cho cảm biến).

## Câu hỏi phỏng vấn
- **Vì sao không nên cấp điện liên tục cho cảm biến nước dạng vạch đồng? Có những cách khắc phục nào?** Gợi ý: điện phân ăn mòn điện cực; khắc phục bằng chỉ cấp điện lúc đo, đảo chiều điện áp xoay chiều (AC excitation), hoặc dùng cảm biến điện dung không tiếp xúc.
- **Thiết kế điều khiển bơm cấp nước cho bể có 2 cảm biến mức (thấp, cao)?** Gợi ý: máy trạng thái 2 trạng thái (BƠM/NGHỈ), bật khi chạm mức thấp, tắt khi chạm mức cao (hysteresis); thêm bảo vệ: bơm chạy quá thời gian tối đa thì báo lỗi (cảm biến hỏng / hết nước nguồn).
