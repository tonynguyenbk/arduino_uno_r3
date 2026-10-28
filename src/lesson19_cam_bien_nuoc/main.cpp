// BÀI 19: Cảm biến nước (water level sensor) - báo mực nước, cảnh báo ngập
// Mô phỏng: wokwi/wokwi.toml (Wokwi không có cảm biến nước -> dùng biến trở trượt thay mực nước)
//
// Nối dây:
//   Cảm biến nước: "+" -> chân 7 (KHÔNG nối thẳng 5V, xem giải thích bên dưới), "-" -> GND, "S" -> A0
//   LED báo mức (mỗi LED 1 điện trở 220Ω): xanh -> 10, vàng -> 9, đỏ -> 8
//
// Cảm biến: các vạch đồng song song trên bảng mạch đỏ. Nước nối các vạch với nhau ->
//   càng ngập sâu, càng nhiều vạch được nối -> điện áp ra ở chân S càng cao.
//   Khô: ~0.   Ngập hết phần vạch: thường ~500-700 (tùy nước sạch/bẩn) -> phải đo thử để chỉnh ngưỡng.
//
// CHỐNG ĂN MÒN: cấp điện liên tục thì dòng điện chạy qua nước gây điện phân,
//   các vạch đồng bị ăn mòn rất nhanh (vài ngày đến vài tuần). Cách xử lý:
//   CHỈ cấp điện lúc đo: bật chân 7 -> chờ 10ms -> đọc -> tắt chân 7. Đo mỗi 0.5 giây
//   -> cảm biến chỉ có điện ~2% thời gian -> bền hơn nhiều lần.
//
// ĐỘ TRỄ (hysteresis): mặt nước gợn sóng làm giá trị dao động quanh ngưỡng -> đèn nhấp nháy loạn.
//   Giải pháp: mức lên khi vượt ngưỡng, nhưng chỉ xuống khi thấp hơn ngưỡng 1 khoảng TRE.

#include <Arduino.h>

const int NGUON_CAM_BIEN = 7;
const int CAM_BIEN_PIN = A0;
const int LED_XANH = 10, LED_VANG = 9, LED_DO = 8;

// Ngưỡng các mức (đo thử với nước thật rồi chỉnh lại)
const int NGUONG[] = {0, 100, 350, 600};  // ngưỡng VÀO mức 0, 1, 2, 3
const int TRE = 30;
const char* TEN_MUC[] = {"Kho", "Thap", "Vua", "CAO - CANH BAO NGAP!"};

int mucHienTai = 0;
unsigned long lanCuoiDo = 0;
unsigned long lanCuoiNhay = 0;
bool doSang = false;

int docCamBien() {
  digitalWrite(NGUON_CAM_BIEN, HIGH);  // cấp điện
  delay(10);                           // chờ điện áp ổn định (10ms - chấp nhận được, đo 2 lần/giây)
  int giaTri = analogRead(CAM_BIEN_PIN);
  digitalWrite(NGUON_CAM_BIEN, LOW);   // ngắt điện ngay -> chống ăn mòn
  return giaTri;
}

// Tính mức mới có độ trễ: lên mức khi >= ngưỡng; xuống mức khi < ngưỡng - TRE
int tinhMuc(int giaTri, int mucCu) {
  int muc = mucCu;
  while (muc < 3 && giaTri >= NGUONG[muc + 1]) muc++;
  while (muc > 0 && giaTri < NGUONG[muc] - TRE) muc--;
  return muc;
}

void setup() {
  pinMode(NGUON_CAM_BIEN, OUTPUT);
  digitalWrite(NGUON_CAM_BIEN, LOW);
  pinMode(LED_XANH, OUTPUT);
  pinMode(LED_VANG, OUTPUT);
  pinMode(LED_DO, OUTPUT);
  Serial.begin(9600);
  Serial.println("Bai 19: cam bien nuoc - bao muc nuoc");
}

void loop() {
  unsigned long bayGio = millis();

  if (bayGio - lanCuoiDo >= 500) {
    lanCuoiDo = bayGio;
    int giaTri = docCamBien();
    int mucMoi = tinhMuc(giaTri, mucHienTai);

    Serial.print("Gia tri: ");
    Serial.print(giaTri);
    Serial.print("\t Muc: ");
    Serial.println(TEN_MUC[mucMoi]);
    if (mucMoi != mucHienTai) {
      Serial.print(">>> Doi muc: ");
      Serial.print(TEN_MUC[mucHienTai]);
      Serial.print(" -> ");
      Serial.println(TEN_MUC[mucMoi]);
      mucHienTai = mucMoi;
    }

    // Thanh báo mức: càng cao càng nhiều đèn
    digitalWrite(LED_XANH, mucHienTai >= 1);
    digitalWrite(LED_VANG, mucHienTai >= 2);
    if (mucHienTai < 3) digitalWrite(LED_DO, LOW);
  }

  // Mức cao: đèn đỏ nháy nhanh cảnh báo
  if (mucHienTai == 3 && bayGio - lanCuoiNhay >= 150) {
    lanCuoiNhay = bayGio;
    doSang = !doSang;
    digitalWrite(LED_DO, doSang);
  }
}
