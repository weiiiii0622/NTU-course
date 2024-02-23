module Forwarding_Unit (
    EXE_rs1_i,
    EXE_rs2_i,
    MEM_RegWrite_i,
    MEM_rd_i,
    WB_RegWrite_i,
    WB_rd_i,

    ForwardA_o,
    ForwardB_o
);

input   [4:0]    EXE_rs1_i;
input   [4:0]    EXE_rs2_i;
input            MEM_RegWrite_i;
input   [4:0]    MEM_rd_i;
input            WB_RegWrite_i;
input   [4:0]    WB_rd_i;

output  [1:0]    ForwardA_o;
output  [1:0]    ForwardB_o;

assign ForwardA_o = (MEM_RegWrite_i && MEM_rd_i != 5'b0 && MEM_rd_i == EXE_rs1_i) ? 2'b10   // EXE Hazard
                    :
                    (WB_RegWrite_i && WB_rd_i != 5'b0 && WB_rd_i == EXE_rs1_i) ? 2'b01  // MEM Hazard
                    :
                    2'b00;

assign ForwardB_o = (MEM_RegWrite_i && MEM_rd_i != 5'b0 && MEM_rd_i == EXE_rs2_i) ? 2'b10   // EXE Hazard
                    :
                    (WB_RegWrite_i && WB_rd_i != 5'b0 && WB_rd_i == EXE_rs2_i) ? 2'b01  // MEM Hazard
                    :
                    2'b00;
    
endmodule