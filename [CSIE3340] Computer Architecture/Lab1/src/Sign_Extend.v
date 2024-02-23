module Sign_Extend (
    inst_i,
    inst_o
);

input   [11:0]  inst_i;
output  [31:0]  inst_o;

assign inst_o = { {20{inst_i[11]}}, inst_i };

endmodule