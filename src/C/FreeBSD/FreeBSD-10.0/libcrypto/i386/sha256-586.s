	# $FreeBSD: stable/10/secure/lib/libcrypto/i386/sha256-586.s 238405 2012-07-12 19:30:53Z jkim $
.file	"sha512-586.s"
.text
.globl	sha256_block_data_order
.type	sha256_block_data_order,@function
.align	16
sha256_block_data_order:
.L_sha256_block_data_order_begin:
	pushl	%ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	movl	20(%esp),%esi
	movl	24(%esp),%edi
	movl	28(%esp),%eax
	movl	%esp,%ebx
	call	.L000pic_point
.L000pic_point:
	popl	%ebp
	leal	.L001K256-.L000pic_point(%ebp),%ebp
	subl	$16,%esp
	andl	$-64,%esp
	shll	$6,%eax
	addl	%edi,%eax
	movl	%esi,(%esp)
	movl	%edi,4(%esp)
	movl	%eax,8(%esp)
	movl	%ebx,12(%esp)
.align	16
.L002loop:
	movl	(%edi),%eax
	movl	4(%edi),%ebx
	movl	8(%edi),%ecx
	movl	12(%edi),%edx
	bswap	%eax
	bswap	%ebx
	bswap	%ecx
	bswap	%edx
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	movl	16(%edi),%eax
	movl	20(%edi),%ebx
	movl	24(%edi),%ecx
	movl	28(%edi),%edx
	bswap	%eax
	bswap	%ebx
	bswap	%ecx
	bswap	%edx
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	movl	32(%edi),%eax
	movl	36(%edi),%ebx
	movl	40(%edi),%ecx
	movl	44(%edi),%edx
	bswap	%eax
	bswap	%ebx
	bswap	%ecx
	bswap	%edx
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	movl	48(%edi),%eax
	movl	52(%edi),%ebx
	movl	56(%edi),%ecx
	movl	60(%edi),%edx
	bswap	%eax
	bswap	%ebx
	bswap	%ecx
	bswap	%edx
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	addl	$64,%edi
	subl	$32,%esp
	movl	%edi,100(%esp)
	movl	(%esi),%eax
	movl	4(%esi),%ebx
	movl	8(%esi),%ecx
	movl	12(%esi),%edi
	movl	%ebx,4(%esp)
	movl	%ecx,8(%esp)
	movl	%edi,12(%esp)
	movl	16(%esi),%edx
	movl	20(%esi),%ebx
	movl	24(%esi),%ecx
	movl	28(%esi),%edi
	movl	%ebx,20(%esp)
	movl	%ecx,24(%esp)
	movl	%edi,28(%esp)
.align	16
.L00300_15:
	movl	92(%esp),%ebx
	movl	%edx,%ecx
	rorl	$14,%ecx
	movl	20(%esp),%esi
	xorl	%edx,%ecx
	rorl	$5,%ecx
	xorl	%edx,%ecx
	rorl	$6,%ecx
	movl	24(%esp),%edi
	addl	%ecx,%ebx
	xorl	%edi,%esi
	movl	%edx,16(%esp)
	movl	%eax,%ecx
	andl	%edx,%esi
	movl	12(%esp),%edx
	xorl	%edi,%esi
	movl	%eax,%edi
	addl	%esi,%ebx
	rorl	$9,%ecx
	addl	28(%esp),%ebx
	xorl	%eax,%ecx
	rorl	$11,%ecx
	movl	4(%esp),%esi
	xorl	%eax,%ecx
	rorl	$2,%ecx
	addl	%ebx,%edx
	movl	8(%esp),%edi
	addl	%ecx,%ebx
	movl	%eax,(%esp)
	movl	%eax,%ecx
	subl	$4,%esp
	orl	%esi,%eax
	andl	%esi,%ecx
	andl	%edi,%eax
	movl	(%ebp),%esi
	orl	%ecx,%eax
	addl	$4,%ebp
	addl	%ebx,%eax
	addl	%esi,%edx
	addl	%esi,%eax
	cmpl	$3248222580,%esi
	jne	.L00300_15
	movl	152(%esp),%ebx
.align	16
.L00416_63:
	movl	%ebx,%esi
	movl	100(%esp),%ecx
	rorl	$11,%esi
	movl	%ecx,%edi
	xorl	%ebx,%esi
	rorl	$7,%esi
	shrl	$3,%ebx
	rorl	$2,%edi
	xorl	%esi,%ebx
	xorl	%ecx,%edi
	rorl	$17,%edi
	shrl	$10,%ecx
	addl	156(%esp),%ebx
	xorl	%ecx,%edi
	addl	120(%esp),%ebx
	movl	%edx,%ecx
	addl	%edi,%ebx
	rorl	$14,%ecx
	movl	20(%esp),%esi
	xorl	%edx,%ecx
	rorl	$5,%ecx
	movl	%ebx,92(%esp)
	xorl	%edx,%ecx
	rorl	$6,%ecx
	movl	24(%esp),%edi
	addl	%ecx,%ebx
	xorl	%edi,%esi
	movl	%edx,16(%esp)
	movl	%eax,%ecx
	andl	%edx,%esi
	movl	12(%esp),%edx
	xorl	%edi,%esi
	movl	%eax,%edi
	addl	%esi,%ebx
	rorl	$9,%ecx
	addl	28(%esp),%ebx
	xorl	%eax,%ecx
	rorl	$11,%ecx
	movl	4(%esp),%esi
	xorl	%eax,%ecx
	rorl	$2,%ecx
	addl	%ebx,%edx
	movl	8(%esp),%edi
	addl	%ecx,%ebx
	movl	%eax,(%esp)
	movl	%eax,%ecx
	subl	$4,%esp
	orl	%esi,%eax
	andl	%esi,%ecx
	andl	%edi,%eax
	movl	(%ebp),%esi
	orl	%ecx,%eax
	addl	$4,%ebp
	addl	%ebx,%eax
	movl	152(%esp),%ebx
	addl	%esi,%edx
	addl	%esi,%eax
	cmpl	$3329325298,%esi
	jne	.L00416_63
	movl	352(%esp),%esi
	movl	4(%esp),%ebx
	movl	8(%esp),%ecx
	movl	12(%esp),%edi
	addl	(%esi),%eax
	addl	4(%esi),%ebx
	addl	8(%esi),%ecx
	addl	12(%esi),%edi
	movl	%eax,(%esi)
	movl	%ebx,4(%esi)
	movl	%ecx,8(%esi)
	movl	%edi,12(%esi)
	movl	20(%esp),%eax
	movl	24(%esp),%ebx
	movl	28(%esp),%ecx
	movl	356(%esp),%edi
	addl	16(%esi),%edx
	addl	20(%esi),%eax
	addl	24(%esi),%ebx
	addl	28(%esi),%ecx
	movl	%edx,16(%esi)
	movl	%eax,20(%esi)
	movl	%ebx,24(%esi)
	movl	%ecx,28(%esi)
	addl	$352,%esp
	subl	$256,%ebp
	cmpl	8(%esp),%edi
	jb	.L002loop
	movl	12(%esp),%esp
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret
.align	64
.L001K256:
.long	1116352408,1899447441,3049323471,3921009573
.long	961987163,1508970993,2453635748,2870763221
.long	3624381080,310598401,607225278,1426881987
.long	1925078388,2162078206,2614888103,3248222580
.long	3835390401,4022224774,264347078,604807628
.long	770255983,1249150122,1555081692,1996064986
.long	2554220882,2821834349,2952996808,3210313671
.long	3336571891,3584528711,113926993,338241895
.long	666307205,773529912,1294757372,1396182291
.long	1695183700,1986661051,2177026350,2456956037
.long	2730485921,2820302411,3259730800,3345764771
.long	3516065817,3600352804,4094571909,275423344
.long	430227734,506948616,659060556,883997877
.long	958139571,1322822218,1537002063,1747873779
.long	1955562222,2024104815,2227730452,2361852424
.long	2428436474,2756734187,3204031479,3329325298
.size	sha256_block_data_order,.-.L_sha256_block_data_order_begin
.byte	83,72,65,50,53,54,32,98,108,111,99,107,32,116,114,97
.byte	110,115,102,111,114,109,32,102,111,114,32,120,56,54,44,32
.byte	67,82,89,80,84,79,71,65,77,83,32,98,121,32,60,97
.byte	112,112,114,111,64,111,112,101,110,115,115,108,46,111,114,103
.byte	62,0
