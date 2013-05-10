#ifndef __REFCOUNTED_H__
#define __REFCOUNTED_H__

#define REFCOUNT_UP(ptr) (ptr->Count())
#define REFCOUNT_DOWN(ptr) do if (!ptr->UnCount()) delete ptr; while(0)

class RefCounted
{
    protected:
        int refCount;

    public:
        RefCounted()
            : refCount(0)
        {
        }

        void Count(void)
        {
            ++refCount;
        }

        bool UnCount(void)
        {
            return --refCount > 0;
        }
};

#endif
