; LEDT: native EDLIN-style editor. Public DOS calls only, no kernel addresses.
; Word-byte records: length followed by 0..254 unchanged bytes. See ledt.inc.
        CPU YOSIC-KAPPA
        JMP ed_start
        INCLUDE common.inc
        INCLUDE ledt.inc
        INCLUDE ledt-edit.inc
        INCLUDE ledt-io.inc
        END
