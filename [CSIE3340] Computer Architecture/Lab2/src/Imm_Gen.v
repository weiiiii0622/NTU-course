module Imm_Gen (
    inst_i,
    inst_o
);

input   [31:0]  inst_i;
output  [31:0]  inst_o;

assign inst_o = (inst_i[6:0] == 7'b0010011) ? { {20{inst_i[31]}}, inst_i[31:20] }                   // I-type
                :
                (inst_i[6:0] == 7'b0000011) ? { {20{inst_i[31]}}, inst_i[31:20] }                   // lw
                :
                (inst_i[6:0] == 7'b0100011) ? { {20{inst_i[31]}}, inst_i[31:25], inst_i[11:7] }     // sw
                :
                { {20{inst_i[31]}}, inst_i[31], inst_i[7], inst_i[30:25], inst_i[11:8] };           // beq

endmodule