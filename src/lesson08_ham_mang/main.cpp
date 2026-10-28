// BÀI 8: Hàm và mảng
// Mạch giữ nguyên như bài 7A (xem ../lesson07_analog/so_do_7a.png):
//   LED chân 9, nút nhấn chân 2, biến trở A0.
// Mô phỏng: wokwi/wokwi.toml
//
// Vặn biến trở để chỉnh độ sáng. Nhấn nút để BẬT/TẮT hiệu chỉnh gamma (bài 6)
// -> so sánh bằng mắt: có gamma thì LED sáng lên ĐỀU hơn khi vặn.
//
// HÀM = một đoạn code được đặt tên, gọi lại được nhiều lần.
//   kiểuTrảVề tênHàm(tham số) { ... return giáTrị; }
//   void = không trả về gì.
// MẢNG = nhiều biến cùng kiểu xếp liền nhau, truy cập bằng số thứ tự (chỉ số) bắt đầu từ 0.
//   byte bang[256];   ->  bang[0], bang[1], ..., bang[255]

#include <Arduino.h>

const int BIEN_TRO_PIN = A0;
const int LED_PIN = 9;
const int NUT_PIN = 2;
const unsigned long THOI_GIAN_CHONG_DOI = 50;

// Bảng gamma: tính sẵn 256 kết quả của pow() MỘT LẦN trong setup().
// Sau đó muốn biết kết quả chỉ cần tra bảng: bangGamma[doSang] -> nhanh hơn pow() rất nhiều.
// byte = số nguyên 0..255, tốn 1 byte RAM -> cả bảng tốn 256 byte (Uno có 2048 byte RAM).
byte bangGamma[256];

bool dungGamma = true;
unsigned long lanCuoiIn = 0;

// --- Biến của hàm nutVuaNhan() ---
int lanDocTruoc = HIGH;
int trangThaiNut = HIGH;
unsigned long thoiDiemThayDoi = 0;

// ================== CÁC HÀM ==================

// Tính sẵn bảng gamma. Không trả về gì -> void.
void taoBangGamma() {
  for (int i = 0; i < 256; i++) {
    float tiLe = i / 255.0;
    bangGamma[i] = pow(tiLe, 2.2) * 255 + 0.5;  // + 0.5 để làm tròn thay vì cắt bỏ phần lẻ
  }
}

// Trả về true đúng 1 lần mỗi khi nút vừa được nhấn xuống (đã chống dội).
// Toàn bộ logic chống dội của bài 5, 6, 7 giờ gói gọn vào đây -> loop() dễ đọc hơn.
bool nutVuaNhan() {
  unsigned long bayGio = millis();
  int docNut = digitalRead(NUT_PIN);
  if (docNut != lanDocTruoc) {
    thoiDiemThayDoi = bayGio;
    lanDocTruoc = docNut;
  }
  if (bayGio - thoiDiemThayDoi >= THOI_GIAN_CHONG_DOI && docNut != trangThaiNut) {
    trangThaiNut = docNut;
    if (trangThaiNut == LOW) {
      return true;
    }
  }
  return false;
}

// Đổi giá trị ADC (0..1023) sang độ sáng (0..255). Nhận 1 tham số, trả về int.
int adcSangDoSang(int giaTriADC) {
  return map(giaTriADC, 0, 1023, 0, 255);
}

// In vài phần tử của mảng để xem bảng gamma trông thế nào.
void inBangGamma() {
  Serial.println("Mot so gia tri trong bangGamma:");
  for (int i = 0; i <= 255; i += 32) {   // i += 32 nghĩa là i = i + 32
    Serial.print("  bangGamma[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(bangGamma[i]);
  }
  Serial.print("  bangGamma[255] = ");
  Serial.println(bangGamma[255]);
}

// Đo xem pow() và tra bảng mất bao lâu. micros() = số micro giây (1/1.000.000 s) từ lúc chạy.
void soSanhTocDo() {
  volatile byte ketQua;  // volatile: bắt trình biên dịch làm thật, không được "lười" bỏ qua phép tính
  unsigned long batDau;

  batDau = micros();
  for (int i = 0; i < 256; i++) {
    ketQua = pow(i / 255.0, 2.2) * 255 + 0.5;
  }
  unsigned long thoiGianPow = micros() - batDau;

  batDau = micros();
  for (int i = 0; i < 256; i++) {
    ketQua = bangGamma[i];
  }
  unsigned long thoiGianBang = micros() - batDau;
  (void)ketQua;

  Serial.print("256 lan pow():    ");
  Serial.print(thoiGianPow);
  Serial.println(" us");
  Serial.print("256 lan tra bang: ");
  Serial.print(thoiGianBang);
  Serial.println(" us");
}

// Gửi độ sáng ra LED, có hoặc không qua bảng gamma. Trả về giá trị PWM thực sự đã ghi.
int datDoSang(int doSang) {
  int giaTriPWM;
  if (dungGamma) {
    giaTriPWM = bangGamma[doSang];
  } else {
    giaTriPWM = doSang;
  }
  analogWrite(LED_PIN, giaTriPWM);
  return giaTriPWM;
}

// ================== CHƯƠNG TRÌNH CHÍNH ==================

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(NUT_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Bai 8: ham va mang");

  taoBangGamma();   // gọi hàm: chỉ 1 dòng thay cho cả vòng for
  inBangGamma();
  soSanhTocDo();
  Serial.println("Vang bien tro; nhan nut de bat/tat gamma");
}

void loop() {
  // Nhờ có hàm, loop() đọc gần như tiếng Việt:
  if (nutVuaNhan()) {
    dungGamma = !dungGamma;
    Serial.println(dungGamma ? "--- Gamma: BAT ---" : "--- Gamma: TAT ---");
  }

  int doSang = adcSangDoSang(analogRead(BIEN_TRO_PIN));
  int giaTriPWM = datDoSang(doSang);

  if (millis() - lanCuoiIn >= 200) {
    lanCuoiIn = millis();
    Serial.print("do sang = ");
    Serial.print(doSang);
    Serial.print("\t PWM = ");
    Serial.println(giaTriPWM);
  }
}
