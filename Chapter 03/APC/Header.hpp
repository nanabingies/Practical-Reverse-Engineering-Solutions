#pragma once

#include <intrin.h>
#include <ntifs.h>
#include <aux_klib.h>
#include <ntddk.h>
#pragma warning(disable : 4214)
#pragma warning(disable : 4201)

#define APC_POOL_TAG '_apc'

//0x430 bytes (sizeof)
struct _KTHREAD
{
	struct _DISPATCHER_HEADER Header;                                       //0x0
	VOID* SListFaultAddress;                                                //0x18
	ULONGLONG QuantumTarget;                                                //0x20
	VOID* InitialStack;                                                     //0x28
	VOID* volatile StackLimit;                                              //0x30
	VOID* StackBase;                                                        //0x38
	ULONGLONG ThreadLock;                                                   //0x40
	volatile ULONGLONG CycleTime;                                           //0x48
	ULONG CurrentRunTime;                                                   //0x50
	ULONG ExpectedRunTime;                                                  //0x54
	VOID* KernelStack;                                                      //0x58
	struct _XSAVE_FORMAT* StateSaveArea;                                    //0x60
	struct _KSCHEDULING_GROUP* volatile SchedulingGroup;                    //0x68
	//union _KWAIT_STATUS_REGISTER WaitRegister;                              //0x70
	CHAR    WaitRegister;
	volatile UCHAR Running;                                                 //0x71
	UCHAR Alerted[2];                                                       //0x72
	union
	{
		struct
		{
			ULONG AutoBoostActive : 1;                                        //0x74
			ULONG ReadyTransition : 1;                                        //0x74
			ULONG WaitNext : 1;                                               //0x74
			ULONG SystemAffinityActive : 1;                                   //0x74
			ULONG Alertable : 1;                                              //0x74
			ULONG UserStackWalkActive : 1;                                    //0x74
			ULONG ApcInterruptRequest : 1;                                    //0x74
			ULONG QuantumEndMigrate : 1;                                      //0x74
			ULONG UmsDirectedSwitchEnable : 1;                                //0x74
			ULONG TimerActive : 1;                                            //0x74
			ULONG SystemThread : 1;                                           //0x74
			ULONG ProcessDetachActive : 1;                                    //0x74
			ULONG CalloutActive : 1;                                          //0x74
			ULONG ScbReadyQueue : 1;                                          //0x74
			ULONG ApcQueueable : 1;                                           //0x74
			ULONG ReservedStackInUse : 1;                                     //0x74
			ULONG UmsPerformingSyscall : 1;                                   //0x74
			ULONG TimerSuspended : 1;                                         //0x74
			ULONG SuspendedWaitMode : 1;                                      //0x74
			ULONG SuspendSchedulerApcWait : 1;                                //0x74
			ULONG CetUserShadowStack : 1;                                     //0x74
			ULONG BypassProcessFreeze : 1;                                    //0x74
			ULONG Reserved : 10;                                              //0x74
		};
		LONG MiscFlags;                                                     //0x74
	};
	union
	{
		struct
		{
			ULONG ThreadFlagsSpare : 2;                                       //0x78
			ULONG AutoAlignment : 1;                                          //0x78
			ULONG DisableBoost : 1;                                           //0x78
			ULONG AlertedByThreadId : 1;                                      //0x78
			ULONG QuantumDonation : 1;                                        //0x78
			ULONG EnableStackSwap : 1;                                        //0x78
			ULONG GuiThread : 1;                                              //0x78
			ULONG DisableQuantum : 1;                                         //0x78
			ULONG ChargeOnlySchedulingGroup : 1;                              //0x78
			ULONG DeferPreemption : 1;                                        //0x78
			ULONG QueueDeferPreemption : 1;                                   //0x78
			ULONG ForceDeferSchedule : 1;                                     //0x78
			ULONG SharedReadyQueueAffinity : 1;                               //0x78
			ULONG FreezeCount : 1;                                            //0x78
			ULONG TerminationApcRequest : 1;                                  //0x78
			ULONG AutoBoostEntriesExhausted : 1;                              //0x78
			ULONG KernelStackResident : 1;                                    //0x78
			ULONG TerminateRequestReason : 2;                                 //0x78
			ULONG ProcessStackCountDecremented : 1;                           //0x78
			ULONG RestrictedGuiThread : 1;                                    //0x78
			ULONG VpBackingThread : 1;                                        //0x78
			ULONG ThreadFlagsSpare2 : 1;                                      //0x78
			ULONG EtwStackTraceApcInserted : 8;                               //0x78
		};
		volatile LONG ThreadFlags;                                          //0x78
	};
	volatile UCHAR Tag;                                                     //0x7c
	UCHAR SystemHeteroCpuPolicy;                                            //0x7d
	UCHAR UserHeteroCpuPolicy : 7;                                            //0x7e
	UCHAR ExplicitSystemHeteroCpuPolicy : 1;                                  //0x7e
	union
	{
		struct
		{
			UCHAR RunningNonRetpolineCode : 1;                                //0x7f
			UCHAR SpecCtrlSpare : 7;                                          //0x7f
		};
		UCHAR SpecCtrl;                                                     //0x7f
	};
	ULONG SystemCallNumber;                                                 //0x80
	ULONG ReadyTime;                                                        //0x84
	VOID* FirstArgument;                                                    //0x88
	struct _KTRAP_FRAME* TrapFrame;                                         //0x90
	union
	{
		struct _KAPC_STATE ApcState;                                        //0x98
		struct
		{
			UCHAR ApcStateFill[43];                                         //0x98
			CHAR Priority;                                                  //0xc3
			ULONG UserIdealProcessor;                                       //0xc4
		};
	};
	volatile LONGLONG WaitStatus;                                           //0xc8
	struct _KWAIT_BLOCK* WaitBlockList;                                     //0xd0
	union
	{
		struct _LIST_ENTRY WaitListEntry;                                   //0xd8
		struct _SINGLE_LIST_ENTRY SwapListEntry;                            //0xd8
	};
	struct _DISPATCHER_HEADER* volatile Queue;                              //0xe8
	VOID* Teb;                                                              //0xf0
	ULONGLONG RelativeTimerBias;                                            //0xf8
	struct _KTIMER Timer;                                                   //0x100
	union
	{
		struct _KWAIT_BLOCK WaitBlock[4];                                   //0x140
		struct
		{
			UCHAR WaitBlockFill4[20];                                       //0x140
			ULONG ContextSwitches;                                          //0x154
		};
		struct
		{
			UCHAR WaitBlockFill5[68];                                       //0x140
			volatile UCHAR State;                                           //0x184
			CHAR Spare13;                                                   //0x185
			UCHAR WaitIrql;                                                 //0x186
			CHAR WaitMode;                                                  //0x187
		};
		struct
		{
			UCHAR WaitBlockFill6[116];                                      //0x140
			ULONG WaitTime;                                                 //0x1b4
		};
		struct
		{
			UCHAR WaitBlockFill7[164];                                      //0x140
			union
			{
				struct
				{
					SHORT KernelApcDisable;                                 //0x1e4
					SHORT SpecialApcDisable;                                //0x1e6
				};
				ULONG CombinedApcDisable;                                   //0x1e4
			};
		};
		struct
		{
			UCHAR WaitBlockFill8[40];                                       //0x140
			struct _KTHREAD_COUNTERS* ThreadCounters;                       //0x168
		};
		struct
		{
			UCHAR WaitBlockFill9[88];                                       //0x140
			struct _XSTATE_SAVE* XStateSave;                                //0x198
		};
		struct
		{
			UCHAR WaitBlockFill10[136];                                     //0x140
			VOID* volatile Win32Thread;                                     //0x1c8
		};
		struct
		{
			UCHAR WaitBlockFill11[176];                                     //0x140
			struct _UMS_CONTROL_BLOCK* Ucb;                                 //0x1f0
			struct _KUMS_CONTEXT_HEADER* volatile Uch;                      //0x1f8
		};
	};
	union
	{
		volatile LONG ThreadFlags2;                                         //0x200
		struct
		{
			ULONG BamQosLevel : 8;                                            //0x200
			ULONG ThreadFlags2Reserved : 24;                                  //0x200
		};
	};
	ULONG Spare21;                                                          //0x204
	struct _LIST_ENTRY QueueListEntry;                                      //0x208
	union
	{
		volatile ULONG NextProcessor;                                       //0x218
		struct
		{
			ULONG NextProcessorNumber : 31;                                   //0x218
			ULONG SharedReadyQueue : 1;                                       //0x218
		};
	};
	LONG QueuePriority;                                                     //0x21c
	struct _KPROCESS* Process;                                              //0x220
	union
	{
		struct _GROUP_AFFINITY UserAffinity;                                //0x228
		struct
		{
			UCHAR UserAffinityFill[10];                                     //0x228
			CHAR PreviousMode;                                              //0x232
			CHAR BasePriority;                                              //0x233
			union
			{
				CHAR PriorityDecrement;                                     //0x234
				struct
				{
					UCHAR ForegroundBoost : 4;                                //0x234
					UCHAR UnusualBoost : 4;                                   //0x234
				};
			};
			UCHAR Preempted;                                                //0x235
			UCHAR AdjustReason;                                             //0x236
			CHAR AdjustIncrement;                                           //0x237
		};
	};
	ULONGLONG AffinityVersion;                                              //0x238
	union
	{
		struct _GROUP_AFFINITY Affinity;                                    //0x240
		struct
		{
			UCHAR AffinityFill[10];                                         //0x240
			UCHAR ApcStateIndex;                                            //0x24a
			UCHAR WaitBlockCount;                                           //0x24b
			ULONG IdealProcessor;                                           //0x24c
		};
	};
	ULONGLONG NpxState;                                                     //0x250
	union
	{
		struct _KAPC_STATE SavedApcState;                                   //0x258
		struct
		{
			UCHAR SavedApcStateFill[43];                                    //0x258
			UCHAR WaitReason;                                               //0x283
			CHAR SuspendCount;                                              //0x284
			CHAR Saturation;                                                //0x285
			USHORT SListFaultCount;                                         //0x286
		};
	};
	union
	{
		struct _KAPC SchedulerApc;                                          //0x288
		struct
		{
			UCHAR SchedulerApcFill0[1];                                     //0x288
			UCHAR ResourceIndex;                                            //0x289
		};
		struct
		{
			UCHAR SchedulerApcFill1[3];                                     //0x288
			UCHAR QuantumReset;                                             //0x28b
		};
		struct
		{
			UCHAR SchedulerApcFill2[4];                                     //0x288
			ULONG KernelTime;                                               //0x28c
		};
		struct
		{
			UCHAR SchedulerApcFill3[64];                                    //0x288
			struct _KPRCB* volatile WaitPrcb;                               //0x2c8
		};
		struct
		{
			UCHAR SchedulerApcFill4[72];                                    //0x288
			VOID* LegoData;                                                 //0x2d0
		};
		struct
		{
			UCHAR SchedulerApcFill5[83];                                    //0x288
			UCHAR CallbackNestingLevel;                                     //0x2db
			ULONG UserTime;                                                 //0x2dc
		};
	};
	struct _KEVENT SuspendEvent;                                            //0x2e0
	struct _LIST_ENTRY ThreadListEntry;                                     //0x2f8
	struct _LIST_ENTRY MutantListHead;                                      //0x308
	UCHAR AbEntrySummary;                                                   //0x318
	UCHAR AbWaitEntryCount;                                                 //0x319
	UCHAR AbAllocationRegionCount;                                          //0x31a
	CHAR SystemPriority;                                                    //0x31b
	ULONG SecureThreadCookie;                                               //0x31c
	struct _KLOCK_ENTRY* LockEntries;                                       //0x320
	struct _SINGLE_LIST_ENTRY PropagateBoostsEntry;                         //0x328
	struct _SINGLE_LIST_ENTRY IoSelfBoostsEntry;                            //0x330
	UCHAR PriorityFloorCounts[16];                                          //0x338
	UCHAR PriorityFloorCountsReserved[16];                                  //0x348
	ULONG PriorityFloorSummary;                                             //0x358
	volatile LONG AbCompletedIoBoostCount;                                  //0x35c
	volatile LONG AbCompletedIoQoSBoostCount;                               //0x360
	volatile SHORT KeReferenceCount;                                        //0x364
	UCHAR AbOrphanedEntrySummary;                                           //0x366
	UCHAR AbOwnedEntryCount;                                                //0x367
	ULONG ForegroundLossTime;                                               //0x368
	union
	{
		struct _LIST_ENTRY GlobalForegroundListEntry;                       //0x370
		struct
		{
			struct _SINGLE_LIST_ENTRY ForegroundDpcStackListEntry;          //0x370
			ULONGLONG InGlobalForegroundList;                               //0x378
		};
	};
	LONGLONG ReadOperationCount;                                            //0x380
	LONGLONG WriteOperationCount;                                           //0x388
	LONGLONG OtherOperationCount;                                           //0x390
	LONGLONG ReadTransferCount;                                             //0x398
	LONGLONG WriteTransferCount;                                            //0x3a0
	LONGLONG OtherTransferCount;                                            //0x3a8
	struct _KSCB* QueuedScb;                                                //0x3b0
	volatile ULONG ThreadTimerDelay;                                        //0x3b8
	union
	{
		volatile LONG ThreadFlags3;                                         //0x3bc
		struct
		{
			ULONG ThreadFlags3Reserved : 8;                                   //0x3bc
			ULONG PpmPolicy : 2;                                              //0x3bc
			ULONG ThreadFlags3Reserved2 : 22;                                 //0x3bc
		};
	};
	ULONGLONG TracingPrivate[1];                                            //0x3c0
	VOID* SchedulerAssist;                                                  //0x3c8
	VOID* volatile AbWaitObject;                                            //0x3d0
	ULONG ReservedPreviousReadyTimeValue;                                   //0x3d8
	ULONGLONG KernelWaitTime;                                               //0x3e0
	ULONGLONG UserWaitTime;                                                 //0x3e8
	union
	{
		struct _LIST_ENTRY GlobalUpdateVpThreadPriorityListEntry;           //0x3f0
		struct
		{
			struct _SINGLE_LIST_ENTRY UpdateVpThreadPriorityDpcStackListEntry; //0x3f0
			ULONGLONG InGlobalUpdateVpThreadPriorityList;                   //0x3f8
		};
	};
	LONG SchedulerAssistPriorityFloor;                                      //0x400
	ULONG Spare28;                                                          //0x404
	ULONGLONG EndPadding[5];                                                //0x408
};

//0x160 bytes (sizeof)
struct _KPROCESS
{
	struct _DISPATCHER_HEADER Header;                                       //0x0
	struct _LIST_ENTRY ProfileListHead;                                     //0x18
	ULONGLONG DirectoryTableBase;                                           //0x28
	struct _LIST_ENTRY ThreadListHead;                                      //0x30
	ULONGLONG ProcessLock;                                                  //0x40
	//struct _KAFFINITY_EX Affinity;                                          //0x48
	CHAR Unknown[0x28];
	struct _LIST_ENTRY ReadyListHead;                                       //0x70
	struct _SINGLE_LIST_ENTRY SwapListEntry;                                //0x80
	//volatile struct _KAFFINITY_EX ActiveProcessors;                         //0x88
	CHAR Unknown1[0x2C];
	union
	{
		struct
		{
			volatile LONG AutoAlignment : 1;                                  //0xb0
			volatile LONG DisableBoost : 1;                                   //0xb0
			volatile LONG DisableQuantum : 1;                                 //0xb0
			volatile ULONG ActiveGroupsMask : 4;                              //0xb0
			volatile LONG ReservedFlags : 25;                                 //0xb0
		};
		volatile LONG ProcessFlags;                                         //0xb0
	};
	CHAR BasePriority;                                                      //0xb4
	CHAR QuantumReset;                                                      //0xb5
	UCHAR Visited;                                                          //0xb6
	UCHAR Unused3;                                                          //0xb7
	ULONG ThreadSeed[4];                                                    //0xb8
	USHORT IdealNode[4];                                                    //0xc8
	USHORT IdealGlobalNode;                                                 //0xd0
	//union _KEXECUTE_OPTIONS Flags;                                          //0xd2
	CHAR Unknown2;
	UCHAR Unused1;                                                          //0xd3
	ULONG Unused2;                                                          //0xd4
	ULONG Unused4;                                                          //0xd8
	//union _KSTACK_COUNT StackCount;                                         //0xdc
	CHAR Unknown3[0x4];
	struct _LIST_ENTRY ProcessListEntry;                                    //0xe0
	volatile ULONGLONG CycleTime;                                           //0xf0
	ULONG KernelTime;                                                       //0xf8
	ULONG UserTime;                                                         //0xfc
	VOID* InstrumentationCallback;                                          //0x100
	//union _KGDTENTRY64 LdtSystemDescriptor;                                 //0x108
	CHAR Unknown4[0x10];
	VOID* LdtBaseAddress;                                                   //0x118
	//struct _KGUARDED_MUTEX LdtProcessLock;                                  //0x120
	CHAR Unknown5[0x38];
	USHORT LdtFreeSelectorHint;                                             //0x158
	USHORT LdtTableLength;                                                  //0x15a
};

#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED 0x00000001
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002 // ?
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004
#define THREAD_CREATE_FLAGS_HAS_SECURITY_DESCRIPTOR 0x00000010 // ?
#define THREAD_CREATE_FLAGS_ACCESS_CHECK_IN_TARGET 0x00000020 // ?
#define THREAD_CREATE_FLAGS_INITIAL_THREAD 0x00000080

// NtCreateThreadEx header definition based on https://undocumented.ntinternals.net/
typedef NTSTATUS(NTAPI* lpNtCreateThreadEx)(
	OUT		PHANDLE				hThread,
	IN		ACCESS_MASK			DesiredAccess,
	IN		PVOID				ObjectAttributes,
	IN		HANDLE				ProcessHandle,
	IN		PVOID				lpStartAddress,
	IN		PVOID				lpParameter,
	IN		ULONG				CreateSuspended,
	IN		SIZE_T				StackZeroBits,
	IN		SIZE_T				SizeOfStackCommit,
	IN		SIZE_T				SizeOfStackReserve,
	OUT		PVOID				lpBytesBuffer
	);

extern "C" NTSTATUS ZwQuerySystemInformation(
	ULONG SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	ULONG * ReturnLength);

//#define MAXIMUM_FILENAME_LENGTH 255 
typedef unsigned short WORD;

typedef struct SYSTEM_MODULE {
	ULONG                Reserved1;
	ULONG                Reserved2;
#ifdef _WIN64
	ULONG				Reserved3;
#endif
	PVOID                ImageBaseAddress;
	ULONG                ImageSize;
	ULONG                Flags;
	WORD                 Id;
	WORD                 Rank;
	WORD                 w018;
	WORD                 NameOffset;
	CHAR                 Name[MAXIMUM_FILENAME_LENGTH];
}SYSTEM_MODULE, * PSYSTEM_MODULE;

typedef struct SYSTEM_MODULE_INFORMATION {
	ULONG                ModulesCount;
	SYSTEM_MODULE        Modules[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULE_INFORMATION {
	PVOID Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	CHAR FullPathName[0x0100];
}RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	WORD LoadCount;
	WORD TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		struct
		{
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union
	{
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	struct _ACTIVATION_CONTEXT* EntryPointActivationContext;
	PVOID PatchInformation;
	LIST_ENTRY ForwarderLinks;
	LIST_ENTRY ServiceTagLinks;
	LIST_ENTRY StaticLinks;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef unsigned char BYTE;

typedef struct _PEB_LDR_DATA {
	ULONG                   Length;
	BOOLEAN                 Initialized;
	PVOID                   SsHandle;
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

//0x380 bytes (sizeof)
typedef struct _PEB
{
	UCHAR InheritedAddressSpace;                                            //0x0
	UCHAR ReadImageFileExecOptions;                                         //0x1
	UCHAR BeingDebugged;                                                    //0x2
	union
	{
		UCHAR BitField;                                                     //0x3
		struct
		{
			UCHAR ImageUsesLargePages : 1;                                    //0x3
			UCHAR IsProtectedProcess : 1;                                     //0x3
			UCHAR IsLegacyProcess : 1;                                        //0x3
			UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
			UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
			UCHAR SpareBits : 3;                                              //0x3
		};
	};
	VOID* Mutant;                                                           //0x8
	VOID* ImageBaseAddress;                                                 //0x10
	struct _PEB_LDR_DATA* Ldr;                                              //0x18
	struct _RTL_USER_PROCESS_PARAMETERS* ProcessParameters;                 //0x20
	VOID* SubSystemData;                                                    //0x28
	VOID* ProcessHeap;                                                      //0x30
	struct _RTL_CRITICAL_SECTION* FastPebLock;                              //0x38
	VOID* AtlThunkSListPtr;                                                 //0x40
	VOID* IFEOKey;                                                          //0x48
	union
	{
		ULONG CrossProcessFlags;                                            //0x50
		struct
		{
			ULONG ProcessInJob : 1;                                           //0x50
			ULONG ProcessInitializing : 1;                                    //0x50
			ULONG ProcessUsingVEH : 1;                                        //0x50
			ULONG ProcessUsingVCH : 1;                                        //0x50
			ULONG ProcessUsingFTH : 1;                                        //0x50
			ULONG ReservedBits0 : 27;                                         //0x50
		};
	};
	union
	{
		VOID* KernelCallbackTable;                                          //0x58
		VOID* UserSharedInfoPtr;                                            //0x58
	};
	ULONG SystemReserved[1];                                                //0x60
	ULONG AtlThunkSListPtr32;                                               //0x64
	VOID* ApiSetMap;                                                        //0x68
	ULONG TlsExpansionCounter;                                              //0x70
	VOID* TlsBitmap;                                                        //0x78
	ULONG TlsBitmapBits[2];                                                 //0x80
	VOID* ReadOnlySharedMemoryBase;                                         //0x88
	VOID* HotpatchInformation;                                              //0x90
	VOID** ReadOnlyStaticServerData;                                        //0x98
	VOID* AnsiCodePageData;                                                 //0xa0
	VOID* OemCodePageData;                                                  //0xa8
	VOID* UnicodeCaseTableData;                                             //0xb0
	ULONG NumberOfProcessors;                                               //0xb8
	ULONG NtGlobalFlag;                                                     //0xbc
	union _LARGE_INTEGER CriticalSectionTimeout;                            //0xc0
	ULONGLONG HeapSegmentReserve;                                           //0xc8
	ULONGLONG HeapSegmentCommit;                                            //0xd0
	ULONGLONG HeapDeCommitTotalFreeThreshold;                               //0xd8
	ULONGLONG HeapDeCommitFreeBlockThreshold;                               //0xe0
	ULONG NumberOfHeaps;                                                    //0xe8
	ULONG MaximumNumberOfHeaps;                                             //0xec
	VOID** ProcessHeaps;                                                    //0xf0
	VOID* GdiSharedHandleTable;                                             //0xf8
	VOID* ProcessStarterHelper;                                             //0x100
	ULONG GdiDCAttributeList;                                               //0x108
	struct _RTL_CRITICAL_SECTION* LoaderLock;                               //0x110
	ULONG OSMajorVersion;                                                   //0x118
	ULONG OSMinorVersion;                                                   //0x11c
	USHORT OSBuildNumber;                                                   //0x120
	USHORT OSCSDVersion;                                                    //0x122
	ULONG OSPlatformId;                                                     //0x124
	ULONG ImageSubsystem;                                                   //0x128
	ULONG ImageSubsystemMajorVersion;                                       //0x12c
	ULONG ImageSubsystemMinorVersion;                                       //0x130
	ULONGLONG ActiveProcessAffinityMask;                                    //0x138
	ULONG GdiHandleBuffer[60];                                              //0x140
	VOID(*PostProcessInitRoutine)();                                       //0x230
	VOID* TlsExpansionBitmap;                                               //0x238
	ULONG TlsExpansionBitmapBits[32];                                       //0x240
	ULONG SessionId;                                                        //0x2c0
	union _ULARGE_INTEGER AppCompatFlags;                                   //0x2c8
	union _ULARGE_INTEGER AppCompatFlagsUser;                               //0x2d0
	VOID* pShimData;                                                        //0x2d8
	VOID* AppCompatInfo;                                                    //0x2e0
	struct _UNICODE_STRING CSDVersion;                                      //0x2e8
	struct _ACTIVATION_CONTEXT_DATA* ActivationContextData;                 //0x2f8
	struct _ASSEMBLY_STORAGE_MAP* ProcessAssemblyStorageMap;                //0x300
	struct _ACTIVATION_CONTEXT_DATA* SystemDefaultActivationContextData;    //0x308
	struct _ASSEMBLY_STORAGE_MAP* SystemAssemblyStorageMap;                 //0x310
	ULONGLONG MinimumStackCommit;                                           //0x318
	struct _FLS_CALLBACK_INFO* FlsCallback;                                 //0x320
	struct _LIST_ENTRY FlsListHead;                                         //0x328
	VOID* FlsBitmap;                                                        //0x338
	ULONG FlsBitmapBits[4];                                                 //0x340
	ULONG FlsHighIndex;                                                     //0x350
	VOID* WerRegistrationData;                                              //0x358
	VOID* WerShipAssertPtr;                                                 //0x360
	VOID* pContextData;                                                     //0x368
	VOID* pImageHeaderHash;                                                 //0x370
	union
	{
		ULONG TracingFlags;                                                 //0x378
		struct
		{
			ULONG HeapTracingEnabled : 1;                                     //0x378
			ULONG CritSecTracingEnabled : 1;                                  //0x378
			ULONG SpareTracingBits : 30;                                      //0x378
		};
	};
}PEB, * PPEB;

typedef  enum  _THREAD_STATE
{
	StateInitialized,
	StateReady,
	StateRunning,
	StateStandby,
	StateTerminated,
	StateWait,
	StateTransition,
	StateUnknown
}THREAD_STATE;

typedef enum _KAPC_ENVIRONMENT
{
	OriginalApcEnvironment,
	AttachedApcEnvironment,
	CurrentApcEnvironment,
	InsertApcEnvironment
} KAPC_ENVIRONMENT;

typedef
VOID
(NTAPI* PKNORMAL_ROUTINE)(
	_In_ PVOID NormalContext,
	_In_ PVOID SystemArgument1,
	_In_ PVOID SystemArgument2
	);

typedef
VOID
(NTAPI* PKKERNEL_ROUTINE)(
	_In_ PKAPC Apc,
	_Inout_ PKNORMAL_ROUTINE* NormalRoutine,
	_Inout_ PVOID* NormalContext,
	_Inout_ PVOID* SystemArgument1,
	_Inout_ PVOID* SystemArgument2
	);

typedef
VOID
(NTAPI* PKRUNDOWN_ROUTINE) (
	_In_ PKAPC Apc
	);

extern "C"
NTKERNELAPI
VOID
NTAPI
KeInitializeApc(
	_Out_ PRKAPC Apc,
	_In_ PETHREAD Thread,
	_In_ KAPC_ENVIRONMENT Environment,
	_In_ PKKERNEL_ROUTINE KernelRoutine,
	_In_opt_ PKRUNDOWN_ROUTINE RundownRoutine,
	_In_opt_ PKNORMAL_ROUTINE NormalRoutine,
	_In_opt_ KPROCESSOR_MODE ApcMode,
	_In_opt_ PVOID NormalContext
);

extern "C"
NTKERNELAPI
BOOLEAN
NTAPI
KeInsertQueueApc(
	_Inout_ PRKAPC Apc,
	_In_opt_ PVOID SystemArgument1,
	_In_opt_ PVOID SystemArgument2,
	_In_ KPRIORITY Increment
);

typedef struct _SYSTEM_THREAD_INFORMATION {
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientId;
	KPRIORITY Priority;
	LONG BasePriority;
	ULONG ContextSwitchCount;
	ULONG State;
	KWAIT_REASON WaitReason;
#if defined(_M_X64) || defined(_M_ARM64) // TODO:ARM64
	LARGE_INTEGER unk;
#endif
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

typedef  struct  _SYSTEM_PROCESS_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	LARGE_INTEGER Reserved[3];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
	KPRIORITY BasePriority;
	HANDLE UniqueProcessId;
	HANDLE ParentProcessId;
	ULONG HandleCount;
	LPCWSTR Reserved2[2];
	ULONG PrivatePageCount;
	VM_COUNTERS VirtualMemoryCounters;
	IO_COUNTERS IoCounters;
	SYSTEM_THREAD_INFORMATION Threads[ANYSIZE_ARRAY];
}SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

EXTERN_C NTSYSCALLAPI PVOID RtlImageDirectoryEntryToData(_In_  PVOID   Base, _In_  BOOLEAN MappedAsImage,
	_In_  USHORT  DirectoryEntry, _Out_ PULONG  Size);