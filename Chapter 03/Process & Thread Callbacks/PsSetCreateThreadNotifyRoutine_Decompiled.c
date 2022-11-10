__int64 __fastcall PsSetCreateThreadNotifyRoutine(__int64 NotifyRoutine)
{
  return sub_1407459C0(NotifyRoutine, 0i64);
}

__int64 __fastcall sub_1407459C0(__int64 NotifyRoutine, unsigned int a2)
{
  char v2; // si
  __int64 CallbackObject; // rdi
  __int64 v4; // rbx

  v2 = a2;

  /*
  **
  ** ExAllocate Callback allocates buffer of size 0x18 in POOL_NX_ALLOCATION pool with tag 'brbC'
  ** returns pointer to struct _EX_CALLBACK_ROUTINE_BLOCK {
  **    // 0x00 EX_RUNDOWN_REF 	RundownProtect
  **    // 0x08 PEX_CALLBACK_FUNCTION 	Function
  **    // 0x10 PVOID 	Context
  ** }
  ** It sets CallbackBlock->Function to the param NotifyRoutine 
  ** Sets CallbackBlock->Context to a2
  */
  CallbackObject = ExAllocateCallBack(NotifyRoutine, a2); 
  if ( !CallbackObject )
    return STATUS_INSUFFICIENT_RESOURCES;
  v4 = 0i64;
  while ( !ExCompareExchangeCallBack(&qword_1405050E0[v4], CallbackObject, 0i64) )
  {
    v4++;
    if ( (unsigned int)v4 >= 0x40 )
    {
      ExFreePoolWithTag(CallbackObject, 0i64);
      return STATUS_INSUFFICIENT_RESOURCES;
    }
  }

  // This block isn't called since v2 is 0. Param passed from previous function
  if ( (v2 & 1) != 0 )
  {
    _InterlockedIncrement(&dword_1409AE944);
    if ( (dword_1409AE5F0 & 0x10) == 0 )
      _interlockedbittestandset(&dword_1409AE5F0, 4u);
  }
  else
  {
    _InterlockedIncrement(&dword_1409AE934);
    if ( (dword_1409AE5F0 & 8) == 0 )
      _interlockedbittestandset(&dword_1409AE5F0, 3u);
  }
  return STATUS_SUCCESS;
}