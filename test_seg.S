.globl testMov
.type testMov,@function
.align 16
testMov:
	movq (%rdi), %rax
	ret

.globl writeTLSVar
.type writeTLSVar,@function
.align 16
writeTLSVar:
	movq %rdi, %fs:(%rsi, %rdx)
	ret

.globl writeTLSVarAddOne
.type writeTLSVarAddOne,@function
.align 16
writeTLSVarAddOne:
	movq %rdi, %fs:0x1(%rsi, %rdx, 2)
	ret

.globl getTls
.type getTls,@function
.align 16
getTls:
	rdfsbase %rax
	ret

.globl setGsBase
.type setGsBase,@function
.align 16
setGsBase:
	wrgsbase %rdi
	ret

.globl getGsBase
.type getGsBase,@function
.align 16
getGsBase:
	rdgsbase %rax
	ret

