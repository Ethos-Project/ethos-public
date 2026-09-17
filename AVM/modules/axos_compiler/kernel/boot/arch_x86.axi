# x86_64 Hardware Initialization

@Packed
struct GDT_Descriptor:
    public limit: uint16
    public base: uint64

@Packed
struct GDT_Entry:
    public limit_low: uint16
    public base_low: uint16
    public base_middle: uint8
    public access: uint8
    public granularity: uint8
    public base_high: uint8

@Packed
struct IDT_Descriptor:
    public limit: uint16
    public base: uint64

@Packed
struct IDT_Entry:
    public offset_low: uint16
    public selector: uint16
    public ist: uint8
    public type_attr: uint8
    public offset_mid: uint16
    public offset_high: uint32
    public zero: uint32

@Packed
struct PML4_Table:
    public entries_base: uint64

state master_gdt_ptr: GDT_Descriptor = @Zero_Init @Align(4096)
state master_idt_ptr: IDT_Descriptor = @Zero_Init @Align(4096)
state master_pml4_ptr: PML4_Table = @Zero_Init @Align(4096)

node init_gdt(gdt_ptr: GDT_Descriptor) -> GDT_Descriptor:
    @C_Native
    """
    // The FLUX compiler will pass gdt_ptr by reference automatically
    __asm__ __volatile__("lgdt %0" : : "m" (*gdt_ptr));
    """

node init_idt(idt_ptr: IDT_Descriptor) -> IDT_Descriptor:
    @C_Native
    """
    __asm__ __volatile__("lidt %0" : : "m" (*idt_ptr));
    """

node enable_paging(pml4_ptr: PML4_Table) -> PML4_Table:
    @C_Native
    """
    // Write PML4 base address to CR3
    __asm__ __volatile__("mov %0, %%cr3" : : "r" (pml4_ptr->entries_base));
    """
