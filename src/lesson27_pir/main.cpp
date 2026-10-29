// BÀI 27: Cảm biến chuyển động PIR (HC-SR501) - đèn hành lang tự động + chế độ báo trộm
// Mô phỏng: wokwi/wokwi.toml (bấm vào cảm biến PIR khi đang chạy = giả lập có người đi qua)
//
// Nối dây:
//   PIR: VCC -> 5V, OUT -> chân 2 (ngắt INT0), GND -> GND
//   LED (đèn hành lang, kèm điện trở 220Ω) -> A0,  chuông -> A1,  nút đổi chế độ -> A2 (và GND)
//   (A0..A2 dùng làm chân digital)
//
// PIR = Passive InfraRed: KHÔNG phát gì, chỉ "nghe" bức xạ hồng ngoại (nhiệt) cơ thể người.
//   Bên trong có 2 nửa cảm biến; người đi ngang -> 2 nửa nhận nhiệt LỆCH nhau -> báo chuyển động.
//   Người đứng yên hoàn toàn -> không báo. Thấu kính trắng (vòm Fresnel) chia vùng nhìn thành nhiều ô.
// Module HC-SR501 có sẵn mạch xử lý: OUT = HIGH khi có chuyển động (giữ HIGH vài giây), LOW khi không.
//   2 núm vặn cam: Tx = thời gian giữ HIGH (3s..5 phút), Sx = độ nhạy/khoảng cách (3..7m).
//   Jumper: H = gặp chuyển động tiếp thì kéo dài HIGH, L = chỉ báo 1 lần.
//   Mới cấp điện cần ~30-60 giây "làm quen" môi trường, trong lúc đó có thể báo sai.

#include <Arduino.h>

const int PIR_PIN = 2;
const int DEN_PIN = A0, CHUONG_PIN = A1, NUT_PIN = A2;
const unsigned long THOI_GIAN_SANG = 10000;  // đèn sáng 10 giây sau lần chuyển động CUỐI CÙNG

volatile bool coChuyenDong = false;   // cờ do hàm ngắt bật lên
volatile unsigned long soLanPhatHien = 0;

// ISR: chỉ đặt cờ + đếm, việc nặng để loop() làm
void khiCoChuyenDong() {
  coChuyenDong = true;
  soLanPhatHien++;
}

enum CheDo { DEN_TU_DONG, BAO_TROM };
CheDo cheDo = DEN_TU_DONG;
unsigned long lanCuoiChuyenDong = 0, lanCuoiNhay = 0;
bool denDangSang = false, dangBaoDong = false, nhay = false;
int nutTruoc = HIGH;
unsigned long lanDoiNut = 0;

void inThoiGian(unsigned long ms) {
  unsigned long giay = ms / 1000;
  Serial.print("[");
  Serial.print(giay / 60);
  Serial.print(":");
  if (giay % 60 < 10) Serial.print('0');
  Serial.print(giay % 60);
  Serial.print("] ");
}

void setup() {
  pinMode(DEN_PIN, OUTPUT);
  pinMode(CHUONG_PIN, OUTPUT);
  pinMode(NUT_PIN, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT);
  Serial.begin(9600);
  Serial.println("Bai 27: PIR - nhan nut de doi che do: den tu dong <-> bao trom");
  // RISING: chỉ gọi khi OUT chuyển LOW -> HIGH (bắt đầu có chuyển động)
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), khiCoChuyenDong, RISING);
}

void loop() {
  unsigned long bayGio = millis();

  // ===== Nút đổi chế độ =====
  int doc = digitalRead(NUT_PIN);
  if (doc != nutTruoc && bayGio - lanDoiNut > 50) {
    lanDoiNut = bayGio;
    nutTruoc = doc;
    if (doc == LOW) {
      cheDo = (cheDo == DEN_TU_DONG) ? BAO_TROM : DEN_TU_DONG;
      dangBaoDong = false;
      denDangSang = false;
      digitalWrite(DEN_PIN, LOW);
      noTone(CHUONG_PIN);
      Serial.println(cheDo == BAO_TROM ? ">> CHE DO BAO TROM (nhan nut de tat)" : ">> CHE DO DEN TU DONG");
    }
  }

  // ===== Xử lý chuyển động (cờ do ngắt bật) =====
  if (coChuyenDong) {
    coChuyenDong = false;           // hạ cờ
    lanCuoiChuyenDong = bayGio;     // mỗi lần có chuyển động -> tính lại 10 giây từ đầu
    inThoiGian(bayGio);
    Serial.print("Phat hien chuyen dong (lan thu ");
    Serial.print(soLanPhatHien);    // đọc 4 byte có thể bị ngắt chen - chấp nhận được khi chỉ để in
    Serial.println(")");
    if (cheDo == DEN_TU_DONG) {
      denDangSang = true;
      digitalWrite(DEN_PIN, HIGH);
    } else if (!dangBaoDong) {
      dangBaoDong = true;
      Serial.println("!!! BAO DONG - CO NGUOI DOT NHAP !!!");
    }
  }

  // ===== Đèn tự tắt khi không còn ai =====
  if (denDangSang && bayGio - lanCuoiChuyenDong >= THOI_GIAN_SANG) {
    denDangSang = false;
    digitalWrite(DEN_PIN, LOW);
    inThoiGian(bayGio);
    Serial.println("Khong con chuyen dong 10s - tat den");
  }

  // ===== Báo động: đèn nháy + còi hú cho tới khi nhấn nút =====
  if (dangBaoDong && bayGio - lanCuoiNhay >= 200) {
    lanCuoiNhay = bayGio;
    nhay = !nhay;
    digitalWrite(DEN_PIN, nhay);
    tone(CHUONG_PIN, nhay ? 1800 : 1200);
  }
}
