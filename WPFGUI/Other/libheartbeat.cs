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

        [DllImport("libheartbeat.dll", EntryPoint = "test", CallingConvention = CallingConvention.Cdecl)]
        public static extern void test(byte[] bgra32, byte[] dst, uint wid, uint hgt);
    }
}
