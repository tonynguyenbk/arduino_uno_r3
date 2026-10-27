// BÀI 6: PWM - điều chỉnh độ sáng LED bằng analogWrite
// Nối dây: CHUYỂN dây cam từ chân 8 sang chân 9 (chân 8 không có PWM!).
//          Nút nhấn giữ nguyên ở chân 2.
// Chân có PWM trên Uno được đánh dấu ~ : 3, 5, 6, 9, 10, 11

#include <Arduino.h>

const int NUT_PIN = 2;
const int LED_PIN = 9;  // phải là chân có dấu ~
const unsigned long THOI_GIAN_CHONG_DOI = 50;

// Chế độ: 0 = "thở" (sáng dần rồi tối dần), 1 = 10%, 2 = 50%, 3 = 100%
int cheDo = 0;

// --- Hiệu ứng "thở" ---
int doSang = 0;      // 0 (tắt) .. 255 (sáng nhất)
int buoc = 5;        // mỗi lần đổi độ sáng bao nhiêu (+5 = sáng dần, -5 = tối dần)
unsigned long lanCuoiTho = 0;

// --- Nút nhấn (chống dội giống bài 5) ---
int lanDocTruoc = HIGH;
int trangThaiNut = HIGH;
unsigned long thoiDiemThayDoi = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(NUT_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Bai 6: PWM - nhan nut de doi che do");
  Serial.println("Che do 0: tho");
}

void loop() {
  unsigned long bayGio = millis();

  // ===== Nút nhấn: đổi chế độ 0 -> 1 -> 2 -> 3 -> 0 ... =====
  int docNut = digitalRead(NUT_PIN);
  if (docNut != lanDocTruoc) {
    thoiDiemThayDoi = bayGio;
    lanDocTruoc = docNut;
  }
  if (bayGio - thoiDiemThayDoi >= THOI_GIAN_CHONG_DOI && docNut != trangThaiNut) {
    trangThaiNut = docNut;
    if (trangThaiNut == LOW) {
      cheDo = (cheDo + 1) % 4;  // % 4: sau 3 thì quay về 0

      Serial.print("Che do ");
      Serial.print(cheDo);
      if (cheDo == 0) {
        Serial.println(": tho");
      } else if (cheDo == 1) {
        Serial.println(": 10% -> analogWrite(25)");
      } else if (cheDo == 2) {
        Serial.println(": 50% -> analogWrite(128)");
      } else {
        Serial.println(": 100% -> analogWrite(255)");
      }
    }
  }

  // ===== Điều khiển độ sáng =====
  if (cheDo == 0) {
    // Mỗi 40ms nhích độ sáng thêm 1 bước -> 51 bước x 40ms ~ 2 giây để sáng hết
    if (bayGio - lanCuoiTho >= 40) {
      lanCuoiTho = bayGio;
      doSang = doSang + buoc;
      if (doSang >= 255 || doSang <= 0) {  // || nghĩa là "hoặc"
        buoc = -buoc;                       // chạm đỉnh/đáy -> đảo chiều
      }
      // Hiệu chỉnh gamma: mắt người nhạy ở vùng tối, "lì" ở vùng sáng,
      // nên phải uốn cong giá trị để LED trông sáng dần ĐỀU.
      float tiLe = doSang / 255.0;        // 255.0 (số thực) -> giữ phần thập phân; /255 sẽ ra 0!
      float tiLeGamma = pow(tiLe, 2.2);   // tiLe mũ 2.2
      int giaTriPWM = tiLeGamma * 255;    // về lại thang 0..255
      analogWrite(LED_PIN, giaTriPWM);
    }
  } else if (cheDo == 1) {
    analogWrite(LED_PIN, 25);   // 25/255 ~ 10% thời gian bật
  } else if (cheDo == 2) {
    analogWrite(LED_PIN, 128);  // ~50%
  } else {
    analogWrite(LED_PIN, 255);  // 100% = bật liên tục
  }
}
