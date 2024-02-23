// `include "./define.v"


`define AND     3'b000
`define XOR     3'b001
`define SLL     3'b010
`define ADD     3'b011
`define SUB     3'b100
`define MUL     3'b101
`define ADDI    3'b110
`define SRAI    3'b111

module ALU (
    ALU_Control,
    data1_i,
    data2_i,
    zero_o,
    res_o
);

input   [3:0]   ALU_Control;
input   [31:0]  data1_i;
input   [31:0]  data2_i;
output          zero_o;
output  [31:0]  res_o;


assign zero_o = (data1_i == data2_i) ? 1'b1 : 1'b0;
assign res_o = 
    (ALU_Control == `AND) ? (data1_i & data2_i) :
    (ALU_Control == `XOR) ? (data1_i ^ data2_i) :
    (ALU_Control == `SLL) ? (data1_i << data2_i) :
    (ALU_Control == `ADD) ? (data1_i + data2_i) :
    (ALU_Control == `SUB) ? (data1_i - data2_i) :
    (ALU_Control == `MUL) ? (data1_i * data2_i) :
    (ALU_Control == `ADDI) ? (data1_i + data2_i) : (data1_i >>> data2_i[4:0]);

    
endmodule