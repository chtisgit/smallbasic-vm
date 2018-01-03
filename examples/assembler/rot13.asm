pushi entertext
lstr $1
push $1
obj 0 24 # TextWindow.Write

obj 0 20 # TextWindow.Read
obj 2 9 # Text.ConvertToUpperCase
pop $1

pushi uppmsg
lstr $2
push $2
obj 0 23 # TextWindow.WriteLine

push $1
obj 2 1 # Text.GetLength
pop $2

estr $4 # new encrypted string

# for($3 = 0; $3 < $2; $3++)
movi $3 0 # counter
jmp mainloop.condition
mainloop:

pushi 1
push $3
push $1
obj 2 5 # Text.GetSubText

obj 2 11 # Text.GetCharacterCode

call rot13
push $0
obj 2 10 # Text.GetCharacter
push $4
obj 2 0 # Text.Append
pop $4

addi $3 $3 1
.condition:
geq $3 $2
jnc mainloop

pushi encrmsg
lstr $1
ccat $5 $1 $4
push $5
obj 0 23 # TextWindow.WriteLine

ret


rot13:
pop $0
# only rot13 encrypt if it's a character!
push $1
movi $1 65
lrt $0 $1
movi $1 90
grt $0 $1
jc .exit

addi $0 $0 -65
addi $0 $0 13
movi $1 26
lrt $0 $1
jc .nosub
addi $0 $0 -26
.nosub:
addi $0 $0 65
.exit:
pop $1
ret


entertext: char "Please Enter Text: " 0
uppmsg: char "I convert your message to uppercase!" 10 0
encrmsg: char "Encrypted output: " 0
