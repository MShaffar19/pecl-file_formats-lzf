/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Marcin Gibula <mg@iceni.pl>                                  |
  +----------------------------------------------------------------------+

  $Id$
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_lzf.h"

#include "lzf.h"

/* {{{ lzf_functions[]
 *
 * Every user visible function must have an entry in lzf_functions[].
 */
function_entry lzf_functions[] = {
	PHP_FE(lzf_compress,		NULL)
	PHP_FE(lzf_decompress,		NULL)
	PHP_FE(lzf_optimized_for,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in lzf_functions[] */
};
/* }}} */

/* {{{ lzf_module_entry
 */
zend_module_entry lzf_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"lzf",
	lzf_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(lzf),
#if ZEND_MODULE_API_NO >= 20010901
	"1.0", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LZF
ZEND_GET_MODULE(lzf)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(lzf)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "lzf support", "enabled");
#if ULTRA_FAST
	php_info_print_table_row(2, "optimized for", "speed");
#else
	php_info_print_table_row(2, "optimized for", "compression quality");
#endif
	php_info_print_table_end();
}
/* }}} */


/* {{{ proto string lzf_compress(string arg)
   Return a string compressed with LZF */
PHP_FUNCTION(lzf_compress)
{
	char *retval, *arg = NULL;
	int arg_len, result;

	if (ZEND_NUM_ARGS() != 1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	retval = emalloc(arg_len + LZF_MARGIN);
    if (!retval) {
		RETURN_FALSE;
    }

	result = lzf_compress(arg, arg_len, retval, arg_len + LZF_MARGIN);
    if (result == 0) {
		efree(retval);
        RETURN_FALSE;
    }

	retval = erealloc(retval, result + 1);
    retval[result] = 0;

    RETURN_STRINGL(retval, result, 0);
}
/* }}} */

/* {{{ proto string lzf_decompress(string arg)
   Return a string decompressed with LZF */
PHP_FUNCTION(lzf_decompress)
{
	char *arg = NULL;
	int arg_len, result, i = 1;
    char *buffer;
    size_t buffer_size = 1024;

	if (ZEND_NUM_ARGS() != 1 || zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	buffer = emalloc(buffer_size);
    if (!buffer) {
		RETURN_FALSE;
    }

	do {
		buffer_size *= i++;
        buffer = erealloc(buffer, buffer_size);

        result = lzf_decompress(arg, arg_len, buffer, buffer_size);
    } while (result == 0 && errno == E2BIG);

	if (result == 0) {
		if (errno == EINVAL) {
        	php_error(E_WARNING, "%s LZF decompression failed, compressed data corrupted", get_active_function_name(TSRMLS_C));
		}

		efree(buffer);
        RETURN_FALSE;
    }

    buffer = erealloc(buffer, result + 1);
    buffer[result] = 0;

    RETURN_STRINGL(buffer, result, 0);
}
/* }}} */

/* {{{ proto int lzf_optimized_for()
   Return 1 if lzf was optimized for speed, 0 for compression */
PHP_FUNCTION(lzf_optimized_for)
{
	RETURN_LONG(ULTRA_FAST);
}

/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */