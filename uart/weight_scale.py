import os
import sys

# ---------- ENVIRONMENT FIX FOR GLIB / GTK SCHEMAS ----------
# Fixes "Settings schema 'org.gnome.settings-daemon.plugins.xsettings' missing" crash
os.environ["GSETTINGS_SCHEMA_DIR"] = "/usr/share/glib-2.0/schemas"
# -------------------------------------------------------------

import re
import time
from collections import deque
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import serial


SERIAL_PORT = "/dev/ttyACM0"  
BAUD_RATE = 115200  
MAX_POINTS = 200 

# Open UART connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
time.sleep(2)  # Allow serial connection to settle
ser.reset_input_buffer()

# Data storage
times = deque(maxlen=MAX_POINTS)
scale1_data = deque(maxlen=MAX_POINTS)
scale2_data = deque(maxlen=MAX_POINTS)

start_time = time.time()

# Set up two independent plots
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 7))
fig.suptitle("STM32 Dual Weighing Scale Monitor")

(line1,) = ax1.plot([], [], color="blue", linewidth=1.5)
(line2,) = ax2.plot([], [], color="orange", linewidth=1.5)

ax1.set_title("Scale 1")
ax1.set_ylabel("Weight (g)")
ax1.grid(True)

ax2.set_title("Scale 2")
ax2.set_xlabel("Time (s)")
ax2.set_ylabel("Weight (g)")
ax2.grid(True)

# Live weight indicators
value1 = ax1.text(
    0.98, 0.90, "Weight: -- g", transform=ax1.transAxes, ha="right", fontsize=12
)
value2 = ax2.text(
    0.98, 0.90, "Weight: -- g", transform=ax2.transAxes, ha="right", fontsize=12
)

# Expected UART format: Scale1: 125.4, Scale2: 231.8
pattern = re.compile(
    r"Scale1:\s*(-?\d+(?:\.\d+)?)\s*,\s*" r"Scale2:\s*(-?\d+(?:\.\d+)?)"
)


def update(frame):
    # Read all currently available complete lines
    while ser.in_waiting:
        raw = ser.readline().decode("utf-8", errors="ignore").strip()

        if not raw:
            continue

        match = pattern.search(raw)
        if not match:
            continue

        weight1 = float(match.group(1))
        weight2 = float(match.group(2))
        elapsed = time.time() - start_time

        times.append(elapsed)
        scale1_data.append(weight1)
        scale2_data.append(weight2)

    if times:
        x = list(times)
        y1 = list(scale1_data)
        y2 = list(scale2_data)

        line1.set_data(x, y1)
        line2.set_data(x, y2)

        # Update current readings
        value1.set_text(f"Weight: {y1[-1]:.2f} g")
        value2.set_text(f"Weight: {y2[-1]:.2f} g")

        # Auto-scale axes
        for ax, values in ((ax1, y1), (ax2, y2)):
            ax.set_xlim(max(0, x[-1] - 30), max(30, x[-1]))
            ymin = min(values)
            ymax = max(values)
            padding = max((ymax - ymin) * 0.15, 1.0)
            ax.set_ylim(ymin - padding, ymax + padding)

    return line1, line2, value1, value2


ani = FuncAnimation(
    fig, update, interval=100, blit=False, cache_frame_data=False
)

try:
    plt.tight_layout()
    plt.show()
finally:
    ser.close()