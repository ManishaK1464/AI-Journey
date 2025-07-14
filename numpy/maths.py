import numpy    as np
#no loops normal list are not good aggregation functions
arr = np.array([1, 2, 3, 4, 5])
print(arr+5)  # Adding 5 to each element
print(arr-5)  # Subtracting 5 from each element
print(arr*5)  # Multiplying each element by 5
print(arr/5)  # Dividing each element by 5
print(arr**2)  # Squaring each element
print(arr % 2)  # Modulus operation on each element
print(arr // 2)  # Floor division on each element
print(np.sqrt(arr))  # Square root of each element
print(np.exp(arr))  # Exponential of each element
print(np.log(arr))  # Natural logarithm of each element
print(np.sin(arr))  # Sine of each element
print(np.cos(arr))  # Cosine of each element
print(np.tan(arr))  # Tangent of each element
print(np.sum(arr))  # Sum of all elements
print(np.mean(arr))  # Mean of all elements is average
print(np.median(arr))  # Median of all elements is middle value
print(np.std(arr))  # Standard deviation of all elements is spread of data
print(np.var(arr))  # Variance of all elements
print(np.min(arr))  # Minimum value in the array
print(np.max(arr))  # Maximum value in the array
print(np.argmin(arr))  # Index of the minimum value in the array
print(np.argmax(arr))  # Index of the maximum value in the array
print(np.sort(arr))  # Sorting the array
print(np.unique(arr))  # Unique elements in the array
print(np.concatenate((arr, arr)))  # Concatenating the array with itself
print(np.reshape(arr, (5, 1)))  # Reshaping the array to a 5x1 matrix
print(np.transpose(np.reshape(arr, (5, 1))))  # Transposing the reshaped array
print(np.dot(arr, arr))  # Dot product of the array with itself
#print(np.cross(arr, arr))  # Cross product of the array with itself
print(np.linalg.norm(arr))  # L2 norm of the array