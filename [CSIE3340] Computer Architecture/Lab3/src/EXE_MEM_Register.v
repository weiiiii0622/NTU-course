module EXE_MEM (
    clk_i,
    rst_i,
    RegWrite_i,
    MemtoReg_i,
    MemRead_i,
    MemWrite_i,
    ALU_res_i,
    Mem_Write_data_i,
    Reg_rd_i,

    RegWrite_o,
    MemtoReg_o,
    MemRead_o,
    MemWrite_o,
    ALU_res_o,
    Mem_Write_data_o,
    Reg_rd_o
);

input               clk_i;
input               rst_i;
input               RegWrite_i;
input               MemtoReg_i;
input               MemRead_i;
input               MemWrite_i;
input       [31:0]  ALU_res_i;
input       [31:0]  Mem_Write_data_i;
input       [4:0]   Reg_rd_i;

output reg          RegWrite_o;
output reg          MemtoReg_o;
output reg          MemRead_o;
output reg          MemWrite_o;
output reg  [31:0]  ALU_res_o;
output reg  [31:0]  Mem_Write_data_o;
output reg  [4:0]   Reg_rd_o;

always@(posedge clk_i or negedge rst_i) begin
    if (~rst_i) begin
        RegWrite_o <= 1'b0;
        MemtoReg_o <= 1'b0;
        MemRead_o <= 1'b0;
        MemWrite_o <= 1'b0;
        ALU_res_o <= 32'b0;
        Mem_Write_data_o <= 32'b0;
        Reg_rd_o <= 5'b0;
    end
    else begin
        RegWrite_o <= RegWrite_i;
        MemtoReg_o <= MemtoReg_i;
        MemRead_o <= MemRead_i;
        MemWrite_o <= MemWrite_i;
        ALU_res_o <= ALU_res_i;
        Mem_Write_data_o <= Mem_Write_data_i;
        Reg_rd_o <= Reg_rd_i;
    end
end
    
endmodule