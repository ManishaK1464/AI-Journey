// File: ITLA_Registers.h
#ifndef ITLA_REGISTERS_H
#define ITLA_REGISTERS_H

// ITLA 1.3 Register Addresses (OIF MSA)
#define ITLA_REG_NOP       0x00  // NOP/Status
#define ITLA_REG_DEV_TYPE  0x01
#define ITLA_REG_MANUF     0x02
#define ITLA_REG_MODEL     0x03
#define ITLA_REG_SN        0x04  // Serial Number (string via AEA)
#define ITLA_REG_MFG_DATE  0x05
#define ITLA_REG_RELEASE   0x06
#define ITLA_REG_REL_BACK  0x07
#define ITLA_REG_GEN_CFG   0x08
#define ITLA_REG_EAC       0x09  // AEA Extended Addr Config
#define ITLA_REG_EA        0x0A  // AEA Extended Addr
#define ITLA_REG_EAR       0x0B  // AEA Extended Addr Data
#define ITLA_REG_IOCAP     0x0D
#define ITLA_REG_LSTRESP   0x13
#define ITLA_REG_DL_CONFIG 0x14
#define ITLA_REG_DL_STATUS 0x15

// Status registers
#define ITLA_REG_STATUSF   0x20  // Fatal status (follows ITLA spec bitfields)
#define ITLA_REG_STATUSW   0x21  // Warning status
// Thresholds
#define ITLA_REG_FPOWTH    0x22
#define ITLA_REG_WPOWTH    0x23
#define ITLA_REG_FFREQTH   0x24
#define ITLA_REG_WFREQTH   0x25
#define ITLA_REG_FTHERMTH  0x26
#define ITLA_REG_WTHERMTH  0x27
#define ITLA_REG_SRQT      0x28
#define ITLA_REG_FATALT    0x29
#define ITLA_REG_ALMT      0x2A

// Optical settings
#define ITLA_REG_CHANNEL   0x30  // Channel LSW (16 bits)
#define ITLA_REG_POWER     0x31  // Power setpoint (dBm*100, signed short)
#define ITLA_REG_RESETA    0x32  // Reset/Enable register (bit 3 = SENA)
#define ITLA_REG_MCB       0x33  // Module config behavior
#define ITLA_REG_GRID      0x34  // Grid spacing integer (fits 0.1 GHz steps)
#define ITLA_REG_FCF1      0x35  // First Channel Freq digits
#define ITLA_REG_FCF2      0x36
#define ITLA_REG_LF1       0x40  // Laser Frequency readback (GHz*1000)
#define ITLA_REG_LF2       0x41
#define ITLA_REG_OOP       0x42  // Optical output power (dBm*100, signed)
#define ITLA_REG_TEMP      0x43  // Current module temp (°C*100, signed)
#define ITLA_REG_FTFR      0x4F  // Fine Tune Frequency Range (GHz*1000)
#define ITLA_REG_OPSL      0x50  // Optical power range min
#define ITLA_REG_OPSH      0x51  // Optical power range max
#define ITLA_REG_LFL1      0x52  // Laser first frequency (low)
#define ITLA_REG_LFL2      0x53
#define ITLA_REG_LFH1      0x54  // Laser last frequency (low)
#define ITLA_REG_LFH2      0x55
#define ITLA_REG_LGRID     0x56  // Min grid spacing low
#define ITLA_REG_CURR      0x57  // Module currents
#define ITLA_REG_TEMPS     0x58  // Module temps
#define ITLA_REG_DITHERE   0x59  // Dither E
#define ITLA_REG_DITHERR   0x5A  // Dither A
#define ITLA_REG_DITHERF   0x5B  // Dither F
#define ITLA_REG_DITHERA   0x5C  // Dither T
#define ITLA_REG_TBTFL     0x5D  // Turn-on/turn-off thresholds
#define ITLA_REG_TBTFH     0x5E
#define ITLA_REG_FAGETH    0x5F  // Age thresholds
#define ITLA_REG_WAGETH    0x60
#define ITLA_REG_AGE       0x61  // Module age
#define ITLA_REG_FTF      0x62  // Fine Tune
#define ITLA_REG_GRID2     0x66  // Grid spacing fractional (0.001 GHz steps)
#define ITLA_REG_FCF3      0x67  // First Channel Freq fractional
#define ITLA_REG_LF3       0x68  // Laser Frequency fractional
#define ITLA_REG_LFL3      0x69  // Laser first freq fractional
#define ITLA_REG_LFH3      0x6A  // Laser last freq fractional
#define ITLA_REG_LGRID2    0x6B  // Min grid fractional

// Error Codes (NOP bits 3:0)
#define ITLA_ERR_OK    0x00  // No error
#define ITLA_ERR_RNI   0x01  // Register not implemented
#define ITLA_ERR_RNW   0x02  // Register not writeable
#define ITLA_ERR_RVE   0x03  // Register value error
#define ITLA_ERR_CIP   0x04  // Cmd ignored, pending operation
#define ITLA_ERR_CII   0x05  // Cmd ignored, init in progress
#define ITLA_ERR_ERE   0x06  // Extended address range error
#define ITLA_ERR_ERO   0x07  // Extended address read-only
#define ITLA_ERR_EXF   0x08  // Execution failed (general)
#define ITLA_ERR_CIE   0x09  // Cmd ignored, output enabled (inhibit)
#define ITLA_ERR_IVC   0x0A  // Invalid configuration
// 0x0B-0x0E reserved, 0x0F = Vendor-specific
#define ITLA_ERR_VSE   0x0F  // Vendor-specific error

#endif // ITLA_REGISTERS_H


#define ITLA_REG_EAC        0x0E
#define ITLA_REG_EA_EXT     0x0F
#define ITLA_REG_EAR_EXT    0x10
#define ITLA_REG_FFREQTH2   0x63
#define ITLA_REG_WFREQTH2   0x64
#define ITLA_REG_CHANNELH   0x65 

