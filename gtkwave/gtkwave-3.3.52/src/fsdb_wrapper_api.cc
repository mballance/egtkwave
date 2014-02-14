/*
 * Copyright (c) Tony Bybell 2013.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <config.h>
#include "fsdb_wrapper_api.h"

#ifdef WAVE_FSDB_READER_IS_PRESENT

#ifdef NOVAS_FSDB
#undef NOVAS_FSDB
#endif

#include "ffrAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

static bool_T __TreeCB(fsdbTreeCBType cb_type, void *client_data, void *tree_cb_data)
{
return(TRUE); /* currently unused along with var/scope traversal */
}

static bool_T __MyTreeCB(fsdbTreeCBType cb_type, void *client_data, void *tree_cb_data);


extern "C" void *fsdbReaderOpenFile(char *nam)
{
fsdbFileType ft;

if(!ffrObject::ffrIsFSDB(nam))
	{
	return(NULL);
	}

ffrFSDBInfo fsdb_info;
ffrObject::ffrGetFSDBInfo(nam, fsdb_info);
if((fsdb_info.file_type != FSDB_FT_VERILOG) && (fsdb_info.file_type != FSDB_FT_VERILOG_VHDL) && (fsdb_info.file_type != FSDB_FT_VHDL))
	{
	return(NULL);
	}

ffrObject *fsdb_obj = ffrObject::ffrOpen3(nam);
if(!fsdb_obj)
	{
	return(NULL);
	}

fsdb_obj->ffrSetTreeCBFunc(__TreeCB, NULL);
    
ft = fsdb_obj->ffrGetFileType();
if((ft != FSDB_FT_VERILOG) && (ft != FSDB_FT_VERILOG_VHDL) && (ft != FSDB_FT_VHDL))
	{
        fsdb_obj->ffrClose();
	return(NULL);
	}

return((void *)fsdb_obj);
}


extern "C" void fsdbReaderReadScopeVarTree(void *ctx,void (*cb)(void *))
{
ffrObject *fsdb_obj = (ffrObject *)ctx;

fsdb_obj->ffrSetTreeCBFunc(__MyTreeCB, (void *) cb);
fsdb_obj->ffrReadScopeVarTree();
}


extern "C" int fsdbReaderGetMaxVarIdcode(void *ctx)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
fsdbVarIdcode max_var_idcode = fsdb_obj->ffrGetMaxVarIdcode();
return(max_var_idcode);
}


extern "C" void fsdbReaderAddToSignalList(void *ctx, int i)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
fsdb_obj->ffrAddToSignalList(i);
}


extern "C" void fsdbReaderLoadSignals(void *ctx)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
fsdb_obj->ffrLoadSignals();
}


extern "C" void *fsdbReaderCreateVCTraverseHandle(void *ctx, int i)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
ffrVCTrvsHdl hdl = fsdb_obj->ffrCreateVCTraverseHandle(i);
return((void *)hdl);
}


extern "C" int fsdbReaderHasIncoreVC(void *ctx, void *hdl)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;
return(fsdb_hdl->ffrHasIncoreVC() == TRUE);
}


extern "C" void fsdbReaderFree(void *ctx, void *hdl)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;

fsdb_hdl->ffrFree();
}


extern "C" uint64_t fsdbReaderGetMinXTag(void *ctx, void *hdl)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;
fsdbTag64 timetag;

fsdb_hdl->ffrGetMinXTag((void*)&timetag);
uint64_t rv = (((uint64_t)timetag.H) << 32) | ((uint64_t)timetag.L);
return(rv);
}


extern "C" uint64_t fsdbReaderGetMaxXTag(void *ctx, void *hdl)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;
fsdbTag64 timetag;

fsdb_hdl->ffrGetMaxXTag((void*)&timetag);
uint64_t rv = (((uint64_t)timetag.H) << 32) | ((uint64_t)timetag.L);
return(rv);
}


extern "C" void fsdbReaderGotoXTag(void *ctx, void *hdl, uint64_t tim)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;
fsdbTag64 timetag;

timetag.H = (uint32_t)(tim >> 32);
timetag.L = (uint32_t)(tim & 0xFFFFFFFFUL);

fsdb_hdl->ffrGotoXTag((void*)&timetag);
}


extern "C" uint64_t fsdbReaderGetXTag(void *ctx, void *hdl)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;
fsdbTag64 timetag;

fsdb_hdl->ffrGetXTag((void*)&timetag);
uint64_t rv = (((uint64_t)timetag.H) << 32) | ((uint64_t)timetag.L);
return(rv);
}


extern "C" int fsdbReaderGetVC(void *ctx, void *hdl, void **val_ptr)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;

return(fsdb_hdl->ffrGetVC((byte_T**)val_ptr) == FSDB_RC_SUCCESS);
}


extern "C" int fsdbReaderGotoNextVC(void *ctx, void *hdl)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;

return(fsdb_hdl->ffrGotoNextVC() == FSDB_RC_SUCCESS);
}


extern "C" void fsdbReaderUnloadSignals(void *ctx)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
fsdb_obj->ffrUnloadSignals();
}


extern "C" void fsdbReaderClose(void *ctx)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
fsdb_obj->ffrClose();
}


extern "C" int fsdbReaderGetBytesPerBit(void *hdl)
{
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;

return(fsdb_hdl->ffrGetBytesPerBit());
}


extern "C" int fsdbReaderGetBitSize(void *hdl)
{
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;

return(fsdb_hdl->ffrGetBitSize());
}


extern "C" int fsdbReaderGetVarType(void *hdl)
{
ffrVCTrvsHdl fsdb_hdl = (ffrVCTrvsHdl)hdl;

return(fsdb_hdl->ffrGetVarType());
}


extern "C" char *fsdbReaderTranslateVC(void *hdl, void *val_ptr)
{ 
ffrVCTrvsHdl vc_trvs_hdl = (ffrVCTrvsHdl)hdl;
byte_T *vc_ptr = (byte_T *)val_ptr;

static byte_T buffer[FSDB_MAX_BIT_SIZE+1];
uint_T i;
fsdbVarType   var_type; 
    
switch (vc_trvs_hdl->ffrGetBytesPerBit()) 
	{
	case FSDB_BYTES_PER_BIT_1B:
	        for (i = 0; i < vc_trvs_hdl->ffrGetBitSize(); i++) 
			{
		    	switch(vc_ptr[i]) 
				{
	 	    		case FSDB_BT_VCD_0:
		        		buffer[i] = '0';
		        		break;
	
		    		case FSDB_BT_VCD_1:
		        		buffer[i] = '1';
		        		break;

		    		case FSDB_BT_VCD_X:
		        		buffer[i] = 'x';
		        		break;
	
		    		case FSDB_BT_VCD_Z:
		        		buffer[i] = 'z';
		        		break;
	
		    		default:
		        		buffer[i] = 'u';
					break;
		    		}
	        	}
	        buffer[i] = 0;
		break;

	case FSDB_BYTES_PER_BIT_4B:
		var_type = vc_trvs_hdl->ffrGetVarType();
		switch(var_type)
			{
			case FSDB_VT_VCD_MEMORY_DEPTH:
			case FSDB_VT_VHDL_MEMORY_DEPTH:
				buffer[0] = 0;
				break;
	               
			default:    
				vc_trvs_hdl->ffrGetVC(&vc_ptr);
				sprintf((char *)buffer, "%f", *((float*)vc_ptr));
				break;
			}
		break;
	
	case FSDB_BYTES_PER_BIT_8B:
		sprintf((char *)buffer, "%lg", *((double*)vc_ptr));
		break;

	default:
		buffer[0] = 0;
		break;
	}

return((char *)buffer);
}


extern "C" int fsdbReaderExtractScaleUnit(void *ctx, int *mult, char *scale)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
uint_T digit;
char *unit;

str_T su = fsdb_obj->ffrGetScaleUnit();
fsdbRC rc = fsdb_obj->ffrExtractScaleUnit(su, digit, unit);

if(rc == FSDB_RC_SUCCESS)
	{
	*mult = digit ? ((int)digit) : 1; /* in case digit is zero */
	*scale = unit[0];
	}

return(rc == FSDB_RC_SUCCESS);
}


extern "C" int fsdbReaderGetMinFsdbTag64(void *ctx, uint64_t *tim)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
fsdbTag64 tag64;
fsdbRC rc = fsdb_obj->ffrGetMinFsdbTag64(&tag64);

if(rc == FSDB_RC_SUCCESS)
	{
	*tim = (((uint64_t)tag64.H) << 32) | ((uint64_t)tag64.L);
	}

return(rc == FSDB_RC_SUCCESS);
}


extern "C" int fsdbReaderGetMaxFsdbTag64(void *ctx, uint64_t *tim)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
fsdbTag64 tag64;
fsdbRC rc = fsdb_obj->ffrGetMaxFsdbTag64(&tag64);

if(rc == FSDB_RC_SUCCESS)
	{
	*tim = (((uint64_t)tag64.H) << 32) | ((uint64_t)tag64.L);
	}

return(rc == FSDB_RC_SUCCESS);
}


static bool_T __fsdbReaderGetStatisticsCB(fsdbTreeCBType cb_type, void *client_data, void *tree_cb_data)
{
struct fsdbReaderGetStatistics_t *gs = (struct fsdbReaderGetStatistics_t *)client_data;

switch (cb_type)
	{
	case FSDB_TREE_CBT_VAR:		gs->varCount++;
					break;
	case FSDB_TREE_CBT_SCOPE:	gs->scopeCount++;
					break;

	default:			break;
	}

return(TRUE);
}


extern "C" struct fsdbReaderGetStatistics_t *fsdbReaderGetStatistics(void *ctx)
{
ffrObject *fsdb_obj = (ffrObject *)ctx;
struct fsdbReaderGetStatistics_t *gs = (struct fsdbReaderGetStatistics_t *)calloc(1, sizeof(struct fsdbReaderGetStatistics_t));

fsdb_obj->ffrSetTreeCBFunc(__fsdbReaderGetStatisticsCB, gs);
fsdb_obj->ffrReadScopeVarTree();

return(gs);
}


static void 
__DumpScope(fsdbTreeCBDataScope* scope, void (*cb)(void *))
{
str_T type;
char bf[65537];

switch (scope->type) 
	{
    	case FSDB_ST_VCD_MODULE:
		type = (str_T) "vcd_module"; 
		break;

	case FSDB_ST_VCD_TASK:
		type = (str_T) "vcd_task"; 
		break;

	case FSDB_ST_VCD_FUNCTION:
		type = (str_T) "vcd_function"; 
		break;
	
	case FSDB_ST_VCD_BEGIN:
		type = (str_T) "vcd_begin"; 
		break;
	
	case FSDB_ST_VCD_FORK:
		type = (str_T) "vcd_fork"; 
		break;

	case FSDB_ST_VCD_GENERATE:
		type = (str_T) "vcd_generate"; 
		break;

	case FSDB_ST_SV_INTERFACE:
		type = (str_T) "sv_interface"; 
		break;

	case FSDB_ST_VHDL_ARCHITECTURE:
		type = (str_T) "vhdl_architecture";
		break;

	case FSDB_ST_VHDL_PROCEDURE:
		type = (str_T) "vhdl_procedure";
		break;

	case FSDB_ST_VHDL_FUNCTION:
		type = (str_T) "vhdl_function";
		break;

	case FSDB_ST_VHDL_RECORD:
		type = (str_T) "vhdl_record";
		break;

	case FSDB_ST_VHDL_PROCESS:
		type = (str_T) "vhdl_process";
		break;

	case FSDB_ST_VHDL_BLOCK:
		type = (str_T) "vhdl_block";
		break;

	case FSDB_ST_VHDL_FOR_GENERATE:
		type = (str_T) "vhdl_for_generate";
		break;

	case FSDB_ST_VHDL_IF_GENERATE:
		type = (str_T) "vhdl_if_generate";
		break;

	case FSDB_ST_VHDL_GENERATE:
		type = (str_T) "vhdl_generate";
		break;

	default:
		type = (str_T) "unknown_scope_type";
		break;
    	}

sprintf(bf, "Scope: %s %s %s", type, scope->name, scope->module ? scope->module : "NULL");
cb(bf);
}


static char* itoa_2(int value, char* result)
{
char* ptr = result, *ptr1 = result, tmp_char;
int tmp_value;

do {
        tmp_value = value;
        value /= 10; 
        *ptr++ = "9876543210123456789" [9 + (tmp_value - value * 10)];
} while ( value );

if (tmp_value < 0) *ptr++ = '-';
result = ptr;
*ptr-- = '\0';
while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
}
return(result);
}  


static void 
__DumpVar(fsdbTreeCBDataVar *var, void (*cb)(void *))
{
str_T type;
str_T bpb;
str_T direction;
char *pnt;
int len;
int typelen;
int dirlen;
char bf[65537];

switch(var->bytes_per_bit) 
	{
   	case FSDB_BYTES_PER_BIT_1B:
		bpb = (str_T) "1B";
		break;

    	case FSDB_BYTES_PER_BIT_2B:
		bpb = (str_T) "2B";
		break;

    	case FSDB_BYTES_PER_BIT_4B:
		bpb = (str_T) "4B";
		break;

    	case FSDB_BYTES_PER_BIT_8B:
		bpb = (str_T) "8B";
		break;

    	default:
		bpb = (str_T) "XB";
		break;
	}

switch (var->type) 
	{
    	case FSDB_VT_VCD_EVENT:
		type = (str_T) "vcd_event"; 
		typelen = 9;
  		break;

    	case FSDB_VT_VCD_INTEGER:
		type = (str_T) "vcd_integer"; 
		typelen = 11;
		break;

    	case FSDB_VT_VCD_PARAMETER:
		type = (str_T) "vcd_parameter"; 
		typelen = 13;
		break;

    	case FSDB_VT_VCD_REAL:
		type = (str_T) "vcd_real"; 
		typelen = 8;
		break;

    	case FSDB_VT_VCD_REG:
		type = (str_T) "vcd_reg"; 
		typelen = 7;
		break;

    	case FSDB_VT_VCD_SUPPLY0:
		type = (str_T) "vcd_supply0"; 
		typelen = 11;
		break;

    	case FSDB_VT_VCD_SUPPLY1:
		type = (str_T) "vcd_supply1"; 
		typelen = 11;
		break;

    	case FSDB_VT_VCD_TIME:
		type = (str_T) "vcd_time";
		typelen = 8;
		break;

    	case FSDB_VT_VCD_TRI:
		type = (str_T) "vcd_tri";
		typelen = 7;
		break;

    	case FSDB_VT_VCD_TRIAND:
		type = (str_T) "vcd_triand";
		typelen = 10;
		break;

    	case FSDB_VT_VCD_TRIOR:
		type = (str_T) "vcd_trior";
		typelen = 9;
		break;

    	case FSDB_VT_VCD_TRIREG:
		type = (str_T) "vcd_trireg";
		typelen = 10;
		break;

    	case FSDB_VT_VCD_TRI0:
		type = (str_T) "vcd_tri0";
		typelen = 8;
		break;

    	case FSDB_VT_VCD_TRI1:
		type = (str_T) "vcd_tri1";
		typelen = 8;
		break;

    	case FSDB_VT_VCD_WAND:
		type = (str_T) "vcd_wand";
		typelen = 8;
		break;

    	case FSDB_VT_VCD_WIRE:
		type = (str_T) "vcd_wire";
		typelen = 8;
		break;

    	case FSDB_VT_VCD_WOR:
		type = (str_T) "vcd_wor";
		typelen = 7;
		break;

    	case FSDB_VT_VHDL_SIGNAL:
    	case FSDB_VT_VHDL_VARIABLE:
    	case FSDB_VT_VHDL_CONSTANT:
    	case FSDB_VT_VHDL_FILE:
    	case FSDB_VT_VCD_MEMORY:
    	case FSDB_VT_VHDL_MEMORY:
    	case FSDB_VT_VCD_MEMORY_DEPTH:
    	case FSDB_VT_VHDL_MEMORY_DEPTH:         
		switch(var->vc_dt)
			{
			case FSDB_VC_DT_FLOAT:
			case FSDB_VC_DT_DOUBLE:
				type = (str_T) "vcd_real";
				typelen = 8;
				break;

			case FSDB_VC_DT_UNKNOWN:
			case FSDB_VC_DT_BYTE:
			case FSDB_VC_DT_SHORT:
			case FSDB_VC_DT_INT:
			case FSDB_VC_DT_LONG:
			case FSDB_VC_DT_HL_INT:
			case FSDB_VC_DT_PHYSICAL:
			default:
				if(var->type == FSDB_VT_VHDL_SIGNAL)
					{
					type = (str_T) "vcd_wire";
					typelen = 8;
					}
				else
					{
					type = (str_T) "vcd_reg";
					typelen = 7;
					}
				break;
			}
		break;

    	default:
		type = (str_T) "vcd_wire";
		typelen = 8;
		break;
    	}

    switch(var->direction)
	{
	case FSDB_VD_INPUT:    	direction = (str_T) "input"; 	dirlen = 5; break;
	case FSDB_VD_OUTPUT:   	direction = (str_T) "output"; 	dirlen = 6; break;
	case FSDB_VD_INOUT:    	direction = (str_T) "inout"; 	dirlen = 5; break;
	case FSDB_VD_BUFFER:   	direction = (str_T) "buffer"; 	dirlen = 6; break;
	case FSDB_VD_LINKAGE:  	direction = (str_T) "linkage"; 	dirlen = 7; break;
	case FSDB_VD_IMPLICIT: 
	default:	       	direction = (str_T) "implicit"; dirlen = 8; break;
	}

/*
sprintf(bf, "Var: %s %s l:%d r:%d %s %d %s %d", type, var->name, var->lbitnum, var->rbitnum, 
	direction,
	var->u.idcode, bpb, var->dtidcode);
*/

memcpy(bf, "Var: ", 5);
pnt = bf+5;
len = typelen; /* strlen(type) */
memcpy(pnt, type, len);
pnt += len;
*(pnt++) = ' ';
len = strlen(var->name);
memcpy(pnt, var->name, len);
pnt += len;
memcpy(pnt, " l:", 3);
pnt += 3;
pnt = itoa_2(var->lbitnum, pnt);
memcpy(pnt, " r:", 3);
pnt += 3;
pnt = itoa_2(var->rbitnum, pnt);
*(pnt++) = ' ';
len = dirlen; /* strlen(direction) */
memcpy(pnt, direction, len);    
pnt += len;
*(pnt++) = ' ';
pnt = itoa_2(var->u.idcode, pnt);
*(pnt++) = ' ';
len = 2; /* strlen(bpb) */
memcpy(pnt, bpb, len);    
pnt += len;
*(pnt++) = ' ';
pnt = itoa_2(var->dtidcode, pnt);
*(pnt) = 0;

cb(bf);
}


static void 
__DumpStruct(fsdbTreeCBDataStructBegin* str, void (*cb)(void *))
{
char bf[65537];

/* printf("NAME: %s FIELDS: %d TYPE: %d is_partial_dumped: %d\n", str->name, (int)str->fieldCount, (int)str->type, (int)str->is_partial_dumped); */

sprintf(bf, "Scope: vcd_struct %s %s", str->name, "NULL");
cb(bf);
}


static void 
__DumpArray(fsdbTreeCBDataArrayBegin* arr, void (*cb)(void *))
{
/* printf("NAME: %s SIZE: %d is_partial_dumped: %d\n", arr->name, (int)arr->size, (int)arr->is_partial_dumped); */
}



static bool_T __MyTreeCB(fsdbTreeCBType cb_type, 
			 void *client_data, void *tree_cb_data)
{
void (*cb)(void *) = (void (*)(void *))client_data;
char bf[16];

switch (cb_type) 
	{
    	case FSDB_TREE_CBT_BEGIN_TREE:
		/* fprintf(stderr, "Begin Tree:\n"); */
		break;

    	case FSDB_TREE_CBT_SCOPE:
		__DumpScope((fsdbTreeCBDataScope*)tree_cb_data, cb);
		break;

    	case FSDB_TREE_CBT_VAR:
		__DumpVar((fsdbTreeCBDataVar*)tree_cb_data, cb);
		break;

    	case FSDB_TREE_CBT_UPSCOPE:
		strcpy(bf, "Upscope:");
		cb(bf);
		break;

    	case FSDB_TREE_CBT_END_TREE:
		/* fprintf(stderr, "End Tree:\n"); */
		break;

	case FSDB_TREE_CBT_STRUCT_BEGIN:
		__DumpStruct((fsdbTreeCBDataStructBegin*)tree_cb_data, cb);
		break;

	case FSDB_TREE_CBT_STRUCT_END:
		strcpy(bf, "Upscope:");
		cb(bf);
		break;

	/* not yet supported */
    	case FSDB_TREE_CBT_ARRAY_BEGIN:
		__DumpArray((fsdbTreeCBDataArrayBegin*)tree_cb_data, cb);
		break;
    	case FSDB_TREE_CBT_ARRAY_END:
		break;

    	case FSDB_TREE_CBT_FILE_TYPE:
    	case FSDB_TREE_CBT_SIMULATOR_VERSION:
    	case FSDB_TREE_CBT_SIMULATION_DATE:
    	case FSDB_TREE_CBT_X_AXIS_SCALE:
    	case FSDB_TREE_CBT_END_ALL_TREE:
    	case FSDB_TREE_CBT_RECORD_BEGIN:
    	case FSDB_TREE_CBT_RECORD_END:
        	break;
             
    	default:
		return(FALSE);
    	}

return(TRUE);
}


#else

static void dummy_compilation_unit(void)
{

}

#endif
