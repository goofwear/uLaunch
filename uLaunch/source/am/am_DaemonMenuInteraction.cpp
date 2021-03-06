#include <am/am_DaemonMenuInteraction.hpp>
#include <fs/fs_Stdio.hpp>
#include <am/am_LibraryApplet.hpp>

namespace am
{
    #define UL_AM_LOOP_TRY_WITH(...) { \
        Result rc = 0; \
        do \
        { \
            __VA_ARGS__ \
            svcSleepThread(10'000'000); \
        } while(R_FAILED(rc)); \
        return rc; \
    }

    #define UL_AM_WAIT_WITH(...) { \
        if(wait) { UL_AM_LOOP_TRY_WITH(__VA_ARGS__) } \
        else \
        { \
            Result rc = 0; \
            __VA_ARGS__ \
            return rc; \
        } \
    }
    
    #define UL_AM_WAIT(expr) UL_AM_WAIT_WITH(rc = (expr);)

    ResultWith<MenuStartMode> Menu_ProcessInput()
    {
        LibAppletArgs in_args;
        auto rc = Menu_DaemonReadImpl(&in_args, sizeof(LibAppletArgs), false);
        return MakeResultWith(rc, (MenuStartMode)in_args.LaVersion);
    }

    Result Daemon_MenuWriteImpl(void *data, size_t size, bool wait)
    UL_AM_WAIT(LibraryAppletSend(data, size))
    
    Result Daemon_MenuReadImpl(void *data, size_t size, bool wait)
    UL_AM_WAIT(LibraryAppletRead(data, size))

    Result Menu_DaemonWriteImpl(void *data, size_t size, bool wait)
    UL_AM_WAIT_WITH(
        AppletStorage st;
        rc = appletCreateStorage(&st, size);
        if(R_SUCCEEDED(rc))
        {
            rc = appletStorageWrite(&st, 0, data, size);
            if(R_SUCCEEDED(rc)) rc = appletPushOutData(&st);
            appletStorageClose(&st);
        }
    )

    Result Menu_DaemonReadImpl(void *data, size_t size, bool wait)
    UL_AM_WAIT_WITH(
        AppletStorage st;
        rc = appletPopInData(&st);
        if(R_SUCCEEDED(rc))
        {
            rc = appletStorageRead(&st, 0, data, size);
            appletStorageClose(&st);
        }
    )
}