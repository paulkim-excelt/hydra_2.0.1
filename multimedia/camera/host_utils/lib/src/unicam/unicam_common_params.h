/*****************************************************************************
 Copyright 2016-2017 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/
#ifndef UNICAM_COMMON_PARAMS_H
#define UNICAM_COMMON_PARAMS_H

/******************************************************************************
File Name:   unicam_common_params.h
Descritpion: Unicam common params for user input and offline driver.
******************************************************************************/

/* EPeripheralMode */
typedef enum
{
    eCpi       = 0,
    eCsi2      = 1,
    eTg        = 2,
    eHispi     = 3,
    eMaxMode   = 4
} EPeripheralMode;

/* AnaCtrl */
typedef struct
{
    uint32_t ptatadj; /* Band-gap Bias Control(manufacturing setting */
    uint32_t ctatadj; /* & not normally altered) */
} AnaCtrl;

/* EBurstLength */
typedef enum
{
    eBurstLen16 = 0,
    eBurstLen8 = 1,
    eBurstLen4 = 2,
    eBurstLen2 = 3
} EBurstLength;

/* EBurstSpacing */
typedef enum
{
    eBurstSpace2 = 0,
    eBurstSpace4 = 1,
    eBurstSpace8 = 2,
    eBurstSpace16 = 3,
    eBurstSpace32 = 4,
    eBurstSpace64 = 5,
    eBurstSpace128 = 6,
    eBurstSpace256 = 7,
    eBurstSpace512 = 8,
    eBurstSpace1024 = 9
} EBurstSpacing;

/* EPanicThreshold */
typedef enum
{
    eOutputFifo25 = 0,
    eOutputFifo50 = 1,
    eOutputFifo75 = 2,
    eOutputFifo90 = 3
} EPanicThreshold;

/* EDataLaneSync */
typedef enum
{
    eSingleBitError = 0,
    eLeaderSeq = 1
} EDataLaneSync;

/* EDataLaneEnable */
typedef enum
{
    eAuto = 0,
    eManual = 1
} EDataLaneEnable;

/* EUnPack */
typedef enum
{
    eUnPackNone = 0,
    eUnPackRaw6 = 1,
    eUnPackRaw7 = 2,
    eUnPackRaw8 = 3,
    eUnPackRaw10 = 4,
    eUnPackRaw12 = 5,
    eUnPackRaw14 = 6,
    eUnPackRaw16 = 7,
    eUnPackRaw20 = 8
} EUnPack;

/* EDpcmDecode */
typedef enum
{
    eDpcmNone = 0,
    eDpcm_8_10 = 1,
    eDpcm_7_10 = 2,
    eDpcm_6_10 = 3,
    eDpcm_8_12 = 4,
    eDpcm_7_12 = 5,
    eDpcm_6_12 = 6,
    eDpcm_10_14 = 7,
    eDpcm_8_14 = 8,
    eDpcm_12_16 = 9,
    eDpcm_10_16 = 10,
    eDpcm_8_16 = 11
} EDpcmDecode;

/* Input raw format */
typedef enum
{
    eRaw6 = 1,
    eRaw7 = 2,
    eRaw8 = 3,
    eRaw10 = 4,
    eRaw12 = 5,
    eRaw14 = 6,
    eRaw16 = 7
} EInputFormat;

/* HDR Params */
/* EExposure */
typedef enum
{
    eLongExp = 0,
    eShortExp = 1
} EExposure;

/* CSI2 Params */
typedef enum
{
    eLePack = 0,
    eNoLePack = 1
} ELineEndSync;

/* ECsiLine */
typedef enum
{
    eCsiNoLePack = 0,
    eCsiLePack = 1
} ECsiLine;

/* CPI Params */
typedef enum
{
    eData8 = 0,
    eData10 = 1,
    eData12 = 2,
    eData16 = 3
} EDataWidth;

/* EResyncEdge */
typedef enum
{
    ePosEdge = 0,
    eNegEdge = 1
} EResyncEdge;

/* EActiveLevel */
typedef enum
{
    eActiveHigh = 0,
    eActiveLow = 1
} EActiveLevel;

/* EShiftSync */
typedef enum
{
    eNoShift = 0,
    eLeftShift2 = 1,
    eRigthShift2 = 2,
    eRightShift6 = 3
} EShiftSync;

/* EFieldMode */
typedef enum
{
    eFieldAuto = 0,
    eFieldDerived = 1,
    eFieldEven = 2,
    eFieldOdd = 3
} EFieldMode;

/* EHvSyncMode */
typedef enum
{
    eActiveLine = 0,
    eStartactLine = 1,
    eDefVsal = 2,
    eIgnored = 3
} EHvSyncMode;

/* ESyncMode */
typedef enum
{
    eHvSync = 0,
    eEmbeddedSync = 1
} ESyncMode;

/* EFieldGate */
typedef enum
{
    eOddEvenField = 0,
    eEvenField = 1,
    eOddField = 2
} EFieldGate;

/* EChannelCode */
typedef enum
{
    eVideoData = 0,
    eAncillaryData = 1 /* Not supported */
} EChannelCode;

/* HiSpi Params */
typedef enum
{
    eStreamingSp = 0,
    ePacketizedSp = 1
} EHispiMode;

/* EHispiFlr */
typedef enum
{
    eNoFlr = 0,
    eFlrCells = 1
} EHispiFlr;

/* EHispiCrc */
typedef enum
{
    eNoCrc = 0,
    eCrcCells = 1
} EHispiCrc;

/* EHispiMsbFirst */
typedef enum
{
    eLsbFirst = 0,
    eMsbFirst = 1
} EHispiMsbFirst;

/* TG Params */
typedef enum
{
    eTc = 0,
    eAxi = 1
} ETgMode;

/* EBayerOrder */
typedef enum
{
    eRggb = 0,
    eGbrg = 1,
    eBggr = 2,
    eGrbg = 3
} EBayerOrder;

/* ETgAxiCfg */
typedef enum
{
    eTrig = 0,
    eCont = 1
} ETgAxiCfg;

/* ENumLanes */
typedef enum
{
    eSingleLane = 1,
    eDualLane   = 2,
    eQuadLane   = 4
} ENumLanes;

/* Ctrl */
typedef struct
{
    EPeripheralMode     inputMode;          /* Camera Peripheral Mode */
    uint32_t            oeTimeout;          /* Output Engine Timeout */
    uint32_t            supressLineStart;   /* Supress Line Start */
    /* Reg: CAMICTL */
    uint32_t            imageBufferOverrun; /* Image Buffer Overrun */
    uint32_t            dataBufferOverrun;  /* Data buffer overrun */
} Ctrl;

/* Priority */
typedef struct
{
    EBurstLength     burstLen;      /* Burst Length */
    EBurstSpacing    burstSpace;    /* Burst Spacing */
    uint32_t         panicPriority; /* Panic Priority */
    uint32_t         normalPriority;/* Normal Priority */
    EPanicThreshold  panicThresh;   /* Panic Threshold */
    uint32_t         panicEnable;   /* Enable Panic logic */
} Priority;

/* ClkLaneTime */
typedef struct
{
    uint32_t     clkSettle; /* Clock Lane TCLK_SETTLE */
    uint32_t     clkTermEn; /* Clock Lane TCLK_TERM_EN */
} ClkLaneTime;

/* DataLaneTime */
typedef struct
{
    uint32_t     trxEnable; /* Data Lane TRX_ENABLE */
    uint32_t     thsSettle; /* Lane THS_SETTLE */
    uint32_t     tdTermEn; /* Data Lane TD_TERM_EN */
} DataLaneTime;


/* Reg: CAMHDRCTL */
typedef struct
{
    uint32_t      passObLines;  /* Optical black lines */
    uint32_t      sonyHdr;      /* Sony HDR mode */
    EExposure     initExp;      /* Exposure information */
    uint32_t      lineOffset;   /* Line offset for HDR mode */
    uint32_t      hdrEnable;    /* Enable HDR mode */
} HdrCtrl;

/* Csi2Params */
typedef struct
{
    uint32_t        longImageId;        /* Image Data Identifier */
    uint32_t        shortImageId;       /* Image Data Identifier */
    /* HDR Params */
    uint32_t        shortExpVc;
    uint32_t        longExpVc;
    uint32_t        csi2VcEn;           /* Use exposure channel using VC */
    /* reg: CAMLINCTL */
    ELineEndSync    lineEndSync;        /* Generate line end sync */
    ECsiLine        lineEnd;            /* Allow line end packets from sensor */
    ECsiLine        lineStart;          /* Allow line start packets from sensor */
    /* Reg: CAMCTL */
    uint32_t        packetFarmeTimeout; /* Packet Framer Time out */
    /* Reg: CAMCLT */
    ClkLaneTime     clkTime;
    /* Reg: CAMDLT */
    DataLaneTime    dataTime;
    EDataLaneSync   laneSync;           /* Data Lane Sync Matching */
    EDataLaneEnable laneTermination;    /* Data Lane Termination Resistor Enable */
    EDataLaneEnable laneHs;             /* Data Lane High Speed Enable */
} Csi2Params;

/* CpiResync */
typedef struct
{
    uint32_t     dataShift;   /* Data bit shift */
    EDataWidth   dwid;        /* Data width */
    EResyncEdge  dataReSync;  /* Data reSync mode */
    EResyncEdge  hReSync;     /* Horizontal Sync */
    EResyncEdge  vReSync;     /* Vertical Sync */
    EActiveLevel hSyncActive; /* Horizontal Sync Active Level */
    EActiveLevel vSyncActive; /* Vertical Sync Active Level */
} CpiResync;

/* CpiFraming */
typedef struct
{
    uint32_t      fildBit;       /* field_bit with in the data */
    uint32_t      hSyncBit;      /* hSyncBit with in the data */
    uint32_t      vSyncBit;      /* Vsync_bit with in the data */
    EShiftSync    shift;         /* Shift Sync */
    uint32_t      enbSyncShift;  /* Shift Enable */
    uint32_t      scMode;        /* Scope Mode: use case TBD */
    EFieldMode    fieldMode;     /* Field Mode */
    EHvSyncMode   hSyncMode;     /* Horizontal Sync Mode */
    EHvSyncMode   vSyncMode;     /* Vertical Sync Mode */
    ESyncMode     syncMode;      /* Sync Mode */
} CpiFraming;

/* CpiWindowing */
typedef struct
{
    uint32_t        gStat;         /* enable statistics */
    EFieldGate      fGate;         /* Field Gating */
    uint32_t        windowingEnb;  /* Enable Windowing */
    /* Reg : CAMCPIWHC */
    uint32_t        hActive;       /* Horizontal Activate Period */
    uint32_t        hFront;        /* Horizontal Front Period */
    /* Reg: CAMCPIWVC */
    uint32_t        vActive;       /* Vertical Active Period */
    uint32_t        vFront;        /* Vertical Front Period */
} CpiWindowing;

/* CpiParams */
typedef struct
{
    CpiResync       reSync;    /* Reg: CAMCPIR */
    CpiFraming      framing;   /* Reg: CAMCPIF */
    CpiWindowing    windowing; /* Reg: CAMCPIW */
    EChannelCode    code;      /* Reg: CAMCPIB */
} CpiParams;

/* Reg: CAMHISPI_CTL */
typedef struct {
    uint32_t         crcIntEn;
    EHispiMsbFirst  msbFirst;       /* LSB or MSB first */
    EHispiCrc       crcEnable;      /* CRC or no CRC cells */
    EHispiFlr       fillerEnable;   /* filler cells or no filler cells */
    EHispiMode      mode;           /* HiSPi mode register */
    EDataLaneSync   laneSync;       /* Data Lane Sync Matching */
    EDataLaneEnable laneTermination;/* Data Lane Termination Resistor Enable */
    EDataLaneEnable laneHs;         /* Data Lane High Speed Enable */
} HispiParams;

/* TgCtrl */
typedef struct
{
    /* Reg: CAM_VC5_TGCTL */
    uint32_t        preBlankPeriod;   /* Pre-Blanking Period */
    ETgMode         mode;
    uint32_t        cfgByPass;        /* Bypass Config Block */
    uint32_t        bpp;              /* Bits per pixel */
    uint32_t        chan;             /* Channel Specify the output channel */
    uint32_t        embeddedData;     /* Embedded data */
    EBayerOrder     bayerOrder;       /* Embedded data */
    uint32_t        distortEn;        /* Distort Enable */
    uint32_t        translateEn;      /* Translate Enable */
    /* Reg: CAM_VC5_TGISZ */
    uint32_t        width;
    uint32_t        height;
    /* AXI related config for Tg */
    /* Reg:CAM_VC5_ACFG */
    ETgAxiCfg       cont;             /* continuous mode */
    /* Reg:CAM_VC5_ASA & CAM_VC5_AEA */
    uint32_t        startAddr;        /* Start Address */
    uint32_t        endAddr;          /* End Address */
    /* Reg:CAM_VC5_ASL */
    uint32_t        stride;           /* Line stride */
    uint32_t        length;           /* Line Length */
    /* Reg:CAM_VC5_ALC */
    uint32_t        lc;               /* AXI Line Count */
    /* Reg:CAM_VC5_AT */
    uint32_t        axiTimer;
} TgCtrl;

/* TgParams */
typedef struct
{
    TgCtrl              ctrl;
    /* Fps information */
    uint32_t            clkFreq;
    uint32_t            fps;
    /* Reg: CAM_VC5_TGBL */
    uint32_t            vBlank;        /* Vertical Blanking */
    uint32_t            hBlank;        /* Horizontal Blanking */
    /* Reg:CAM_VC5_TGBSZ */
    uint32_t            blockYSize;    /* Block Y Size */
    uint32_t            blockXSize;    /* Block X Size */
    /* Reg: CAM_VC5_TGPS */
    uint32_t            blankPrescale; /* Blanking period prescaling */
    /* Reg:CAM_VC5_AMM */
    uint32_t            max;           /* Maximim burst transaction length */
    uint32_t            min;           /* Minimum burst transaction length */
} TgParams;

#endif /* UNICAM_COMMON_PARAMS_H */
