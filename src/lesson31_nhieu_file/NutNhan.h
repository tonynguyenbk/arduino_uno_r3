// NutNhan.h - KHAI BÁO class nút nhấn: "nút này làm được gì" (giao diện)
// File .h chỉ nói CÓ những hàm nào; phần thân hàm nằm trong NutNhan.cpp.
// Ai muốn dùng nút chỉ cần #include "NutNhan.h" - không cần biết bên trong chống dội thế nào.

#pragma once            // chống include 2 lần (nếu không: lỗi "redefinition of class NutNhan")

#include <Arduino.h>

class NutNhan {
public:                 // phần CÔNG KHAI: code bên ngoài được gọi
  // Hàm tạo (constructor): chạy khi tạo đối tượng, vd NutNhan nutDo(12);
  explicit NutNhan(uint8_t chan, unsigned long thoiGianDoi = 50);

  void begin();                       // gọi trong setup()
  void capNhat();                     // gọi MỖI vòng loop(), không chặn

  bool vuaNhan() const;               // true đúng 1 lần ngay lúc vừa nhấn xuống
  bool vuaNha() const;                // true đúng 1 lần lúc vừa nhả ra
  bool dangGiu(unsigned long ms) const;   // đang giữ lâu hơn ms

private:                // phần RIÊNG TƯ: chỉ code trong class được đụng vào
  uint8_t _chan;
  unsigned long _thoiGianDoi;
  bool _trangThai = HIGH;             // trạng thái đã ổn định (INPUT_PULLUP: HIGH = nhả)
  bool _docTruoc = HIGH;
  unsigned long _lanDoi = 0;          // lúc tín hiệu đọc được thay đổi gần nhất
  unsigned long _lucNhan = 0;
  bool _vuaNhan = false, _vuaNha = false;
};
