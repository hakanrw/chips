#-------------------------------------------------------------------------------
#   mc6800_gen.py
#   Generate instruction decoder for mc6800.h emulator.
#-------------------------------------------------------------------------------
from string import Template

InpPath = 'mc6800.template.h'
OutPath = '../chips/mc6800.h'

# flag bits
CF = (1<<0)
VF = (1<<1)
ZF = (1<<2)
NF = (1<<3)
IF = (1<<4)
HF = (1<<5)
XF = (1<<6)
YF = (1<<7)

def flag_name(f):
    if f == CF: return 'C'
    elif f == VF: return 'V'
    elif f == ZF: return 'Z'
    elif f == NF: return 'N'
    elif f == IF: return 'I'
    elif f == HF: return 'H'
    elif f == XF: return 'X'
    elif f == YF: return 'Y'

def branch_name(m, v):
    # TODO
    if m == NF:
        return 'BPL' if v==0 else 'BMI'
    elif m == VF:
        return 'BVC' if v==0 else 'BVS'
    elif m == CF:
        return 'BCC' if v==0 else 'BCS'
    elif m == ZF:
        return 'BNE' if v==0 else 'BEQ'
    # TODO

# addressing mode constants
A____ = 0       # no addressing mode
A_IMM = 1       # immediate
A_ZER = 2       # zero-page
#A_ZPX = 3       # zp,X
#A_ZPY = 4       # zp,Y
A_ABS = 3       # abs
#A_ABX = 6       # abs,X
#A_ABY = 7       # abs,Y
A_IDX = 4       # (zp, X)
#A_IDY = 9       # (zp),Y
#A_JMP = 10      # special JMP abs
#A_JSR = 11      # special JSR abs
A_INV = 5      # an invalid instruction

# addressing mode strings
addr_mode_str = ['', '#', 'zp', 'abs', '(zp,X)', 'INVALID']

# memory access modes
M___ = 0        # no memory access
M_R_ = 1        # read access
M__W = 2        # write access
M_RW = 3        # read-modify-write

# addressing-modes and memory accesses for each instruction
# TODO fix memory access modes (not implemented correctly yet)
ops = [
    # aa = 00
    [
        # --          # NOP        # --         # --         # --         # --         # TAP        # TPA        # INX        # DEX        # CLV        # SEV        # CLC        # SEC        # CLI        # SEI
        [[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___]],

        # SBA         # CBA        # --         # --         # --         # --         # TAB        # TBA        # --         # DAA        # --         # ABA        # --         # --         # --         # --
        [[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___]],

        # BRA         # --         # BHI        # BLS        # BCC        # BCS        # BNE        # BEQ        # BVC        # BVS        # BPL        # BMI        # BGE        # BLT        # BGT        # BLE
        [[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___],[A_IMM,M___]],

        # TSX         # INS        # PULA       # PULB       # DES        # TXS        # PSHA       # PSHB       # --         # RTS        # --         # RTI        # --         # --         # WAI        # SWI
        [[A____,M___],[A____,M___],[A____,M_R_],[A____,M_R_],[A____,M___],[A____,M___],[A____,M__W],[A____,M__W],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___]],
    ],
    # aa = 01
    [
        # NEGA        # --         # --         # COMA       # LSRA       # --         # RORA       # ASRA       # ASLA       # ROLA       # DECA       # --         # INCA       # TSTA       # --         # CLRA
        [[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___]],
        # NEGB        # --         # --         # COMB       # LSRB       # --         # RORB       # ASRB       # ASLB       # ROLB       # DECB       # --         # INCB       # TSTB       # --         # CLRB
        [[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___]],

        # NEG         # --         # --         # COM        # LSR        # --         # ROR        # ASR        # ASL        # ROL        # DEC        # --         # INC        # TST        # JMP        # CLR
        [[A_IDX,M_R_],[A____,M___],[A____,M___],[A_IDX,M_R_],[A_IDX,M__W],[A____,M___],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_RW],[A____,M___],[A_IDX,M_RW],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_]],
        [[A_ABS,M_R_],[A____,M___],[A____,M___],[A_ABS,M_R_],[A_ABS,M__W],[A____,M___],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_RW],[A____,M___],[A_ABS,M_RW],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_]],
    ],
    # aa = 10
    [
        # SUBA        # CMPA       # SBCA       # --         # ANDA       # BITA       # LDAA       # STAA       # EORA       # ADCA       # ORAA       # ADDA       # CPXA       # BSR/JSR    # LDS        # STS
        [[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___]],
        [[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A____,M___],[A_ZER,M__W],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M__W],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M__W],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_]],
        [[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A____,M___],[A_IDX,M__W],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M__W],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M__W],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_]],
        [[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A____,M___],[A_ABS,M__W],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M__W],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M__W],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_]],
    ],
    # aa = 11
    [
        # SUBB        # CMPB       # SBCB       # --         # ANDB       # BITB       # LDAB       # STAB       # EORB       # ADCB       # ORAB       # ADDB       # --        # --         # LDX        # STX
        [[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A____,M___],[A_IMM,M_R_],[A____,M___]],
        [[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A____,M___],[A_ZER,M__W],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M__W],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A____,M___],[A____,M___],[A_ZER,M_R_],[A_ZER,M_R_]],
        [[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A____,M___],[A_IDX,M__W],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M__W],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A____,M___],[A____,M___],[A_IDX,M_R_],[A_IDX,M_R_]],
        [[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A____,M___],[A_ABS,M__W],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M__W],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A____,M___],[A____,M___],[A_ABS,M_R_],[A_ABS,M_R_]],
    ]
]

class opcode:
    def __init__(self, op):
        self.code = op
        self.cmt = None
        self.i = 0
        self.src = [None] * 8
    def t(self, src):
        self.src[self.i] = src
        self.i += 1
    def ta(self, src):
        self.src[self.i-1] += src

#-------------------------------------------------------------------------------
#   output a src line
#
out_lines = ''
def l(s) :
    global out_lines
    out_lines += s + '\n'

#-------------------------------------------------------------------------------
def write_op(op):
    if not op.cmt:
        op.cmt = '???'
    l('    /* {} */'.format(op.cmt if op.cmt else '???'))
    for t in range(0, 16):
        if t < op.i:
            l('        case (0x{:02X}<<4)|{}: {}break;'.format(op.code, t, op.src[t]))
        else:
            l('        case (0x{:02X}<<4)|{}: assert(false);break;'.format(op.code, t))

#-------------------------------------------------------------------------------
def cmt(o,cmd):
    cccc = op & 15
    bb = (op>>4) & 3
    aa = (op>>6) & 3
    addr_mode = ops[aa][bb][cccc][0]
    o.cmt = cmd;
    if addr_mode != '':
        o.cmt += ' '+addr_mode_str[addr_mode]

#-------------------------------------------------------------------------------
def u_cmt(o,cmd):
    cmt(o,cmd)
    o.cmt += ' (undoc)'

#-------------------------------------------------------------------------------
def invalid_opcode(op):
    cccc = op & 15
    bb = (op>>4) & 3
    aa = (op>>6) & 3
    addr_mode = ops[aa][bb][cccc][0]
    return addr_mode == A_INV

#-------------------------------------------------------------------------------
def enc_addr(op, addr_mode, mem_access):
    if addr_mode == A____:
        # no addressing
        pass
    elif addr_mode == A_IMM:
        # immediate mode
        op.t('_SA(c->PC++);')
    elif addr_mode == A_ZER:
        # zero page
        op.t('_SA(c->PC++);')
        op.t('_SA(_GD());')
        if mem_access in [M__W, M_RW]:
            op.ta('_VMA();') # VMA is low, calculating address
            if mem_access == M_RW:
                op.t('') # wait on next instruction for read fetch
    elif addr_mode == A_ABS:
        # absolute
        op.t('_SA(c->PC++);')
        op.t('_SA(c->PC++);c->AD=_GD();')
        op.t('_SA(_GD()|(c->AD<<8));')
        if mem_access in [M__W, M_RW]:
            op.ta('_VMA();') # VMA is low, calculating address
            if mem_access == M_RW:
                op.t('') # wait on next instruction for read fetch
    elif addr_mode == A_IDX:
        # (zp,X)
        op.t('_SA(c->PC++);')
        op.t('c->AD=_GD();_SA(c->AD);')
        op.t('c->AD=(c->AD+c->IX)&0xFF;_SA(c->AD);')
        op.t('_SA((c->AD+1)&0xFF);c->AD=_GD();')
        op.t('_SA((_GD()<<8)|c->AD);')
        # TODO
        pass
    else:
        # invalid instruction
        pass

#-------------------------------------------------------------------------------
def i_nop(o):
    cmt(o,'NOP')
    o.t('')

#-------------------------------------------------------------------------------
def i_swi(o):
    cmt(o, 'SWI')
    o.t('1')
    o.t('2')
    o.t('3')
    o.t('4')
    o.t('5')
    o.t('6')
    o.t('7')
    o.t('8')
    o.t('9')
    o.t('10')
    o.t('11')

#-------------------------------------------------------------------------------
def i_deca(o):
    cmt(o, 'DECA')
    o.t('_VF(c->A,0x80);c->A--;_NZ(c->A);')

#-------------------------------------------------------------------------------
def i_decb(o):
    cmt(o, 'DECB')
    o.t('_VF(c->B,0x80);c->B--;_NZ(c->B);')

#-------------------------------------------------------------------------------
def i_dec(o):
    cmt(o, 'DEC')
    o.t('_VF(_GD(),0x80);c->AD=_GD()-1;_NZ(c->AD);_SD(c->AD);_WR();')

#-------------------------------------------------------------------------------
def i_inca(o):
    cmt(o, 'INCA')
    o.t('_VF(c->A,0x7F);c->A++;_NZ(c->A);')

#-------------------------------------------------------------------------------
def i_incb(o):
    cmt(o, 'INCB')
    o.t('_VF(c->A,0x7F);c->B++;_NZ(c->B);')

#-------------------------------------------------------------------------------
def i_inc(o):
    cmt(o, 'INC')
    o.t('_VF(_GD(),0x7F);c->AD=_GD()+1;_NZ(c->AD);_SD(c->AD);_WR();')

#-------------------------------------------------------------------------------
def i_suba(o):
    cmt(o, 'SUBA')
    o.t('_mc6800_sub(c, _GD(), true, false);');

#-------------------------------------------------------------------------------
def i_subb(o):
    cmt(o, 'SUBB')
    o.t('_mc6800_sub(c, _GD(), false, false);');

#-------------------------------------------------------------------------------
def i_sbca(o):
    cmt(o, 'SBCA')
    o.t('_mc6800_sub(c, _GD(), true, true);');

#-------------------------------------------------------------------------------
def i_sbcb(o):
    cmt(o, 'SBCB')
    o.t('_mc6800_sub(c, _GD(), false, true);');

#-------------------------------------------------------------------------------
def i_ldaa(o):
    cmt(o,'LDAA')
    o.t('c->A=_GD();_NZ(c->A);')

#-------------------------------------------------------------------------------
def i_ldab(o):
    cmt(o,'LDAB')
    o.t('c->B=_GD();_NZ(c->B);')

#-------------------------------------------------------------------------------
def i_staa(o):
    cmt(o,'STAA')
    o.t('_SD(c->A);_WR();')

#-------------------------------------------------------------------------------
def i_stab(o):
    cmt(o,'STAB')
    o.t('_SD(c->B);_WR();')

#-------------------------------------------------------------------------------
def i_adca(o):
    cmt(o, 'ADCA')
    o.t('_mc6800_add(c, _GD(), true, true);');

#-------------------------------------------------------------------------------
def i_adcb(o):
    cmt(o, 'ADCB')
    o.t('_mc6800_add(c, _GD(), false, true);');

#-------------------------------------------------------------------------------
def i_adda(o):
    cmt(o, 'ADDA')
    o.t('_mc6800_add(c, _GD(), true, false);');

#-------------------------------------------------------------------------------
def i_addb(o):
    cmt(o, 'ADDB')
    o.t('_mc6800_add(c, _GD(), false, false);');

#-------------------------------------------------------------------------------
def enc_op(op):
    o = opcode(op)
    if invalid_opcode(op):
        x_jam(o);
        return o

    # decode the opcode byte
    cccc = op & 15
    bb = (op>>4) & 3
    aa = (op>>6) & 3
    addr_mode = ops[aa][bb][cccc][0]
    mem_access = ops[aa][bb][cccc][1]
    # addressing mode
    enc_addr(o, addr_mode, mem_access);
    # actual instruction
    if aa == 0:
        i_nop(o)
    elif aa == 1:
        if cccc == 10:       # DEC
            if bb == 0:        i_deca(o)
            elif bb == 1:        i_decb(o)
            elif bb == 2:        i_dec(o)
            elif bb == 3:        i_dec(o)
        elif cccc == 12:     # INC
            if bb == 0:        i_inca(o)
            elif bb == 1:        i_incb(o)
            elif bb == 2:        i_inc(o)
            elif bb == 3:        i_inc(o)
        else:                i_nop(o)
    elif aa == 2:
        if cccc == 0:        i_suba(o)
        if cccc == 1:        i_nop(o)
        if cccc == 2:        i_sbca(o)
        if cccc == 3:        i_nop(o)
        if cccc == 4:        i_nop(o)
        if cccc == 5:        i_nop(o)
        if cccc == 6:        i_ldaa(o)
        if cccc == 7:
            if bb == 0:        i_nop(o)
            else:              i_staa(o)
        if cccc == 8:        i_nop(o)
        if cccc == 9:        i_adca(o)
        if cccc == 10:       i_nop(o)
        if cccc == 11:       i_adda(o)
        if cccc == 12:       i_nop(o)
        if cccc == 13:       i_nop(o)
        if cccc == 14:       i_nop(o)
        if cccc == 15:       i_nop(o)
    elif aa == 3:
        if cccc == 0:        i_subb(o)
        if cccc == 1:        i_nop(o)
        if cccc == 2:        i_sbcb(o)
        if cccc == 3:        i_nop(o)
        if cccc == 4:        i_nop(o)
        if cccc == 5:        i_nop(o)
        if cccc == 6:        i_ldab(o)
        if cccc == 7:
            if bb == 0:        i_nop(o)
            else:              i_stab(o)
        if cccc == 8:        i_nop(o)
        if cccc == 9:        i_adcb(o)
        if cccc == 10:       i_nop(o)
        if cccc == 11:       i_addb(o)
        if cccc == 12:       i_nop(o)
        if cccc == 13:       i_nop(o)
        if cccc == 14:       i_nop(o)
        if cccc == 15:       i_nop(o)

    # since read operation is actually done on clock
    # phase 2 but we required another cycle,
    # we make up for the lost cycle by early-loading the
    # next instruction
    if mem_access == M_R_:
        o.ta('_FETCH();')
    else:
        if mem_access == M___:
            o.ta('_VMA();') # no memory instruction
        o.t('_FETCH();')
    return o

#-------------------------------------------------------------------------------
#   execution starts here
#
for op in range(0, 256):
    write_op(enc_op(op))

with open(InpPath, 'r') as inf:
    templ = Template(inf.read())
    c_src = templ.safe_substitute(decode_block=out_lines)
    with open(OutPath, 'w') as outf:
        outf.write(c_src)
