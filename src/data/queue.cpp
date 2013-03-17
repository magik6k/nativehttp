/*
Copyright (c) 2013 Lukasz Magiera

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/
#include "queue.h"

namespace data
{
    template<class T>queue<T>::queue()
    {
        newest=NULL;
        oldest=NULL;
        elems=0;
    }

    template<class T>queue<T>::~queue()
    {
        while(newest)this->pop();
    }

    template<class T>int queue<T>::push(T tp)
    {
        qelm<T>* nqe=new qelm<T>;
        if(!nqe)return 1;
        nqe->d=tp;
        nqe->newer=NULL;
        nqe->older=NULL;

        if(newest)
        {
            newest->newer=nqe;
            nqe.older=newest;
        }
        newest=nqe;
        if(!oldest)oldest=nqe;

        elems++;
        return 0;
    }

    template<class T>int queue<T>::pop()
    {
        if(oldest)
        {
            qelm<T>* tf=oldest;
            oldest=oldest->newer;
            oldest.older=NULL;
            if(!oldest)newest=NULL;
            delete tf;
            elems--;
            return 0;
        }
        return 1;
    }

    template<class T>T queue<T>::front()
    {
        return oldest->d;
    }

}
