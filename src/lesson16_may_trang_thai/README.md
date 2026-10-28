# Bài 16: Máy trạng thái — đèn giao thông ngã tư có pha người đi bộ — ứng dụng thực tế

## Sơ đồ trạng thái của bài
```
 ┌─────────┐ 10s ┌─────────┐ 3s  ┌─ có người chờ? ─┐
 │ BN_XANH │────►│ BN_VANG │────►│ có  → NGUOI_DI   │
 └─────────┘     └─────────┘     │ không → DT_XANH  │
      ▲                          └──────────────────┘
      │          ┌─────────┐ 3s  ┌─ có người chờ? ─┐
 DT_XANH ──8s──► │ DT_VANG │────►│ có  → NGUOI_DI   │
                 └─────────┘     │ không → BN_XANH  │
                                 └──────────────────┘
 NGUOI_DI ──7s──► NGUOI_NHAY ──3s──► hướng xe tiếp theo
 (bất kỳ) ──giữ nút 2s──► DEM (vàng nháy) ──giữ nút 2s──► BN_XANH
```
Bảng đèn (Đ = đỏ, V = vàng, X = xanh):

| Trạng thái | Bắc–Nam | Đông–Tây | Người đi bộ |
|---|---|---|---|
| BN_XANH | X | Đ | Đ |
| BN_VANG | V | Đ | Đ |
| DT_XANH | Đ | X | Đ |
| DT_VANG | Đ | V | Đ |
| NGUOI_DI / NGUOI_NHAY | Đ | Đ | X (nháy khi sắp hết) |
| DEM | V nháy | V nháy | tắt |

## Phần cứng/kỹ thuật này dùng ở đâu
- **Đèn giao thông ngã tư:** mỗi hướng có 2 cột đèn đối diện sáng giống nhau. Pha "tất cả xe dừng, người đi mọi hướng" như bài này có thật ở các ngã tư đông người (nổi tiếng nhất là ngã tư Shibuya, Tokyo).
- **Máy trạng thái có trong gần như mọi thiết bị:** máy giặt (chờ → cấp nước → giặt → xả → vắt → xong), lò vi sóng, thang máy, máy bán hàng tự động, cửa cuốn, máy ATM, nồi cơm điện.

## Kiến thức của bài trong hệ thống nhúng
- **Máy trạng thái hữu hạn (FSM)** là cách tổ chức firmware phổ biến nhất khi hệ thống có nhiều "chế độ": thay vì hàng chục biến cờ `if` lồng nhau, hệ thống luôn ở **đúng 1 trạng thái**, mỗi trạng thái chỉ quan tâm tới vài sự kiện.
- **Hành động khi vào trạng thái (entry action):** `vaoTrangThai()` đặt đèn đúng 1 lần — tránh ghi lại chân liên tục và dễ thêm log.
- **Bảng dữ liệu thay cho code lặp:** mảng `THOI_GIAN[]` theo trạng thái → 1 dòng `if` xử lý hết giờ cho mọi trạng thái.
- **Tách sự kiện khỏi xử lý:** `docNut()` chỉ trả về *sự kiện* (`NHAN_NGAN`, `NHAN_GIU`), máy trạng thái quyết định làm gì.
- **Giám sát an toàn độc lập:** `kiemTraAnToan()` không tin máy trạng thái, mà đọc lại chính các chân đang xuất; phát hiện 2 hướng cùng đi → về chế độ an toàn. Hệ thống thật có thêm **mạch phần cứng giám sát xung đột đèn (conflict monitor)** cắt điện tủ đèn khi phát hiện xanh–xanh.
- **Giao thức truyền thông cũng là máy trạng thái:** bộ giải mã UART, I2C, USB, TCP đều được viết dưới dạng FSM.

## Dự án thực tế
- **Bộ điều khiển đèn giao thông thương mại:** nhiều pha cho nhiều hướng và rẽ trái, chế độ đêm nháy vàng, ưu tiên xe cứu thương, đồng bộ "làn sóng xanh" giữa các nút giao, đổi thời gian pha theo lưu lượng đo bằng camera/vòng từ dưới mặt đường.
- **Thang máy:** trạng thái đứng/đi lên/đi xuống/mở cửa/đóng cửa + hàng đợi yêu cầu các tầng.
- **PLC công nghiệp:** dây chuyền chiết rót, đóng gói lập trình theo từng bước (ngôn ngữ SFC — Sequential Function Chart trong chuẩn IEC 61131-3 chính là máy trạng thái).
- **Ngành yêu cầu an toàn** (ô tô, đường sắt, y tế): thiết kế bằng sơ đồ trạng thái, có công cụ sinh code tự động (MATLAB Stateflow, QP framework) và kiểm chứng từng mũi tên.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Đèn chuyển lung tung nhiều trạng thái cùng lúc | Quên `break;` trong `switch` → chạy tràn xuống nhánh dưới | Mỗi `case` kết thúc bằng `break;` (trừ khi cố ý gộp như `NGUOI_DI`/`NGUOI_NHAY`) |
| Nhấn nút mà người chưa được đi ngay | Đúng thiết kế: phải hết pha xanh + vàng của hướng đang đi | Xem LCD: "Nguoi cho" và số giây còn lại |
| Giữ nút để vào chế độ đêm thì lại tính thêm 1 lần nhấn ngắn | Không đánh dấu đã báo "giữ" | Biến `daBaoGiu` trong `docNut()` |
| Serial báo "XUNG DOT DEN" | Lỗi trong bảng đèn của `vaoTrangThai()` | Sửa bảng; hàm an toàn đã chặn kịp |
| LED cháy hoặc rất nóng (mạch thật) | Nối LED thẳng không điện trở như hình mô phỏng | Mỗi LED 1 điện trở 220Ω |

## Bài tập mở rộng
1. **Dễ:** thêm 1 giây **tất cả đỏ** sau mỗi pha vàng (ngã tư thật có để xe cuối cùng kịp qua) — thêm trạng thái mới.
2. **Vừa:** thêm còi buzzer kêu "tít tít" khi người đi bộ được đi, nhanh dần khi sắp hết giờ (giúp người khiếm thị).
3. **Khó:** viết lại máy trạng thái dạng **bảng chuyển trạng thái** (mảng các dòng `{trangThai, suKien, trangThaiMoi}`) thay cho `switch` — cách các thư viện FSM chuyên nghiệp làm.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** finite state machine (FSM), state diagram, entry/exit action, event-driven, Moore/Mealy machine, Sequential Function Chart, UML statechart, traffic signal conflict monitor, pedestrian scramble.
- **Đọc thêm:** mô hình **Moore** (đầu ra chỉ phụ thuộc trạng thái — như bài này) và **Mealy** (đầu ra phụ thuộc cả sự kiện).

## Câu hỏi phỏng vấn
- **Khi nào nên dùng máy trạng thái thay vì `if/else`?** Gợi ý: khi hệ thống có nhiều chế độ, cùng 1 sự kiện mà xử lý khác nhau tùy chế độ, hoặc cần đảm bảo không rơi vào tổ hợp trạng thái vô lý (2 hướng cùng xanh).
- **Làm sao đảm bảo 2 hướng không bao giờ cùng xanh?** Gợi ý: đèn chỉ được đặt ở 1 chỗ (`vaoTrangThai()`) theo bảng đã kiểm tra; thêm giám sát độc lập bằng phần mềm (đọc lại đầu ra) và phần cứng (conflict monitor); khi lỗi thì về trạng thái an toàn (nháy vàng hoặc đỏ hết).
