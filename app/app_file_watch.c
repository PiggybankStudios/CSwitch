/*
File:   app_file_watch.c
Author: Taylor Robbins
Date:   03\02\2025
Description: 
	** Holds functions that help us manage a list of "File Watches"
	** which basically are just a recording of the last file write time
	** along with some info to help us decide how often to check the write
	** time and to handle the file existing or not
*/

void FreeFileWatch(Arena* arena, FileWatch* watch)
{
	NotNull(arena);
	NotNull(watch);
	FreeStr8(arena, &watch->path);
	FreeStr8(arena, &watch->fullPath);
	ClearPointer(watch);
}

void InitFileWatches(VarArray* watches)
{
	NotNull(watches);
	InitVarArray(FileWatch, watches, stdHeap);
}

void UpdateFileWatches(VarArray* watches)
{
	u64 programTime = (appIn != nullptr) ? appIn->programTime : 0;
	VarArrayLoop(watches, wIndex)
	{
		VarArrayLoopGet(FileWatch, watch, watches, wIndex);
		if (watch->id != 0)
		{
			if (watch->checkPeriod == 0 || TimeSinceBy(programTime, watch->lastCheck) >= watch->checkPeriod)
			{
				bool doesExist = OsDoesFileExist(watch->fullPath);
				if (doesExist != watch->fileExists)
				{
					watch->lastChangeTime = programTime;
					watch->changed = true;
					watch->fileExists = doesExist;
				}
				if (doesExist)
				{
					OsFileWriteTime newWriteTime = ZEROED;
					Result getWriteTimeResult = OsGetFileWriteTime(watch->fullPath, &newWriteTime);
					Assert(getWriteTimeResult == Result_Success);
					//TODO: We should make a proper OsWriteTimeEquals function!
					if (!watch->gotWriteTime || !MyMemEquals(&watch->writeTime, &newWriteTime, sizeof(OsFileWriteTime)))
					{
						watch->lastChangeTime = programTime;
						watch->changed = true;
					}
					watch->writeTime = newWriteTime;
					watch->gotWriteTime = true;
				}
				else { watch->gotWriteTime = false; }
			}
		}
	}
}

uxx AddFileWatch(VarArray* watches, FilePath path, uxx checkPeriod)
{
	NotNull(watches);
	NotNullStr(path);
	ScratchBegin1(scratch, watches->arena);
	FilePath fullPath = OsGetFullPath(scratch, path);
	VarArrayLoop(watches, wIndex)
	{
		VarArrayLoopGet(FileWatch, watch, watches, wIndex);
		if (watch->id != 0 && StrAnyCaseEquals(watch->fullPath, fullPath))
		{
			DebugAssert(watch->id == wIndex+1);
			ScratchEnd(scratch);
			if (checkPeriod < watch->checkPeriod) { watch->checkPeriod = checkPeriod; }
			watch->numReferences++;
			return watch->id;
		}
	}
	
	FileWatch* emptySlot = nullptr;
	VarArrayLoop(watches, wIndex)
	{
		VarArrayLoopGet(FileWatch, watch, watches, wIndex);
		if (watch->id == 0)
		{
			emptySlot = watch;
			break;
		}
	}
	
	FileWatch* newWatch = nullptr;
	if (emptySlot != nullptr)
	{
		uxx watchIndex = 0;
		bool foundIndex = VarArrayGetIndexOf(FileWatch, watches, emptySlot, &watchIndex);
		Assert(foundIndex);
		emptySlot->id = watchIndex+1;
		newWatch = emptySlot;
	}
	else
	{
		newWatch = VarArrayAdd(FileWatch, watches);
		NotNull(newWatch);
		ClearPointer(newWatch);
		newWatch->id = watches->length;
	}
	
	newWatch->checkPeriod = checkPeriod;
	newWatch->numReferences = 1;
	newWatch->path = AllocStr8(stdHeap, path);
	newWatch->fullPath = AllocStr8(stdHeap, fullPath);
	newWatch->changed = false;
	newWatch->lastCheck = (appIn != nullptr) ? appIn->programTime : 0;
	newWatch->fileExists = OsDoesFileExist(fullPath);
	if (newWatch->fileExists)
	{
		Result getWriteTimeResult = OsGetFileWriteTime(fullPath, &newWatch->writeTime);
		Assert(getWriteTimeResult == Result_Success);
		newWatch->gotWriteTime = true;
	}
	else { newWatch->gotWriteTime = false; }
	ScratchEnd(scratch);
	
	return newWatch->id;
}
void RemoveFileWatch(VarArray* watches, uxx watchId)
{
	NotNull(watches);
	Assert(watchId != 0);
	Assert(watchId <= watches->length);
	FileWatch* watch = VarArrayGetHard(FileWatch, watches, watchId-1);
	Assert(watch->id == watchId);
	if (watch->numReferences <= 1)
	{
		FreeFileWatch(stdHeap, watch); //this sets watch->id to 0
	}
	else
	{
		watch->numReferences--;
	}
}

bool HasFileWatchChangedWithDelay(const VarArray* watches, uxx watchId, u64 delayMs)
{
	NotNull(watches);
	Assert(watchId != 0);
	Assert(watchId <= watches->length);
	const FileWatch* watch = VarArrayGetHard(FileWatch, watches, watchId-1);
	Assert(watch->id == watchId);
	if (watch->changed)
	{
		if (delayMs > 0)
		{
			u64 programTime = (appIn != nullptr) ? appIn->programTime : 0;
			return (TimeSinceBy(programTime, watch->lastChangeTime) >= delayMs);
		}
		return true;
	}
	else { return false; }
}
bool HasFileWatchChanged(const VarArray* watches, uxx watchId)
{
	return HasFileWatchChangedWithDelay(watches, watchId, 0);
}

void ClearFileWatchChanged(VarArray* watches, uxx watchId)
{
	NotNull(watches);
	Assert(watchId != 0);
	Assert(watchId <= watches->length);
	FileWatch* watch = VarArrayGetHard(FileWatch, watches, watchId-1);
	Assert(watch->id == watchId);
	watch->fileExists = OsDoesFileExist(watch->fullPath);
	if (watch->fileExists)
	{
		Result getWriteTimeResult = OsGetFileWriteTime(watch->fullPath, &watch->writeTime);
		Assert(getWriteTimeResult == Result_Success);
		watch->gotWriteTime = true;
	}
	else { watch->gotWriteTime = false; }
	watch->changed = false;
	watch->lastCheck = (appIn != nullptr) ? appIn->programTime : 0;
}
