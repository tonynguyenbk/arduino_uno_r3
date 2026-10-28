# Bài 24: Relay — khóa cửa điện từ — ứng dụng thực tế

## Relay hoạt động thế nào
```
 Phía ĐIỀU KHIỂN (5V, ~70mA)          Phía TẢI (tới 250VAC / 10A)
   IN ──► transistor ──► cuộn dây ))) lá đồng:  COM ──┬── NO (thường hở: relay hút thì nối)
                       (nam châm điện)                └── NC (thường đóng: relay hút thì hở)
```
- Cuộn dây có điện → hút lá đồng → **COM nối sang NO**, nghe "tạch". Hai phía **cách ly hoàn toàn về điện** — Arduino 5V điều khiển được mạch 220V mà không chạm tới nó.
- Module relay có sẵn: **transistor** (chân Arduino không cấp nổi ~70mA cho cuộn dây), **diode** chống điện áp ngược khi cuộn dây ngắt (bài 15), **đèn báo**.
- **Module đen (SONGLE) có thêm opto cách ly** (chip 4 chân cạnh relay): chân IN chỉ điều khiển 1 LED bên trong opto, ánh sáng mới kích transistor phía relay → nhiễu/xung điện áp từ phía relay không lọt về Arduino. **Jumper J0/J1** chọn kích bằng mức THẤP hay CAO.

## ⚠️ An toàn điện
- Học và thử với **tải điện áp thấp**: khóa điện từ 12V, đèn LED, quạt/động cơ 5–12V.
- **Không tự đấu điện lưới 220V** khi chưa có người có chuyên môn hướng dẫn: mặt dưới module relay có các đường mạch mang điện 220V lộ ra, chạm vào có thể nguy hiểm tính mạng.
- Thông số in trên relay (10A 250VAC) là của **tiếp điểm**; đường mạch trên module rẻ thường không chịu được dòng đó lâu dài.

## Phần cứng này dùng ở đâu
- **Khóa cửa điện từ, chuông cửa, cổng tự động**: relay cấp điện cho khóa khi xác thực đúng.
- **Nhà thông minh:** công tắc wifi điều khiển đèn, bình nóng lạnh, máy bơm (bên trong thường là relay + vi điều khiển ESP8266/ESP32).
- **Ô tô:** relay đèn pha, còi, quạt két nước, bơm xăng — để công tắc nhỏ điều khiển dòng lớn.
- **Tủ điện công nghiệp:** PLC xuất tín hiệu ra relay/contactor để đóng cắt động cơ, bơm, van; tủ điện máy bơm tự động theo mực nước (bài 19).

## Kiến thức của bài trong hệ thống nhúng
- **Cách ly điện (galvanic isolation):** relay, opto cách ly, biến áp — tách phần điều khiển an toàn khỏi phần công suất. Bắt buộc trong thiết bị công nghiệp, y tế.
- **Logic kích mức thấp/cao (active-low/active-high):** hàm `datRelay(dong)` che giấu chi tiết này — phần còn lại của code chỉ nói "đóng/nhả". Đổi module chỉ sửa 1 hằng số.
- **Trạng thái an toàn khi khởi động:** ghi mức "nhả" trước rồi mới `pinMode(OUTPUT)` → relay không đóng nhầm 1 nhịp lúc board khởi động/reset. Thiết bị thật còn cần trạng thái an toàn khi mất điện (khóa *fail-safe*: mất điện thì mở — cửa thoát hiểm; *fail-secure*: mất điện vẫn khóa — két sắt).
- **Yêu cầu chức năng an toàn:** nút thoát hiểm bên trong luôn mở được cửa, kể cả khi bàn phím đang bị khóa vì nhập sai.
- **Relay cơ vs relay bán dẫn (SSR):** relay cơ có tuổi thọ tiếp điểm (~100.000 lần), đóng chậm (~10ms), phát tia lửa; SSR không tiếng động, đóng cắt nhanh, bền hơn nhưng tỏa nhiệt, có dòng rò — dùng khi đóng cắt liên tục (lò nhiệt PWM chậm, bài 6).

## Dự án thực tế
- **Hệ thống kiểm soát ra vào văn phòng:** bộ điều khiển nhận thẻ/vân tay/mã → relay → khóa từ (magnetic lock) dạng fail-safe kết nối với hệ thống báo cháy để tự mở khi có cháy.
- **Công tắc thông minh trong nhà:** vi điều khiển wifi + relay, điều khiển bằng app, hẹn giờ, đo công suất.
- **Tủ điều khiển bơm nước:** relay/contactor + cảm biến mực nước + bảo vệ chạy khô, quá dòng.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| Relay đóng khi lẽ ra nhả và ngược lại | Module kích mức thấp mà code đặt mức cao (hoặc ngược lại) | Đổi `RELAY_KICH_MUC_THAP`, hoặc đổi jumper J0/J1 trên module đen |
| Vừa cắm điện relay "tạch" 1 cái | Chân chưa có mức xác định lúc khởi động | Ghi mức "nhả" trước `pinMode` (đã có trong code) |
| Relay đóng thì Arduino reset/LCD loạn | Cuộn dây + tải gây sụt áp, nhiễu | Nguồn riêng cho relay/tải; dùng module có opto và bỏ jumper JD-VCC để cách ly hẳn |
| Đèn relay sáng mà tải không chạy | Đấu tải vào NC thay vì NO, hoặc mạch tải chưa có nguồn | Tải nối COM–NO; kiểm tra nguồn phía tải |
| Khóa điện từ làm relay dính tiếp điểm/hỏng sớm | Cuộn khóa sinh điện áp ngược khi ngắt | Thêm diode ngược song song với khóa điện từ (DC) |

## Bài tập mở rộng
1. **Dễ:** thêm đèn báo 2 màu (xanh = mở, đỏ = khóa) dùng chân A0, A1.
2. **Vừa:** chế độ "giữ mở": nhập mã + `A` thì cửa mở cho tới khi nhập mã + `B`.
3. **Khó:** ghép RFID (bài 22) để mở bằng **thẻ hoặc mã**: RC522 cần SPI (10–13) + RST → chuyển bàn phím sang 2–8 và A0, relay sang A1, chuông sang A2, nút trong sang A3.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** relay module, NO/NC/COM, optocoupler, galvanic isolation, active-low trigger, flyback diode, fail-safe/fail-secure lock, solid state relay (SSR), contactor.
- **Datasheet:** relay **SRD-05VDC-SL-C / JQC-3FF** (điện áp cuộn dây, dòng cuộn dây, thông số tiếp điểm, tuổi thọ cơ/điện), opto **PC817**.

## Câu hỏi phỏng vấn
- **Vì sao không nối thẳng cuộn relay vào chân vi điều khiển?** Gợi ý: cuộn dây cần dòng lớn hơn khả năng của chân (70mA > 20–40mA) và sinh điện áp ngược khi ngắt → cần transistor + diode.
- **Relay cơ và SSR, chọn cái nào cho lò nhiệt đóng cắt mỗi vài giây?** Gợi ý: SSR — đóng cắt nhiều lần không mòn, không tia lửa, im lặng; chú ý tản nhiệt và dòng rò.
- **Fail-safe và fail-secure khác nhau thế nào? Cửa thoát hiểm dùng loại nào?** Gợi ý: fail-safe mất điện thì mở (cửa thoát hiểm, theo quy định phòng cháy); fail-secure mất điện vẫn khóa (két, kho).
