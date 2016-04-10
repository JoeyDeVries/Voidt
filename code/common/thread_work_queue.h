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
#ifndef THREAD_WORK_QUEUE_H
#define THREAD_WORK_QUEUE_H


typedef WorkQueueCallback(platform_work_queue *queue, void *data);

struct platform_work_entry 
{
    WorkQueueCallback *Callback;
    void *Data;
};

// circular threaded work queue buffer
struct platform_work_queue 
{
    uint32 volatile Completed;
    uint32 volatile NextEntryToRead;
    uint32 volatile NextEntryToWrite;
    
    platform_work_entry WorkEntries[256]; 
};

#endif