/******************************************************************************
 *
 * Name: acglobal.h - Declarations for global variables
 *       $Revision: 109 $
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999, 2000, 2001, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights.  You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code.  No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision.  In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change.  Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee.  Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution.  In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE.  ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT,  ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES.  INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS.  INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.  THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government.  In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/

#ifndef __ACGLOBAL_H__
#define __ACGLOBAL_H__


/*
 * Ensure that the globals are actually defined only once.
 *
 * The use of these defines allows a single list of globals (here) in order
 * to simplify maintenance of the code.
 */
#ifdef DEFINE_ACPI_GLOBALS
#define ACPI_EXTERN
#else
#define ACPI_EXTERN extern
#endif


/*****************************************************************************
 *
 * Debug support
 *
 ****************************************************************************/

/* Runtime configuration of debug print levels */

extern      UINT32                      AcpiDbgLevel;
extern      UINT32                      AcpiDbgLayer;

/* Procedure nesting level for debug output */

extern      UINT32                      AcpiGbl_NestingLevel;


/*****************************************************************************
 *
 * ACPI Table globals
 *
 ****************************************************************************/

/*
 * Table pointers.
 * Although these pointers are somewhat redundant with the global AcpiTable,
 * they are convenient because they are typed pointers.
 *
 * These tables are single-table only; meaning that there can be at most one
 * of each in the system.  Each global points to the actual table.
 *
 */
ACPI_EXTERN RSDP_DESCRIPTOR             *AcpiGbl_RSDP;
ACPI_EXTERN XSDT_DESCRIPTOR             *AcpiGbl_XSDT;
ACPI_EXTERN FADT_DESCRIPTOR             *AcpiGbl_FADT;
ACPI_EXTERN ACPI_TABLE_HEADER           *AcpiGbl_DSDT;
ACPI_EXTERN ACPI_COMMON_FACS            *AcpiGbl_FACS;

/*
 * Since there may be multiple SSDTs and PSDTS, a single pointer is not
 * sufficient; Therefore, there isn't one!
 */


/*
 * ACPI Table info arrays
 */
extern      ACPI_TABLE_DESC             AcpiGbl_AcpiTables[NUM_ACPI_TABLES];
extern      ACPI_TABLE_SUPPORT          AcpiGbl_AcpiTableData[NUM_ACPI_TABLES];

/*
 * Predefined mutex objects.  This array contains the
 * actual OS mutex handles, indexed by the local ACPI_MUTEX_HANDLEs.
 * (The table maps local handles to the real OS handles)
 */
ACPI_EXTERN ACPI_MUTEX_INFO             AcpiGbl_AcpiMutexInfo [NUM_MTX];


/*****************************************************************************
 *
 * Miscellaneous globals
 *
 ****************************************************************************/


ACPI_EXTERN ACPI_MEMORY_LIST            AcpiGbl_MemoryLists[ACPI_NUM_MEM_LISTS];
ACPI_EXTERN ACPI_OBJECT_NOTIFY_HANDLER  AcpiGbl_DrvNotify;
ACPI_EXTERN ACPI_OBJECT_NOTIFY_HANDLER  AcpiGbl_SysNotify;
ACPI_EXTERN UINT8                      *AcpiGbl_Gpe0EnableRegisterSave;
ACPI_EXTERN UINT8                      *AcpiGbl_Gpe1EnableRegisterSave;
ACPI_EXTERN ACPI_WALK_STATE            *AcpiGbl_BreakpointWalk;
ACPI_EXTERN ACPI_HANDLE                 AcpiGbl_GlobalLockSemaphore;

ACPI_EXTERN UINT32                      AcpiGbl_GlobalLockThreadCount;
ACPI_EXTERN UINT32                      AcpiGbl_RestoreAcpiChipset;
ACPI_EXTERN UINT32                      AcpiGbl_OriginalMode;
ACPI_EXTERN UINT32                      AcpiGbl_EdgeLevelSave;
ACPI_EXTERN UINT32                      AcpiGbl_IrqEnableSave;
ACPI_EXTERN UINT32                      AcpiGbl_RsdpOriginalLocation;
ACPI_EXTERN UINT32                      AcpiGbl_NsLookupCount;
ACPI_EXTERN UINT32                      AcpiGbl_PsFindCount;
ACPI_EXTERN UINT16                      AcpiGbl_Pm1EnableRegisterSave;
ACPI_EXTERN UINT16                      AcpiGbl_NextTableOwnerId;
ACPI_EXTERN UINT16                      AcpiGbl_NextMethodOwnerId;
ACPI_EXTERN UINT8                       AcpiGbl_DebuggerConfiguration;
ACPI_EXTERN BOOLEAN                     AcpiGbl_GlobalLockAcquired;
ACPI_EXTERN BOOLEAN                     AcpiGbl_StepToNextCall;
ACPI_EXTERN BOOLEAN                     AcpiGbl_AcpiHardwarePresent;
ACPI_EXTERN BOOLEAN                     AcpiGbl_GlobalLockPresent;

extern BOOLEAN                          AcpiGbl_Shutdown;
extern UINT32                           AcpiGbl_StartupFlags;
extern const UINT8                      AcpiGbl_DecodeTo8bit[8];
extern const NATIVE_CHAR                *AcpiGbl_DbSleepStates[ACPI_NUM_SLEEP_STATES];


/*****************************************************************************
 *
 * Namespace globals
 *
 ****************************************************************************/

#define NUM_NS_TYPES                    INTERNAL_TYPE_INVALID+1
#define NUM_PREDEFINED_NAMES            9


ACPI_EXTERN ACPI_NAMESPACE_NODE         AcpiGbl_RootNodeStruct;
ACPI_EXTERN ACPI_NAMESPACE_NODE        *AcpiGbl_RootNode;

extern const UINT8                      AcpiGbl_NsProperties[NUM_NS_TYPES];
extern const PREDEFINED_NAMES           AcpiGbl_PreDefinedNames [NUM_PREDEFINED_NAMES];

#ifdef ACPI_DEBUG
ACPI_EXTERN UINT32                      AcpiGbl_CurrentNodeCount;
ACPI_EXTERN UINT32                      AcpiGbl_CurrentNodeSize;
ACPI_EXTERN UINT32                      AcpiGbl_MaxConcurrentNodeCount;
ACPI_EXTERN UINT32                      AcpiGbl_EntryStackPointer;
ACPI_EXTERN UINT32                      AcpiGbl_LowestStackPointer;
ACPI_EXTERN UINT32                      AcpiGbl_DeepestNesting;
#endif

/*****************************************************************************
 *
 * Interpreter globals
 *
 ****************************************************************************/


ACPI_EXTERN ACPI_THREAD_STATE           *AcpiGbl_CurrentWalkList;

/* Control method single step flag */

ACPI_EXTERN UINT8                       AcpiGbl_CmSingleStep;


/*****************************************************************************
 *
 * Parser globals
 *
 ****************************************************************************/

ACPI_EXTERN ACPI_PARSE_OBJECT           *AcpiGbl_ParsedNamespaceRoot;


/*****************************************************************************
 *
 * Event globals
 *
 ****************************************************************************/

ACPI_EXTERN ACPI_FIXED_EVENT_INFO       AcpiGbl_FixedEventHandlers[ACPI_NUM_FIXED_EVENTS];
ACPI_EXTERN ACPI_HANDLE                 AcpiGbl_GpeObjHandle;
ACPI_EXTERN UINT32                      AcpiGbl_GpeRegisterCount;
ACPI_EXTERN ACPI_GPE_REGISTERS         *AcpiGbl_GpeRegisters;
ACPI_EXTERN ACPI_GPE_LEVEL_INFO        *AcpiGbl_GpeInfo;

/*
 * Gpe validation and translation table
 * Indexed by the GPE number, returns GPE_INVALID if the GPE is not supported.
 * Otherwise, returns a valid index into the global GPE table.
 *
 * This table is needed because the GPE numbers supported by block 1 do not
 * have to be contiguous with the GPE numbers supported by block 0.
 */
ACPI_EXTERN UINT8                       AcpiGbl_GpeValid [ACPI_NUM_GPE];

/* AcpiEvent counter for debug only */

#ifdef ACPI_DEBUG
ACPI_EXTERN UINT32                      AcpiGbl_EventCount[ACPI_NUM_FIXED_EVENTS];
#endif


/*****************************************************************************
 *
 * Debugger globals
 *
 ****************************************************************************/

#ifdef ENABLE_DEBUGGER
ACPI_EXTERN BOOLEAN                     AcpiGbl_MethodExecuting;
ACPI_EXTERN BOOLEAN                     AcpiGbl_DbTerminateThreads;
#endif


#endif /* __ACGLOBAL_H__ */
