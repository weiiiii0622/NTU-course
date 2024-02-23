module Hazard_Detection (
    EXE_MemRead_i,
    EXE_rd_i,
    ID_rs1_i,
    ID_rs2_i,

    PCWrite_o,
    Stall_o,
    NoOp_o,
);

input           EXE_MemRead_i;
input   [4:0]   EXE_rd_i;
input   [4:0]   ID_rs1_i;
input   [4:0]   ID_rs2_i;

output          PCWrite_o;
output          Stall_o;
output          NoOp_o;

assign PCWrite_o = (EXE_MemRead_i && (EXE_rd_i == ID_rs1_i || EXE_rd_i == ID_rs2_i)) ? 1'b0  // load-use data hazard
                    :
                    1'b1;

assign Stall_o = (EXE_MemRead_i && (EXE_rd_i == ID_rs1_i || EXE_rd_i == ID_rs2_i)) ? 1'b1  // load-use data hazard
                    :
                    1'b0;

assign NoOp_o = (EXE_MemRead_i && (EXE_rd_i == ID_rs1_i || EXE_rd_i == ID_rs2_i)) ? 1'b1  // load-use data hazard
                    :
                    1'b0;

endmodule