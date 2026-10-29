// BÀI 31: Tổ chức code nhiều file + class (.h / .cpp)
// Mô phỏng: wokwi/wokwi.toml
//
// Cấu trúc thư mục:
//   main.cpp      - chương trình chính: chỉ còn "logic", ngắn gọn dễ đọc
//   NutNhan.h/.cpp - class nút nhấn: chống dội, nhấn, nhả, giữ lâu
//   Den.h/.cpp     - class đèn: tắt, sáng, nháy không chặn
// PlatformIO tự biên dịch MỌI file .cpp trong thư mục bài, rồi "liên kết" (link) lại thành 1 chương trình.
//
// Nối dây:
//   Đèn phòng khách -> 8, đèn phòng ngủ -> 5, đèn nhịp tim -> 2 (kèm điện trở 220Ω)
//   Nút đỏ -> 12 và GND, nút xanh -> A2 và GND
//
// Hoạt động:
//   Nút đỏ: đổi chế độ đèn khách:  tắt -> sáng -> nháy chậm -> nháy nhanh -> tắt ...
//   Nút xanh: tương tự cho đèn ngủ
//   Giữ nút xanh 2 giây: tắt hết
//   Đèn nhịp tim luôn nháy 1 giây: chứng tỏ chương trình không bị treo (heartbeat - có trong mọi thiết bị thật)

#include <Arduino.h>
#include "NutNhan.h"
#include "Den.h"

// Tạo đối tượng: cùng 1 class, nhiều "bản" độc lập, mỗi bản nhớ dữ liệu riêng
Den denKhach(8), denNgu(5), denNhipTim(2);
NutNhan nutDo(12), nutXanh(A2);

bool daTatHet = false;      // giữ nút xanh: chỉ tắt 1 lần, nhả ra mới tính tiếp

// Truyền THAM CHIẾU (Den&): hàm làm việc trực tiếp trên đèn thật, không phải bản sao
void doiCheDo(Den& den, const char* ten) {
  Serial.print(ten);
  if (den.cheDo() == Den::TAT) {
    den.sang();
    Serial.println(F(": sang"));
  } else if (den.cheDo() == Den::SANG) {
    den.nhay(1000);
    Serial.println(F(": nhay cham"));
  } else if (den.chuKy() == 1000) {
    den.nhay(250);
    Serial.println(F(": nhay nhanh"));
  } else {
    den.tat();
    Serial.println(F(": tat"));
  }
}

void setup() {
  Serial.begin(9600);
  denKhach.begin();
  denNgu.begin();
  denNhipTim.begin();
  nutDo.begin();
  nutXanh.begin();
  denNhipTim.nhay(1000);
  Serial.println(F("Bai 31: nut do = den khach, nut xanh = den ngu, giu xanh 2s = tat het"));
}

void loop() {
  nutDo.capNhat();
  nutXanh.capNhat();

  if (nutDo.vuaNhan()) doiCheDo(denKhach, "Den khach");
  // Nút xanh đổi chế độ lúc NHẢ (nếu đổi lúc nhấn thì giữ 2s để tắt hết sẽ bị đổi chế độ thừa 1 lần)
  if (nutXanh.vuaNha()) {
    if (!daTatHet) doiCheDo(denNgu, "Den ngu");
    daTatHet = false;
  }
  if (nutXanh.dangGiu(2000) && !daTatHet) {
    daTatHet = true;
    denKhach.tat();
    denNgu.tat();
    Serial.println(F(">> Tat het"));
  }

  denKhach.capNhat();
  denNgu.capNhat();
  denNhipTim.capNhat();
}
