# unix cat program

start:
obj 0 20 # call TextWindow.Read
obj 0 23 #call TextWindow.WriteLine
jmp start # repeat
ret

