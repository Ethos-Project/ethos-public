# ARM64 Hardware Initialization

@Packed
struct VBAR_EL1_Descriptor:
    public base: uint64

@Packed
struct TTBR0_EL1_Descriptor:
    public baddr: uint64

state master_vbar_ptr: VBAR_EL1_Descriptor = @Zero_Init @Align(2048)
state master_ttbr0_ptr: TTBR0_EL1_Descriptor = @Zero_Init @Align(4096)

node init_vector_table(vbar_ptr: VBAR_EL1_Descriptor) -> VBAR_EL1_Descriptor:
    @C_Native
    """
    // Write vector table base address to VBAR_EL1
    __asm__ __volatile__("msr vbar_el1, %0" : : "r" (vbar_ptr->base));
    __asm__ __volatile__("isb");
    """

node enable_mmu(ttbr0_ptr: TTBR0_EL1_Descriptor) -> TTBR0_EL1_Descriptor:
    @C_Native
    """
    // Write translation table base address to TTBR0_EL1
    __asm__ __volatile__("msr ttbr0_el1, %0" : : "r" (ttbr0_ptr->baddr));
    __asm__ __volatile__("isb");

    // Enable MMU by setting the M bit (bit 0) in SCTLR_EL1
    uint64 sctlr;
    __asm__ __volatile__("mrs %0, sctlr_el1" : "=r" (sctlr));
    sctlr |= 1;
    __asm__ __volatile__("msr sctlr_el1, %0" : : "r" (sctlr));
    __asm__ __volatile__("isb");
    """
