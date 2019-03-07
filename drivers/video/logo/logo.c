
/*
 *  Linux logo to be displayed on boot
 *
 *  Copyright (C) 1996 Larry Ewing (lewing@isc.tamu.edu)
 *  Copyright (C) 1996,1998 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 *  Copyright (C) 2001 Greg Banks <gnb@alphalink.com.au>
 *  Copyright (C) 2001 Jan-Benedict Glaw <jbglaw@lug-owl.de>
 *  Copyright (C) 2003 Geert Uytterhoeven <geert@linux-m68k.org>
 */

#include <linux/linux_logo.h>
#include <linux/stddef.h>
#include <linux/module.h>

#ifdef CONFIG_M68K
#include <asm/setup.h>
#endif

#include <linux/of_device.h>
#include <linux/of_platform.h>

static bool nologo;
module_param(nologo, bool, 0);
MODULE_PARM_DESC(nologo, "Disables startup logo");

/*
 * Logos are located in the initdata, and will be freed in kernel_init.
 * Use late_init to mark the logos as freed to prevent any further use.
 */

static bool logos_freed;

static int __init fb_logo_late_init(void)
{
	logos_freed = true;
	return 0;
}

late_initcall_sync(fb_logo_late_init);

#ifdef CONFIG_LOGO_OF_SELECT
const static inline struct linux_logo *assign_if_match(const char *of_logo, const struct linux_logo *logo)
{
	if (of_logo && strcmp(of_logo, logo->name) == 0) {
		return logo;
	}

	return NULL;
}
#endif

#ifndef CONFIG_LOGO_OF_SELECT
/* just assign the logo since it is not retrieved from the device tree */
#define ASSIGN_LOGO(x) logo = &x
#else
#define ASSIGN_LOGO(x) do {			     \
	if (!logo) {				     \
		logo = assign_if_match(of_logo, &x); \
	}					     \
} while (0)
#endif

/* logo's are marked __initdata. Use __ref to tell
 * modpost that it is intended that this function uses data
 * marked __initdata.
 */
const struct linux_logo * __ref fb_find_logo(int depth)
{
	const struct linux_logo *logo = NULL;
	const char *of_logo = NULL;

#ifdef CONFIG_LOGO_OF_SELECT
	struct device_node *of_chosen;
#endif

	if (nologo || logos_freed)
		return NULL;

#ifdef CONFIG_LOGO_OF_SELECT
	of_chosen = of_find_node_by_path("/chosen");
	if (of_chosen == NULL)
		of_chosen = of_find_node_by_path("/chosen@0");

	if (of_chosen != NULL)
		of_property_read_string(of_chosen, "logo", &of_logo);
#endif

	if (depth >= 1) {
#ifdef CONFIG_LOGO_LINUX_MONO
		/* Generic Linux logo */
		ASSIGN_LOGO(logo_linux_mono);
#endif
#ifdef CONFIG_LOGO_SUPERH_MONO
		/* SuperH Linux logo */
		ASSIGN_LOGO(logo_superh_mono);
#endif
	}
	
	if (depth >= 4) {
#ifdef CONFIG_LOGO_LINUX_VGA16
		/* Generic Linux logo */
		ASSIGN_LOGO(logo_linux_vga16);
#endif
#ifdef CONFIG_LOGO_SUPERH_VGA16
		/* SuperH Linux logo */
		ASSIGN_LOGO(logo_superh_vga16);
#endif
	}
	
	if (depth >= 8) {
#ifdef CONFIG_LOGO_LINUX_CLUT224
		/* Generic Linux logo */
		ASSIGN_LOGO(logo_linux_clut224);
#endif
#ifdef CONFIG_LOGO_DEC_CLUT224
		/* DEC Linux logo on MIPS/MIPS64 or ALPHA */
		ASSIGN_LOGO(logo_dec_clut224);
#endif
#ifdef CONFIG_LOGO_MAC_CLUT224
		/* Macintosh Linux logo on m68k */
		if (MACH_IS_MAC)
			ASSIGN_LOGO(logo_mac_clut224);
#endif
#ifdef CONFIG_LOGO_PARISC_CLUT224
		/* PA-RISC Linux logo */
		ASSIGN_LOGO(logo_parisc_clut224);
#endif
#ifdef CONFIG_LOGO_SGI_CLUT224
		/* SGI Linux logo on MIPS/MIPS64 */
		ASSIGN_LOGO(logo_sgi_clut224);
#endif
#ifdef CONFIG_LOGO_SUN_CLUT224
		/* Sun Linux logo */
		ASSIGN_LOGO(logo_sun_clut224);
#endif
#ifdef CONFIG_LOGO_SUPERH_CLUT224
		/* SuperH Linux logo */
		ASSIGN_LOGO(logo_superh_clut224);
#endif
#ifdef CONFIG_LOGO_BAW_1024X600_CLUT224
		ASSIGN_LOGO(logo_baw_1024x600_clut224);
#endif
#ifdef CONFIG_LOGO_BAW_800X480_CLUT224
		ASSIGN_LOGO(logo_baw_800x480_clut224);
#endif
#ifdef CONFIG_LOGO_BAW_480X272_CLUT224
		ASSIGN_LOGO(logo_baw_480x272_clut224);
#endif
	}
	return logo;
}
EXPORT_SYMBOL_GPL(fb_find_logo);
