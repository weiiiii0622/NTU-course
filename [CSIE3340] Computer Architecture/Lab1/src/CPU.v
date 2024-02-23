module CPU
(
    clk_i, 
    rst_i,
);

// Ports
input           clk_i;
input           rst_i;


wire    [31:0]  PC_i;
wire    [31:0]  PC_o;
wire    [31:0]  inst;
wire    [1:0]   ALUOp;
wire            ALUSrc;
wire            RegWrite;
wire    [31:0]  RegData1;
wire    [31:0]  RegData2;
wire    [31:0]  SignExtended_Imd;
wire    [2:0]   ALUCtrl;
wire    [31:0]  ALURes;
wire            zero_o;
wire    [31:0]  MUX_ALU;


Control Control(
    .inst       (inst[6:0]),
    .ALU_Op     (ALUOp),
    .ALU_Src    (ALUSrc),
    .RegWrite   (RegWrite)
);

Adder Add_PC(
    .data1_i    (PC_o),
    .data2_i    (4),
    .data_o     (PC_i)
);

PC PC(
    .clk_i      (clk_i),
    .rst_i      (rst_i),
    .pc_i       (PC_i),
    .pc_o       (PC_o)
);

Instruction_Memory Instruction_Memory(
    .addr_i     (PC_o),
    .instr_o    (inst)
);

Registers Registers(
    .rst_i      (rst_i),
    .clk_i      (clk_i),
    .RS1addr_i  (inst[19:15]),
    .RS2addr_i  (inst[24:20]),
    .RDaddr_i   (inst[11:7]),
    .RDdata_i   (ALURes),
    .RegWrite_i (RegWrite),
    .RS1data_o  (RegData1),
    .RS2data_o  (RegData2)
);

MUX32 MUX_ALUSrc(
    .select     (ALUSrc),
    .data1_i    (RegData2),
    .data2_i    (SignExtended_Imd),
    .data_o     (MUX_ALU)
);

Sign_Extend Sign_Extend(
    .inst_i     (inst[31:20]),
    .inst_o     (SignExtended_Imd)
);
  
ALU ALU(
    .ALU_Control (ALUCtrl),
    .data1_i    (RegData1),
    .data2_i    (MUX_ALU),
    .zero_o     (zero_o),
    .res_o      (ALURes)
);

ALU_Control ALU_Control(
    .inst       ({ inst[31:25], inst[14:12] }),
    .ALU_Op     (ALUOp),
    .ALUControl_o (ALUCtrl)
);

endmodule

