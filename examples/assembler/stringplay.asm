pushi str # push address of NUL-terminated string
lstr $2 # load string into register 2
push $2 # push the string
obj 0 23 # call TextWindow.WriteLine

movi $3 5
movi $4 -5
movi $5 1

slice $10 $2 $3
push $10 # push the string
obj 0 23 # call TextWindow.WriteLine

slice $11 $2 $4 
push $11 # push the string
obj 0 23 # call TextWindow.WriteLine





ret # end program (return on empty stack)



str: char "Hallo Welt!" 0
