import numpy as np
#add remove stack split merge split join new array will be created here arrays are fixed size
#insrt remove append extend pop clear reverse sort
arr = np.array([10, 12, 11, 13, 12])  # Create a numpy array
np.insert(arr, 2, 15, axis = None)  # Insert 15 at index 2 
#axis = None means 1D array axis = 0 means 2D array row wise axis = 1 means 2D array column wise
np.delete(arr, 3, axis = None)  # Delete element at index 3
np.append(arr, [14, 16])  # Append elements 14 and 16 to the end of the array
np.sort(arr)  # Sort the array in ascending order
# Note: The above operations return new arrays, as numpy arrays are immutable.
# To modify the original array, you need to assign the result back to the original variable.
arrd_2d = np.array([[1,2],[3,4]])
new_arr = np.insert(arrd_2d, 1, 5, axis=0)  # Insert a new row at index 1 here 5,5 
print("Original Array:\n", new_arr)
