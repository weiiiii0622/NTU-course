module Branch_Checker(
    PC_i,
    branch_addr_i,
    zero_i,
    branch_i,
    predict_i,
    resolved_addr_o,
    select_o            // Used for flush and select for MUX in IF
);

input  [31:0]   PC_i;               // PC addr. (Need to add 4)
input  [31:0]   branch_addr_i;      // Branch addr.
input           zero_i;             // Result for branch in EXE stage
input           branch_i;           // is beq?
input           predict_i;          // Result for branch prediction in ID stage

output  [31:0]  resolved_addr_o;    // Correct addr.
output          select_o;           // 0: Not beq or prediction is correct, 1: prediction is wrong (need to use resovled addr.)

assign select_o = (predict_i == zero_i || ~branch_i) ? 1'b0 : 1'b1;
assign resolved_addr_o = (predict_i == 1'b1) ? PC_i + 4 : branch_addr_i;
    
endmodule