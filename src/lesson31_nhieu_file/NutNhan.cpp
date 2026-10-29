// NutNhan.cpp - ĐỊNH NGHĨA (phần thân) các hàm của class NutNhan
// "NutNhan::" nghĩa là: hàm này thuộc class NutNhan.

#include "NutNhan.h"

// Danh sách khởi tạo ": _chan(chan), ..." gán giá trị cho biến thành viên trước khi vào thân hàm
NutNhan::NutNhan(uint8_t chan, unsigned long thoiGianDoi)
  : _chan(chan), _thoiGianDoi(thoiGianDoi) {}

void NutNhan::begin() {
  pinMode(_chan, INPUT_PULLUP);
}

void NutNhan::capNhat() {
  _vuaNhan = _vuaNha = false;         // sự kiện chỉ sống trong 1 vòng loop
  bool doc = digitalRead(_chan);
  unsigned long bayGio = millis();

  if (doc != _docTruoc) {             // tín hiệu vừa nhảy -> bắt đầu đếm lại thời gian ổn định
    _docTruoc = doc;
    _lanDoi = bayGio;
  }
  // Giữ nguyên đủ lâu và khác trạng thái cũ -> thay đổi thật sự (đã hết dội)
  if (bayGio - _lanDoi >= _thoiGianDoi && doc != _trangThai) {
    _trangThai = doc;
    if (_trangThai == LOW) {
      _vuaNhan = true;
      _lucNhan = bayGio;
    } else {
      _vuaNha = true;
    }
  }
}

bool NutNhan::vuaNhan() const { return _vuaNhan; }
bool NutNhan::vuaNha() const { return _vuaNha; }

bool NutNhan::dangGiu(unsigned long ms) const {
  return _trangThai == LOW && millis() - _lucNhan >= ms;
}
