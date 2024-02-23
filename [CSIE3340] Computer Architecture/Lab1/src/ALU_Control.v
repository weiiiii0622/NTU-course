// `include "./define.v"

`define AND     3'b000
`define XOR     3'b001
`define SLL     3'b010
`define ADD     3'b011
`define SUB     3'b100
`define MUL     3'b101
`define ADDI    3'b110
`define SRAI    3'b111

module ALU_Control (
    inst,
    ALU_Op,
    ALUControl_o
);

input   [9:0]   inst;
input   [1:0]   ALU_Op;
output  [2:0]   ALUControl_o;

assign ALUControl_o = (ALU_Op == 2'b00) ? 
    (
        (inst == 10'b0000000111) ? `AND :
        (inst == 10'b0000000100) ? `XOR :
        (inst == 10'b0000000001) ? `SLL :
        (inst == 10'b0000000000) ? `ADD :
        (inst == 10'b0100000000) ? `SUB : `MUL
    )
: 
    (
        (inst == 10'b0100000101) ? `SRAI : `ADDI
    );
    
endmodule