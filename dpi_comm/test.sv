module dpi_comm#(
  string name = "dpi_comm",
  int port = 7337,
  bit wait_for_connection = 1'b1
)(
  input		      clk,
  input		      reset,
  //
  input logic [63:0]  tx_data,
  input logic	      tx_valid,
  output logic	      tx_ready,
  //
  output logic [63:0] rx_data,
  output logic	      rx_valid,
  input logic	      rx_ready
);
  import "DPI-C" function int dpi_comm_init(string name, int port, bit wait_for_connection);
  import "DPI-C" function bit dpi_comm_tx(int id, longint unsigned tx_data);
  import "DPI-C" function bit dpi_comm_rx(int id, bit rx_ready, output longint unsigned rx_data);
  int id;
  
  initial begin
    id = dpi_comm_init(name, port, wait_for_connection);
  end

  always_ff @(posedge clk) begin
    if (!reset) begin
      rx_valid <= dpi_comm_rx(id, rx_ready, rx_data);
    end else begin
      rx_valid <= '0;
      rx_data <= '0;
    end
  end

  always_ff @(posedge clk) begin
    if (!reset) begin
      if (tx_valid) begin
	tx_ready <= dpi_comm_tx(id, tx_data);
      end else begin 
	tx_ready <= '1;
      end
    end else begin
      tx_ready <= '0;
    end
  end

endmodule

module test;
  logic clk;
  logic reset;
  //
  logic [63:0] tx_data_0;
  logic        tx_valid_0;
  logic        tx_ready_0;
  // 
  logic [63:0] rx_data_0;
  logic        rx_valid_0;
  logic        rx_ready_0; 
  // 
  logic [63:0] rx_data_1;
  logic        rx_valid_1;
  logic        rx_ready_1;


  // receiver from host
  dpi_comm#(.name("test_0"), .port(7337)) c0(
			      .clk,
			      .reset,
			      .tx_data(rx_data_1),
			      .tx_valid(rx_valid_1),
			      .tx_ready(rx_ready_1),
			      .rx_data(rx_data_0),
			      .rx_valid(rx_valid_0),
			      .rx_ready(rx_ready_0)
			      );

  // transmit to host
  dpi_comm#(.name("test_1"), .port(7338)) c1(
				.clk,
				.reset,
				.tx_data(rx_data_0),
				.tx_valid(rx_valid_0),
				.tx_ready(rx_ready_0),
				.rx_data(rx_data_1),
				.rx_valid(rx_valid_1),
				.rx_ready(rx_ready_1)
				);

  // clock and reset
  initial begin
    reset = 1'b1;
    #10;
    reset = 1'b0;
  end

  initial begin
    clk = 0;
    forever #1 clk = ~clk;
  end
endmodule
