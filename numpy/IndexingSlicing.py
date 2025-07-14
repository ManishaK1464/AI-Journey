import numpy as np  
# Indexing and slicing in numpy arrays
#indexing I can access elements of an array using their index
#slicing I can access a range of elements in an array using slicing
#fancy indexing I can access elements of an array using a list of indices
#boolean indexing I can access elements of an array using a boolean mask
# boolean mask is an array of boolean values that indicates which elements to access with a condition
# zero indexing in numpy arrays starts from 0 and negative indexing starts from -1 right to left (last element)
#(1,2,3,4,5) (0,1,2,3,4) (-5,-4,-3,-2,-1)
# 1D array[index] 2D array[row_index, column_index] 3D array[depth_index, row_index, column_index]
arr = np.array([10, 12, 11, 13, 12])  # 1D array
print("1D Array:", arr)
print("Element at index 0:", arr[0])    # Accessing first element
print("Element at index -1:", arr[-1])  # Accessing last element
# Slicing the array
print("Slicing from index 1 to 3:", arr[1:4])  # Accessing elements from index 1 to 3 (exclusive)
print("Slicing from index 2 to end:", arr[2:])  # Accessing elements from index 2 to the end
print("Slicing from start to index 3:", arr[:4])  # Accessing elements from start to index 3 (exclusive)
print("Slicing with step 2:", arr[::2])  # Accessing every second element start omitted so default is 0 stop omitted so default is end
print("Slicing with negative step:", arr[::-1])  # Reversing the array
#fancy indexing
indices = [0, 2, 4]  # List of indices to access non sequential elements
fancy_indexed_arr = arr[indices]    # Accessing elements at specified indices
print("Fancy Indexed Array:", fancy_indexed_arr) 
#boolean indexing for filtering elements based on a condition
mask = arr > 11  # Creating a boolean mask for elements greater than 11
boolean_indexed_arr = arr[mask]  # Accessing elements using the boolean mask
print("Boolean Indexed Array (elements > 11):", boolean_indexed_arr) 