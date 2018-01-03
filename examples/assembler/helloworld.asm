pushi str # push address of NUL-terminated string
lstr $2 # load string into register 2
push $2 # push the string
obj 0 23 # call TextWindow.WriteLine
obj 0 17 # call TextWindow.Pause
ret # end program (return on empty stack)



str: char "Hallo Welt!" 0
