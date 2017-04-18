import sys

output = []
	
def checknum(x, negatives_ok=True):
	if len(x) == 0:
		return False
	if negatives_ok and x[0] == '-':
		x = x[1:]
		if len(x) == 0:
			return False
	for c in x:
		if not (c >= '0' and c <= '9'):
			return False
	return True

def register(name):
	if name[0] != 'r' or not checknum(name[1:],False):
		raise Exception("expected register, got "+name)
	n = int(name[1:])
	assert n >= 0 and n <= 65535
	return [n & 0xFF, (n >> 8) & 0xFF]

def number2(num):
	if not checknum(num):
		raise Exception("expected small number, got ",num)
	n = int(num)
	assert n >= -32768 and n <= 32767
	if n < 0:
		n = 65536 + n
	return [n & 0xFF, (n >> 8) & 0xFF]

def number4(num):
	if not checknum(num):
		raise Exception("expected large number, got ",num)
	n = int(num)
	assert n >= -2**31 and n < 2**31
	if n < 0:
		n = 2**32 + n
	


# tests
assert register("r0") == [0,0]
assert register("r15") == [0xF,0]
assert register("r256") == [0,1]
assert register("r65535") == [0xFF, 0xFF]
assert number2("-1") == [0xFF, 0xFF]
assert number2("511") == [0xFF, 0x01]
assert number2("0") == [0,0]
assert number2("-257") == [0xFF,0xFE]

class Opcode(object):
	NONE = 0
	REGISTER = 1
	NUMBER2 = 2
	NUMBER4 = 4

	def __init__(self, code, first_p = NONE, second_p = NONE):
		assert second_p == Opcode.NONE or first_p != Opcode.NONE # second_p != NONE implies first_p != NONE
		assert first_p != Opcode.NUMBER4 or second_p == Opcode.NONE # first_p == NUMBER4 implies second_p == NONE
		self.code = code # opcode
		assert self.code >= 0 and self.code <= 255
		self.pcount = 0 # number of parameters (0,1,2)
		if first_p != Opcode.NONE:
			self.pcount = 1
			if second_p != Opcode.NONE:
				self.pcount = 2
		self.first_p = first_p
		self.second_p = second_p

	def genOpcode(self, p1, p2):
		global output
		output += [self.code]
		if self.first_p == Opcode.NONE:
			output += [0,0]
		elif self.first_p == Opcode.REGISTER:
			output += register(p1)
		elif self.first_p == Opcode.NUMBER2:
			output += number2(p1)
		elif self.first_p == Opcode.NUMBER4:
			output += number4(p1)

		if self.second_p == Opcode.NONE:
			output += [0,0]
		elif self.second_p == Opcode.REGISTER:
			output += register(p2)
		elif self.second_p == Opcode.NUMBER2:
			output += number2(p2)

mn = {}
mn["mov"] = Opcode(0x01, Opcode.REGISTER, Opcode.REGISTER)
mn["movi"] = Opcode(0x02, Opcode.REGISTER, Opcode.NUMBER2)


for line in sys.stdin:
	m = line.split()
	m[0] = m[0].lower()
	try:
		if mn[m[0]].pcount != len(m) - 1:
			print "mnemonic",m[0],"takes",mn[m[0]].pcount,"parameters"
		else:
			mn[m[0]].genOpcode(m[1], m[2])
	except KeyError as s:
		print "error: mnemonic ",s,"unkown"
	except Exception as s:
		print "error: ",s

print output
