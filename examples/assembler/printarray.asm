
# prints the array in $10
printarray:
push $1 # save register 1, restore before returning
push $2
push $3
push $4
push $5

push $10
obj 1 2 # Array.GetAllIndices
pop $1 # this array contains all the indices from $10
len $2 $1 # length of array $1 in $2
estr $4
pushi .space
lstr $5


.chris1:
read $3 $1 $2
read $3 $10 $3

ccat $4 $5 $4
ccat $4 $3 $4

.chris1start:
addi $2 $2 -1
.chris1test:
zero $3
grt $2 $3
jc .chris1

pushi .arrstr
lstr $1
ccat $4 $1 $4

push $4
obj 0 23 # TextWindow.WriteLine

pop $5
pop $4
pop $3
pop $2
pop $1
ret 0
.arrstr: char "Array: " 0 
.space: char " " 0
