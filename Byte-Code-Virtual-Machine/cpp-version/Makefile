# hello:
# 	echo "Hello, World"
# 	echo "This line will print if the file hello does not exist."
# 


# blah: blah.o
# 	cc blah.o -o blah # Runs third

# blah.o: blah.cpp
# 	cc -c blah.cpp -o blah.o # Runs second

# # Typically blah.c would already exist, but I want to limit any additional required files
# blah.cpp:
# 	echo "int main() { return 0; }" > blah.cpp # Runs first
# 
# 
# 
# 



# some_file: 
# 	touch some_file

# clean:
# 	rm -f some_file



# files := file1 file2
# some_file: $(files)
# 	echo "Look at this variable: " $(files)
# 	touch some_file
	
# file1:
# 	touch file1
# file2:
# 	touch file2

# clean:
# 	rm -f file1 file2 some_file




a := one two# a is set to the string "one two"
b := 'one two' # Not recommended. b is set to the string "'one two'"
all:
	printf '$a'
	printf $b