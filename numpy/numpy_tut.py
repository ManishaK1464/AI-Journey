python_list = [1,2,3,4,5]
print(python_list)
import numpy as np
# Rename your file from numpy.py to something else (e.g., my_numpy_example.py)
numpy_array = np.array([1,2,3,4,5])
print(numpy_array)
# np array faster less memory direct calcu and saving data effeiciently and no loops normal list are not good 
mark1 = [90,91,93,94,95]
#1d 2d 3d array and multidimensional arrays
#one_dimensional array
one_d_array = np.array([10,20,30,40,50])
print(one_d_array)
#two_dimensional array rows and columns
two_d_array = np.array([[10,20,30],
                        [40,50,60],
                        [70,80,90]])
print(two_d_array)
#three_dimensional array
three_d_array = np.array([[[10,20,30],
                           [40,50,60]],
                          [[70,80,90],
                           [100,110,120]]])
print(three_d_array)
#multidimensional array 3d 4d .. AI deep learning Mri scan 
#matrix 2d array np.array([1,2,3]) list to array 
#matrix array with default values why zeros and ones used for initialization builtin functions
#default values to future values to fill np.zeros((3, 4))  # 3 rows, 4 columns ((shape))
ones_array = np.ones((3, 4))  # 3 rows, 4 columns
print(ones_array)
full_array = np.full((3, 4), 7)  # 3 rows, 4 columns filled with 7( shape and value)
print(full_array)
#creating sequence of numbers
#arange function similar to range but returns an array range and arrange 
sequence_array = np.arange(0, 20, 2)  # start, stop, step 
print(sequence_array)
#creating identity matrix
identity_matrix = np.eye(3)  # 3x3 identity matrix size
print(identity_matrix)
#numpy array properties
