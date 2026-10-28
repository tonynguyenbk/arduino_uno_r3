"""Sinh diagram.json (Wokwi) + ảnh xem trước cho bài 16 - ngã tư.
Dùng: python -I gen_nga_tu.py <diagram.json> <preview.svg>
"""
import json, sys

UNO_X = 233.1  # mép phải Uno (đã xoay 90 độ): hàng chân DIGITAL
PIN_Y = {"13": 93.0, "12": 102.4, "11": 111.9, "10": 121.4, "9": 130.9, "8": 140.9,
         "7": 156.9, "6": 166.4, "5": 175.9, "4": 185.4, "3": 194.9, "2": 204.4,
         "A5.2": 54.9, "A4.2": 64.9, "GND.1": 83.4}
LEFT_X = 50.6  # mép trái Uno: POWER + ANALOG
LEFT_Y = {"5V": 127.9, "GND.2": 137.4, "GND.3": 146.9, "A0": 175.9, "A1": 185.4, "A2": 194.9, "A3": 204.4}

def pin_pos(p):
    if p.startswith("uno:"):
        n = p[4:]
        return (UNO_X, PIN_Y[n]) if n in PIN_Y else (LEFT_X, LEFT_Y[n])
    return PART_PINS[p]

parts = [
    {"type": "wokwi-arduino-uno", "id": "uno", "top": 0, "left": 0, "rotate": 90, "attrs": {}},
    {"type": "wokwi-lcd1602", "id": "lcd1", "top": -330, "left": 300, "attrs": {"pins": "i2c"}},
    {"type": "wokwi-pushbutton-6mm", "id": "btn1", "top": 500, "left": 520, "attrs": {"color": "blue"}},
]
PART_PINS = {
    "lcd1:GND": (304, -298), "lcd1:VCC": (304, -288.5), "lcd1:SDA": (304, -279), "lcd1:SCL": (304, -269.5),
    "btn1:1.l": (520, 502.2), "btn1:2.l": (520, 521), "btn1:2.r": (548, 521),
}
BOXES = [("UNO", 32, -32, 242, 242), ("LCD", 300, -330, 600, -210), ("NUT", 520, 500, 548, 523)]

# Cột đèn: LED xoay 90 độ (chiều kim đồng hồ) -> chân ở bên TRÁI, C ở trên, A ở dưới
COLOR = ["red", "yellow", "limegreen"]
def cot(ten, L, T, so_den=3, mau=COLOR):
    ids = []
    for i in range(so_den):
        t = T + 45 * i
        pid = f"{ten}{i}"
        parts.append({"type": "wokwi-led", "id": pid, "top": t, "left": L, "rotate": 90, "attrs": {"color": mau[i]}})
        PART_PINS[f"{pid}:C"] = (L + 3, t + 20)
        PART_PINS[f"{pid}:A"] = (L + 3, t + 30)
        BOXES.append((pid, L - 5, t + 5, L + 45, t + 45))
        ids.append(pid)
    return ids

NW = cot("bacNam1_", 630, -170)   # góc Tây-Bắc: đèn cho xe từ phía Bắc đi xuống
NE = cot("dongTay1_", 845, -170)  # góc Đông-Bắc: đèn cho xe từ phía Đông đi sang
SW = cot("dongTay2_", 630, 210)   # góc Tây-Nam: đèn cho xe từ phía Tây đi sang
SE = cot("bacNam2_", 845, 210)    # góc Đông-Nam: đèn cho xe từ phía Nam đi lên
PED = cot("nguoi", 630, 380, 2, ["red", "limegreen"])

conns = []
def noi(a, b, mau, duong):
    """duong: danh sách điểm trung gian tuyệt đối -> đổi ra lệnh h/v tương đối của Wokwi"""
    x, y = pin_pos(a)
    lenh = []
    for (nx, ny) in duong:
        if abs(nx - x) > 0.01:
            lenh.append(f"h{round(nx - x, 1):g}")
        if abs(ny - y) > 0.01:
            lenh.append(f"v{round(ny - y, 1):g}")
        x, y = nx, ny
    conns.append([a, b, mau, lenh])

def len_phai(chan, led, lan, mau):        # từ chân Uno sang phải tới làn, lên/xuống, rồi sang phải vào chân A
    _, ay = PART_PINS[f"{led}:A"]
    noi(f"uno:{chan}", f"{led}:A", mau, [(lan, PIN_Y[chan]), (lan, ay)])

MAU_DAY = ["red", "gold", "green"]
for chan, led, lan, m in zip(["13", "12", "11"], NW, [400, 415, 430], MAU_DAY): len_phai(chan, led, lan, m)
for chan, led, lan, m in zip(["10", "9", "8"], NE, [780, 795, 810], MAU_DAY): len_phai(chan, led, lan, m)
for chan, led, lan, m in zip(["7", "6", "5"], SE, [770, 755, 740], MAU_DAY): len_phai(chan, led, lan, m)
for chan, led, lan, m in zip(["4", "3", "2"], SW, [590, 575, 560], MAU_DAY): len_phai(chan, led, lan, m)

# Đèn người đi bộ: chân A1 (xanh), A2 (đỏ) bên trái Uno -> vòng xuống dưới
_, y_do = PART_PINS[f"{PED[0]}:A"]
_, y_xanh = PART_PINS[f"{PED[1]}:A"]
noi("uno:A2", f"{PED[0]}:A", "red", [(20, LEFT_Y["A2"]), (20, y_do)])
noi("uno:A1", f"{PED[1]}:A", "green", [(12, LEFT_Y["A1"]), (12, y_xanh)])
noi("uno:A3", "btn1:1.l", "blue", [(4, LEFT_Y["A3"]), (4, 502.2)])

# GND: nối chuỗi chân C trong mỗi cột (đi vòng bên trái cột)
def chuoi_gnd(ids):
    for a, b in zip(ids, ids[1:]):
        xa, ya = PART_PINS[f"{a}:C"]
        _, yb = PART_PINS[f"{b}:C"]
        noi(f"{a}:C", f"{b}:C", "black", [(xa - 8, ya), (xa - 8, yb)])
for c in (NW, NE, SW, SE, PED):
    chuoi_gnd(c)
noi("uno:GND.1", f"{NW[0]}:C", "black", [(385, 83.4), (385, -150)])
noi(f"{NW[0]}:C", f"{NE[0]}:C", "black", [(633, -175), (848, -175)])
noi(f"{SE[2]}:C", f"{SW[2]}:C", "black", [(840, 320), (840, 360), (625, 360), (625, 320)])
noi(f"{SW[2]}:C", f"{PED[0]}:C", "black", [(625, 320), (625, 400)])
noi(f"{PED[1]}:C", "btn1:2.r", "black", [(625, 445), (625, 521)])
noi("btn1:2.l", "uno:GND.3", "black", [(520, 541), (-4, 541), (-4, 146.9)])

# LCD (I2C)
noi("uno:A5.2", "lcd1:SCL", "blue", [(260, 54.9), (260, -269.5)])
noi("uno:A4.2", "lcd1:SDA", "green", [(270, 64.9), (270, -279)])
noi("uno:GND.1", "lcd1:GND", "black", [(280, 83.4), (280, -298)])
noi("uno:5V", "lcd1:VCC", "red", [(20.6, 127.9), (20.6, -340), (295, -340), (295, -288.5)])

# ---------- "Vẽ" đường bằng chữ (part wokwi-text) ----------
# Đường dọc Bắc-Nam: x 690..830, đường ngang Đông-Tây: y -10..130
DUONG = []
def chu(id_, text, left, top, mau="#bbbbbb"):
    parts.append({"type": "wokwi-text", "id": id_, "top": top, "left": left, "attrs": {"text": text, "color": mau}})
    DUONG.append((text, left, top))
# 2 mép đường dọc (mỗi đoạn 1 ký tự ║, cách nhau 20) - bỏ qua đoạn giao với đường ngang
k = 0
for y in range(-260, 520, 20):
    if -20 < y < 130:
        continue
    chu(f"mepDoc{k}a", "║", 684, y); chu(f"mepDoc{k}b", "║", 824, y); k += 1
# 2 mép đường ngang
k = 0
for x in range(300, 1000, 20):
    if 680 < x < 830:
        continue
    chu(f"mepNgang{k}a", "═", x, -22); chu(f"mepNgang{k}b", "═", x, 128); k += 1
# vạch kẻ giữa đường (nét đứt)
for i, y in enumerate(list(range(-250, -40, 40)) + list(range(170, 500, 40))):
    chu(f"vachDoc{i}", "¦", 755, y, "#e0e0e0")
for i, x in enumerate(list(range(320, 660, 40)) + list(range(860, 1000, 40))):
    chu(f"vachNgang{i}", "-", x, 52, "#e0e0e0")
# vạch qua đường cho người đi bộ (phía Tây của ngã tư)
for i, y in enumerate(range(0, 125, 14)):
    chu(f"zebra{i}", "▬▬", 640, y, "#ffffff")
# tên hướng
chu("tenBac", "BẮC ↑", 735, -290, "#ffdd55")
chu("tenNam", "NAM ↓", 735, 520, "#ffdd55")
chu("tenTay", "← TÂY", 300, 52, "#ffdd55")
chu("tenDong", "ĐÔNG →", 950, 52, "#ffdd55")

diagram = {"version": 1, "author": "tonynguyenbk", "editor": "wokwi", "parts": parts,
           "connections": conns, "dependencies": {}}
out = json.dumps(diagram, ensure_ascii=False, indent=2)
open(sys.argv[1], "w", encoding="utf-8").write(out + "\n")

# ---------- ảnh xem trước (gần đúng) ----------
X0, Y0, S = 40, 380, 0.9
def tx(x): return X0 + x * S
def ty(y): return Y0 + y * S
svg = [f'<svg xmlns="http://www.w3.org/2000/svg" width="1000" height="900"><rect width="1000" height="900" fill="#2b2b2b"/>']
# đường (chỉ để hình dung)
svg.append(f'<rect x="{tx(690)}" y="{ty(-380)}" width="{140*S}" height="{960*S}" fill="#444"/>')
svg.append(f'<rect x="{tx(300)}" y="{ty(-10)}" width="{720*S}" height="{140*S}" fill="#444"/>')
for n, x1, y1, x2, y2 in BOXES:
    fill = "#0e7c86" if n == "UNO" else ("#2a63b8" if n in ("LCD", "NUT") else "#555")
    svg.append(f'<rect x="{tx(x1)}" y="{ty(y1)}" width="{(x2-x1)*S}" height="{(y2-y1)*S}" fill="{fill}" stroke="#999"/>')
for text, left, top in DUONG:
    svg.append(f'<text x="{tx(left)}" y="{ty(top)+12}" font-size="13" fill="#ddd">{text}</text>')
for p in parts:
    if p["type"] == "wokwi-led":
        x, y = PART_PINS[p["id"] + ":A"]
        c = {"red": "#f33", "yellow": "#fd0", "limegreen": "#3f3"}[p["attrs"]["color"]]
        svg.append(f'<circle cx="{tx(x+30)}" cy="{ty(y-5)}" r="8" fill="{c}"/>')
for a, b, mau, lenh in conns:
    x, y = pin_pos(a)
    pts = [(x, y)]
    for l in lenh:
        d = float(l[1:])
        if l[0] == "h": x += d
        else: y += d
        pts.append((x, y))
    pts.append(pin_pos(b))
    col = {"gold": "#e0b000", "limegreen": "#3f3"}.get(mau, mau)
    svg.append('<polyline fill="none" stroke-width="2" stroke="' + col + '" points="' + " ".join(f"{tx(px):.1f},{ty(py):.1f}" for px, py in pts) + '"/>')
svg.append("</svg>")
open(sys.argv[2], "w", encoding="utf-8").write("\n".join(svg))
print(len(parts), "parts", len(conns), "connections")
