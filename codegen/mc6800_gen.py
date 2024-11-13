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
A_ABS = 3       # abs
A_IDX = 4       # (zp, X)
A_I16 = 5       # immediate 16 bits
#A_JMP = 10      # special JMP abs
#A_JSR = 11      # special JSR abs
A_INV = 6      # an invalid instruction

# addressing mode strings
addr_mode_str = ['', '#', 'zp', 'abs', '(zp,X)', '#16', 'INVALID']

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
        [[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_I16,M_R_],[A____,M___]],
        [[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A____,M___],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M__W],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M__W],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M__W]],
        [[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A____,M___],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M__W],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M__W],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M__W]],
        [[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A____,M___],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M__W],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M__W],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M__W]],
    ],
    # aa = 11
    [
        # SUBB        # CMPB       # SBCB       # --         # ANDB       # BITB       # LDAB       # STAB       # EORB       # ADCB       # ORAB       # ADDB       # --        # --         # LDX        # STX
        [[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A____,M___],[A_I16,M_R_],[A____,M___]],
        [[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A____,M___],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M__W],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A____,M___],[A____,M___],[A_ZER,M_R_],[A_ZER,M__W]],
        [[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A____,M___],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M__W],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A____,M___],[A____,M___],[A_IDX,M_R_],[A_IDX,M__W]],
        [[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A____,M___],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M__W],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A____,M___],[A____,M___],[A_ABS,M_R_],[A_ABS,M__W]],
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
    elif addr_mode == A_I16:
        # immediate mode 16 bits
        op.t('_SA(c->PC++);c->PC++;')
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
def i_decx(o,x):
    cmt(o,'DEC'+x)
    o.t(f'_VF(c->{x}==0x80);c->{x}--;_NZ(c->{x});')

#-------------------------------------------------------------------------------
def i_dec(o):
    cmt(o,'DEC')
    o.t('_VF(_GD()==0x80);c->AD=_GD()-1;_NZ(c->AD);_SD(c->AD);_WR();')

#-------------------------------------------------------------------------------
def i_incx(o,x):
    cmt(o,'INC'+x)
    o.t(f'_VF(c->{x}==0x7F);c->{x}++;_NZ(c->{x});')

#-------------------------------------------------------------------------------
def i_inc(o):
    cmt(o,'INC')
    o.t('_VF(_GD()==0x7F);c->AD=_GD()+1;_NZ(c->AD);_SD(c->AD);_WR();')

#-------------------------------------------------------------------------------
def i_subx(o,x):
    cmt(o,'SUB'+x)
    o.t(f'_mc6800_sub(c, _GD(), {"true" if x == "A" else "false"}, false);');

#-------------------------------------------------------------------------------
def i_cmpx(o,x):
    cmt(o,'CMP'+x)
    o.t(f'_mc6800_cmp(c, _GD(), {"true" if x == "A" else "false"});')

#-------------------------------------------------------------------------------
def i_sbcx(o,x):
    cmt(o,'SBC'+x)
    o.t(f'_mc6800_sub(c, _GD(), {"true" if x == "A" else "false"}, true);');

#-------------------------------------------------------------------------------
def i_andx(o,x):
    cmt(o,'AND'+x)
    o.t(f'_VF(false);c->{x}&=_GD();_NZ(c->{x});')

#-------------------------------------------------------------------------------
def i_bitx(o,x):
    cmt(o,'BIT'+x)
    o.t(f'_VF(false);_NZ(c->{x}&_GD());')

#-------------------------------------------------------------------------------
def i_ldax(o,x):
    cmt(o,'LDA'+x)
    o.t(f'_VF(false);c->{x}=_GD();_NZ(c->{x});')

#-------------------------------------------------------------------------------
def i_stax(o,x):
    cmt(o,'STA'+x)
    o.t(f'_VF(false);_SD(c->{x});_NZ(c->{x});_WR();')

#-------------------------------------------------------------------------------
def i_eorx(o,x):
    cmt(o,'EOR'+x)
    o.t(f'_VF(false);c->{x}^=_GD();_NZ(c->{x});')

#-------------------------------------------------------------------------------
def i_adcx(o,x):
    cmt(o,'ADC'+x)
    o.t(f'_mc6800_add(c, _GD(), {"true" if x == "A" else "false"}, true);');

#-------------------------------------------------------------------------------
def i_orax(o,x):
    cmt(o,'ORA'+x)
    o.t(f'_VF(false);c->{x}|=_GD();_NZ(c->{x});')

#-------------------------------------------------------------------------------
def i_addx(o,x):
    cmt(o,'ADD'+x)
    o.t(f'_mc6800_add(c, _GD(), {"true" if x == "A" else "false"}, false);');

#-------------------------------------------------------------------------------
def i_lds(o):
    cmt(o,'LDS')
    o.t(f'c->SP=_GD()<<8;_SA(_GA()+1);');
    o.t(f'_VF(false);c->SP|=_GD();_NZ16(c->SP);');

#-------------------------------------------------------------------------------
def i_ldx(o):
    cmt(o,'LDX')
    o.t(f'c->IX=_GD()<<8;_SA(_GA()+1);');
    o.t(f'_VF(false);c->IX|=_GD();_NZ16(c->IX);');

#-------------------------------------------------------------------------------
def i_sts(o):
    cmt(o,'STS')
    o.t(f'_NZ16(c->SP);_SD(c->SP>>8);_WR();');
    o.t(f'_VF(false);_SA(_GA()+1);_SD(c->SP&0xFF);_WR();');

#-------------------------------------------------------------------------------
def i_stx(o):
    cmt(o,'STX')
    o.t(f'_NZ16(c->IX);_SD(c->IX>>8);_WR();');
    o.t(f'_VF(false);_SA(_GA()+1);_SD(c->IX&0xFF);_WR();');

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
            if bb == 0:        i_decx(o, "A")
            elif bb == 1:        i_decx(o, "B")
            elif bb == 2:        i_dec(o)
            elif bb == 3:        i_dec(o)
        elif cccc == 12:     # INC
            if bb == 0:        i_incx(o, "A")
            elif bb == 1:        i_incx(o, "B")
            elif bb == 2:        i_inc(o)
            elif bb == 3:        i_inc(o)
        else:                i_nop(o)
    else:
        accx = aa == 2
        acc = "A" if accx else "B"
        if cccc == 0:        i_subx(o,acc)
        if cccc == 1:        i_nop(o)
        if cccc == 2:        i_sbcx(o,acc)
        if cccc == 3:        i_cmpx(o,acc)
        if cccc == 4:        i_andx(o,acc)
        if cccc == 5:        i_bitx(o,acc)
        if cccc == 6:        i_ldax(o,acc)
        if cccc == 7:
            if bb == 0:        i_nop(o)
            else:              i_stax(o,acc)
        if cccc == 8:        i_eorx(o,acc)
        if cccc == 9:        i_adcx(o,acc)
        if cccc == 10:       i_orax(o,acc)
        if cccc == 11:       i_addx(o,acc)
        if cccc == 12:       i_nop(o)
        if cccc == 13:       i_nop(o)
        if cccc == 14:
            if aa == 2:        i_lds(o)
            else:              i_ldx(o)
        if cccc == 15:
            if bb == 0:        i_nop(o)
            elif aa == 2:        i_sts(o)
            else:        	 i_stx(o)

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


# TODO
# implement half-carry flag on sub/add/sbc/adc
