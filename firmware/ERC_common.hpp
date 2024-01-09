#ifndef __ERC_COMMON_HPP
#define __ERC_COMMON_HPP

#include <string>
#include <array>
#include <map>

extern void print_log(std::string file, unsigned int line, unsigned int log_tag, std::string message);
#define ERC_PRINT_LOG(log_tag,message) (print_log(__FILE__, __LINE__, log_tag, message));

//---- Custom Types ----

namespace ERC
{
constexpr unsigned int NETWORK_ADDRESS_LEN = 2;  //bytes
constexpr unsigned int DEFAULT_HEADER_LEN  = (NETWORK_ADDRESS_LEN*2 + 4); //in bytes
constexpr unsigned int FRAME_BUFFER_SIZE   = 1000; //bytes

typedef struct 
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint32_t usecond;
    uint32_t psecond;
    uint32_t asecond;
    uint32_t ysecond;
} datetime_t;

typedef struct 
{
    uint8_t  buff[FRAME_BUFFER_SIZE];
    uint32_t buff_len;
    uint32_t valid_len;
    uint16_t data_coding;
    uint8_t  src[NETWORK_ADDRESS_LEN];
    uint8_t  hop_src[NETWORK_ADDRESS_LEN];
    uint8_t  dest[NETWORK_ADDRESS_LEN];
} frame_t;

typedef struct 
{
    uint8_t* buff;
    uint32_t valid_start_index;
    uint32_t valid_end_index;
    uint32_t acccess_index;
    const uint32_t len; //max buffer length
} frame_buff_t;

typedef struct
{
    uint8_t src[NETWORK_ADDRESS_LEN];
    uint8_t src_msbitmask;
    uint8_t dest[NETWORK_ADDRESS_LEN];
    uint8_t dest_msbitmask;
} erc_routing_rule_t;

typedef struct 
{
    uint8_t network_address[NETWORK_ADDRESS_LEN];
    void (*main)();
}erc_basic_mape_t;

//exec ~= imperative data
typedef struct erc_process_t erc_process_t;
struct erc_process_t
{
    const uint8_t network_address[NETWORK_ADDRESS_LEN];
    uint8_t state; //One of ERC_STATE_* 
    bool in_sig[4];
    bool out_sig[4];
    frame_buff_t* in_buff;
    frame_buff_t* out_buff;
    void (*main)();
};

typedef struct erc_symbol_t erc_symbol_t;
struct erc_symbol_t
{
    uint16_t alias;
    uint8_t type;
    void* bus_address;
};

//Stream ~= File ~= Bash pipe
typedef struct erc_stream_t erc_stream_t;
struct erc_stream_t
{
    uint8_t* base_bus_address; //May or may not be used depending on the driver
    //Feature(s) not implemented
    //const uint8_t permissions; //One of ERC_PERMISSION_*
    const uint32_t max_len;
    uint32_t end_index; // byte count - 1
    uint32_t access_index; //for operations
    uint32_t (*write_driver) (void*, uint32_t, erc_stream_t*);
    uint32_t (*read_driver) (void*, uint32_t, erc_stream_t*);
};

//ref ~= block of data~= bus accessible data ~= declarative data
typedef struct erc_block_t erc_block_t;
struct erc_block_t
{
    uint8_t* base_bus_address;
    //Features not implemented
    //    bool is_obj;
    //    erc_block_t* children_info;
    //    const uint8_t permissions; //One of ERC_PERMISSION_*
    const uint8_t indirection;
    //Note: Use len = 0 for informational or end addresses
    const uint32_t len;
};


#define ERCV 7
//TODO Update per documentation
typedef union {
    struct
    {
        //Build info
        uint32_t ercv                   ;
    } strct;
    uint32_t array32[5];
    uint8_t array8[36];
} meta_t;


//Constants

const std::map<uint8_t, int> hextoi = 
{
{'0', 0},
{'1', 1},
{'2', 2},
{'3', 3},
{'4', 4},
{'5', 5},
{'6', 6},
{'7', 7},
{'8', 8},
{'9', 9},
{'a',10},
{'b',11},
{'c',12},
{'d',13},
{'e',14},
{'f',15},
{'A',10},
{'B',11},
{'C',12},
{'D',13},
{'E',14},
{'F',15}
};


//index directly with the value
const uint8_t itoahex[16] = 
{
'0',
'1',
'2',
'3',
'4',
'5',
'6',
'7',
'8',
'9',
'a',
'b',
'c',
'd',
'e',
'f'    
};


const uint8_t azitoa[] =
{
'A', //0
'B', //1
'C', //2
'D', //3
'E', //4
'F', //5
'G', //6
'H', //7
'I', //8
'J', //9
'K', //10
'L', //11
'M', //12
'N', //13
'O', //14
'P', //15
'Q', //16
'R', //17
'S', //18
'T', //29
'U', //20
'V', //21
'W', //22
'X', //23
'Y', //24
'Z'  //25
};

//Standard pinout lettering
//Extracted from Xilinx xqzu29drffrf1760. 
//Lettering order verified with Intel 1sm16b package.
const uint16_t electronic_azitoa[] = 
{
(' ' << 8) + 'A', //0
(' ' << 8) + 'B', //1
(' ' << 8) + 'C', //2
(' ' << 8) + 'D', //3
(' ' << 8) + 'E', //4
(' ' << 8) + 'F', //5
(' ' << 8) + 'G', //6
(' ' << 8) + 'H', //7
(' ' << 8) + 'J', //8
(' ' << 8) + 'K', //9
(' ' << 8) + 'L', //10
(' ' << 8) + 'M', //11
(' ' << 8) + 'N', //12
(' ' << 8) + 'P', //13
(' ' << 8) + 'R', //14
(' ' << 8) + 'T', //15
(' ' << 8) + 'U', //16
(' ' << 8) + 'V', //17
(' ' << 8) + 'W', //18
(' ' << 8) + 'Y', //19
('A' << 8) + 'A', //20
('A' << 8) + 'B', //21
('A' << 8) + 'C', //22
('A' << 8) + 'D', //23
('A' << 8) + 'E', //24
('A' << 8) + 'F', //25* E.g. ERC network address -> Ethernet MAC Address
('A' << 8) + 'G', //26
('A' << 8) + 'H', //27
('A' << 8) + 'J', //28
('A' << 8) + 'K', //29
('A' << 8) + 'L', //30
('A' << 8) + 'M', //31
('A' << 8) + 'N', //32
('A' << 8) + 'P', //33
('A' << 8) + 'R', //34
('A' << 8) + 'T', //35
('A' << 8) + 'U', //36
('A' << 8) + 'V', //37
('A' << 8) + 'W', //38
('A' << 8) + 'Y', //39
('B' << 8) + 'A', //40
('B' << 8) + 'B'  //41
};

enum 
{
    TYPE_PROCESS, 
    TYPE_GENERIC_STREAM, 
    TYPE_FILE, //Includes access to non-memory-mapped block devices like in UNIX?
    TYPE_SHELL,
    TYPE_PERIPHERAL,
    TYPE_BLOCK //block of memory mapped / bus accessible declarative or executable data
};

//Indicates how data is SerialUSBized.
//Does not fully dictate format: subvariants of these are possible and too numerous to list. 
//frame:[[field 1][field2][field3]...]
enum
{
    frame_coding_raw,         //raw binary, known data widths in bits, no field delimiters, no field metadata, no frame delimiter. Most compact form. Useful for providing directly to bit bang interface drivers. Integers in 2s complement.
    frame_coding_raw_bytes,   //raw bytes, same as RAW but data widths in 8 bit bytes and with padding on bits to meet n*8bit widths (byte aligned)
    frame_coding_raw_MD,      //Raw but includes metadata such as datatypes and bit/byte widths to enable encoding or decoding. Enables variable width fields, optional fields, and variable type fields. Examples with variable lengths: payload = data1+data2+len3+len4+data3+data4, payload = data1+data2+len3+data3+len4+data4, len1+data1+len2+data2+len3+data3+len4+data4,
    frame_coding_PT_UTF8FWKF, //Plain text UTF8 with fixed widths (in character counts) and known formats (hex, octal, decimal, string, etc.). printf + scanf can encode + decode this data. Examples of fixed widths: b16/hex >20char width (>64bit blocks), fixed width 20char decimal (64bit) 10char decimal (32bit) 5char decimal (16bit) 3char decimal (8bit) 1char decimal (1bit). May or may not be human readable. Due to fixed widths, field delimiter is optional but frame delimiter must be specified (typically '\n' '\r\n')
    frame_coding_PT_UTF8MWKF, //Plain text UTF8 with minimal widths (in character counts) and known formats (hex, octal, decimal, string, etc.). printf + scanf can encode + decode this data. Field and frame delimiters must be specified (typically ' ' '\n' '\r\n')
    frame_coding_PT_UTF8C,    //Plain text UTF8 with C type syntax: decimal 23, hex 0xdeadbeef, binary 0b0101, "string", {'b','l','o','c','k'}, compact block 0xdeadbeef (can extend larger than machine size). Field delimiters are ' ' but frame delimiters must be specified (typically, '\n' '\r\n') 
    frame_coding_PT_UTF8CC,   //Plain text UTF8 with Compact C syntax: decimal 23 (0-9 first character), hex xdeadbeef, binary b0101, "string" (closing quote allows spaces), compact block xdeadbeef (same as hex but can extend larger than machine size). Field delimiters are ' ' but frame delimiters must be specified (typically, '\n' '\r\n') 
    frame_coding_B16,         //Base16 + some given frame terminator character (typically '\n')
    frame_coding_B64,         //Base64 + some given frame terminator character (typically '\n')
    frame_coding_PB,          //Protobuff, no frame delimiter / len indication
    frame_coding_SCPI,        //Practically a variety of plain text UTF8. typically 'h' or '0x' or '#h'
    frame_coding_MODBUS_RTU,
    frame_coding_MODBUS_ASCII,
    frame_coding_MODBUS_TCP,
    frame_coding_DEVICENET,
    frame_coding_PROFIBUS,
    frame_coding_HART,
    frame_coding_ARINC_429,
    frame_coding_MIL_STD_1553,
    frame_coding_CAN,
    frame_coding_LIN,
    frame_coding_FLEXRAY,
    frame_coding_MOST,
    frame_coding_BB_UART,     //bitbang
    frame_coding_BB_I2C,      //bitbang
    frame_coding_ETHERNET,
    frame_coding_TCP_IP,
    frame_coding_UDP_IP,
    frame_coding_SONET,
    frame_coding_ASN_1,
    frame_coding_APACHE_THRIFT,
    frame_coding_D_BUS,
    frame_coding_SOAP,
    frame_coding_MESHTATIC,
    frame_coding_MQTT,
    frame_coding_HTTP1,
    frame_coding_HTTP2,
    frame_coding_HTTP3,
    frame_coding_QUIC,
    frame_coding_LoRA,
    frame_coding_nRF24L01,
    frame_coding_FUSE_file,
    frame_coding_NFS_file,
    frame_coding_rsync_file,
    frame_coding_git_file,
    frame_coding_file_io
};

enum
{
    ECC_MD5sum,
    ECC_SHA1sum,
    ECC_SHA256sum,
    ECC_SHA3sum,
    ECC_CRC32,
    ECC_parity,
};

enum
{
    LC_NRZ_L,
    LC_NRZ_M,
    LC_NRZ_S,
    LC_RZ,
    LC_MANCHESTER,
    LC_6B_8B,
    LC_8B_10B,
    LC_64B_66B,
    LC_128B_130B,
    LC_PAM2,
    LC_PAM4,
    LC_QAM4,
    LC_QAM8,
    LC_QAM16,
    LC_QAM32,
    LC_QAM64,
    LC_QAM128,
};

enum
{
    COMPRESSION_GZIP,
    COMPRESSION_LZMA
};

enum 
{
    PERMISSION_NONE     = 0,
    PERMISSION_READ     = 1,
    PERMISSION_WRITE    = 2,
    PERMISSION_EXECUTE  = 4,
};

//These serve as return values for complex functions and actual state
typedef enum 
{
    status_code_success, //Doubles as return boolean true
    status_code_error_buff,
    status_code_error_no_memory,
    status_code_error_input_domain, // Special type of syntax error. Domain considers only one literal (ignoring the entire input as a single literal) whereas syntax covers multiple literals and their relationships. 
    status_code_error_input_syntax,
    status_code_error_rxbuff,
    status_code_error_txbuff,
    status_code_error_time_sequence, // Includes programming sequence errors which are caught at run time
    status_code_error_timeout,
    status_code_error_network_pressure,
    status_code_error_no_route,
    status_code_error_overload,
    status_code_error_lock_failure,
    status_code_error_not_initialized, // Special type of time sequence error
    status_code_error_not_implemented, // Special type of time sequence error (if the feature ever gets implemented). Special type of input error since user should have known that the feature was not implemented.
    status_code_error_data_holes, // Incompletely assembled data is insufficient to do something
    status_code_corruption,
    status_code_error1, //Doubles as return boolean false
    status_code_error2,
    status_code_error3,
    status_code_error4
} status_code_t;

//Input signals
enum
{
    ERC_ISIG_RX_AVAILABLE,
    ERC_ISIG_TX_HANDLED,
    ERC_ISIG_USER1,
    ERC_ISIG_USER2
};

//Output signals
enum
{
    ERC_OSIG_TX_AVAILABLE,
    ERC_OSIG_RX_HANDLED,
    ERC_OSIG_USER1,
    ERC_OSIG_USER2
};

//Recommended ERC Network Addresses / Process IDs / Thread IDs
//Process -> DeSerialUSBizer -> Function
//Basic processes
#define MMIO_DRIVER       1
#define STREAM_DRIVER     1
#define FILE_DRIVER       1
#define SYMBOL_MANAGER    2
#define PROC_MANAGER      3
#define RESET             1
#define MEMCPY_SYM        4
#define MEMCPY_ADD        5
#define DUMMY             6
#define DEBUG_MANAGER     8

//Generic API (to/from arbitrary destination/source buffers)
//Direct API (to/from frame data)
#define GPIO_DRIVER      9
#define UART_DRIVER      10
#define I2C_DRIVER       11
#define SPI_DRIVER       12
#define ADC_DRIVER       13
#define TIM_DRIVER       14
#define DAC_DRIVER       15
#define COMP_DRIVER      16
#define BITPIPE_DRIVER   17

#define BASIC_ADDRESS_END 25

//Recommended ERC symbol IDs
//No symbol IDs for processes - use network address
//Hardware Peripherals
#define GPIOA            1
#define GPIOB            2
#define GPIOC            3
#define GPIOD            4
#define GPIOE            5
#define GPIOF            6
#define GPIOG            7
#define GPIOH            8
#define GPIOI            9

#define BUTTON0          10
#define BUTTON1          11
#define BUTTON2          12
#define BUTTON3          13
#define BUTTON4          14
#define BUTTON5          15
#define BUTTON6          16
#define BUTTON7          17
#define BUTTON8          18
#define BUTTON9          19
#define BUTTON10         20
#define BUTTON11         21
#define BUTTON12         22
#define BUTTON13         23
#define BUTTON14         24
#define BUTTON15         25
#define BUTTON16         26

#define LED0             27
#define LED1             28
#define LED2             29
#define LED3             30
#define LED4             31
#define LED5             32
#define LED6             33
#define LED7             34
#define LED8             35
#define LED9             36
#define LED10            37
#define LED11            38
#define LED12            39
#define LED13            40
#define LED14            41
#define LED15            42
#define LED16            43

#define UART0            44
#define UART1            45
#define UART2            46
#define UART3            47
#define UART4            48

#define SPI0             49
#define SPI1             50
#define SPI2             51
#define SPI3             52
#define SPI4             53

#define I2C0             54
#define I2C1             55
#define I2C2             56
#define I2C3             57
#define I2C4             58

#define USB0             59
#define USB1             60
#define USB2             61
#define USB3             62
#define USB4             63
#define USB5             64
#define USB6             65
#define USB7             66
#define USB8             67

#define ETH0             68
#define ETH1             69
#define ETH2             70
#define ETH3             71
#define ETH4             72
#define ETH5             73
#define ETH6             74
#define ETH7             75
#define ETH8             76
#define ETH9             77
#define ETH10            78
#define ETH11            79
#define ETH12            80
#define ETH13            81
#define ETH14            82
#define ETH15            83
#define ETH16            84

#define RADIO1           85
#define RADIO2           86
#define RADIO3           87
#define RADIO4           88

#define HDMI0            89
#define HDMI1            90
#define HDMI2            91
#define HDMI3            92
#define HDMI4            93

#define DP0              94
#define DP1              95
#define DP2              96
#define DP3              97
#define DP4              98

#define TIM0             99
#define TIM1             100
#define TIM2             101
#define TIM3             102
#define TIM4             103
#define TIM5             104
#define TIM6             105
#define TIM7             106
#define TIM8             107
#define TIM9             108
#define TIM10            109

#define ADC0             110
#define ADC1             111
#define ADC2             112
#define ADC3             113
#define ADC4             114
#define ADC5             115
#define ADC6             116
#define ADC7             117
#define ADC8             118
#define ADC9             119
#define ADC10            120

#define DAC0             121
#define DAC1             122
#define DAC2             123
#define DAC3             124
#define DAC4             125
#define DAC5             126
#define DAC6             127
#define DAC7             128
#define DAC8             129
#define DAC9             130
#define DAC10            131

#define COMP0            132
#define COMP1            133
#define COMP2            134
#define COMP3            135
#define COMP4            136
#define COMP5            137
#define COMP6            138
#define COMP7            139
#define COMP8            140
#define COMP9            141
#define COMP10           142

#define TEMP0            143
#define TEMP1            144
#define TEMP2            145
#define TEMP3            146
#define TEMP4            147
#define TEMP5            148
#define TEMP6            149
#define TEMP7            150
#define TEMP8            151
#define TEMP9            152

#define RTC0             153
#define RTC1             154
#define RTC2             155

//Block variables
#define SYMBOL_MAP       156
#define ROUTING_TABLE    157
#define META             158
#define DEBUG_ADD        159
#define WVAR1            160
#define WVAR2            161
#define WVAR3            162
#define WVAR4            163

//Useful bus addresses - may not be applicable on some platforms
#define UNIQUE_DEV_ID        164
#define BASE_FLASH           165
#define END_FLASH            166
#define BASE_RAM             167
#define END_RAM              168
#define BASE_PERIPH          169
#define END_PERIPH           170

//Streams
#define STDIN            171
#define STDOUT           172
#define STDERR           173
#define STDLOG           174

#define BASIC_SYMBOL_END        173

const std::array<uint8_t, NETWORK_ADDRESS_LEN> debug_address = {STDERR, 0x0};
uint32_t get_tick();

//Definition per https://protobuf.dev/reference/protobuf/textformat-spec/
//Same as https://en.cppreference.com/w/cpp/string/byte/isspace

inline bool is_whitespace(uint8_t val)
{
    return ((val == ' ') || (val == '\n') || (val == '\t') || (val == '\v') || (val == '\f') || (val == '\r'));
}

inline bool is_printable(uint8_t val)
{
    return (('!' <= val) && (val <= '~'));
}

inline bool is_upper(uint8_t val)
{
    return (('A' <= val) && (val <= 'Z'));
}

inline bool is_lower(uint8_t val)
{
    return (('a' <= val) && (val <= 'z'));
}

inline bool is_alpha(uint8_t val)
{
    return (is_upper(val) || is_lower(val));
}

inline bool is_numeric(uint8_t val)
{
    return (('0' <= val) && (val <= '9'));
}

inline bool is_alphanumeric(uint8_t val)
{
    return (is_alpha(val) || is_numeric(val));
}

inline bool is_alphanumeric_u(uint8_t val)
{
    return (is_alphanumeric(val) || (val == '_'));
}

inline bool is_escape(uint8_t val)
{
    return (is_alphanumeric(val) || (val == '_'));
}

//Unicode UTF-8
// Users are responsible for indexing and casting when used with strings
// uint8_t(*(val.c_str()) or val[index]
//https://en.cppreference.com/w/cpp/string/byte/isalpha
inline bool is_alphanumeric(uint8_t val);
inline bool is_alphanumeric_u(uint8_t val);
inline bool is_escape(uint8_t val);
uint8_t reverse_bytes(uint8_t* buff, uint32_t len);
std::string to_printable(std::string input);
}//namespace
#endif //__ERC_COMMON_HPP
