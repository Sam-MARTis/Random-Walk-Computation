import cupy as cp 
import numpy as np
import matplotlib.pyplot as plt
import time as t

n = 30000

t0 = t.perf_counter()
trajs = cp.random.normal(size=(1000, n))
trajs = cp.cumsum(trajs, axis=0)
t1 = t.perf_counter()
plt.figure(figsize=(8, 6))
for i in range(0, n):
    plt.plot(trajs[:, i].get(), alpha=0.1)
t2 = t.perf_counter()
print(f"Time taken for GPU computation: {t1 - t0:.4f} seconds")
print(f"Time taken for plotting: {t2 - t1:.4f} seconds")
plt.title("2D Random Walk")
plt.xlabel("X")
plt.ylabel("Y")
plt.grid()
plt.show()