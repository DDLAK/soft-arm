import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

estimated_data = pd.read_csv("../data/estimation.csv")


# Configure the figure.
fig = plt.figure()

# 折线图
x = estimated_data['time']
plt.plot(x, list(estimated_data['esti_x']))

plt.plot(x, list(estimated_data['esti_y']))

plt.plot(x, list(estimated_data['esti_z']))

plt.show()

# Configure the figure.
fig = plt.figure()

# 折线图
x = estimated_data['time']
plt.plot(x, list(estimated_data['esti_a']))

plt.plot(x, list(estimated_data['esti_b']))

plt.plot(x, list(estimated_data['esti_c']))

plt.show()

