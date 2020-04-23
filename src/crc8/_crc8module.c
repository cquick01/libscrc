/*
*********************************************************************************************************
*                              		(c) Copyright 2018-2020, Hexin
*                                           All Rights Reserved
* File    : _crc8module.c
* Author  : Heyn (heyunhuan@gmail.com)
* Version : V1.1
*
* LICENSING TERMS:
* ---------------
*		New Create at 	2017-08-10 08:42AM
*                       2017-08-17 [Heyn] Optimized code.
*                       2017-08-21 [Heyn] Optimization code for the C99 standard.
*                                         for ( unsigned int i=0; i<256; i++ ) -> for ( i=0; i<256; i++ )
*                       2017-08-22 [Heyn] Bugfixes Parsing arguments
*                                         Change PyArg_ParseTuple(* , "y#|I")
*                                         To     PyArg_ParseTuple(* , "y#|B")
*                                         "B" : Convert a Python integer to a tiny int without overflow
*                                               checking, stored in a C unsigned char.
*                       2017-09-21 [Heyn] Optimized code for _crc8_maxim
*                                         New CRC8-MAXIM8 CRC8-ROHC CRC8-ITU8 CRC8
*                       2020-02-17 [Heyn] New CRC8-SUM.
*                       2020-03-20 [Heyn] New CRC8-FLETCHER8.
*                       2020-04-17 [Heyn] Issues #1
*
*********************************************************************************************************
*/

#include <Python.h>
#include "_crc8tables.h"

static unsigned char hexin_PyArg_ParseTuple( PyObject *self, PyObject *args,
                                             unsigned char init,
                                             unsigned char (*function)( unsigned char *,
                                                                        unsigned int,
                                                                        unsigned char ),
                                             unsigned char *result )
{
    Py_buffer data = { NULL, NULL };

#if PY_MAJOR_VERSION >= 3
    if ( !PyArg_ParseTuple( args, "y*|B", &data, &init ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return FALSE;
    }
#else
    if ( !PyArg_ParseTuple( args, "s*|B", &data, &init ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return FALSE;
    }
#endif /* PY_MAJOR_VERSION */

    *result = function( (unsigned char *)data.buf, (unsigned int)data.len, init );

    if ( data.obj )
       PyBuffer_Release( &data );

    return TRUE;
}

static unsigned char hexin_PyArg_ParseTuple_Parameters( PyObject *self, PyObject *args, struct _crc8_parameters *param, unsigned char *result )
{
    Py_buffer data = { NULL, NULL };

#if PY_MAJOR_VERSION >= 3
    if ( !PyArg_ParseTuple( args, "y*", &data ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return FALSE;
    }
#else
    if ( !PyArg_ParseTuple( args, "s*", &data ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return FALSE;
    }
#endif /* PY_MAJOR_VERSION */

    param->data = ( unsigned char * )data.buf;
    param->size = ( unsigned int    )data.len;

    *result = hexin_calc_crc8_compute( param );

    if ( data.obj )
       PyBuffer_Release( &data );

    return TRUE;
}

static PyObject * _crc8_intel( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0x00;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_lrc, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_bcc( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0x00;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_bcc, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_lrc( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0x00;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_lrc, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_maxim( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0x00;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_maxim, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_rohc( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0xFF;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_rohc, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_itu( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0x00;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_07, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result ^ 0x55 );
}

static PyObject * _crc8( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0x00;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_07, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_sum( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0x00;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_sum, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

/*
*********************************************************************************************************
                                    Print CRC8 table.
*********************************************************************************************************
*/
static PyObject * _crc8_table( PyObject *self, PyObject *args )
{
    unsigned int i = 0x00000000L;
    unsigned char poly = CRC8_POLYNOMIAL_31;
    unsigned char table[MAX_TABLE_ARRAY] = { 0x00 };
    PyObject* plist = PyList_New( MAX_TABLE_ARRAY );

#if PY_MAJOR_VERSION >= 3
    if ( !PyArg_ParseTuple( args, "B", &poly ) )
        return NULL;
#else
    if ( !PyArg_ParseTuple( args, "B", &poly ) )
        return NULL;
#endif /* PY_MAJOR_VERSION */

    if ( HEXIN_POLYNOMIAL_IS_HIGH( poly ) ) {
        hexin_crc8_init_table_poly_is_high( poly, table );
    } else {
        hexin_crc8_init_table_poly_is_low ( poly, table );
    }

    for ( i=0; i<MAX_TABLE_ARRAY; i++ ) {
        PyList_SetItem( plist, i, Py_BuildValue( "B", table[i] ) );
    }

    return plist;
}

/*
*********************************************************************************************************
*                                   For hacker
*********************************************************************************************************
*/
static PyObject * _crc8_hacker( PyObject *self, PyObject *args, PyObject* kws )
{
    Py_buffer data = { NULL, NULL };
    unsigned char init    = 0xFF;
    unsigned char xorout  = 0x00;
    unsigned int  ref     = 0x00000000L;
    unsigned char result  = 0x00;
    unsigned char polynomial = CRC8_POLYNOMIAL_31;
    static char* kwlist[]={ "data", "poly", "init", "xorout", "ref", NULL };

#if PY_MAJOR_VERSION >= 3
    if ( !PyArg_ParseTupleAndKeywords( args, kws, "y*|BBBp", kwlist, &data, &polynomial, &init, &xorout, &ref ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return NULL;
    }
#else
    if ( !PyArg_ParseTupleAndKeywords( args, kws, "s*|BBBp", kwlist, &data, &polynomial, &init, &xorout, &ref ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return NULL;
    }
#endif /* PY_MAJOR_VERSION */

    if ( HEXIN_REFIN_OR_REFOUT_IS_TRUE( ref ) ) {
        polynomial = hexin_reverse8( polynomial );
    }

    result = hexin_calc_crc8_hacker( (unsigned char *)data.buf, (unsigned int)data.len, init, polynomial );
    result = result ^ xorout;

    if ( data.obj )
       PyBuffer_Release( &data );

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_fletcher( PyObject *self, PyObject *args )
{
    unsigned char result   = 0x00;
    unsigned char reserved = 0x00;
 
    if ( !hexin_PyArg_ParseTuple( self, args, reserved, hexin_calc_crc8_fletcher, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_smbus( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    static unsigned char crc8_smbus_table[MAX_TABLE_ARRAY] = { 0x00 };
    static struct _crc8_parameters param = { .is_initial = FALSE,
                                             .crc8  = 0x00,
                                             .poly  = 0x07,
                                             .table = crc8_smbus_table,
                                             .initial_table_function = hexin_crc8_init_table_poly_is_low };

    if ( !hexin_PyArg_ParseTuple_Parameters( self, args, &param, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_autosar8( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    static unsigned char crc8_autosar_table[MAX_TABLE_ARRAY] = { 0x00 };
    static struct _crc8_parameters param = { .is_initial = FALSE,
                                             .crc8  = 0xFF,
                                             .poly  = 0x2F,
                                             .table = crc8_autosar_table,
                                             .initial_table_function = hexin_crc8_init_table_poly_is_low };

    if ( !hexin_PyArg_ParseTuple_Parameters( self, args, &param, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", (result ^ 0xFF) );
}

static PyObject * _crc8_lte8( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    static unsigned char crc8_lte8_table[MAX_TABLE_ARRAY] = { 0x00 };
    static struct _crc8_parameters param = { .is_initial = FALSE,
                                             .crc8  = 0x00,
                                             .poly  = 0x9B,
                                             .table = crc8_lte8_table,
                                             .initial_table_function = hexin_crc8_init_table_poly_is_low };

    if ( !hexin_PyArg_ParseTuple_Parameters( self, args, &param, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_wcdma( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    static unsigned char crc8_wcdma_table[MAX_TABLE_ARRAY] = { 0x00 };
    static struct _crc8_parameters param = { .is_initial = FALSE,
                                             .crc8  = 0x00,
                                             .poly  = 0x9B,
                                             .table = crc8_wcdma_table,
                                             .initial_table_function = hexin_crc8_init_table_poly_is_high };

    param.poly = hexin_reverse8( param.poly );
    if ( !hexin_PyArg_ParseTuple_Parameters( self, args, &param, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_sae_j1855( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0xFF;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_1d, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result ^ 0xFF );
}

static PyObject * _crc8_icode( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0xFD;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_1d, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_gsm8_a( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0x00;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_1d, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_gsm8_b( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    static unsigned char crc8_gsmb_table[MAX_TABLE_ARRAY] = { 0x00 };
    static struct _crc8_parameters param = { .is_initial = FALSE,
                                             .crc8  = 0x00,
                                             .poly  = 0x49,
                                             .table = crc8_gsmb_table,
                                             .initial_table_function = hexin_crc8_init_table_poly_is_low };

    if ( !hexin_PyArg_ParseTuple_Parameters( self, args, &param, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", ( result ^ 0xFF ) );
}

static PyObject * _crc8_nrsc_5( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    unsigned char init   = 0xFF;
 
    if ( !hexin_PyArg_ParseTuple( self, args, init, hexin_calc_crc8_31, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_bluetooth( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    static unsigned char crc8_bluetooth_table[MAX_TABLE_ARRAY] = { 0x00 };
    static struct _crc8_parameters param = { .is_initial = FALSE,
                                             .crc8  = 0x00,
                                             .poly  = 0xA7,
                                             .table = crc8_bluetooth_table,
                                             .initial_table_function = hexin_crc8_init_table_poly_is_high };
    param.poly = hexin_reverse8( param.poly );
    if ( !hexin_PyArg_ParseTuple_Parameters( self, args, &param, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_dvb_s2( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    static unsigned char crc8_dvb_s2_table[MAX_TABLE_ARRAY] = { 0x00 };
    static struct _crc8_parameters param = { .is_initial = FALSE,
                                             .crc8  = 0x00,
                                             .poly  = 0xD5,
                                             .table = crc8_dvb_s2_table,
                                             .initial_table_function = hexin_crc8_init_table_poly_is_low };

    if ( !hexin_PyArg_ParseTuple_Parameters( self, args, &param, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_ebu8( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    static unsigned char crc8_ebu8_table[MAX_TABLE_ARRAY] = { 0x00 };
    static struct _crc8_parameters param = { .is_initial = FALSE,
                                             .crc8  = 0xFF,
                                             .poly  = 0x1D,
                                             .table = crc8_ebu8_table,
                                             .initial_table_function = hexin_crc8_init_table_poly_is_high };
    param.poly = hexin_reverse8( param.poly );
    if ( !hexin_PyArg_ParseTuple_Parameters( self, args, &param, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_darc( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    static unsigned char crc8_darc_table[MAX_TABLE_ARRAY] = { 0x00 };
    static struct _crc8_parameters param = { .is_initial = FALSE,
                                             .crc8  = 0x00,
                                             .poly  = 0x39,
                                             .table = crc8_darc_table,
                                             .initial_table_function = hexin_crc8_init_table_poly_is_high };
    param.poly = hexin_reverse8( param.poly );
    if ( !hexin_PyArg_ParseTuple_Parameters( self, args, &param, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

static PyObject * _crc8_opensafety8( PyObject *self, PyObject *args )
{
    unsigned char result = 0x00;
    static unsigned char crc8_opensafety_table[MAX_TABLE_ARRAY] = { 0x00 };
    static struct _crc8_parameters param = { .is_initial = FALSE,
                                             .crc8  = 0x00,
                                             .poly  = 0x2F,
                                             .table = crc8_opensafety_table,
                                             .initial_table_function = hexin_crc8_init_table_poly_is_low };

    if ( !hexin_PyArg_ParseTuple_Parameters( self, args, &param, &result ) ) {
        return NULL;
    }

    return Py_BuildValue( "B", result );
}

/* method table */
static PyMethodDef _crc8Methods[] = {
    { "intel",      (PyCFunction)_crc8_intel,        METH_VARARGS, "Calculate Intel hexadecimal of CRC8 [Initial = 0x00]" },
    { "bcc",        (PyCFunction)_crc8_bcc,          METH_VARARGS, "Calculate BCC of CRC8 [Initial = 0x00]" },
    { "lrc",        (PyCFunction)_crc8_lrc,          METH_VARARGS, "Calculate LRC of CRC8 [Initial = 0x00]" },
    { "maxim8",     (PyCFunction)_crc8_maxim,        METH_VARARGS, "Calculate MAXIM of CRC8 [Poly = 0x31 Initial = 0x00 Xorout=0x00 Refin=True Refout=True] e.g. DS18B20" },
    { "rohc",       (PyCFunction)_crc8_rohc,         METH_VARARGS, "Calculate ROHC of CRC8 [Poly = 0x07 Initial = 0xFF Xorout=0x00 Refin=True Refout=True]" },
    { "itu8",       (PyCFunction)_crc8_itu,          METH_VARARGS, "Calculate ITU  of CRC8 [Poly = 0x07 Initial = 0x00 Xorout=0x55 Refin=False Refout=False]" },
    { "crc8",       (PyCFunction)_crc8,              METH_VARARGS, "Calculate CRC  of CRC8 [Poly = 0x07 Initial = 0x00 Xorout=0x00 Refin=False Refout=False]" },
    { "sum8",       (PyCFunction)_crc8_sum,          METH_VARARGS, "Calculate SUM  of CRC8 [Initial = 0x00]" },
    { "table8",     (PyCFunction)_crc8_table,        METH_VARARGS, "Print CRC8 table to list. libscrc.table8( polynomial )" },
    { "hacker8",    (PyCFunction)_crc8_hacker,       METH_KEYWORDS|METH_VARARGS, "User calculation CRC8\n"
                                                                                 "@data   : bytes\n"
                                                                                 "@poly   : default=0x31\n"
                                                                                 "@init   : default=0xFF\n"
                                                                                 "@xorout : default=0x00\n"
                                                                                 "@ref    : default=False" },
    { "fletcher8",  (PyCFunction)_crc8_fletcher,     METH_VARARGS, "Calculate fletcher8" },
    { "smbus",      (PyCFunction)_crc8_smbus,        METH_VARARGS, "Calculate SMBUS of CRC8 [Poly = 0x07 Initial = 0x00 Xorout=0x00 Refin=True Refout=True]" },
    { "autosar8",   (PyCFunction)_crc8_autosar8,     METH_VARARGS, "Calculate AUTOSAR of CRC8 [Poly = 0x2F Initial = 0xFF Xorout=0xFF Refin=True Refout=True]" },
    { "lte8",       (PyCFunction)_crc8_lte8,         METH_VARARGS, "Calculate LTE of CRC8 [Poly = 0x9B Initial = 0x00 Xorout=0x00 Refin=True Refout=True]" },
    { "sae_j1855",  (PyCFunction)_crc8_sae_j1855,    METH_VARARGS, "Calculate SAE-J1855 of CRC8 [Poly = 0x1D Initial = 0xFF Xorout=0xFF Refin=True Refout=True]" },
    { "icode",      (PyCFunction)_crc8_icode,        METH_VARARGS, "Calculate I-CODE of CRC8 [Poly = 0x1D Initial = 0xFD Xorout=0x00 Refin=True Refout=True]" },
    { "gsm8_a",     (PyCFunction)_crc8_gsm8_a,       METH_VARARGS, "Calculate GSM8-A of CRC8 [Poly = 0x1D Initial = 0x00 Xorout=0x00 Refin=True Refout=True]" },
    { "gsm8_b",     (PyCFunction)_crc8_gsm8_b,       METH_VARARGS, "Calculate GSM8-B of CRC8 [Poly = 0x49 Initial = 0x00 Xorout=0xFF Refin=True Refout=True]" },
    { "nrsc_5",     (PyCFunction)_crc8_nrsc_5,       METH_VARARGS, "Calculate NRSC-5 of CRC8 [Poly = 0x31 Initial = 0xFF Xorout=0x00 Refin=True Refout=True]" },
    { "wcdma",      (PyCFunction)_crc8_wcdma,        METH_VARARGS, "Calculate WCDMA of CRC8 [Poly = 0x9B Initial = 0x00 Xorout=0x00 Refin=True Refout=True]" },
    { "bluetooth",  (PyCFunction)_crc8_bluetooth,    METH_VARARGS, "Calculate BLUETOOTH of CRC8 [Poly = 0xA7 Initial = 0x00 Xorout=0x00 Refin=True Refout=True]" },
    { "dvb_s2",     (PyCFunction)_crc8_dvb_s2,       METH_VARARGS, "Calculate DVB-S2 of CRC8 [Poly = 0xD5 Initial = 0x00 Xorout=0x00 Refin=True Refout=True]" },
    { "ebu8",       (PyCFunction)_crc8_ebu8,         METH_VARARGS, "Calculate EBU of CRC8 [Poly = 0x1D Initial = 0xFF Xorout=0x00 Refin=True Refout=True]" },
    { "darc",       (PyCFunction)_crc8_darc,         METH_VARARGS, "Calculate DARC of CRC8 [Poly = 0x39 Initial = 0x00 Xorout=0x00 Refin=True Refout=True]" },
    { "opensafety8",(PyCFunction)_crc8_opensafety8,  METH_VARARGS, "Calculate OPENSAFETY of CRC8 [Poly = 0x2F Initial = 0x00 Xorout=0x00 Refin=True Refout=True]" },
    
    { NULL, NULL, 0, NULL }        /* Sentinel */
};


/* module documentation */
PyDoc_STRVAR( _crc8_doc,
"Calculation of CRC8 \n"
"libscrc.intel      -> Calculate Intel hexadecimal of CRC8 [Initial = 0x00]\n"
"libscrc.bcc        -> Calculate BCC(XOR) of CRC8 [Initial = 0x00]\n"
"libscrc.lrc        -> Calculate LRC of CRC8 [Initial = 0x00]\n"
"libscrc.maxim8     -> Calculate MAXIM of CRC8 [Poly = 0x31 Initial = 0x00 Xorout=0x00 Refin=True Refout=True] e.g. DS18B20\n"
"libscrc.rohc       -> Calculate ROHC of CRC8 [Poly = 0x07 Initial = 0xFF Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.itu8       -> Calculate ITU  of CRC8 [Poly = 0x07 Initial = 0x00 Xorout=0x55 Refin=False Refout=False]\n"
"libscrc.crc8       -> Calculate CRC  of CRC8 [Poly = 0x07 Initial = 0x00 Xorout=0x00 Refin=False Refout=False]\n"
"libscrc.sum8       -> Calculate SUM  of CRC8 [Initial = 0x00]\n"
"libscrc.hacker8    -> Free calculation CRC8 [poly=any(default=0x31), init=any(default=0xFF), xorout=0x00 Refin=False Refout=False]\n"
"libscrc.fletcher8  -> Calculate fletcher8 \n"
"libscrc.smbus      -> Calculate SMBUS of CRC8 [Poly = 0x07 Initial = 0x00 Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.autosar8   -> Calculate AUTOSAR of CRC8 [Poly = 0x2F Initial = 0xFF Xorout=0xFF Refin=True Refout=True]\n"
"libscrc.lte8       -> Calculate LTE of CRC8 [Poly = 0x9B Initial = 0x00 Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.sae_j1855  -> Calculate SAE-J1855 of CRC8 [Poly = 0x1D Initial = 0xFF Xorout=0xFF Refin=True Refout=True]\n"
"libscrc.icode      -> Calculate I-CODE of CRC8 [Poly = 0x1D Initial = 0xFD Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.gsm8_a     -> Calculate GSM8-A of CRC8 [Poly = 0x1D Initial = 0x00 Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.gsm8_b     -> Calculate GSM8-B of CRC8 [Poly = 0x49 Initial = 0x00 Xorout=0xFF Refin=True Refout=True]\n"
"libscrc.nrsc_5     -> Calculate NRSC-5 of CRC8 [Poly = 0x31 Initial = 0xFF Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.wcdma      -> Calculate WCDMA of CRC8 [Poly = 0x9B Initial = 0x00 Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.bluetooth  -> Calculate BLUETOOTH of CRC8 [Poly = 0xA7 Initial = 0x00 Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.dvb_s2     -> Calculate DVB-S2 of CRC8 [Poly = 0xD5 Initial = 0x00 Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.ebu8       -> Calculate EBU of CRC8 [Poly = 0x1D Initial = 0xFF Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.darc       -> Calculate DARC of CRC8 [Poly = 0x39 Initial = 0x00 Xorout=0x00 Refin=True Refout=True]\n"
"libscrc.opensafety8-> Calculate OPENSAFETY of CRC8 [Poly = 0x2F Initial = 0x00 Xorout=0x00 Refin=True Refout=True]\n"
"\n" );


#if PY_MAJOR_VERSION >= 3

/* module definition structure */
static struct PyModuleDef _crc8module = {
   PyModuleDef_HEAD_INIT,
   "_crc8",                    /* name of module */
   _crc8_doc,                  /* module documentation, may be NULL */
   -1,                         /* size of per-interpreter state of the module */
   _crc8Methods
};

/* initialization function for Python 3 */
PyMODINIT_FUNC
PyInit__crc8( void )
{
    PyObject *m = NULL;

    m = PyModule_Create( &_crc8module );
    if ( m == NULL ) {
        return NULL;
    }

    PyModule_AddStringConstant( m, "__version__", "1.1"   );
    PyModule_AddStringConstant( m, "__author__",  "Heyn"  );

    return m;
}

#else

/* initialization function for Python 2 */
PyMODINIT_FUNC
init_crc8( void )
{
    (void) Py_InitModule3( "_crc8", _crc8Methods, _crc8_doc );
}

#endif /* PY_MAJOR_VERSION */
