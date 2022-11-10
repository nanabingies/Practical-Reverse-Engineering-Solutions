__int64 __fastcall PsSetLoadImageNotifyRoutine(__int64 NotifyRoutine)
{
  return PsSetLoadImageNotifyRoutineEx(NotifyRoutine, 0i64);
}

 __int64 __fastcall PsSetLoadImageNotifyRoutineEx(__int64 NotifyRoutine, __int64 Flags)
{
  __int64 CallbackObject; // rdi
  __int64 index; // rbx
  unsigned int v5; // ebx
  unsigned int v7; // [rsp+30h] [rbp-48h] BYREF
  __int64 v8; // [rsp+38h] [rbp-40h] BYREF
  __int64 *v9; // [rsp+40h] [rbp-38h] BYREF
  int v10; // [rsp+48h] [rbp-30h]
  int v11; // [rsp+4Ch] [rbp-2Ch]
  int *v12; // [rsp+50h] [rbp-28h]
  int v13; // [rsp+58h] [rbp-20h]
  int v14; // [rsp+5Ch] [rbp-1Ch]

  if ( (Flags & 0xFFFFFFFFFFFFFFFEui64) != 0 )
    return 0xC00000F0i64;

    
  /*
  **
  ** ExAllocate Callback allocates buffer of size 0x18 in POOL_NX_ALLOCATION pool with tag 'brbC'
  ** returns pointer to struct _EX_CALLBACK_ROUTINE_BLOCK {
  **    // 0x00 EX_RUNDOWN_REF 	RundownProtect
  **    // 0x08 PEX_CALLBACK_FUNCTION 	Function
  **    // 0x10 PVOID 	Context
  ** }
  ** It sets CallbackBlock->Function to the param NotifyRoutine 
  ** Sets CallbackBlock->Context to Flags
  */
  CallbackObject = ExAllocateCallBack(NotifyRoutine, Flags);
  if ( CallbackObject )
  {
    index = 0i64;
    while ( !ExCompareExchangeCallBack(&PspLoadImageNotifyRoutine[index], CallbackObject, 0i64) )
    {
      index = (unsigned int)(index + 1);
      if ( (unsigned int)index >= 0x40 )
      {
        ExFreePoolWithTag(CallbackObject, 0i64);
        goto LABEL_15;
      }
    }
    _InterlockedIncrement(&PspLoadImageNotifyRoutineCount);
    if ( (PspNotifyEnableMask & 1) == 0 )
      _interlockedbittestandset(&PspNotifyEnableMask, 0);
    v5 = 0;
  }
  else
  {
LABEL_15:
    v5 = 0xC000009A;
  }
  v7 = v5;
  v8 = NotifyRoutine;
  if ( EtwApiCallsProvRegHandle )
  {
    v11 = 0;
    v14 = 0;
    v9 = &v8;
    v10 = 8;
    v12 = (int *)&v7;
    v13 = 4;
    EtwWrite(EtwApiCallsProvRegHandle, (int)&KERNEL_AUDIT_API_PSSETLOADIMAGENOTIFYROUTINE, 0i64, 2, (__int64)&v9);
  }
  return v5;
}