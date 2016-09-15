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

        public Heart()
        {
            LibHeartBeat.log_switch(true);
            handle = LibHeartBeat.i_come();
        }
        
        public bool TakeImage(BitmapSource bitmap)
        {
            inner_image = new IntermediateImage(bitmap);
            var bgra32 = inner_image.ImagePixels;
            var wid = (uint)inner_image.Info.Width;
            var hgt = (uint)inner_image.Info.Height;

            return LibHeartBeat.i_see(handle, bgra32, wid, hgt) != 0;
        }

        public enum Action
        {
            Idel,
            Jump,
            Down
        }

        public Action Choice()
        {
            switch(LibHeartBeat.i_decide(handle)) {
            case 0: return Action.Idel;
            case 1: return Action.Jump;
            case 2: return Action.Down;
            default: return Action.Idel;
            }
        }

        public bool IsReady()
        {
            return LibHeartBeat.am_i_ready(handle) != 0;
        }

        public bool IsGameOver()
        {
            return LibHeartBeat.am_i_dead(handle) != 0;
        }

        public void Start()
        {
            LibHeartBeat.i_start(handle);
        }

        public void Pause()
        {
            LibHeartBeat.i_rest(handle);
        }

        public void Resume()
        {
            LibHeartBeat.i_continue(handle);
        }

        public void Stop()
        {
            LibHeartBeat.i_stop(handle);
        }

        public BitmapSource GetDebugImage()
        {
            if(inner_image == null)
                return null;

            var bgra32 = inner_image.ImagePixels;
            var wid = (uint)inner_image.Info.Width;
            var hgt = (uint)inner_image.Info.Height;
            LibHeartBeat.i_know(handle, bgra32, wid, hgt);

            return inner_image.ToBitmapSource();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
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
