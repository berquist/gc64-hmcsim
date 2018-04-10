/*
 * _HMC_CMC_C_
 *
 * Hybrid memory cube simulation library
 *
 * Custom memory cube functionality
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "hmc_sim.h"


/* Function Prototypes */
extern int hmcsim_trace_rqst( struct hmcsim_t *hmc,
                              char *rqst,
                              uint32_t dev,
                              uint32_t quad,
                              uint32_t vault,
                              uint32_t bank,
                              uint64_t addr,
                              uint32_t size );


/* conversion table for cmc request enums, opcodes and struct indices */
struct cmc_table{
  hmc_rqst_t type;
  uint32_t cmd;
  uint32_t idx;
};

struct cmc_table ctable[HMC_MAX_CMC] = {

  {CMC04,4,0},
  {CMC05,5,1},
  {CMC06,6,2},
  {CMC07,7,3},
  {CMC20,20,4},
  {CMC21,21,5},
  {CMC22,22,6},
  {CMC23,23,7},
  {CMC32,32,8},
  {CMC36,36,9},
  {CMC37,37,10},
  {CMC38,38,11},
  {CMC39,39,12},
  {CMC41,41,13},
  {CMC42,42,14},
  {CMC43,43,15},
  {CMC44,44,16},
  {CMC45,45,17},
  {CMC46,46,18},
  {CMC47,47,19},
  {CMC56,56,20},
  {CMC57,57,21},
  {CMC58,58,22},
  {CMC59,59,23},
  {CMC60,60,24},
  {CMC61,61,25},
  {CMC62,62,26},
  {CMC63,63,27},
  {CMC69,69,28},
  {CMC70,70,29},
  {CMC71,71,30},
  {CMC72,72,31},
  {CMC73,73,32},
  {CMC74,74,33},
  {CMC75,75,34},
  {CMC76,76,35},
  {CMC77,77,36},
  {CMC78,78,37},
  {CMC85,85,38},
  {CMC86,86,39},
  {CMC87,87,40},
  {CMC88,88,41},
  {CMC89,89,42},
  {CMC90,90,43},
  {CMC91,91,44},
  {CMC92,92,45},
  {CMC93,93,46},
  {CMC94,94,47},
  {CMC102,102,48},
  {CMC103,103,49},
  {CMC107,107,50},
  {CMC108,108,51},
  {CMC109,109,52},
  {CMC110,110,53},
  {CMC111,111,54},
  {CMC112,112,55},
  {CMC113,113,56},
  {CMC114,114,57},
  {CMC115,115,58},
  {CMC116,116,59},
  {CMC117,117,60},
  {CMC118,118,61},
  {CMC120,120,62},
  {CMC121,121,63},
  {CMC122,122,64},
  {CMC123,123,65},
  {CMC124,124,66},
  {CMC125,125,67},
  {CMC126,126,68},
  {CMC127,127,69}

};


/* ----------------------------------------------------- HMCSIM_CMC_RAWTOIDX */
extern uint32_t hmcsim_cmc_rawtoidx( uint32_t raw ){
  uint32_t i = 0;

  for( i=0; i<HMC_MAX_CMC; i++ ){
    if( ctable[i].cmd == raw ){
      return i;
    }
  }
  return HMC_MAX_CMC; /* redundant, but squashes gcc warning */
}

/* ----------------------------------------------------- HMCSIM_CMC_IDXTOCMD */
extern hmc_rqst_t hmcsim_cmc_idxtocmd( uint32_t idx ){
  return ctable[idx].type;
}

/* ----------------------------------------------------- HMCSIM_CMC_CMDTOIDX */
extern uint32_t hmcsim_cmc_cmdtoidx( hmc_rqst_t rqst ){
  uint32_t i = 0;

  for( i=0; i<HMC_MAX_CMC; i++ ){
    if( ctable[i].type == rqst ){
      return i;
    }
  }
  return HMC_MAX_CMC; /* redundant, but squashes gcc warning */
}

/* ----------------------------------------------------- HMCSIM_CMC_TRACE_HEADER */
extern void hmcsim_cmc_trace_header( struct hmcsim_t *hmc ){

  /* vars */
  uint32_t i      = 0;
  uint32_t active = 0;
  char str[256];
  void (*cmc_str)(char *)  = NULL;
  /* ---- */

  for( i=0; i<HMC_MAX_CMC; i++ ){
    active += hmc->cmcs[i].active;
  }

  if( active == 0 ){
    /* nothing active, dump out */
    return ;
  }

  /* print everything active */
  fprintf( hmc->tfile, "%s\n",    "#---------------------------------------------------------" );
  fprintf( hmc->tfile, "%s\n",    "# CMC_OP:CMC_STR:RQST_LEN:RSP_LEN:RSP_CMD_CODE" );
  for( i=0; i<HMC_MAX_CMC; i++ ){
    if( hmc->cmcs[i].active == 1 ){
      cmc_str = hmc->cmcs[i].cmc_str;
      (*cmc_str)(&(str[0]));
      fprintf( hmc->tfile, "%s%d%s%s%s%d%s%d%s%d\n",
               "#",
               hmc->cmcs[i].cmd,
               ":",
               str,
               ":",
               hmc->cmcs[i].rqst_len,
               ":",
               hmc->cmcs[i].rsp_len,
               ":",
               hmc->cmcs[i].rsp_cmd_code );
    }
  }
  fprintf( hmc->tfile, "%s\n",    "#---------------------------------------------------------" );
}

/* ----------------------------------------------------- HMCSIM_REGISTER_FUNCTIONS */
/*
 * HMCSIM_REGISTER_FUNCTIONS
 *
 */
static int    hmcsim_register_functions( struct hmcsim_t *hmc, char *cmc_lib ){

  /* vars */
  hmc_rqst_t rqst;
  uint32_t cmd;
  uint32_t idx;
  uint32_t rqst_len;
  uint32_t rsp_len;
  hmc_response_t rsp_cmd;
  uint8_t rsp_cmd_code;

  void *handle = NULL;
  int (*cmc_register)(hmc_rqst_t *,
                      uint32_t *,
                      uint32_t *,
                      uint32_t *,
                      hmc_response_t *,
                      uint8_t *) = NULL;
  int (*cmc_execute)(void *,
                     uint32_t,
                     uint32_t,
                     uint32_t,
                     uint32_t,
                     uint64_t,
                     uint32_t,
                     uint64_t,
                     uint64_t,
                     uint64_t *,
                     uint64_t *) = NULL;
  void (*cmc_str)(char *) = NULL;
  void (*cmc_power)(uint32_t *,float *) = NULL;
  uint32_t (*cmc_dyn_rsp)() = NULL;
  uint32_t (*cmc_dyn_rqst)() = NULL;
  uint32_t (*cmc_dyn)() = NULL;
  uint32_t (*cmc_mem_ops)(uint64_t) = NULL;
  uint32_t dynamic = 0;
  /* ---- */

  /* attempt to load the library */
#ifdef HMC_DEBUG
  HMCSIM_PRINT_TRACE( "LOADING CMC LIBRARY" );
#endif
  handle = dlopen( cmc_lib, RTLD_NOW );

  if( handle == NULL ){
#ifdef HMC_DEBUG
    HMCSIM_PRINT_TRACE(dlerror());
#endif
    return -1;
  }

  /* library is loaded, resolve the functions */
  /* -- hmcsim_register_cmc */
  cmc_register = (int (*)(hmc_rqst_t *,
                          uint32_t *,
                          uint32_t *,
                          uint32_t *,
                          hmc_response_t *,
                          uint8_t *))dlsym(handle,"hmcsim_register_cmc");
  if( cmc_register == NULL ){
    dlclose( handle );
    return -1;
  }

  if( (*cmc_register)(&rqst,
                      &cmd,
                      &rqst_len,
                      &rsp_len,
                      &rsp_cmd,
                      &rsp_cmd_code) != 0 ){
    dlclose( handle );
    return -1;
  }

  /* -- hmcsim_execute_cmc */
  cmc_execute = (int (*)(void *,
                     uint32_t,
                     uint32_t,
                     uint32_t,
                     uint32_t,
                     uint64_t,
                     uint32_t,
                     uint64_t,
                     uint64_t,
                     uint64_t *,
                     uint64_t *))dlsym(handle,"hmcsim_execute_cmc");
  if( cmc_execute == NULL ){
    dlclose( handle );
    return -1;
  }

  /* -- hmcsim_cmc_str */
  cmc_str = (void (*)(char *))dlsym(handle,"hmcsim_cmc_str");
  if( cmc_str == NULL ){
    dlclose( handle );
    return -1;
  }

  /* -- hmcsim_cmc_power */
  cmc_power = (void (*)(uint32_t *,float *))dlsym(handle,"hmcsim_cmc_power");

  /* hmcsim_cmc_dynamic */
  cmc_dyn = (uint32_t (*)())dlsym(handle,"hmcsim_cmc_dynamic");
  if( cmc_dyn != NULL ){
    dynamic = (*cmc_dyn)();
  }
  if( dynamic ){
    /* load the remainder of the dynamic rqst/rsp functions */
    cmc_dyn_rsp = (uint32_t (*)())dlsym(handle,"hmcsim_cmc_dynamic_rsp_len");
    cmc_dyn_rqst = (uint32_t (*)())dlsym(handle,"hmcsim_cmc_dynamic_rqst_len");
  }

  /* hmcsim_cmc_mem_ops */
  cmc_mem_ops = (uint32_t (*)(uint64_t))dlsym(handle,"hmcsim_cmc_mem_ops");
#ifdef HMC_DEBUG
  if( cmc_mem_ops != NULL ){
    printf( "HMCSIM_REGISTER_FUNCTIONS: Found hmcsim_cmc_mem_ops symbol\n");
  }
#endif

  /* done loading functions */

  idx = hmcsim_cmc_rawtoidx( cmd );
#ifdef HMC_DEBUG
  printf( "HMCSIM_REGISTER_FUNCTIONS: Setting CMC command (%d) at IDX=%d to ACTIVE\n",
          cmd,idx );
#endif

  if( hmc->cmcs[idx].active == 1 ){
    /* previously activated, this is an error */
    dlclose( handle );
    return -1;
  }

  /* write the necessary references into the structure */
  if( cmc_power != NULL ){
    hmc->cmcs[idx].cmc_power    = cmc_power;
    hmc->cmcs[idx].track_power  = 1;
  }else{
    hmc->cmcs[idx].cmc_power    = NULL;
    hmc->cmcs[idx].track_power  = 0;
  }
  hmc->cmcs[idx].type         = rqst;
  hmc->cmcs[idx].cmd          = cmd;
  hmc->cmcs[idx].rqst_len     = rqst_len;
  hmc->cmcs[idx].rsp_len      = rsp_len;
  hmc->cmcs[idx].rsp_cmd      = rsp_cmd;

  hmc->cmcs[idx].active       = 1;
  hmc->cmcs[idx].handle       = handle;
  hmc->cmcs[idx].cmc_register = cmc_register;
  hmc->cmcs[idx].cmc_execute  = cmc_execute;
  hmc->cmcs[idx].cmc_str      = cmc_str;

  hmc->cmcs[idx].dynamic      = dynamic;
  hmc->cmcs[idx].cmc_dyn_rsp  = cmc_dyn_rsp;
  hmc->cmcs[idx].cmc_dyn_rqst = cmc_dyn_rqst;
  hmc->cmcs[idx].cmc_mem_ops  = cmc_mem_ops;

  return 0;
}

/* ----------------------------------------------------- HMCSIM_QUERY_CMC */
extern int  hmcsim_query_cmc( struct hmcsim_t *hmc,
                              hmc_rqst_t type,
                              uint32_t *flits,
                              uint8_t *cmd ){
  /* vars */
  uint32_t idx      = HMC_MAX_CMC;
  uint32_t (*cmc_dyn_rqst)() = NULL;
  /* ---- */

  idx = hmcsim_cmc_cmdtoidx( type );

#ifdef HMC_DEBUG
  printf( "HMCSIM_QUERY_CMC: RQST_TYPE = %d; IDX = %d\n",
       type, idx );
#endif

  if( idx == HMC_MAX_CMC ){
    return -1;
  }else if( hmc->cmcs[idx].active == 0 ){
#ifdef HMC_DEBUG
    printf( "ERROR : HMCSIM_QUERY_CMC: CMC OP AT IDX=%d IS INACTIVE\n",
            idx );
#endif
    return -1;
  }

  *flits  = hmc->cmcs[idx].rqst_len;
  *cmd    = hmc->cmcs[idx].cmd;

  /* check for a dynamic command */
  if( hmc->cmcs[idx].dynamic == 1){
    // this cmc command has a dynamic request or response length
    cmc_dyn_rqst = hmc->cmcs[idx].cmc_dyn_rqst;
    *flits = (*cmc_dyn_rqst)();
  }

  return 0;
}

/* ----------------------------------------------------- HMCSIM_PROCESS_CMC */
extern int  hmcsim_process_cmc( struct hmcsim_t *hmc,
                                uint32_t rawcmd,
                                uint32_t dev,
                                uint32_t quad,
                                uint32_t vault,
                                uint32_t bank,
                                uint64_t addr,
                                uint32_t length,
                                uint64_t head,
                                uint64_t tail,
                                uint64_t *rqst_payload,
                                uint64_t *rsp_payload,
                                uint32_t *rsp_len,
                                hmc_response_t *rsp_cmd,
                                uint8_t *raw_rsp_cmd,
                                uint32_t *row_ops,
                                float *tpower,
                                uint32_t *cmc_ops ){

  /* vars */
  uint32_t idx  = 0;
  int rtn       = 0;
  char op_name[256];
  int (*cmc_execute)(void *,
                     uint32_t,
                     uint32_t,
                     uint32_t,
                     uint32_t,
                     uint64_t,
                     uint32_t,
                     uint64_t,
                     uint64_t,
                     uint64_t *,
                     uint64_t *) = NULL;
  void (*cmc_str)(char *);
  void (*cmc_power)(uint32_t *,float *) = NULL;
  uint32_t (*cmc_dyn_rsp)() = NULL;
  uint32_t (*cmc_mem_ops)(uint64_t) = NULL;
  /* ---- */

  /* resolve the index of the cmc in the lookup table */
  idx = hmcsim_cmc_rawtoidx( rawcmd );

  if( idx == HMC_MAX_CMC ){
    /* erroneous request */
    return -1;
  }else if( hmc->cmcs[idx].active == 0 ){
    /* command not active */
    return -1;
  }

  /* -- new power measurement items */
  if( (row_ops == NULL) || (tpower == NULL) ){
    return -1;
  }

  /* command is active, process it */
#ifdef HMC_DEBUG
  HMCSIM_PRINT_TRACE( "PROCESSING CMC PACKET" );
  printf( "CMC RAWCMD:IDX = %d:%d\n", rawcmd,idx );
#endif
  cmc_execute = hmc->cmcs[idx].cmc_execute;
  rtn = (*cmc_execute)( (void *)(hmc),
                        dev,
                        quad,
                        vault,
                        bank,
                        addr,
                        length,
                        head,
                        tail,
                        rqst_payload,
                        rsp_payload);

  if( rtn == -1 ){
    return HMC_ERROR;
  }
#ifdef HMC_DEBUG
  HMCSIM_PRINT_TRACE( "DONE PROCESSING CMC PACKET" );
  HMCSIM_PRINT_TRACE( "REGISTERING RESPONSES IF NECESSARY" );
#endif

  /* register all the response data */
  if( hmc->cmcs[idx].dynamic == 1 ){
    cmc_dyn_rsp = hmc->cmcs[idx].cmc_dyn_rsp;
    *rsp_len      = (*cmc_dyn_rsp)();
  }else{
    *rsp_len      = hmc->cmcs[idx].rsp_len;
  }
  *rsp_cmd      = hmc->cmcs[idx].rsp_cmd;

  if( *rsp_len > 0 ){
    if( *rsp_cmd == RSP_CMC ){
      *raw_rsp_cmd  = hmc->cmcs[idx].rsp_cmd_code;
    }else{
      /* encode the normal reponse */
      switch( *rsp_cmd ){
      case RD_RS:
        *raw_rsp_cmd = 0x38;
        break;
      case WR_RS:
        *raw_rsp_cmd = 0x39;
        break;
      case MD_RD_RS:
        *raw_rsp_cmd = 0x3A;
        break;
      case MD_WR_RS:
        *raw_rsp_cmd = 0x3B;
        break;
      case RSP_ERROR:
      default:
        *raw_rsp_cmd = 0x00;
        break;
      }
    }
  }else{
    *raw_rsp_cmd = 0x00;
  }

  /* trace it */
#ifdef HMC_DEBUG
  HMCSIM_PRINT_TRACE( "DUMPING TRACE DATA FOR CMC COMMAND" );
#endif
  /* -- get the name of the op */
  cmc_str = hmc->cmcs[idx].cmc_str;
  (*cmc_str)(&(op_name[0]));

  /* -- insert the trace */
  hmcsim_trace_rqst( hmc,
                     &(op_name[0]),
                     dev,
                     quad,
                     vault,
                     bank,
                     addr,
                     length );

#ifdef HMC_DEBUG
  HMCSIM_PRINT_TRACE( "DUMPING POWER/THERMAL DATA FOR CMC COMMAND" );
#endif
  /* -- get the power */
  if( hmc->cmcs[idx].track_power == 1 ){
    cmc_power = hmc->cmcs[idx].cmc_power;
    (*cmc_power)(row_ops,tpower);
  }else{
    *row_ops = 1;
    *tpower  = 0.;
  }

  if( hmc->cmcs[idx].cmc_mem_ops != NULL ){
    cmc_mem_ops = hmc->cmcs[idx].cmc_mem_ops;
    *cmc_ops = (*cmc_mem_ops)(addr);
  }else{
    *cmc_ops = 1;
  }

#ifdef HMC_DEBUG
  HMCSIM_PRINT_TRACE( "CMC PROCESSING COMPLETE" );
#endif
  return 0;
}

/* ----------------------------------------------------- HMCSIM_FREE_CMC */
/*
 * HMCSIM_FREE_CMC
 *
 */
extern int    hmcsim_free_cmc( struct hmcsim_t *hmc ){
  uint32_t i = 0;

  if( hmc == NULL ){
    return -1;
  }

  if( hmc->cmcs == NULL ){
    return -1;
  }

  for( i=0; i<HMC_MAX_CMC; i++ ){
    if( hmc->cmcs[i].active == 1 ){
      dlclose( hmc->cmcs[i].handle );
    }
  }

  return 0;
};

/* ----------------------------------------------------- HMCSIM_LOAD_CMC */
/*
 * HMCSIM_LOAD_CMC
 *
 */
extern int      hmcsim_load_cmc( struct hmcsim_t *hmc, char *cmc_lib ){

  if((hmc == NULL) || (cmc_lib == NULL)){
#ifdef HMC_DEBUG
  HMCSIM_PRINT_TRACE( "ERROR PROCESSING FUNCTION ARGUMENTS" );
#endif
    return -1;
  }

#ifdef HMC_DEBUG
  printf( "ATTEMPTING TO LOAD CMC LIBRARY: %s\n", cmc_lib );
#endif

  /* register the library functions */
  if( hmcsim_register_functions( hmc, cmc_lib ) != 0 ){
#ifdef HMC_DEBUG
  HMCSIM_PRINT_TRACE( "FAILED TO REGISTER CMC FUNCTIONS" );
#endif
    return -1;
  }

  return 0;
}

/* EOF */
