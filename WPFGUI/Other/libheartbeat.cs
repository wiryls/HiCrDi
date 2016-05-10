using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WPFGUI.Other
{
    static class libheartbeat
    {

        [DllImport("libheartbeat.dll", EntryPoint = "version", CallingConvention = CallingConvention.Cdecl)]
        public static extern uint version();

        [DllImport("libheartbeat.dll", EntryPoint = "i_come", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr i_come();

        [DllImport("libheartbeat.dll", EntryPoint = "i_see", CallingConvention = CallingConvention.Cdecl)]
        public static extern byte i_see(IntPtr i, byte[] bgra32, uint wid, uint hgt);

        [DllImport("libheartbeat.dll", EntryPoint = "i_know", CallingConvention = CallingConvention.Cdecl)]
        public static extern void i_know(IntPtr i, byte[] bgra32, uint wid, uint hgt);

        [DllImport("libheartbeat.dll", EntryPoint = "i_decide", CallingConvention = CallingConvention.Cdecl)]
        public static extern int i_decide(IntPtr i);

        [DllImport("libheartbeat.dll", EntryPoint = "i_sleep", CallingConvention = CallingConvention.Cdecl)]
        public static extern void i_sleep(IntPtr i);

        [DllImport("libheartbeat.dll", EntryPoint = "i_start", CallingConvention = CallingConvention.Cdecl)]
        public static extern void i_start(IntPtr i);

        [DllImport("libheartbeat.dll", EntryPoint = "i_rest", CallingConvention = CallingConvention.Cdecl)]
        public static extern void i_rest(IntPtr i);

        [DllImport("libheartbeat.dll", EntryPoint = "i_continue", CallingConvention = CallingConvention.Cdecl)]
        public static extern void i_continue(IntPtr i);

        [DllImport("libheartbeat.dll", EntryPoint = "i_stop", CallingConvention = CallingConvention.Cdecl)]
        public static extern void i_stop(IntPtr i);

        [DllImport("libheartbeat.dll", EntryPoint = "am_i_dead", CallingConvention = CallingConvention.Cdecl)]
        public static extern byte am_i_dead(IntPtr i);



        [DllImport("libheartbeat.dll", EntryPoint = "test", CallingConvention = CallingConvention.Cdecl)]
        public static extern void test(byte[] bgra32, byte[] dst, uint wid, uint hgt);
    }
}
