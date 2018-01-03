movi $3 5
movi $4 -5
movi $5 3
movi $6 99
movi $7 1
movi $8 42

movi $1 0 #i
movi $2 6 #elems

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

movi $1 0 #i
movi $11 0 #j
#display arraycontents
pushi BeforeStr
lstr $137
push $137
obj 0 23 # TextWindow.WriteLine
call printarray

call sort

#print sorted array
pushi AfterStr
lstr $137
push $137
obj 0 23 # TextWindow.WriteLine
call printarray

ret 0 # end program (return on empty stack)

sort:
#selectionsort

loop1:
lrt $1 $2 #i<elems  | checked all elems? 
jnc end
copy $11 $1 #j=i
movi $12 0x7fff #minelem
copy $20 $1 #minelempos
loop2:
read $13 $10 $11 #x

#if minelem > x  
grt $12 $13
jc mg
jmp mg_end #minelem <= x
mg: #minelem > x
copy $12 $13 #minelem = x
copy $20 $11
mg_end:

addi $11 $11 1 #j++
lrt $11 $2 #j < elems?
jc loop2
#innerloop end
#swap minelem <-> elem i
read $13 $10 $1 #a[i]
write $10 $1 $12 #a[i] = minelem
write $10 $20 $13 #a[minelempos] = a[i]
addi $1 $1 1 #i++
jmp loop1
end:

ret

include printarray.asm

BeforeStr: char "Before sort:" 0
AfterStr: char "After sort:" 0
