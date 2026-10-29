// Den.cpp - phần thân các hàm của class Den

#include "Den.h"

Den::Den(uint8_t chan) : _chan(chan) {}

void Den::begin() {
  pinMode(_chan, OUTPUT);
  ghi(false);
}

void Den::ghi(bool sangTat) {
  _dangSang = sangTat;
  digitalWrite(_chan, sangTat);
}

void Den::tat() {
  _cheDo = TAT;
  ghi(false);
}

void Den::sang() {
  _cheDo = SANG;
  ghi(true);
}

void Den::nhay(unsigned long chuKyMs) {
  _cheDo = NHAY;
  _chuKy = chuKyMs;
  _lanDoi = millis();
  ghi(true);
}

void Den::capNhat() {
  if (_cheDo != NHAY) return;
  if (millis() - _lanDoi >= _chuKy / 2) {
    _lanDoi += _chuKy / 2;            // cộng dồn (không gán = millis()) -> nháy đều, không trôi nhịp
    ghi(!_dangSang);
  }
}
