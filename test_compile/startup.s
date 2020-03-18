.global reset
reset:
        LDR sp, =ld_stack_top
        BL 	main
        SWI     0
