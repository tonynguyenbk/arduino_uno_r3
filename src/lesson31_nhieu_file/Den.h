// Den.h - class đèn LED: bật, tắt, nháy với chu kỳ tùy ý - KHÔNG dùng delay()
// Mỗi đối tượng Den tự nhớ chân, chế độ, thời điểm nháy của riêng nó
// -> 3 đèn nháy 3 tốc độ khác nhau cùng lúc mà loop() vẫn gọn.

#pragma once

#include <Arduino.h>

class Den {
public:
  enum CheDo { TAT, SANG, NHAY };     // enum nằm TRONG class: dùng bên ngoài viết Den::NHAY

  explicit Den(uint8_t chan);

  void begin();
  void capNhat();                     // gọi mỗi vòng loop()

  void tat();
  void sang();
  void nhay(unsigned long chuKyMs);   // sáng nửa chu kỳ, tắt nửa chu kỳ

  CheDo cheDo() const { return _cheDo; }      // hàm ngắn có thể viết luôn trong .h
  unsigned long chuKy() const { return _chuKy; }

private:
  void ghi(bool sangTat);             // hàm phụ chỉ dùng nội bộ

  uint8_t _chan;
  CheDo _cheDo = TAT;
  unsigned long _chuKy = 1000;
  unsigned long _lanDoi = 0;
  bool _dangSang = false;
};
