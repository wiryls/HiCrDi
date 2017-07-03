using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Security.Permissions;
using System.Windows.Media.Imaging;

namespace GUI.Utility
{
    class Heart : IDisposable
    {
        private IntPtr handle;
        private IntermediateImage inner_image;
        private System.Threading.Mutex mutex = new System.Threading.Mutex();

        public Heart()
        {
            LibHeartBeat.log_switch(true);
            handle = LibHeartBeat.i_come();
        }

        public bool TakeImage(BitmapSource bitmap)
        {
            mutex.WaitOne();

            inner_image = new IntermediateImage(bitmap);
            var bgra32 = inner_image.ImagePixels;
            var wid = (uint)inner_image.Info.Width;
            var hgt = (uint)inner_image.Info.Height;
            var rv = LibHeartBeat.i_see(handle, bgra32, wid, hgt) != 0;

            mutex.ReleaseMutex();
            return rv;
        }

        public enum Action
        {
            Idle,
            Jump,
            Down
        }

        public Action Choice()
        {
            mutex.WaitOne();

            Action rv;
            switch(LibHeartBeat.i_decide(handle)) {
            case 0:  rv = Action.Idle; break;
            case 1:  rv = Action.Jump; break;
            case 2:  rv = Action.Down; break;
            default: rv = Action.Idle; break;
            }

            mutex.ReleaseMutex();
            return rv;
        }

        public bool IsReady()
        {
            mutex.WaitOne();
            var rv = LibHeartBeat.am_i_ready(handle) != 0;
            mutex.ReleaseMutex();
            return rv;
        }

        public bool IsGameOver()
        {
            mutex.WaitOne();
            var rv = LibHeartBeat.am_i_dead(handle) != 0;
            mutex.ReleaseMutex();
            return rv;
        }

        public void Start()
        {
            mutex.WaitOne();
            LibHeartBeat.i_start(handle);
            mutex.ReleaseMutex();
        }

        public void Pause()
        {
            mutex.WaitOne();
            LibHeartBeat.i_rest(handle);
            mutex.ReleaseMutex();
        }

        public void Resume()
        {
            mutex.WaitOne();
            LibHeartBeat.i_continue(handle);
            mutex.ReleaseMutex();
        }

        public void Stop()
        {
            mutex.WaitOne();
            LibHeartBeat.i_stop(handle);
            mutex.ReleaseMutex();
        }

        public BitmapSource GetDebugImage()
        {
            if(inner_image == null)
                return null;

            mutex.WaitOne();

            var bgra32 = inner_image.ImagePixels;
            var wid = (uint)inner_image.Info.Width;
            var hgt = (uint)inner_image.Info.Height;
            LibHeartBeat.i_know(handle, bgra32, wid, hgt);

            var rv = inner_image.ToBitmapSource();
            mutex.ReleaseMutex();
            return rv;
        }

        public void Dispose()
        {
            mutex.WaitOne();

            Dispose(true);
            GC.SuppressFinalize(this);

            mutex.ReleaseMutex();
        }

        [SecurityPermission(SecurityAction.Demand, UnmanagedCode = true)]
        protected virtual void Dispose(bool disposing)
        {
            if(handle != null)
                LibHeartBeat.i_sleep(handle);
        }
    }


    [SuppressUnmanagedCodeSecurity()]
    internal static class LibHeartBeat
    {
        private const string dll = "libheartbeat.dll";

        [DllImport(dll, EntryPoint = "version", CallingConvention = CallingConvention.Cdecl)]
        internal static extern uint version();

        [DllImport(dll, EntryPoint = "log_switch", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void log_switch(bool enable);


        [DllImport(dll, EntryPoint = "i_come", CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr i_come();

        [DllImport(dll, EntryPoint = "i_see", CallingConvention = CallingConvention.Cdecl)]
        internal static extern byte i_see(IntPtr i, byte[] bgra32, uint wid, uint hgt);

        [DllImport(dll, EntryPoint = "i_know", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void i_know(IntPtr i, byte[] bgra32, uint wid, uint hgt);

        [DllImport(dll, EntryPoint = "i_decide", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int i_decide(IntPtr i);

        [DllImport(dll, EntryPoint = "i_sleep", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void i_sleep(IntPtr i);

        [DllImport(dll, EntryPoint = "i_start", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void i_start(IntPtr i);

        [DllImport(dll, EntryPoint = "i_rest", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void i_rest(IntPtr i);

        [DllImport(dll, EntryPoint = "i_continue", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void i_continue(IntPtr i);

        [DllImport(dll, EntryPoint = "i_stop", CallingConvention = CallingConvention.Cdecl)]
        internal static extern void i_stop(IntPtr i);

        [DllImport(dll, EntryPoint = "am_i_ready", CallingConvention = CallingConvention.Cdecl)]
        internal static extern byte am_i_ready(IntPtr i);

        [DllImport(dll, EntryPoint = "am_i_dead", CallingConvention = CallingConvention.Cdecl)]
        internal static extern byte am_i_dead(IntPtr i);
    }
}
