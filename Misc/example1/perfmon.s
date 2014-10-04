	.file	"perfmon.c"
	.comm	t1,16,16
	.comm	t2,16,16
	.comm	t3,16,16
	.comm	t4,16,16
	.globl	cycleCnt
	.bss
	.align 8
	.type	cycleCnt, @object
	.size	cycleCnt, 8
cycleCnt:
	.zero	8
	.globl	idx
	.align 8
	.type	idx, @object
	.size	idx, 8
idx:
	.zero	8
	.globl	jdx
	.data
	.align 8
	.type	jdx, @object
	.size	jdx, 8
jdx:
	.quad	1
	.globl	seqIterations
	.align 8
	.type	seqIterations, @object
	.size	seqIterations, 8
seqIterations:
	.quad	47
	.globl	reqIterations
	.align 8
	.type	reqIterations, @object
	.size	reqIterations, 8
reqIterations:
	.quad	1
	.globl	Iterations
	.align 8
	.type	Iterations, @object
	.size	Iterations, 8
Iterations:
	.quad	1
	.globl	fib
	.bss
	.align 8
	.type	fib, @object
	.size	fib, 8
fib:
	.zero	8
	.globl	fib0
	.align 8
	.type	fib0, @object
	.size	fib0, 8
fib0:
	.zero	8
	.globl	fib1
	.data
	.align 8
	.type	fib1, @object
	.size	fib1, 8
fib1:
	.quad	1
	.text
	.globl	fib_wrapper
	.type	fib_wrapper, @function
fib_wrapper:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	$0, idx(%rip)
	jmp	.L2
.L5:
	movq	fib0(%rip), %rdx
	movq	fib1(%rip), %rax
	addq	%rdx, %rax
	movq	%rax, fib(%rip)
	jmp	.L3
.L4:
	movq	fib1(%rip), %rax
	movq	%rax, fib0(%rip)
	movq	fib(%rip), %rax
	movq	%rax, fib1(%rip)
	movq	fib0(%rip), %rdx
	movq	fib1(%rip), %rax
	addq	%rdx, %rax
	movq	%rax, fib(%rip)
	movq	jdx(%rip), %rax
	addq	$1, %rax
	movq	%rax, jdx(%rip)
.L3:
	movq	jdx(%rip), %rdx
	movq	seqIterations(%rip), %rax
	cmpq	%rax, %rdx
	jb	.L4
	movq	idx(%rip), %rax
	addq	$1, %rax
	movq	%rax, idx(%rip)
.L2:
	movq	idx(%rip), %rdx
	movq	Iterations(%rip), %rax
	cmpq	%rax, %rdx
	jb	.L5
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	fib_wrapper, .-fib_wrapper
	.section	.rodata
.LC1:
	.string	"%ld"
.LC2:
	.string	"Using defaults"
	.align 8
.LC3:
	.string	"Usage: fibtest [Num iterations]"
.LC4:
	.string	"Cycle Count=%llu\n"
	.align 8
.LC6:
	.string	"Based on usleep accuracy, CPU clk rate = %lu clks/sec,"
.LC7:
	.string	" %7.1f Ghz\n"
	.align 8
.LC8:
	.string	"\nRunning Fibonacci(%d) Test for %ld iterations\n"
.LC9:
	.string	"startTSC =0x%016x\n"
.LC10:
	.string	"stopTSC =0x%016x\n"
	.align 8
.LC11:
	.string	"\nFibonacci(%lu)=%lu (0x%08lx)\n"
.LC12:
	.string	"\nCycle Count=%llu\n"
.LC13:
	.string	"\nInst Count=%lu\n"
.LC14:
	.string	"\nCPI=%1.32f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$72, %rsp
	movl	%edi, -52(%rbp)
	movq	%rsi, -64(%rbp)
	movl	$0, %eax
	movq	%rax, -40(%rbp)
	movl	$0, %eax
	movq	%rax, -32(%rbp)
	movq	$0, -24(%rbp)
	cmpl	$2, -52(%rbp)
	jne	.L7
	.cfi_offset 3, -24
	movl	$.LC1, %ecx
	movq	-64(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movl	$reqIterations, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_sscanf
	movq	reqIterations(%rip), %rcx
	movabsq	$6672226579852391011, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	movq	%rcx, %rax
	subq	%rdx, %rax
	shrq	%rax
	addq	%rdx, %rax
	movq	%rax, %rdx
	shrq	$5, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$4, %rax
	subq	%rdx, %rax
	movq	%rcx, %rdx
	subq	%rax, %rdx
	movq	%rdx, seqIterations(%rip)
	movq	reqIterations(%rip), %rax
	movq	seqIterations(%rip), %rdx
	movq	%rdx, -72(%rbp)
	movl	$0, %edx
	divq	-72(%rbp)
	movq	%rax, Iterations(%rip)
	jmp	.L8
.L7:
	cmpl	$1, -52(%rbp)
	jne	.L9
	movl	$.LC2, %edi
	call	puts
	jmp	.L8
.L9:
	movl	$.LC3, %edi
	call	puts
.L8:
	movq	seqIterations(%rip), %rdx
	movq	%rdx, %rax
	salq	$4, %rax
	movq	%rax, %rcx
	subq	%rdx, %rcx
	movq	Iterations(%rip), %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	movl	$t1, %esi
	movl	$0, %edi
	call	clock_gettime
	movl	$1000000, %edi
	call	usleep
	movl	$t2, %esi
	movl	$0, %edi
	call	clock_gettime
	movq	t2(%rip), %rdx
	movq	t1(%rip), %rax
	movq	%rdx, %rcx
	subq	%rax, %rcx
	movq	%rcx, %rax
	imulq	$1000000000, %rax, %rax
	movq	t2+8(%rip), %rcx
	movq	t1+8(%rip), %rdx
	movq	%rcx, %rbx
	subq	%rdx, %rbx
	movq	%rbx, %rdx
	addq	%rdx, %rax
	movq	%rax, cycleCnt(%rip)
	movq	cycleCnt(%rip), %rdx
	movl	$.LC4, %eax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movq	cycleCnt(%rip), %rax
	testq	%rax, %rax
	js	.L10
	cvtsi2sdq	%rax, %xmm0
	jmp	.L11
.L10:
	movq	%rax, %rdx
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	cvtsi2sdq	%rdx, %xmm0
	addsd	%xmm0, %xmm0
.L11:
	movsd	.LC5(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -40(%rbp)
	movq	cycleCnt(%rip), %rdx
	movl	$.LC6, %eax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$.LC7, %eax
	movsd	-40(%rbp), %xmm0
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf
	movq	Iterations(%rip), %rdx
	movq	seqIterations(%rip), %rcx
	movl	$.LC8, %eax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$t3, %esi
	movl	$0, %edi
	call	clock_gettime
	movq	$0, idx(%rip)
	jmp	.L12
.L15:
	movq	fib0(%rip), %rdx
	movq	fib1(%rip), %rax
	addq	%rdx, %rax
	movq	%rax, fib(%rip)
	jmp	.L13
.L14:
	movq	fib1(%rip), %rax
	movq	%rax, fib0(%rip)
	movq	fib(%rip), %rax
	movq	%rax, fib1(%rip)
	movq	fib0(%rip), %rdx
	movq	fib1(%rip), %rax
	addq	%rdx, %rax
	movq	%rax, fib(%rip)
	movq	jdx(%rip), %rax
	addq	$1, %rax
	movq	%rax, jdx(%rip)
.L13:
	movq	jdx(%rip), %rdx
	movq	seqIterations(%rip), %rax
	cmpq	%rax, %rdx
	jb	.L14
	movq	idx(%rip), %rax
	addq	$1, %rax
	movq	%rax, idx(%rip)
.L12:
	movq	idx(%rip), %rdx
	movq	Iterations(%rip), %rax
	cmpq	%rax, %rdx
	jb	.L15
	movl	$t4, %esi
	movl	$0, %edi
	call	clock_gettime
	movq	t3(%rip), %rdx
	movl	$.LC9, %eax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movq	t4(%rip), %rdx
	movl	$.LC10, %eax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movq	t4(%rip), %rdx
	movq	t3(%rip), %rax
	movq	%rdx, %rcx
	subq	%rax, %rcx
	movq	%rcx, %rax
	imulq	$1000000000, %rax, %rax
	movq	t4+8(%rip), %rcx
	movq	t3+8(%rip), %rdx
	movq	%rcx, %rbx
	subq	%rdx, %rbx
	movq	%rbx, %rdx
	addq	%rdx, %rax
	movq	%rax, cycleCnt(%rip)
	movq	fib(%rip), %rcx
	movq	fib(%rip), %rdx
	movq	seqIterations(%rip), %rsi
	movl	$.LC11, %eax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movq	cycleCnt(%rip), %rdx
	movl	$.LC12, %eax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movl	$.LC13, %eax
	movq	-24(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	movq	cycleCnt(%rip), %rax
	testq	%rax, %rax
	js	.L16
	cvtsi2sdq	%rax, %xmm0
	jmp	.L17
.L16:
	movq	%rax, %rdx
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	cvtsi2sdq	%rdx, %xmm0
	addsd	%xmm0, %xmm0
.L17:
	movq	-24(%rbp), %rax
	testq	%rax, %rax
	js	.L18
	cvtsi2sdq	%rax, %xmm1
	jmp	.L19
.L18:
	movq	%rax, %rdx
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	cvtsi2sdq	%rdx, %xmm1
	addsd	%xmm1, %xmm1
.L19:
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -32(%rbp)
	movl	$.LC14, %eax
	movsd	-32(%rbp), %xmm0
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf
	addq	$72, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC5:
	.long	0
	.long	1104006501
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits
