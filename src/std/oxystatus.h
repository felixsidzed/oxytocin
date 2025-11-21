#pragma once

typedef uint16_t oxystatus;

/*
#define STATUS_SEVERITY_SUCCESS 0
#define STATUS_SEVERITY_INFO	1
#define STATUS_SEVERITY_ERROR	2
#define STATUS_SEVERITY_FATAL	3

#define STATUS_CLASS_NONE		0
#define STATUS_CLASS_ARITHM		1
#define STATUS_CLASS_INVALID	2
#define STATUS_CLASS_DEBUG		3
#define STATUS_CLASS_SEGMENT	4
#define STATUS_CLASS_MEMORY		5

#define STATUS_CODE_SUCCESS		0
#define STATUS_CODE_DIVBYZERO	1
#define STATUS_CODE_OVERFLOW	2
#define STATUS_CODE_ILLINSN		3
#define STATUS_CODE_INVALIDTSS	4
#define STATUS_CODE_NOCOPROC	5
#define STATUS_CODE_SINGLESTEP	6
#define STATUS_CODE_BREAKPOINT	7
#define STATUS_CODE_COSEGOR		8
#define STATUS_CODE_NOSEG		9
#define STATUS_CODE_PAGEFAULT	10
#define STATUS_CODE_PROTFAULT	11
*/

/// Severity:	00 (SUCCESS)
/// Class:		00 (NONE)
/// Code:		00 (SUCCESS)
#define STATUS_SUCCESS			(oxystatus)0x0000

/// Severity:	02 (ERROR)
/// Class:		01 (ARITHM)
/// Code:		01 (DIVBYZERO)
#define STATUS_DIVIDE_BY_ZERO	(oxystatus)0x0106

/// Severity:	02 (ERROR)
/// Class:		01 (ARITHM)
/// Code:		02 (OVERFLOW)
#define STATUS_OVERFLOW			(oxystatus)0x0206

/// Severity:	03 (FATAL)
/// Class:		02 (INVALID)
/// Code:		03 (ILLINSN)
#define STATUS_INVALID_OPCODE	(oxystatus)0x030B

/// Severity:	03 (FATAL)
/// Class:		02 (INVALID)
/// Code:		04 (INVALIDTSS)
#define STATUS_INVALID_TSS		(oxystatus)0x040B

/// Severity:	03 (FATAL)
/// Class:		02 (INVALID)
/// Code:		05 (NOCOPROC)
#define STATUS_NO_COPROCESSOR	(oxystatus)0x050B

/// Severity:	01 (INFO)
/// Class:		03 (DEBUG)
/// Code:		06 (SINGLESTEP)
#define STATUS_DBG_SINGLESTEP	(oxystatus)0x060D

/// Severity:	01 (INFO)
/// Class:		03 (DEBUG)
/// Code:		07 (BREAKPOINT)
#define STATUS_DBG_BREAKPOINT	(oxystatus)0x070D

/// Severity:	03 (FATAL)
/// Class:		04 (SEGMENT)
/// Code:		08 (COSEGOR)
#define STATUS_COPROC_OVERRUN	(oxystatus)0x0813

/// Severity:	03 (FATAL)
/// Class:		04 (SEGMENT)
/// Code:		09 (NOSEG)
#define STATUS_MISSING_SEGMENT	(oxystatus)0x0913

/// Severity:	03 (FATAL)
/// Class:		04 (SEGMENT)
/// Code:		09 (NOSEG)
#define STATUS_MISSING_SEGMENT	(oxystatus)0x0913

/// Severity:	03 (FATAL)
/// Class:		04 (MEMORY)
/// Code:		10 (PAGEFAULT)
#define STATUS_PAGE_FAULT		(oxystatus)0x0A17

/// Severity:	02 (ERROR)
/// Class:		04 (MEMORY)
/// Code:		11 (PROTFAULT)
#define STATUS_ACCESS_VIOLATION	(oxystatus)0x0B16
