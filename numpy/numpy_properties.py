import numpy    as np   
# numpy array properties and mathematical operations
#numpy shape size and dimension type 
# warehouse of data products and items c
# numpy array properties shape 
arr_2d = np.array([[1, 2, 3], [4, 5, 4]])
print("Array:\n", arr_2d)   
print("Shape:", arr_2d.shape)  # (rows, columns) matrix shape
print("Size:", arr_2d.size)  # Total number of elements
print("Data Type:", arr_2d.dtype)   # Data type of elements
print("Number of Dimensions:", arr_2d.ndim)  # Number of dimensions no of rows = ndimensions
print("Total Bytes:", arr_2d.nbytes)  # Total bytes used by the array
print("Item Size (bytes):", arr_2d.itemsize)  # Size of each element in bytes
print("Memory Layout:", arr_2d.flags)  # Memory layout information  
arr_3d = np.array([[[1, 2], [3, 4]], [[5, 6], [7, 8]]])
print(arr_3d.shape)
#data type conversion
arr_float = arr_2d.astype(float)  # Convert to float type
print("Converted Array:\n", arr_float)
print("Converted Data Type:", arr_float.dtype)