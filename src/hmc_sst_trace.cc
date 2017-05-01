/*
 * HMC_SST_TRACE.CC
 *
 * Implementation file for the SST trace handlers
 *
 */

#ifdef SST_ENABLE_STAT

#include "hmc_sst_trace.h"
#include "hmc_sim.h"

// force the use of the SST namespace
using namespace SST;

/* ----------------------------------------------------- HMCSIM_SST_REGISTER_THERMAL */
static void hmcsim_sst_register_thermal(){
  LinkPhyTherm = registerStatistic<uint64_t>("LinkPhyTherm");
  LinkLocalRouteTherm = registerStatistic<uint64_t>("LinkLocalRouteTherm");
  LinkRemoteRouteTherm = registerStatistic<uint64_t>("LinkRemoteRouteTherm");
  XbarRqstSlotTherm = registerStatistic<uint64_t>("XbarRqstSlotTherm");
  XbarRspSlotTherm = registerStatistic<uint64_t>("XbarRspSlotTherm");
  XbarRouteExternTherm = registerStatistic<uint64_t>("XbarRouteExternTherm");
  VaultRqstSlotTherm = registerStatistic<uint64_t>("VaultRqstSlotTherm");
  VaultRspSlotTherm = registerStatistic<uint64_t>("VaultRspSlotTherm");
  VaultCtrlTherm = registerStatistic<uint64_t>("VaultCtrlTherm");
  RowAccessTherm = registerStatistic<uint64_t>("RowAccessTherm");
}

/* ----------------------------------------------------- HMCSIM_SST_REGISTER_POWER */
static void hmcsim_sst_register_power(){
  LinkPhyPower = registerStatistic<uint64_t>("LinkPhyPower");
  LinkLocalRoutePower = registerStatistic<uint64_t>("LinkLocalRoutePower");
  LinkRemoteRoutePower = registerStatistic<uint64_t>("LinkRemoteRoutePower");
  XbarRqstSlotPower = registerStatistic<uint64_t>("XbarRqstSlotPower");
  XbarRspSlotPower = registerStatistic<uint64_t>("XbarRspSlotPower");
  XbarRouteExternPower = registerStatistic<uint64_t>("XbarRouteExternPower");
  VaultRqstSlotPower = registerStatistic<uint64_t>("VaultRqstSlotPower");
  VaultRspSlotPower = registerStatistic<uint64_t>("VaultRspSlotPower");
  VaultCtrlPower = registerStatistic<uint64_t>("VaultCtrlPower");
  RowAccessPower = registerStatistic<uint64_t>("RowAccessPower");
}

/* ----------------------------------------------------- HMCSIM_SST_REGISTER_MISC */
static void hmcsim_sst_register_misc(){
  BankConflict = registerStatistic<uint64_t>("BankConflict");
  XbarLatency = registerStatistic<uint64_t>("XbarLatency");
}

/* ----------------------------------------------------- HMCSIM_SST_REGISTER_STALLS */
static void hmcsim_sst_register_stalls(){
  XbarRqstStall = registerStatistic<uint64_t>("XbarRqstStall");
  VaultRqstStall = registerStatistic<uint64_t>("VaultRqstStall");
  XbarRspStall = registerStatistic<uint64_t>("XbarRspStall");
  RouteRqstStall = registerStatistic<uint64_t>("RouteRqstStall");
  RouteRspStall = registerStatistic<uint64_t>("RouteRspStall");
  UndefStall = registerStatistic<uint64_t>("UndefStall");
}

/* ----------------------------------------------------- HMCSIM_SST_REGISTER_PACKETS */
static void hmcsim_sst_register_packets(){
  Write16Ops = registerStatistic<uint64_t>("WR16");
  Write32Ops = registerStatistic<uint64_t>("WR32");
  Write48Ops = registerStatistic<uint64_t>("WR48");
  Write64Ops = registerStatistic<uint64_t>("WR64");
  Write80Ops = registerStatistic<uint64_t>("WR80");
  Write96Ops = registerStatistic<uint64_t>("WR96");
  Write112Ops = registerStatistic<uint64_t>("WR112");
  Write128Ops = registerStatistic<uint64_t>("WR128");
  Write256Ops = registerStatistic<uint64_t>("WR256");
  Read16Ops = registerStatistic<uint64_t>("RD16");
  Read32Ops = registerStatistic<uint64_t>("RD32");
  Read48Ops = registerStatistic<uint64_t>("RD48");
  Read64Ops = registerStatistic<uint64_t>("RD64");
  Read80Ops = registerStatistic<uint64_t>("RD80");
  Read96Ops = registerStatistic<uint64_t>("RD96");
  Read112Ops = registerStatistic<uint64_t>("RD112");
  Read128Ops = registerStatistic<uint64_t>("RD128");
  Read256Ops = registerStatistic<uint64_t>("RD256");
  ModeWriteOps = registerStatistic<uint64_t>("MD_WR");
  ModeReadOps = registerStatistic<uint64_t>("MD_RD");
  BWROps = registerStatistic<uint64_t>("BWR");
  TwoAdd8Ops = registerStatistic<uint64_t>("2ADD8");
  Add16Ops = registerStatistic<uint64_t>("ADD16");
  PWrite16Ops = registerStatistic<uint64_t>("P_WR16");
  PWrite32Ops = registerStatistic<uint64_t>("P_WR32");
  PWrite48Ops = registerStatistic<uint64_t>("P_WR48");
  PWrite64Ops = registerStatistic<uint64_t>("P_WR64");
  PWrite80Ops = registerStatistic<uint64_t>("P_WR80");
  PWrite96Ops = registerStatistic<uint64_t>("P_WR96");
  PWrite112Ops = registerStatistic<uint64_t>("P_WR112");
  PWrite128Ops = registerStatistic<uint64_t>("P_128");
  PWrite256Ops = registerStatistic<uint64_t>("P_256");
  TwoAddS8ROps = registerStatistic<uint64_t>("2ADDS8R");
  AddS16ROps = registerStatistic<uint64_t>("ADDS16");
  Inc8Ops = registerStatistic<uint64_t>("INC8");
  PInc8Ops = registerStatistic<uint64_t>("P_INC8");
  Xor16Ops = registerStatistic<uint64_t>("XOR16");
  Or16Ops = registerStatistic<uint64_t>("OR16");
  Nor16Ops = registerStatistic<uint64_t>("NOR16");
  And16Ops = registerStatistic<uint64_t>("AND16");
  Nand16Ops = registerStatistic<uint64_t>("NAND16");
  CasGT8Ops = registerStatistic<uint64_t>("CASGT8");
  CasGT16Ops = registerStatistic<uint64_t>("CASGT16");
  CasLT8Ops = registerStatistic<uint64_t>("CASLT8");
  CasLT16Ops = registerStatistic<uint64_t>("CASLT16");
  CasEQ8Ops = registerStatistic<uint64_t>("CASEQ8");
  CasZero16Ops = registerStatistic<uint64_t>("CASZERO16");
  Eq8Ops = registerStatistic<uint64_t>("EQ8");
  Eq16Ops = registerStatistic<uint64_t>("EQ16");
  BWR8ROps = registerStatistic<uint64_t>("BWR8R");
  Swap16Ops = registerStatistic<uint64_t>("SWAP16");
}

/* ----------------------------------------------------- HMCSIM_SST_TRACE_INIT */
extern "C" int hmcsim_sst_trace_init();
int hmcsim_sst_trace_init(){
  /* tell the SST sim not to end without this data */
  registerAsPrimaryComponent();
  primaryComponentDoNotEndSim();

  /* register all the new components */
  hmcsim_sst_register_packets();
  hmcsim_sst_register_stalls();
  hmcsim_sst_register_power();
  hmcsim_sst_register_thermal();
  hmcsim_sst_register_misc();

  return 0;
}

/* ----------------------------------------------------- HMCSIM_SST_TRACE_CLOSE */
extern "C" int hmcsim_sst_trace_close();
int hmcsim_sst_trace_close(){
  primaryComponentOKToEndSim();
  return 0;
}

/* ----------------------------------------------------- HMCSIM_SST_TRACE_STALL */
extern "C" int hmcsim_sst_trace_stall( struct hmcsim_t *hmc,
                                       uint32_t dev,
                                       uint32_t quad,
                                       uint32_t vault,
                                       uint32_t src,
                                       uint32_t dest,
                                       uint32_t link,
                                       uint32_t slot,
                                       uint32_t type );
int hmcsim_sst_trace_stall( struct hmcsim_t *hmc,
                            uint32_t dev,
                            uint32_t quad,
                            uint32_t vault,
                            uint32_t src,
                            uint32_t dest,
                            uint32_t link,
                            uint32_t slot,
                            uint32_t type ){
  return 0;
}

/* ----------------------------------------------------- HMCSIM_SST_TRACE_RQST */
extern "C" int hmcsim_sst_trace_rqst( struct hmcsim_t *hmc,
                           hmc_rqst_t rqst,
                           uint32_t dev,
                           uint32_t quad,
                           uint32_t vault,
                           uint32_t bank,
                           uint64_t addr1,
                           uint32_t size );
int hmcsim_sst_trace_rqst( struct hmcsim_t *hmc,
                           hmc_rqst_t rqst,
                           uint32_t dev,
                           uint32_t quad,
                           uint32_t vault,
                           uint32_t bank,
                           uint64_t addr1,
                           uint32_t size ){
  switch( rqst ){
  case WR16:
    Write16Ops->addData(1);
    break;
  case WR32:
    Write32Ops->addData(1);
    break;
  case WR48:
    Write48Ops->addData(1);
    break;
  case WR64:
    Write64Ops->addData(1);
    break;
  case WR80:
    Write80Ops->addData(1);
    break;
  case WR96:
    Write96Ops->addData(1);
    break;
  case WR112:
    Write112Ops->addData(1);
    break;
  case WR128:
    Write128Ops->addData(1);
    break;
  case WR256:
    Write256Ops->addData(1);
    break;
  case RD16:
    Read16Ops->addData(1);
    break;
  case RD32:
    Read32Ops->addData(1);
    break;
  case RD48:
    Read48Ops->addData(1);
    break;
  case RD64:
    Read64Ops->addData(1);
    break;
  case RD80:
    Read80Ops->addData(1);
    break;
  case RD96:
    Read96Ops->addData(1);
    break;
  case RD112:
    Read112Ops->addData(1);
    break;
  case RD128:
    Read128Ops->addData(1);
    break;
  case RD256:
    Read256Ops->addData(1);
    break;
  case MD_WR:
    ModeWriteOps->addData(1);
    break;
  case BWR:
    BWROps->addData(1);
    break;
  case TWOADD8:
    TwoAdd8Ops->addData(1);
    break;
  case ADD16:
    Add16Ops->addData(1);
    break;
  case P_WR16:
    PWrite16Ops->addData(1);
    break;
  case P_WR32:
    PWrite32Ops->addData(1);
    break;
  case P_WR48:
    PWrite48Ops->addData(1);
    break;
  case P_WR64:
    PWrite64Ops->addData(1);
    break;
  case P_WR80:
    PWrite80Ops->addData(1);
    break;
  case P_WR96:
    PWrite96Ops->addData(1);
    break;
  case P_WR112:
    PWrite112Ops->addData(1);
    break;
  case P_WR128:
    PWrite128Ops->addData(1);
    break;
  case P_WR256:
    PWrite256Ops->addData(1);
    break;
  case MD_RD:
    ModeReadOps->addData(1);
    break;
  case TWOADDS8R:
    TwoAddS8ROps->addData(1);
    break;
  case ADDS16R:
    AddS16ROps->addData(1);
    break;
  case INC8:
    Inc8Ops->addData(1);
    break;
  case P_INC8:
    PInc8Ops->addData(1);
    break;
  case XOR16:
    Xor16Ops->addData(1);
    break;
  case OR16:
    Or16Ops->addData(1);
    break;
  case NOR16:
    Nor16Ops->addData(1);
    break;
  case AND16:
    And16Ops->addData(1);
    break;
  case NAND16:
    Nand16Ops->addData(1);
    break;
  case CASGT8:
    CasGT8Ops->addData(1);
    break;
  case CASGT16:
    CasGT16Ops->addData(1);
    break;
  case CASLT8:
    CasLT8Ops->addData(1);
    break;
  case CASLT16:
    CasLT16Ops->addData(1);
    break;
  case CASEQ8:
    CasEQ8Ops->addData(1);
    break;
  case CASZERO16:
    CasZero16Ops->addData(1);
    break;
  case EQ8:
    Eq8Ops->addData(1);
    break;
  case EQ16:
    Eq16Ops->addData(1);
    break;
  case BWR8R:
    BWR8ROps->addData(1);
    break;
  case SWAP16:
    Swap16Ops->addData(1);
    break;
  case FLOW_NULL:
  case PRET:
  case TRET:
  case IRTRY:
  default:
    return -1;
    break;
  }

  return 0;
}

/* ----------------------------------------------------- HMCSIM_SST_TRACE_BANK_CONFLICT */
extern "C" int hmcsim_sst_trace_bank_conflict( struct hmcsim_t *hmc,
                                    uint32_t dev,
                                    uint32_t quad,
                                    uint32_t vault,
                                    uint32_t bank,
                                    uint64_t addr );
int hmcsim_sst_trace_bank_conflict( struct hmcsim_t *hmc,
                                    uint32_t dev,
                                    uint32_t quad,
                                    uint32_t vault,
                                    uint32_t bank,
                                    uint64_t addr ){
  return 0;
}

/* ----------------------------------------------------- HMCSIM_SST_TRACE_LATENCY */
extern "C" int hmcsim_sst_trace_latency( struct hmcsim_t *hmc,
                              uint32_t dev,
                              uint32_t link,
                              uint32_t slot,
                              uint32_t quad,
                              uint32_t vault );
int hmcsim_sst_trace_latency( struct hmcsim_t *hmc,
                              uint32_t dev,
                              uint32_t link,
                              uint32_t slot,
                              uint32_t quad,
                              uint32_t vault ){
  return 0;
}

#endif /* SST_ENABLE_STAT */

/* EOF */
