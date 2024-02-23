// `include "./define.v"

`define AND     3'b000
`define XOR     3'b001
`define SLL     3'b010
`define ADD     3'b011
`define SUB     3'b100
`define MUL     3'b101
`define ADDI    3'b110
`define SRAI    3'b111
`define LW_SW   3'b011
`define BEQ     3'b100

module ALU_Control (
    funct7,
    funct3,
    ALU_Op,
    ALUControl_o
);

input   [6:0]   funct7;
input   [2:0]   funct3;
input   [1:0]   ALU_Op;
output  [2:0]   ALUControl_o;

assign ALUControl_o = 
    (ALU_Op == 2'b10) ?                     // R-type
        (
            (funct3 == 3'b111) ? `AND :
            (funct3 == 3'b100) ? `XOR :
            (funct3 == 3'b001) ? `SLL :
            (funct7 == 7'b0000000 && funct3 == 3'b000) ? `ADD :
            (funct7 == 7'b0100000 && funct3 == 3'b000) ? `SUB : `MUL
        )
    : 
    (ALU_Op == 2'b00) ?                     // I-type, lw, rw
        (
            (funct3 == 3'b000) ? `ADDI : 
            (funct3 == 3'b101) ? `SRAI : `LW_SW
        )
    : `BEQ;     // beq
    
endmodule