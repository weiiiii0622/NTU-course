module Control (
    inst,
    ALU_Op,
    ALU_Src,
    RegWrite
);

input   [6:0]   inst;
output  [1:0]   ALU_Op;
output          ALU_Src;
output          RegWrite;

assign ALU_Op = (inst == 7'b0110011) ? 2'b00 : 2'b01;
assign ALU_Src = (inst == 7'b0110011) ? 1'b0 : 1'b1;
assign RegWrite = (inst == 7'b0110011 || inst == 7'b0010011) ? 1'b1 : 1'b0;

endmodule