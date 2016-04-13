/*******************************************************************
** Copyright (C) 2015-2016 {Joey de Vries} {joey.d.vries@gmail.com}
**
** This code is part of Voidt.
** https://github.com/JoeyDeVries/Voidt
**
** Voidt is free software: you can redistribute it and/or modify it
** under the terms of the CC BY-NC 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
*******************************************************************/
#ifndef PLATFORM_WORK_QUEUE_H
#define PLATFORM_WORK_QUEUE_H

struct platform_work_queue;
typedef void platform_work_queue_func(platform_work_queue *queue, void *data);
typedef void platform_add_work_entry(platform_work_queue *queue, platform_work_queue_func *callback, void *data);
typedef void platform_complete_all_work(platform_work_queue *queue);

struct platform_work_queue_entry
{
    platform_work_queue_func *Func;
    void *Data;
};

struct platform_work_queue
{
    uint32 volatile CompletionGoal;
    uint32 volatile CompletionCount;
    
    uint32 volatile NextEntryToWrite; // circular work queue
    uint32 volatile NextEntryToRead;  // circular work queue
    // NOTE(Joey): HANDLE is defined in windows as PVOID; can be used as void*, but be
    // aware of function interdepdendancy and/or API changes when using this handle.
    void *SemaphoreHandle;
    
    platform_work_queue_entry Entries[256];     
};



#endif