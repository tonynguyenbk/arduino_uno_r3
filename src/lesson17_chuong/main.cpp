// BÀI 17: Chuông (buzzer) - phát giai điệu, còi hú, còi báo động bằng tone()
// Mô phỏng: wokwi/wokwi.toml
//
// Nối dây: chuông chân (+) -> chân 8, chân (-) -> GND.  Nút nhấn: chân 2 và GND.
//
// 2 loại chuông trong kit (lật mặt dưới lên xem):
//   - Chuông THỤ ĐỘNG (passive): thấy bảng mạch xanh lộ ra. Chỉ là cái loa nhỏ: phải tự tạo sóng vuông
//     bằng tone() mới kêu, đổi tần số -> đổi nốt nhạc. BÀI NÀY dùng loại này.
//   - Chuông CHỦ ĐỘNG (active): đáy bịt kín nhựa đen, thường có tem dán. Bên trong có sẵn mạch dao động:
//     cấp HIGH là kêu "bíp" 1 tiếng cố định, không đổi được nốt. Dùng loại này thì chỉ nghe bíp đều.
//
// tone(chân, tần số Hz): tạo sóng vuông 50% liên tục ra chân đó (dùng Timer2 của chip,
//   nên khi đang kêu thì analogWrite chân 3 và 11 bị ảnh hưởng). noTone(chân): tắt.
// Âm thanh = rung động. Tần số cao -> âm cao. Nốt La (A4) = 440 Hz; lên 1 quãng tám = nhân đôi tần số.
//
// Nhấn nút để đổi chế độ: 0 = tắt, 1 = giai điệu, 2 = còi hú (cứu thương), 3 = báo động (bíp bíp)

#include <Arduino.h>

const int CHUONG_PIN = 8;
const int NUT_PIN = 2;

// Tần số các nốt nhạc (Hz), quãng tám thứ 4
const int DO = 262, RE = 294, MI = 330, FA = 349, SOL = 392, LA = 440, SI = 494, NGHI = 0;

// Giai điệu "Twinkle Twinkle Little Star" (dân gian, không bản quyền): 2 MẢNG song song
const int NOT_NHAC[] = {DO, DO, SOL, SOL, LA, LA, SOL, FA, FA, MI, MI, RE, RE, DO,
                        SOL, SOL, FA, FA, MI, MI, RE, SOL, SOL, FA, FA, MI, MI, RE};
const int TRUONG_DO[] = {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2,
                         1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2};  // số phách
const int SO_NOT = sizeof(NOT_NHAC) / sizeof(NOT_NHAC[0]);  // tự đếm số phần tử của mảng
const unsigned long MOT_PHACH = 400;  // ms

int cheDo = 0;
const char* TEN_CHE_DO[] = {"Tat", "Giai dieu", "Coi hu", "Bao dong"};

// Trạng thái của từng chế độ (không dùng delay -> nút luôn phản hồi ngay)
int notHienTai = 0;
unsigned long lanCuoiDoi = 0;
int tanSoHu = 600;
int buocHu = 10;
bool dangBip = false;

// --- Nút nhấn (chống dội như bài 5) ---
int lanDocTruoc = HIGH, trangThaiNut = HIGH;
unsigned long thoiDiemThayDoi = 0;

bool nutVuaNhan() {
  int doc = digitalRead(NUT_PIN);
  if (doc != lanDocTruoc) {
    thoiDiemThayDoi = millis();
    lanDocTruoc = doc;
  }
  if (millis() - thoiDiemThayDoi >= 50 && doc != trangThaiNut) {
    trangThaiNut = doc;
    return trangThaiNut == LOW;
  }
  return false;
}

void choiGiaiDieu(unsigned long bayGio) {
  unsigned long doDai = TRUONG_DO[notHienTai] * MOT_PHACH;
  if (bayGio - lanCuoiDoi < doDai) return;  // nốt hiện tại chưa hết
  lanCuoiDoi = bayGio;
  notHienTai = (notHienTai + 1) % SO_NOT;
  int tanSo = NOT_NHAC[notHienTai];
  if (tanSo == NGHI) {
    noTone(CHUONG_PIN);
  } else {
    // Kêu 90% thời lượng nốt, im 10% -> 2 nốt giống nhau liền nhau vẫn nghe tách rời
    tone(CHUONG_PIN, tanSo, TRUONG_DO[notHienTai] * MOT_PHACH * 9 / 10);
  }
}

// Còi hú: tần số trượt lên xuống liên tục 600 <-> 1200 Hz
void coiHu(unsigned long bayGio) {
  if (bayGio - lanCuoiDoi < 5) return;
  lanCuoiDoi = bayGio;
  tanSoHu += buocHu;
  if (tanSoHu >= 1200 || tanSoHu <= 600) buocHu = -buocHu;
  tone(CHUONG_PIN, tanSoHu);
}

// Báo động: bíp 150ms, im 150ms
void baoDong(unsigned long bayGio) {
  if (bayGio - lanCuoiDoi < 150) return;
  lanCuoiDoi = bayGio;
  dangBip = !dangBip;
  if (dangBip) tone(CHUONG_PIN, 2000);
  else noTone(CHUONG_PIN);
}

void setup() {
  pinMode(NUT_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Bai 17: chuong - nhan nut de doi che do");
  Serial.print("Giai dieu co ");
  Serial.print(SO_NOT);
  Serial.println(" not");
}

void loop() {
  unsigned long bayGio = millis();

  if (nutVuaNhan()) {
    cheDo = (cheDo + 1) % 4;
    noTone(CHUONG_PIN);
    notHienTai = SO_NOT - 1;  // để lần đầu tăng lên thành nốt 0
    lanCuoiDoi = 0;
    Serial.print("Che do: ");
    Serial.println(TEN_CHE_DO[cheDo]);
  }

  switch (cheDo) {
    case 1: choiGiaiDieu(bayGio); break;
    case 2: coiHu(bayGio); break;
    case 3: baoDong(bayGio); break;
    default: break;  // 0 = tắt
  }
}
