# Há»c Arduino Uno R3 â€” tá»« cÆ¡ báº£n Ä‘áº¿n nhÃºng chuyÃªn sÃ¢u

CÃ´ng cá»¥: VS Code + PlatformIO. Board á»Ÿ cá»•ng `COM5` (khai bÃ¡o trong `platformio.ini`; COM4 trÃªn mÃ¡y nÃ y lÃ  webcam).

## CÃ¡ch náº¡p má»™t bÃ i
- **CÃ¡ch 1 (báº¥m nÃºt):** thanh tráº¡ng thÃ¡i dÆ°á»›i cÃ¹ng VS Code â†’ chá»n env (vd `env:lesson01`) â†’ báº¥m mÅ©i tÃªn **â†’ (Upload)**. Báº¥m biá»ƒu tÆ°á»£ng **á»• cáº¯m (Serial Monitor)** Ä‘á»ƒ xem dá»¯ liá»‡u board gá»­i lÃªn.
- **CÃ¡ch 2 (lá»‡nh):** báº¥m nÃºt `>_` (PlatformIO Core CLI) trÃªn thanh tráº¡ng thÃ¡i Ä‘á»ƒ má»Ÿ terminal, rá»“i gÃµ `pio run -e lesson01 -t upload` vÃ  `pio device monitor`

**Má»—i bÃ i má»™t thÆ° má»¥c** `src/lessonNN_*/`: code `main.cpp`, áº£nh sÆ¡ Ä‘á»“ ná»‘i dÃ¢y (`so_do*.png` chá»¥p tá»« Wokwi, hoáº·c `*.svg`), vÃ  náº¿u cÃ³ mÃ´ phá»ng thÃ¬ `wokwi.toml` + `diagram.json`.
Cháº¡y mÃ´ phá»ng (cáº§n extension Wokwi Simulator): build env cá»§a bÃ i â†’ `Ctrl+Shift+P` â†’ **Wokwi: Select Config File** â†’ chá»n `wokwi.toml` cá»§a bÃ i â†’ **Wokwi: Start Simulator**.

**LÆ°u Ã½**
- Má»—i láº§n má»Ÿ Serial Monitor, ná»™i dung Ä‘Æ°á»£c lÆ°u vÃ o file `logs/device-monitor-*.log`.
- NÃºt Upload cá»§a PlatformIO tá»± Ä‘Ã³ng Serial Monitor rá»“i má»Ÿ láº¡i sau khi náº¡p. Khi náº¡p báº±ng lá»‡nh `pio run -t upload` thÃ¬ pháº£i tá»± Ä‘Ã³ng Serial Monitor trÆ°á»›c (báº¥m vÃ o terminal â†’ `Ctrl+C`, hoáº·c ðŸ—‘ Kill Terminal), náº¿u khÃ´ng sáº½ bÃ¡o lá»—i `Access is denied`.
- Khi cÃ³ bÃ i má»›i Ä‘Æ°á»£c thÃªm vÃ o `platformio.ini`, cháº¡y `Ctrl+Shift+P` â†’ **Developer: Reload Window**, chá» PlatformIO táº£i xong rá»“i má»›i Upload. Náº¿u khÃ´ng, nÃºt Upload cÃ³ thá»ƒ bÃ¡o lá»—i *"Cannot read properties of undefined (reading 'id')"*.

## Lá»™ trÃ¬nh

### Pháº§n 1 â€” LÃ m quen (C++ cÆ¡ báº£n + chÃ¢n I/O)
1. [x] NhÃ¡y LED cÃ³ sáºµn â€” `setup()`, `loop()`, `delay()`
2. [x] Serial Monitor â€” in chá»¯, biáº¿n, kiá»ƒu dá»¯ liá»‡u, phÃ©p toÃ¡n
3. [x] LED ngoÃ i + breadboard + Ä‘iá»‡n trá»Ÿ â€” Ä‘á»‹nh luáº­t Ohm, `if/else`, `for`
4. [x] NÃºt nháº¥n â€” `digitalRead`, `INPUT_PULLUP`, chá»‘ng dá»™i phÃ­m
5. [x] `millis()` thay `delay()` â€” lÃ m nhiá»u viá»‡c cÃ¹ng lÃºc
6. [x] PWM â€” lÃ m má» LED vá»›i `analogWrite`
7. [x] Äá»c analog â€” biáº¿n trá»Ÿ / LDR, `analogRead`, `map()`, `constrain()` *(7B quang trá»Ÿ: má»›i mÃ´ phá»ng, chÆ°a cáº¯m tháº­t)*
8. [x] HÃ m, máº£ng â€” tá»• chá»©c code gá»n gÃ ng, báº£ng tra gamma (nhanh hÆ¡n `pow()` ~460 láº§n)

### Pháº§n 2 â€” Linh kiá»‡n ngoáº¡i vi
9. [x] LED 7 Ä‘oáº¡n 5611AH â€” máº£ng chá»¯ sá»‘, `bitRead` *(mÃ´ phá»ng âœ“, chÆ°a cáº¯m tháº­t)*
10. [x] Cáº£m biáº¿n siÃªu Ã¢m HC-SR04 â€” `pulseIn`, cáº£m biáº¿n lÃ¹i xe *(mÃ´ phá»ng âœ“, chÆ°a cáº¯m tháº­t)*
11. [x] DHT11 â€” dÃ¹ng thÆ° viá»‡n (`lib_deps`), Ä‘á»‘i tÆ°á»£ng, `isnan` *(mÃ´ phá»ng âœ“ vá»›i DHT22, chÆ°a cáº¯m tháº­t â€” Ä‘á»•i `LOAI_CAM_BIEN` thÃ nh DHT11)*
12. [x] LCD 1602 / OLED qua I2C â€” `Wire`, dÃ² Ä‘á»‹a chá»‰ I2C, sá»‘ hex, con trá» *(mÃ´ phá»ng âœ“, chÆ°a cáº¯m tháº­t)*
13. [x] Servo SG90 — thư viện Servo, độ dài xung, Timer1 *(mô phỏng ✓, chưa cắm thật)*
14. [ ] Äá»™ng cÆ¡ bÆ°á»›c 28BYJ-48 + ULN2003
15. [ ] Äá»™ng cÆ¡ DC + L298N
16. [ ] MÃ¡y tráº¡ng thÃ¡i (state machine) â€” dá»± Ã¡n tá»•ng há»£p

### Pháº§n 3 â€” NhÃºng chuyÃªn sÃ¢u (AVR ATmega328P)
17. [ ] BÃªn trong `digitalWrite`: thanh ghi `DDRx`, `PORTx`, `PINx`, phÃ©p toÃ¡n bit
18. [ ] Ngáº¯t ngoÃ i (INT0/INT1), `volatile`, ISR
19. [ ] Timer/Counter â€” tá»± táº¡o `millis()`, PWM báº±ng thanh ghi
20. [ ] ADC báº±ng thanh ghi
21. [ ] UART báº±ng thanh ghi (tá»± viáº¿t `Serial`)
22. [ ] I2C / SPI cáº¥p tháº¥p
23. [ ] Tiáº¿t kiá»‡m nÄƒng lÆ°á»£ng, watchdog
24. [ ] Láº­p trÃ¬nh AVR thuáº§n (khÃ´ng dÃ¹ng Arduino framework), Ä‘á»c datasheet
