import pandas as pd
#handles millisons of rows efficiently
#integrate with other libraries like numpy, scipy, matplotlib
#series is a one-dimensional array with labels with index
#dataframe is a two-dimensional array with labels with index and columns and rows
#panel is a three-dimensional array with labels with index and columns
#df = pd.read_csv('sample.csv', encoding = "latin1")  # Read a CSV file into a DataFrame
#print(df)  
#unicode characters are used to represent text in pandas 
#encoding is used to convert text to unicode characters
#utf-8 is a common encoding format that supports all unicode characters
#cloud gcsfs can be used to store large datasets in pandas
df1 = {'Name': ['Alice', 'Bob', 'Charlie'],
       'Age': [25, 30, 35],
       'City': ['New York', 'Los Angeles', 'Chicago']}
df2 = pd.DataFrame(df1)
#print(df2)  # Print the DataFrame
#df2.to_csv('output.csv', index=False)  # Save the DataFrame to a CSV file without the index

#head and tail 
#print(df2.head(2))  # Print the first 2 rows of the DataFrame
#print(df2.tail(2))  # Print the last 2 rows of the DataFrame   
# columns and rows , what type , missing values info()
#print(df2.info())  # Print information about the DataFrame
#describe() gives statistical summary of the DataFrame
print(df2.describe())  # Print statistical summary of the DataFrame
# pd.read_csv etc for files and pd.dataframe for lists and dictionaries 
# std small and large (48,49,50,51,52,53,54,55,56,57,58,59,60) small deviation and large deviation b(10,30,50,70,90,110,130,150,170,190,210,230
# 250) large deviation variation in the data  mean check and see deviation 
#min 