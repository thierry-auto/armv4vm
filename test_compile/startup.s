.global reset
reset:

        LDR sp, =ld_stack_top   /* Initialize the stack */
        BL 	main            /* Call the main */
        SWI     2    /* Stop the virtual machine */
