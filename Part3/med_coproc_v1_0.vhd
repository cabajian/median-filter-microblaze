library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity med_coproc_v1_0 is
	generic (
		-- Users to add parameters here

		-- User parameters ends
		-- Do not modify the parameters beyond this line


		-- Parameters of Axi Slave Bus Interface S00_AXIS
		C_S00_AXIS_TDATA_WIDTH	: integer	:= 32;

		-- Parameters of Axi Master Bus Interface M00_AXIS
		C_M00_AXIS_TDATA_WIDTH	: integer	:= 32;
		C_M00_AXIS_START_COUNT	: integer	:= 32
	);
	port (
		-- Users to add ports here

		-- User ports ends
		-- Do not modify the ports beyond this line


		-- Ports of Axi Slave Bus Interface S00_AXIS
		s00_axis_aclk	: in std_logic;
		s00_axis_aresetn	: in std_logic;
		s00_axis_tready	: out std_logic;
		s00_axis_tdata	: in std_logic_vector(C_S00_AXIS_TDATA_WIDTH-1 downto 0);
		s00_axis_tstrb	: in std_logic_vector((C_S00_AXIS_TDATA_WIDTH/8)-1 downto 0);
		s00_axis_tlast	: in std_logic;
		s00_axis_tvalid	: in std_logic;

		-- Ports of Axi Master Bus Interface M00_AXIS
		m00_axis_aclk	: in std_logic;
		m00_axis_aresetn	: in std_logic;
		m00_axis_tvalid	: out std_logic;
		m00_axis_tdata	: out std_logic_vector(C_M00_AXIS_TDATA_WIDTH-1 downto 0);
		m00_axis_tstrb	: out std_logic_vector((C_M00_AXIS_TDATA_WIDTH/8)-1 downto 0);
		m00_axis_tlast	: out std_logic;
		m00_axis_tready	: in std_logic
	);
end med_coproc_v1_0;

architecture arch_imp of med_coproc_v1_0 is

    signal med_rst : std_logic;
    signal out_valid, out_valid_hold : std_logic;
    signal out_data : unsigned(7 downto 0);
    signal out_data_hold  : std_logic_vector(C_M00_AXIS_TDATA_WIDTH-1 downto 0);
begin

    med_rst <= not s00_axis_aresetn;

    median_top_inst : entity work.median_top
    port map (
        clk => s00_axis_aclk,
        rst => med_rst,
        i_data_vld => s00_axis_tvalid,
        i_data => unsigned(s00_axis_tdata(7 downto 0)),
        o_data => out_data,
        o_data_vld => out_valid
    );
    s00_axis_tready <= '1';
    m00_axis_tstrb <= (others => '1');
    m00_axis_tvalid <= out_valid_hold;
    m00_axis_tlast <= out_valid_hold;
    m00_axis_tdata <= out_data_hold;

    process (m00_axis_aclk) is
    begin
        if (rising_edge(m00_axis_aclk)) then
            if (out_valid = '1' or m00_axis_tready = '1') then
                out_valid_hold <= out_valid;
                out_data_hold(31 downto 8) <= (others => '0');
                out_data_hold(7 downto 0)  <= std_logic_vector(out_data);
            else 
                out_valid_hold <= out_valid_hold;
                out_data_hold <= out_data_hold;
            end if;
        end if;
    end process;

end arch_imp;