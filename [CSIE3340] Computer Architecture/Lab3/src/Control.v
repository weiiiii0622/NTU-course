module Control (
    inst,
    NoOp,
    ALU_Op,
    ALU_Src,
    RegWrite,
    MemtoReg,
    MemRead,
    MemWrite,
    Branch_o
);

input   [6:0]   inst;
input           NoOp;
output  [1:0]   ALU_Op;
output          ALU_Src;
output          RegWrite;
output          MemtoReg;
output          MemRead;
output          MemWrite;
output          Branch_o;

assign ALU_Op = (NoOp == 1'b1) ? 2'b00             // NOP
                :
                (inst == 7'b0110011) ? 2'b10        // R-type
                :
                (inst == 7'b1100011) ? 2'b01        // beq
                :
                2'b00;                              // lw, rw, I-type

assign ALU_Src = (NoOp == 1'b1) ? 1'b0             // NOP
                :
                (inst == 7'b0110011 || inst == 7'b1100011) ? 1'b0   // R-type, beq
                :
                1'b1;                               // I-type, lw, rw
                
assign RegWrite = (NoOp == 1'b1) ? 1'b0            // NOP
                :
                (inst == 7'b0110011 || inst == 7'b0010011 || inst == 7'b0000011) ? 1'b1     // R-type, I-type, lw
                :
                1'b0;                               // sw, beq

assign MemtoReg = (NoOp == 1'b1) ? 1'b0            // NOP
                :
                (inst == 7'b0000011) ? 1'b1         // lw
                :
                1'b0;                               // R-type, I-type (sw, beq don't care)

assign MemRead = (NoOp == 1'b1) ? 1'b0             // NOP
                :
                (inst == 7'b0000011) ? 1'b1         // lw
                :
                1'b0;                               // R-type, I-type, sw, beq

assign MemWrite = (NoOp == 1'b1) ? 1'b0            // NOP
                :
                (inst == 7'b0100011) ? 1'b1         // sw
                :
                1'b0;                               // R-type, I-type, lw, beq

assign Branch_o = (NoOp == 1'b1) ? 1'b0              // NOP
                :
                (inst == 7'b1100011) ? 1'b1         // beq
                :
                1'b0;                               // R-type, I-type, lw, sw

endmodule