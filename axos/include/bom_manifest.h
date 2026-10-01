// generated 2026-09-25T20:13:47Z — do not hand-edit
#ifndef BOM_MANIFEST_H
#define BOM_MANIFEST_H

#include <stdio.h>
#include <string.h>

static inline void print_bom_manifest(const char* cli_name) {
    if (cli_name && cli_name[0]) {
        printf("%s=0.1.0\n", cli_name);
    }
    printf("allforge.package_manager=0.1.0\n");
    printf("allforge.root=0.1.0\n");
    printf("axi.avm=0.1.0\n");
    printf("axi.bin.avm_cli=0.1.0\n");
    printf("axi.bin.axi_cli=0.1.0\n");
    printf("axi.bin.axi_dll=0.1.0\n");
    printf("axi.dvcs=0.1.0\n");
    printf("axi.lang=0.1.0\n");
    printf("axi.repo_meta=0.1.0\n");
    printf("axos.axos_lang=0.1.0\n");
    printf("axos.bin.avmc_cli=0.1.0\n");
    printf("axos.bin.axi_cli=0.1.0\n");
    printf("axos.bin.libaxi_dll=0.1.0\n");
    printf("axos.ide.app=0.1.0\n");
    printf("axos.ide.axi_dll=0.1.0\n");
    printf("axos.packaging.installer=0.1.0\n");
    printf("axos.stdlib.core=0.1.0\n");
}

#endif // BOM_MANIFEST_H
