module IF_ID (
    clk_i,
    rst_i,
    inst_i,
    pc_i,
    stall_i,
    flush_i,
    pc_o,
    inst_o
);

input               clk_i;
input               rst_i;
input       [31:0]  inst_i;
input       [31:0]  pc_i;
input               stall_i;
input               flush_i;
output reg  [31:0]  inst_o;
output reg  [31:0]  pc_o;

always@(posedge clk_i or negedge rst_i) begin
    if (~rst_i || flush_i) begin
        inst_o <= 32'b0;
        pc_o <= 32'b0;
    end
    else if(~stall_i) begin
        pc_o <= pc_i;
        inst_o <= inst_i;
    end
end
    
endmodule