# 短时间波动是否大
# 长时间运行 是否有变化
# 间隔一段时间重启测试 是否有变化

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

path = "../data/test1_"

#ax = []
# ay = []
# az = []
# gx = []
# gy = []
# gz = []
# for i in range(1, 6):
   # Import the data which stored in csv files.
csv_path = path + str(1) + ".csv"
rawdata = pd.read_csv(csv_path)
#
#     ax.append(np.average(rawdata['r_accel_x']))
#     ay.append(np.average(rawdata['r_accel_y']))
#     az.append(np.average(rawdata['r_accel_z']))
#     gx.append(np.average(rawdata['r_gyro_x']))
#     gy.append(np.average(rawdata['r_gyro_y']))
#     gz.append(np.average(rawdata['r_gyro_z']))
#
# #print(np.average(ax))
#print(np.average(ay))
#print(np.average(az))
#print(np.average(gx))
#print(np.average(gy))
#print(np.average(gz))

#Pre-process the data.
x = np.linspace(0, 60, 3600, False)

# Configure the figure.
fig = plt.figure()

# 折线图
plt.plot(x, list(rawdata['r_accel_x']), 'r')
plt.plot(x, list(rawdata['r_accel_y']), 'y')
plt.plot(x, list(rawdata['r_accel_z']), 'g')
# plt.plot(x, list(rawdata['r_gyro_x']), 'r')
# l = []
# for i in range(3600):
#     l.append(np.average(rawdata['r_gyro_x']))
# plt.plot(x, l, 'b')
# plt.plot(x, list(rawdata['r_gyro_y']), 'y')
# l = []
# for i in range(3600):
#     l.append(np.average(rawdata['r_gyro_y']))
# plt.plot(x, l, 'b')
# plt.plot(x, list(rawdata['r_gyro_z']), 'g')
# l = []
# for i in range(3600):
#     l.append(np.average(rawdata['r_gyro_z']))
# plt.plot(x, l, 'b')

plt.show()

# 箱线图
# plt.boxplot(list(rawdata['r_gyro_x']))

# display the plot
# plt.show()