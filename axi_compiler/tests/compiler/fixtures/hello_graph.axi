@C_Include("<stdio.h>")

node emit_axi_marker() -> int:
    @C_Native
    """
    printf("AXI_OK\n");
    """

start -> emit_axi_marker
