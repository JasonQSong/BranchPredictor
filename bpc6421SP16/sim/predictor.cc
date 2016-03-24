#include "predictor.h"


#define PHT_CTR_MAX  3
#define PHT_CTR_INIT 2

#define HIST_LEN   4
#define HIST_MASK  0xf

/////////////// STORAGE BUDGET JUSTIFICATION ////////////////
// total bht size=9843(*4bits)
// total bht counters=9843*2^4(*2bits)
// total size = 63954 Bytes
////////////////////////////////////////////////////////////
//// DEPRECATED BEGIN
//// Total storage budget: 32KB + 17 bits
//// Total PHT counters: 2^17 
//// Total PHT size = 2^17 * 2 bits/counter = 2^18 bits = 32KB
//// GHR size: 17 bits
//// Total Size = PHT size + GHR size
//// DEPRECATED END
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

PREDICTOR::PREDICTOR(void){

  // largest prime number smaller than 14222(64000*8/36)
  numBhtEntries=12281;

  bht=new UINT32[numBhtEntries];
  bhtCounters=new UINT32[numBhtEntries<<HIST_LEN];
  for(UINT32 ii=0;ii<numBhtEntries;ii++){
    bht[ii]=0;
  }
  for(UINT32 ii=0;ii<(numBhtEntries<<HIST_LEN);ii++){
    bhtCounters[ii]=PHT_CTR_INIT;
  }
/*
  historyLength    = HIST_LEN;
  ghr              = 0;
  numPhtEntries    = (1<< HIST_LEN);

  pht = new UINT32[numPhtEntries];

  for(UINT32 ii=0; ii< numPhtEntries; ii++){
    pht[ii]=PHT_CTR_INIT; 
  }
*/
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

bool   PREDICTOR::GetPrediction(UINT32 PC){

  UINT32 bhtIndex = PC % numBhtEntries;
  UINT32 bhtCounterIndex = (bhtIndex<<HIST_LEN)|(bht[bhtIndex]&HIST_MASK);
  UINT32 bhtCounter=bhtCounters[bhtCounterIndex];

  if(bhtCounter > PHT_CTR_MAX/2){
    return TAKEN; 
  }else{
    return NOT_TAKEN; 
  }

/*
  UINT32 phtIndex   = (PC^ghr) % (numPhtEntries);
  UINT32 phtCounter = pht[phtIndex];
  
  if(phtCounter > PHT_CTR_MAX/2){
    return TAKEN; 
  }else{
    return NOT_TAKEN; 
  }
*/
}


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void  PREDICTOR::UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget){

  UINT32 bhtIndex = PC % numBhtEntries;
  UINT32 bhtCounterIndex = (bhtIndex<<HIST_LEN)|(bht[bhtIndex]&HIST_MASK);
  UINT32 bhtCounter=bhtCounters[bhtCounterIndex];

  if(resolveDir == TAKEN){
    bhtCounters[bhtCounterIndex] = SatIncrement(bhtCounter, PHT_CTR_MAX);
  }else{
    bhtCounters[bhtCounterIndex] = SatDecrement(bhtCounter);
  }

  bht[bhtIndex]=(bht[bhtIndex]<<1);
  if(resolveDir == TAKEN){
    bht[bhtIndex]++; 
  }

/*
  UINT32 phtIndex   = (PC^ghr) % (numPhtEntries);
  UINT32 phtCounter = pht[phtIndex];

  // update the PHT

  if(resolveDir == TAKEN){
    pht[phtIndex] = SatIncrement(phtCounter, PHT_CTR_MAX);
  }else{
    pht[phtIndex] = SatDecrement(phtCounter);
  }

  // update the GHR
  ghr = (ghr << 1);

  if(resolveDir == TAKEN){
    ghr++; 
  }
*/
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void    PREDICTOR::TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget){

  // This function is called for instructions which are not
  // conditional branches, just in case someone decides to design
  // a predictor that uses information from such instructions.
  // We expect most contestants to leave this function untouched.

  return;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
