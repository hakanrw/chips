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
        [[A_IMM,M_R_],[A_IMM,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_]],

        # TSX         # INS        # PULA       # PULB       # DES        # TXS        # PSHA       # PSHB       # --         # RTS        # --         # RTI        # --         # --         # WAI        # SWI
        [[A____,M___],[A____,M___],[A____,M_R_],[A____,M_R_],[A____,M___],[A____,M___],[A____,M__W],[A____,M__W],[A____,M___],[A____,M_R_],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___]],
    ],
    # aa = 01
    [
        # NEGA        # --         # --         # COMA       # LSRA       # --         # RORA       # ASRA       # ASLA       # ROLA       # DECA       # --         # INCA       # TSTA       # --         # CLRA
        [[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___]],
        # NEGB        # --         # --         # COMB       # LSRB       # --         # RORB       # ASRB       # ASLB       # ROLB       # DECB       # --         # INCB       # TSTB       # --         # CLRB
        [[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___],[A____,M___]],

        # NEG         # --         # --         # COM        # LSR        # --         # ROR        # ASR        # ASL        # ROL        # DEC        # --         # INC        # TST        # JMP        # CLR
        [[A_IDX,M_RW],[A____,M___],[A____,M___],[A_IDX,M_RW],[A_IDX,M_RW],[A____,M___],[A_IDX,M_RW],[A_IDX,M_RW],[A_IDX,M_RW],[A_IDX,M_RW],[A_IDX,M_RW],[A____,M___],[A_IDX,M_RW],[A_IDX,M_R_],[A_IMM,M_R_],[A_IDX,M__W]],
        [[A_ABS,M_RW],[A____,M___],[A____,M___],[A_ABS,M_RW],[A_ABS,M_RW],[A____,M___],[A_ABS,M_RW],[A_ABS,M_RW],[A_ABS,M_RW],[A_ABS,M_RW],[A_ABS,M_RW],[A____,M___],[A_ABS,M_RW],[A_ABS,M_R_],[A_I16,M_R_],[A_ABS,M__W]],
    ],
    # aa = 10
    [
        # SUBA        # CMPA       # SBCA       # --         # ANDA       # BITA       # LDAA       # STAA       # EORA       # ADCA       # ORAA       # ADDA       # CPX        # BSR/JSR    # LDS        # STS
        [[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A____,M___],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_IMM,M_R_],[A_I16,M_R_],[A____,M___]],
        [[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A____,M___],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M__W],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A_ZER,M_R_],[A____,M___],[A_ZER,M_R_],[A_ZER,M__W]],
        [[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A____,M___],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M__W],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IDX,M_R_],[A_IMM,M_R_],[A_IDX,M_R_],[A_IDX,M__W]],
        [[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A____,M___],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M__W],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_ABS,M_R_],[A_I16,M_R_],[A_ABS,M_R_],[A_ABS,M__W]],
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
        self.src = [None] * 16
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
        op.t('c->AD=_GD()+c->IX;_VMA();')  # these two lines are made inefficient on
        op.t('_SA(c->AD);_VMA();')         # purpose to match mc6800 cycle count
        op.t('')
        if mem_access in [M__W, M_RW]:
            op.ta('_VMA();') # VMA is low, calculating address
            if mem_access == M_RW:
                op.t('') # wait on next instruction for read fetch
    else:
        # invalid instruction
        pass

#-------------------------------------------------------------------------------
def i_nop(o):
    cmt(o,'NOP')
    o.t('')

#-------------------------------------------------------------------------------
def i_tap(o):
    cmt(o,'TAP')
    o.t('c->P=0xC0|c->A;') # most significant two bits of P is 1

#-------------------------------------------------------------------------------
def i_tpa(o):
    cmt(o,'TPA')
    o.t('c->A=c->P;')

#-------------------------------------------------------------------------------
def i_inx(o):
    cmt(o,'INX')
    o.t('c->IX++;_ZF(c->IX);')

#-------------------------------------------------------------------------------
def i_dex(o):
    cmt(o,'DEX')
    o.t('c->IX--;_ZF(c->IX);')

#-------------------------------------------------------------------------------
def i_clv(o):
    cmt(o,'CLV')
    o.t('_VF(false);')

#-------------------------------------------------------------------------------
def i_sev(o):
    cmt(o,'SEV')
    o.t('_VF(true);')

#-------------------------------------------------------------------------------
def i_clc(o):
    cmt(o,'CLC')
    o.t('_CF(false);')

#-------------------------------------------------------------------------------
def i_sec(o):
    cmt(o,'SEC')
    o.t('_CF(true);')

#-------------------------------------------------------------------------------
def i_cli(o):
    cmt(o,'CLI')
    o.t('_IF(false);')

#-------------------------------------------------------------------------------
def i_sei(o):
    cmt(o,'SEI')
    o.t('_IF(true);')

#-------------------------------------------------------------------------------
def i_sba(o):
    cmt(o,'SBA')
    o.t('c->A=_mc6800_sub(c, c->A, c->B, false);')

#-------------------------------------------------------------------------------
def i_cba(o):
    cmt(o,'CBA')
    o.t('_mc6800_sub(c, c->A, c->B, false);')

#-------------------------------------------------------------------------------
def i_tab(o):
    cmt(o,'TAB')
    o.t('_VF(false);c->B=c->A;_NZ(c->B);')

#-------------------------------------------------------------------------------
def i_tba(o):
    cmt(o,'TBA')
    o.t('_VF(false);c->A=c->B;_NZ(c->A);')

#-------------------------------------------------------------------------------
def i_daa(o):
    cmt(o,'DAA')
    o.t('_mc6800_daa(c);')

#-------------------------------------------------------------------------------
def i_aba(o):
    cmt(o,'ABA')
    o.t('c->A=_mc6800_add(c, c->A, c->B, false);')

#-------------------------------------------------------------------------------
def i_bra(o):
    cmt(o,'BRA')
    o.t('c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_bhi(o):
    cmt(o,'BHI')
    o.t('if(!(c->P&MC6800_CF)&&!(c->P&MC6800_ZF))c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_bls(o):
    cmt(o,'BLS')
    o.t('if((c->P&MC6800_CF)||(c->P&MC6800_ZF))c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_bcc(o):
    cmt(o,'BCC')
    o.t('if(!(c->P&MC6800_CF))c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_bcs(o):
    cmt(o,'BCS')
    o.t('if(c->P&MC6800_CF)c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_bne(o):
    cmt(o,'BNE')
    o.t('if(!(c->P&MC6800_ZF))c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_beq(o):
    cmt(o,'BEQ')
    o.t('if(c->P&MC6800_ZF)c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_bvc(o):
    cmt(o,'BVC')
    o.t('if(!(c->P&MC6800_VF))c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_bvs(o):
    cmt(o,'BVS')
    o.t('if(c->P&MC6800_VF)c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_bpl(o):
    cmt(o,'BPL')
    o.t('if(!(c->P&MC6800_NF))c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_bmi(o):
    cmt(o,'BMI')
    o.t('if(c->P&MC6800_NF)c->PC+=(int8_t)_GD();')

#-------------------------------------------------------------------------------
def i_tsx(o):
    cmt(o,'TSX')
    o.t('c->IX=c->SP+1;')
    o.t('_VMA();')
    o.t('_VMA();')

#-------------------------------------------------------------------------------
def i_ins(o):
    cmt(o,'INS')
    o.t('c->SP++;')
    o.t('_VMA();')
    o.t('_VMA();')

#-------------------------------------------------------------------------------
def i_pulx(o,x):
    cmt(o,'PUL'+x)
    o.t(f'c->SP++;_VMA();')
    o.t(f'_SA(c->SP);_VMA();')
    o.t(f'') # reads here
    o.t(f'c->{x}=_GD();')

#-------------------------------------------------------------------------------
def i_des(o):
    cmt(o,'DES')
    o.t('c->SP--;')
    o.t('_VMA();')
    o.t('_VMA();')

#-------------------------------------------------------------------------------
def i_txs(o):
    cmt(o,'TXS')
    o.t('c->SP=c->IX-1;')
    o.t('_VMA();')
    o.t('_VMA();')

#-------------------------------------------------------------------------------
def i_pshx(o,x):
    cmt(o,'PSH'+x)
    o.t(f'_SA(c->SP);_VMA();')
    o.t(f'_SD(c->{x});_WR();')
    o.t(f'c->SP--;_VMA();')

#-------------------------------------------------------------------------------
def i_rts(o):
    cmt(o,'RTS')
    o.t('c->SP++;_SA(c->SP);_VMA();')
    o.t('') # reads here
    o.t('c->PC=_GD()<<8;c->SP++;_SA(c->SP);')
    o.t('') # reads here
    o.t('c->PC|=_GD();')

#-------------------------------------------------------------------------------
# internal push mechanism of interrupt handlers
def _i_swi_push(o,val):
    o.ta(f'_SA(c->SP);_SD({val});_WR();')
    o.t('c->SP--;')

#-------------------------------------------------------------------------------    
# internal pull mechanism of interrupt handlers
def _i_swi_pull(o):
    o.ta(f'c->SP++;_SA(c->SP);')

#-------------------------------------------------------------------------------
def i_rti(o):
    cmt(o,'RTI')
    o.t('') # used by first o.ta of _i_swi_pull
    _i_swi_pull(o)
    o.t('c->P=_GD();')
    _i_swi_pull(o)
    o.t('c->B=_GD();')
    _i_swi_pull(o)
    o.t('c->A=_GD();')
    _i_swi_pull(o)
    o.t('c->IX=_GD()<<8;')
    _i_swi_pull(o)
    o.t('c->IX|=_GD();')
    _i_swi_pull(o)
    o.t('c->PC=_GD()<<8;')
    _i_swi_pull(o)
    o.t('c->PC|=_GD();_VMA();')
    o.t('') # match mc6800 cycle count

#-------------------------------------------------------------------------------
def i_swi(o):
    cmt(o,'SWI')
    o.t('') # used by first o.ta of _i_swi_push
    _i_swi_push(o,"c->PC")
    _i_swi_push(o,"c->PC>>8")
    _i_swi_push(o,"c->IX")
    _i_swi_push(o,"c->IX>>8")
    _i_swi_push(o,"c->A")
    _i_swi_push(o,"c->B")
    _i_swi_push(o,"c->P")
    o.ta('_IF(true);_VMA();')
    o.t('_SA(0xFFFF-5);')
    o.t('c->PC=_GD()<<8;_SA(0xFFFF-4);')
    o.t('c->PC|=_GD();')

#-------------------------------------------------------------------------------
def i_negx(o,x):
    cmt(o,'NEG'+x)
    o.t(f'_VF(c->{x}==0x80);_CF(c->{x}!=0x00);c->{x}=~c->{x}+1;_NZ(c->{x});')

#-------------------------------------------------------------------------------
def i_neg(o):
    cmt(o,'NEG')
    o.t('_VF(_GD()==0x80);_CF(_GD()!=0x00);_SD(~_GD()+1);_NZ(_GD());_WR();')

#-------------------------------------------------------------------------------
def i_comx(o,x):
    cmt(o,'COM'+x)
    o.t(f'_VF(false);_CF(true);c->{x}=~c->{x};_NZ(c->{x});')

#-------------------------------------------------------------------------------
def i_com(o):
    cmt(o,'COM')
    o.t('_VF(false);_CF(true);_SD(~_GD());_NZ(_GD());_WR();')

#-------------------------------------------------------------------------------
def i_lsrx(o,x):
    cmt(o,'LSR'+x)
    o.t(f'c->{x}=_mc6800_lsr(c, c->{x});')

#-------------------------------------------------------------------------------
def i_lsr(o):
    cmt(o,'LSR')
    o.t('_SD(_mc6800_lsr(c, _GD()));_WR();')

#-------------------------------------------------------------------------------
def i_rorx(o,x):
    cmt(o,'ROR'+x)
    o.t(f'c->{x}=_mc6800_ror(c, c->{x});')

#-------------------------------------------------------------------------------
def i_ror(o):
    cmt(o,'ROR')
    o.t('_SD(_mc6800_ror(c, _GD()));_WR();')

#-------------------------------------------------------------------------------
def i_asrx(o,x):
    cmt(o,'ASR'+x)
    o.t(f'c->{x}=(_GD()&0x80)|_mc6800_lsr(c, c->{x});_NZ(c->{x});')

#-------------------------------------------------------------------------------
def i_asr(o):
    cmt(o,'ASR')
    o.t('_SD((_GD()&0x80)|_mc6800_lsr(c, _GD()));_NZ(_GA());_WR();')

#-------------------------------------------------------------------------------
def i_aslx(o,x):
    cmt(o,'ASL'+x)
    o.t(f'c->{x}=_mc6800_asl(c, c->{x});')

#-------------------------------------------------------------------------------
def i_asl(o):
    cmt(o,'ASL')
    o.t('_SD(_mc6800_asl(c, _GD()));_WR();')

#-------------------------------------------------------------------------------
def i_rolx(o,x):
    cmt(o,'ROL'+x)
    o.t(f'c->{x}=_mc6800_rol(c, c->{x});')

#-------------------------------------------------------------------------------
def i_rol(o):
    cmt(o,'ROL')
    o.t('_SD(_mc6800_rol(c, _GD()));_WR();')

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
def i_tstx(o,x):
    cmt(o,'TST'+x)
    o.t(f'_mc6800_sub(c, c->{x}, 0, false);')

#-------------------------------------------------------------------------------
def i_tst(o):
    cmt(o,'TST')
    o.t('_VF(false);_CF(false);_NZ(_GD());')

#-------------------------------------------------------------------------------
def i_jmpidx(o):
    cmt(o,'JMP')
    o.t('c->AD=_GD()+c->IX;_VMA();')
    o.t('_VMA();')
    o.t('c->PC=c->AD;')

#-------------------------------------------------------------------------------
def i_jmpabs(o):
    cmt(o,'JMP')
    o.t('c->AD=_GD();c->AD<<=8;_SA(_GA()+1);')
    o.t('c->PC=c->AD|_GD();')

#-------------------------------------------------------------------------------
def i_clrx(o,x):
    cmt(o,'CLR'+x)
    o.t(f'_VF(false);_CF(false);c->{x}=0;_NZ(0);')

#-------------------------------------------------------------------------------
def i_clr(o):
    cmt(o,'CLR')
    o.t('_VF(false);_CF(false);_NZ(0);_SD(0);_WR();')

#-------------------------------------------------------------------------------
def i_subx(o,x):
    cmt(o,'SUB'+x)
    o.t(f'c->{x}=_mc6800_sub(c, c->{x}, _GD(), false);');

#-------------------------------------------------------------------------------
def i_cmpx(o,x):
    cmt(o,'CMP'+x)
    o.t(f'_mc6800_cmp(c, c->{x}, _GD());')

#-------------------------------------------------------------------------------
def i_sbcx(o,x):
    cmt(o,'SBC'+x)
    o.t(f'c->{x}=_mc6800_sub(c, c->{x}, _GD(), true);');

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
    o.t(f'c->{x}=_mc6800_add(c, c->{x}, _GD(), true);');

#-------------------------------------------------------------------------------
def i_orax(o,x):
    cmt(o,'ORA'+x)
    o.t(f'_VF(false);c->{x}|=_GD();_NZ(c->{x});')

#-------------------------------------------------------------------------------
def i_addx(o,x):
    cmt(o,'ADD'+x)
    o.t(f'c->{x}=_mc6800_add(c, c->{x}, _GD(), false);');

#-------------------------------------------------------------------------------
def i_cpx(o):
    cmt(o,'CPX')
    o.t('c->AD=_GD();c->AD<<=8;_SA(_GA()+1);')
    o.t('c->AD=c->AD|_GD();_mc6800_cpx(c, c->AD);')

#-------------------------------------------------------------------------------
def i_bsr(o):
    cmt(o,'BSR')
    o.t('c->AD=c->PC+(int8_t)_GD();_SA(c->SP);_VMA();')
    o.t('_SD(c->PC);_WR();')
    o.t('c->SP--;_VMA();')
    o.t('_SA(c->SP);_VMA();')
    o.t('_SD(c->PC>>8);_WR();')
    o.t('c->SP--;_VMA();')
    o.t('c->PC=c->AD;')

#-------------------------------------------------------------------------------
def i_jsridx(o):
    cmt(o,'JSR')
    o.t('c->AD=_GD()+c->IX;_SA(c->SP);_VMA();')
    o.t('_SD(c->PC);_WR();')
    o.t('c->SP--;_VMA();')
    o.t('_SA(c->SP);_VMA();')
    o.t('_SD(c->PC>>8);_WR();')
    o.t('c->SP--;_VMA();')
    o.t('c->PC=c->AD;')

#-------------------------------------------------------------------------------
def i_jsrabs(o):
    cmt(o,'JSR')
    o.t('c->AD=_GD();c->AD<<=8;_SA(_GA()+1);')
    o.t('c->AD=c->AD|_GD();_SA(c->SP);_VMA();')
    o.t('_SD(c->PC);_WR();')
    o.t('c->SP--;_VMA();')
    o.t('_SA(c->SP);_VMA();')
    o.t('_SD(c->PC>>8);_WR();')
    o.t('c->SP--;_VMA();')
    o.t('c->PC=c->AD;')

#-------------------------------------------------------------------------------
def i_lds(o):
    cmt(o,'LDS')
    o.t(f'c->SP=_GD();c->SP<<=8;_SA(_GA()+1);');
    o.t(f'_VF(false);c->SP|=_GD();_NZ16(c->SP);');

#-------------------------------------------------------------------------------
def i_ldx(o):
    cmt(o,'LDX')
    o.t(f'c->IX=_GD();c->IX<<=8;_SA(_GA()+1);');
    o.t(f'_VF(false);c->IX|=_GD();_NZ16(c->IX);');

#-------------------------------------------------------------------------------
def i_sts(o):
    cmt(o,'STS')
    o.t(f'_NZ16(c->SP);_SD(c->SP>>8);_WR();');
    o.t(f'_VF(false);_SA(_GA()+1);_SD(c->SP);_WR();');

#-------------------------------------------------------------------------------
def i_stx(o):
    cmt(o,'STX')
    o.t(f'_NZ16(c->IX);_SD(c->IX>>8);_WR();');
    o.t(f'_VF(false);_SA(_GA()+1);_SD(c->IX);_WR();');

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
        if bb == 0:
            if cccc == 0:        i_nop(o)
            elif cccc == 1:      i_nop(o)       # NOP
            elif cccc == 2:      i_nop(o)
            elif cccc == 3:      i_nop(o)
            elif cccc == 4:      i_nop(o)
            elif cccc == 5:      i_nop(o)
            elif cccc == 6:      i_tap(o)       # TAP
            elif cccc == 7:      i_tpa(o)       # TPA
            elif cccc == 8:      i_inx(o)       # INX
            elif cccc == 9:      i_dex(o)       # DEX
            elif cccc == 10:     i_clv(o)       # CLV
            elif cccc == 11:     i_sev(o)       # SEV
            elif cccc == 12:     i_clc(o)       # CLC
            elif cccc == 13:     i_sec(o)       # SEC
            elif cccc == 14:     i_cli(o)       # CLI
            elif cccc == 15:     i_sei(o)       # SEI
        elif bb == 1:
            if cccc == 0:        i_sba(o)       # SBA
            elif cccc == 1:      i_cba(o)       # CBA
            elif cccc == 2:      i_nop(o)
            elif cccc == 3:      i_nop(o)
            elif cccc == 4:      i_nop(o)
            elif cccc == 5:      i_nop(o)
            elif cccc == 6:      i_tab(o)       # TAB
            elif cccc == 7:      i_tba(o)       # TBA
            elif cccc == 8:      i_nop(o)
            elif cccc == 9:      i_daa(o)       # DAA TODO
            elif cccc == 10:     i_nop(o)
            elif cccc == 11:     i_aba(o)       # ABA
            elif cccc == 12:     i_nop(o)
            elif cccc == 13:     i_nop(o)
            elif cccc == 14:     i_nop(o)
            elif cccc == 15:     i_nop(o)
        elif bb == 2:
            if cccc == 0:        i_bra(o)       # BRA
            elif cccc == 1:      i_nop(o)
            elif cccc == 2:      i_bhi(o)       # BHI
            elif cccc == 3:      i_bls(o)       # BLS
            elif cccc == 4:      i_bcc(o)       # BCC
            elif cccc == 5:      i_bcs(o)       # BCS
            elif cccc == 6:      i_bne(o)       # BNE
            elif cccc == 7:      i_beq(o)       # BEQ
            elif cccc == 8:      i_bvc(o)       # BVC
            elif cccc == 9:      i_bvs(o)       # BVS
            elif cccc == 10:     i_bpl(o)       # BPL
            elif cccc == 11:     i_bmi(o)       # BMI
            elif cccc == 12:     i_nop(o)       # BGE TODO
            elif cccc == 13:     i_nop(o)       # BLT TODO
            elif cccc == 14:     i_nop(o)       # BGT TODO
            elif cccc == 15:     i_nop(o)       # BLE TODO
        elif bb == 3:
            if cccc == 0:        i_tsx(o)       # TSX
            elif cccc == 1:      i_ins(o)       # INS
            elif cccc == 2:      i_pulx(o, "A") # PULA
            elif cccc == 3:      i_pulx(o, "B") # PULB
            elif cccc == 4:      i_des(o)       # DES
            elif cccc == 5:      i_txs(o)       # TXS
            elif cccc == 6:      i_pshx(o, "A") # PSHA
            elif cccc == 7:      i_pshx(o, "B") # PSHB
            elif cccc == 8:      i_nop(o)
            elif cccc == 9:      i_rts(o)       # RTS
            elif cccc == 10:     i_nop(o)
            elif cccc == 11:     i_rti(o)       # RTI
            elif cccc == 12:     i_nop(o)
            elif cccc == 13:     i_nop(o)
            elif cccc == 14:     i_nop(o)       # WAI TODO
            elif cccc == 15:     i_swi(o)       # SWI
    elif aa == 1:
        if cccc == 0:        # NEG
            if bb == 0:          i_negx(o, "A")
            elif bb == 1:        i_negx(o, "B")
            elif bb == 2:        i_neg(o)
            elif bb == 3:        i_neg(o)
        elif cccc == 3:      # COM
            if bb == 0:          i_comx(o, "A")
            elif bb == 1:        i_comx(o, "B")
            elif bb == 2:        i_com(o)
            elif bb == 3:        i_com(o)
        elif cccc == 4:      # LSR
            if bb == 0:          i_lsrx(o, "A")
            elif bb == 1:        i_lsrx(o, "B")
            elif bb == 2:        i_lsr(o)
            elif bb == 3:        i_lsr(o)
        elif cccc == 6:      # ROR
            if bb == 0:          i_rorx(o, "A")
            elif bb == 1:        i_rorx(o, "B")
            elif bb == 2:        i_ror(o)
            elif bb == 3:        i_ror(o)
        elif cccc == 7:      # ASR
            if bb == 0:          i_asrx(o, "A")
            elif bb == 1:        i_asrx(o, "B")
            elif bb == 2:        i_asr(o)
            elif bb == 3:        i_asr(o)
        elif cccc == 8:      # ASL
            if bb == 0:          i_aslx(o, "A")
            elif bb == 1:        i_aslx(o, "B")
            elif bb == 2:        i_asl(o)
            elif bb == 3:        i_asl(o)
        elif cccc == 9:      # ROL
            if bb == 0:          i_rolx(o, "A")
            elif bb == 1:        i_rolx(o, "B")
            elif bb == 2:        i_rol(o)
            elif bb == 3:        i_rol(o)
        elif cccc == 10:     # DEC
            if bb == 0:          i_decx(o, "A")
            elif bb == 1:        i_decx(o, "B")
            elif bb == 2:        i_dec(o)
            elif bb == 3:        i_dec(o)
        elif cccc == 12:     # INC
            if bb == 0:          i_incx(o, "A")
            elif bb == 1:        i_incx(o, "B")
            elif bb == 2:        i_inc(o)
            elif bb == 3:        i_inc(o)
        elif cccc == 13:     # TST
            if bb == 0:          i_tstx(o, "A")
            elif bb == 1:        i_tstx(o, "B")
            elif bb == 2:        i_tst(o)
            elif bb == 3:        i_tst(o)
        elif cccc == 14:     # JMP
            if bb == 0:          i_nop(o)
            elif bb == 1:        i_nop(o)
            elif bb == 2:        i_jmpidx(o)
            elif bb == 3:        i_jmpabs(o)
        elif cccc == 15:     # CLR
            if bb == 0:          i_clrx(o, "A")
            elif bb == 1:        i_clrx(o, "B")
            elif bb == 2:        i_clr(o)
            elif bb == 3:        i_clr(o)
        else:                i_nop(o)
    else:
        accx = aa == 2
        acc = "A" if accx else "B"
        if cccc == 0:        i_subx(o,acc)      # SUB
        if cccc == 1:        i_cmpx(o,acc)      # CMP
        if cccc == 2:        i_sbcx(o,acc)      # SBC
        if cccc == 3:        i_nop(o)
        if cccc == 4:        i_andx(o,acc)      # AND
        if cccc == 5:        i_bitx(o,acc)      # BIT
        if cccc == 6:        i_ldax(o,acc)      # LDA
        if cccc == 7:
            if bb == 0:        i_nop(o)
            else:              i_stax(o,acc)    # STA
        if cccc == 8:        i_eorx(o,acc)      # EOR
        if cccc == 9:        i_adcx(o,acc)      # ADC
        if cccc == 10:       i_orax(o,acc)      # ORA
        if cccc == 11:       i_addx(o,acc)      # ADD
        if cccc == 12:       i_cpx(o)           # CPX
        if cccc == 13:       
            if bb == 0:        i_bsr(o)         # BSR
            elif bb == 1:      i_nop(o)
            elif bb == 2:      i_jsridx(o)      # JSR
            elif bb == 3:      i_jsrabs(o)
        if cccc == 14:
            if aa == 2:        i_lds(o)         # LDS
            else:              i_ldx(o)         # LDX
        if cccc == 15:
            if bb == 0:        i_nop(o)
            elif aa == 2:      i_sts(o)         # STS
            else:              i_stx(o)         # STX

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
# fix wrong cycle count on TST (must be 6 on abs, 7 on idx)
# implement last 4 branch operations
# implement bsr/jsr
# check cycle counts for all instructions
