module Branch_Predictor(
    clk_i,
    rst_i,
    branch_i,
    zero_i,
    predict_o
);
    
input               clk_i;
input               rst_i;
input               branch_i;   // If =1, update state
input               zero_i;
output              predict_o;

// 2'b00: Strongly not taken
// 2'b01: Weakly not taken
// 2'b10: Weakly taken
// 2'b11: Strongly taken
reg     [1:0]       state;

always@(posedge clk_i or negedge rst_i) begin
    if(~rst_i) begin
        state <= 2'b11;
    end
    else if(branch_i) begin
        // Update State
        if(state == 2'b00) begin
            if(zero_i) begin    // Taken
                state <= 2'b01;
            end
            else begin          // Not Taken
                state <= 2'b00;
            end
        end
        else if(state == 2'b01) begin
            if(zero_i) begin    // Taken
                state <= 2'b10;
            end
            else begin          // Not Taken
                state <= 2'b00; 
            end
        end
        else if(state == 2'b10) begin
            if(zero_i) begin    // Taken
                state <= 2'b11;
            end
            else begin          // Not Taken
                state <= 2'b01;
            end
        end
        else if(state == 2'b11) begin
            if(zero_i) begin    // Taken
                state <= 2'b11;
            end
            else begin          // Not Taken
                state <= 2'b10;
            end
        end
    end
end

assign predict_o = state[1];

endmodule