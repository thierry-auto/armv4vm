	.cpu arm7tdmi
	.arch armv4t
	.fpu vfp
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 0
	.eabi_attribute 18, 4
	.file	"float_test1.cpp"
	.text
	.align	2
	.global	_Z8functionRKfRf
	.syntax unified
	.arm
	.type	_Z8functionRKfRf, %function
_Z8functionRKfRf:
	.fnstart
.LFB0:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	fp, [sp, #-4]!
	add	fp, sp, #0
	sub	sp, sp, #12
	str	r0, [fp, #-8]
	str	r1, [fp, #-12]
	ldr	r3, [fp, #-8]
	vldr.32	s14, [r3]
	ldr	r3, [fp, #-12]
	vldr.32	s15, [r3]
	vadd.f32	s15, s14, s15
	vcvt.f64.f32	d7, s15
	vldr.64	d6, .L2
	vadd.f64	d7, d7, d6
	vcvt.f32.f64	s15, d7
	ldr	r3, [fp, #-12]
	vstr.32	s15, [r3]
	nop
	add	sp, fp, #0
	@ sp needed
	ldr	fp, [sp], #4
	bx	lr
.L3:
	.align	3
.L2:
	.word	-1553060174
	.word	1077361665
	.cantunwind
	.fnend
	.size	_Z8functionRKfRf, .-_Z8functionRKfRf
	.ident	"GCC: (crosstool-NG 1.28.0_rc1) 15.2.0"
