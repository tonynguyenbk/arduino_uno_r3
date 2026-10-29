# Bài 31: Tổ chức code nhiều file + class (`.h` / `.cpp`)

## Vì sao phải tách file, viết class
- Các bài trước, mọi thứ nằm trong 1 file `main.cpp` với rất nhiều biến toàn cục (`nutTruoc`, `lanDoiNut`, `lanCuoiNhay`...). Thêm nút thứ 2, đèn thứ 3 là phải chép thêm cả bộ biến → dễ nhầm, khó sửa.
- **Class = khuôn đúc:** gom *dữ liệu* (chân, trạng thái, thời điểm) và *hành vi* (bật, tắt, nháy) vào một chỗ. Mỗi **đối tượng** đúc ra từ khuôn tự nhớ dữ liệu riêng: `Den denKhach(8), denNgu(5);`.
- **File `.h` (header) = giao diện:** liệt kê class có hàm gì. **File `.cpp` = phần thân:** viết các hàm làm việc thế nào. Người dùng class chỉ cần đọc `.h`.
- Các bài trước đã dùng sẵn class của người khác (`SoftwareSerial`, `LiquidCrystal_I2C`, `RTC_DS1307`) — bài này **tự viết** class của mình.

## Từng khái niệm trong code
| Trong code | Ý nghĩa |
|---|---|
| `#pragma once` | File `.h` chỉ được đưa vào 1 lần dù bị `#include` nhiều chỗ |
| `class NutNhan { public: ... private: ... };` | `public` = bên ngoài dùng được; `private` = chỉ class tự dùng → không ai lỡ tay sửa `_trangThai` |
| `NutNhan(uint8_t chan)` | **Hàm tạo (constructor):** chạy khi tạo đối tượng |
| `: _chan(chan), ...` | **Danh sách khởi tạo:** gán biến thành viên |
| `explicit` | Cấm C++ tự đổi kiểu ngầm (vd vô tình viết `NutNhan n = 12;`) |
| `NutNhan::capNhat()` | Hàm `capNhat` *thuộc* class `NutNhan` (viết trong `.cpp`) |
| `bool vuaNhan() const` | Hàm `const` hứa không thay đổi đối tượng — chỉ đọc |
| `enum CheDo` trong class, dùng `Den::NHAY` | Tên gọn, không đụng hàng với enum khác |
| `void doiCheDo(Den& den, ...)` | **Tham chiếu:** làm việc trên đèn thật chứ không phải bản sao |
| `begin()` / `capNhat()` | Mẫu quen thuộc của thư viện Arduino: khởi tạo trong `setup`, cập nhật không chặn trong `loop` |

## Biên dịch nhiều file diễn ra thế nào
1. **Tiền xử lý:** `#include "Den.h"` = chép nguyên nội dung `Den.h` vào chỗ đó.
2. **Biên dịch:** mỗi `.cpp` dịch **riêng** thành file `.o` (`main.cpp.o`, `Den.cpp.o`, `NutNhan.cpp.o`). `main.cpp` chỉ cần biết *khai báo* trong `.h` là dịch được.
3. **Liên kết (link):** ghép các `.o` + thư viện thành `firmware.elf` → `firmware.hex`. Nếu khai báo mà quên viết thân hàm → lỗi lúc link: `undefined reference to 'Den::nhay(unsigned long)'`.
- Sửa `Den.cpp` thì chỉ file đó dịch lại → dự án lớn build nhanh hơn nhiều.

## Kiến thức của bài trong hệ thống nhúng
- **Module hóa / driver:** firmware thật chia thành lớp: *driver* (nói chuyện với phần cứng: nút, LED, cảm biến) → *dịch vụ* (chống dội, lọc) → *ứng dụng* (logic). Mỗi lớp một nhóm file.
- **Đóng gói (encapsulation):** biến `private` → muốn đổi cách chống dội chỉ sửa `NutNhan.cpp`, code dùng nút không phải sửa gì.
- **Tái sử dụng:** chép `NutNhan.h/.cpp` sang dự án khác là dùng được ngay; đóng gói thành thư viện riêng trong thư mục `lib/` của PlatformIO.
- **Heartbeat LED:** đèn nháy đều = chương trình còn sống. Treo ở đâu đó là thấy ngay — thiết bị công nghiệp, router đều có.
- **Chi phí của class trên vi điều khiển:** gần như bằng 0 so với viết hàm thường (không dùng `virtual`, không cấp phát động `new`).

## Dự án thực tế
- **Thư viện cho cả nhóm dùng:** công ty viết sẵn driver cảm biến, giao tiếp — dự án mới chỉ ghép lại.
- **Nhà thông minh (bài 34):** class `Phong` gồm đèn + quạt + cảm biến; nhà có nhiều phòng = nhiều đối tượng.
- **Bảng điều khiển máy:** mảng `NutNhan nut[8]` quét 8 nút chỉ bằng 1 vòng `for`.

## Lỗi thường gặp
| Triệu chứng | Nguyên nhân | Cách sửa |
|---|---|---|
| `redefinition of 'class Den'` | `.h` bị include 2 lần, thiếu `#pragma once` | Thêm `#pragma once` đầu file `.h` |
| `undefined reference to 'Den::nhay(...)'` | Khai báo trong `.h` nhưng chưa viết thân trong `.cpp`, hoặc viết thiếu `Den::` | Viết `void Den::nhay(...) { ... }` |
| `multiple definition of 'bienX'` | Định nghĩa biến toàn cục trong `.h`, `.h` được include vào 2 file `.cpp` | `.h` chỉ để `extern int bienX;`, định nghĩa trong đúng 1 file `.cpp` |
| `'NutNhan' does not name a type` | Quên `#include "NutNhan.h"` | Include header ở file dùng class |
| Gọi hàm mà đèn không đổi | Truyền `Den den` (bản sao) thay vì `Den& den` | Dùng tham chiếu `&` |
| `'int Den::_chan' is private` | Code bên ngoài đụng biến `private` | Thêm hàm `public` để đọc (getter) |

## Bài tập mở rộng
1. **Dễ:** thêm vào class `Den` chế độ `NHAY_DOI` (nháy 2 lần rồi nghỉ, như đèn báo của HC-05 ở bài 30).
2. **Vừa:** viết class `CoiBao` (chuông, bài 17) với hàm `beep(soLan)` không chặn; nhấn nút nào cũng kêu "bíp" ngắn.
3. **Khó:** đưa `NutNhan` và `Den` thành thư viện trong thư mục `lib/ThietBi/` của PlatformIO (có `library.json`), rồi dùng lại trong bài 27 thay cho code xử lý nút cũ.

## Từ khóa và datasheet
- **Từ khóa tra cứu:** C++ class, constructor, member initializer list, header file, include guard, pragma once, translation unit, linker, undefined reference, encapsulation, const member function, pass by reference, PlatformIO lib folder, library.json.
- **Tài liệu:** "Writing a Library for Arduino" (arduino.cc), PlatformIO "Library Dependency Finder" và cấu trúc thư mục `lib/`.

## Câu hỏi phỏng vấn
- **Khai báo (declaration) và định nghĩa (definition) khác nhau thế nào?** Gợi ý: khai báo cho trình biên dịch biết tên/kiểu; định nghĩa tạo ra bộ nhớ hoặc thân hàm. Được khai báo nhiều lần nhưng chỉ định nghĩa đúng 1 lần (ODR).
- **Vì sao không nên dùng `new`/`String`/cấp phát động nhiều trên Arduino Uno?** Gợi ý: RAM chỉ 2KB, cấp phát rồi giải phóng gây phân mảnh heap → chạy lâu bị treo khó tìm; firmware nhúng ưu tiên cấp phát tĩnh.
- **Hàm `virtual` tốn gì trên vi điều khiển?** Gợi ý: mỗi class có bảng vtable trong flash, mỗi đối tượng thêm con trỏ vtable (2 byte trên AVR), gọi hàm qua con trỏ chậm hơn chút và không inline được — dùng khi thật sự cần đa hình (như `Stream*` bài 30).
