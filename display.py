import matplotlib.pyplot as plt
import numpy as np

#5000 5000 100 0.02
# n nt T dt

#-0.0146497 -0.00602234 -0.0515265 -0.100039 -0.101055 -0.153446 -0.157871 -0.199772 -0.247559 -0.180808 -0.244171 -0.165765 -0.15298 -0.29146 -0.438081 -0.326677 -0.305566 
def read_data(file_path):
    particles = []
    with open(file_path, 'r') as file:
        data = file.readlines()
        header = data[0].strip().split()
        n, nt, T, dt = map(float, header)

        for line in data[1:]:
            line = line.strip()
            line_data = list(map(float, line.split()))
            particles.append(line_data)
    return particles, (n, nt, T, dt)

if __name__ == "__main__":
    particles, (n, nt, T, dt) = read_data("particles.txt")
    time = [np.sqrt(i*dt) for i in range(int(nt))]
        
    for particle in particles:
        plt.plot(time, particle)
    plt.show()