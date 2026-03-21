import numpy as np
import pandas as pd
import matplotlib as mlt

from scipy import special
from scipy import integrate
import scipy


sinDegrees = special.sindg(90)
print(sinDegrees)



a = 0
b = 4
def targetFunc(x):
    func = 3 * x **2
    return func

result = integrate.quad(targetFunc, a, b)
print(result)



def TargetFub(y,x):
    return -2.0 * y

xi = np.linspace(0,1,10)
y0 = 1.0
result = integrate.odeint(TargetFub,y0,xi)
print(result)
