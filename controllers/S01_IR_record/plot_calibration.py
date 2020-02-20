#********************************************************************************************
# Calibration Plot Demo
# 
# Description: Plot data from CSV file.
#********************************************************************************************
from pandas import *
import pandas
import numpy as np
import matplotlib.pyplot as plt

def plot_sensors(csv,name) :
    # drop last empty column
    csv.drop(csv.columns[8], axis=1, inplace = True)
    # plot data with subplots
    csv.plot(subplots=True,sharey='col')
    plt.savefig(name+'sub.png')
    plt.savefig(name+'sub.pdf')
    plt.show()
    # plot data on single plot
    csv.plot()
    # set the legend on right corner
    plt.legend(loc='upper right')
    plt.savefig(name+'.png')
    plt.savefig(name+'.pdf')
    plt.show() 

# get data from CSV file
uncalib = read_csv('uncalib.csv', index_col=0)
plot_sensors(uncalib,"uncalib")

calib = read_csv('calib.csv', index_col=0)
plot_sensors(calib,"calib")




