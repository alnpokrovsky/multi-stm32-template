#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

#include <string.h>
#include <stdlib.h>

#define USBD_VID                1155
#define USBD_PID_FS             22360
#define USBD_LANGID_STRING      1033
#define USBD_MANUFACTURER_STRING      "STMicroelectronics"
#define USBD_PRODUCT_STRING_FS        "STM32 Custom Human interface"
#define USBD_CONFIGURATION_STRING_FS  "Custom HID Config"
#define USBD_INTERFACE_STRING_FS      "Custom HID Interface"
#define USB_SIZ_BOS_DESC            0x0C


/*---------- -----------*/
#define USBD_MAX_NUM_INTERFACES     1U
/*---------- -----------*/
#define USBD_MAX_NUM_CONFIGURATION     1U
/*---------- -----------*/
#define USBD_MAX_STR_DESC_SIZ     256U
/*---------- -----------*/
#define USBD_DEBUG_LEVEL     0U
/*---------- -----------*/
#define USBD_LPM_ENABLED     1U
/*---------- -----------*/
#define USBD_SELF_POWERED     1U
/*---------- -----------*/
//#define USBD_CUSTOMHID_OUTREPORT_BUF_SIZE     2U
/*---------- -----------*/
//#define USBD_CUSTOM_HID_REPORT_DESC_SIZE     2U

/****************************************/
/* #define for FS and HS identification */
#define DEVICE_FS 		0
#define DEVICE_HS 		1


/* Memory management macros */

/** Alias for memory allocation. */
#define USBD_malloc         malloc

/** Alias for memory release. */
#define USBD_free           free

/** Alias for memory set. */
#define USBD_memset         memset

/** Alias for memory copy. */
#define USBD_memcpy         memcpy

/** Alias for delay. */
#define USBD_Delay          HAL_Delay

/* DEBUG macros */

#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLog(...)    printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 1)

#define USBD_ErrLog(...)    printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 2)
#define USBD_DbgLog(...)    printf("DEBUG : ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)
#endif


#endif