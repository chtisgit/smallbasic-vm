#pushi str # push address of NUL-terminated string
#lstr $2 # load string into register 2
movi $3 5
movi $4 -5
movi $5 3
movi $6 99
movi $7 1
movi $8 42

movi $1 0 #i
movi $2 11 #elems-1

#fill array
write $10 $1 $3
addi $1 $1 1 #i++
write $10 $1 $4
addi $1 $1 1 #i++
write $10 $1 $5
addi $1 $1 1 #i++
write $10 $1 $6
addi $1 $1 1 #i++
write $10 $1 $7
addi $1 $1 1 #i++
write $10 $1 $8
addi $1 $1 1 #i++
#moreints
movi $3 54
write $10 $1 $3
addi $1 $1 1 #i++
movi $3 -54
write $10 $1 $3
addi $1 $1 1 #i++
movi $3 3333
write $10 $1 $3
addi $1 $1 1 #i++
movi $3 -321
write $10 $1 $3
addi $1 $1 1 #i++
movi $3 712
write $10 $1 $3
addi $1 $1 1 #i++
movi $3 535
write $10 $1 $3
addi $1 $1 1 #i++


#display arraycontents
reg_debug $10


pushi 0
push $2
call sort
pop $1
pop $1
#print sorted array
reg_debug $10

ret 0 # end program (return on empty stack)

sort:
#arguments
pop $31 #right=j
pop $30 #left=i
push $30
push $31
#$32 tmp
add $33 $30 $31
movi $32 2
div $33 $33 $32
read $33 $10 $33 #pivot = arr[(left+right)/2]

copy $38 $30
copy $39 $31

partition:
grt $30 $31 #while i <= j
jc p_end

lp:
read $35 $10 $30
geq $35 $33 #while arr[i] < pivot
jc lp_end
addi $30 $30 1
jmp lp
lp_end:

gp:
read $35 $10 $31
leq $35 $33 #while arr[j] > pivot
jc gp_end
addi $31 $31 -1
jmp gp
gp_end:

grt $30 $31 #if i <= j
jc igrtj
read $32 $10 $30
read $35 $10 $31
write $10 $30 $35
write $10 $31 $32
addi $30 $30 1
addi $31 $31 -1
igrtj:

jmp partition
p_end:





#rekursion

geq $38 $31 #if left < j
jc llrtj
push $30 #registersaving
push $31 
push $32 
push $33 
push $34 
push $35 
push $36 
push $37 
push $38 
push $39 
push $40 
push $38 #arguments
push $31
call sort #sort(left, j)
pop $31
pop $38
pop $40
pop $39
pop $38
pop $37
pop $36
pop $35
pop $34
pop $33
pop $32
pop $31 #arguments
pop $30
llrtj:

geq $30 $39 #if i < right
jc ilrtr
push $30 #registersaving
push $31 
push $32 
push $33 
push $34 
push $35 
push $36 
push $37 
push $38 
push $39 
push $40 
push $30 
push $39
call sort #sort(i, right))
pop $39
pop $30
pop $40
pop $39
pop $38
pop $37
pop $36
pop $35
pop $34
pop $33
pop $32
pop $31
pop $30
ilrtr:



ret 0



str: char "Hallo Welt!" 0




























