import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

estimated_data = pd.read_csv("../data/filter.csv")


# Configure the figure.
fig = plt.figure()

# 折线图
x = estimated_data['time']
plt.plot(x, list(estimated_data['esti_x']), label='x')

plt.plot(x, list(estimated_data['esti_y']), label='y')

plt.plot(x, list(estimated_data['esti_z']), label='z')
aver_z = np.ones(shape=2000) * np.average(estimated_data['esti_z'])
plt.plot(x, aver_z, 'black')

plt.legend()
plt.show()

# Configure the figure.
fig = plt.figure()

# 折线图
x = estimated_data['time']
plt.plot(x, list(estimated_data['esti_a']), label='x')

plt.plot(x, list(estimated_data['esti_b']), label='y')

plt.plot(x, list(estimated_data['esti_c']), label='z')

plt.legend()
plt.show()

