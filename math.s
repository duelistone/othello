    .text

/*====================================================================
 * int min(int x, int y)
 */
.globl min
min:
	cmpl	%esi, %edi
	movl	%esi, %eax
	cmovle	%edi, %eax
	ret

/*====================================================================
 * int max(int x, int y)
 */
.globl max
max:
	cmpl	%esi, %edi
	movl	%esi, %eax
	cmovge	%edi, %eax
	ret

.globl other_side
other_side:
	cmpl	$0, %edi
	sete	%al
	movzbl	%al, %eax
	ret
