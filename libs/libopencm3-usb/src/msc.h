#ifndef __LIB_USB_PRIVATE_MSC_H__
#define __LIB_USB_PRIVATE_MSC_H__

#include <stdint.h>

usbd_mass_storage *custom_usb_msc_init(
	usbd_device *usbd_dev,
	uint8_t ep_in, uint8_t ep_in_size,
	uint8_t ep_out, uint8_t ep_out_size,
	const char *vendor_id,
	const char *product_id,
	const char *product_revision_level,
	const uint32_t block_count,
	int (*read_block)(uint32_t lba, uint8_t *copy_to),
	int (*write_block)(uint32_t lba, const uint8_t *copy_from),
	uint8_t msc_interface_index0 //  Index of MSC interface
);

#endif