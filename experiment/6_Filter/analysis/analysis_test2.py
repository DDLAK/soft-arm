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

