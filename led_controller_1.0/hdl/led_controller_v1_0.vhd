library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity led_controller_v1_0 is
	generic (
		-- Users to add parameters here

        C_LED_INTERFACES : integer := 2;
        C_ACTIVE_HIGH : boolean := false;
    
        -- User parameters ends
        -- Do not modify the parameters beyond this line
    
    
        -- Parameters of Axi Slave Bus Interface S_AXI
        C_S_AXI_DATA_WIDTH    : integer    := 32;
        C_S_AXI_ADDR_WIDTH    : integer    := 7
	);
	port (
		-- Users to add ports here
    
        m_led_out : out std_logic_vector (C_LED_INTERFACES-1 downto 0);
    
        -- User ports ends
        -- Do not modify the ports beyond this line
    
    
        -- Ports of Axi Slave Bus Interface S_AXI
        s_axi_aclk    : in std_logic;
        s_axi_aresetn    : in std_logic;
        s_axi_awaddr    : in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
        s_axi_awprot    : in std_logic_vector(2 downto 0);
        s_axi_awvalid    : in std_logic;
        s_axi_awready    : out std_logic;
        s_axi_wdata    : in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        s_axi_wstrb    : in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
        s_axi_wvalid    : in std_logic;
        s_axi_wready    : out std_logic;
        s_axi_bresp    : out std_logic_vector(1 downto 0);
        s_axi_bvalid    : out std_logic;
        s_axi_bready    : in std_logic;
        s_axi_araddr    : in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
        s_axi_arprot    : in std_logic_vector(2 downto 0);
        s_axi_arvalid    : in std_logic;
        s_axi_arready    : out std_logic;
        s_axi_rdata    : out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        s_axi_rresp    : out std_logic_vector(1 downto 0);
        s_axi_rvalid    : out std_logic;
        s_axi_rready    : in std_logic
	);
end led_controller_v1_0;

architecture arch_imp of led_controller_v1_0 is

	-- component declaration
	component led_controller_v1_0_S_AXI is
		generic (
		C_LED_INTERFACES : integer := 2;
        
        C_S_AXI_DATA_WIDTH    : integer    := 32;
        C_S_AXI_ADDR_WIDTH    : integer    := 3
        );
        port (
        m_led_mode : out std_logic_vector ((C_LED_INTERFACES*2)-1 downto 0);
        m_led_val : out std_logic_vector ((C_LED_INTERFACES*8)-1 downto 0);
        
        S_AXI_ACLK    : in std_logic;
        S_AXI_ARESETN    : in std_logic;
        S_AXI_AWADDR    : in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
        S_AXI_AWPROT    : in std_logic_vector(2 downto 0);
        S_AXI_AWVALID    : in std_logic;
        S_AXI_AWREADY    : out std_logic;
        S_AXI_WDATA    : in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        S_AXI_WSTRB    : in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
        S_AXI_WVALID    : in std_logic;
        S_AXI_WREADY    : out std_logic;
        S_AXI_BRESP    : out std_logic_vector(1 downto 0);
        S_AXI_BVALID    : out std_logic;
        S_AXI_BREADY    : in std_logic;
        S_AXI_ARADDR    : in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
        S_AXI_ARPROT    : in std_logic_vector(2 downto 0);
        S_AXI_ARVALID    : in std_logic;
        S_AXI_ARREADY    : out std_logic;
        S_AXI_RDATA    : out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        S_AXI_RRESP    : out std_logic_vector(1 downto 0);
        S_AXI_RVALID    : out std_logic;
        S_AXI_RREADY    : in std_logic
		);
	end component led_controller_v1_0_S_AXI;
	
	component led_ctrlr
    generic (
        ACTIVE_HIGH : boolean := true
    );
    port (
        RST : in STD_LOGIC;
        CLK : in STD_LOGIC;
        MODE : in STD_LOGIC_VECTOR (1 downto 0);
        VAL : in STD_LOGIC_VECTOR (7 downto 0);
        LED : out STD_LOGIC);
    end component;
	
    signal s_rst : std_logic;
    signal s_led_mode : std_logic_vector ((C_LED_INTERFACES*2)-1 downto 0);
    signal s_led_val : std_logic_vector ((C_LED_INTERFACES*8)-1 downto 0);

begin

-- Instantiation of Axi Bus Interface S_AXI
led_controller_v1_0_S_AXI_inst : led_controller_v1_0_S_AXI
	generic map (
        C_LED_INTERFACES => C_LED_INTERFACES,
    
        C_S_AXI_DATA_WIDTH    => C_S_AXI_DATA_WIDTH,
        C_S_AXI_ADDR_WIDTH    => C_S_AXI_ADDR_WIDTH
    )
    port map (
        m_led_mode => s_led_mode,
        m_led_val => s_led_val,
    
        S_AXI_ACLK    => s_axi_aclk,
        S_AXI_ARESETN    => s_axi_aresetn,
        S_AXI_AWADDR    => s_axi_awaddr,
        S_AXI_AWPROT    => s_axi_awprot,
        S_AXI_AWVALID    => s_axi_awvalid,
        S_AXI_AWREADY    => s_axi_awready,
        S_AXI_WDATA    => s_axi_wdata,
        S_AXI_WSTRB    => s_axi_wstrb,
        S_AXI_WVALID    => s_axi_wvalid,
        S_AXI_WREADY    => s_axi_wready,
        S_AXI_BRESP    => s_axi_bresp,
        S_AXI_BVALID    => s_axi_bvalid,
        S_AXI_BREADY    => s_axi_bready,
        S_AXI_ARADDR    => s_axi_araddr,
        S_AXI_ARPROT    => s_axi_arprot,
        S_AXI_ARVALID    => s_axi_arvalid,
        S_AXI_ARREADY    => s_axi_arready,
        S_AXI_RDATA    => s_axi_rdata,
        S_AXI_RRESP    => s_axi_rresp,
        S_AXI_RVALID    => s_axi_rvalid,
        S_AXI_RREADY    => s_axi_rready
    );

	-- Add user logic here
	s_rst <= not s_axi_aresetn;
	
	LED_INTERFACES: for I in 0 to C_LED_INTERFACES-1 generate
	   led_ctrlr_inst : led_ctrlr
	   generic map (
	       ACTIVE_HIGH => C_ACTIVE_HIGH
       )
	   port map (
	       RST => s_rst,
	       CLK => s_axi_aclk,
	       MODE => s_led_mode ((I+1)*2-1 downto I*2),
	       VAL => s_led_val ((I+1)*8-1 downto I*8),
	       LED => m_led_out(I)
	   );
	end generate LED_INTERFACES;

	-- User logic ends

end arch_imp;