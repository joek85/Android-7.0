/*
 * Copyright (c) 2008-2015 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __HAL_BUFFER_LIST_H__
#define __HAL_BUFFER_LIST_H__

#include <fcntl.h>
#include <cutils/list.h>
#include <utils/Mutex.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace android {

typedef struct BUFFER_NODE_t
{
    struct listnode    i_list;
    int                id;
    void *            data;
    int                size;
    char            priv[128];
    int             fd;
}buffer_node;

class BufferListManager {
public:
    BufferListManager();
    ~BufferListManager();

    // return buffer start address
    buffer_node * allocBuffer(uint32_t id, uint32_t min_size);
    void releaseBuffer(buffer_node * node);

    bool isListEmpty();

    buffer_node * pop();
    void push(buffer_node * node);

    int getItemCnt();

private:
    Mutex                mLock;

    struct listnode        mList;

    int                    mItemCnt;
};

}; /* namespace android */

#endif  /* __HAL_BUFFER_LIST_H__ */

