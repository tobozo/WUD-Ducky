#pragma once

// WORK IN PROGRESS, NOTHING IS FUNCTIONAL HERE


#if 0

//#include "USB.h"


// #ifndef CFG_TUD_ENDPOINT0_SIZE
// #define CFG_TUD_ENDPOINT0_SIZE    64
// #endif
//
// //------------- CLASS -------------//
// #define CFG_TUD_CDC               0
// #define CFG_TUD_MSC               0
// #define CFG_TUD_HID               0
// #define CFG_TUD_MIDI              0
// #define CFG_TUD_VENDOR            0
// #define CFG_TUD_NET               1

#include "esp32-hal-tinyusb.h"
//#include "tinyusb.h"
//#include "tusb_net.h"
//#include "tusb_bth.h"

// usb rndis/emc MAC address
const uint8_t tud_network_mac_address[6] = {0x02,0x02,0x84,0x6A,0x96,0x00};

//tinyusb network interface callbacks
bool tud_network_recv_cb(const uint8_t *src, uint16_t size);
uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg);
void tud_network_init_cb(void);

//extern tusb_desc_device_t desc_device;
//extern tusb_desc_strarray_device_t desc_string;
// gv
static esp_netif_t* netif;
static uint8_t tud_rx_buf[2048];
static size_t tud_rx_len = 0;

// usb descriptors

tusb_desc_device_t const dsc_device =
{
  .bLength            = sizeof(tusb_desc_device_t),
  .bDescriptorType    = TUSB_DESC_DEVICE,
  .bcdUSB             = 0x0200,

  // Use Interface Association Descriptor (IAD) device class
  .bDeviceClass       = TUSB_CLASS_MISC,
  .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
  .bDeviceProtocol    = MISC_PROTOCOL_IAD,

  .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

  .idVendor           = 0xeeee, //vendor ID
  .idProduct          = 0x0001, //product ID
  .bcdDevice          = 0x0101, //device

  .iManufacturer      = 0x01, //manufacturer index in string descriptors
  .iProduct           = 0x02, //product index in string descriptors
  .iSerialNumber      = 0x03, //serial number in string descriptors

  .bNumConfigurations = 0x02 // multiple configurations (RNDIS + ECM)
};

// array of pointer to string descriptors
static const char *dsc_string[] =
{
  [0] = (char[]) { 0x09, 0x04 }, // supported language is English (0x0409)
  [1] = "Tech Genesis Enterprise",     // Manufacturer
  [2] = "usb-rndis-2-pppos",             // Product
  [3] = "000001",                      // Serial
  [4] = "USB PPPOS Network Interface"  // Interface Description
  // STRID_MAC index is handled separately
};

/*
tinyusb_config_cdcacm_t amc_cfg =
{
    .usb_dev = TINYUSB_USBDEV_0,
    .cdc_port = TINYUSB_CDC_ACM_0,
    .rx_unread_buf_sz = 128,
    .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
    .callback_rx_wanted_char = NULL,
    .callback_line_state_changed = &tinyusb_cdc_line_state_changed_callback,
    .callback_line_coding_changed = NULL
};
*/

/*
tinyusb_config_t tusb_rndis_cfg =
{
  .descriptor = &dsc_device,
  .string_descriptor = dsc_string,
  .external_phy = false // In the most cases you need to use a `false` value
};
*/

#endif



#if 0

extern "C" {

  #include "tusb.h"
  //#include "dhserver.h"
  //#include "dnserver.h"
  #include "lwip/init.h"
  #include "lwip/netif.h"
  //#include "lwip/ip4_addr.h"
  #include "lwip/ethip6.h"
  #include "lwip/timeouts.h"
  #include "lwip/dhcp.h"
  #include "lwip/autoip.h"
  #include "lwip/err.h"
  #include "lwip/udp.h"
  #include "netif/etharp.h"
  //typedef ip_addr_t ip4_addr_t;


  /* declared here, NOT in usb_descriptors.c, so that the driver can intelligently ZLP as needed */
  #define CFG_TUD_NET_ENDPOINT_SIZE (TUD_OPT_HIGH_SPEED ? 512 : 64)

  /* Maximum Transmission Unit (in bytes) of the network, including Ethernet header */
  #ifndef CFG_TUD_NET_MTU
    #define CFG_TUD_NET_MTU           1514
  #endif

  #ifndef CFG_TUD_NCM_IN_NTB_MAX_SIZE
    #define CFG_TUD_NCM_IN_NTB_MAX_SIZE 3200
  #endif

  #ifndef CFG_TUD_NCM_OUT_NTB_MAX_SIZE
    #define CFG_TUD_NCM_OUT_NTB_MAX_SIZE 3200
  #endif

  #ifndef CFG_TUD_NCM_MAX_DATAGRAMS_PER_NTB
    #define CFG_TUD_NCM_MAX_DATAGRAMS_PER_NTB 8
  #endif

  #ifndef CFG_TUD_NCM_ALIGNMENT
    #define CFG_TUD_NCM_ALIGNMENT 4
  #endif


  typedef struct dhcp_entry
  {
    uint8_t    mac[6];
    ip_addr_t addr;
    uint32_t   lease;
  } dhcp_entry_t;

  typedef struct dhcp_config
  {
    ip_addr_t    router;
    uint16_t      port;
    ip_addr_t    dns;
    const char   *domain;
    int           num_entry;
    dhcp_entry_t *entries;
  } dhcp_config_t;

  err_t dhserv_init(const dhcp_config_t *config);
  void dhserv_free(void);

  /* DHCP message type */
  #define DHCP_DISCOVER       1
  #define DHCP_OFFER          2
  #define DHCP_REQUEST        3
  #define DHCP_DECLINE        4
  #define DHCP_ACK            5
  #define DHCP_NAK            6
  #define DHCP_RELEASE        7
  #define DHCP_INFORM         8

  /* DHCP options */
  enum DHCP_OPTIONS
  {
    DHCP_PAD                    = 0,
    DHCP_SUBNETMASK             = 1,
    DHCP_ROUTER                 = 3,
    DHCP_DNSSERVER              = 6,
    DHCP_HOSTNAME               = 12,
    DHCP_DNSDOMAIN              = 15,
    DHCP_MTU                    = 26,
    DHCP_BROADCAST              = 28,
    DHCP_PERFORMROUTERDISC      = 31,
    DHCP_STATICROUTE            = 33,
    DHCP_NISDOMAIN              = 40,
    DHCP_NISSERVER              = 41,
    DHCP_NTPSERVER              = 42,
    DHCP_VENDOR                 = 43,
    DHCP_IPADDRESS              = 50,
    DHCP_LEASETIME              = 51,
    DHCP_OPTIONSOVERLOADED      = 52,
    DHCP_MESSAGETYPE            = 53,
    DHCP_SERVERID               = 54,
    DHCP_PARAMETERREQUESTLIST   = 55,
    DHCP_MESSAGE                = 56,
    DHCP_MAXMESSAGESIZE         = 57,
    DHCP_RENEWALTIME            = 58,
    DHCP_REBINDTIME             = 59,
    DHCP_CLASSID                = 60,
    DHCP_CLIENTID               = 61,
    DHCP_USERCLASS              = 77,  /* RFC 3004 */
    DHCP_FQDN                   = 81,
    DHCP_DNSSEARCH              = 119, /* RFC 3397 */
    DHCP_CSR                    = 121, /* RFC 3442 */
    DHCP_MSCSR                  = 249, /* MS code for RFC 3442 */
    DHCP_END                    = 255
  };

  typedef struct
  {
      uint8_t  dp_op;           /* packet opcode type */
      uint8_t  dp_htype;        /* hardware addr type */
      uint8_t  dp_hlen;         /* hardware addr length */
      uint8_t  dp_hops;         /* gateway hops */
      uint32_t dp_xid;          /* transaction ID */
      uint16_t dp_secs;         /* seconds since boot began */
      uint16_t dp_flags;
      uint8_t  dp_ciaddr[4];    /* client IP address */
      uint8_t  dp_yiaddr[4];    /* 'your' IP address */
      uint8_t  dp_siaddr[4];    /* server IP address */
      uint8_t  dp_giaddr[4];    /* gateway IP address */
      uint8_t  dp_chaddr[16];   /* client hardware address */
      uint8_t  dp_legacy[192];
      uint8_t  dp_magic[4];
      uint8_t  dp_options[275]; /* options area */
  } DHCP_TYPE;

  DHCP_TYPE dhcp_data;
  static struct udp_pcb *pcb = NULL;
  static const dhcp_config_t *config = NULL;

  char magic_cookie[] = {0x63,0x82,0x53,0x63};


  //--------------------------------------------------------------------+
  // Application API
  //--------------------------------------------------------------------+

  // indicate to network driver that client has finished with the packet provided to network_recv_cb()
  void tud_network_recv_renew(void);

  // poll network driver for its ability to accept another packet to transmit
  bool tud_network_can_xmit(uint16_t size);

  // if network_can_xmit() returns true, network_xmit() can be called once
  void tud_network_xmit(void *ref, uint16_t arg);

  //--------------------------------------------------------------------+
  // Application Callbacks (WEAK is optional)
  //--------------------------------------------------------------------+

  // client must provide this: return false if the packet buffer was not accepted
  bool tud_network_recv_cb(const uint8_t *src, uint16_t size);

  // client must provide this: copy from network stack packet pointer to dst
  uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg);

  //------------- ECM/RNDIS -------------//

  // client must provide this: initialize any network state back to the beginning
  void tud_network_init_cb(void);

  // client must provide this: 48-bit MAC address
  // TODO removed later since it is not part of tinyusb stack
  //extern const uint8_t tud_network_mac_address[6];

  //------------- NCM -------------//

  // callback to client providing optional indication of internal state of network driver
  void tud_network_link_state_cb(bool state);

  //--------------------------------------------------------------------+
  // INTERNAL USBD-CLASS DRIVER API
  //--------------------------------------------------------------------+
  void     netd_init            (void);
  void     netd_reset           (uint8_t rhport);
  uint16_t netd_open            (uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len);
  bool     netd_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * request);
  bool     netd_xfer_cb         (uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes);
  void     netd_report          (uint8_t *buf, uint16_t len);

  #define INIT_IP4(a,b,c,d) { PP_HTONL(LWIP_MAKEU32(a,b,c,d)) }

  /* lwip context */
  static struct netif netif_data;

  /* shared between tud_network_recv_cb() and service_traffic() */
  static struct pbuf *received_frame;

  /* this is used by this code, ./class/net/net_driver.c, and usb_descriptors.c */
  /* ideally speaking, this should be generated from the hardware's unique ID (if available) */
  /* it is suggested that the first byte is 0x02 to indicate a link-local address */
  const uint8_t tud_network_mac_address[6] = {0x02,0x02,0x84,0x6A,0x96,0x00};

  /* network parameters of this MCU */
  static const ip4_addr_t ipaddr  = INIT_IP4(192, 168, 7, 1);
  static const ip4_addr_t netmask = INIT_IP4(255, 255, 255, 0);
  static const ip4_addr_t gateway = INIT_IP4(0, 0, 0, 0);

  /* database IP addresses that can be offered to the host; this must be in RAM to store assigned MAC addresses */
  static dhcp_entry_t entries[] =
  {
      /* mac ip address                          lease time */
      { {0}, INIT_IP4(192, 168, 7, 2), 24 * 60 * 60 },
      { {0}, INIT_IP4(192, 168, 7, 3), 24 * 60 * 60 },
      { {0}, INIT_IP4(192, 168, 7, 4), 24 * 60 * 60 },
  };

  static const dhcp_config_t dhcp_config =
  {
      .router = INIT_IP4(0, 0, 0, 0),            /* router address (if any) */
      .port = 67,                                /* listen port */
      .dns = INIT_IP4(192, 168, 7, 1),           /* dns server (if any) */
      "usb",                                     /* dns suffix */
      TU_ARRAY_SIZE(entries),                    /* num entry */
      entries                                    /* entries */
  };
  static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
  {
    (void)netif;

    for (;;)
    {
      /* if TinyUSB isn't ready, we must signal back to lwip that there is nothing we can do */
      if (!tud_ready())
        return ERR_USE;

      /* if the network driver can accept another packet, we make it happen */
      if (tud_network_can_xmit(p->tot_len))
      {
        tud_network_xmit(p, 0 /* unused for this example */);
        return ERR_OK;
      }

      /* transfer execution to TinyUSB in the hopes that it will finish transmitting the prior packet */
      tud_task();
    }
  }

  static err_t ip4_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *addr)
  {
    return etharp_output(netif, p, addr);
  }

  #if LWIP_IPV6
  static err_t ip6_output_fn(struct netif *netif, struct pbuf *p, const ip6_addr_t *addr)
  {
    return ethip6_output(netif, p, addr);
  }
  #endif

  static err_t netif_init_cb(struct netif *netif)
  {
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    netif->mtu = CFG_TUD_NET_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state = NULL;
    netif->name[0] = 'E';
    netif->name[1] = 'X';
    netif->linkoutput = linkoutput_fn;
    netif->output = ip4_output_fn;
  #if LWIP_IPV6
    netif->output_ip6 = ip6_output_fn;
  #endif
    return ERR_OK;
  }

  static void init_lwip(void)
  {
    struct netif *netif = &netif_data;

    lwip_init();

    /* the lwip virtual MAC address must be different from the host's; to ensure this, we toggle the LSbit */
    netif->hwaddr_len = sizeof(tud_network_mac_address);
    memcpy(netif->hwaddr, tud_network_mac_address, sizeof(tud_network_mac_address));
    netif->hwaddr[5] ^= 0x01;

    netif = netif_add(netif, &ipaddr, &netmask, &gateway, NULL, netif_init_cb, ip_input);
  #if LWIP_IPV6
    netif_create_ip6_linklocal_address(netif, 1);
  #endif
    netif_set_default(netif);
  }

  /* handle any DNS requests from dns-server */
  bool dns_query_proc(const char *name, ip4_addr_t *addr)
  {
    if (0 == strcmp(name, "tiny.usb"))
    {
      *addr = ipaddr;
      return true;
    }
    return false;
  }

  bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
  {
    /* this shouldn't happen, but if we get another packet before
    parsing the previous, we must signal our inability to accept it */
    if (received_frame) return false;

    if (size)
    {
      struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

      if (p)
      {
        /* pbuf_alloc() has already initialized struct; all we need to do is copy the data */
        memcpy(p->payload, src, size);

        /* store away the pointer for service_traffic() to later handle */
        received_frame = p;
      }
    }

    return true;
  }

  uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg)
  {
    struct pbuf *p = (struct pbuf *)ref;

    (void)arg; /* unused for this example */

    return pbuf_copy_partial(p, dst, p->tot_len, 0);
  }

  static void service_traffic(void)
  {
    /* handle any packet received by tud_network_recv_cb() */
    if (received_frame)
    {
      ethernet_input(received_frame, &netif_data);
      pbuf_free(received_frame);
      received_frame = NULL;
      tud_network_recv_renew();
    }

    sys_check_timeouts();
  }

  void tud_network_init_cb(void)
  {
    /* if the network is re-initializing and we have a leftover packet, we must do a cleanup */
    if (received_frame)
    {
      pbuf_free(received_frame);
      received_frame = NULL;
    }
  }


  typedef enum
  {
    NCM_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK = 0x01
  } ncm_data_interface_protocol_code_t;


  // Table 6.2 Class-Specific Request Codes for Network Control Model subclass
  typedef enum
  {
    NCM_SET_ETHERNET_MULTICAST_FILTERS               = 0x40,
    NCM_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER = 0x41,
    NCM_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER = 0x42,
    NCM_SET_ETHERNET_PACKET_FILTER                   = 0x43,
    NCM_GET_ETHERNET_STATISTIC                       = 0x44,
    NCM_GET_NTB_PARAMETERS                           = 0x80,
    NCM_GET_NET_ADDRESS                              = 0x81,
    NCM_SET_NET_ADDRESS                              = 0x82,
    NCM_GET_NTB_FORMAT                               = 0x83,
    NCM_SET_NTB_FORMAT                               = 0x84,
    NCM_GET_NTB_INPUT_SIZE                           = 0x85,
    NCM_SET_NTB_INPUT_SIZE                           = 0x86,
    NCM_GET_MAX_DATAGRAM_SIZE                        = 0x87,
    NCM_SET_MAX_DATAGRAM_SIZE                        = 0x88,
    NCM_GET_CRC_MODE                                 = 0x89,
    NCM_SET_CRC_MODE                                 = 0x8A,
  } ncm_request_code_t;

  /* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
  * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
  *
  * Auto ProductID layout's Bitmap:
  *   [MSB]       NET | VENDOR | MIDI | HID | MSC | CDC          [LSB]
  */
  #define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )
  #define USB_PID           (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
                            _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4) | _PID_MAP(ECM_RNDIS, 5) | _PID_MAP(NCM, 5) )

  // String Descriptor Index
  enum
  {
    STRID_LANGID = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
    STRID_INTERFACE,
    STRID_MAC
  };

  enum
  {
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
    ITF_NUM_TOTAL
  };

  enum
  {
  #if CFG_TUD_ECM_RNDIS
    CONFIG_ID_RNDIS = 0,
    CONFIG_ID_ECM   = 1,
  #else
    CONFIG_ID_NCM   = 0,
  #endif
    CONFIG_ID_COUNT
  };

  //--------------------------------------------------------------------+
  // Device Descriptors
  //--------------------------------------------------------------------+
  tusb_desc_device_t const desc_device =
  {
      .bLength            = sizeof(tusb_desc_device_t),
      .bDescriptorType    = TUSB_DESC_DEVICE,
      .bcdUSB             = 0x0200,

      // Use Interface Association Descriptor (IAD) device class
      .bDeviceClass       = TUSB_CLASS_MISC,
      .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
      .bDeviceProtocol    = MISC_PROTOCOL_IAD,

      .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

      .idVendor           = 0xCafe,
      .idProduct          = USB_PID,
      .bcdDevice          = 0x0101,

      .iManufacturer      = STRID_MANUFACTURER,
      .iProduct           = STRID_PRODUCT,
      .iSerialNumber      = STRID_SERIAL,

      .bNumConfigurations = CONFIG_ID_COUNT // multiple configurations
  };

  // Invoked when received GET DEVICE DESCRIPTOR
  // Application return pointer to descriptor
  uint8_t const * tud_descriptor_device_cb(void)
  {
    return (uint8_t const *) &desc_device;
  }

  //--------------------------------------------------------------------+
  // Configuration Descriptor
  //--------------------------------------------------------------------+
  #define MAIN_CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_RNDIS_DESC_LEN)
  #define ALT_CONFIG_TOTAL_LEN     (TUD_CONFIG_DESC_LEN + TUD_CDC_ECM_DESC_LEN)
  #define NCM_CONFIG_TOTAL_LEN     (TUD_CONFIG_DESC_LEN + TUD_CDC_NCM_DESC_LEN)

  #if CFG_TUSB_MCU == OPT_MCU_LPC175X_6X || CFG_TUSB_MCU == OPT_MCU_LPC177X_8X || CFG_TUSB_MCU == OPT_MCU_LPC40XX
    // LPC 17xx and 40xx endpoint type (bulk/interrupt/iso) are fixed by its number
    // 0 control, 1 In, 2 Bulk, 3 Iso, 4 In etc ...
    #define EPNUM_NET_NOTIF   0x81
    #define EPNUM_NET_OUT     0x02
    #define EPNUM_NET_IN      0x82

  #elif CFG_TUSB_MCU == OPT_MCU_SAMG  || CFG_TUSB_MCU ==  OPT_MCU_SAMX7X
    // SAMG & SAME70 don't support a same endpoint number with different direction IN and OUT
    //    e.g EP1 OUT & EP1 IN cannot exist together
    #define EPNUM_NET_NOTIF   0x81
    #define EPNUM_NET_OUT     0x02
    #define EPNUM_NET_IN      0x83

  #else
    #define EPNUM_NET_NOTIF   0x81
    #define EPNUM_NET_OUT     0x02
    #define EPNUM_NET_IN      0x82
  #endif

  #if CFG_TUD_ECM_RNDIS

  static uint8_t const rndis_configuration[] =
  {
    // Config number (index+1), interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(CONFIG_ID_RNDIS+1, ITF_NUM_TOTAL, 0, MAIN_CONFIG_TOTAL_LEN, 0, 100),

    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_RNDIS_DESCRIPTOR(ITF_NUM_CDC, STRID_INTERFACE, EPNUM_NET_NOTIF, 8, EPNUM_NET_OUT, EPNUM_NET_IN, CFG_TUD_NET_ENDPOINT_SIZE),
  };

  static uint8_t const ecm_configuration[] =
  {
    // Config number (index+1), interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(CONFIG_ID_ECM+1, ITF_NUM_TOTAL, 0, ALT_CONFIG_TOTAL_LEN, 0, 100),

    // Interface number, description string index, MAC address string index, EP notification address and size, EP data address (out, in), and size, max segment size.
    TUD_CDC_ECM_DESCRIPTOR(ITF_NUM_CDC, STRID_INTERFACE, STRID_MAC, EPNUM_NET_NOTIF, 64, EPNUM_NET_OUT, EPNUM_NET_IN, CFG_TUD_NET_ENDPOINT_SIZE, CFG_TUD_NET_MTU),
  };

  #else

  static uint8_t const ncm_configuration[] =
  {
    // Config number (index+1), interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(CONFIG_ID_NCM+1, ITF_NUM_TOTAL, 0, NCM_CONFIG_TOTAL_LEN, 0, 100),

    // Interface number, description string index, MAC address string index, EP notification address and size, EP data address (out, in), and size, max segment size.
    TUD_CDC_NCM_DESCRIPTOR(ITF_NUM_CDC, STRID_INTERFACE, STRID_MAC, EPNUM_NET_NOTIF, 64, EPNUM_NET_OUT, EPNUM_NET_IN, CFG_TUD_NET_ENDPOINT_SIZE, CFG_TUD_NET_MTU),
  };

  #endif

  // Configuration array: RNDIS and CDC-ECM
  // - Windows only works with RNDIS
  // - MacOS only works with CDC-ECM
  // - Linux will work on both
  static uint8_t const * const configuration_arr[2] =
  {
  #if CFG_TUD_ECM_RNDIS
    [CONFIG_ID_RNDIS] = rndis_configuration,
    [CONFIG_ID_ECM  ] = ecm_configuration
  #else
    [CONFIG_ID_NCM  ] = ncm_configuration
  #endif
  };

  // Invoked when received GET CONFIGURATION DESCRIPTOR
  // Application return pointer to descriptor
  // Descriptor contents must exist long enough for transfer to complete
  uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
  {
    return (index < CONFIG_ID_COUNT) ? configuration_arr[index] : NULL;
  }

  //--------------------------------------------------------------------+
  // String Descriptors
  //--------------------------------------------------------------------+

  // array of pointer to string descriptors
  static char const* string_desc_arr [] =
  {
    [STRID_LANGID]       = (const char[]) { 0x09, 0x04 }, // supported language is English (0x0409)
    [STRID_MANUFACTURER] = "TinyUSB",                     // Manufacturer
    [STRID_PRODUCT]      = "TinyUSB Device",              // Product
    [STRID_SERIAL]       = "123456",                      // Serial
    [STRID_INTERFACE]    = "TinyUSB Network Interface"    // Interface Description

    // STRID_MAC index is handled separately
  };

  static uint16_t _desc_str[32];

  // Invoked when received GET STRING DESCRIPTOR request
  // Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
  uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
  {
    (void) langid;

    unsigned int chr_count = 0;

    if (STRID_LANGID == index)
    {
      memcpy(&_desc_str[1], string_desc_arr[STRID_LANGID], 2);
      chr_count = 1;
    }
    else if (STRID_MAC == index)
    {
      // Convert MAC address into UTF-16

      for (unsigned i=0; i<sizeof(tud_network_mac_address); i++)
      {
        _desc_str[1+chr_count++] = "0123456789ABCDEF"[(tud_network_mac_address[i] >> 4) & 0xf];
        _desc_str[1+chr_count++] = "0123456789ABCDEF"[(tud_network_mac_address[i] >> 0) & 0xf];
      }
    }
    else
    {
      // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
      // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

      if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

      const char* str = string_desc_arr[index];

      // Cap at max char
      chr_count = strlen(str);
      if ( chr_count > (TU_ARRAY_SIZE(_desc_str) - 1)) chr_count = TU_ARRAY_SIZE(_desc_str) - 1;

      // Convert ASCII string into UTF-16
      for (unsigned int i=0; i<chr_count; i++)
      {
        _desc_str[1+i] = str[i];
      }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

    return _desc_str;
  }





  int mainRNDIS(void)
  {
    /* initialize TinyUSB */
    //board_init();
    //tusb_init();

    /* initialize lwip, dhcp-server, dns-server, and http */
    init_lwip();
    while (!netif_is_up(&netif_data));
    while (dhserv_init(&dhcp_config) != ERR_OK);
    //while (dnserv_init(IP_ADDR_ANY, 53, dns_query_proc) != ERR_OK);
    //httpd_init();
/*
    while (1)
    {
      tud_task();
      service_traffic();
    }
*/
    return 0;
  }

//   /* lwip has provision for using a mutex, when applicable */
//   sys_prot_t sys_arch_protect(void)
//   {
//     return 0;
//   }
//   void sys_arch_unprotect(sys_prot_t pval)
//   {
//     (void)pval;
//   }
//
//   /* lwip needs a millisecond time source, and the TinyUSB board support code has one available */
//   uint32_t sys_now(void)
//   {
//     return millis();
//   }



};



#endif
