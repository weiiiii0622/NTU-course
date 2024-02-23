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

module MUX32_4 (
    select,
    data1_i,
    data2_i,
    data3_i,
    data4_i,
    data_o
);

input   [1:0]   select;
input   [31:0]  data1_i;
input   [31:0]  data2_i;
input   [31:0]  data3_i;
input   [31:0]  data4_i;
output  [31:0]  data_o;

assign data_o = (select == 2'b00) ? data1_i 
                : 
                (select == 2'b01) ? data2_i
                : 
                (select == 2'b10) ? data3_i
                : 
                data4_i;
    
endmodule