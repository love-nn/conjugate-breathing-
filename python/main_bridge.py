import serial
import numpy as np
from scipy.signal import savgol_filter
import random
import time
import csv
import os
from datetime import datetime

# === 自动生成带时间戳的 CSV 文件名 ===
timestamp_str = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
filename = f"breath_log_{timestamp_str}.csv"
filepath = os.path.join(os.getcwd(), filename)

# === 串口配置（修改为你自己的端口号）===
nano_ser = serial.Serial('COM9', baudrate=9600, timeout=1)  # Nano Every
uno_ser = serial.Serial('COM14', baudrate=9600, timeout=1)   # UNO

# === 参数配置 ===
window_size = 7
poly_order = 2
k_amp = 1.2
lambda_amp = 0.00015
phase_delay = np.pi / 1.3
conjugate_offset = 5

raw_min, raw_max = 0, 5
mapped_min, mapped_max = -50, 50

# === 状态变量 ===
x_data = []
raw_data = []
sample_count = 0
start_time = None
last_value = 0
last_time = None

# === 打开 CSV 文件并写入表头 ===
csv_file = open(filepath, mode='w', newline='')
csv_writer = csv.writer(csv_file)
csv_writer.writerow(["Timestamp(s)", "Raw Voltage", "Smoothed", "Mapped Human", "Mapped Ocean", "Derivative"])

def map_range(value, in_min, in_max, out_min, out_max):
    if in_max - in_min == 0:
        return (out_min + out_max) / 2
    return (value - in_min) / (in_max - in_min) * (out_max - out_min) + out_min

print(f"🟢 Logging started... Output file will be saved as:\n{filepath}\n")

try:
    while True:
        raw_bytes = nano_ser.readline()
        raw = raw_bytes.decode('utf-8', errors='ignore').strip()

        if raw:
            if start_time is None:
                start_time = time.time()
                last_time = start_time

            # 尝试将 raw 转为浮点数，跳过非数值行
            try:
                value = float(raw)
            except ValueError:
                print(f"[跳过非浮点数据] {raw}")
                continue

            current_time = time.time()
            x_data.append(sample_count)
            raw_data.append(value)

            # 平滑滤波
            if len(raw_data) >= window_size:
                smoothed = savgol_filter(raw_data, window_size, poly_order)
            else:
                smoothed = np.array(raw_data)

            # 映射为 [-50, 50]
            mapped_human = [map_range(v, raw_min, raw_max, mapped_min, mapped_max) for v in smoothed]

            # 共轭呼吸生成逻辑
            amp_decay = k_amp * np.exp(-lambda_amp * sample_count)
            random_factor = random.choice([1, 0.5])
            delay_steps = int((window_size / (1.3 * np.pi)) * phase_delay * random_factor)
            short_T = max(3, delay_steps // 3)
            segment = mapped_human[-short_T:] if len(mapped_human) >= short_T else mapped_human
            copied_segment = list(segment) * 3
            copied_segment = copied_segment[:len(mapped_human)]

            # 随机扰动
            if random.random() < 1/30 and len(copied_segment) > 0:
                spike_index = random.randint(0, len(copied_segment) - 1)
                copied_segment[spike_index] += random.choice([-1, 1]) * random.uniform(50, 120)

            # mapped_ocean = [(-abs(v) + conjugate_offset) * amp_decay for v in copied_segment]
            mapped_ocean = [((-abs(v) + conjugate_offset) * amp_decay) * 10 for v in copied_segment]


            # 导数（变化速率）
            dt = current_time - last_time
            derivative = (mapped_ocean[-1] - last_value) / dt if dt > 0 else 0
            last_value = mapped_ocean[-1]
            last_time = current_time

            # 发送到 UNO，格式为：数值|导数\n
            if len(mapped_ocean) > 0:
                out_str = f"{mapped_ocean[-1]:.2f}|{derivative:.2f}\n"
                uno_ser.write(out_str.encode())

                # 写入 CSV
                csv_writer.writerow([
                    round(current_time - start_time, 3),
                    round(value, 4),
                    round(smoothed[-1], 4) if len(smoothed) > 0 else "",
                    round(mapped_human[-1], 4) if len(mapped_human) > 0 else "",
                    round(mapped_ocean[-1], 4) if len(mapped_ocean) > 0 else "",
                    round(derivative, 4)
                ])

            sample_count += 1

            if time.time() - start_time > 60:
                print("\n⏱️ Logging complete: 1 minute reached.")
                break

            time.sleep(0.025)  # 限制为约 40Hz

except KeyboardInterrupt:
    print("🛑 Logging stopped manually.")
finally:
    nano_ser.close()
    uno_ser.close()
    csv_file.close()
    print(f"\n💾 Data saved to:\n{filepath}")
