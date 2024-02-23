module MEM_WB(
    clk_i,
    rst_i,
    RegWrite_i,
    MemtoReg_i,
    ALU_res_i,
    Mem_Read_data_i,
    Reg_rd_i,
    RegWrite_o,
    MemtoReg_o,
    ALU_res_o,
    Mem_Read_data_o,
    Reg_rd_o
);

input               clk_i;
input               rst_i;
input               RegWrite_i;
input               MemtoReg_i;
input       [31:0]  ALU_res_i;
input       [31:0]  Mem_Read_data_i;
input       [4:0]   Reg_rd_i;

output reg          RegWrite_o;
output reg          MemtoReg_o;
output reg  [31:0]  ALU_res_o;
output reg  [31:0]  Mem_Read_data_o;
output reg  [4:0]   Reg_rd_o;

always@(posedge clk_i or negedge rst_i) begin
    if (~rst_i) begin
        RegWrite_o <= 1'b0;
        MemtoReg_o <= 1'b0;
        ALU_res_o <= 32'b0;
        Mem_Read_data_o <= 32'b0;
        Reg_rd_o <= 5'b0;
    end
    else begin
        RegWrite_o <= RegWrite_i;
        MemtoReg_o <= MemtoReg_i;
        ALU_res_o <= ALU_res_i;
        Mem_Read_data_o <= Mem_Read_data_i;
        Reg_rd_o <= Reg_rd_i;
    end
end
    
endmodule