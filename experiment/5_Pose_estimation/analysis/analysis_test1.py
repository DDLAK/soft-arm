# 短时间波动是否大
# 长时间运行 是否有变化
# 间隔一段时间重启测试 是否有变化

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

path = "../data/test1_"

csv_path = path + str(5) + ".csv"
rawdata = pd.read_csv(csv_path)

# Pre-process the data.
rawdata['l_accel_x'] *= 9.8118
rawdata['l_accel_y'] *= 9.8118
rawdata['l_accel_z'] *= 9.8118

# Configure the figure.
fig = plt.figure()

# 折线图
x = np.linspace(0, 60, 3600, False)
plt.plot(x, list(rawdata['l_gyro_x']), 'r')
aver_x = np.ones(shape=3600) * np.average(rawdata['l_gyro_x'])
plt.plot(x, aver_x, 'black')

plt.plot(x, list(rawdata['l_gyro_y']), 'y')
aver_y = np.ones(shape=3600) * np.average(rawdata['l_gyro_y'])
plt.plot(x, aver_y, 'black')

plt.plot(x, list(rawdata['l_gyro_z']), 'g')
aver_z = np.ones(shape=3600) * np.average(rawdata['l_gyro_z'])
plt.plot(x, aver_z, 'black')

plt.show()

# 箱线图
# plt.boxplot(list(rawdata['r_gyro_x']))

