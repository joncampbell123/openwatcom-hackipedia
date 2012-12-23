#include "zip.h"
#include "error.h"


int
main(int argc, char *argv[])
{
    struct zip *za, *destza;
    
    if (argc != 3) {
	fprintf(stderr, "%s: call with two options: src dest\n", argv[0]);
	return 1;
    }

    if ((za=zip_open(argv[1], ZIP_CHECKCONS))==NULL) {
	fprintf(stderr, "%s: %s: can't open file: %s\n", argv[0], argv[1],
		zip_err_str[zip_err]);
	return 1;
    }

    if ((destza=zip_open(argv[2], ZIP_CREATE))==NULL) {
	fprintf(stderr, "%s: %s: can't open file: %s\n", argv[0], argv[2],
		zip_err_str[zip_err]);
	return 1;
    }

    if (zip_add_zip(destza, NULL, NULL, za, 0, 0, 0) == -1)
	fprintf(stderr, "%s: %s: can't add file to zip '%s': %s\n", argv[0],
		za->entry[0].fn, argv[1], zip_err_str[zip_err]);
    
    if (zip_close(destza)!=0) {
	fprintf(stderr, "%s: %s: can't close file: %s\n", argv[0], argv[2],
		zip_err_str[zip_err]);
	return 1;
    }

    if (zip_close(za)!=0) {
	fprintf(stderr, "%s: %s: can't close file: %s\n", argv[0], argv[1],
		zip_err_str[zip_err]);
	return 1;
    }
    
    return 0;
}
