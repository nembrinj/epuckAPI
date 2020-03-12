#********************************************************************************************
# Calibration Plot Demo
# 
# Description: Plot data from CSV file.
# Author: Beat Hirsbrunner, Julien Nembrini, Simon Studer (University of Fribourg)
# Version: 1.0 (2016-03-03)
#********************************************************************************************
from pandas import *
import pandas
import numpy as np
import matplotlib.pyplot as plt

# config
FILENAME='logPID.csv'

# get data from CSV file
log = read_csv(FILENAME, index_col=0)
#log.drop(log.columns[3], axis=1, inplace = True)

# plot data (dropping first 10 readings)
log[10:].plot(subplots=True,figsize=(12,9))
plt.legend(loc='upper right')
plt.savefig('test.svg')
plt.savefig('test.png')
plt.show()

