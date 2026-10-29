// BÀI 26: Núm xoay vô cực (rotary encoder KY-040) - đọc bằng NGẮT, núm âm lượng 0..100
// Mô phỏng: wokwi/wokwi.toml (bấm mũi tên 2 bên núm để xoay, bấm vào núm = nhấn nút)
//
// Nối dây: CLK -> 2, DT -> 3, SW -> 4, "+" -> 5V, GND -> GND
//   (chân 2 và 3 là 2 chân NGẮT NGOÀI duy nhất của Uno: INT0, INT1)
//
// Khác biến trở (bài 7): xoay mãi không hết vòng, không biết "vị trí tuyệt đối", chỉ biết
// ĐÃ XOAY MẤY NẤC và THEO CHIỀU NÀO. Bên trong: 2 công tắc A (CLK) và B (DT) đóng mở lệch nhau
// 1/4 chu kỳ (tín hiệu "cầu phương" - quadrature):
//   xoay phải:  A B = 11 -> 01 -> 00 -> 10 -> 11      (A đổi trước B)
//   xoay trái:  A B = 11 -> 10 -> 00 -> 01 -> 11      (B đổi trước A)
//   -> nhìn thứ tự đổi là biết chiều. Mỗi nấc ("tách") = 4 lần đổi trạng thái.
//
// Vì sao dùng NGẮT: xoay nhanh, các xung chỉ dài vài mili giây. Nếu loop() đang bận (vẽ màn hình,
// delay...) thì sẽ BỎ LỠ xung -> đếm sai. Ngắt: phần cứng chip tự "gọi" hàm xuLyXoay() NGAY khi chân đổi
// mức, chen ngang loop() rồi quay lại chỗ cũ.

#include <Arduino.h>

const int CLK_PIN = 2, DT_PIN = 3, SW_PIN = 4;

// volatile: biến bị thay đổi trong hàm ngắt -> bắt trình biên dịch luôn đọc lại từ RAM,
// không được "nhớ tạm" giá trị cũ trong thanh ghi CPU.
volatile long soBuoc = 0;    // mỗi nấc = 4 bước
volatile uint8_t trangThaiCu = 0;

// Bảng tra: [trạng thái cũ AB][trạng thái mới AB] -> +1 (phải), -1 (trái), 0 (không đổi / nhiễu)
const int8_t BANG_CHUYEN[4][4] = {
  // mới: 00  01  10  11
  {  0, -1, +1,  0},  // cũ 00
  { +1,  0,  0, -1},  // cũ 01
  { -1,  0,  0, +1},  // cũ 10
  {  0, +1, -1,  0},  // cũ 11
};

// HÀM NGẮT (ISR): phải thật NGẮN và nhanh - không Serial.print, không delay
void xuLyXoay() {
  uint8_t moi = (digitalRead(CLK_PIN) << 1) | digitalRead(DT_PIN);  // ghép 2 bit: A B
  soBuoc += BANG_CHUYEN[trangThaiCu][moi];
  trangThaiCu = moi;
}

// Đọc biến 4 byte mà ngắt có thể sửa giữa chừng: tạm tắt ngắt để đọc nguyên vẹn
long docSoBuoc() {
  noInterrupts();
  long giaTri = soBuoc;
  interrupts();
  return giaTri;
}

int amLuong = 50;    // 0..100
long nacCu = 0;
bool tatTieng = false;
int nutTruoc = HIGH;
unsigned long lanDoiNut = 0;

void inAmLuong() {
  Serial.print(tatTieng ? "TAT TIENG " : "Am luong ");
  Serial.print(amLuong);
  Serial.print("\t[");
  for (int i = 0; i < 20; i++) Serial.print(i < amLuong / 5 && !tatTieng ? '#' : ' ');
  Serial.println("]");
}

void setup() {
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  pinMode(SW_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Bai 26: num xoay - am luong. Xoay de chinh, nhan de tat/bat tieng");

  trangThaiCu = (digitalRead(CLK_PIN) << 1) | digitalRead(DT_PIN);
  // Gọi xuLyXoay() mỗi khi chân 2 hoặc 3 ĐỔI MỨC (CHANGE: cả lên lẫn xuống)
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), xuLyXoay, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT_PIN), xuLyXoay, CHANGE);
  inAmLuong();
}

void loop() {
  long nac = docSoBuoc() / 4;  // 4 bước = 1 nấc
  if (nac != nacCu) {
    int thayDoi = nac - nacCu;
    nacCu = nac;
    amLuong = constrain(amLuong + thayDoi * 5, 0, 100);  // mỗi nấc 5%
    Serial.print(thayDoi > 0 ? ">> phai  " : "<< trai   ");
    inAmLuong();
  }

  // Nút nhấn trên núm: chống dội đơn giản bằng thời gian
  int doc = digitalRead(SW_PIN);
  if (doc != nutTruoc && millis() - lanDoiNut > 50) {
    lanDoiNut = millis();
    nutTruoc = doc;
    if (doc == LOW) {
      tatTieng = !tatTieng;
      inAmLuong();
    }
  }
}
