import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator
from mpl_toolkits.mplot3d import Axes3D 
import numpy as np

fig = plt.figure()
ax = fig.add_subplot(projection='3d')

xRaw = []
zRaw = [[],[]]
Z = np.array([[],[]])
with open("2.dat") as fp:
    for line in fp:
        sp = line.split();
        xRaw.append(float(sp[0]))
        zRaw[0].append(float(sp[1]))
        zRaw[1].append(float(sp[2]))
X = np.array(xRaw)
Y = [1, 2]
X, Y = np.meshgrid(X, Y)
Z = np.array(zRaw)
ax.set_yticks([1, 2])
surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                       linewidth=0, antialiased=False)

plt.show()
