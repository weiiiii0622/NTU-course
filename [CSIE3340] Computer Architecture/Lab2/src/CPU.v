module CPU
(
    clk_i, 
    rst_i,
);

// Ports
input           clk_i;
input           rst_i;


// Hazard Detection
wire            PCWrite;
wire            Stall;
wire            NoOp;

Hazard_Detection Hazard_Detection(
    .EXE_MemRead_i      (EXE_MemRead),
    .EXE_rd_i           (EXE_Reg_rd),
    .ID_rs1_i           (ID_inst[19:15]),
    .ID_rs2_i           (ID_inst[24:20]),
    .PCWrite_o          (PCWrite),
    .Stall_o            (Stall),
    .NoOp_o             (NoOp)
); 

// Forwarding
wire    [1:0]   ForwardA;
wire    [1:0]   ForwardB;

Forwarding_Unit Forwarding_Unit(
    .EXE_rs1_i          (EXE_Reg_rs1),
    .EXE_rs2_i          (EXE_Reg_rs2),
    .MEM_RegWrite_i     (MEM_RegWrite),
    .MEM_rd_i           (MEM_Reg_rd),
    .WB_RegWrite_i      (WB_RegWrite),
    .WB_rd_i            (WB_Reg_rd),
    .ForwardA_o         (ForwardA),
    .ForwardB_o         (ForwardB)
);

// IF
wire            MUX_PC_select;
wire    [31:0]  PC_data0;
wire    [31:0]  PC_i;
wire    [31:0]  PC_o;
wire    [31:0]  inst;

assign MUX_PC_select = ID_FlushIF;

MUX32 MUX32_PC(
    .select         (MUX_PC_select),
    .data1_i        (PC_data0),
    .data2_i        (PC_data1),
    .data_o         (PC_i)
);

PC PC(
    .clk_i          (clk_i),
    .rst_i          (rst_i),
    .PCWrite_i      (PCWrite),
    .pc_i           (PC_i),
    .pc_o           (PC_o)
);

Adder Add_PC(
    .data1_i        (PC_o),
    .data2_i        (4),
    .data_o         (PC_data0)
);

Instruction_Memory Instruction_Memory(
    .addr_i         (PC_o),
    .instr_o        (inst)
);

// IF/ID Reg
wire    [31:0]  ID_PC;
wire    [31:0]  ID_inst;

IF_ID IF_ID(
    .clk_i          (clk_i),
    .rst_i          (rst_i),
    .inst_i         (inst),
    .pc_i           (PC_o),
    .stall_i        (Stall),
    .flush_i        (ID_FlushIF),
    .pc_o           (ID_PC),
    .inst_o         (ID_inst)
);

// ID
wire    [31:0]  PC_data1;
wire    [31:0]  ID_shift_imm;
wire            ID_RegWrite;
wire            ID_MemtoReg;
wire            ID_MemRead;
wire            ID_MemWrite;
wire    [1:0]   ID_ALUOp;
wire            ID_ALUSrc;
wire            Branch;
wire    [31:0]  ID_Reg_data1;
wire    [31:0]  ID_Reg_data2;
wire    [31:0]  ID_Imm;
wire            ID_FlushIF;

assign ID_shift_imm = (ID_Imm << 1);
assign ID_FlushIF = (ID_Reg_data1 == ID_Reg_data2) & Branch;

Adder Add_ID_PC(
    .data1_i        (ID_PC),
    .data2_i        (ID_shift_imm),
    .data_o         (PC_data1)
);

Control Control(
    .inst           (ID_inst[6:0]),
    .NoOp           (NoOp),
    .ALU_Op         (ID_ALUOp),
    .ALU_Src        (ID_ALUSrc),
    .RegWrite       (ID_RegWrite),
    .MemtoReg       (ID_MemtoReg),
    .MemRead        (ID_MemRead),
    .MemWrite       (ID_MemWrite),
    .Branch_o       (Branch)
);

Registers Registers(
    .rst_i          (rst_i),
    .clk_i          (clk_i),
    .RS1addr_i      (ID_inst[19:15]),
    .RS2addr_i      (ID_inst[24:20]),
    .RDaddr_i       (WB_Reg_rd),
    .RDdata_i       (WB_Write_data),
    .RegWrite_i     (WB_RegWrite),
    .RS1data_o      (ID_Reg_data1),
    .RS2data_o      (ID_Reg_data2)
);

Imm_Gen Imm_Gen(
    .inst_i         (ID_inst),
    .inst_o         (ID_Imm)
);

// ID/EXE Reg
wire            EXE_RegWrite;
wire            EXE_MemtoReg;
wire            EXE_MemRead;
wire            EXE_MemWrite;
wire    [1:0]   EXE_ALUOp;
wire            EXE_ALUSrc;
wire    [31:0]  EXE_Reg_data1;
wire    [31:0]  EXE_Reg_data2;
wire    [31:0]  EXE_Imm;
wire    [2:0]   EXE_funct3;
wire    [6:0]   EXE_funct7;
wire    [4:0]   EXE_Reg_rs1;
wire    [4:0]   EXE_Reg_rs2;
wire    [4:0]   EXE_Reg_rd;

ID_EXE ID_EXE (
    .clk_i          (clk_i),
    .rst_i          (rst_i),
    .RegWrite_i     (ID_RegWrite),
    .MemtoReg_i     (ID_MemtoReg),
    .MemRead_i      (ID_MemRead),
    .MemWrite_i     (ID_MemWrite),
    .ALU_Op_i       (ID_ALUOp),
    .ALU_Src_i      (ID_ALUSrc),
    .Reg_data1_i    (ID_Reg_data1),
    .Reg_data2_i    (ID_Reg_data2),
    .Imm_gen_i      (ID_Imm),
    .funct3_i       (ID_inst[14:12]),
    .funct7_i       (ID_inst[31:25]),
    .Reg_rs1_i      (ID_inst[19:15]),
    .Reg_rs2_i      (ID_inst[24:20]),
    .Reg_rd_i       (ID_inst[11:7]),
    .RegWrite_o     (EXE_RegWrite),
    .MemtoReg_o     (EXE_MemtoReg),
    .MemRead_o      (EXE_MemRead),
    .MemWrite_o     (EXE_MemWrite),
    .ALU_Op_o       (EXE_ALUOp),
    .ALU_Src_o      (EXE_ALUSrc),
    .Reg_data1_o    (EXE_Reg_data1),
    .Reg_data2_o    (EXE_Reg_data2),
    .Imm_gen_o      (EXE_Imm),
    .funct3_o       (EXE_funct3),
    .funct7_o       (EXE_funct7),
    .Reg_rs1_o      (EXE_Reg_rs1),
    .Reg_rs2_o      (EXE_Reg_rs2),
    .Reg_rd_o       (EXE_Reg_rd)
);

// EXE
wire    [31:0]  EXE_ALU_data1;
wire    [31:0]  EXE_ALU_tmp_data2;
wire    [31:0]  EXE_ALU_data2;
wire    [2:0]   EXE_ALUCtrl;
wire    [31:0]  EXE_ALU_res;

MUX32_4 MUX32_4_EXE_ALU_DATA1(
    .select         (ForwardA),
    .data1_i        (EXE_Reg_data1),
    .data2_i        (WB_Write_data),
    .data3_i        (MEM_ALU_res),
    .data4_i        (),
    .data_o         (EXE_ALU_data1)
);

MUX32_4 MUX32_4_EXE_ALU_tmp_DATA2(
    .select         (ForwardB),
    .data1_i        (EXE_Reg_data2),
    .data2_i        (WB_Write_data),
    .data3_i        (MEM_ALU_res),
    .data4_i        (),
    .data_o         (EXE_ALU_tmp_data2)
);

MUX32 MUX32_EXE_ALU_DATA2(
    .select         (EXE_ALUSrc),
    .data1_i        (EXE_ALU_tmp_data2),
    .data2_i        (EXE_Imm),
    .data_o         (EXE_ALU_data2)
);

ALU ALU(
    .ALU_Control    (EXE_ALUCtrl),
    .data1_i        (EXE_ALU_data1),
    .data2_i        (EXE_ALU_data2),
    .res_o          (EXE_ALU_res)
);

ALU_Control ALU_Control(
    .funct7         (EXE_funct7),
    .funct3         (EXE_funct3),
    .ALU_Op         (EXE_ALUOp),
    .ALUControl_o   (EXE_ALUCtrl)
);

// EXE/MEM Reg
wire            MEM_RegWrite;
wire            MEM_MemtoReg;
wire            MEM_MemRead;
wire            MEM_MemWrite;
wire    [31:0]  MEM_ALU_res;
wire    [31:0]  MEM_Write_data;
wire    [4:0]   MEM_Reg_rd;

EXE_MEM EXE_MEM(
    .clk_i          (clk_i),
    .rst_i          (rst_i),
    .RegWrite_i     (EXE_RegWrite),
    .MemtoReg_i     (EXE_MemtoReg),
    .MemRead_i      (EXE_MemRead),
    .MemWrite_i     (EXE_MemWrite),
    .ALU_res_i      (EXE_ALU_res),
    .Mem_Write_data_i   (EXE_ALU_tmp_data2),
    .Reg_rd_i       (EXE_Reg_rd),
    .RegWrite_o     (MEM_RegWrite),
    .MemtoReg_o     (MEM_MemtoReg),
    .MemRead_o      (MEM_MemRead),
    .MemWrite_o     (MEM_MemWrite),
    .ALU_res_o      (MEM_ALU_res),
    .Mem_Write_data_o   (MEM_Write_data),
    .Reg_rd_o       (MEM_Reg_rd)
);

// MEM
wire    [31:0]  MEM_Read_data;

Data_Memory Data_Memory(
    .clk_i          (clk_i), 
    .addr_i         (MEM_ALU_res), 
    .MemRead_i      (MEM_MemRead),
    .MemWrite_i     (MEM_MemWrite),
    .data_i         (MEM_Write_data),
    .data_o         (MEM_Read_data)
);

// MEM/WB Reg
wire            WB_RegWrite;
wire            WB_MemtoReg;
wire    [31:0]  WB_ALU_res;
wire    [31:0]  WB_Read_data;
wire    [4:0]   WB_Reg_rd;

MEM_WB MEM_WB(
    .clk_i          (clk_i),
    .rst_i          (rst_i),
    .RegWrite_i     (MEM_RegWrite),
    .MemtoReg_i     (MEM_MemtoReg),
    .ALU_res_i      (MEM_ALU_res),
    .Mem_Read_data_i    (MEM_Read_data),
    .Reg_rd_i       (MEM_Reg_rd),
    .RegWrite_o     (WB_RegWrite),
    .MemtoReg_o     (WB_MemtoReg),
    .ALU_res_o      (WB_ALU_res),
    .Mem_Read_data_o    (WB_Read_data),
    .Reg_rd_o       (WB_Reg_rd)
);

// WB
wire    [31:0]  WB_Write_data;

MUX32 MUX32_WB_REG_WRITE_DATA(
    .select         (WB_MemtoReg),
    .data1_i        (WB_ALU_res),
    .data2_i        (WB_Read_data),
    .data_o         (WB_Write_data)
);

endmodule