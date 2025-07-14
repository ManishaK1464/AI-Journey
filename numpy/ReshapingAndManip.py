import numpy as np
# Reshaping and manipulating numpy arrays
# Reshape a 1D array to a 2D array
arr = np.array([1, 2, 3, 4, 5])
#reshaped_arr = arr.reshape((5, 1))  # Reshape to 5 rows and 1 column
reshaped_arr = arr.reshape(5, 1)
#((5, 1)) means 5 rows and 1 column its a tuple (5, 1) it expects a tuple as input as single argument
print("Reshaped Array:\n", reshaped_arr) 
#flattering array is copy of the original array
flattened_arr = reshaped_arr.flatten()
print("Flattened Array:\n", flattened_arr)  # Flatten the array to 1D is similar to ravel but returns a copy of the original array
#ravel array
raveled_arr = reshaped_arr.ravel()  # Ravel the array to 1D is view of the original array if possible
print("Raveled Array:\n", raveled_arr)  # Ravel the array to 1D is similar to flatten but returns a view of the original array if possible
#view of the array means it shares the same memory as the original array
#ravel and flatten difference
# ravel returns a view of the original array if possible, while flatten returns a copy of the original array.
# If you modify the raveled array, it will affect the original array, but modifying the flattened array will not affect the original array.