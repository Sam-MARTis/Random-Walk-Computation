import numpy as np
import pandas as pd
import datashader as ds
import datashader.transfer_functions as tf
from datashader.utils import export_image


def read_data(file_path):
    particles = []
    # pids = []
    with open(file_path, 'r') as file:
        data = file.readlines()
        header = data[0].strip().split()
        n, nt, T, dt = map(float, header)
        # pid = 0
        for line in data[1:]:
            vals = list(map(float, line.split()))
            vals[-1] = np.nan
            particles.append(list(map(float, line.split())))
            # particles.append(np.nan)  # Append NaN to mark the end of trajectory
            # particles[-1][-1] = np.nan  # Append NaN to mark the end of trajectory
            # pids.append(pid)


    return np.array(particles), int(n), int(nt), T, dt


if __name__ == "__main__":
    particles, n, nt, T, dt = read_data("particles.txt")

    # Build (t, x) pairs for all trajectories
 # Build (t, x) pairs for all trajectories
    t = np.arange(nt) * dt
    t = np.tile(t, n)
    x = particles.reshape(-1)

    pid = np.repeat(np.arange(n), nt)

    df = pd.DataFrame({
        "t": t,
        "x": x,
        "pid": pid
    })

    cvs = ds.Canvas(
    plot_width=1600,
    plot_height=900,
    x_range=(0, T),
    y_range=(x.min(), x.max())
    )

    agg = cvs.line(df, "t", "x", agg=ds.count())

    img = tf.shade(agg, how="eq_hist")
    img = tf.set_background(img, "black")

    export_image(img, "trajectories_datashader")


    # agg = cvs.paths(df, "t", "x", agg=ds.any())

    # img = tf.shade(agg)
    # img = tf.set_background(img, "black")

    # export_image(img, "trajectories_datashader")

