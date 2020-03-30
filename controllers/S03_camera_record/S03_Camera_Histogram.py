#!/usr/bin/env python

import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import matplotlib.patches as patches

import numpy as np

BIN_WIDTH = 1
nbins = np.arange(0, 255 + BIN_WIDTH, BIN_WIDTH)

img = mpimg.imread('./images/image003.bmp')

fig = plt.figure(figsize=(15,9))

grid = plt.GridSpec(4, 3, wspace=0.1, hspace=0.2, figure=fig)
    
b_hist = plt.subplot(grid[2, 0:2])
b_hist.hist(img[:,:,2].ravel(), bins=255, range=(0, 255), fc='b', ec='b');
b_hist.set_ylim(0,19200)

g_hist = plt.subplot(grid[1, 0:2],sharex=b_hist)
g_hist.hist(img[:,:,1].ravel(), bins=255, range=(0, 255), fc='g', ec='g');
g_hist.set_ylim(0,19200)

r_hist = plt.subplot(grid[0, 0:2],sharex=b_hist)
r_hist.hist(img[:,:,0].ravel(), bins=255, range=(0, 255), fc='r', ec='r');
r_hist.set_ylim(0,19200)

b_img = plt.subplot(grid[2, 2])
plt.imshow(img[:,:,2], cmap="Blues", vmin=0, vmax=255)
plt.colorbar()

plt.subplot(grid[1, 2])
plt.imshow(img[:,:,1], cmap="Greens", vmin=0, vmax=255)
plt.colorbar()

plt.subplot(grid[0, 2],sharex=b_img)
plt.imshow(img[:,:,0], cmap="Reds", vmin=0, vmax=255)
plt.colorbar()

plt.subplot(grid[3, 2],sharex=b_img)
plt.imshow(img)
plt.colorbar()
fig = plt.gcf()
fig.delaxes(fig.get_axes()[-1])

plt.savefig('histogram.png')
plt.show()


# letterboxing


npix = 10

fig = plt.figure(figsize=(15,9))

grid = plt.GridSpec(4, 3, wspace=0.1, hspace=0.2, figure=fig)

b_hist = plt.subplot(grid[2, 0:2])
#df['Blue'].plot.hist(ax = b_hist,bins=nbins,color='b',ylim=[0,19200],xlim=[-1,256])
b_hist.hist(img[60-int(npix/2):59+int(npix/2),:,2].ravel(), bins=255, range=(0, 255), fc='b', ec='b');
b_hist.set_ylim(0,160*npix)

g_hist = plt.subplot(grid[1, 0:2],sharex=b_hist)
#df['Green'].plot.hist(ax = g_hist,bins=nbins,color='g')
g_hist.hist(img[60-int(npix/2):59+int(npix/2),:,1].ravel(), bins=255, range=(0, 255), fc='g', ec='g');
g_hist.set_ylim(0,160*npix)

r_hist = plt.subplot(grid[0, 0:2],sharex=b_hist)
#df['Red'].plot.hist(ax = r_hist,bins=nbins,color='r')
r_hist.hist(img[60-int(npix/2):59+int(npix/2),:,0].ravel(), bins=255, range=(0, 255), fc='r', ec='r');
r_hist.set_ylim(0,160*npix)

plt.subplot(grid[2, 2])
plt.imshow(img[60-int(npix/2):59+int(npix/2),:,2], cmap="Blues", vmin=0, vmax=255)
plt.colorbar()

plt.subplot(grid[1, 2])
plt.imshow(img[60-int(npix/2):59+int(npix/2),:,1], cmap="Greens", vmin=0, vmax=255)
plt.colorbar()

plt.subplot(grid[0, 2],sharex=b_img)
plt.imshow(img[60-int(npix/2):59+int(npix/2),:,0], cmap="Reds", vmin=0, vmax=255)
plt.colorbar()

ax = plt.subplot(grid[3, 2],sharex=b_img)
#plt.imshow(img[60-int(npix/2):59+int(npix/2),:,:])
plt.imshow(img)
plt.colorbar()
fig = plt.gcf()
fig.delaxes(fig.get_axes()[-1])

# Create a Rectangle patch and Add the patch to the Axes
rect = patches.Rectangle((0,60-int(npix/2)),160,npix,linewidth=1,edgecolor='k',facecolor='none')
ax.add_patch(rect)

plt.savefig('histogram_letterbox.png')
plt.show()


