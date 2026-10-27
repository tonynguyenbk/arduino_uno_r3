// BÀI 5: millis() THAY delay() - làm nhiều việc cùng lúc
// Giữ nguyên mạch bài 4: LED ở chân 8, nút nhấn ở chân 2.
// Chương trình KHÔNG có lệnh delay() nào. Mỗi vòng loop() chỉ "liếc đồng hồ"
// rồi quyết định việc nào đã đến lúc làm.

#include <Arduino.h>

const int NUT_PIN = 2;
const int LED_PIN = 8;
const unsigned long THOI_GIAN_CHONG_DOI = 50;  // ms
const bool DEBUG_LED = true;  // true = in ra mỗi lần ledNgoaiSang đổi giá trị

// unsigned long: số nguyên 32 bit KHÔNG ÂM (0 .. ~4,29 tỷ) - kiểu mà millis() trả về.
// Dùng int (tối đa 32767) sẽ bị tràn chỉ sau ~33 giây!

// --- Việc 1: nháy đèn L trên board, đổi trạng thái mỗi 500ms ---
unsigned long lanCuoiL = 0;
bool denLSang = false;

// --- Việc 2: nháy LED ngoài, nhịp thay đổi bằng nút ---
unsigned long lanCuoiNgoai = 0;
unsigned long nhipNgoai = 500;  // ms giữa 2 lần đổi trạng thái
bool ledNgoaiSang = false;

// --- Việc 3: đọc nút, chống dội KHÔNG dùng delay ---
int lanDocTruoc = HIGH;          // giá trị đọc thô ở vòng trước (có thể đang dội)
int trangThaiNut = HIGH;         // trạng thái đã "chốt" (ổn định đủ 50ms)
unsigned long thoiDiemThayDoi = 0;

// --- Việc 4: đếm xem loop() chạy được bao nhiêu vòng mỗi giây ---
unsigned long soVong = 0;
unsigned long lanCuoiIn = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(NUT_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Bai 5: millis() - nhan nut de doi nhip LED ngoai");
}

void loop() {
  unsigned long bayGio = millis();  // xem đồng hồ MỘT lần mỗi vòng
  soVong++;

  // ===== Việc 1: đèn L =====
  // "Đã đủ 500ms kể từ lần đổi trước chưa?" - chưa thì bỏ qua, đi làm việc khác.
  if (bayGio - lanCuoiL >= 500) {
    lanCuoiL = bayGio;
    denLSang = !denLSang;
    digitalWrite(LED_BUILTIN, denLSang);
  }

  // ===== Việc 2: LED ngoài =====
  if (bayGio - lanCuoiNgoai >= nhipNgoai) {
    lanCuoiNgoai = bayGio;
    ledNgoaiSang = !ledNgoaiSang;
    digitalWrite(LED_PIN, ledNgoaiSang);

    if (DEBUG_LED) {
      Serial.print("[");
      Serial.print(bayGio);
      Serial.print(" ms] ledNgoaiSang = ");
      Serial.print(ledNgoaiSang);
      Serial.print(" (nhip ");
      Serial.print(nhipNgoai);
      Serial.println(")");
    }
  }

  // ===== Việc 3: nút nhấn =====
  int docNut = digitalRead(NUT_PIN);
  if (docNut != lanDocTruoc) {
    // Giá trị vừa nhảy (do nhấn/nhả hoặc do dội) -> bấm lại đồng hồ chống dội
    thoiDiemThayDoi = bayGio;
    lanDocTruoc = docNut;
  }
  // Chỉ "chốt" trạng thái mới khi nó đứng yên đủ 50ms
  if (bayGio - thoiDiemThayDoi >= THOI_GIAN_CHONG_DOI && docNut != trangThaiNut) {
    trangThaiNut = docNut;
    if (trangThaiNut == LOW) {  // vừa nhấn xuống -> đổi nhịp: 500 -> 250 -> 100 -> 500 ...
      if (nhipNgoai == 500) {
        nhipNgoai = 250;
      } else if (nhipNgoai == 250) {
        nhipNgoai = 100;
      } else {
        nhipNgoai = 500;
      }
      Serial.print("Nut nhan! Nhip LED ngoai = ");
      Serial.print(nhipNgoai);
      Serial.println(" ms");
    }
  }

  // ===== Việc 4: báo cáo mỗi giây =====
  if (bayGio - lanCuoiIn >= 1000) {
    lanCuoiIn = bayGio;
    Serial.print("Thoi gian chay: ");
    Serial.print(bayGio / 1000);
    Serial.print(" s | loop() chay ");
    Serial.print(soVong);
    Serial.println(" vong/giay");
    soVong = 0;
  }
}
