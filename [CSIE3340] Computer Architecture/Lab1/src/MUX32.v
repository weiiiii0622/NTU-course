module MUX32 (
    select,
    data1_i,
    data2_i,
    data_o
);

input           select;
input   [31:0]  data1_i;
input   [31:0]  data2_i;
output  [31:0]  data_o;

assign data_o = (select == 1'b0) ? data1_i : data2_i;
    
endmodule