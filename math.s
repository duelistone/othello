    .text

/*====================================================================
 * int min(int x, int y)
 */
.globl _min
_min:
	cmpl	%esi, %edi
	movl	%esi, %eax
	cmovle	%edi, %eax
	ret

/*====================================================================
 * int max(int x, int y)
 */
.globl _max
_max:
	cmpl	%esi, %edi
	movl	%esi, %eax
	cmovge	%edi, %eax
	ret

.globl _other_side
_other_side:
	cmpl	$0, %edi
	sete	%al
	movzbl	%al, %eax
	ret
