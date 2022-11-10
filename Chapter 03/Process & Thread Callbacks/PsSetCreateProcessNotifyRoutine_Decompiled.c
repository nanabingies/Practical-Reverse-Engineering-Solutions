__int64 __fastcall PsSetCreateProcessNotifyRoutine(__int64 NotifyRoutine, char Remove)
{
  return PspSetCreateProcessNotifyRoutine(NotifyRoutine, Remove != 0);
}

__int64 __fastcall PspSetCreateProcessNotifyRoutine(__int64 NotifyRoutine, unsigned int Flags)
{
  __int64 v2; // rbx
  bool bIsExRoutine; // si
  __int64 LdrDataTableEntryFlags; // rdx
  __int64 CallbackObject; // rdi
  __int64 index; // rbx
  unsigned __int64 CurrentThread; // rbp
  __int64 v9; // r15
  __int64 v10; // rax
  __int64 v11; // rdi
  volatile signed __int32 *v12; // rax

  v2 = Flags;
  *(_DWORD *)&bIsExRoutine = Flags & 2;
  if ( (Flags & 1) != 0 )
  {
    CurrentThread = __readgsqword(0x188u);
    --*(_WORD *)(CurrentThread + 0x1E4);        // # KernelApcDisable
    v9 = 0i64;
    while ( 1 )
    {
      v10 = sub_1400036E0(&PspCreateProcessNotifyRoutine[v9]);
      v11 = v10;
      if ( v10 )
      {
        LODWORD(v2) = v2 & 0xFFFFFFFE;
        if ( *(_QWORD *)(v10 + 8) == NotifyRoutine
          && *(_DWORD *)(v10 + 0x10) == (_DWORD)v2
          && ExCompareExchangeCallBack(&PspCreateProcessNotifyRoutine[v9], 0i64, v10) )
        {
          v12 = &dword_1409AE93C;
          if ( *(_DWORD *)&bIsExRoutine )
            v12 = &dword_1409AE938;
          _InterlockedDecrement(v12);
          sub_140003690(&PspCreateProcessNotifyRoutine[v9], v11);
          sub_14003A9F0(CurrentThread);
          ExWaitForRundownProtectionRelease(v11);
          ExFreePoolWithTag(v11, 0i64);
          return STATUS_SUCCESS;
        }
        sub_140003690(&PspCreateProcessNotifyRoutine[v9], v11);
      }
      v9 = (unsigned int)(v9 + 1);
      if ( (unsigned int)v9 >= 0x40 )
      {
        sub_14003A9F0(CurrentThread);
        return STATUS_PROCEDURE_NOT_FOUND;
      }
    }
  }
  if ( (Flags & 2) != 0 )
    LdrDataTableEntryFlags = 0x20i64;
  else
    LdrDataTableEntryFlags = 0i64;
  if ( !(unsigned int)((__int64 (__fastcall *)(__int64, __int64))MmVerifyCallbackFunctionCheckFlags)(
                        NotifyRoutine,
                        LdrDataTableEntryFlags) )
    return STATUS_ACCESS_DENIED;                       
  CallbackObject = ExAllocateCallBack(NotifyRoutine, v2);
  if ( !CallbackObject )
    return STATUS_INSUFFICIENT_RESOURCES;                       
  index = 0i64;
  while ( !ExCompareExchangeCallBack(&PspCreateProcessNotifyRoutine[index], CallbackObject, 0i64) )
  {
    index = (unsigned int)(index + 1);
    if ( (unsigned int)index >= 0x40 )
    {
      ExFreePoolWithTag(CallbackObject, 0i64);
      return STATUS_INVALID_PARAMETER;
    }
  }
  if ( *(_DWORD *)&bIsExRoutine )
  {
    _InterlockedIncrement(&dword_1409AE938);
    if ( (dword_1409AE5F0 & 4) == 0 )
      _interlockedbittestandset(&dword_1409AE5F0, 2u);
  }
  else
  {
    _InterlockedIncrement(&dword_1409AE93C);
    if ( (dword_1409AE5F0 & 2) == 0 )
      _interlockedbittestandset(&dword_1409AE5F0, 1u);
  }
  return STATUS_SUCCESS;
}