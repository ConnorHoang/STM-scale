import os

# ---------- ENVIRONMENT FIX FOR GLIB / GTK SCHEMAS ----------
# Fixes "Settings schema 'org.gnome.settings-daemon.plugins.xsettings' missing" crash
os.environ["GSETTINGS_SCHEMA_DIR"] = "/usr/share/glib-2.0/schemas"
# -------------------------------------------------------------

import math
import time
from collections import deque

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import serial


SERIAL_PORT = "/dev/ttyACM0"
BAUD_RATE = 115200

WINDOW_S = 30          # seconds of history shown
POLL_HZ = 100          # firmware sends up to 100 lines/s per scale
MAX_POINTS = WINDOW_S * POLL_HZ
STALE_S = 0.5          # no line for this long -> switch off or board missing
TARE_SHOW_S = 1.0      # how long the "TARED" tag stays up after a tare

# Firmware line formats (one line per scale per poll):
#   A,<seq>,<mass>,<adc>,<tare>,<flags>\r\n   valid frame
#   A,ERR\r\n                                 failed sync/checksum

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
time.sleep(2)  # allow serial connection to settle
ser.reset_input_buffer()

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 7), sharex=True)
fig.suptitle("STM32 Dual Weighing Scale Monitor")
ax2.set_xlabel("Time (s)")


def make_scale(ax, title, color):
    (line,) = ax.plot([], [], color=color, linewidth=1.5)
    ax.set_title(title)
    ax.set_ylabel("Weight (g)")
    ax.grid(True)
    text = ax.text(
        0.98, 0.94, "No data", transform=ax.transAxes,
        ha="right", va="top", fontsize=11, family="monospace",
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.8),
    )
    return {
        "ax": ax, "line": line, "text": text,
        "t": deque(maxlen=MAX_POINTS),
        "mass": deque(maxlen=MAX_POINTS),
        "last": None,        # last valid frame (dict)
        "last_rx": -1e9,     # time any line for this scale arrived
        "frames": 0,
        "errors": 0,
        "tare_until": 0.0,
    }


scales = {
    "A": make_scale(ax1, "Scale A", "blue"),
    "B": make_scale(ax2, "Scale B", "orange"),
}

start_time = time.monotonic()


def parse(raw):
    """Return (scale_id, frame_dict) or (scale_id, None) for ERR, or None."""
    parts = raw.split(",")
    if not parts or parts[0] not in scales:
        return None  # header line, noise, or partial line

    if len(parts) == 2 and parts[1] == "ERR":
        return parts[0], None

    if len(parts) != 6:
        return None
    try:
        seq, mass, adc, tare, flags = (int(p) for p in parts[1:])
    except ValueError:
        return None
    return parts[0], {"seq": seq, "mass": mass, "adc": adc,
                      "tare": tare, "flags": flags}


def update(_frame):
    now = time.monotonic() - start_time

    # Read every complete line currently waiting
    while ser.in_waiting:
        raw = ser.readline().decode("ascii", errors="ignore").strip()
        result = parse(raw)
        if result is None:
            continue

        sid, data = result
        s = scales[sid]
        s["last_rx"] = now
        s["t"].append(now)

        if data is None:
            s["errors"] += 1
            s["mass"].append(math.nan)   # NaN breaks the line -> visible gap
            continue

        s["frames"] += 1
        s["mass"].append(data["mass"])
        s["last"] = data
        if data["flags"] & 0x01:
            s["tare_until"] = now + TARE_SHOW_S

    # Scroll the shared x-axis with real time, so a switched-off scale still moves
    ax2.set_xlim(max(0, now - WINDOW_S), max(WINDOW_S, now))

    for s in scales.values():
        if s["t"]:
            y = list(s["mass"])
            s["line"].set_data(list(s["t"]), y)

            finite = [v for v in y if not math.isnan(v)]
            if finite:
                ymin, ymax = min(finite), max(finite)
                pad = max((ymax - ymin) * 0.15, 1.0)
                s["ax"].set_ylim(ymin - pad, ymax + pad)

        if now - s["last_rx"] > STALE_S:
            status = "Switch off / no data"
        elif s["last"] is None:
            status = f"No valid frame\nerrors {s['errors']}"
        else:
            d = s["last"]
            status = (
                f"{d['mass']:6d} g\n"
                f"adc {d['adc']:4d}  tare {d['tare']:4d}\n"
                f"seq {d['seq']:3d}  err {s['errors']}"
            )
            if now < s["tare_until"]:
                status += "\nTARED"
        s["text"].set_text(status)

    return [s["line"] for s in scales.values()] + [s["text"] for s in scales.values()]


ani = FuncAnimation(fig, update, interval=100, blit=False, cache_frame_data=False)

try:
    plt.tight_layout()
    plt.show()
finally:
    ser.close()