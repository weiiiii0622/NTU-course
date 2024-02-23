module ID_EXE (
    clk_i,
    rst_i,
    RegWrite_i,
    MemtoReg_i,
    MemRead_i,
    MemWrite_i,
    ALU_Op_i,
    ALU_Src_i,
    Reg_data1_i,
    Reg_data2_i,
    Imm_gen_i,
    funct3_i,
    funct7_i,
    Reg_rs1_i,
    Reg_rs2_i,
    Reg_rd_i,

    RegWrite_o,
    MemtoReg_o,
    MemRead_o,
    MemWrite_o,
    ALU_Op_o,
    ALU_Src_o,
    Reg_data1_o,
    Reg_data2_o,
    Imm_gen_o,
    funct3_o,
    funct7_o,
    Reg_rs1_o,
    Reg_rs2_o,
    Reg_rd_o
);

input               clk_i;
input               rst_i;
input               RegWrite_i;
input               MemtoReg_i;
input               MemRead_i;
input               MemWrite_i;
input       [1:0]   ALU_Op_i;
input               ALU_Src_i;
input       [31:0]  Reg_data1_i;
input       [31:0]  Reg_data2_i;
input       [31:0]  Imm_gen_i;
input       [2:0]   funct3_i;
input       [6:0]   funct7_i;
input       [4:0]   Reg_rs1_i;
input       [4:0]   Reg_rs2_i;
input       [4:0]   Reg_rd_i;

output reg          RegWrite_o;
output reg          MemtoReg_o;
output reg          MemRead_o;
output reg          MemWrite_o;
output reg  [1:0]   ALU_Op_o;
output reg          ALU_Src_o;
output reg  [31:0]  Reg_data1_o;
output reg  [31:0]  Reg_data2_o;
output reg  [31:0]  Imm_gen_o;
output reg  [2:0]   funct3_o;
output reg  [6:0]   funct7_o;
output reg  [4:0]   Reg_rs1_o;
output reg  [4:0]   Reg_rs2_o;
output reg  [4:0]   Reg_rd_o;

always@(posedge clk_i or negedge rst_i) begin
    if (~rst_i) begin
        RegWrite_o <= 1'b0;
        MemtoReg_o <= 1'b0;
        MemRead_o <= 1'b0;
        MemWrite_o <= 1'b0;
        ALU_Op_o <= 2'b0;
        ALU_Src_o <= 1'b0;
        Reg_data1_o <= 32'b0;
        Reg_data2_o <= 32'b0;
        Imm_gen_o <= 32'b0;
        funct3_o <= 3'b0;
        funct7_o <= 7'b0;
        Reg_rs1_o <= 5'b0;
        Reg_rs2_o <= 5'b0;
        Reg_rd_o <= 5'b0;
    end
    else begin
        RegWrite_o <= RegWrite_i;
        MemtoReg_o <= MemtoReg_i;
        MemRead_o <= MemRead_i;
        MemWrite_o <= MemWrite_i;
        ALU_Op_o <= ALU_Op_i;
        ALU_Src_o <= ALU_Src_i;
        Reg_data1_o <= Reg_data1_i;
        Reg_data2_o <= Reg_data2_i;
        Imm_gen_o <= Imm_gen_i;
        funct3_o <= funct3_i;
        funct7_o <= funct7_i;
        Reg_rs1_o <= Reg_rs1_i;
        Reg_rs2_o <= Reg_rs2_i;
        Reg_rd_o <= Reg_rd_i;
    end
end
    
endmodule