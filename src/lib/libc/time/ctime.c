/*
 * libc/time/ctime.c
 */

#include <xboot/module.h>
#include <time.h>

char * ctime(const time_t * t)
{
	return asctime((const struct tm *)localtime(t));
}
EXPORT_SYMBOL(ctime);
